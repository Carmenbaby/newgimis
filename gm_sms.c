#include "newgimis.bid"
#include "newgimis.h"

#include "gm_sms.h"
#include "gmTools.h"
/*===========================================================================
// 设置短信接收结束的铃声
 =========================================================================== */
#define Sms_FinishAlert(pMe) {	\
	ISHELL_Beep(pMe->a.m_pIShell, BEEP_MSG, TRUE);		\
	ISHELL_Beep(pMe->a.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);	\
	ISHELL_SetTimer(pMe->a.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*)pMe );	\
}

// =======================================================================================
boolean Sms_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	cSms* me;

	if (pMe->pSms != NULL)
	{
		me = (cSms *)pMe->pSms;
	}
	else if (eCode != SMS_EVT_START)
	{
		return FALSE;
	}

	switch (eCode)
	{
    case SMS_EVT_START:
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"in SMS_EVT_START");
		
		if (pMe->pSms == NULL) 
		{
			me = (cSms *)MALLOC(sizeof(cSms));
			MEMSET(me,0,sizeof(cSms));
			me->m_pParent = pMe;
			pMe->pSms = (void *)me;
			if ( Sms_Init(me) == SUCCESS )
				return TRUE;
			else
				return FALSE;
		}
		else
		{
			return TRUE;
		}
		break;
    case SMS_EVT_STOP:							
		Sms_Close(me);		
		me->m_pParent = 0;
		FREEIF(pMe->pSms);
        break;
	case EVT_SAVE_SMS:
		{
			int iStatus = wParam; 		// 1 : 解析收到的数据，存入短信列表中
			switch (iStatus)
			{
			case 1:
				Sms_trf(me);
				me->m_SMSList.nCurId = 0;
				ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,EVT_SAVE_SMS,0,0);
				return TRUE;
			case 0:
				Sms_SaveSms(me);
				return TRUE;
			}
		}
		break;
	case SMS_EVT_DOWNLOAD:
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"in SMS_EVT_DOWNLOAD, nState=%d", me->nState);
		if (me->nState == 0) 
		{
			Sms_DownLoad(me);
			return TRUE;
		}
		break;
		
    default:
		break;
    }
	
    return(FALSE);
}

/********************************************************************************************
// 初始化
// 传入参数
********************************************************************************************/
static int Sms_Init(cSms* me)
{
	//  char buf[128];   
	newgimis *pMe = me->m_pParent;
	int nErr = SUCCESS;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_Initt in");
	
	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)&me->pWeb);	
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "Sms_Init IWeb err=%d", nErr);
		nErr = EFAILED;
		goto ExitHere;
	}
	
    nErr = SMS_CreateInterface(me);
	me->nState = 0;
	me->nBytes = 0;
	me->nBodyHasRead = 0;
	me->pRecData = NULL;
	MEMSET(me->sUrl,0,sizeof(me->sUrl));
	
ExitHere:
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_Init out nErr=%d",nErr);
	
	return nErr;
}

// 实例化短信接口
// 返回值：
//		成功 ：SUCCESS, 失败 EFAILED
static int SMS_CreateInterface(cSms * me)
{
	newgimis *pMe = me->m_pParent;
	int nErr = 0;
	
	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SMSSTORAGE, (void**)&me->pISmsStorage);
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISHELL_CreateInstance,AEECLSID_SMSSTORAGE,nErr=%d",nErr);
		me->pISmsStorage = NULL;
		nErr = EFAILED;
		goto bail;
	}	
	
	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SMSMSG, (void**)&me->pISmsMsg);
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISHELL_CreateInstance,AEECLSID_SMSMSG,nErr=%d",nErr);
		me->pISmsMsg = NULL;
		nErr = EFAILED;
		goto bail;
	}	
	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SMS, (void**)&me->pISMS);
    if (nErr != SUCCESS)
    {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISHELL_CreateInstance,AEECLSID_SMS,nErr=%d",nErr);
		me->pISMS = NULL;
		nErr = EFAILED;
		goto bail;
    }
	
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISHELL_CreateInstance,AEECLSID_SMSMSG and AEECLSID_SMSSTORAGE ok");
	
bail:
	return nErr;
}


// 下载信息
static void Sms_DownLoad(cSms* me)
{
	int idx;
	newgimis *pMe;
	WebOpt zWebOpts[7];
	char sFormat[250] = {0};
	
	pMe = me->m_pParent;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd) ,"Sms_DownLoad in");
	
	// 设置http地址 zc.gota.richmap.cn/brewGPS/BrewServlet?act=httpdec; 变成  zc.gota.richmap.cn/message/getMessage?ver=1.0
	idx = ud_DistToChar(me->m_pParent->strCenterIp,'/');  
	if ( idx >= 0 )
		MEMCPY(sFormat, me->m_pParent->strCenterIp, idx); 
	else
		STRCPY(sFormat, me->m_pParent->strCenterIp);
	// Change 2013-09-23	SPRINTF(me->sUrl,"http://%s/message/getMessage?ver=1.0",sFormat); 
	if (STRLEN(pMe->MyMobileNo) > 0 )
		SPRINTF(me->sUrl,"http://%s/message/getMessage?tel=%s&imsi=%s&ver=1.0",sFormat,pMe->MyMobileNo,pMe->szMobileID); 
	else
		SPRINTF(me->sUrl,"http://%s/message/getMessage?ver=1.0",sFormat); 
	// Change End
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd) ,"SMS Url=%s",me->sUrl);
	
	me->Overtime = 0;

	{
		int i = 0;
		
		zWebOpts[i].nId  = WEBOPT_CONNECTTIMEOUT;
		zWebOpts[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		// 2.6.2D2		zWebOpts[i].nId  = WEBOPT_FLAGS;
		// 2.6.2D2		zWebOpts[i].pVal = (void*) WEBREQUEST_NOKEEPALIVE;
		// 2.6.2D2		i++;		
		zWebOpts[i].nId  = WEBOPT_PROXYSPEC;
		zWebOpts[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		zWebOpts[i].nId  = WEBOPT_END;
		
	}
	
	CALLBACK_Init(&me->webcb, Sms_GotResp, (void*)me);
	/*IWEB_GetResponseV(me->pWeb,	&me->pWebResp,	&me->webcb, me->sUrl,	zWebOpts);*/

	IWEB_GetResponse(me->pWeb,
		(me->pWeb, &me->pWebResp, &me->webcb, me->sUrl, 
		WEBOPT_HANDLERDATA, me,
		WEBOPT_STATUSHANDLER, Sms_Status,  
			WEBOPT_END));

	me->nState = 1;		// 正在接收数据
	
	return;
}


static void Sms_Status(void *p, WebStatus ws, void *pVal)
{
	char lStr_Sts[51];
	cSms  *me;
	
	if (p == NULL)
		return;
	
	me = (cSms *)p;
	//    pMe = pwa->m_pParent;

	
	MEMSET(lStr_Sts,0,sizeof(lStr_Sts));
	switch (ws) 
	{
	case WEBS_CANCELLED:
		STRCPY(lStr_Sts, "** cancelled...");
		break;
	case WEBS_GETHOSTBYNAME:
		STRCPY(lStr_Sts, "** finding host...");
		break;          
	case WEBS_CONNECT:
		STRCPY(lStr_Sts, "** connecting...");
		break;
	case WEBS_SENDREQUEST:
		STRCPY(lStr_Sts, "** sending...");
		break;
	case WEBS_READRESPONSE: 
		me->Overtime = GETTIMEMS();
		ISHELL_SetTimer(me->m_pParent->a.m_pIShell, 60 * 1000, (PFNNOTIFY)Sms_MonitorSendFile, me);
		STRCPY(lStr_Sts, "** receiving...");
		break;
	case WEBS_GOTREDIRECT:
		STRCPY(lStr_Sts, "** redirect...");
		break;
	case WEBS_CACHEHIT:
		STRCPY(lStr_Sts, "** cache hit...");
		break;
	default:
		if (me->Overtime == 0)
		{			//  Add 2013-09-23
			me->Overtime = GETTIMEMS();														
			ISHELL_SetTimer(me->m_pParent->a.m_pIShell, 60 * 1000, (PFNNOTIFY)Sms_MonitorSendFile, me);		
		}		// Add End
		SPRINTF(lStr_Sts, "** other info=%d",ws);
		break;
	}
	
	uf_RecLog(me->m_pParent,lStr_Sts);
}

void Sms_MonitorSendFile(cSms *me)
{
	char sLogBuf[128];
// 	AECHAR szText[6];
//     AECHAR szFailure[] = {0x4E2D,0x5FC3,0x65E0,0x5E94,0x7B54,0x0000};	// 中心无应答
	
	SPRINTF(sLogBuf,"Sms_MonitorSendFile in,Overtime=%d",me->Overtime);
	uf_RecLog(me->m_pParent,sLogBuf);
	
	if ( me->Overtime == 0)
	{
		//uf_RecLog(me->m_pParent,"没有超时没有超时");
		return;
	}


	Sms_CloseWeb(me);	//备注 考虑还有没有其他没有释放
	
	
	// 如果超过60秒没收到，则出现，主动断掉
// 	WSTRCPY(szText,szFailure);
// 	
// 	
// 	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
// 	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
// 		8, 2,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
// 	IDISPLAY_Update(me->a.m_pIDisplay);
	
// 	// 发送失败，振铃提示
// 	ISHELL_Beep(me->a.m_pIShell, BEEP_MSG, TRUE);
// 	ISHELL_Beep(me->a.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
// 	ISHELL_SetTimer(me->a.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*)me->m_pParent );
// 	
// 	ISHELL_SetTimer(me->a.m_pIShell, 55*100, (PFNNOTIFY)Record_Afterpost, me->m_pParent);
}


/*********************** 应答http返回 *******************************************************/
static void Sms_GotResp(void *p)
{
	
	int nLen =0;
	cSms *me = (cSms *)p;
	newgimis * pMe = me->m_pParent; 
	WebRespInfo *pInfo;
	
	ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"Sms_GotResp in");
	
	pInfo = IWEBRESP_GetInfo(me->pWebResp);
	
	ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"Sms_GotResp response, code: %d",pInfo->nCode);
	if (!WEB_ERROR_SUCCEEDED(pInfo->nCode)) 
		return;
	
	if(0 != pInfo->pisMessage) 
	{
		if(0 == pInfo->lContentLength) 
			return; 	 
		
		me->Overtime = 0;				// 已收到应答

		me->nBytes = pInfo->lContentLength;
		me->pSource = pInfo->pisMessage;
		ISOURCE_AddRef(me->pSource);				// 在Read Body中用到
		IWEBRESP_Release(me->pWebResp);
		me->pWebResp = 0;
		
		CALLBACK_Cancel(&me->webcb);
		CALLBACK_Init(&me->webcb, Sms_ReadBody, me);
		Sms_ReadBody(me);
	}
}



// ==========================================================
// 读取中心端返回的信息
// ==========================================================
static void Sms_ReadBody(cSms *me)
{
	newgimis *pMe;
	int32 nRead;
	int nRecCount;
	char sBuf[MAX_MESSAGE_LEN];
	
	pMe = me->m_pParent;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_ReadBody in");
	
	//	MEMSET(sBodybuf,0,sizeof(sBodybuf));
	nRead = ISOURCE_Read(me->pSource, sBuf, sizeof(sBuf));
	if(ISOURCE_WAIT == nRead) 
	{
		ISOURCE_Readable(me->pSource, &me->webcb);
		return;
	}
	
	if (nRead > 0)		
	{  // -- ISOURCE_ERROR
		
		if (me->pRecData == NULL) 
		{
			if (me->nBytes > 0)	
				me->pRecData = (char *)MALLOC( (me->nBytes+1)*sizeof(char) );
			else
				me->pRecData = (char *)MALLOC( (nRead+1)*sizeof(char));
			
			MEMCPY(me->pRecData,sBuf,nRead);
			me->nBodyHasRead = nRead; 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_ReadBody, dataLen=%d, recevie=%d",me->nBytes,nRead);    
		}
		else 
		{
			if ( me->nBytes == -1) 
			{		
				me->pRecData = (char *)REALLOC( me->pRecData, (me->nBodyHasRead + nRead+1)*sizeof(char) );
				MEMSET(me->pRecData+me->nBodyHasRead,0,(nRead+1)*sizeof(char));
			}	
			
			MEMCPY(me->pRecData+me->nBodyHasRead,sBuf,nRead);
			me->nBodyHasRead += nRead;	
		}
		
	}  // -- ISOURCE_ERROR  
	
	if (!ISOURCE_Exhausted(nRead))			// 检查数据是否已经全部获取
	{
		ISOURCE_Readable(me->pSource, &me->webcb);
		return;
	}
	
	Sms_CloseWeb(me);
	
	nRecCount = *(me->pRecData+4)<<8 | *(me->pRecData+5);		
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_ReadBody in, item count = %d",nRecCount);

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),me->pRecData);//cs 
	
	if ( nRecCount == 0)  
	{		// 没有数据
		me->nState = 0;
		Sms_ReleaseBuf(me);
		Sms_FinishAlert(pMe);

// 		//如果释放。第二条不行
// 		Sms_CloseSMS(me);
// 		CALLBACK_Cancel(&me->cbMsg);
	}
	else
		ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,EVT_SAVE_SMS,1,0);	
	
	return;
}


// 关闭http接口
static void Sms_CloseWeb(cSms *me)
{
	newgimis *pMe;
	//	int nErr = SUCCESS;
	
	pMe = me->m_pParent;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_CloseWeb in");
	
	if (me->pSource != NULL)
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_Close in,pSource != NULL");
		ISOURCE_Release(me->pSource);
		me->pSource = NULL;
	}
	
	if (me->pWebResp != NULL)
	{
		int n=0;			// 2.6.4D2
		uint32 nErr = IWEBRESP_Release(me->pWebResp);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_Close in,clear piWResp,return=%d,n=%d",nErr,n);
		me->pWebResp = NULL;
	}
	
	CALLBACK_Cancel(&me->webcb);
	
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_CloseWeb out");
	return;
}

// 释放m_SMSList中资源
static void Sms_ReleaseBuf(cSms *me)
{
	FREEIF(me->pRecData);
	
	if ( me->m_SMSList.count>0 )
	{
		int i = 0;
		for(i=0;i<me->m_SMSList.count;i++)
			FREEIF(me->m_SMSList.pSmsInfo[i].aeContent);
		me->m_SMSList.count = 0;
		FREEIF(me->m_SMSList.pSmsInfo);
	}
	
	return;
}

// 解析收到的数据，存入短信列表中
static void Sms_trf(cSms * me)
{
	//	char sBuf[] = {0x00,0x00,0x00,0xA4,0x00,0x02,0x00,0x02,0x6E,0x6F,0x00,0x06,0x38,0x38,0x38,0x38,0x38,0x39,0x63,0x74,0x00,0x0F,0xE6,0xB5,0x8B,0xE8,0xAF,0x95,0xE7,0x9A,0x84,0xE7,0x9F,0xAD,0xE4,0xBF,0xA1,0x6E,0x6F,0x00,0x06,0x38,0x38,0x38,0x38,0x38,0x38,0x63,0x74,0x00,0x71,0xE8,0xAF,0xA5,0xE7,0x89,0x88,0xE6,0x9C,0xAC,0xE4,0xB8,0xBB,0xE8,0xA6,0x81,0xE6,0x98,0xAF,0x20,0x62,0x75,0x67,0x20,0xE4,0xBF,0xAE,0xE5,0xA4,0x8D,0xE5,0x92,0x8C,0xE4,0xB8,0x80,0xE4,0xBA,0x9B,0xE5,0x8F,0xAF,0xE7,0x94,0xA8,0xE6,0x80,0xA7,0xE7,0x9A,0x84,0xE6,0x8F,0x90,0xE5,0x8D,0x87,0xEF,0xBC,0x8C,0xE4,0xBD,0x86,0xE6,0x88,0x91,0xE4,0xBB,0xAC,0xE4,0xB9,0x9F,0xE5,0xA2,0x9E,0xE5,0x8A,0xA0,0xE4,0xBA,0x86,0xE6,0x96,0xB0,0xE5,0x8A,0x9F,0xE8,0x83,0xBD,0xEF,0xBC,0x8C,0xE5,0x8C,0x85,0xE6,0x8B,0xAC,0xE8,0xB5,0x84,0xE6,0x96,0x99,0xE5,0xBA,0x93,0xE5,0x9B,0xBE,0xE8,0xA1,0xA8};
	//	char sBuf[] = {0,0,0,164,0,02,00,02,00,110,00,111,00,06,56,56,56,56,56,56,00,99,00,116,00,15,230,181,139,232,175,149,231,154,132,231,159,173,228,0xBF,0xA1,0x00,0x6E,0x00,0x6F,0x00,0x06,0x38,0x38,0x38,0x38,0x38,0x38,0x00,0x63,0x00,74,00,71,E8,AF,A5,E7,89,88,E6,9C,AC,E4,B8,BB,E8,A6,81,E6,98,AF,20,62,75,67,20,E4,BF,AE,E5,A4,8D,E5,92,8C,E4,B8,80,E4,BA,9B,E5,8F,AF,E7,94,A8,E6,80,A7,E7,9A,84,E6,8F,90,E5,8D,87,EF,BC,8C,E4,BD,86,E6,88,91,E4,BB,AC,E4,B9,9F,E5,A2,9E,E5,8A,A0,E4,BA,86,E6,96,B0,E5,8A,9F,E8,83,BD,EF,BC,8C,E5,8C,85,E6,8B,AC,E8,B5,84,E6,96,99,E5,BA,93,E5,9B,BE,E8,A1,A8};
	uint16  nMsgCount = 0;
	uint16  nItemCount = 0;
	
	ISmsRecords *pRecords = NULL;

	me->nState = 5;

	pRecords = gm_SMSformat(me->pRecData);
	if (pRecords) 
	{
		int i = 0;
		me->m_SMSList.count = pRecords->nRecordCount;
		me->m_SMSList.pSmsInfo = (ISMSInfo *)MALLOC(me->m_SMSList.count*sizeof(ISMSInfo));
		
		for (i = 0; i<pRecords->nRecordCount;i++)  
		{
			int j = 0;
			for (j=0;j<pRecords->nFieldCount;j++) 
			{
				if (STRCMP(pRecords->pItems[i].ppNam[j] ,"no") == 0) 
				{
					STRCPY(me->m_SMSList.pSmsInfo[i].sCALLNO,pRecords->pItems[i].ppVal[j]); 
				}
				else if (STRCMP(pRecords->pItems[i].ppNam[j],"ct") == 0) 
				{
					int nContentLen = STRLEN(pRecords->pItems[i].ppVal[j]);
					me->m_SMSList.pSmsInfo[i].nContentLen = (nContentLen+1)*sizeof(AECHAR);
					me->m_SMSList.pSmsInfo[i].aeContent = (AECHAR *)MALLOC(me->m_SMSList.pSmsInfo[i].nContentLen);
					UTF8_TO_WSTR((byte *)pRecords->pItems[i].ppVal[j],nContentLen,me->m_SMSList.pSmsInfo[i].aeContent,me->m_SMSList.pSmsInfo[i].nContentLen);
					// Change by zjie  2014-05-04 应该有误  me->m_SMSList.pSmsInfo->nContentLen = WSTRLEN(me->m_SMSList.pSmsInfo[i].aeContent);
					me->m_SMSList.pSmsInfo[i].nContentLen = WSTRLEN(me->m_SMSList.pSmsInfo[i].aeContent);
				}
				
				FREEIF(pRecords->pItems[i].ppNam[j]);
				FREEIF(pRecords->pItems[i].ppVal[j]);
			}
		}
		FREEIF(pRecords->pItems)
			FREEIF(pRecords);
	}
	
	FREEIF(me->pRecData);
}

/* ======================================================
解析类似短信信息的数据包结构
传入参数：
//	IShell *pShell ： 为写日志用，完善后可不要
//	uint16 *pLogIdx   :  为写日志用，完善后可不要	
	char *sBuf : 数据包
	static IRecords *gm_SMSformat(char *sBuf,IShell *pShell, uint16 *pLogIdx)
传出参数： 解析后的记录
======================================================== */ 
static ISmsRecords *gm_SMSformat(char *sBuf)
{
	ISmsRecords *pRecords = NULL;
	uint16  nRecCount = 0;						// 记录条数
	uint16  nFldCount = 0;						// 字段条数
	uint16  nContentLen=0;
	int nLoc = 8;	// 配置项的内容是从第8位开始
	int i = 0;
	
	nRecCount = sBuf[4]<<8 | sBuf[5];		
	nFldCount = sBuf[6]<<8 | sBuf[7];
	if (nRecCount == 0) goto ExitHere;
	if (nFldCount == 0) goto ExitHere;

	pRecords = (ISmsRecords *)MALLOC(sizeof(ISmsRecords));
	pRecords->nRecordCount = nRecCount;
	pRecords->nFieldCount = nFldCount;
	pRecords->pItems = (IItems *)MALLOC(nRecCount*sizeof(IItems));

	for (i = 0; i<nRecCount;i++)
	{
		int j = 0;
		pRecords->pItems[i].ppNam = (char **)MALLOC(nFldCount*sizeof(long));
		pRecords->pItems[i].ppVal = (char **)MALLOC(nFldCount*sizeof(long));
		
		for (j = 0; j< nFldCount; j++)
		{
			nContentLen = sBuf[nLoc+ITEM_NAME_LEN]<<8|sBuf[nLoc+ITEM_NAME_LEN+1];
			pRecords->pItems[i].ppNam[j] = (char *)MALLOC((ITEM_NAME_LEN+1)*sizeof(char));
			pRecords->pItems[i].ppVal[j] = (char *)MALLOC((nContentLen+1)*sizeof(char));
			MEMCPY(pRecords->pItems[i].ppNam[j],sBuf+nLoc,ITEM_NAME_LEN);
			MEMCPY(pRecords->pItems[i].ppVal[j],sBuf+nLoc+ITEM_NAME_LEN+CONTENT_BYTE_LEN,nContentLen);
			
			nLoc += (ITEM_NAME_LEN+CONTENT_BYTE_LEN+nContentLen);						// 这里的nContentLen是前一配置项的内容长度
		}
	}
ExitHere:
	return pRecords;
}

// =======================================================
// 保存信息到短信收件箱
static void Sms_SaveSms(cSms * me)
{
	newgimis *pMe;
	JulianType pDate;				
	uint32 uErr = 0;
	AEESMSStorageType st;
	uint32 nIndex =0;
	SMSMsgOpt apwoz[10];
	//	AECHAR pszBuf[100];
	int idx = me->m_SMSList.nCurId;
	
	pMe = me->m_pParent;
	
	if ( (me->pISmsMsg == NULL) || (me->pISmsStorage == NULL) ) 
	{
		Sms_ReleaseBuf(me);
		me->nState = 0;
		return;
	}
	
	GETJULIANDATE(0,&pDate);		//	取出日期	
	me->nState = 6;
	
	{
		int i = 0;
		//		me->m_SMSList.nCurId = idx;
		
		apwoz[i].nId  = MSGOPT_TO_DEVICE_SZ ;
		apwoz[i].pVal = (void *)me->m_SMSList.pSmsInfo[idx].sCALLNO; 		//DESTINATION_NUMBER;
		i++;
		
		/* unicode text to be send */
		apwoz[i].nId  = MSGOPT_PAYLOAD_WSZ ;
		apwoz[i].pVal = (void *)me->m_SMSList.pSmsInfo[idx].aeContent;
		i++;
		
		/* encoding */
		apwoz[i].nId  = MSGOPT_PAYLOAD_ENCODING;
		apwoz[i].pVal = (void *)AEE_ENC_UNICODE ;
		i++;
		
		apwoz[i].nId  = MSGOPT_MOSMS_ENCODING;
		apwoz[i].pVal = (void *)AEESMS_ENC_UNICODE;
		i++;
		
		apwoz[i].nId =  MSGOPT_LOOPBACK_MSG;				// 收件箱
		apwoz[i].pVal = (void *)TRUE;
		i++;
		
		apwoz[i].nId = MSGOPT_TAG;							// 未读
		apwoz[i].pVal = (void *)AEESMS_TAG_MT_NOT_READ;
		i++;
		
		apwoz[i].nId = MSGOPT_TIMESTAMP;					// 写入时间
		apwoz[i].pVal = (void *)&pDate;
		i++;
		
		apwoz[i].nId = MSGOPT_END;
		ISMSMSG_AddOpt(me->pISmsMsg,apwoz);
		
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"===============================================================");
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISMSMSG_AddOpt ok,sCALLNO=%s",me->m_SMSList.pSmsInfo[idx].sCALLNO);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISMSMSG_AddOpt ok,nContentLen=%d",me->m_SMSList.pSmsInfo[idx].nContentLen);
		
	}	
	
	st = AEESMS_NV_CDMA;
	CALLBACK_Init(&me->cbMsg, SMS_StoreMsgCb, me);
	
	ISMSSTORAGE_StoreMsg(me->pISmsStorage,st,me->pISmsMsg,&me->cbMsg,&nIndex,&uErr);
	//ISMSSTORAGE2_StoreMsg(me->pISmsStorage,st,me->pISmsMsg,&me->cbMsg,&nIndex,&uErr);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISMSSTORAGE_StoreMsg return=%d,index=%d",uErr,nIndex);

		
	return;
}

// 保存短信的回调函数
static void SMS_StoreMsgCb(cSms * me)
{
	newgimis *pMe;
	pMe = me->m_pParent;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SMS_StoreMsgCb in,nCurId=%d,count=%d",me->m_SMSList.nCurId,me->m_SMSList.count);

	me->m_SMSList.nCurId++;
	if ( me->m_SMSList.nCurId<me->m_SMSList.count )
	{
		ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,EVT_SAVE_SMS,0,0);
	}
	else
	{
		int nRecCounts = me->m_SMSList.count;
		Sms_ReleaseBuf(me);
		me->nState = 0;
		if ( nRecCounts == MAX_REC_COUNT )  
			ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,SMS_EVT_DOWNLOAD,0,0);
		else
		{
			Sms_FinishAlert(pMe);
// 			Sms_CloseSMS(me);
// 			CALLBACK_Cancel(&me->cbMsg);
		}
			
	}
	
	return;
}


/********************************************************************************************
// 退出前的动作
********************************************************************************************/
static void Sms_Close(cSms *me)
{
	newgimis* pMe;
	pMe = (newgimis *)me->m_pParent;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_Close in");
	
	Sms_ReleaseBuf(me);
	Sms_CloseWeb(me);
	Sms_CloseSMS(me);
	
	if(me->pWeb)	
	{
		int i,n;					// 2.6.4D2
		n = 0;
		i=IWEB_Release(me->pWeb);
		if (i>0) 
		{
			n++;
			i=IWEB_Release(me->pWeb);
		}
		
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_Close, IWEB_Release=%d,n=%d",i,n);
		me->pWeb = 0;
	}
	
	CALLBACK_Cancel(&me->cbMsg);
	
	return;
}


// 关闭短信接口
static void Sms_CloseSMS(cSms *me)
{
	int nErr = SUCCESS;
	newgimis *pMe;
	pMe = me->m_pParent;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_CloseSMS in");
	
	if (me->pISmsStorage != NULL) 
	{
		ISMSSTORAGE_Release(me->pISmsStorage);
		me->pISmsStorage = NULL;

		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pISmsStorage in");
	}
	
	if (me->pISmsMsg != NULL) 
	{
		ISMSMSG_Release(me->pISmsMsg);
		me->pISmsMsg = NULL;

		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pISmsMsg in");
	}

	if (me->pISMS != NULL)
	{
		ISMS_Release(me->pISMS);
		me->pISMS = NULL;
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pISMS in");
	}
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_CloseSMS out");
	return ;
	
}