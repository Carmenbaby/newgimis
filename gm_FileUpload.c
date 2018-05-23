/*===============================================================================
 �����й��ϴ��ļ�
=============================================================================== */

#include "newgimis.bid"
#include "newgimis.h"

#include "gm_FileUpload.h"
#include "gmTools.h"

#include "MD5.h"

#define SEGMENT_LEN		1			// �������εĳ��ȡ���Ҫ���ֽ���


/*-------------------------------------------------------------------
Static function prototypes
-------------------------------------------------------------------*/
// ��http post �����ļ���صĺ���
static void FleUpd_Start(FileUpload* me);
static void FleUpd_GotResp(void *p);		// Ӧ��http����
//static void FleUpd_Header(void *p, const char *cpszName, GetLine *pglVal);
static void FleUpd_Status(void *p, WebStatus ws, void *pVal);
static void FleUpd_ReadBody(void *p);
// ��ʼ��
static void FleUpd_Init(FileUpload* me);
// ��ʼ��IAudRecRecordsA(�ϴ�¼���ļ�)
static boolean FleUpd_InitAudRec(FileUpload* me,char *sParam);			// Add by ---- 2014-04-10

// �õ���ƵĿ¼�µ���Ƶ��Ϣ
static void FleUpd_GetVideo(FileUpload* me);
// ɾ���Ѿ����͵���Ƶ�ļ�(����Ƭ�ļ�)
static void FleUpd_DelFile(FileUpload* me);

// �����ϴ��ļ�
static void FleUpd_Upload(FileUpload* me);
// ����Ҫ�ϴ�����Ϣ
static void FleUpd_SetData(FileUpload* me);
// �����ϴ��ĵ�1��������
static void FleUpd_SetData01(FileUpload* me);
// �˳�������
static void FleUpd_Close(newgimis *pMe);		// FileUpload* me);

// �����ϴ��ļ�
static void FleUpd_Send(FileUpload* me);

/*-------------------------------------------------------------------
function prototypes
-------------------------------------------------------------------*/
// ����ļ������Ƿ�ʱ
void FleUpd_MonitorSendFile(FileUpload *me);  // uf_MonitorSendFile(FileUpload *me);
// ������ɺ�Ķ���
void FleUpd_Afterpost(newgimis *pMe);
// ��ʾ�ϴ��ļ��Ľ��
void FleUpd_SendFileResult(FileUpload *me);
// ��ʾ�ϴ��ļ��Ľ�� �ϴ�������ʧ�ܵĽ��
void FleUpd_SendFileResult_error(FileUpload *me);

// =================== �й��ϴ�¼���ļ� =================================
// �ͷ�IAudRecRecordsA����Դ
void FleUpd_ReleaseRec(FileUpload* me);
// ��ʼ��IAudRecRecordsA(�ϴ�¼���ļ�)�����ò���
boolean FleUpd_InitAudRec(FileUpload* me,char *sParam);
// �ѷ���������¼���ļ����浽 IAudRecRecordsA ��
void FleUpd_SaveAudRec(FileUpload* me,char* sFileName);
// �ҳ�����������¼���ļ�
void FleUpd_GetAudRec(FileUpload* me);
// ������һ��¼���ļ�
void FleUpd_SendNextAudioRec(FileUpload* me);
// =======================================================================================
boolean FleUpd_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	FileUpload* me;
	
	if (pMe->FleUpdApp != NULL)
		me = (FileUpload *)pMe->FleUpdApp;
	else if (eCode != FLDUPD_EVT_START)
		return FALSE;
	
	switch (eCode)
	{
    case FLDUPD_EVT_START:
		{
			boolean bRtn = TRUE;
			if (pMe->FleUpdApp	== NULL)
			    ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_HandleEvent in FLDUPD_EVT_START, FleUpdApp is null");
			else
			    ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_HandleEvent in FLDUPD_EVT_START, cType=%d",me->cType);
			
			if (pMe->FleUpdApp	== NULL) {
				me = (FileUpload *)MALLOC(sizeof(FileUpload));
				MEMSET(me,0,sizeof(FileUpload));
				me->m_pParent = pMe;
				me->a = pMe->a;
				pMe->FleUpdApp = (void *)me;
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"shang chuan jin lai 3");
				FleUpd_Init(me);			// ---- ��ʼ����ת����̨����
				if ( me->nState != SUCCESS )
					bRtn = FALSE;
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"shang chuan jin lai 4");
			}
			else if (me->pAudRecRecordsA != NULL) {  // -0					// Add by ---- 2014-04-10
				// ��������ϴ�¼���ļ����Ͳ��ܴ������ļ�
				if (me->cType != '5')  {
					// ��ʾͼƬ
					newgimis* pMe = me->m_pParent;
					pMe->iPressSts = 27;
/*
					if (!uf_ForgeGroundEvt(pMe)) {
						ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Upload,FLDUPD_EVT_START,m_bGoBg=0,cType=%d",me->cType);
						uf_ShowSendPic(pMe);
						cgimis_FinishAlert(pMe,2);
						ISHELL_SetTimer(pMe->theApp.m_pIShell, 2500, (PFNNOTIFY)gimis_ToBackground, pMe);

					}
*/					
					bRtn = FALSE;
				}
			}	// -0

			if (bRtn) {	// -- true
				if ( (me->cType != 0 ) && (me->cType != '5') ) {
					FleUpd_Init(me);
					me->cType = '0';
				}

				if (wParam == 1)  {		// ȡ���ϴ�¼���ļ������ڷ�Χ
					char *pStrAudRec = (char *)dwParam;
					bRtn = FleUpd_InitAudRec(me,pStrAudRec);
				}
			} // -- true
			// Add End
			return bRtn;
		}
		
		break;
    case FLDUPD_EVT_STOP:							

		DBGPRINTF("-----FLDUPD_EVT_STOP");
		// ������ struct _FileUpload �к���  cgimis	*m_pParent�����ĳ�����ţ�����ֻ���ǹرճ���ʱ�����˳�
		// ��������������Ҳ������������������⡣�л�����ϸ�о�.    2012-10-26 ©�˸�break, �����ˣ�������
		FleUpd_Close(pMe);
		
		me->m_pParent = 0;
		FREEIF(pMe->FleUpdApp);
        break;
    case FLDUPD_EVT_UPLOAD:		
		me->cType = (char)wParam;			// �����ļ�������  52 --- '4'

	//	FleUpd_HandleEvent in FLDUPD_EVT_START
		FleUpd_Upload(me);
		return TRUE; 
	case FLDUPD_EVT_SEND:
//		uf_RecLog(me->m_pParent,"������һ�� ����");
		uf_RecLog(me->m_pParent,"send the next leg come");
		FleUpd_Send(me);
		return TRUE; 

    default:
		break;
    }
	
    return(FALSE);
}

/********************************************************************************************
// ������ɺ�Ķ���
// �������
  FileUpload * pMe	: 
********************************************************************************************/
//static void FleUpd_Close(FileUpload* me)
static void FleUpd_Close(newgimis *pMe)
{
	FileUpload* me;
	me = (FileUpload*)pMe->FleUpdApp;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Close in");

	if (me->pFileUpload != NULL) {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Close in,pFileUpload!=NULL");
		IFILE_Release(me->pFileUpload);
		me->pFileUpload = NULL;
	}

	if (me->pFileMgr != NULL) {
		IFILEMGR_Release(me->pFileMgr);
		me->pFileMgr = NULL;
	}

	if (me->pszPostData != NULL) {
		FREE( me->pszPostData );
		me->pszPostData = NULL;
		me->iFileRead = 0;
	}
	
	if (me->pipPostData != NULL)
	{
		IPEEK_Release(me->pipPostData);
		me->pipPostData = NULL;
	}
	
	if (me->pSource != NULL)
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Close in,pSource != NULL");
		ISOURCE_Release(me->pSource);
		me->pSource = NULL;
	}
	
	if (me->piWResp != NULL)
	{
		uint32 nErr = IWEBRESP_Release(me->piWResp);		// 2.6.4D2
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Close in,clear piWResp,return=%d,n=%d",nErr);
		me->piWResp = NULL;
	}

	CALLBACK_Cancel(&me->webcb);

	if (me->pisu != NULL) {
		ISOURCEUTIL_Release(me->pisu);
		me->pisu = NULL;
	}
	
	if(me->pWeb)	{
		int i,n;			// 2.6.4D2
		n = 0;
		i=IWEB_Release(me->pWeb);
		while (i>0) {
			n++;
			i=IWEB_Release(me->pWeb);
		}
		
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Close, IWEB_Release=%d,n=%d",i,n);
		me->pWeb = 0;
	}

	me->nState = SUCCESS;
	me->cType = '0';

// 	pMe->FleUpdApp = 0;			// �����к���Ҫ����������
//  	me->m_pParent = 0;
//  	FREEIF(me);
   
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Close out");

	return;
}


/********************************************************************************************
// ������ɺ�Ķ���
// �������
  FileUpload * pMe	: 
********************************************************************************************/
void FleUpd_Afterpost(newgimis *pMe)
{
	FileUpload* me;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Afterpost in,iPressSts=%d",pMe->iPressSts);
	
	if (pMe->FleUpdApp != NULL)
		me = (FileUpload *)pMe->FleUpdApp;
	else
		return;

// Mark 2014-04-10 Ӧ���д�ע��
//	if (me->pFileUpload != NULL) {
//		ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Afterpost in,pFileUpload!=NULL");
//		IFILE_Release(me->pFileUpload);
//		me->pFileUpload = NULL;
//	}

	if (me->cType == '1') {
		// ������Ƶ�ļ��Ѵ�����
		pMe->iPressSts = 0;
		IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
		IDISPLAY_Update(pMe->a.m_pIDisplay);

		pMe->m_bSendPF = FALSE;
//		uf_RecLog(pMe,"�������˵�");
		pMe->m_WitchPag = 10;
		testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );  //���ص�������

		// ԭ�����ں�̨���ص���̨
/*
		if (pMe->m_bOldGoBg) {
			pMe->m_bGoBg = TRUE;
			ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE);
		}
*/
	}
	else if (me->cType == '4') {		// 2012-10-16
		uf_Afterpost(pMe);
		pMe->m_bSendPF = FALSE;
//		uf_RecLog(pMe,"�������˵�");
		pMe->m_WitchPag = 10;
		testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );  //���ص�������
	}

	else if (me->cType == '3') {		// add by 2015-9-14
		uf_Afterpost(pMe);
		pMe->m_bSendPF = FALSE;
//		uf_RecLog(pMe,"���شβ˵�");
		pMe->m_WitchPag = 4;
		testgps_GotoScreen( pMe, SCREENID_MORE, 0 );  //���ص�������
	}

	// Add by ---- 2014-04-10
	FleUpd_Close(pMe);		
	// Add End

// 2014-04-10 ����FLDUPD_EVT_STOP��¼�����������⣬�����ע����	ISHELL_PostEvent( pMe->theApp.m_pIShell,pMe->theApp.clsID, FLDUPD_EVT_STOP,0, 0); 

	return;
}
/********************************************************************************************
// ��ʼ��
// �������
  FileUpload * me	: 
********************************************************************************************/
static void FleUpd_Init(FileUpload* me)
{
	//  char buf[128];   
	newgimis *pMe = me->m_pParent;
	int nErr = SUCCESS;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FileUpload FleUpd_Init in, cType=%d",me->cType);

	me->nState = EFAILED;
	MEMSET(me->pText,0,(TEXT_MAXSIZE+1)*sizeof(AECHAR));	
	
	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&me->pFileMgr);
	if(SUCCESS != nErr)  	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FileUpload AEECLSID_FILEMGR error");
		goto ExitHere;
	}

	nErr = ISHELL_CreateInstance(me->a.m_pIShell, AEECLSID_WEB, (void**)&me->pWeb);			// ---- Web�ӿ�
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "FleUpd_Init IWeb err=%d", nErr);
		goto ExitHere;
	}
	
	ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&me->pisu);
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Init ISourceUtil err=%d", nErr);
		goto ExitHere;
	}

	me->nState = SUCCESS;
	me->cType = 0;

	me->pAudRecRecordsA = NULL;			// 2014-04-10

ExitHere:
	if (SUCCESS != nErr)
		me->nState = EFAILED;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FileUpload FleUpd_Init out nState=%d",me->nState);

	return;
}

/********************************************************************************************
// ��ʼ��IAudRecRecordsA(�ϴ�¼���ļ�)�����ò���
// �������
//    char *sParam : �ϴ�ʱ�䷶Χ��¼���ļ�,��ʼʱ��(yymmddhhmm,������ʱ��),����ʱ��(yymmddhhmm)�� ���磺 1404100800,1404101500, ��ʾ 14-04-10 08:00 �� 14-04-10 15:00��¼���ļ�
// ��������
//   TRUE : �������
********************************************************************************************/
boolean FleUpd_InitAudRec(FileUpload* me,char *sParam)
{
	boolean bRtn = TRUE;
	newgimis *pMe = me->m_pParent;
	char *pStr = sParam;
	int nLen = 0;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_InitAudRecin, pAudRecRecordsA=%d,sParam=%d(%s)",me->pAudRecRecordsA,sParam,sParam);

	if (me->pAudRecRecordsA == NULL) {
		me->pAudRecRecordsA = (IAudRecRecordsA *)MALLOC(sizeof(IAudRecRecordsA));
	}

	nLen = STRLEN(sParam);
	pStr = uf_GetSubStr(pStr,nLen,me->pAudRecRecordsA->sDate1,sizeof(me->pAudRecRecordsA->sDate1),DEFAULT_SPLIT);
	if (pStr != 0) {
		nLen = nLen - STRLEN(me->pAudRecRecordsA->sDate1) - 1;
		pStr = uf_GetSubStr(pStr,nLen,me->pAudRecRecordsA->sDate2,sizeof(me->pAudRecRecordsA->sDate2),DEFAULT_SPLIT);
	}

	me->pAudRecRecordsA->nRecordCount = 0;
	me->pAudRecRecordsA->nSelRec = me->pAudRecRecordsA->nRecordCount;
	me->pAudRecRecordsA->cStatus = ( (STRLEN(me->pAudRecRecordsA->sDate1) >0) && (STRLEN(me->pAudRecRecordsA->sDate2) >0) ) ? 1 : 0;
	bRtn = me->pAudRecRecordsA->cStatus;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_InitAudRecin, pAudRecRecordsA=%d,Date1=%s,Date2=%s,return=%d"
		      ,me->pAudRecRecordsA,me->pAudRecRecordsA->sDate1,me->pAudRecRecordsA->sDate2,bRtn);

	return bRtn;

}
// =============================================================
// �ͷ�IAudRecRecordsA����Դ
// =============================================================
void FleUpd_ReleaseRec(FileUpload* me)
{
	
	if (me->pAudRecRecordsA != NULL) {
		int i = 0;
		for(i=0;i<me->pAudRecRecordsA->nRecordCount;i++) {
			MEMSET(me->pAudRecRecordsA->pAudRecFieldA[i].sRecName,0,sizeof(me->pAudRecRecordsA->pAudRecFieldA[i].sRecName));
		}
		FREEIF(me->pAudRecRecordsA->pAudRecFieldA);
		me->pAudRecRecordsA->nRecordCount = 0;
		FREEIF(me->pAudRecRecordsA);
	}
	
	return;
}

// =============================================================
// �ѷ���������¼���ļ����浽 IAudRecRecordsA ��
// ���������
//  char sFileName : ¼���ļ���(��·��)
// return :  0: ����ɹ��� 1: ʧ��
// =============================================================
void FleUpd_SaveAudRec(FileUpload* me,char* sFileName)
{
	newgimis *pMe = me->m_pParent;
	int i = 0;

//	ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_SaveAudRec in,sFileName=%s",sFileName);

	for (i=0; i<me->pAudRecRecordsA->nRecordCount; i++)	 {
		if ( STRCMP(me->pAudRecRecordsA->pAudRecFieldA[i].sRecName,sFileName) == 0)
			return;
	}

	i = me->pAudRecRecordsA->nRecordCount;
	if ( i == 0 ) 
		me->pAudRecRecordsA->pAudRecFieldA = (IAudRecFieldA *)MALLOC(sizeof(IAudRecFieldA));
	else {
		me->pAudRecRecordsA->pAudRecFieldA = (IAudRecFieldA *)REALLOC(me->pAudRecRecordsA->pAudRecFieldA, (i+1)*sizeof(IAudRecFieldA));
	}
	me->pAudRecRecordsA->nRecordCount++;
	MEMSET(me->pAudRecRecordsA->pAudRecFieldA+i,0,sizeof(IAudRecFieldA));

	me->pAudRecRecordsA->pAudRecFieldA[i].cSendStatus = 0;
	STRCPY(me->pAudRecRecordsA->pAudRecFieldA[i].sRecName,sFileName);

	return;
}
// =============================================================
// ɾ���Ѿ����͵���Ƶ�ļ�(����Ƭ�ļ�)
// =============================================================
static void FleUpd_DelFile(FileUpload* me)
{
	newgimis *pMe = me->m_pParent;
	 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_DelFile in");

    if ( STRLEN(me->sFile) <= 0 )	// �������ļ�
		return;

	if (me->pFileUpload != NULL) {
		IFILE_Release(me->pFileUpload);
		me->pFileUpload = NULL;
	}
	
	if ( IFILEMGR_Remove(me->pFileMgr,me->sFile) == SUCCESS )
		MEMSET(me->sFile,0,sizeof(me->sFile));
}

// =====================================================================
// �õ���ƵĿ¼�µ���Ƭ��Ϣ
// =====================================================================
static void FleUpd_GetVideo(FileUpload* me)
{

  FileInfo oFileInfo;	
  char    *pszTok = NULL;
  char    *pszBuf = NULL;

  char sFileName[MAX_FILE_NAME];				// ��Ƶ�ļ���
  uint32 dwDateTime;							// ���µ���Ƶ�ļ�������ʱ��
  char strRoot[512];
  newgimis *pMe = me->m_pParent;

  char sBufLog[126];
  int idx;

  me->nState = EFAILED;
  dwDateTime = 0;

   // �������ļ��ж����ֻ���Ƶ�ļ����·��
   SPRINTF(strRoot, AEEFS_ROOT_DIR"%s", me->m_pParent->sVideoPath);
   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"video_path=%s",strRoot);

   // �������µ���Ƶ�ļ�
  if ( IFILEMGR_EnumInit( me->pFileMgr, strRoot, FALSE )  != SUCCESS)
  {
	  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_GetVideo IFILEMGR_EnumInit [%s] error",strRoot);
	  goto ExitHere;
  }

    ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_GetVideo IFILEMGR_EnumNext");

	idx = 0;
    while( IFILEMGR_EnumNext( me->pFileMgr, &oFileInfo ) )   {		// while -- 1
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"EnumNext File %s, dwCreationDate=%d",oFileInfo.szName,oFileInfo.dwCreationDate);

		if ( (STRCMP(me->m_pParent->sMobileType,"33") == 0) ) {
			MEMSET(sBufLog,0,STRLEN( oFileInfo.szName )+1);
			STRCPY(sBufLog,oFileInfo.szName);
			STRLOWER(sBufLog);
			if ( STRENDS( "sys", sBufLog ) )
				continue;
		}

		if (oFileInfo.dwCreationDate == 0)  continue;

		if (oFileInfo.dwCreationDate > dwDateTime) {	    
			dwDateTime = oFileInfo.dwCreationDate;
			STRCPY( sFileName, oFileInfo.szName );
		}

    }  // while -- 1

	// ����Ҫ���͵���Ƶ�ļ�����·����
	if ( dwDateTime > 0 ) {
		SPRINTF(me->sFile, "%s", sFileName);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"will send video file=%s",me->sFile);
		me->nState = SUCCESS;
	}

ExitHere:

  FREEIF(pszTok);
  FREEIF(pszBuf);
  return;	
}

/********************************************************************************************
// ��Ҫ�ϴ����ļ�
********************************************************************************************/
static void FleUpd_OpenFile(FileUpload* me)
{
	char sLogBuf[128];
	newgimis *pMe = me->m_pParent;

	if (me->cType != '5')
	    ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_OpenFile in,me->cType=%c[%d]",me->cType,me->cType);
	else
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_OpenFile in,nRecordCount=%d,nSelRec=%d"
		  ,me->pAudRecRecordsA->nRecordCount,me->pAudRecRecordsA->nSelRec);

	me->nState = EFAILED;

	me->cSendSts = 'A';					// ���������ֵ
	me->nHasRead = 0;
	me->nPartNum = 0;					// ���ڴ��ڼ��Σ�1 ��1�Σ�2 ��2�Σ�����������  
    me->nPartsTotal = 0;
	MEMSET(me->pText,0,(TEXT_MAXSIZE+1)*sizeof(AECHAR));	

	switch(me->cType)
	{
	case '1':				// �ҵ�Ҫ������Ƶ�ļ�
		if(STRLEN(me->m_pParent->sFile) > 0)
		{
			me->nState = SUCCESS;
		}
		else
		{
			me->nState = EFAILED;
		}
		SPRINTF(me->sFile, "%s", me->m_pParent->sFile);
//		FleUpd_GetVideo(me);
		if (me->nState == EFAILED) 	goto ExitHere;
		break;
	case '2':
		SPRINTF( me->sFile, AEEFS_HOME_DIR"%s",SPD_CONFIG_FILE);
		break;
	case '3':
		SPRINTF( me->sFile, AEEFS_HOME_DIR"rec_info.log");
		break;
	case '4':						// 2012-10-16   4: ��Ƭ
		{
		   STRCPY(me->sFile,me->m_pParent->sFileName);		// 2012-10-16  sFileNameҪ�ĳɺ�·��
		   //  [3/28/2016 Administrator]
		   //ITEXTCTL_GetText(me->m_pParent->textCtl,me->m_pParent->pText,256);
		   WSTRCPY(me->pText,me->m_pParent->pText);
		}
		break;
	case '5':			// Add by ---- 2014-04-10
		{
			int idx = me->pAudRecRecordsA->nSelRec;
			// ö��¼���ļ�·�����ѷ����������ļ��������ϴ��ṹ��
			if (me->pAudRecRecordsA->cStatus == 1) { // 1: �յ��µ��ϴ�ָ��
				  FleUpd_GetAudRec(me);	
				  me->pAudRecRecordsA->cStatus = 0;	
			}

			if (idx >= me->pAudRecRecordsA->nRecordCount) {
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_OpenFile in,send audio recorder file finish");
				return;	// �����ļ�
			}

			if (me->pAudRecRecordsA->pAudRecFieldA[idx].cSendStatus == 10) {
				me->pAudRecRecordsA->nSelRec++;
				idx = me->pAudRecRecordsA->nSelRec;
			}

			if (me->pAudRecRecordsA->pAudRecFieldA[idx].cSendStatus != 0) {
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_OpenFile in,cSendStatus[%d]=%d",idx,me->pAudRecRecordsA->pAudRecFieldA[idx].cSendStatus);
				return;	// �ļ�״̬������δ��״̬
			}

			STRCPY(me->sFile,me->pAudRecRecordsA->pAudRecFieldA[idx].sRecName);

		}
		break;			
	case '6':
		{
			SPRINTF( me->sFile, "%s",pMe->file_Name_7Day);
			break;
		}// Add End
	}
	
	if (SUCCESS != IFILEMGR_Test(me->pFileMgr, me->sFile)) {
		SPRINTF(sLogBuf,"IFILEMGR_Test file[%s] error",me->sFile);
		uf_RecLog(me->m_pParent, sLogBuf);
		me->nState = EFAILED;
		goto ExitHere;
	}
	
	me->pFileUpload = IFILEMGR_OpenFile(me->pFileMgr, me->sFile, _OFM_READ);
	if (0 == me->pFileUpload)
	{
		SPRINTF(sLogBuf,"read file[%s] error",me->sFile);
		uf_RecLog(me->m_pParent, sLogBuf);
		goto ExitHere;
	}
	
    if ( SUCCESS != IFILE_GetInfo( me->pFileUpload, &me->oFileInfo ) ) {
		uf_RecLog(me->m_pParent, "IFILE_GetInfo err");
		me->nState = EFAILED;
		goto ExitHere;
	}
	
    if ( me->oFileInfo.dwSize == 0 ) {
		uf_RecLog(me->m_pParent, "dwSize = 0");
		me->nState = EFAILED;
		goto ExitHere;
	}
	
	IFILE_Seek(me->pFileUpload,_SEEK_START ,0);
	
	me->cSendSts = '1';
	me->nPartsTotal = (uint8)(me->oFileInfo.dwSize / MAX_FILE_BYTES_POST);				// �ܶ���
	if ( (me->oFileInfo.dwSize % MAX_FILE_BYTES_POST) != 0 ) {
		me->nPartsTotal += 1;
	}
	
	SPRINTF(sLogBuf,"FleUpd_OpenFile,MAX_FILE_BYTES_POST=%d,oFileInfo.dwSize =%u, nPartsTotal=%d",MAX_FILE_BYTES_POST,me->oFileInfo.dwSize,me->nPartsTotal);
	uf_RecLog(me->m_pParent,sLogBuf);

	me->nState = SUCCESS;

	// Add by ---- 2014-04-10
	if (me->cType == '5') 
		me->pAudRecRecordsA->pAudRecFieldA[me->pAudRecRecordsA->nSelRec].cSendStatus = 1;	// �򿪷����ļ�
	// Add End


ExitHere:
	
	SPRINTF(sLogBuf,"FleUpd_OpenFile out nState=%d",me->nState);
	uf_RecLog(me->m_pParent,sLogBuf);
	
	return;

}
/********************************************************************************************
// ��ʼ�ϴ��ļ�
********************************************************************************************/
static void FleUpd_Upload(FileUpload* me)
{
	newgimis *pMe;
	char sFormat[250];
	int idx;

	
	//-----------�й�MD5У��
	FileInfo pFileInfo ;
	MD5_CTX md5;
	unsigned char *file_buf = NULL;	//�ļ�buf
	unsigned char uc_md5[16];
	unsigned char PADDING[]={0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//--------------end

	pMe = me->m_pParent;

	uf_RecLog(me->m_pParent,"FleUpd_Upload in");
	// ���÷��͵�http��ַ
	MEMSET(sFormat,0,sizeof(sFormat));
    idx = ud_DistToChar(me->m_pParent->strCenterIp,'/');  
	if ( idx >= 0 )
		MEMCPY(sFormat, me->m_pParent->strCenterIp, idx); 
	else
		STRCPY(sFormat, me->m_pParent->strCenterIp);
	if ( (me->cType == '2') || (me->cType == '3') )		// 2013-09-01 ��־��������Ȩ����
	    SPRINTF(me->pURL,"http://www.richtalk.cn/FileUploadServlet");		//  61.131.1.153:8218		www.richtalk.cn
	else
		SPRINTF(me->pURL,"http://%s/FileUploadServlet",sFormat); /* http://zc.gota.richmap.cn/FileUploadServlet */
		//SPRINTF(me->pURL,"http://61.131.1.153:10018/partition-web/FileUploadServlet");//����
    // Add 2013-09-23
	if (STRLEN(pMe->MyMobileNo) > 0) {
		STRCAT(me->pURL,"?tel=");
		STRCAT(me->pURL,pMe->MyMobileNo);
		STRCAT(me->pURL,"&imsi=");
		STRCAT(me->pURL,pMe->szMobileID);
	}
	// Add End

	FleUpd_OpenFile(me);
// Change by ---- 2014-04-10	if (me->nPartNum == 0) 
	if ( (me->nPartNum == 0) && (me->nPartsTotal > 0) ) { // -- 0
		// ǰ̨��ʾ�������С�������������ͼƬ
		if ( (me->cType == '1') || (me->cType == '4') ) { // --1			// 2012-10-16	
			newgimis* pMe = me->m_pParent;
			pMe->iPressSts = (me->nState != SUCCESS)? 26: 25;

			if (!uf_ForgeGroundEvt(pMe)) {
				uf_RecLog(pMe,"FleUpd_Upload,m_bGoBg=0 ");
				uf_ShowSendPic(pMe);
			}
		}  // --1

	}	// -- 0	

	if ( me->nState != SUCCESS ) {
		if ( (me->cType == '1') || (me->cType == '4') )  {		// 2012-10-16
			ISHELL_Beep(me->a.m_pIShell, BEEP_MSG, TRUE);
			ISHELL_Beep(me->a.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
			ISHELL_SetTimer(me->a.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*) me->m_pParent);
			ISHELL_SetTimer(me->a.m_pIShell, 55 * 100, (PFNNOTIFY)FleUpd_Afterpost, me->m_pParent);
		}
		return;
	}

	if (me->cType == '4' && pMe->m_enable_MD5 == 1)
	{
		//------------------ȡMD5У��ֵ
		IFILE_GetInfo( me->pFileUpload, &pFileInfo );
		if (pFileInfo.dwSize > 0 )  // ˵�������Լ�����������
		{
			file_buf = (unsigned char *)MALLOC(pFileInfo.dwSize + 1);
			MEMSET(file_buf,0,pFileInfo.dwSize + 1);
			IFILE_Seek(me->pFileUpload,_SEEK_START,0);  
			IFILE_Read( me->pFileUpload, file_buf, pFileInfo.dwSize );
			
			//--wh���� --�ı��ַ�������admin
			// 		file_buf = (unsigned char*)MALLOC(11);
			// 		MEMSET(file_buf,0,10 + 1);
			// 		STRCPY((char*)file_buf,"admin12345");
			//--end
			
			MEMSET(pMe->s_MD5,0,33);
			MD5Init(&md5);         		
			MD5Update(&md5,file_buf,pFileInfo.dwSize);
			MD5Final(&md5,uc_md5,PADDING);  
			
			SPRINTF(pMe->s_MD5,"%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",uc_md5[0],uc_md5[1],uc_md5[2],\
				uc_md5[3],uc_md5[4],uc_md5[5],uc_md5[6],\
				uc_md5[7],uc_md5[8],uc_md5[9],uc_md5[10],\
				uc_md5[11],uc_md5[12],uc_md5[13],uc_md5[14],uc_md5[15]);
			
			ud_RecLog(me->m_pParent->a.m_pIShell,1,&(me->m_pParent->nInd),"MD5 pFileInfo.dwSize == %d",pFileInfo.dwSize);
			ud_RecLog(me->m_pParent->a.m_pIShell,1,&(me->m_pParent->nInd),"MD5 == %s",pMe->s_MD5);
			
			FREE(file_buf);
			IFILE_Seek(me->pFileUpload,_SEEK_START,0); //ҪŪ�ؿ�ͷ�� 
			
			//У��ֵ�õ�����Ҫ�����ϴ���ַ  &encode=
			STRCAT(me->pURL,"&encode=");
			STRCAT(me->pURL,pMe->s_MD5);
		}
	}//---------------------end

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Upload in url=%s",me->pURL);

	FleUpd_Send(me);
}

/********************************************************************************************
// ��ʼ�ϴ��ļ�
********************************************************************************************/
static void FleUpd_Send(FileUpload* me)
{

	// ���÷�������
	FleUpd_SetData(me);
	if (me->nState != SUCCESS)
		return;
	

	// ��ʼWeb����
	FleUpd_Start(me);

}

// ============================================
// ��post������������
static void FleUpd_Start(FileUpload* me)
{
    WebOpt awo[4]; 

//	cgimis * pMe = pwa->m_pParent;
	int iLen = 0;

    // Add Options
	awo[0].nId  = WEBOPT_CONNECTTIMEOUT;
    awo[0].pVal = (void *)20000;			// 2013-03-27  5000 
 // 2.6.2D2   awo[1].nId  = WEBOPT_FLAGS;
 // 2.6.2D2   awo[1].pVal = (void *)WEBREQUEST_NOWAITCONN;
    awo[1].nId  = WEBOPT_PROXYSPEC;
    awo[1].pVal = (void *)"http:///http://10.0.0.200:80";
    awo[2].nId  = WEBOPT_END;
	IWEB_AddOpt(me->pWeb, awo);

	me->dwPostOvertime = 0;
	
	if (me->pipPostData != NULL) {
		IPEEK_Release(me->pipPostData);
		me->pipPostData = NULL;
	}

	// if there's post data, construct a stream for IWeb to consume
	if ((char *)0 != me->pszPostData) {
		iLen = me->iFileRead;
		ISOURCEUTIL_PeekFromMemory(me->pisu, me->pszPostData, iLen, 0, 0,&me->pipPostData);
	}
	

	// start transaction, pass callbacks for web status, web headers
	// the extra WEBOPT_HEADER is used to help test what's sent 
	// (snoop.sh above shows all the headers)
	if ((IPeek *)0 != me->pipPostData) {

		uf_RecLog(me->m_pParent,"FleUpd_Start send file begin");

		// initialize the callback, where I'll be called when the request  completes
		CALLBACK_Init(&me->webcb, FleUpd_GotResp, me);

		IWEB_GetResponse(me->pWeb,
			(me->pWeb, &me->piWResp, &me->webcb, me->pURL, 
			WEBOPT_HANDLERDATA, me,
			WEBOPT_HEADER, "X-Method: POST\r\n", /* for kicks */
			WEBOPT_HEADERHANDLER, NULL,   
			WEBOPT_STATUSHANDLER, FleUpd_Status,  
			WEBOPT_METHOD, "POST",
			WEBOPT_BODY, me->pipPostData,
			WEBOPT_CONTENTLENGTH, iLen,		
			WEBOPT_END));
		
	}
	
}


/*********************** Ӧ��http���� *******************************************************/
static void FleUpd_GotResp(void *p)
{
	
	char sLogBuf[257];
	int nLen =0;
	FileUpload *me = (FileUpload *)p;
//	cgimis * pMe = pwa->m_pParent; 
	WebRespInfo *pInfo;
	
	uf_RecLog(me->m_pParent,"FleUpd_GotResp in");
	
	//   char szBuf[500];
	pInfo = IWEBRESP_GetInfo(me->piWResp);
	
	SPRINTF(sLogBuf, "FleUpd_GotResp response, code: %d",pInfo->nCode);
	nLen = STRLEN(sLogBuf);
	uf_RecLog(me->m_pParent,sLogBuf);
	if (!WEB_ERROR_SUCCEEDED(pInfo->nCode)) {  // 2014-04-10 ��returnǰ��������

		FleUpd_SendFileResult_error(me); // ��ʾʧ�ܵĽ���
		me->m_pParent->m_bSendPF = FALSE; //���÷�����ƬΪ����״̬

   	   ISHELL_SetTimer(me->a.m_pIShell, 3000, (PFNNOTIFY)FleUpd_Afterpost, me->m_pParent);
		return;
	}


	if(0 != pInfo->pisMessage) 
	{
		SPRINTF(sLogBuf,"FleUpd_GotResp,Got Message, time:%u",(GETTIMEMS()-me->dwPostOvertime));
		uf_RecLog(me->m_pParent, sLogBuf);
		if(0 == pInfo->lContentLength) 
			return; 	      
		
		me->dwPostOvertime = 0;				// ���յ�Ӧ��

		me->pSource = pInfo->pisMessage;
		ISOURCE_AddRef(me->pSource);				// ---- ��Read Body���õ�
// 2.6.2D2		IWEBRESP_Release(me->piWResp);
// 2.6.2D2		me->piWResp = 0;
		{
			newgimis * pMe; 
			int n=0;			// 2.6.4D2
			uint32 nErr = IWEBRESP_Release(me->piWResp);
			pMe=me->m_pParent;
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Close in,clear piWResp,return=%d,n=%d",nErr,n);
			me->piWResp = NULL;
		}
		
		CALLBACK_Cancel(&me->webcb);
		CALLBACK_Init(&me->webcb, FleUpd_ReadBody, me);
		FleUpd_ReadBody(me);
	}
}

// FleUpd_SendFileResult_error 

// ==========================================================
// ��ʾ�ϴ��ļ��Ľ��  ͨ�Ź��ϵĽ��
// ==========================================================
void FleUpd_SendFileResult_error(FileUpload *me)
{
	// ��ʾ���ͽ����Ϣ 
	// \u53d1\u9001\u5931\u8d25\uff0c\u624b\u673a\u6b20\u8d39\u6216\u6545\u969c  ����ʧ�ܣ��ֻ�Ƿ�ѻ����
	AECHAR szFailure[] = {0x53d1,0x9001,0x5931,0x8d25,0xff0c,0x624b,0x673a,0x6b20,0x8d39,0x6216,0x6545,0x969c,0x0000};	// ����ʧ��
//	AECHAR szNoFile[] = {0x5DF2,0x65E0,0x89C6,0x9891,0x6587,0x4EF6,0x0000}; // ������Ƶ�ļ� \uFEFF\u5DF2\u65E0\u89C6\u9891\u6587\u4EF6

				// ��ʾ����Ƶ�ļ��ɹ� 
	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1,    // -1 = �����ַ����Ķ���ʾ
	8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(me->a.m_pIDisplay);
	return;
}

// ==========================================================
// ��ʾ�ϴ��ļ��Ľ��
// ==========================================================
void FleUpd_SendFileResult(FileUpload *me)
{
	// ��ʾ���ͽ����Ϣ
	AECHAR szSuccess[] = {0x53D1,0x9001,0x6210,0x529F,0000};	// ���ͳɹ�  \uFEFF\u53D1\u9001\u6210\u529F
	AECHAR szFailure[] = {0x53D1,0x9001,0x5931,0x8D25,0x0000};	// ����ʧ��
//	AECHAR szNoFile[] = {0x5DF2,0x65E0,0x89C6,0x9891,0x6587,0x4EF6,0x0000}; // ������Ƶ�ļ� \uFEFF\u5DF2\u65E0\u89C6\u9891\u6587\u4EF6

				// ��ʾ����Ƶ�ļ��ɹ� 
	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	if ( me->cSendSts == '1' )
		IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szSuccess, -1,    // -1 = �����ַ����Ķ���ʾ
					8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	else              // �������  if ( me->cSendSts == 'A' ) 
		IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1,    // -1 = �����ַ����Ķ���ʾ
		8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);

	IDISPLAY_Update(me->a.m_pIDisplay);
				
	ISHELL_Beep(me->a.m_pIShell, BEEP_MSG, TRUE);
	ISHELL_Beep(me->a.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
	ISHELL_SetTimer(me->a.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*) me->m_pParent);
	ISHELL_SetTimer(me->a.m_pIShell, 55 * 100, (PFNNOTIFY)FleUpd_Afterpost, me->m_pParent);

	return;
}

// ==========================================================
// ��ȡ���Ķ˷��ص���Ϣ
// ==========================================================
static void FleUpd_ReadBody(void *p)
{

	FileUpload* me = (FileUpload* )p;
	int32 nRead;
	char sBodybuf[10];			// ���Ķ�ֻӦ��1������0���� ��Ҫ̫��
	char sLogBuf[256];

	uf_RecLog(me->m_pParent,"FleUpd_ReadBody in");
	

	MEMSET(sBodybuf,0,sizeof(sBodybuf));
	nRead = ISOURCE_Read(me->pSource, sBodybuf, sizeof(sBodybuf)-1 );
	if(ISOURCE_WAIT == nRead) 
	{
		ISOURCE_Readable(me->pSource, &me->webcb);
		return;
	}
	
	if (nRead>0)		// ---- ��������
	{
		int iLen = (nRead>sizeof(me->sBodyBuf))?sizeof(me->sBodyBuf):nRead;
		if (me->nBytes == 0)
		{
			MEMSET(me->sBodyBuf,0,sizeof(me->sBodyBuf));
			MEMCPY(me->sBodyBuf,sBodybuf,iLen);
		}
// ֻҪ��1���ַ� 		else 
//			   MEMCPY(me->sBodyBuf+me->nBytes,sBodybuf,iLen);
//   FleUpd_ReadBody Data Reading:1,sBodybuf = 0
		me->nBytes += nRead;
		SPRINTF(sLogBuf, "FleUpd_ReadBody Data Reading:%d,sBodybuf = %s", me->nBytes,sBodybuf);
		uf_RecLog(me->m_pParent, sLogBuf);
	}
	
	if (!ISOURCE_Exhausted(nRead))	{
		ISOURCE_Readable(me->pSource, &me->webcb);
		return;
	}
	
	ISOURCE_Release(me->pSource);
	me->pSource = NULL;
	CALLBACK_Cancel(&me->webcb);

	SPRINTF(sLogBuf,"FleUpd_ReadBody, me->sBodyBuf=%s",me->sBodyBuf);
	uf_RecLog(me->m_pParent,sLogBuf);

	me->cSendSts =  ( STRLEN(me->sBodyBuf) > 1 ) ? 'E' : me->sBodyBuf[0];		// 'E' ���������
	me->nBytes = 0;

	SPRINTF(sLogBuf,"FleUpd_ReadBody, me->cSendSts=%c",me->cSendSts);
	uf_RecLog(me->m_pParent,sLogBuf);

	// ���ݷ��صĽ������
	switch (me->cSendSts)
	{
	case '1':
		if (me->nPartNum == me->nPartsTotal) {	// �Ѿ����
			uf_RecLog(me->m_pParent,"Send file finish");
			if ( (me->cType == '1') || (me->cType == '4') )  {		// 2012-10-08
				// ɾ�����ͳɹ����ļ�
				FleUpd_DelFile(me);
				FleUpd_SendFileResult(me);
			}
			///// add by 2015-9-14  start  �������������ɾ��    ��Ϊһ��ֻ�����µ�һ�βű�����  ///////  
			else if (me->cType == '2')  // ˵���Ƿ��͵��������ļ�
			{
//				FleUpd_DelFile(me);
//				FleUpd_SendFileResult(me);
			}

			else if (me->cType == '3')  // ˵������־�ļ�  ɾ����־�ļ�  ֮���Էֿ� ���ܵ�ʱ����Ҫɾ��
			{
//				FleUpd_DelFile(me);
				if (me->m_pParent->mb_MsgLog == TRUE)
				{
					me->m_pParent->mb_MsgLog = FALSE;
				}
				else
				{
					FleUpd_SendFileResult(me);	
				}
				
			}

			///// add end ///////////////////////
			else if (me->cType == '5') {		// Add by ---- 2014-04-10
				FleUpd_SendNextAudioRec(me);
			}		// Add End  2014-04-10
			else
				
				ISHELL_SetTimer(me->a.m_pIShell, 700, (PFNNOTIFY)FleUpd_Afterpost, me->m_pParent);	
		}
		else {
			me->cSendSts = '1';			// ����һ��
			uf_RecLog(me->m_pParent,"send next leg 2");
	//		ISHELL_PostEvent( me->a.m_pIShell,me->a.clsID, FLDUPD_EVT_SEND,0, 0);  //�����¼�û��������
			FleUpd_Send(me);
		}
		break;
	case '0':
		me->cSendSts = '0';			// �ط�
	//	ISHELL_PostEvent( me->a.m_pIShell,me->a.clsID, FLDUPD_EVT_SEND,0, 0);  //�����¼�û��������
		FleUpd_Send(me);
		break;
	default:
		SPRINTF(sLogBuf,"Send file error, receive=%s",me->sBodyBuf);
		uf_RecLog(me->m_pParent,sLogBuf);
		me->cSendSts = 'A';	
		if ( (me->cType == '1') || (me->cType == '4') ) {		// 2012-10-08
			FleUpd_SendFileResult(me);
		}
		else
			ISHELL_SetTimer(me->a.m_pIShell, 700, (PFNNOTIFY)FleUpd_Afterpost, me->m_pParent);	
		break;
	}

}


// ==========================================================
static void FleUpd_Status(void *p, WebStatus ws, void *pVal)
{
	char lStr_Sts[51];
	FileUpload  *me;
	
	if (p == NULL)
		return;
	
	me = (FileUpload *)p;
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
		me->dwPostOvertime = GETTIMEMS();
		ISHELL_SetTimer(me->a.m_pIShell, 60 * 1000, (PFNNOTIFY)FleUpd_MonitorSendFile, me);
		STRCPY(lStr_Sts, "** sending...");
		break;
	case WEBS_READRESPONSE: 
		STRCPY(lStr_Sts, "** receiving...");
		break;
	case WEBS_GOTREDIRECT:
		STRCPY(lStr_Sts, "** redirect...");
		break;
	case WEBS_CACHEHIT:
		STRCPY(lStr_Sts, "** cache hit...");
		break;
	default:
		if (me->dwPostOvertime == 0) {			//  Add 2013-09-23
			me->dwPostOvertime = GETTIMEMS();														
			ISHELL_SetTimer(me->a.m_pIShell, 60 * 1000, (PFNNOTIFY)FleUpd_MonitorSendFile, me);		
		}		// Add End
		SPRINTF(lStr_Sts, "** other info=%d",ws);
		break;
	}

	uf_RecLog(me->m_pParent,lStr_Sts);
}

/********************************************************************************************
	����ļ������Ƿ�ʱ
**********************************************************************************************/
void FleUpd_MonitorSendFile(FileUpload* me)
{
	char sLogBuf[128];
	AECHAR szText[6];
    AECHAR szFailure[] = {0x4E2D,0x5FC3,0x65E0,0x5E94,0x7B54,0x0000};	// ������Ӧ��

	SPRINTF(sLogBuf,"FleUpd_MonitorSendFile in,dwPostOvertime=%d",me->dwPostOvertime);
	uf_RecLog(me->m_pParent,sLogBuf);

	if ( me->dwPostOvertime == 0)
		return;
	
	// �������60��û�յ�������֣������ϵ�
	WSTRCPY(szText,szFailure);

	if ( (me->cType == '1') || (me->cType == '4') ) {				// 2012-10-16
		 IDISPLAY_ClearScreen(me->a.m_pIDisplay);
		 IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
			 8, 2,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
		 IDISPLAY_Update(me->a.m_pIDisplay);

		 // ����ʧ�ܣ�������ʾ
		 ISHELL_Beep(me->a.m_pIShell, BEEP_MSG, TRUE);
		 ISHELL_Beep(me->a.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
		 ISHELL_SetTimer(me->a.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*)me->m_pParent );

		 ISHELL_SetTimer(me->a.m_pIShell, 55*100, (PFNNOTIFY)FleUpd_Afterpost, me->m_pParent);
	}
}



// ===================================================== //
// ����Ҫ�ϴ�����Ϣ
// ����������֮һ��2012091109121300531����ABCmp415***************
// ����: �ļ�����ʱ�� 2012-09-11 09:12:13��5��Uunicode�ַ�������ABC�����ļ�����ռ3���ֽڣ�mp4��ʽ, ����Ϣռ1���ֽڣ���1�Σ��ܹ�5�Σ� ***************�����ļ�������
// 
// ����������֮����2012091109121300031mp425***************
// ����: �ļ�����ʱ�� 2012-09-11 09:12:13��0��Uunicode�ַ����ļ�����ռ3���ֽڣ�mp4��ʽ, ����Ϣռ1���ֽڣ���2�Σ��ܹ�5�Σ� ***************�����ļ�������
// ===================================================== //
static void FleUpd_SetData(FileUpload* me)
{
	int32 iLen = 0;
	char sLogBuf[256];
	char sFormat[50];

	SPRINTF(sLogBuf,"FleUpd_SetData in,me->cSendSts=%d[%c]",me->cSendSts,me->cSendSts);
	uf_RecLog(me->m_pParent,sLogBuf);

	// ��һ�η���ʧ�ܣ��ط�
	if (me->cSendSts == '0') {		
		me->nState = SUCCESS;
		goto ExitHere;
	}
		
	// ���ͳɹ���������һ��
	if (me->cSendSts == '1') { // if -- 1

		FREEIF(me->pszPostData);
		me->iFileRead = 0;


// ���õ�1,2��������
		FleUpd_SetData01(me);

//  ���õ�3��������
		// Allocate enough memory to read the full text into memory
		if ( (uint32)(me->nHasRead+MAX_FILE_BYTES_POST) > me->oFileInfo.dwSize )
			iLen = me->oFileInfo.dwSize - me->nHasRead;
		else
			iLen = MAX_FILE_BYTES_POST;
		me->nPartNum += 1;
		me->pszPostData = REALLOC(me->pszPostData,(iLen+SEGMENT_LEN+SEGMENT_LEN)+me->iFileRead+1 );
		MEMSET(me->pszPostData+me->iFileRead,0,(iLen+SEGMENT_LEN+SEGMENT_LEN+1));
		MEMSET(sLogBuf,0,sizeof(sLogBuf));
		{
			char sTmp[5];
			SPRINTF(sTmp,"%c",me->pszPostData[18]);		// ��18Ϊ�ε��ֽ���
			STRCPY(sFormat,"%0");
			STRCAT(sFormat,sTmp);
			STRCAT(sFormat,"d%0");
			STRCAT(sFormat,sTmp);
			STRCAT(sFormat,"d");
		}
		SPRINTF(sLogBuf,sFormat,me->nPartNum,me->nPartsTotal);
        MEMCPY(me->pszPostData+me->iFileRead,sLogBuf,STRLEN(sLogBuf));
		me->iFileRead += STRLEN(sLogBuf);
																							// test by ---- 2012-09-21
																					// 		uf_RecLog(me->m_pParent,"test=");
																					// 		uf_RecLog(me->m_pParent,me->pszPostData);
																							// test end
		IFILE_Seek(me->pFileUpload,_SEEK_CURRENT ,0);
		iLen = IFILE_Read( me->pFileUpload, me->pszPostData+me->iFileRead, iLen );
		SPRINTF(sLogBuf,"FleUpd_SetData in, File Len=%d,Has Read=%d,IFILE_Read return=%d",
			             me->oFileInfo.dwSize,me->nHasRead,iLen);
		uf_RecLog(me->m_pParent,sLogBuf);

		if ( iLen <= 0 ) {
			FREEIF( me->pszPostData );
			me->iFileRead = 0;
			goto ExitHere;
		}
		 
		me->iFileRead += iLen;
		me->nHasRead += iLen;

		me->nState = SUCCESS;
		goto ExitHere;
	}	// if -- 1

	me->nState = EFAILED;

ExitHere:
	return;
}


// ===================================================== 
// �����ϴ��ĵ�1,2 ������
//   ��1��������
//       �ļ�����ʱ��(14)+Uunicode�ַ�������(3)+�ļ������ֽ���(1)+�ε��ֽ���(1) + CRC16У��ֵ�ֽ���(1)
//   ��2���֣��ַ�������+�ļ�����(�ļ���չ��)+CRCУ��ֵ 
//�ַ���������Uunicode�ַ�    
//�ַ���������Uunicode�ַ�����
//  ����������֮һ��20120911091213 005 3 1 ����ABC mp4
//  ���� �ļ�����ʱ�� 2012-09-11 09:12:13��5��Uunicode�ַ�������ABC�����ļ�����ռ3���ֽڣ�mp4��ʽ������Ϣռ1���ֽ�
//  ����������֮����2012091109121300031mp4
//  ���� �ļ�����ʱ�� 2012-09-11 09:12:13��0��Uunicode�ַ����ļ�����ռ3���ֽڣ�mp4��ʽ������Ϣռ1���ֽ�
// ===================================================== 
static void FleUpd_SetData01(FileUpload* me)
{
	char sLogBuf[128+TEXT_MAXSIZE*2];
	char sDateTime[15];						// �ļ��Ĵ���ʱ��
	char sExpName[10];						// �ļ�����չ��	
	char sFormat[50];
	int  nPartLen = 0;						// �ε��ֽ���
	int  nTextLen = 0;						// Uunicode�ַ����ֽ���
	int idx;


	SPRINTF(sLogBuf,"FleUpd_SetData01 in cType=%c,filename=%s",me->cType,me->oFileInfo.szName);
	uf_RecLog(me->m_pParent,sLogBuf);


	// �ļ�����ʱ��
	if (me->cType == '1')	// ��Ƶ�ļ�
		uf_GetDate(me->m_pParent,me->oFileInfo.dwCreationDate,sDateTime,sizeof(sDateTime),'2');
	else if (me->cType == '4')		// ��Ƭ
		uf_GetDate(me->m_pParent,me->oFileInfo.dwCreationDate,sDateTime,sizeof(sDateTime),'1');
	else if (me->cType == '5')		// ¼���ļ�	 Add by ---- 2014-04-10
		uf_GetDate(me->m_pParent,me->oFileInfo.dwCreationDate,sDateTime,sizeof(sDateTime),'1');
	else
		uf_GetDate(me->m_pParent,me->oFileInfo.dwCreationDate,sDateTime,sizeof(sDateTime),'0');
	
	idx = ud_rDistToChar(me->oFileInfo.szName,'.');
	if (idx == -1) {
		SPRINTF(sLogBuf,"ud_rDistToChar return -1, File[%s]",me->oFileInfo.szName);
		uf_RecLog(me->m_pParent,sLogBuf);
		me->nState = EFAILED;
		return;
	}
	// �ļ���չ��
	MEMSET(sExpName,0,sizeof(sExpName));
	SPRINTF(sExpName,"%s",(me->oFileInfo.szName+idx+1));

	// �ε��ֽ���
	SPRINTF(sLogBuf,"%d",me->nPartsTotal);
	nPartLen = STRLEN(sLogBuf);

	// Uunicode�ַ����ֽ���

	//ITEXTCTL_GetText(me->textCtl,me->pText,256);

	nTextLen = WSTRLEN(me->pText) * sizeof(AECHAR);

	// ��1��������  �ļ�����ʱ��(14)+Uunicode�ַ�������(3)+�ļ������ֽ���(1)+�ε��ֽ���(1)
	MEMSET(sLogBuf,0,sizeof(sLogBuf));
	MEMSET(sFormat,0,sizeof(sFormat));
//	SPRINTF(sLogBuf,"%d",SEGMENT_LEN);
	STRCPY(sFormat,"%s%03d%01d%01d");// STRCPY(sFormat,"%s3d%01d%01d");  
	SPRINTF(sLogBuf,sFormat,sDateTime,nTextLen,STRLEN(sExpName),nPartLen);//	SPRINTF(sLogBuf,"%s000%01d%0"SEGMENT_LENd"%s;",sDateTime,STRLEN(sExpName),nPartLen,sExpName);

	me->iFileRead = STRLEN(sLogBuf);
	uf_RecLog(me->m_pParent, sLogBuf);

	// ��2�������� �ַ�������+�ļ�����(�ļ���չ��) ,�ַ���������Uunicode�ַ�    
	MEMSET(sLogBuf+me->iFileRead,0,(nTextLen+1));
	MEMCPY(sLogBuf+me->iFileRead,me->pText,nTextLen);
	me->iFileRead += nTextLen;

    idx = STRLEN(sExpName);
	MEMSET(sLogBuf+me->iFileRead,0,(idx+1));
	MEMCPY(sLogBuf+me->iFileRead,sExpName,idx);
	me->iFileRead += idx;

// 	idx = 32;
// 	MEMSET(sLogBuf+me->iFileRead,0,(idx+1));
// 	MEMCPY(sLogBuf+me->iFileRead,me->m_pParent->s_MD5,idx);
// 	me->iFileRead += idx;
	
	me->pszPostData = MALLOC( me->iFileRead + 1 );
	MEMSET(me->pszPostData,0,me->iFileRead+1);
	MEMCPY(me->pszPostData,sLogBuf,me->iFileRead);


	nTextLen = me->iFileRead;
	{
		char sBuf[500];
		MEMSET(sBuf,0,sizeof(sBuf));
		SPRINTF(sBuf,"len=%d,  text AECHAR=",nTextLen);
		{
			int idx;
			for(idx=0;idx<nTextLen;idx++)
			{
				char cTmp = me->pszPostData[idx];
				char sTmp[5];
				SPRINTF(sTmp,"%d,",cTmp);
				STRCAT(sBuf,sTmp);
			}
		}
		uf_RecLog(me->m_pParent,sBuf);
	}



}


// ===================================================== 
// �ҳ�����������¼���ļ�
// ===================================================== 
void FleUpd_GetAudRec(FileUpload* me)
{
	newgimis *pMe = me->m_pParent;
	FileInfo* pFileInfo = NULL;	
	IFileMgr* pFileMgr = NULL;

	char sTmp[LOG_STR_LEN];
	char strRoot[512] = {0};
	int idx = 0;
	
	SPRINTF(strRoot, AEEFS_ROOT_DIR"%s", pMe->sAudRecPath);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_GetAudRec in, AUDIO RECORD PATH=%s",strRoot);
	
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_GetAudRec in, ISHELL_CreateInstance err");
		me->pAudRecRecordsA->nRecordCount = 0;		// �ֻ���û��¼���ļ�
		return;
	}
	
	// Initialize the file manager's file index in the application's root directory
	if ( IFILEMGR_EnumInit( pFileMgr, strRoot, FALSE ) )   {
		int iAccess;
		iAccess = IFILEMGR_CheckPathAccess(pFileMgr,strRoot,AEEFP_READ,NULL);
	}
	
	// Get each file name in the aplication's root directory.
	pFileInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
	if( pFileInfo )   {		// if -- pFileInfo
		
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_GetAudRec, pFileInfo 10");
		idx = 0;
		
		while( IFILEMGR_EnumNext( pFileMgr, pFileInfo ) )
		{  // while -- 1
			FileInfo * pInfo;	
			if ( (STRCMP(pMe->sMobileType,"33") == 0) ) {
				MEMSET(sTmp,0,sizeof(sTmp));
				STRCPY(sTmp,pFileInfo->szName);
				STRLOWER(sTmp);
				if ( STRENDS( "sys", sTmp ) ) continue;
			}
//			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_GetAudRec, pFileInfo 11");

			pInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
			if ( IFILEMGR_GetInfo(pFileMgr, pFileInfo->szName, pInfo) == SUCCESS )  {

				if (pInfo->dwCreationDate != 0) {
					int nRtn = 0;
					char sDateTime[15] = {0};  // �ļ��Ĵ���ʱ��, yyyymmddhhmmss
					uf_GetDate(me->m_pParent, pFileInfo->dwCreationDate,sDateTime,sizeof(sDateTime),'1');
					nRtn = MEMCMP(sDateTime+2,me->pAudRecRecordsA->sDate1,sizeof(me->pAudRecRecordsA->sDate1));
					if (nRtn <0 ) {
// 						ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_GetAudRec, FileName=%s,CreationDate=%s,nRtn=%d"
// 							,pInfo->szName,sDateTime,nRtn);
						continue;
					}
					nRtn = MEMCMP(sDateTime+2,me->pAudRecRecordsA->sDate2,sizeof(me->pAudRecRecordsA->sDate2));
					if (nRtn >0 ) {
// 						ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_GetAudRec_1, FileName=%s,CreationDate=%s,nRtn=%d"
// 							,pInfo->szName,sDateTime,nRtn);
						continue;
					}
				
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_GetAudRec, save FileName=%s,CreationDate=%s",pInfo->szName,sDateTime);
					FleUpd_SaveAudRec(me,pInfo->szName);
					idx++;
				}
			}
			
			FREEIF( pInfo );
		}  // while -- 1
		
		if ( idx > 0 ) { 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_GetAudRec,Find [%d] Audio Record File",idx);
			me->pAudRecRecordsA->cStatus = 1; 
		}	
		else 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"can not Find Audio Record File,nSelRec=%d,nRecordCount=%d",me->pAudRecRecordsA->nSelRec,me->pAudRecRecordsA->nRecordCount );
		
		FREEIF( pFileInfo );
		
	}	// if -- pFileInfo
	
	IFILEMGR_Release( pFileMgr );
}

// ===================================================== 
// ������һ��¼���ļ�
// ===================================================== 
void FleUpd_SendNextAudioRec(FileUpload* me)
{
	newgimis *pMe = me->m_pParent;
	
	int idx = me->pAudRecRecordsA->nSelRec;

	me->pAudRecRecordsA->pAudRecFieldA[idx].cSendStatus = 10;
	if ( (idx+1) >= me->pAudRecRecordsA->nRecordCount )  {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_SendNextAudioRec in,send audio recorder file finish,nSelRec=%d,nRecordCount=%d",idx,me->pAudRecRecordsA->nRecordCount);
		FleUpd_ReleaseRec(me);
		return;		// �����ļ�
	}

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_SendNextAudioRec in,nSelRec=%d,nRecordCount=%d",idx,me->pAudRecRecordsA->nRecordCount);

	// ����һ���ļ�
	ISHELL_SetTimer(me->a.m_pIShell, 90 * 1000, (PFNNOTIFY)FleUpd_Upload, me);
	return;

}

