#include "newgimis.bid"
#include "newgimis.h"

#include "gm_sms.h"
#include "gmTools.h"
/*===========================================================================
// ���ö��Ž��ս���������
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
			int iStatus = wParam; 		// 1 : �����յ������ݣ���������б���
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
// ��ʼ��
// �������
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

// ʵ�������Žӿ�
// ����ֵ��
//		�ɹ� ��SUCCESS, ʧ�� EFAILED
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


// ������Ϣ
static void Sms_DownLoad(cSms* me)
{
	int idx;
	newgimis *pMe;
	WebOpt zWebOpts[7];
	char sFormat[250] = {0};
	
	pMe = me->m_pParent;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd) ,"Sms_DownLoad in");
	
	// ����http��ַ zc.gota.richmap.cn/brewGPS/BrewServlet?act=httpdec; ���  zc.gota.richmap.cn/message/getMessage?ver=1.0
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

	me->nState = 1;		// ���ڽ�������
	
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
//     AECHAR szFailure[] = {0x4E2D,0x5FC3,0x65E0,0x5E94,0x7B54,0x0000};	// ������Ӧ��
	
	SPRINTF(sLogBuf,"Sms_MonitorSendFile in,Overtime=%d",me->Overtime);
	uf_RecLog(me->m_pParent,sLogBuf);
	
	if ( me->Overtime == 0)
	{
		//uf_RecLog(me->m_pParent,"û�г�ʱû�г�ʱ");
		return;
	}


	Sms_CloseWeb(me);	//��ע ���ǻ���û������û���ͷ�
	
	
	// �������60��û�յ�������֣������ϵ�
// 	WSTRCPY(szText,szFailure);
// 	
// 	
// 	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
// 	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
// 		8, 2,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
// 	IDISPLAY_Update(me->a.m_pIDisplay);
	
// 	// ����ʧ�ܣ�������ʾ
// 	ISHELL_Beep(me->a.m_pIShell, BEEP_MSG, TRUE);
// 	ISHELL_Beep(me->a.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
// 	ISHELL_SetTimer(me->a.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*)me->m_pParent );
// 	
// 	ISHELL_SetTimer(me->a.m_pIShell, 55*100, (PFNNOTIFY)Record_Afterpost, me->m_pParent);
}


/*********************** Ӧ��http���� *******************************************************/
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
		
		me->Overtime = 0;				// ���յ�Ӧ��

		me->nBytes = pInfo->lContentLength;
		me->pSource = pInfo->pisMessage;
		ISOURCE_AddRef(me->pSource);				// ��Read Body���õ�
		IWEBRESP_Release(me->pWebResp);
		me->pWebResp = 0;
		
		CALLBACK_Cancel(&me->webcb);
		CALLBACK_Init(&me->webcb, Sms_ReadBody, me);
		Sms_ReadBody(me);
	}
}



// ==========================================================
// ��ȡ���Ķ˷��ص���Ϣ
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
	
	if (!ISOURCE_Exhausted(nRead))			// ��������Ƿ��Ѿ�ȫ����ȡ
	{
		ISOURCE_Readable(me->pSource, &me->webcb);
		return;
	}
	
	Sms_CloseWeb(me);
	
	nRecCount = *(me->pRecData+4)<<8 | *(me->pRecData+5);		
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Sms_ReadBody in, item count = %d",nRecCount);

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),me->pRecData);//cs 
	
	if ( nRecCount == 0)  
	{		// û������
		me->nState = 0;
		Sms_ReleaseBuf(me);
		Sms_FinishAlert(pMe);

// 		//����ͷš��ڶ�������
// 		Sms_CloseSMS(me);
// 		CALLBACK_Cancel(&me->cbMsg);
	}
	else
		ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,EVT_SAVE_SMS,1,0);	
	
	return;
}


// �ر�http�ӿ�
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

// �ͷ�m_SMSList����Դ
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

// �����յ������ݣ���������б���
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
					// Change by zjie  2014-05-04 Ӧ������  me->m_SMSList.pSmsInfo->nContentLen = WSTRLEN(me->m_SMSList.pSmsInfo[i].aeContent);
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
�������ƶ�����Ϣ�����ݰ��ṹ
���������
//	IShell *pShell �� Ϊд��־�ã����ƺ�ɲ�Ҫ
//	uint16 *pLogIdx   :  Ϊд��־�ã����ƺ�ɲ�Ҫ	
	char *sBuf : ���ݰ�
	static IRecords *gm_SMSformat(char *sBuf,IShell *pShell, uint16 *pLogIdx)
���������� ������ļ�¼
======================================================== */ 
static ISmsRecords *gm_SMSformat(char *sBuf)
{
	ISmsRecords *pRecords = NULL;
	uint16  nRecCount = 0;						// ��¼����
	uint16  nFldCount = 0;						// �ֶ�����
	uint16  nContentLen=0;
	int nLoc = 8;	// ������������Ǵӵ�8λ��ʼ
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
			
			nLoc += (ITEM_NAME_LEN+CONTENT_BYTE_LEN+nContentLen);						// �����nContentLen��ǰһ����������ݳ���
		}
	}
ExitHere:
	return pRecords;
}

// =======================================================
// ������Ϣ�������ռ���
static void Sms_SaveSms(cSms * me)
{
	newgimis *pMe;
	JulianType pDate;				
	uint32 uErr = 0;
	AEESMSStorageType st;
	uint32 nIndex =0;
	SMSMsgOpt apwoz[10];

	AECHAR	*aeContent;//lSMS
	int aeContent_len = 0;//lSMS
	char titleBuf[256];//������

	int idx = me->m_SMSList.nCurId;
	
	pMe = me->m_pParent;
	
	if ( (me->pISmsMsg == NULL) || (me->pISmsStorage == NULL) ) 
	{
		Sms_ReleaseBuf(me);
		me->nState = 0;
		return;
	}
	
	GETJULIANDATE(0,&pDate);		//	ȡ������	
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
		
		apwoz[i].nId =  MSGOPT_LOOPBACK_MSG;				// �ռ���
		apwoz[i].pVal = (void *)TRUE;
		i++;
		
		apwoz[i].nId = MSGOPT_TAG;							// δ��
		apwoz[i].pVal = (void *)AEESMS_TAG_MT_NOT_READ;
		i++;
		
		apwoz[i].nId = MSGOPT_TIMESTAMP;					// д��ʱ��
		apwoz[i].pVal = (void *)&pDate;
		i++;
		
		apwoz[i].nId = MSGOPT_END;
		ISMSMSG_AddOpt(me->pISmsMsg,apwoz);
		
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"===============================================================");
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISMSMSG_AddOpt ok,sCALLNO=%s",me->m_SMSList.pSmsInfo[idx].sCALLNO);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISMSMSG_AddOpt ok,nContentLen=%d",me->m_SMSList.pSmsInfo[idx].nContentLen);
		
	}	

	//˳�㱣�����ļ��С�����֪���ļ����м����ļ�
	//�����ļ������� ���ļ�����1
	//����tm
	//------------------
	
	
	//MEMSET(titleBuf,0x00,sizeof(titleBuf));
	//SPRINTF(titleBuf,"%010d* %04d-%02d-%02d %02d:%02d:%02d",GETTIMEMS(),pDate.wYear,pDate.wMonth,pDate.wDay,pDate.wHour,pDate.wMinute,pDate.wSecond);
	//aeContent = me->m_SMSList.pSmsInfo[idx].aeContent;
	//aeContent_len = me->m_SMSList.pSmsInfo[idx].nContentLen;

	//MEMSET(pMe->titleBuf_LongSMS,0x00,sizeof(pMe->titleBuf_LongSMS));
	//MEMCPY(pMe->titleBuf_LongSMS,titleBuf,10);

	//write_LongSMS_Data(pMe,me->m_SMSList.pSmsInfo[idx].aeContent,me->m_SMSList.pSmsInfo[idx].nContentLen,titleBuf);
	//------------------
	
	SMS_StoreMsgCb(me);
	st = AEESMS_NV_CDMA;
	CALLBACK_Init(&me->cbMsg, SMS_StoreMsgCb, me);
	
	ISMSSTORAGE_StoreMsg(me->pISmsStorage,st,me->pISmsMsg,&me->cbMsg,&nIndex,&uErr);
	//ISMSSTORAGE2_StoreMsg(me->pISmsStorage,st,me->pISmsMsg,&me->cbMsg,&nIndex,&uErr);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISMSSTORAGE_StoreMsg return=%d,index=%d",uErr,nIndex);

		
	return;
}

// ������ŵĻص�����
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

// 			if (!pMe->m_FOBFlag)
// 			{
// 				if (pMe->m_WitchPag ==11)	//���ռ�������ˢ��
// 				{
// 					//�����ռ���
// 					//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SMS_StoreMsgCb �¶����� ˢ�½���");
// 					testgps_GotoScreen( pMe, SCREENID_LONGSMS, 0 );
// 				}
// 			}
// 			else
// 			{
// 				if (pMe->m_WitchPag != 13)		//�Ѿ����¶��Ž���Ͳ���Ҫ��ˢ��
// 				{
// 					pMe->m_bDoLongSMS = 1;										//��־λ
// 					ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
// 				}
// 			}
		}	
	}
	return;
}


/********************************************************************************************
// �˳�ǰ�Ķ���
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


// �رն��Žӿ�
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

/************************************************************************/
/* ��ȡ��Ϣ���� - pMe->m_LongSMS_Num
   ��ȡ��Ϣ�Ѷ���Ŀ - pMe->m_LongSMS_uReadNum
   ��ȡ���һ�����¶�����ϵͳ�������λ�� pMe->m_select_num | ���ں�̨������ʾ�¶���ʱ�򣬵õ�ѡ����                                                                      */
/************************************************************************/
void load_LongSMS_Num(newgimis* pMe)
{
	FileInfo* pFileInfo = NULL;	
	IFileMgr* pFileMgr = NULL;
	int i = 0;

	pMe->m_LongSMS_uReadNum = 0;
	pMe->m_LongSMS_Num = 0;

	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"load_LongSMS_Num in, ISHELL_CreateInstance err");
		return;
	}
	
	if ( IFILEMGR_EnumInit( pFileMgr, AEEFS_SMSDATA_DIR, FALSE ) )  
	{
		int iAccess;
		iAccess = IFILEMGR_CheckPathAccess(pFileMgr,AEEFS_SMSDATA_DIR,AEEFP_READ,NULL);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"load_LongSMS_Num IFILEMGR_CheckPathAccess iAccess = %d",iAccess);
	}
	
	pFileInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
	MEMSET(pFileInfo, 0x00, sizeof(FileInfo) );
	if( pFileInfo )   
	{	
		//����Ŀ¼���ļ�
		while( IFILEMGR_EnumNext( pFileMgr, pFileInfo ) )
		{  // while -- 1
			//�ļ�����
			pMe->m_LongSMS_Num = pMe->m_LongSMS_Num  +1;

			//δ���ļ�����
			if (STRSTR(pFileInfo->szName,"*") != NULL)
			{
				pMe->m_LongSMS_uReadNum = pMe->m_LongSMS_uReadNum + 1;
			}

			if (NULL != STRSTR(pFileInfo->szName,pMe->titleBuf_LongSMS) )
			{
				pMe->m_select_num = i;
			}
			
			MEMSET(pFileInfo, 0x00, sizeof(FileInfo) );

			i++;
		}  // while -- 1
		
		FREEIF( pFileInfo );	
	}	// if -- pFileInfo	
	
	if (pFileMgr != NULL)
	{
		IFILEMGR_Release( pFileMgr );
		pFileMgr = NULL;
	}
	
	return;
}

/************************************************************************/
/* ��ȡ�洢 ���ж��� ��Ϣ������ pMe->s_SaveLongSMSName                                                                     */
/************************************************************************/
void load_LongSMS_Data(newgimis* pMe)
{
	FileInfo* pFileInfo = NULL;	
	IFileMgr* pFileMgr = NULL;
	IFile	*pTempFile = NULL;

	int i = 0;
	int idx = 0;

	pMe->s_SaveLongSMSName = (SaveLongSMSName *)MALLOC((pMe->m_LongSMS_Num + 1) * sizeof(SaveLongSMSName));
	MEMSET(pMe->s_SaveLongSMSName,0x00,(pMe->m_LongSMS_Num + 1) * sizeof(SaveLongSMSName));
	
	//����Ŀ¼ �����ļ��������� �ռ����б�����
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"load_LongSMS_Data in, ISHELL_CreateInstance err");
		return;
	}
	
	if ( IFILEMGR_EnumInit( pFileMgr, AEEFS_SMSDATA_DIR, FALSE ) )  
	{
		int iAccess;
		iAccess = IFILEMGR_CheckPathAccess(pFileMgr,AEEFS_SMSDATA_DIR,AEEFP_READ,NULL);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"load_LongSMS_Data IFILEMGR_CheckPathAccess iAccess = %d",iAccess);
	}
	
	pFileInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
	MEMSET(pFileInfo, 0x00, sizeof(FileInfo) );
	if( pFileInfo )   
	{	
		//����Ŀ¼���ļ�
		while( IFILEMGR_EnumNext( pFileMgr, pFileInfo ) )
		{  // while -- 1
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_LongSMS_HandleEvent pFileInfo->szName = %s",pFileInfo->szName);
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_LongSMS_HandleEvent pFileInfo->dwSize = %d",pFileInfo->dwSize)
			STRCPY(pMe->s_SaveLongSMSName[i].FileName, pFileInfo->szName);
			idx = ud_rDistToChar(pFileInfo->szName,'/');
			
			if (STRSTR(pFileInfo->szName,"*") != NULL)
			{
				//δ��
				pMe->s_SaveLongSMSName[i].ihasRead = 0;
				STRCPY(pMe->s_SaveLongSMSName[i].LongSMSName, pFileInfo->szName + idx + 1 + 11);//10λ����� + *����
			}
			else
			{
				//�Ѷ�
				pMe->s_SaveLongSMSName[i].ihasRead = 1;	
				STRCPY(pMe->s_SaveLongSMSName[i].LongSMSName, pFileInfo->szName + idx + 1 + 10);//10λ����� + *����
			}
			
			i++;
			
			MEMSET(pFileInfo, 0x00, sizeof(FileInfo) );
		}  // while -- 1
		
		FREEIF( pFileInfo );	
	}	// if -- pFileInfo	
	
	if (pFileMgr != NULL)
	{
		IFILEMGR_Release( pFileMgr );
		pFileMgr = NULL;
	}

	return;
}

/************************************************************************/
/* ��������Ϣд�뵽�ļ���                                                                     */
/************************************************************************/
void write_LongSMS_Data(newgimis *pMe,AECHAR* aeContent,int aeContent_len,char *titleBuf)
{
	IFileMgr		*pFileMgr = NULL;
	IFile			*pTempFile = NULL;
	char file_name[50] = {0};
	
	JulianType dDate;												
	GETJULIANDATE(0,&dDate);
	
	SPRINTF(file_name,AEEFS_SMSDATA_DIR"%s",titleBuf);
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"write_LongSMS_Data file_name is %s",file_name);
	
//	pMe->mux_7Day = 1;
	
	// �����ļ��ӿ�
	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
	{
		if (IFILEMGR_Test(pFileMgr, file_name) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, file_name, _OFM_READWRITE);
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, file_name, _OFM_CREATE);
		}
	}
	else
	{
		return;
	}

	
	if (pTempFile != NULL)
	{
		int si_Count = 0;
		FileInfo fiInfo;
		
	//	if ( IFILE_GetInfo( pTempFile, &fiInfo ) == SUCCESS) 
	//	{
		IFILE_Truncate(pTempFile, 0);
	//	}
			
		IFILE_Seek(pTempFile,_SEEK_END,0);
		si_Count = IFILE_Write(pTempFile, aeContent, aeContent_len); //д���һ������
	
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"write_LongSMS_Data IFILE_Write data is %d",si_Count);	
	}
	   // Free the IFileMgr and IFile instances
	if (pTempFile != NULL)
		IFILE_Release( pTempFile );
	
	if (pFileMgr != NULL)
		IFILEMGR_Release( pFileMgr );
	
//	pMe->mux_7Day = 0;
	
	return ;
}




/***************************************************************************
SamplePosDet_SendAudio_HandleEvent �ռ������
****************************************************************************/
boolean SamplePosDet_LongSMS_HandleEvent( newgimis *pMe, AEEEvent eCode,
										 uint16 wParam, uint32 dwParam )
{
	AEERect rRect;
	
	int  m_nFontHeight = 0;
	AECHAR c_Tmp[64] = {0};

	char str_title[64] = {0};
	AECHAR aes_tmp[64] = {0};

	int i = 0;
	//int j = 0;
	
	boolean bHandled = FALSE;
	IMenuCtl *pMenuCtl = testgps_GetScreenData_LongSMS( pMe );
	IMenuCtl *pMenuCtl_N = testgps_GetScreenData( pMe );
	
	switch( eCode ) {
	case EVT_SCREEN:
		if( wParam == SCREEN_PARAM_INIT ) {
			if( pMenuCtl_N ) {  /* This is not expected to happen */
				testgps_DrawScreen( pMe, 0 );
				bHandled = TRUE;
			}
			else {
				if( (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_MENUCTL, (void **)&pMenuCtl_N ) == SUCCESS)	&&
					(ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&pMenuCtl ) == SUCCESS) ) {
					IMENUCTL_SetProperties( pMenuCtl_N, MP_WRAPSCROLL);
					m_nFontHeight = IDISPLAY_GetFontMetrics( pMe->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL );
					
					load_LongSMS_Num(pMe);
					load_LongSMS_Data(pMe);

					MEMSET(str_title,0x0,64);
					SPRINTF(str_title,"SMS InBox ( %d / %d )",	pMe->m_LongSMS_uReadNum,pMe->m_LongSMS_Num);
					STRTOWSTR(str_title,aes_tmp,sizeof(aes_tmp));
					IMENUCTL_SetTitle( pMenuCtl_N, NULL, NULL, aes_tmp );

					for (i; i < pMe->m_LongSMS_Num; i++)
					{
						MEMSET(c_Tmp,0x00,64);
						STR_TO_WSTR(pMe->s_SaveLongSMSName[i].LongSMSName,c_Tmp,64);
					
						if (0 == pMe->s_SaveLongSMSName[i].ihasRead){
							MP_AddMenuItemEx(pMenuCtl_N,NULL, c_Tmp, IDB_HAVE_TASK, i, (uint32)pMenuCtl_N);
						}
						else{
							IMENUCTL_AddItem( pMenuCtl_N, NULL, NULL, i, c_Tmp, (uint32)pMenuCtl_N );
						}
					}

					SETAEERECT(&rRect, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen -  pMe->m_SYSFNH-10);
					
					IMENUCTL_SetRect( pMenuCtl_N, &rRect );
					
					IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_OPEN,0, NULL, (uint32)pMenuCtl );
					
					SETAEERECT(&rRect, 0, pMe->DeviceInfo.cyScreen -  pMe->m_SYSFNH-10, pMe->DeviceInfo.cxScreen,  pMe->m_SYSFNH+10);
					
					IMENUCTL_SetRect( pMenuCtl, &rRect );
					
					IMENUCTL_SetActive( pMenuCtl_N, TRUE );
					IMENUCTL_SetActive( pMenuCtl, TRUE );
					
					{
						AEEMenuSortType t;
						t = MCS_NAME_DESCENDING;
						IMENUCTL_Sort(pMenuCtl_N,t);
					}

					{	// ��������	
						AEEMenuColors stColors;
						stColors.wMask = CLR_SYS_ITEM|CLR_SYS_ITEM_TEXT|CLR_SYS_ITEM_SEL|CLR_SYS_ITEM_SEL_TEXT;
						stColors.cBack = MAKE_RGB(199,230,248);
						stColors.cText = MAKE_RGB(9,90,136);
						stColors.cSelBack = MAKE_RGB(25,137,199);
						stColors.cSelText = MAKE_RGB(255,255,255);
						IMENUCTL_SetColors(pMenuCtl_N,&stColors);			
					}  // ��������

					
					{	// ��������	//
						AEEMenuColors stColors;
						stColors.wMask = CLR_SYS_ITEM|CLR_SYS_ITEM_TEXT|CLR_SYS_ITEM_SEL|CLR_SYS_ITEM_SEL_TEXT;
						stColors.cBack = MAKE_RGB(199,230,248);
						stColors.cText = MAKE_RGB(9,90,136);
						stColors.cSelBack = MAKE_RGB(25,137,199);
						stColors.cSelText = MAKE_RGB(255,255,255);
						IMENUCTL_SetColors(pMenuCtl,&stColors);			
					}  // ��������
				
					
					IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
					IDISPLAY_Update (pMe->a.m_pIDisplay);
					testgps_SetScreenData( pMe, (void *)pMenuCtl_N );
					testgps_SetScreenData_LongSMS( pMe, (void *)pMenuCtl );
					testgps_DrawScreen( pMe, 0 );
					bHandled = TRUE;
				}
			}
		}
		else if( wParam == SCREEN_PARAM_CLOSE ) {
			if (pMenuCtl)
			{
				IMENUCTL_Release( pMenuCtl );
			}
			if (pMenuCtl_N)
			{
				IMENUCTL_Release( pMenuCtl_N );
			}
			
			if (pMe->m_WitchPag != 12)	//��Ϣ���鲻���ͷ� //�Ƿ�ɾ��ȫ������ҳ�治���ͷţ����ǵ��������ǾɵĶ�����Ŀ�����ᵼ�º���;�¼���Ķ��Ÿ��ţ�
			{
				FREEIF(pMe->s_SaveLongSMSName);
			}
			
			
			testgps_SetScreenData( pMe, 0 );
			testgps_SetScreenData_LongSMS( pMe, 0 );
			pMe->currentHandler = 0;
			
			pMenuCtl = NULL;
			bHandled = TRUE;
		}
		else if( wParam == SCREEN_PARAM_PAINT ) {

			IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);

			IMENUCTL_Redraw( pMenuCtl_N );	
			IMENUCTL_Redraw( pMenuCtl );
			IMENUCTL_SetActive( pMenuCtl_N, TRUE );
			IMENUCTL_SetActive( pMenuCtl, TRUE );
			bHandled = TRUE;
		}
		break;
		
	case EVT_KEY:
		{
			bHandled = IMENUCTL_HandleEvent( pMenuCtl_N, eCode, wParam, dwParam );		   
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "�����䴥�� EVT_KEY_RELEASE");
			
			if (wParam == AVK_SOFT2)	
			{
				if ( pMe->m_Right_Key == '1' )
				{
					if (pMenuCtl)
					{
						IMENUCTL_Release( pMenuCtl );
					}
					if (pMenuCtl_N)
					{
						IMENUCTL_Release( pMenuCtl_N );
					}
					
					pMenuCtl = NULL;
					pMenuCtl_N = NULL;
					
					
					pMe->currentHandler = 0;
					testgps_SetScreenData( pMe, 0 );
					
					//	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
					pMe->m_RelyExit = FALSE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
					ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
					pMe->m_FOBFlag = TRUE; //�ں�̨����
					pMe->m_WitchPag = 0;
					//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"��̨���� 6:%d",pMe->m_WitchPag);
					bHandled = TRUE;
					return TRUE;
				}
			}
			//////////////////////////////////////
			if (wParam == AVK_END)
			{
				if (STRCMP(pMe->sMobileType,"55") == 0 || (STRCMP(pMe->sMobileType,"03") == 0) || (STRCMP(pMe->sMobileType,"58") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0) || (STRCMP(pMe->sMobileType,"57") == 0) )
				{
					// ����ֱ�ӻص���̨
					if (pMenuCtl)
					{
						IMENUCTL_Release( pMenuCtl );
					}
					if (pMenuCtl_N)
					{
						IMENUCTL_Release( pMenuCtl_N );
					}
					
					pMenuCtl = NULL;
					pMenuCtl_N = NULL;
					
					pMe->currentHandler = 0;
					testgps_SetScreenData( pMe, 0 );
					
					//	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
					pMe->m_RelyExit = FALSE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
					ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
					pMe->m_FOBFlag = TRUE; //�ں�̨����
					pMe->m_WitchPag = 0;
					//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"��̨���� 6:%d",pMe->m_WitchPag);
					bHandled = TRUE;
					return TRUE;
				}
			}
			
			if( wParam == AVK_END  || wParam == AVK_CLR ) 
			{
				pMe->m_WitchPag = 10; // ����������
				testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
				//		   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
				//		   pMe->m_FOBFlag = TRUE; //�ں�̨����
				pMe->m_WitchPag = 10;
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"���������� 11:%d",pMe->m_WitchPag);
				bHandled = TRUE;
			}
		}
		
		break;
		
	case EVT_COMMAND:
		if( dwParam == (uint32)pMenuCtl_N ) {

			pMe->m_select_num = wParam;
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_LongSMS_HandleEvent pMe->m_select_num = %d",pMe->m_select_num);

			//����� �ı����� ȥ��#����Ѷ�
			if (pMe->s_SaveLongSMSName[pMe->m_select_num].ihasRead == 0)
			{
				LongSMS_hasread(pMe);

				pMe->m_LongSMS_uReadNum = pMe->m_LongSMS_uReadNum - 1;//Ϊ�˿���ʱ�򣬱�ֱ�Ӱ�����������ˣ��ڽ�������ʱ����ʵ�Ѷ��Ѿ�û���ˡ�
			}

			
			IMENUCTL_SetActive( pMenuCtl_N, FALSE );
			pMe->m_WitchPag = 12; // ��Ϣ����
			testgps_GotoScreen( pMe, SCREENID_LONGSMS_CONTENT, 0 );
			return TRUE;
		}
		break;
   }
   return bHandled;
}




/************************************************************************/
/* ��Ϣ���鴰��                                                                     */
/************************************************************************/
boolean SamplePosDet_LongSMS_Content_HandleEvent( newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam )
{
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );
	
	boolean bHandled = FALSE;

	AEERect rRect;

	int ret;
	char  StrFileName[128];
	
	IFileMgr		*pFileMgr;
	IFile			*pTempFile;
	FileInfo fiInfo;		// �ļ���Ϣ

	switch( eCode ) 
	{
	case EVT_SCREEN:
		if( wParam == SCREEN_PARAM_INIT ) 
		{
			if(pMenuCtl) 
			{
				testgps_DrawScreen( pMe, 0 );
				bHandled = TRUE;
			}
			else
			{
				int nErr = 0;
				//��̬�ı�       ///////////////////////////////////////
				nErr = ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_STATIC,(void**)(&pMe->m_pIStatic));
				if ( nErr != SUCCESS)   {
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_LongSMS_Content_HandleEvent m_pIStatic create err=%d  ##### ",nErr);
					return TRUE;
				}
				//���þ�̬�ı���
				rRect.x = 0;
				rRect.y = 0;
				rRect.dx = pMe->DeviceInfo.cxScreen;
				rRect.dy = pMe->DeviceInfo.cyScreen - pMe->iFontHeight-10;
				ISTATIC_SetRect(pMe->m_pIStatic, &rRect);

				//�ı�����
				ISTATIC_SetProperties(pMe->m_pIStatic, ST_CENTERTITLE);
				
				//MEMSET(pMe->pContentText,0x00,404);

				//�����ı�����
				{
					AECHAR* psTextBuf = NULL;
					uint32  bufSize = 0;
					uint32  nRtn;
					//��ǩ��"��������"
					ISHELL_GetResSize(pMe->a.m_pIShell,NEWGIMIS_RES_FILE, IDS_SMS_CONTENT,RESTYPE_STRING,&bufSize);
					psTextBuf = (AECHAR*)MALLOC((bufSize+1)*sizeof(AECHAR));
					nRtn = ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE,IDS_SMS_CONTENT, psTextBuf,bufSize);
					
					if (nRtn >0 )
					{
						MEMSET(StrFileName,0,sizeof(StrFileName));
						SPRINTF(StrFileName,"%s",pMe->s_SaveLongSMSName[pMe->m_select_num].FileName);

						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_LongSMS_Content_HandleEvent StrFileName= %s",StrFileName);
						
						// �����ļ��ӿ�
						if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
						{
							if (IFILEMGR_Test(pFileMgr, StrFileName) == SUCCESS)
								pTempFile = IFILEMGR_OpenFile(pFileMgr, StrFileName, _OFM_READ);
						}
						else
							return;
						
						if (pTempFile != NULL)
						{
							if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) {
								// Change by ---- 2013-01-01			if ( fiInfo.dwSize > 1024*500 ) {
								if ( (fiInfo.dwSize > 0) ) {
									IFILE_Seek(pTempFile,_SEEK_START,0);

									//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_LongSMS_Content_HandleEvent fiInfo.dwSize = %d",fiInfo.dwSize);
									
									pMe->aeContent_LongSMS = (AECHAR*)MALLOC(fiInfo.dwSize + 1);
									MEMSET(pMe->aeContent_LongSMS, 0x00, fiInfo.dwSize + 1);
									ret = IFILE_Read(pTempFile,pMe->aeContent_LongSMS,fiInfo.dwSize);

									
									ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_LongSMS_Content_HandleEvent IFILE_Read ret= %d",ret);
								}
							}
						}
						
						if (pTempFile != NULL)
							IFILE_Release( pTempFile );
						
						if (pFileMgr != NULL)
							IFILEMGR_Release( pFileMgr );

						nErr = ISTATIC_SetText(pMe->m_pIStatic, psTextBuf, pMe->aeContent_LongSMS, AEE_FONT_BOLD,AEE_FONT_BOLD);
					}

					FREE(psTextBuf);
					FREE(pMe->aeContent_LongSMS);
				}

				//�ײ���soft�˵��ؼ�   ����///////////////////////////////////////////////////////////////////////////		
				nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void **) (&(pMenuCtl)));
				if ( nErr != SUCCESS)   {
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_LongSMS_Content_HandleEvent soft menu create err=%d  ##### ",nErr);
					return TRUE;
				}

				SETAEERECT(&rRect, 0, pMe->DeviceInfo.cyScreen -  pMe->iFontHeight-10, pMe->DeviceInfo.cxScreen,  pMe->iFontHeight+10);
				IMENUCTL_SetRect( pMenuCtl, &rRect );

				// "����"
				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IMENU_RTN, 0, NULL, (uint32)pMenuCtl);
				//ɾ��
				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IMENU_DEL, 1, NULL, (uint32)pMenuCtl);

				{	// ��������
					AEEMenuColors stColors;
					stColors.wMask = CLR_SYS_ITEM|CLR_SYS_ITEM_TEXT|CLR_SYS_ITEM_SEL|CLR_SYS_ITEM_SEL_TEXT;
					stColors.cBack = MAKE_RGB(199,230,248);
					stColors.cText = MAKE_RGB(9,90,136);
					stColors.cSelBack = MAKE_RGB(25,137,199);
					stColors.cSelText = MAKE_RGB(255,255,255);
					IMENUCTL_SetColors(pMenuCtl,&stColors);			
				}  // ��������
				
				testgps_SetScreenData( pMe, (void *)pMenuCtl);
				testgps_DrawScreen( pMe, 0 );
				bHandled = TRUE;
			}
		}
		else if( wParam == SCREEN_PARAM_CLOSE ) 
		{
			ISTATIC_Reset(pMe->m_pIStatic);

			if (pMe->m_pIStatic != NULL)
			{
				ISTATIC_Release(pMe->m_pIStatic );
				pMe->m_pIStatic = NULL;
			}
			
			if( pMenuCtl != NULL)	
			{								
				if ( IMENUCTL_GetItemCount(pMenuCtl)>0 )	
				{
					IMENUCTL_DeleteAll(pMenuCtl);  
					IMENUCTL_Release(pMenuCtl);    
					pMenuCtl = NULL;	
				}
			}	
			
			testgps_SetScreenData( pMe, 0 );
			pMe->currentHandler = 0;
			bHandled = TRUE;

			FREEIF(pMe->s_SaveLongSMSName);
		}
		else if( wParam == SCREEN_PARAM_PAINT ) 
		{
			IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
			IDISPLAY_Update (pMe->a.m_pIDisplay);

			ISTATIC_Redraw( pMe->m_pIStatic );
			IMENUCTL_Redraw( pMenuCtl);

			IMENUCTL_SetActive(pMenuCtl,TRUE);

			ISTATIC_SetActive(pMe->m_pIStatic, TRUE);

		}
		break;
	case EVT_KEY:
		{
// 			if ( !IMENUCTL_IsActive(pMenuCtl) )
// 			{
// 				if (wParam == AVK_SELECT)
// 				{
// 					ISTATIC_SetActive(pMe->m_pIStatic, FALSE);
// 					IMENUCTL_SetActive(pMenuCtl, TRUE);
// 					return TRUE;
// 				}
// 			}

			
			if (wParam == AVK_SOFT2)	
			{
				if ( pMe->m_Right_Key == '1' )
				{
					ISTATIC_Reset(pMe->m_pIStatic);
					
					// ����ֱ�ӻص���̨
					if (pMe->m_pIStatic != NULL)
					{
						ISTATIC_Release(pMe->m_pIStatic );
						pMe->m_pIStatic = NULL;
					}
					
					if( pMenuCtl != NULL)	
					{								
						if ( IMENUCTL_GetItemCount(pMenuCtl)>0 )	
						{
							IMENUCTL_DeleteAll(pMenuCtl);  
							IMENUCTL_Release(pMenuCtl);    
							pMenuCtl = NULL;	
						}
					}	
					
					pMe->currentHandler = 0;
					testgps_SetScreenData( pMe, 0 );

					FREEIF(pMe->s_SaveLongSMSName);	//
					
					pMe->m_RelyExit = FALSE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
					ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
					pMe->m_FOBFlag = TRUE; //�ں�̨����
					pMe->m_WitchPag = 0;
					bHandled = TRUE;
					return TRUE;
				}
			}

			
			if (wParam == AVK_END)
			{
				if (STRCMP(pMe->sMobileType,"55") == 0 || (STRCMP(pMe->sMobileType,"03") == 0)  || (STRCMP(pMe->sMobileType,"58") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0) || (STRCMP(pMe->sMobileType,"57") == 0) )
				{
					ISTATIC_Reset(pMe->m_pIStatic);

					// ����ֱ�ӻص���̨
					if (pMe->m_pIStatic != NULL)
					{
						ISTATIC_Release(pMe->m_pIStatic );
						pMe->m_pIStatic = NULL;
					}
					
					if( pMenuCtl != NULL)	
					{								
						if ( IMENUCTL_GetItemCount(pMenuCtl)>0 )	
						{
							IMENUCTL_DeleteAll(pMenuCtl);  
							IMENUCTL_Release(pMenuCtl);    
							pMenuCtl = NULL;	
						}
					}	
			
					pMe->currentHandler = 0;
					testgps_SetScreenData( pMe, 0 );

					FREEIF(pMe->s_SaveLongSMSName);//
					
					pMe->m_RelyExit = FALSE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
					ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
					pMe->m_FOBFlag = TRUE; //�ں�̨����
					pMe->m_WitchPag = 0;
					bHandled = TRUE;
					return TRUE;
				}
			}
			
			
			if( wParam == AVK_END  || wParam == AVK_CLR ) 
			{	
				pMe->m_WitchPag = 11; // �����ռ���
				testgps_GotoScreen( pMe, SCREENID_LONGSMS, 0 );
				
				bHandled = TRUE;
			}
			
			
// 			if (IMENUCTL_IsActive(pMenuCtl))
// 			{
// 				if (wParam == AVK_DOWN || wParam == AVK_UP )
// 				{
// 					ISTATIC_SetActive(pMe->m_pIStatic, TRUE);
// 					IMENUCTL_SetActive(pMenuCtl, FALSE);
// 					return TRUE;
// 				}
// 				IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam);
// 				//return TRUE;
// 			}
// 			if (ISTATIC_IsActive(pMe->m_pIStatic))
// 			{
// 				ISTATIC_HandleEvent(pMe->m_pIStatic, eCode, wParam, dwParam);
// 				//return TRUE;
// 			}

			if (wParam == AVK_DOWN || wParam == AVK_UP )
			{
				ISTATIC_HandleEvent(pMe->m_pIStatic, eCode, wParam, dwParam);
				return TRUE;
			}

			if (wParam == AVK_LEFT || wParam == AVK_RIGHT || wParam == AVK_SELECT)
			{
				IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam);
				return TRUE;
			}

		}
	break;
	case EVT_COMMAND:
		{
			if( dwParam == (uint32)pMenuCtl) 
			{
				if (wParam == 0)
				{
					pMe->m_WitchPag = 11; // �����ռ���
					testgps_GotoScreen( pMe, SCREENID_LONGSMS, 0 );
					bHandled = TRUE;
				}

				if (wParam == 1)
				{
					//��ɾ���ļ�
					Delete_file(pMe,pMe->s_SaveLongSMSName[pMe->m_select_num].FileName);
					//�����ռ���
					pMe->m_WitchPag = 11; // �����ռ���
					testgps_GotoScreen( pMe, SCREENID_LONGSMS, 0 );
					bHandled = TRUE;
				}
			}
		}
		break;
	}
	
	return bHandled;
}


/************************************************************************/
/* ��δ���ļ���Ϊ�Ѷ��ļ�                                                                     */
/************************************************************************/
void LongSMS_hasread(newgimis *pMe)
{
	IFileMgr* pFileMgr = NULL;

	char s_Name[MAX_FILE_NAME + 1] = {0};
	char s_buf[MAX_FILE_NAME + 1] = {0};
	int idx = 0;
	
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
	{
		return;
	}

	idx = ud_rDistToChar(pMe->s_SaveLongSMSName[pMe->m_select_num].FileName,'*');
	STRNCPY(s_buf, pMe->s_SaveLongSMSName[pMe->m_select_num].FileName, idx);
	SPRINTF(s_Name,"%s%s", s_buf, pMe->s_SaveLongSMSName[pMe->m_select_num].FileName + idx + 1);
	
	if ( IFILEMGR_Rename(pFileMgr, pMe->s_SaveLongSMSName[pMe->m_select_num].FileName, s_Name) == SUCCESS ) 
	{
		MEMSET(pMe->s_SaveLongSMSName[pMe->m_select_num].FileName,0x00,64);
		STRCPY(pMe->s_SaveLongSMSName[pMe->m_select_num].FileName,s_Name);

		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"LongSMS_hasread %s success",pMe->s_SaveLongSMSName[pMe->m_select_num].FileName); 
	}
	else
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"LongSMS_hasread %s fail",pMe->s_SaveLongSMSName[pMe->m_select_num].FileName);
	}
	
	if (pFileMgr)
	{
		IFILEMGR_Release(pFileMgr);
		pFileMgr = NULL;
	}

	return;
}


/************************************************************************/
/* ��̨ģʽ��������                                                                     */
/************************************************************************/
boolean SamplePosDet_LongSMSNotify_HandleEvent( newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam )
{
	AEERect rRect;
	boolean bHandled = FALSE;
	IMenuCtl *pMenuCtl = testgps_GetScreenData_LongSMS_Notity( pMe );
	IMenuCtl *pMenuCtl_N = testgps_GetScreenData( pMe );
	
	switch( eCode )
	{
		case EVT_SCREEN:
			if( wParam == SCREEN_PARAM_INIT )
			{
				if( pMenuCtl_N ) 
				{  /* This is not expected to happen */
					testgps_DrawScreen( pMe, 0 );
					bHandled = TRUE;
				}
				else
				{
					if( (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_MENUCTL, (void **)&pMenuCtl_N ) == SUCCESS)	&&
						(ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&pMenuCtl ) == SUCCESS) ) {

						IMENUCTL_SetTitle( pMenuCtl_N, NEWGIMIS_RES_FILE, IDS_LONGSMS_TITLE_1, NULL );
						IMENUCTL_AddItem( pMenuCtl_N, NEWGIMIS_RES_FILE, IDS_LONGSMS_TITLE_2,0, NULL, (uint32)pMenuCtl_N );
						IMENUCTL_AddItem( pMenuCtl_N, NEWGIMIS_RES_FILE, IDS_LONGSMS_TITLE_3,1, NULL, (uint32)pMenuCtl_N );
						IMENUCTL_AddItem( pMenuCtl_N, NEWGIMIS_RES_FILE, IDS_LONGSMS_TITLE_4,2, NULL, (uint32)pMenuCtl_N );
						IMENUCTL_AddItem( pMenuCtl_N, NEWGIMIS_RES_FILE, IDS_LONGSMS_TITLE_5,3, NULL, (uint32)pMenuCtl_N );
						IMENUCTL_AddItem( pMenuCtl_N, NEWGIMIS_RES_FILE, IDS_LONGSMS_TITLE_6,4, NULL, (uint32)pMenuCtl_N );

						IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_LONGSMS_NOTIFY, 0, NULL, (uint32)pMenuCtl );
						
						SETAEERECT(&rRect, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen -  pMe->iFontHeight-10);
						IMENUCTL_SetRect( pMenuCtl_N, &rRect );

						SETAEERECT(&rRect, 0, pMe->DeviceInfo.cyScreen -  pMe->iFontHeight-10, pMe->DeviceInfo.cxScreen,  pMe->iFontHeight+10);
						IMENUCTL_SetRect( pMenuCtl, &rRect );

						{	// ��������
							AEEMenuColors stColors;
							stColors.wMask = CLR_SYS_ITEM|CLR_SYS_ITEM_TEXT|CLR_SYS_ITEM_SEL|CLR_SYS_ITEM_SEL_TEXT;
							stColors.cBack = MAKE_RGB(199,230,248);
							stColors.cText = MAKE_RGB(9,90,136);
							stColors.cSelBack = MAKE_RGB(25,137,199);
							stColors.cSelText = MAKE_RGB(255,255,255);
							IMENUCTL_SetColors(pMenuCtl,&stColors);			
						}  // ��������
						
						IMENUCTL_SetActive( pMenuCtl_N, FALSE );
						IMENUCTL_SetActive( pMenuCtl, TRUE );
						
						IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
						IDISPLAY_Update (pMe->a.m_pIDisplay);
						testgps_SetScreenData( pMe, (void *)pMenuCtl_N );
						testgps_SetScreenData_LongSMS_Notity( pMe, (void *)pMenuCtl );
						testgps_DrawScreen( pMe, 0 );
						bHandled = TRUE;
					}
				}
			}
			else if( wParam == SCREEN_PARAM_CLOSE )
			{
				if (pMenuCtl)
				{
					IMENUCTL_Release( pMenuCtl );
				}
				if (pMenuCtl_N)
				{
					IMENUCTL_Release( pMenuCtl_N );
				}
				
				testgps_SetScreenData( pMe, 0 );
				testgps_SetScreenData_LongSMS_Notity( pMe, 0 );
				pMe->currentHandler = 0;
				
				pMenuCtl = NULL;
				bHandled = TRUE;
			}
			else if( wParam == SCREEN_PARAM_PAINT ) 
			{
				IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
				if( IMENUCTL_IsActive( pMenuCtl ) )
				{
					IMENUCTL_Redraw( pMenuCtl );			
				}
				else
				{
					IMENUCTL_SetActive( pMenuCtl, TRUE );  /* Also causes a menu draw */			 
				}

				IMENUCTL_Redraw( pMenuCtl_N );
				bHandled = TRUE;
			}	
			break;
			
		case EVT_KEY:
		
			bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );		   
			
			if (wParam == AVK_SOFT2)	
			{
				if ( pMe->m_Right_Key == '1' )
				{
					if (pMenuCtl)
					{
						IMENUCTL_Release( pMenuCtl );
					}
					if (pMenuCtl_N)
					{
						IMENUCTL_Release( pMenuCtl_N );
					}
					
					pMenuCtl = NULL;
					pMenuCtl_N = NULL;
					
					pMe->currentHandler = 0;
					testgps_SetScreenData( pMe, 0 );
					
					//	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
					pMe->m_RelyExit = FALSE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
					ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
					pMe->m_FOBFlag = TRUE; //�ں�̨����
					pMe->m_WitchPag = 0;
					//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"��̨���� 6:%d",pMe->m_WitchPag);
					bHandled = TRUE;
					return TRUE;
				}
			}
			//////////////////////////////////////
			if (wParam == AVK_END)
			{
				if (STRCMP(pMe->sMobileType,"55") == 0 || (STRCMP(pMe->sMobileType,"03") == 0) || (STRCMP(pMe->sMobileType,"58") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0) || (STRCMP(pMe->sMobileType,"57") == 0) )
				{
					// ����ֱ�ӻص���̨
					if (pMenuCtl)
					{
						IMENUCTL_Release( pMenuCtl );
					}
					if (pMenuCtl_N)
					{
						IMENUCTL_Release( pMenuCtl_N );
					}
					
					pMenuCtl = NULL;
					pMenuCtl_N = NULL;
					
					pMe->currentHandler = 0;
					testgps_SetScreenData( pMe, 0 );
					//	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
					pMe->m_RelyExit = FALSE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
					ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
					pMe->m_FOBFlag = TRUE; //�ں�̨����
					pMe->m_WitchPag = 0;
					//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"��̨���� 6:%d",pMe->m_WitchPag);
					bHandled = TRUE;
					return TRUE;
				}
			}
			
			if( wParam == AVK_END  || wParam == AVK_CLR ) 
			{
				pMe->m_WitchPag = 10; // ����������
				testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
				//		   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
				//		   pMe->m_FOBFlag = TRUE; //�ں�̨����
				pMe->m_WitchPag = 10;
				bHandled = TRUE;
			}
			break;
		
		case EVT_COMMAND:
			if( dwParam == (uint32)pMenuCtl ) 
			{
				//IMenuCtl *pMenuCtl_N = NULL;
				
				//pMe->m_WitchPag = 11; 
				//testgps_GotoScreen( pMe, SCREENID_LONGSMS, 0 );

				//pMenuCtl_N = testgps_GetScreenData( pMe );

				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_LongSMSNotify_HandleEvent ��ת����Ϣ�������");	

				load_LongSMS_Num(pMe);//�õ�ȫ����Ϣ����  �����и�����������Ϣ��ֵ pMe->m_select_num
				load_LongSMS_Data(pMe);//�õ��������顣��Ϣ���������Ҫʹ�á�

				//����� �ı����� ȥ��#����Ѷ�
				if (pMe->s_SaveLongSMSName[pMe->m_select_num].ihasRead == 0)
				{
					LongSMS_hasread(pMe);
					
					pMe->m_LongSMS_uReadNum = pMe->m_LongSMS_uReadNum - 1;//Ϊ�˿���ʱ�򣬱�ֱ�Ӱ�����������ˣ��ڽ�������ʱ����ʵ�Ѷ��Ѿ�û���ˡ�
				}

				pMe->m_WitchPag = 12; 
				testgps_GotoScreen( pMe, SCREENID_LONGSMS_CONTENT, 0 );

				//ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,EVT_COMMAND,pMe->m_select_num,(uint32)pMenuCtl_N);//

				return TRUE;
			}
		break;
		
	}
	return bHandled;
}
