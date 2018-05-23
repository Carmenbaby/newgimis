/*===============================================================================
 处理有关XTRA
=============================================================================== */
#include "AEEFile.h"
#include "AEEStdLib.h"
#include "AEEXtra.bid"

#include "newgimis.h"
#include "gpsOneXTRA.h"

#include "gmTools.h"

#define XTRA_EVT_USER_INIT_DWNLD_EFS EVT_USER + 1022
#define XTRA_EVT_USER_SET_PARAM      EVT_USER + 1024
#define XTRA_EVT_USER_SET_TIME       EVT_USER + 1025
#define XTRA_EVT_USER_TBD            EVT_USER + 1026
#define XTRA_EVT_STOP			     EVT_USER + 1027


#define LIFETIMESOCKET 1				// Can be done just one time at the start of application

#define SECSINSEVENTYYEARS 2208988800	// 后面2208988800ll  在ARM下编译是正确的，在win下应写为2208988800，		// from 1 Jan 1900 to 1970(start of Unix time)
#define SECSINTENYEARS     315964800				// from 1970 to 1980(Star of GPS time)
#define SECSINAWEEK 604800
#define TENMILLION 10000000

/*-------------------------------------------------------------------
Static function prototypes
-------------------------------------------------------------------*/
static void SetDefaultSntpParam(XtraDcApp* me);
static void ProcessConfig(XtraDcApp* me);
static void GetConfig(XtraDcApp* me, char* data, char* str, boolean bNonAscii);

static void XtraDc_Init(XtraDcApp* me);
static int OneTimeSocketInit(XtraDcApp* me);
static void XtraDc_TimerEvtCbk(void* po);
static void XTRA_CleanState(XtraDcApp* me);
static void XtraDc_TimerEvtCbk(void* po);
static void ReadBody(void* po);
static void WebCbk(void* po);
static int web20(XtraDcApp* pMe);
static void XTRA_DwnldEfs(XtraDcApp* me, char* pFile);
static void XTRA_DwnldNet(XtraDcApp* me);

void GetHostByNameCbk(void* po);
static void CleanSntp(XtraDcApp* me);
static int writeSock(XtraDcApp* me);
static int readSock(XtraDcApp* me);
static int initSntp(XtraDcApp* me);
static int getSntpTime(XtraDcApp* me);
static void PopulateSntpUrls(XtraDcApp* me);
static void GetHostByName(XtraDcApp* me);			// ---- 

static boolean XTRA_ReqSetTime(XtraDcApp* me);
static boolean XTRA_ReqDataValidity(XtraDcApp* me);
static boolean XTRA_ReqDwnld(XtraDcApp* me);
static boolean XTRA_Dwnld(XtraDcApp* me);

static boolean StateCheck(XtraDcApp* me)
{
	if (me->nCmdReq != 0)
	{
		return FALSE;
	}
	return TRUE;
}

static void XTRA_StartTimer(XtraDcApp* me)
{
	uf_RecLog(me->m_pParent, "IN XTRA_StartTimer");

	me->cTimeOut = 0;

	CALLBACK_Cancel(&me->m_evtTimer);
	CALLBACK_Init(&me->m_evtTimer,  XtraDc_TimerEvtCbk,  (void*)me);
	ISHELL_SetTimerEx(me->m_pParent->a.m_pIShell, XTRA_TIMEOUT_VALUE_MSEC, &me->m_evtTimer);
}

static void XtraDc_TimerEvtCbk(void* po)
{
	XtraDcApp* me = (XtraDcApp*) po;
	newgimis *pMe = me->m_pParent;
	
	uf_RecLog(pMe,  "XtraDc_TimerEvtCbk,Timeout Callback Request error ");
	me->cTimeOut = 1;		// 2013-07-31
	
//	SendResponseToUI(po, XTRA_USER_TIMEOUT);
	XTRA_CleanState(me);
}

/*-------------------------------------------------------------------
function prototypes
-------------------------------------------------------------------*/
void XTRA_CmdCbk(void* pCtx, AEEXTRACmdType cmdId, int32 nErr);
void XTRA_EvtCbk(void* pCtx, AEEXTRAEventType evtId);

void SockPortWriteCbk(void* po)
{
	XtraDcApp* me = (XtraDcApp*) po;
	uf_RecLog(me->m_pParent,"Retrying... writeSock()");
	writeSock(me);
}

void SockPortReadCbk(void* po)
{
	XtraDcApp* me = (XtraDcApp*) po;
	//SPRINTF(log,"Retrying... readsock()");
	//XtraDc_LogMsg(pMe,log); 
	uf_RecLog(me->m_pParent,"Retrying... readsock()");
	readSock(me);
}

//===============================================================================
boolean XtraDc_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
  int nErr = SUCCESS;
  XtraDcApp* me;		// = (XtraDcApp*) pMe;
//  char sLogBuf[256];

  if (pMe->pXtrxDcApp != NULL)
	  me = (XtraDcApp *)pMe->pXtrxDcApp;

  switch (eCode)
  {
    case EVT_APP_START:
		if (pMe->pXtrxDcApp	== NULL) {
			me = (XtraDcApp *)MALLOC(sizeof(XtraDcApp));
			MEMSET(me,0,sizeof(XtraDcApp));
			me->m_pParent = pMe;
			me->a = pMe->a;
			pMe->pXtrxDcApp = (void *)me;
		}

      nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&me->pFileMgr);
      if(SUCCESS != nErr) 
      {
        uf_RecLog(pMe,"XTRA START AEECLSID_FILEMGR error");
        return FALSE;
      }

      me->iLifeOfSocket=LIFETIMESOCKET;			// ---- 功能不详

	  XtraDc_Init(me);			// ---- 初始化，转到后台运行
      return(me->bState);		//TRUE);
       
    case XTRA_EVT_STOP:							// EVT_APP_STOP:
      
		uf_RecLog(pMe,"in XTRA_EVT_STOP");

        if (me->pFileMgr)
        {
          IFILEMGR_Release(me->pFileMgr);
        }
		    
        if(me->pWeb)
	    {
			int i,n;		// 2.6.4D2
			n = 0;
			i=IWEB_Release(me->pWeb);
			while (i>0) {
				n++;
				i=IWEB_Release(me->pWeb);
			}
			
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"XTRA_CleanState, IWEB_Release=%d,n=%d",i,n);
			me->pWeb = 0;
	    }
      
// 改到退出时		FREEIF(pMe->pXtrxDcApp);
		uf_RecLog(pMe,"out XTRA_EVT_STOP");

      return(TRUE);
        
    case XTRA_EVT_USER_INIT_DWNLD:				// ---- 下载星历
	    return XTRA_ReqDwnld(me);

	case XTRA_EVT_USER_DATA_VALIDITY:			// ---- 验证星历数据有效性
		return XTRA_ReqDataValidity(me);
    default:

		break;
    }

    return(FALSE);
}

// =======================================================
static void XtraDc_Init(XtraDcApp* me)
{
//  char buf[128];   
  newgimis *pMe = me->m_pParent;
  char sLogBuf[256];
  int nErr = SUCCESS;
  me->pIXtra = 0;

  me->cDwnState	= 0;			// 缺省是不成功  2013-04-12

  nErr = ISHELL_CreateInstance(me->a.m_pIShell, AEECLSID_WEB, (void**)&me->pWeb);			// ---- Web接口

  if (SUCCESS != nErr) 
  {
    SPRINTF(sLogBuf, "IWeb CI err: %d", nErr);
    uf_RecLog(pMe, sLogBuf);
    goto bail;
  }

  if (SUCCESS != IFILEMGR_Test(me->pFileMgr, XTRA_TMP_FILE))
  {
	  me->pTmpFile = IFILEMGR_OpenFile(me->pFileMgr, XTRA_TMP_FILE, _OFM_CREATE);
	  if (0 == me->pTmpFile)
	  {
	    uf_RecLog(pMe, "Fatal Error in creating tmp file");
		goto bail;
	  }

	  IFILE_Release(me->pTmpFile);
	  me->pTmpFile = 0;
  }

  me->pTmpFile = IFILEMGR_OpenFile(me->pFileMgr, XTRA_TMP_FILE, _OFM_READWRITE);
  if (0 == me->pTmpFile)
  {
	DBGPRINTF("-----Err create tmp file");
	uf_RecLog(pMe,  "Fatal Error in creating tmp file");
	goto bail;
  }  
   
  if (0 != me->pTmpFile) DBGPRINTF("-----Created tmp file");

  nErr = ISHELL_CreateInstance(me->a.m_pIShell, AEECLSID_XTRA, (void**)&me->pIXtra);
  if(SUCCESS != nErr) 
  {
    SPRINTF(sLogBuf, "IXTRA interface CI err: %d", nErr);
    uf_RecLog(pMe, sLogBuf);
    goto bail;     
  }

  SPRINTF(sLogBuf, "Xtra CI success: %x", me->pIXtra);
  uf_RecLog(pMe, sLogBuf);

  nErr = IXTRA_RegisterCallback(me->pIXtra, XTRA_EvtCbk, 0x7FFFFFFF, (void*)me);			// ---- 注册事件回调函数 
  if(SUCCESS != nErr) 
  {
    SPRINTF(sLogBuf, "Xtra Register Cbk err: %d", nErr);
	uf_RecLog(pMe, sLogBuf);
    goto bail;     
  }

  SPRINTF(sLogBuf, "Register Evt Callback success: %d", nErr);
  uf_RecLog(pMe, sLogBuf);

  nErr = IXTRA_RegisterCmdCallback(me->pIXtra, XTRA_CmdCbk, 0x7FFFFFFF, (void*)me);			// ---- 注册命令的回调函数
  if(SUCCESS != nErr) 
  {
    SPRINTF(sLogBuf, "Xtra Register Cmd Cbk err: %d", nErr);
	uf_RecLog(pMe, sLogBuf);
    goto bail;     
  }

  SPRINTF(sLogBuf, "Register Cmd Callback success: %d", nErr);
  uf_RecLog(pMe, sLogBuf);


  me->iSocketCount=0;
  SetDefaultSntpParam(me);

  ProcessConfig(me);

  SPRINTF(sLogBuf, "in XtraDc_Init,me->iLifeOfSocket=%d",me->iLifeOfSocket); 
  uf_RecLog(me->m_pParent,sLogBuf);

	  if(me->iLifeOfSocket==LIFETIMESOCKET)
	  {
		if(OneTimeSocketInit(me)!=SUCCESS)									// ---- 似乎是打开socket
		{
		   SPRINTF(sLogBuf, "OneTimeSocketInit() in  XtraDc_Init() failed:");
			uf_RecLog(pMe, sLogBuf);			// XtraDc_LogMsg(me, buf);
		}
	  }
bail:
  me->bState = TRUE;
  if (SUCCESS != nErr)
  {
	  me->bState = FALSE;
  }
  return;
}

// =======================================================
static int OneTimeSocketInit(XtraDcApp* me)
{  
	char log[128];
	int nErr = SUCCESS;
	
	uf_RecLog(me->m_pParent,"in OneTimeSocketInit");	

	me->sockAddr.inet.port  = HTONS(NTP_SERV_PORT);
	me->sockAddr.wFamily    = AEE_AF_INET;
	nErr = ISHELL_CreateInstance(me->a.m_pIShell, AEECLSID_SOCKPORT,(void**)&me->pSockPort);
	
	if(SUCCESS != nErr) 
	{
		//XtraDc_LogMsg(pMe,"In OneTimeSocketInit() while ISHELL_CreateInstance()");
		uf_RecLog(me->m_pParent,"In OneTimeSocketInit() while ISHELL_CreateInstance()");
		goto bail;
	}
	
	nErr = ISOCKPORT_OpenEx(me->pSockPort, AEE_AF_INET, AEE_SOCKPORT_DGRAM, 0);
	
	if(SUCCESS != nErr) 
	{
//		XtraDc_LogMsg(pMe,"In OneTimeSocketInit() while ISOCKPORT_OpenEx()");
		uf_RecLog(me->m_pParent,"In OneTimeSocketInit() while ISOCKPORT_OpenEx()");
		goto bail;
	}
	
	me->iSocketCount++;
	return SUCCESS;
	
bail:
	
	nErr = ISOCKPORT_GetLastError(me->pSockPort);
	SPRINTF(log, "In OneTimeSocketInit(), (error code: %d), unable to initialize socket", nErr);	   
	uf_RecLog(me->m_pParent,log);			//	XtraDc_LogMsg(pMe, log); 
	CleanSntp(me);
	XTRA_CleanState(me);
	return EFAILED;
}


// =======================================================
static boolean XTRA_Dwnld(XtraDcApp* me)
{
	int nErr;
	char buf[128];
	
	nErr = IXTRA_TriggerDownload(me->pIXtra);
	if(SUCCESS != nErr) 
	{
		SPRINTF(buf, "Trigger Download err: %d", nErr);
		uf_RecLog(me->m_pParent, buf);
		return FALSE;     
	}
	
//	if(me->iDebugLogLevel>=LEVELTWO)	{
		SPRINTF(buf, "Trigger Download success: %d", nErr);
		uf_RecLog(me->m_pParent, buf);
		return TRUE;
//	}

}
// =======================================================
// 星历下载
static boolean XTRA_ReqDwnld(XtraDcApp* me)
{
//	if(me->iDebugLogLevel>=LEVELTWO)	{ 
	uf_RecLog(me->m_pParent, "IN XTRA_ReqDwnld request downLoad data");
	//}
	if (!StateCheck(me) || !me->bState)
	{
//		XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_INIT_DWNLD);
		uf_RecLog(me->m_pParent, "ReqDwnld error");
		return FALSE;
	}
	
	me->dwDwnldMode = XTRA_DWNLD_MODE_NET;						// ---- 网站下载
	me->nCmdReq  = AEE_XTRA_CMD_TRIGGERDOWNLOAD;
	return XTRA_Dwnld(me);
}


// =======================================================
// 验证有效性  ----
static boolean XTRA_ReqDataValidity(XtraDcApp* me)			
{
	if (!StateCheck(me) || !me->bState)
	{
		uf_RecLog(me->m_pParent, "ReqDataValidity");
		return FALSE;
	}

	me->nCmdReq = AEE_XTRA_CMD_DATA_VALIDITY;
	IXTRA_DataValidity(me->pIXtra);			// 验证gpsOne Subsystem数据的有效性
	return TRUE;

}
// =======================================================
// 注入时间  ----
static boolean XTRA_ReqSetTime(XtraDcApp* me)			
{
//	if(me->iDebugLogLevel>=LEVELTWO)	{ 
	uf_RecLog(me->m_pParent, "IN XTRA_ReqSetTime request set time");
//	}
	if (!StateCheck(me) || !me->bState)			// 检查是否有指令
	{
		uf_RecLog(me->m_pParent, "XTRA_ReqSetTime error");
		return FALSE;
	}	
	
	me->nCmdReq  = AEE_XTRA_CMD_TIME_INFO;
	XTRA_StartTimer(me);							// ---- 注册超时处理，15秒未返回就是超时
	PopulateSntpUrls(me);
	getSntpTime(me);

	return TRUE;
}

// =======================================================
static int initSntp(XtraDcApp* me)						 // ----
{
	int nErr=0;

	uf_RecLog(me->m_pParent,"in initSntp");

	if(me->iLifeOfSocket!=LIFETIMESOCKET)
	{
		uf_RecLog(me->m_pParent,"me->iLifeOfSocket!=LIFETIMESOCKET");
		me->sockAddr.inet.port    = HTONS(NTP_SERV_PORT);
		me->sockAddr.wFamily      = AEE_AF_INET;
	}
	
	me->nBytesR = 0;					// ？？
	me->nBytesW = 0;
	
	GetHostByName(me);
	return nErr;
}

// =======================================================
static void GetHostByName(XtraDcApp* me)			// ----
{   
	char log[128];
//	if(pMe->iDebugLogLevel>=LEVELTWO)	{
		SPRINTF(log,"GetHostByName, Requesting SNTP URL %s",me->pSntpUrl);
		uf_RecLog(me->m_pParent,log);	//		XtraDc_LogMsg(pMe,log);
//	}
	
	CALLBACK_Init(&me->m_cb,GetHostByNameCbk,me);
	ISHELL_CreateInstance(me->a.m_pIShell, AEECLSID_NET, (void**)(&me->m_pINetMgr));
	INETMGR_GetHostByName(me->m_pINetMgr,&me->m_dnsr, me->pSntpUrl,&me->m_cb);
}

// =======================================================
void GetHostByNameCbk(void* po)
{
	int nErr; 
	char log[128];
	XtraDcApp* me = (XtraDcApp*) po;
	nErr = me->m_dnsr.nResult;
	
	uf_RecLog(me->m_pParent,"in GetHostByNameCbk");

	if (nErr > AEEDNSMAXADDRS) 	{
		SPRINTF(log, "DNS Lookup Failed: Error %d", nErr);
		uf_RecLog(me->m_pParent,log);	//		XtraDc_LogMsg(pMe,log);
		me->bDnsErrorFlag=TRUE;
		goto bail;
	}
	
	me->sockAddr.inet.addr=(me->m_dnsr.addrs[0]);
	//Can be done just one time at the start of application		
	if(me->iLifeOfSocket!=LIFETIMESOCKET)
	{
		nErr = ISHELL_CreateInstance(me->a.m_pIShell, AEECLSID_SOCKPORT,(void**)&me->pSockPort);
		if(SUCCESS != nErr) 
		{
			// XtraDc_LogMsg(pMe,"In GetHostByNameCbk() while ISHELL_CreateInstance()");
			uf_RecLog(me->m_pParent,"In GetHostByNameCbk() while ISHELL_CreateInstance()");
			goto bail;
		}
		
		nErr = ISOCKPORT_OpenEx(me->pSockPort, AEE_AF_INET, AEE_SOCKPORT_DGRAM, 0);
		if(SUCCESS != nErr) 
		{
			uf_RecLog(me->m_pParent,"In GetHostByNameCbk() while ISOCKPORT_OpenEx()"); // XtraDc_LogMsg(pMe,"In GetHostByNameCbk() while ISOCKPORT_OpenEx()");
			goto bail;
		}
		
		me->iSocketCount++;
	}
	
	MEMSET(&me->ntpInfo, 0, sizeof(me->ntpInfo));
	me->ntpInfo.mMode = 27;										// ---- ????  何意
	
	// set orig timestamp as current time
	me->ntpInfo.mOrigTimestamp.dwInt = GETUPTIMEMS();
	writeSock(me);
	return;
	
bail:
	
	nErr = ISOCKPORT_GetLastError(me->pSockPort);
	SPRINTF(log, "In GetHostByNameCbk(), SNTP session failed (error code: %d), unable to set time", nErr);	   
	uf_RecLog(me->m_pParent,log); //XtraDc_LogMsg(pMe, log); 
	//XtraDc_SendResp(pMe, XTRA_USER_FAIL, XTRA_EVT_USER_DATA_VALIDITY);
	CleanSntp(me);					
	XTRA_CleanState(me);
	return;
}

// =======================================================
static void CleanSntp(XtraDcApp* me)
{
	if(me->iLifeOfSocket!=LIFETIMESOCKET)
	{
		char log[128];
		int iRefCount;
		if(me->pSockPort) 
		{
			if(SUCCESS!= ISOCKPORT_Close(me->pSockPort)) {
				SPRINTF(log,"In CleanSntp() Error while ISOCKPORT_Close()");
				uf_RecLog(me->m_pParent,log);		//	XtraDc_LogMsg(pMe,log);
			}
			
			iRefCount=ISOCKPORT_Release(me->pSockPort);
			if(me->bDnsErrorFlag==TRUE)
			{
				SPRINTF(log,"DNS Error: ISOCKPORT_Release() Ref Count: %d",iRefCount);
				uf_RecLog(me->m_pParent,log);		// XtraDc_LogMsg(pMe,log);
			}
			
			me->pSockPort = NULL;
			me->iSocketCount--;
		}
	}
}

// =======================================================
static int writeSock(XtraDcApp* me)
{
	int nErr = SUCCESS;
	char log[128];
	char ntpInfoBuf[48];
	
	uf_RecLog(me->m_pParent,"in writeSock");

	MEMCPY(ntpInfoBuf,&me->ntpInfo,sizeof(me->ntpInfo));
// 	SPRINTF(log,"ntpInfoBuf=%s",ntpInfoBuf);			= 1B 0D 0A
// 	uf_RecLog(me->m_pParent,log);
	
	// set orig timestamp as current time
	me->dwSntpRequestTimeMs = GETUPTIMEMS();
//	if(pMe->iDebugLogLevel>=LEVELTWO) 	{
		SPRINTF(log, "SNTP request sent at: %u ms", me->dwSntpRequestTimeMs);
		uf_RecLog(me->m_pParent,log);		//XtraDc_LogMsg(pMe, log); 
//	}
	
	nErr = ISOCKPORT_SendTo(me->pSockPort, (ntpInfoBuf + me->nBytesW), (sizeof(me->ntpInfo) - me->nBytesW), 0, &me->sockAddr);
	
//	if(pMe->iDebugLogLevel>=LEVELTWO)	{
		SPRINTF(log, "ISOCKPORT_SendTo Returned: %d", nErr); 
		uf_RecLog(me->m_pParent,log);		// XtraDc_LogMsg(pMe, log); 
//	}
	
	if (nErr == AEEPORT_WAIT) 
	{
		ISOCKPORT_WriteableEx(me->pSockPort, &me->m_writecb, SockPortWriteCbk, me);
		
//		if(pMe->iDebugLogLevel>=LEVELTWO)		{
		uf_RecLog(me->m_pParent,  "nErr == AEEPORT_WAIT");	//	XtraDc_LogMsg(pMe, "nErr == AEEPORT_WAIT"); 
//		}
		return nErr;
	}
	else if (0 > nErr)
	{
		goto bail;
	}
	
	me->nBytesW += nErr;
	
	if (me->nBytesW < sizeof(me->ntpInfo))
	{
//		if(pMe->iDebugLogLevel>=LEVELTWO) 		{
		uf_RecLog(me->m_pParent, "All bytes were not written.");	//	XtraDc_LogMsg(pMe, "All bytes were not written."); 
//		}
		
		ISOCKPORT_WriteableEx(me->pSockPort, &me->m_writecb, SockPortWriteCbk, me);
	}
	else 
	{
		me->nBytesR = 0;
		readSock(me);
	}
	
	return nErr;
	
bail:
	nErr = ISOCKPORT_GetLastError(me->pSockPort);
	SPRINTF(log, "In writesock(), SNTP session failed (error code: %d), unable to set time", nErr);	   
	uf_RecLog(me->m_pParent, log);  // XtraDc_LogMsg(pMe, log); 
	CleanSntp(me);
	XTRA_CleanState(me);
			//	XtraDc_SendResp(pMe, XTRA_USER_FAIL, XTRA_EVT_USER_SET_TIME);
	return nErr;
}

// =======================================================
static int readSock(XtraDcApp* me)
{
  NtpResp buf;   
  char log[128];
  int nSize;
  int nErr = 0;
  AEESockAddrStorage sa;
  uint32 dwLocalTime;
  uint32 dwIntegerSeconds, dwFractSeconds,dwSeconds,dwMilliSeconds, dwGPSWeek,dwGPSTow;
  uint32 dwRoundTripTimeMs = 0;
  AEEXTRATime xTime;
  MEMSET(&buf, 0, sizeof(buf));

  uf_RecLog(me->m_pParent, "in readSock");

  nSize = sizeof(AEESockAddrStorage);
  nErr = ISOCKPORT_RecvFrom(me->pSockPort, (char*)&buf, sizeof(buf), 0, &sa, &nSize);

//  if(pMe->iDebugLogLevel>=LEVELTWO)  {
    SPRINTF(log, "ISOCKPORT_RecvFrom Returned: %d", nErr); 
    uf_RecLog(me->m_pParent, log);		// XtraDc_LogMsg(pMe, log); 
//  } 

  if(AEEPORT_WAIT == nErr) 
  {
//    if(pMe->iDebugLogLevel>=LEVELTWO)    {
      uf_RecLog(me->m_pParent,"AEEPORT_WAIT == nErr");    // XtraDc_LogMsg(pMe, "AEEPORT_WAIT == nErr"); 
//    } 

    ISOCKPORT_ReadableEx(me->pSockPort, &me->m_readcb, SockPortReadCbk, me);
    return 0;
  }
  else if (0 > nErr)
  {
    goto bail; 
  }

  me->nBytesR = nErr;
  if (me->nBytesR < 48)
  {
    ISOCKPORT_ReadableEx(me->pSockPort, &me->m_readcb, SockPortReadCbk, me);
    return 0;
  }

  //getuptimems - msec since phone booted
  dwLocalTime = GETUPTIMEMS();

//  if(pMe->iDebugLogLevel>=LEVELTWO)  {
    SPRINTF(log, "Received SNTP response at: %u ms", dwLocalTime);
    uf_RecLog(me->m_pParent, log);		// XtraDc_LogMsg(pMe, log); 
//  }

  MEMSET(&xTime, 0, sizeof(xTime));
  dwIntegerSeconds = NTOHL(buf.nBasic.mTxTimestamp.dwInt);

//  if(pMe->iDebugLogLevel>=LEVELTWO)  {
    SPRINTF(log, "Integer Seconds from SNTP: %u", dwIntegerSeconds);
    uf_RecLog(me->m_pParent, log);		// XtraDc_LogMsg(pMe, log); 
//  }

  // following statements may be required if Uncertainty or time offset and round trip delay
  // is to be calculated
  // +  ((buf.nBasic.mRecvTimestamp-buf.nBasic.mOrigTimestamp)
  // +(buf.nBasic.mTxTimestamp-dwLocalTime))/2;
  dwRoundTripTimeMs = dwLocalTime - me->dwSntpRequestTimeMs;		// 接收时间-发送请求时间
//  if(pMe->iDebugLogLevel>=LEVELTWO)  {
    SPRINTF(log, "RTT for SNTP request: %u ms, curr = %u ms, orig = %u ms", 
      dwRoundTripTimeMs, dwLocalTime, me->dwSntpRequestTimeMs);
    uf_RecLog(me->m_pParent, log);		// XtraDc_LogMsg(pMe, log); 
//  }

  dwFractSeconds=NTOHL(buf.nBasic.mTxTimestamp.dwFract);
  dwMilliSeconds=dwFractSeconds/TENMILLION ;// To get first three digit from 10 digit no
   
  dwSeconds= dwIntegerSeconds - ( SECSINSEVENTYYEARS + SECSINTENYEARS );
  dwGPSWeek=dwSeconds/SECSINAWEEK;
  dwGPSTow=dwSeconds-(dwGPSWeek*SECSINAWEEK);
  
  // To print remaining time
  SPRINTF(log,"readSock,me->bGetStatus=%d",me->bGetStatus);
  uf_RecLog(me->m_pParent, log);	
  if(me->bGetStatus==TRUE)						// ---- 验证时间
  {		// if -- 1
      int iTimeRemaining;
	  uint32 uiTimeRemaining;
	  uiTimeRemaining= ((dwGPSWeek - me->sTimeStatus.wStartGpsWeek)*604800)+(dwGPSTow - (me->sTimeStatus.wStartGpsMinute)*60);
	  iTimeRemaining=(me->sTimeStatus.wValidDurationHours)*3600- uiTimeRemaining;
      iTimeRemaining=iTimeRemaining/3600;
//	  if(pMe->iDebugLogLevel>=LEVELONE)    {	  
      SPRINTF(log, "Time Remaining : %d Hrs",iTimeRemaining);
	  uf_RecLog(me->m_pParent, log);		//   XtraDc_LogMsg(pMe, log);
 //   }
	  me->bGetStatus=FALSE; 
//    XtraDc_SendResp(pMe, XTRA_USER_SUCCESS, XTRA_EVT_USER_DATA_VALIDITY);
	  uf_RecLog(me->m_pParent, "-----  XTRA_USER_SUCCESS");	
	  CleanSntp(me);
	  XTRA_CleanState(me);

	  // ----  有效期到了，	重新下载星历
	  if (iTimeRemaining <= 5)	  {	
		  ISHELL_SetTimer(me->a.m_pIShell, 500, (PFNNOTIFY)XTRA_ReqDwnld, (uint32*) me);
	  }

	   return nErr;
  }		// if -- 1

  xTime.dTimeMsec= (uint64)dwSeconds*1000 + (uint64)dwMilliSeconds;
  //  xTime.dwTimeUncMsec=7500;
  xTime.dwTimeUncMsec = dwRoundTripTimeMs;
  xTime.bRefToUtcTime=TRUE;
  xTime.bForceFlag=FALSE;

//  if(pMe->iDebugLogLevel>=LEVELTWO)  {
    SPRINTF(log,"Time:%u Seconds  [ %u GPSWeek   %u GPSTow(ms)] Uncertainty: %u ms, UTCFlag: %u, ForceFlag: %u",
      dwSeconds,dwGPSWeek,((1000*dwGPSTow)+dwMilliSeconds),xTime.dwTimeUncMsec,xTime.bRefToUtcTime,xTime.bForceFlag);
    uf_RecLog(me->m_pParent, log);		// XtraDc_LogMsg(pMe, log);
//  }
  
//  if(pMe->iDebugLogLevel>=LEVELONE)  {
    SPRINTF(log,"For URL %s Total time taken by SNTP Session: %u ms",me->pSntpUrl,GETUPTIMEMS()-me->dwSntpSesStarted);
    uf_RecLog(me->m_pParent, log);		// XtraDc_LogMsg(pMe, log);
//  }

  nErr = IXTRA_SetTime(me->pIXtra, &xTime);			// ---- 注入时间
   
  if (SUCCESS != nErr) 
  {
    SPRINTF(log,"Error in IXTRA_SetTime()");
    uf_RecLog(me->m_pParent, log);		//  XtraDc_LogMsg(pMe, log); 
    goto bail;
  }
   
  // If successful settime cmd callback will drive response back to UI
  CleanSntp(me);
  return nErr;
   
bail:
  nErr = ISOCKPORT_GetLastError(me->pSockPort);
  SPRINTF(log, "In readSock(), SNTP session failed (error code: %d), unable to set time", nErr);	   
  uf_RecLog(me->m_pParent, log);		//  XtraDc_LogMsg(pMe, log); 
  CleanSntp(me);
  XTRA_CleanState(me);
//   XtraDc_SendResp(pMe, XTRA_USER_FAIL, XTRA_EVT_USER_SET_TIME);
  uf_RecLog(me->m_pParent, "-----XTRA_USER_FAIL");
  return nErr;
}
// =======================================================
static int getSntpTime(XtraDcApp* me)
{  
	int nErr = SUCCESS;
	me->dwSntpSesStarted=GETUPTIMEMS();

	uf_RecLog(me->m_pParent, "in getSntpTime");
	   
	nErr = initSntp(me);
	return SUCCESS;
}

// =======================================================
static void ParsePDAPIUrls(XtraDcApp* me, char* pBuffer,int nNumUrls, boolean isDataUrl)
{
	/*
	newgimis *pMe;

	char cSeperator = (char)',';
	char* pStr = pBuffer;

	pMe = me->m_pParent;

	if (nNumUrls > 3) 
	{
		nNumUrls = 3; // max handled by client
	}
	
	while (nNumUrls > 0)
	{
		char* pTok = STRCHR(pStr, cSeperator);
		
		if(0 == pTok) 
		{
			if (0 != pStr[0]) 
			{
				STRLCPY(me->pUrl[3 - nNumUrls], pStr, 128);
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ParsePDAPIUrls in,nNumUrls=%d,me->pUrl[%d]=%s",nNumUrls,3 - nNumUrls,me->pUrl[3 - nNumUrls]);
			}
			break;
		}
		
		// we found another entry.
		*pTok = 0;
		STRLCPY(me->pUrl[3 - nNumUrls], pStr, 128);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ParsePDAPIUrls in,nNumUrls=%d,me->pUrl[%d]=%s",nNumUrls,3 - nNumUrls,me->pUrl[3 - nNumUrls]);
		
		pStr = pTok + 1;
		--nNumUrls;
	}
	*/

	newgimis *pMe;

	char cSeperator = (char)',';
	char* pStr = pBuffer;
	char *sc_tmp = NULL;
	char *sc_buf = NULL;
	char *sc_buf1 = NULL;
	char *sc_buf_rl = NULL;

	pMe = me->m_pParent;

	if (nNumUrls > 3) 
	{
		nNumUrls = 3; // max handled by client
	}
	
	sc_buf1 = (char *)MALLOC(128);
	while (nNumUrls > 0)
	{
		char* pTok = STRCHR(pStr, cSeperator);

		
		MEMSET(sc_buf1,0x00,128);
//		STRCPY(sc_buf1,pStr);
		sc_buf = (char *)MALLOC(128);	
		if(0 == pTok) 
		{
			if (0 != pStr[0]) 
			{
				STRCPY(sc_buf1,pStr);
				sc_tmp = STRSTR(sc_buf1,"http");
				if (!sc_tmp)
				{
					sc_tmp = STRSTR(sc_buf1,"HTTP");
					if (!sc_tmp)
					{

						MEMSET(sc_buf,0x00,128);
						STRCPY(sc_buf,"http://");
						STRCAT(sc_buf,sc_buf1);
//						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"sc_buf = %s", sc_buf);
						
					}
					else
					{
						STRCPY(sc_buf,sc_buf1);
					}
				}
				else
				{
					STRCPY(sc_buf,sc_buf1);
				}
				
				if (STRSTR(sc_buf, "xtra.bin") == NULL )
				{
					STRCAT(sc_buf, "/xtra.bin");
				}
				
				STRLCPY(me->pUrl[3 - nNumUrls], sc_buf, 128);
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ParsePDAPIUrls in 1,nNumUrls=%d,me->pUrl[%d]=%s",nNumUrls,3 - nNumUrls,me->pUrl[3 - nNumUrls]);
			}
			break;
		}
		
		// we found another entry.
		*pTok = 0;
		STRCPY(sc_buf1,pStr);
		sc_tmp = STRSTR(sc_buf1,"http");
		if (!sc_tmp)
		{
			sc_tmp = STRSTR(sc_buf1,"HTTP");
			if (!sc_tmp)
			{
				
				MEMSET(sc_buf,0x00,128);


				STRCPY(sc_buf,"http://");
				STRCAT(sc_buf,sc_buf1);
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"sc_buf 1 = %s", sc_buf);			
			}
			else
			{
				STRCPY(sc_buf,sc_buf1);
			}
		}
		else
		{
			STRCPY(sc_buf,sc_buf1);
		}

		if (STRSTR(sc_buf, "xtra.bin") == NULL )
		{
			STRCAT(sc_buf, "/xtra.bin");
		}

		STRLCPY(me->pUrl[3 - nNumUrls], sc_buf, 128);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ParsePDAPIUrls in 0,nNumUrls=%d,me->pUrl[%d]=%s",nNumUrls,3 - nNumUrls,me->pUrl[3 - nNumUrls]);
		
		pStr = pTok + 1;
		--nNumUrls;

		
	}
	FREEIF(sc_buf);
	sc_tmp = NULL;
	FREEIF(sc_buf1);
	
}

// =======================================================
static void PopulateUrls(XtraDcApp* me, boolean isDataUrl)
{
	if(isDataUrl==TRUE)
	{
		MEMCPY(me->pXtraUrl,me->pUrl[0],sizeof(me->pUrl[0]));
		MEMCPY(me->sXtraUrl,me->pUrl[1],sizeof(me->pUrl[1]));
		MEMCPY(me->tXtraUrl,me->pUrl[2],sizeof(me->pUrl[2]));
	}
	else
	{
		MEMCPY(me->pSntpUrl,me->pUrl[0],sizeof(me->pUrl[0]));
		MEMCPY(me->sSntpUrl,me->pUrl[1],sizeof(me->pUrl[1]));
		MEMCPY(me->tSntpUrl,me->pUrl[2],sizeof(me->pUrl[2]));
	}
}

// ======================================================
static void GetConfig(XtraDcApp* me, char* data, char* str, boolean bNonAscii)
{
  char* p;
  uint32 dwVal = 0;
  char buf[130];
  buf[0] = 0;

  p = STRSTR(data, str);
  if(p) 
  {
	  int len = 0;

    p += STRLEN(str);
    while((p[len] != '\r') && len <= 127)
	  {
		  ++len;
	  }

    if (p[len] == '\r')
	{
		char log[130];
		MEMMOVE(buf, p, len);
		buf[len] = 0;

		if (bNonAscii)		  {
		    dwVal = ATOI(buf);
		}

//		  if(me->iDebugLogLevel>=LEVELTWO)      {
			SPRINTF(log, "Cfg: %s", str);
			uf_RecLog(me->m_pParent,log);		//	    XtraDc_LogMsg(me, log);
  //    }
		   
		if (0 == STRCMP(str, "XTRA_PARAM_AUTODWNLD_FLAG:"))
		{
			me->bAutoDwnld = (boolean) dwVal;
		    SPRINTF(log, "Value: %d", me->bAutoDwnld);
		}
		else if (0 == STRCMP(str, "XTRA_PARAM_AUTODWNLD_INTERVAL:"))
		{
			me->dwDwnldInterval = dwVal;
			SPRINTF(log, "Value: %d", me->dwDwnldInterval);
		}
		else if (0 == STRCMP(str, "XTRA_SRV1:"))
		{
			STRLCPY(me->pXtraUrl, buf, 128);
			SPRINTF(log, "Value: %s", me->pXtraUrl);
		}
		else if (0 == STRCMP(str, "XTRA_SRV2:"))
		{
			STRLCPY(me->sXtraUrl, buf, 128);
			SPRINTF(log, "Value: %s", me->sXtraUrl);
		}
		else if (0 == STRCMP(str, "XTRA_SRV3:"))
		{
			STRLCPY(me->tXtraUrl, buf, 128);
			SPRINTF(log, "Value: %s", me->tXtraUrl);
		}
		else if (0 == STRCMP(str, "SNTP_SRV1:"))
		{
 			STRLCPY(me->pSntpUrlFromCfg, buf, 128);
			SPRINTF(log, "Value: %s", me->pSntpUrlFromCfg);
		}
		else if (0 == STRCMP(str, "SNTP_SRV2:"))
		{
			STRLCPY(me->sSntpUrlFromCfg, buf, 128);
			SPRINTF(log, "Value: %s", me->sSntpUrlFromCfg);
		}
		else if (0 == STRCMP(str, "SNTP_SRV3:"))
		{
			STRLCPY(me->tSntpUrlFromCfg, buf, 128);
 			SPRINTF(log, "Value: %s", me->tSntpUrlFromCfg);
		}
// 		else if (0 == STRCMP(str, "SNTP_DELAY_THRESHOLD:"))
// 		{
// 			me->dwDelayThreshold = dwVal;
// 			SPRINTF(log, "Value: %d", me->dwDelayThreshold);
// 		}
// 		else if (0 == STRCMP(str, "TIME_FORCE_FLAG:"))
// 		{
// 			me->bForceFlag = (boolean) dwVal;
// 			SPRINTF(log, "Value: %d", me->bForceFlag);
// 		}
// 		else if (0 == STRCMP(str, "VERBOSE_LOGGING_LEVEL:"))
// 		{
// 			me->iDebugLogLevel = (uint8)dwVal;
// 			SPRINTF(log, "Value: %d", me->iDebugLogLevel);
// 		}
		else if (0 == STRCMP(str, "LIFETIME_SOCKET_CONNECTION:"))
		{
			me->iLifeOfSocket = (uint8)dwVal;
			SPRINTF(log, "Value: %d", me->iLifeOfSocket);
		}
       
//      if(me->iDebugLogLevel>=LEVELTWO)      {
        uf_RecLog(me->m_pParent,log);				// XtraDc_LogMsg(me, log);
//      }
    }
  }
}
// ======================================================
static void ProcessConfig(XtraDcApp* me)
{
	IFile* pIFile = 0;
	char buf[128];
	AEEFileInfo fInfo;
	int nErr = SUCCESS;
	char* p = "fs:/~0x01039574/gpsOneXtraClient.cfg";
	char* data = 0;

	uf_RecLog(me->m_pParent, "in ProcessConfig");		//	XtraDc_LogMsg(me, buf);

	pIFile = IFILEMGR_OpenFile(me->pFileMgr, p, _OFM_READ);
	
	if (pIFile == NULL) 
	{
		SPRINTF(buf, "gpsOneXtraClient.cfg doesn't exist or permission denied");
		uf_RecLog(me->m_pParent, buf);		//	XtraDc_LogMsg(me, buf);
		SetDefaultSntpParam(me);
		return;
	}
	
	nErr = IFILE_GetInfo(pIFile, &fInfo);
	if (SUCCESS != nErr) 
	{
		uf_RecLog(me->m_pParent,"IFILE_GetInfo failed");
		goto bail;
	}
	
//	if(me->iDebugLogLevel>=LEVELTWO) 	{
		SPRINTF(buf, "cfg file size: %d", fInfo.dwSize);
		uf_RecLog(me->m_pParent, buf);			// XtraDc_LogMsg(me, buf);
//	}
	
	data = MALLOC(fInfo.dwSize + 10); 
	if (0 == data) 
	{
		goto bail;
	}
    
	nErr = IFILE_Read(pIFile, data, fInfo.dwSize);			// ---- 从CFG文件中读入参数
	
	// expect in one shot !! can do better
	if(nErr != (signed)fInfo.dwSize)
	{
		uf_RecLog(me->m_pParent,"one shot read failed!");	// 	XtraDc_LogMsg(me, "one shot read failed!");
		goto bail;
	}
	
	GetConfig(me, data, "XTRA_PARAM_AUTODWNLD_FLAG:", TRUE);     
	GetConfig(me, data, "XTRA_PARAM_AUTODWNLD_INTERVAL:", TRUE); 
	
	GetConfig(me, data, "XTRA_SRV1:", FALSE); 
	GetConfig(me, data, "XTRA_SRV2:", FALSE); 
	GetConfig(me, data, "XTRA_SRV3:", FALSE); 
	
	GetConfig(me, data, "SNTP_SRV1:", FALSE); 
	GetConfig(me, data, "SNTP_SRV2:", FALSE); 
	GetConfig(me, data, "SNTP_SRV3:", FALSE); 
	
	GetConfig(me, data, "SNTP_DELAY_THRESHOLD:",TRUE);
	GetConfig(me, data, "TIME_FORCE_FLAG:", TRUE); 
	GetConfig(me, data, "VERBOSE_LOGGING_LEVEL:", TRUE); 
	
bail:
	if(0 != pIFile) 
	{
		IFILE_Release(pIFile);
	}
	if(0 != data) 
	{
		FREE(data);
	}
}

// ======================================================
static void SetDefaultSntpParam(XtraDcApp* me)
{
	
	MEMSET(me->pSntpUrlFromCfg,0,sizeof(me->pSntpUrlFromCfg));
	MEMSET(me->sSntpUrlFromCfg,0,sizeof(me->sSntpUrlFromCfg));
	MEMSET(me->tSntpUrlFromCfg,0,sizeof(me->tSntpUrlFromCfg));

	MEMCPY(me->pSntpUrlFromCfg,PSNTPURL,sizeof(PSNTPURL));
	MEMCPY(me->sSntpUrlFromCfg,SSNTPURL,sizeof(SSNTPURL));
	MEMCPY(me->tSntpUrlFromCfg,TSNTPURL,sizeof(TSNTPURL));
	
//	me->dwDelayThreshold = DELAYTHRESHOLD;
//	me->bForceFlag=(boolean)FORCEFLAG;
//	me->iDebugLogLevel=LEVELONE;
}

// =======================================================
static void PopulateSntpUrls(XtraDcApp* me)
{
	uf_RecLog(me->m_pParent, "in PopulateSntpUrls");

	MEMSET(me->pSntpUrl,0,sizeof(me->pSntpUrl));
	MEMSET(me->sSntpUrl,0,sizeof(me->sSntpUrl));
	MEMSET(me->tSntpUrl,0,sizeof(me->tSntpUrl));
	
	MEMCPY(me->pSntpUrl,me->pSntpUrlFromCfg,sizeof(me->pSntpUrlFromCfg));
	MEMCPY(me->sSntpUrl,me->sSntpUrlFromCfg,sizeof(me->sSntpUrlFromCfg));
	MEMCPY(me->tSntpUrl,me->tSntpUrlFromCfg,sizeof(me->tSntpUrlFromCfg));
}

// =======================================================
static void XTRA_CleanState(XtraDcApp* me)
{

	newgimis *pMe = me->m_pParent;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"XTRA_CleanState in");

	me->nCmdReq  = 0;
	CALLBACK_Cancel(&me->m_evtTimer);
	if (0 != me->pWebResp) 
	{
		int n=0;			// 2.6.4D2
		uint32 nErr = IWEBRESP_Release(me->pWebResp);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"XTRA_CleanState in, clear piWResp,return=%d,n=%d",nErr,n);
		me->pWebResp = NULL;
	}
	if(0 != me->pSource) 
	{
		ISOURCE_Release(me->pSource);
		me->pSource = 0;
	}
	
	if (0 != me->pIXTRAFile)
	{
		IFILE_Release(me->pIXTRAFile);
		me->pIXTRAFile = 0;
	}
	if (0 != me->pSetDataBuf)
	{
		FREE(me->pSetDataBuf);
		me->pSetDataBuf = 0;
	}

	// Add 2013-04-27
	if (me->pTmpFile != NULL) {
		IFILE_Release(me->pTmpFile);
		me->pTmpFile = 0;
	}
	if (me->pFileMgr != NULL)
	{
		IFILEMGR_Release( me->pFileMgr );
		me->pFileMgr = NULL;
	}

// 2.6.3Ag1 test
	if(me->pWeb)
	{
		int i,n;		// 2.6.4D2
		n = 0;
		i=IWEB_Release(me->pWeb);
		while (i>0) {
			n++;
			i=IWEB_Release(me->pWeb);
		}
		
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"XTRA_CleanState, IWEB_Release=%d,n=%d",i,n);
		me->pWeb = 0;
	}
	
	if (me->pIXtra)
	{
		IXTRA_Release(me->pIXtra);
		me->pIXtra = 0;
    }
// 2.6.3Ag1 test end
	// Add End
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"XTRA_CleanState out");

}
/* =======================================================
					XTRA_EvtCbk
======================================================= */
void XTRA_EvtCbk(void* pCtx, AEEXTRAEventType evtId)
{
  XtraDcApp* me = (XtraDcApp*) pCtx;
  newgimis *pMe = me->m_pParent;

  char buf[128];
  int nErr = SUCCESS;
  int nRead = 0;
   
  SPRINTF(buf, "Evt Callback Rxed, evtId:%d, CmdReq:%d", evtId, me->nCmdReq);
  uf_RecLog(pMe, buf);

  if (0 == me->nCmdReq) {
      uf_RecLog(pMe, "Evt received state: 0");
	  return;
  }

  switch (evtId)
  {
  case AEE_XTRA_EVENT_DOWNLOAD_REQ:			// ---- IXTRA_TriggerDownload 出发回调
	  {
		  AEEXTRADwnldInfo dwnldInfo;
		  
		  if ((me->nCmdReq != AEE_XTRA_CMD_TRIGGERDOWNLOAD) && (me->nCmdReq != 0)) {
			  uf_RecLog(pMe, "state not AEE_XTRA_CMD_TRIGGERDOWNLOAD or NULL");
			  return;
		  }
		  
		  MEMSET(&dwnldInfo, 0, sizeof(dwnldInfo));
		  IXTRA_GetDwnldInfo(me->pIXtra, &dwnldInfo);
		  dwnldInfo.serverInfo.pBuffer=MALLOC(512); 
		  
		  IXTRA_GetDwnldInfo(me->pIXtra, &dwnldInfo);
// 		  {
			  SPRINTF(buf, "XTRA_EvtCbk, Dwnld info - MaxPart: %d, MaxFile: %d", dwnldInfo.dwMaxFilePartSize, dwnldInfo.dwMaxFileSize);
			  uf_RecLog(pMe, buf);
			  SPRINTF(buf, "XTRA_EvtCbk,Dwnld info -  NumSvr: %d", dwnldInfo.serverInfo.dwServerNum);
			  uf_RecLog(pMe, buf);
			  SPRINTF(buf, "XTRA_EvtCbk,Dwnld info - BufLen: %d", dwnldInfo.serverInfo.dwBufferLen);
			  uf_RecLog(pMe, buf);
//		  }
		  
		  if(dwnldInfo.serverInfo.pBuffer!=0)
		  {
			  ParsePDAPIUrls(me, dwnldInfo.serverInfo.pBuffer, dwnldInfo.serverInfo.dwServerNum, TRUE);			//  ---- 解析星历下载url
			  PopulateUrls(me, TRUE);
		  }
		  
		  FREE(dwnldInfo.serverInfo.pBuffer);
		  dwnldInfo.serverInfo.pBuffer=NULL;
		  
		  if((0 == me->nCmdReq)||(me->nCmdReq == AEE_XTRA_CMD_TRIGGERDOWNLOAD)) 
		  {
			  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "XTRA_EvtCbk, AEE_XTRA_CMD_TRIGGERDOWNLOAD,Starting download, dwDwnldMode=%d",me->dwDwnldMode);

			  XTRA_StartTimer(me);
			  me->nCmdReq  = AEE_XTRA_CMD_SET_DATA;
			  
			  if (me->dwDwnldMode == XTRA_DWNLD_MODE_EFS) 
			  {
				  XTRA_DwnldEfs(me, "fs:/~/xtra.bin");	
			  }
			  else 
			  {
				  XTRA_DwnldNet(me);
			  }
		  }
	  }
	  break;
  case AEE_XTRA_EVENT_STATUS:
	  {
		AEEXTRAStatusInfo statusInfo;
		MEMSET(&statusInfo, 0, sizeof(statusInfo));
		IXTRA_GetStatusInfo(me->pIXtra, &statusInfo);

//		if(me->iDebugLogLevel>=LEVELTWO) 		{
		  SPRINTF(buf, "Event status: %d", statusInfo.cStatus);
		  uf_RecLog(pMe, buf);
//		}

		if (me->nCmdReq == AEE_XTRA_CMD_DATA_VALIDITY)
		{
//		  if(me->iDebugLogLevel>=LEVELTWO)		  {
			SPRINTF(buf, "StartGPSWeek: %d, StartGPSMin: %d, Duration(hours): %d", 
						statusInfo.wStartGpsWeek, statusInfo.wStartGpsMinute, 
						statusInfo.wValidDurationHours);
			uf_RecLog(pMe, buf);
//		  }

		  // check status value for SUCCESS
		  if (statusInfo.cStatus == SUCCESS)
		  {
			me->sTimeStatus=statusInfo;
			me->bGetStatus=TRUE;
			me->nCmdReq = 0;
			XTRA_ReqSetTime(me);
		  }
		  else
		  {
			XTRA_CleanState(me);
		  }
		}
		else if (me->nCmdReq == AEE_XTRA_CMD_SET_DATA)			// ---- 要求继续写其他部分
		{
		  if (statusInfo.cStatus == 0)
		  {
			// all parts have been sent and acknowledged
			if (me->nSetDataPartNum == me->nSetDataTotalParts)
			{
			  uf_RecLog(pMe, "SetData complete - received last successful ack.");	// ----  下载完成

			 //  ----  下载完成，开始注入时间  	
			 // ISHELL_SetTimer(pMe->theApp.m_pIShell, 1000, (PFNNOTIFY)XTRA_ReqSetTime, (uint32*) me);
			 // zjie  下载完成，验证有效性
			 // ISHELL_SetTimer(pMe->theApp.m_pIShell, 500, (PFNNOTIFY)XTRA_ReqDataValidity, (uint32*) me);
			  ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_STOP,0, 0);

			  goto Set_Data_Complete;
			}

			// read next part
			nRead = IFILE_Read(me->pIXTRAFile, me->pSetDataBuf, MAX_READ_XTRA);

			// read error
			if (nRead <= 0)
			{
			  SPRINTF(buf, "XTRA file read returned less than expected: %d", nRead);
			  uf_RecLog(pMe, buf);
			  goto Set_Data_Complete;
			}

			++me->nSetDataPartNum;
			me->dataInfo.bDwnldSuccess = TRUE;
			me->dataInfo.cPartNum = me->nSetDataPartNum;
			me->dataInfo.cPartsTotal = me->nSetDataTotalParts;
			me->dataInfo.dwXTRADataLen = nRead;
			me->dataInfo.pXTRAData = (uint8*)me->pSetDataBuf;

//			if(me->iDebugLogLevel>=LEVELTWO) 			{
			  SPRINTF(buf, "SetData Read: %d, PartNum: %d, TotalParts: %d", 
				nRead, me->nSetDataPartNum, me->nSetDataTotalParts);
			  uf_RecLog(pMe, buf);
//			}

			// call SetData 
			nErr = IXTRA_SetData(me->pIXtra, &me->dataInfo);
			if (SUCCESS != nErr)
			{
			  uf_RecLog(pMe, "IXTRA_SetData failed");
			  goto Set_Data_Complete;
			}
		  }
		  else
		  {
			goto Set_Data_Complete;
		  }
		}
		else if (me->nCmdReq == AEE_XTRA_CMD_SET_PARAM)
		{
			// 		  // send response to UI
			// 		  if (statusInfo.cStatus == SUCCESS)
			// 		  {
			// 			XtraDc_SendResp(me, XTRA_USER_SUCCESS, XTRA_EVT_USER_SET_PARAM);
			// 		  }
			// 		  else
			// 		  {
			// 			XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_SET_PARAM);
			// 		  }
			// 
			// 		  XTRA_CleanState(me);
		}
		else if (me->nCmdReq == AEE_XTRA_CMD_TIME_INFO)			
		{
	// 		  // send response to UI
			  if (statusInfo.cStatus == SUCCESS)
			  {
	// 			XtraDc_SendResp(me, XTRA_USER_SUCCESS, XTRA_EVT_USER_SET_TIME);
				  uf_RecLog(pMe,"-----AEE_XTRA_EVENT_STATUS-AEE_XTRA_CMD_TIME_INFO SUCCESS");
			  }
			  else
			  {
				 uf_RecLog(pMe,"-----AEE_XTRA_EVENT_STATUS-AEE_XTRA_CMD_TIME_INFO FAIL");
	// 			XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_SET_TIME);
			  }
			  XTRA_CleanState(me);
		}
		else if (me->nCmdReq == AEE_XTRA_CMD_TRIGGERDOWNLOAD)
		{
		  // trigger download request should send a AEE_XTRA_EVENT_DOWNLOAD_REQ event
		  // unless there was an error.
		  // ignore success status event from the trigger download call.
		  if (statusInfo.cStatus != SUCCESS)
		  {
			if (me->dwDwnldMode == XTRA_DWNLD_MODE_NET)
			{
		//	  XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_INIT_DWNLD);
			    uf_RecLog(pMe, "CMD_TRIGGERDOWNLOAD, MODE_NET, failed");
			}
			else 
			{
			 // XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_INIT_DWNLD_EFS);
				uf_RecLog(pMe, "CMD_TRIGGERDOWNLOAD, MODE_EFS, failed");
			}

			XTRA_CleanState(me);
		  }
		}
	}
	  break;
  case AEE_XTRA_EVENT_TIME_REQ:
	  {
		  // Get info on packet size and max packets.
		  AEEXTRATimeInfo timeInfo;
		  
//		  if(me->iDebugLogLevel>=LEVELTWO)		  {
			uf_RecLog(pMe,"In AEE_XTRA_EVENT_TIME_REQ");    //  XtraDc_LogMsg(me, "In AEE_XTRA_EVENT_TIME_REQ");
//		  }
		  
		  if (me->nCmdReq != 0)
		  {
//			  if(me->iDebugLogLevel>=LEVELTWO) 			  {
			  uf_RecLog(pMe,"state not NULL");		//	  XtraDc_LogMsg(me, "state not NULL");
//			  }
			  return;
		  }
		  
		  MEMSET(&timeInfo, 0, sizeof(timeInfo));
		  IXTRA_GetTimeInfo(me->pIXtra, &timeInfo);
		  timeInfo.serverInfo.pBuffer=MALLOC(512); 
		  
		  IXTRA_GetTimeInfo(me->pIXtra, &timeInfo);
		  
//		  if(me->iDebugLogLevel>=LEVELTWO) 		  {
			  SPRINTF(buf, "Dwnld info - Failover Thresh: %d", 
				  timeInfo.dwOneWayDelayFailoverThreshold);
			  uf_RecLog(pMe, buf);			// XtraDc_LogMsg(me, buf);
			  
			  SPRINTF(buf, "Time info - NumSvr: %d", timeInfo.serverInfo.dwServerNum);
			  uf_RecLog(pMe, buf);		// XtraDc_LogMsg(me, buf);
			  SPRINTF(buf, "Time info - BufLen: %d", timeInfo.serverInfo.dwBufferLen);
			  uf_RecLog(pMe, buf);		//	XtraDc_LogMsg(me, buf);
//		  }
		  
		  if(timeInfo.serverInfo.pBuffer[0]!=0)
		  {
			  ParsePDAPIUrls(me, timeInfo.serverInfo.pBuffer, timeInfo.serverInfo.dwServerNum, FALSE);
			  PopulateUrls(me, FALSE);
		  }
		  
		  FREE(timeInfo.serverInfo.pBuffer);
		  timeInfo.serverInfo.pBuffer=NULL;    
		  
		  if(0 == me->nCmdReq) 
		  {
			  me->nCmdReq  = AEE_XTRA_CMD_TIME_INFO;
			  XTRA_StartTimer(me);  // cleanup timeout timer.
			  getSntpTime(me);
		  }
	  }
	  break;
  default:
	  break;
  }

  return;

Set_Data_Complete:
  if (nErr != SUCCESS)
  {
	   uf_RecLog(pMe, "download failure ");
  }
  else
  {
	  uf_RecLog(pMe, "download SUCCESS ");
	  me->cDwnState	= 1;			// 2013-04-12
  }

  XTRA_CleanState(me);
  return;
}

void XTRA_CmdCbk(void* pCtx, AEEXTRACmdType cmdId, int32 nErr)
{
  char buf[256];
  XtraDcApp* me = (XtraDcApp*) pCtx;

//	  if(me->iDebugLogLevel>=LEVELTWO)  {
		SPRINTF(buf, "Cmd Callback Rxed, cmdId:%d, err:%d", cmdId, nErr);
		uf_RecLog(me->m_pParent, buf);
//	  }

	  if (0 == me->nCmdReq)
	  {
//		if(me->iDebugLogLevel>=LEVELTWO)		{
		  uf_RecLog(me->m_pParent, "Cmd callback received state: 0!");
//		}
		return;
	  }

	  if (cmdId == AEE_XTRA_CMD_SET_DATA) 
	  {		// if AEE_XTRA_CMD_SET_DATA
			if (me->nCmdReq != AEE_XTRA_CMD_SET_DATA)  {
	//		  if(me->iDebugLogLevel>=LEVELTWO)		  {
				uf_RecLog(me->m_pParent, "state not AEE_XTRA_CMD_SET_DATA");
	//		  }
			  return;
			}
			else if (nErr != SUCCESS)
			{
			  if (me->dwDwnldMode == XTRA_DWNLD_MODE_NET)  {
	//			XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_INIT_DWNLD);
					uf_RecLog(me->m_pParent, "AEE_XTRA_CMD_SET_DATA,XTRA_DWNLD_MODE_NET failure");
			  }
			  else 
			  {
				  uf_RecLog(me->m_pParent, "AEE_XTRA_CMD_SET_DATA,XTRA_EVT_USER_INIT_DWNLD_EFS failure");
	//			XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_INIT_DWNLD_EFS);
			  }
			  XTRA_CleanState(me);
			  return;
			}
	  }	// if AEE_XTRA_CMD_SET_DATA
	  else if(cmdId == AEE_XTRA_CMD_TRIGGERDOWNLOAD) 
	  {		// if AEE_XTRA_CMD_TRIGGERDOWNLOAD
			if (me->nCmdReq != AEE_XTRA_CMD_TRIGGERDOWNLOAD)  {
			//  if(me->iDebugLogLevel>=LEVELTWO)		  {    
				 uf_RecLog(me->m_pParent, "state not AEE_XTRA_CMD_TRIGGERDOWNLOAD");
			 // }
				return;
			}
			else if (nErr != SUCCESS)
			{
				if (me->dwDwnldMode == XTRA_DWNLD_MODE_NET)  {
				//	XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_INIT_DWNLD);
					uf_RecLog(me->m_pParent, "AEE_XTRA_CMD_TRIGGERDOWNLOAD,XTRA_DWNLD_MODE_NET failure");
				}
				else {
//					XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_INIT_DWNLD_EFS);
					uf_RecLog(me->m_pParent, "AEE_XTRA_CMD_TRIGGERDOWNLOAD,XTRA_EVT_USER_INIT_DWNLD_EFS failure");
				}
				XTRA_CleanState(me);
				return;
			}
	  }  // if AEE_XTRA_CMD_TRIGGERDOWNLOAD
	  else if(cmdId == AEE_XTRA_CMD_SET_PARAM) 
	  { //	AEE_XTRA_CMD_SET_PARAM
		  if (me->nCmdReq != AEE_XTRA_CMD_SET_PARAM)  {
			//  if(me->iDebugLogLevel>=LEVELTWO)			  {    
					uf_RecLog(me->m_pParent, "state not AEE_XTRA_CMD_SET_PARAM");
			//  }
			  return;
		  }
		  else if (SUCCESS != nErr)
		  {
			// XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_SET_PARAM);
			  uf_RecLog(me->m_pParent, "AEE_XTRA_CMD_SET_PARAM failure");
 		      XTRA_CleanState(me);
		  }
	  }	//	AEE_XTRA_CMD_SET_PARAM
	  else if(cmdId == AEE_XTRA_CMD_DATA_VALIDITY) 
	  { // if AEE_XTRA_CMD_DATA_VALIDITY
		  if (me->nCmdReq != AEE_XTRA_CMD_DATA_VALIDITY)
		  {
			//  if(me->iDebugLogLevel>=LEVELTWO)			  {    
//				XtraDc_LogMsg(me, "state not AEE_XTRA_CMD_DATA_VALIDITY");
				uf_RecLog(me->m_pParent, "state not AEE_XTRA_CMD_DATA_VALIDITY");
			 // }
			  return;
		  }
		  else if (SUCCESS != nErr)
		  {
			  // XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_DATA_VALIDITY);
			   uf_RecLog(me->m_pParent, "AEE_XTRA_CMD_DATA_VALIDITY failure");
		       XTRA_CleanState(me);
		  }
	  }	// if AEE_XTRA_CMD_DATA_VALIDITY
	  else if(cmdId == AEE_XTRA_CMD_TIME_INFO) 
	  {	// if AEE_XTRA_CMD_TIME_INFO
		  if (me->nCmdReq != AEE_XTRA_CMD_TIME_INFO)  {
			//  if(me->iDebugLogLevel>=LEVELTWO)
			//  {    
			  uf_RecLog(me->m_pParent, "state not AEE_XTRA_CMD_TIME_INFO");
			  //	XtraDc_LogMsg(me, "state not AEE_XTRA_CMD_TIME_INFO");
			//  }
			  return;
		  }
		  else if (SUCCESS != nErr)
		  {
//			XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_SET_TIME);
			  uf_RecLog(me->m_pParent, "AEE_XTRA_CMD_TIME_INFO failure");
			  XTRA_CleanState(me);
		  }
	  }	// if AEE_XTRA_CMD_TIME_INFO
}

/* ===================================================================
		XTRA_DwnldEfs		// zjie 把下载的星历数据写入到手机
 =================================================================== */
static void XTRA_DwnldEfs(XtraDcApp* me, char* pFile)		
{
  newgimis *pMe = me->m_pParent;
  char buf[128];
  int nRead;
  AEEFileInfo fInfo;
  int nErr;

  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"XTRA_DwnldEfs, pIXtra addr=%d",me->pIXtra);
  // Add by zjie 2013-07031
  if ( (me->pIXtra == NULL) || (me->cTimeOut == 1) ) {		
	 XTRA_CleanState(me);
	  return;
  }
  // Add End

  me->pIXTRAFile = 0;
  me->nSetDataPartNum = 0;
  me->pSetDataBuf = 0;
  me->nSetDataTotalParts = 0;

  // Open xtra.bin file.
  me->pIXTRAFile = IFILEMGR_OpenFile(me->pFileMgr, pFile, _OFM_READ);

  // check if open succeeded
  if (0 == me->pIXTRAFile)			// 打开失败
  {
    AEEXTRADataInfo tempDataInfo;
    SPRINTF(buf, "Error in opening file: %s", pFile);
	uf_RecLog(pMe, buf);

	MEMSET(&tempDataInfo, 0, sizeof(tempDataInfo));
    tempDataInfo.bDwnldSuccess = FALSE;

    IXTRA_SetData(me->pIXtra, &tempDataInfo);
	uf_RecLog(pMe,"open pIXTRAFile failure");
	XTRA_CleanState(me);
    return;
  }				// 打开失败

  // get file info
  nErr = IFILE_GetInfo(me->pIXTRAFile, &fInfo);
  if (SUCCESS != nErr)				// 得不到文件信息
  {
    AEEXTRADataInfo tempDataInfo;
    SPRINTF(buf, "GetInfo failed for the file: %s", pFile);

	MEMSET(&tempDataInfo, 0, sizeof(tempDataInfo));
    tempDataInfo.bDwnldSuccess = FALSE;

    IXTRA_SetData(me->pIXtra, &tempDataInfo);
	uf_RecLog(pMe,"GetInfo pIXTRAFile failure");
    XTRA_CleanState(me);
    return;
  }						// 得不到文件信息

  // calculate total number of parts that the file needs to be broken up into
  me->nSetDataTotalParts = (uint8)(fInfo.dwSize / MAX_READ_XTRA);
  ++me->nSetDataTotalParts;

  // malloc space for each parts
  me->pSetDataBuf = (char*)MALLOC(MAX_READ_XTRA * 2);
  if (0 == me->pSetDataBuf)
  {
    uf_RecLog(pMe, "malloc failed");
    goto bail;
  }

  // read a part
  nRead = IFILE_Read(me->pIXTRAFile, me->pSetDataBuf, MAX_READ_XTRA);
  if (nRead < 0)
  {
    uf_RecLog(pMe, "XTRA file read returned less than expected.");
    goto bail;
  }
  ++me->nSetDataPartNum;

//  {
    SPRINTF(buf, "read: %d bytes from file", nRead);
    uf_RecLog(pMe, buf);
//  }
   
  me->dataInfo.bDwnldSuccess = TRUE;							// zjie 写第1部分，其他的在回调中写
  me->dataInfo.cPartNum = me->nSetDataPartNum;
  me->dataInfo.cPartsTotal = me->nSetDataTotalParts;
  me->dataInfo.dwXTRADataLen = nRead;
  me->dataInfo.pXTRAData = (uint8*)me->pSetDataBuf;

//  if(me->iDebugLogLevel>=LEVELTWO)  {
    SPRINTF(buf, "SetData - %d bytes, part: %d, total parts: %d", 
					nRead, me->nSetDataPartNum, me->nSetDataTotalParts);
     uf_RecLog(pMe, buf);
 // }

  // call SetData
  nErr = IXTRA_SetData(me->pIXtra, &me->dataInfo);

  if (SUCCESS != nErr)
  {
    uf_RecLog(pMe, "IXTRA_SetData failed");
    goto bail;
  }

  return;
   
bail:
//  XtraDc_SendResp(me, XTRA_USER_FAIL, XTRA_EVT_USER_INIT_DWNLD_EFS);
  XTRA_CleanState(me);
  return;
}

// ============================================
//  发出http请求
// ============================================
static void XTRA_DwnldNet(XtraDcApp* me)
{

	newgimis *pMe = me->m_pParent;

	uf_RecLog(pMe, "XTRA_DwnldNet in");

	CALLBACK_Init(&me->m_webcb, WebCbk, (void*)me);
	me->dwHttpSesStarted=GETUPTIMEMS();
	web20(me);
}

// =============================================
// Http handling code
static int web20(XtraDcApp* me)
{
  newgimis *pMe = me->m_pParent;

  WebOpt zWebOpts[2];
   
  // 2.6.4D2 zWebOpts[0].nId  = WEBOPT_FLAGS;
  // 2.6.4D2 zWebOpts[0].pVal = (void*) WEBREQUEST_NOKEEPALIVE;
  zWebOpts[0].nId  = WEBOPT_END;
																		//   MEMSET(me->pXtraUrl,0,sizeof(me->pXtraUrl));				// 2013-07-31
																		//   STRCPY(me->pXtraUrl,"http://xtra3.gpsonextra.net/xtra.bin");
  IWEB_GetResponseV(me->pWeb,
                    &me->pWebResp,
                    &me->m_webcb,
                    me->pXtraUrl,
                    zWebOpts);

  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "web20 in,Url=%s",me->pXtraUrl);

  return SUCCESS;
}

// ===============================================
static void WebCbk(void* po)				//  zjie 下载星历http返回
{
  newgimis *pMe;
  XtraDcApp* me = (XtraDcApp*) po;
  WebRespInfo* pInfo;
  char buf[256];

//  Add by zjie 2013-07-31
  pMe = me->m_pParent;
  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"WebCbk in,pWebResp=%d",me->pWebResp);
  if ( (me->pWebResp == NULL) || (me->cTimeOut == 1) ) {	
	 XTRA_CleanState(me);
	 return;
  }
// Add End

  pInfo = IWEBRESP_GetInfo(me->pWebResp);
  if(0 == pInfo) 
  {
    uf_RecLog(me->m_pParent, "WebResponse NULL");
    XTRA_CleanState(me);
    return;
  }

  if (!WEB_ERROR_SUCCEEDED(pInfo->nCode)) 
  {
    SPRINTF(buf, "WebResponse Error: %d", pInfo->nCode);
    uf_RecLog(me->m_pParent, buf);
	ISHELL_PostEvent( me->a.m_pIShell,me->a.clsID, XTRA_EVT_STOP,0, 0);		//2012-10-16  test 2012-10-24 
    XTRA_CleanState(me);
    return;
  }

 // {
	uf_RecLog(me->m_pParent, "WebCbk in, WebResponse Success");
	SPRINTF(buf, "Content Length: %d", pInfo->lContentLength);
	uf_RecLog(me->m_pParent,buf);
  //}

  XTRA_StartTimer(me);			//  Add by zjie 2013-07-31

  me->cPartTotal = (char)(pInfo->lContentLength / 1460);
  me->cPart = 0;
  me->cPartTotal += 1;

			SPRINTF(buf, "Total Parts : %d", me->cPartTotal);
			uf_RecLog(me->m_pParent, buf);

  if(0 != pInfo->cpszContentType) 
  {
	  SPRINTF(buf, "Content Type: %s", pInfo->cpszContentType);
	  uf_RecLog(me->m_pParent, buf);
  }
   
  if(0 != pInfo->cpszCharset) 
  {
    SPRINTF(buf, "CharSet: %s", pInfo->cpszCharset);
	uf_RecLog(me->m_pParent, buf);
  }

  if(0 != pInfo->pisMessage) 
  {
    uf_RecLog(me->m_pParent, "Got Message");
    if(0 == pInfo->lContentLength) 
    {
      return; 	      
    }

    me->pSource = pInfo->pisMessage;
    ISOURCE_AddRef(me->pSource);				// zjie 在ReadBody中用到
	{
		int n=0;			// 2.6.4D2
		uint32 nErr = IWEBRESP_Release(me->pWebResp);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"WebCbk in, clear piWResp,return=%d,n=%d",nErr,n);
		me->pWebResp = NULL;
	}

    me->tStart = GETUPTIMEMS(); // start reading
    me->nBytes = 0; // init read counter
      
    CALLBACK_Cancel(&me->m_webcb);
    CALLBACK_Init(&me->m_webcb, ReadBody, me);

    IFILE_Truncate(me->pTmpFile, 0);			// zjie 把旧的文件清空
    ReadBody(me);
  }
}
// ===============================================
static void ReadBody(void* po)
{
  XtraDcApp* pMe = (XtraDcApp*) po;
  int32 n;
  char buf[1460];
  char log[128];

  // Add by zjie 2013-07-31
  if ( (pMe->pSource == NULL) || (pMe->cTimeOut == 1) )
  {	
	  newgimis *Me = pMe->m_pParent;
	  ud_RecLog(Me->a.m_pIShell,Me->cWrtLog,&(Me->nInd), "ReadBody in, pSource addr=%d",pMe->pSource);
	  XTRA_CleanState(pMe);
	  return;
  }
  // End by zjie 2013-07-31

  n = ISOURCE_Read(pMe->pSource, buf, sizeof(buf));
  if(ISOURCE_WAIT == n) 
  {
    ISOURCE_Readable(pMe->pSource, &pMe->m_webcb);
	  return;
  }

  if (0 < n)		// zjie 读到数据
  {
    int nErr;

    pMe->nBytes += n;
	pMe->tEnd = GETUPTIMEMS(); // end reading

      SPRINTF(log, "Interim Data Reading: %d, time:%u", pMe->nBytes, (pMe->tEnd - pMe->tStart));
      uf_RecLog(pMe->m_pParent, log);
	  nErr = IFILE_Write(pMe->pTmpFile, buf, n);
	  if (n != nErr)
	  {
		  SPRINTF(log, "IFILE_Write() exp :%d, got: %d", n, nErr);
		  uf_RecLog(pMe->m_pParent, log);
	  }
  }

  if (!ISOURCE_Exhausted(n))			// zjie 检查数据是否已经全部下载完成
  {
    ISOURCE_Readable(pMe->pSource, &pMe->m_webcb);
    return;
  }

  pMe->tEnd = GETUPTIMEMS(); // end reading

//  {
		SPRINTF(buf, "For URL %s, Time taken by  - HTTP Download + EFS write: %u", pMe->pXtraUrl,
			(pMe->tEnd - pMe->tStart));
		uf_RecLog(pMe->m_pParent, buf);
//  }

  ISOURCE_Release(pMe->pSource);
  pMe->pSource = 0;
  pMe->cPart = 0;
  pMe->cPartTotal = 0;

  XTRA_DwnldEfs(pMe, XTRA_TMP_FILE);
}





