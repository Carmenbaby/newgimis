/*===============================================================================
 处理有关上传文件
=============================================================================== */

#include "newgimis.bid"
#include "newgimis.h"

#include "gm_FileUpload.h"
#include "gmTools.h"

#include "MD5.h"

#define SEGMENT_LEN		1			// 表述“段的长度”需要的字节数


/*-------------------------------------------------------------------
Static function prototypes
-------------------------------------------------------------------*/
// 用http post 发送文件相关的函数
static void FleUpd_Start(FileUpload* me);
static void FleUpd_GotResp(void *p);		// 应答http返回
//static void FleUpd_Header(void *p, const char *cpszName, GetLine *pglVal);
static void FleUpd_Status(void *p, WebStatus ws, void *pVal);
static void FleUpd_ReadBody(void *p);
// 初始化
static void FleUpd_Init(FileUpload* me);
// 初始化IAudRecRecordsA(上传录音文件)
static boolean FleUpd_InitAudRec(FileUpload* me,char *sParam);			// Add by ---- 2014-04-10

// 得到视频目录下的视频信息
static void FleUpd_GetVideo(FileUpload* me);
// 删除已经传送的视频文件(或照片文件)
static void FleUpd_DelFile(FileUpload* me);

// 启动上传文件
static void FleUpd_Upload(FileUpload* me);
// 设置要上传的信息
static void FleUpd_SetData(FileUpload* me);
// 设置上传的第1部分内容
static void FleUpd_SetData01(FileUpload* me);
// 退出本功能
static void FleUpd_Close(newgimis *pMe);		// FileUpload* me);

// 正在上传文件
static void FleUpd_Send(FileUpload* me);

/*-------------------------------------------------------------------
function prototypes
-------------------------------------------------------------------*/
// 监控文件发送是否超时
void FleUpd_MonitorSendFile(FileUpload *me);  // uf_MonitorSendFile(FileUpload *me);
// 传送完成后的动作
void FleUpd_Afterpost(newgimis *pMe);
// 显示上传文件的结果
void FleUpd_SendFileResult(FileUpload *me);
// 显示上传文件的结果 上传过程中失败的结果
void FleUpd_SendFileResult_error(FileUpload *me);

// =================== 有关上传录音文件 =================================
// 释放IAudRecRecordsA中资源
void FleUpd_ReleaseRec(FileUpload* me);
// 初始化IAudRecRecordsA(上传录音文件)，设置参数
boolean FleUpd_InitAudRec(FileUpload* me,char *sParam);
// 把符合条件的录音文件保存到 IAudRecRecordsA 中
void FleUpd_SaveAudRec(FileUpload* me,char* sFileName);
// 找出符合条件的录音文件
void FleUpd_GetAudRec(FileUpload* me);
// 发送下一个录音文件
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
				FleUpd_Init(me);			// ---- 初始化，转到后台运行
				if ( me->nState != SUCCESS )
					bRtn = FALSE;
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"shang chuan jin lai 4");
			}
			else if (me->pAudRecRecordsA != NULL) {  // -0					// Add by ---- 2014-04-10
				// 如果正在上传录音文件，就不能传其它文件
				if (me->cType != '5')  {
					// 显示图片
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

				if (wParam == 1)  {		// 取得上传录音文件的日期范围
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
		// 估计是 struct _FileUpload 中含有  cgimis	*m_pParent，造成某种困扰，所以只能是关闭程序时才能退出
		// 但在星历下载中也有这个变量，不出问题。有机会仔细研究.    2012-10-26 漏了个break, 两次了！！！！
		FleUpd_Close(pMe);
		
		me->m_pParent = 0;
		FREEIF(pMe->FleUpdApp);
        break;
    case FLDUPD_EVT_UPLOAD:		
		me->cType = (char)wParam;			// 传入文件的类型  52 --- '4'

	//	FleUpd_HandleEvent in FLDUPD_EVT_START
		FleUpd_Upload(me);
		return TRUE; 
	case FLDUPD_EVT_SEND:
//		uf_RecLog(me->m_pParent,"发送下一段 进来");
		uf_RecLog(me->m_pParent,"send the next leg come");
		FleUpd_Send(me);
		return TRUE; 

    default:
		break;
    }
	
    return(FALSE);
}

/********************************************************************************************
// 传送完成后的动作
// 传入参数
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

// 	pMe->FleUpdApp = 0;			// 这两行很重要，不能忘了
//  	me->m_pParent = 0;
//  	FREEIF(me);
   
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Close out");

	return;
}


/********************************************************************************************
// 传送完成后的动作
// 传入参数
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

// Mark 2014-04-10 应该有错，注释
//	if (me->pFileUpload != NULL) {
//		ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Afterpost in,pFileUpload!=NULL");
//		IFILE_Release(me->pFileUpload);
//		me->pFileUpload = NULL;
//	}

	if (me->cType == '1') {
		// 发送视频文件已处理完
		pMe->iPressSts = 0;
		IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
		IDISPLAY_Update(pMe->a.m_pIDisplay);

		pMe->m_bSendPF = FALSE;
//		uf_RecLog(pMe,"返回主菜单");
		pMe->m_WitchPag = 10;
		testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );  //返回到主界面

		// 原来是在后台，回到后台
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
//		uf_RecLog(pMe,"返回主菜单");
		pMe->m_WitchPag = 10;
		testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );  //返回到主界面
	}

	else if (me->cType == '3') {		// add by 2015-9-14
		uf_Afterpost(pMe);
		pMe->m_bSendPF = FALSE;
//		uf_RecLog(pMe,"返回次菜单");
		pMe->m_WitchPag = 4;
		testgps_GotoScreen( pMe, SCREENID_MORE, 0 );  //返回到主界面
	}

	// Add by ---- 2014-04-10
	FleUpd_Close(pMe);		
	// Add End

// 2014-04-10 根据FLDUPD_EVT_STOP记录，好像有问题，很早就注释了	ISHELL_PostEvent( pMe->theApp.m_pIShell,pMe->theApp.clsID, FLDUPD_EVT_STOP,0, 0); 

	return;
}
/********************************************************************************************
// 初始化
// 传入参数
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

	nErr = ISHELL_CreateInstance(me->a.m_pIShell, AEECLSID_WEB, (void**)&me->pWeb);			// ---- Web接口
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
// 初始化IAudRecRecordsA(上传录音文件)，设置参数
// 传入参数
//    char *sParam : 上传时间范围内录音文件,开始时间(yymmddhhmm,年月日时分),结束时间(yymmddhhmm)。 例如： 1404100800,1404101500, 表示 14-04-10 08:00 到 14-04-10 15:00的录音文件
// 传出参数
//   TRUE : 设置完成
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
// 释放IAudRecRecordsA中资源
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
// 把符合条件的录音文件保存到 IAudRecRecordsA 中
// 传入参数：
//  char sFileName : 录音文件名(含路径)
// return :  0: 保存成功， 1: 失败
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
// 删除已经传送的视频文件(或照片文件)
// =============================================================
static void FleUpd_DelFile(FileUpload* me)
{
	newgimis *pMe = me->m_pParent;
	 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_DelFile in");

    if ( STRLEN(me->sFile) <= 0 )	// 不存在文件
		return;

	if (me->pFileUpload != NULL) {
		IFILE_Release(me->pFileUpload);
		me->pFileUpload = NULL;
	}
	
	if ( IFILEMGR_Remove(me->pFileMgr,me->sFile) == SUCCESS )
		MEMSET(me->sFile,0,sizeof(me->sFile));
}

// =====================================================================
// 得到视频目录下的相片信息
// =====================================================================
static void FleUpd_GetVideo(FileUpload* me)
{

  FileInfo oFileInfo;	
  char    *pszTok = NULL;
  char    *pszBuf = NULL;

  char sFileName[MAX_FILE_NAME];				// 视频文件名
  uint32 dwDateTime;							// 最新的视频文件的生成时间
  char strRoot[512];
  newgimis *pMe = me->m_pParent;

  char sBufLog[126];
  int idx;

  me->nState = EFAILED;
  dwDateTime = 0;

   // 从配置文件中读出手机视频文件存放路径
   SPRINTF(strRoot, AEEFS_ROOT_DIR"%s", me->m_pParent->sVideoPath);
   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"video_path=%s",strRoot);

   // 查找最新的视频文件
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

	// 设置要发送的视频文件（含路径）
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
// 打开要上传的文件
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

	me->cSendSts = 'A';					// 其它任意的值
	me->nHasRead = 0;
	me->nPartNum = 0;					// 正在传第几段，1 第1段，2 第2段，。。。。。  
    me->nPartsTotal = 0;
	MEMSET(me->pText,0,(TEXT_MAXSIZE+1)*sizeof(AECHAR));	

	switch(me->cType)
	{
	case '1':				// 找到要发的视频文件
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
	case '4':						// 2012-10-16   4: 照片
		{
		   STRCPY(me->sFile,me->m_pParent->sFileName);		// 2012-10-16  sFileName要改成含路径
		   //  [3/28/2016 Administrator]
		   //ITEXTCTL_GetText(me->m_pParent->textCtl,me->m_pParent->pText,256);
		   WSTRCPY(me->pText,me->m_pParent->pText);
		}
		break;
	case '5':			// Add by ---- 2014-04-10
		{
			int idx = me->pAudRecRecordsA->nSelRec;
			// 枚举录音文件路径，把符合条件的文件保存在上传结构中
			if (me->pAudRecRecordsA->cStatus == 1) { // 1: 收到新的上传指令
				  FleUpd_GetAudRec(me);	
				  me->pAudRecRecordsA->cStatus = 0;	
			}

			if (idx >= me->pAudRecRecordsA->nRecordCount) {
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_OpenFile in,send audio recorder file finish");
				return;	// 已无文件
			}

			if (me->pAudRecRecordsA->pAudRecFieldA[idx].cSendStatus == 10) {
				me->pAudRecRecordsA->nSelRec++;
				idx = me->pAudRecRecordsA->nSelRec;
			}

			if (me->pAudRecRecordsA->pAudRecFieldA[idx].cSendStatus != 0) {
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_OpenFile in,cSendStatus[%d]=%d",idx,me->pAudRecRecordsA->pAudRecFieldA[idx].cSendStatus);
				return;	// 文件状态不处于未打开状态
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
	me->nPartsTotal = (uint8)(me->oFileInfo.dwSize / MAX_FILE_BYTES_POST);				// 总段数
	if ( (me->oFileInfo.dwSize % MAX_FILE_BYTES_POST) != 0 ) {
		me->nPartsTotal += 1;
	}
	
	SPRINTF(sLogBuf,"FleUpd_OpenFile,MAX_FILE_BYTES_POST=%d,oFileInfo.dwSize =%u, nPartsTotal=%d",MAX_FILE_BYTES_POST,me->oFileInfo.dwSize,me->nPartsTotal);
	uf_RecLog(me->m_pParent,sLogBuf);

	me->nState = SUCCESS;

	// Add by ---- 2014-04-10
	if (me->cType == '5') 
		me->pAudRecRecordsA->pAudRecFieldA[me->pAudRecRecordsA->nSelRec].cSendStatus = 1;	// 打开发送文件
	// Add End


ExitHere:
	
	SPRINTF(sLogBuf,"FleUpd_OpenFile out nState=%d",me->nState);
	uf_RecLog(me->m_pParent,sLogBuf);
	
	return;

}
/********************************************************************************************
// 开始上传文件
********************************************************************************************/
static void FleUpd_Upload(FileUpload* me)
{
	newgimis *pMe;
	char sFormat[250];
	int idx;

	
	//-----------有关MD5校验
	FileInfo pFileInfo ;
	MD5_CTX md5;
	unsigned char *file_buf = NULL;	//文件buf
	unsigned char uc_md5[16];
	unsigned char PADDING[]={0x80,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
							0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	//--------------end

	pMe = me->m_pParent;

	uf_RecLog(me->m_pParent,"FleUpd_Upload in");
	// 设置发送的http地址
	MEMSET(sFormat,0,sizeof(sFormat));
    idx = ud_DistToChar(me->m_pParent->strCenterIp,'/');  
	if ( idx >= 0 )
		MEMCPY(sFormat, me->m_pParent->strCenterIp, idx); 
	else
		STRCPY(sFormat, me->m_pParent->strCenterIp);
	if ( (me->cType == '2') || (me->cType == '3') )		// 2013-09-01 日志都传到授权中心
	    SPRINTF(me->pURL,"http://www.richtalk.cn/FileUploadServlet");		//  61.131.1.153:8218		www.richtalk.cn
	else
		SPRINTF(me->pURL,"http://%s/FileUploadServlet",sFormat); /* http://zc.gota.richmap.cn/FileUploadServlet */
		//SPRINTF(me->pURL,"http://61.131.1.153:10018/partition-web/FileUploadServlet");//王辉
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
		// 前台显示“发送中。。。。。。”图片
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
		//------------------取MD5校验值
		IFILE_GetInfo( me->pFileUpload, &pFileInfo );
		if (pFileInfo.dwSize > 0 )  // 说明还可以继续保存数据
		{
			file_buf = (unsigned char *)MALLOC(pFileInfo.dwSize + 1);
			MEMSET(file_buf,0,pFileInfo.dwSize + 1);
			IFILE_Seek(me->pFileUpload,_SEEK_START,0);  
			IFILE_Read( me->pFileUpload, file_buf, pFileInfo.dwSize );
			
			//--wh测试 --文本字符串测试admin
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
			IFILE_Seek(me->pFileUpload,_SEEK_START,0); //要弄回开头。 
			
			//校验值得到后需要放入上传地址  &encode=
			STRCAT(me->pURL,"&encode=");
			STRCAT(me->pURL,pMe->s_MD5);
		}
	}//---------------------end

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_Upload in url=%s",me->pURL);

	FleUpd_Send(me);
}

/********************************************************************************************
// 开始上传文件
********************************************************************************************/
static void FleUpd_Send(FileUpload* me)
{

	// 设置发送数据
	FleUpd_SetData(me);
	if (me->nState != SUCCESS)
		return;
	

	// 开始Web请求
	FleUpd_Start(me);

}

// ============================================
// 用post方法发送数据
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


/*********************** 应答http返回 *******************************************************/
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
	if (!WEB_ERROR_SUCCEEDED(pInfo->nCode)) {  // 2014-04-10 在return前加上条件

		FleUpd_SendFileResult_error(me); // 显示失败的界面
		me->m_pParent->m_bSendPF = FALSE; //设置发送照片为空闲状态

   	   ISHELL_SetTimer(me->a.m_pIShell, 3000, (PFNNOTIFY)FleUpd_Afterpost, me->m_pParent);
		return;
	}


	if(0 != pInfo->pisMessage) 
	{
		SPRINTF(sLogBuf,"FleUpd_GotResp,Got Message, time:%u",(GETTIMEMS()-me->dwPostOvertime));
		uf_RecLog(me->m_pParent, sLogBuf);
		if(0 == pInfo->lContentLength) 
			return; 	      
		
		me->dwPostOvertime = 0;				// 已收到应答

		me->pSource = pInfo->pisMessage;
		ISOURCE_AddRef(me->pSource);				// ---- 在Read Body中用到
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
// 显示上传文件的结果  通信故障的结果
// ==========================================================
void FleUpd_SendFileResult_error(FileUpload *me)
{
	// 显示发送结果信息 
	// \u53d1\u9001\u5931\u8d25\uff0c\u624b\u673a\u6b20\u8d39\u6216\u6545\u969c  发送失败，手机欠费或故障
	AECHAR szFailure[] = {0x53d1,0x9001,0x5931,0x8d25,0xff0c,0x624b,0x673a,0x6b20,0x8d39,0x6216,0x6545,0x969c,0x0000};	// 发送失败
//	AECHAR szNoFile[] = {0x5DF2,0x65E0,0x89C6,0x9891,0x6587,0x4EF6,0x0000}; // 已无视频文件 \uFEFF\u5DF2\u65E0\u89C6\u9891\u6587\u4EF6

				// 显示传视频文件成功 
	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1,    // -1 = 所有字符串的都表示
	8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(me->a.m_pIDisplay);
	return;
}

// ==========================================================
// 显示上传文件的结果
// ==========================================================
void FleUpd_SendFileResult(FileUpload *me)
{
	// 显示发送结果信息
	AECHAR szSuccess[] = {0x53D1,0x9001,0x6210,0x529F,0000};	// 发送成功  \uFEFF\u53D1\u9001\u6210\u529F
	AECHAR szFailure[] = {0x53D1,0x9001,0x5931,0x8D25,0x0000};	// 发送失败
//	AECHAR szNoFile[] = {0x5DF2,0x65E0,0x89C6,0x9891,0x6587,0x4EF6,0x0000}; // 已无视频文件 \uFEFF\u5DF2\u65E0\u89C6\u9891\u6587\u4EF6

				// 显示传视频文件成功 
	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	if ( me->cSendSts == '1' )
		IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szSuccess, -1,    // -1 = 所有字符串的都表示
					8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	else              // 其它情况  if ( me->cSendSts == 'A' ) 
		IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1,    // -1 = 所有字符串的都表示
		8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);

	IDISPLAY_Update(me->a.m_pIDisplay);
				
	ISHELL_Beep(me->a.m_pIShell, BEEP_MSG, TRUE);
	ISHELL_Beep(me->a.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
	ISHELL_SetTimer(me->a.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*) me->m_pParent);
	ISHELL_SetTimer(me->a.m_pIShell, 55 * 100, (PFNNOTIFY)FleUpd_Afterpost, me->m_pParent);

	return;
}

// ==========================================================
// 读取中心端返回的信息
// ==========================================================
static void FleUpd_ReadBody(void *p)
{

	FileUpload* me = (FileUpload* )p;
	int32 nRead;
	char sBodybuf[10];			// 中心端只应答‘1’，‘0’， 不要太长
	char sLogBuf[256];

	uf_RecLog(me->m_pParent,"FleUpd_ReadBody in");
	

	MEMSET(sBodybuf,0,sizeof(sBodybuf));
	nRead = ISOURCE_Read(me->pSource, sBodybuf, sizeof(sBodybuf)-1 );
	if(ISOURCE_WAIT == nRead) 
	{
		ISOURCE_Readable(me->pSource, &me->webcb);
		return;
	}
	
	if (nRead>0)		// ---- 读到数据
	{
		int iLen = (nRead>sizeof(me->sBodyBuf))?sizeof(me->sBodyBuf):nRead;
		if (me->nBytes == 0)
		{
			MEMSET(me->sBodyBuf,0,sizeof(me->sBodyBuf));
			MEMCPY(me->sBodyBuf,sBodybuf,iLen);
		}
// 只要第1个字符 		else 
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

	me->cSendSts =  ( STRLEN(me->sBodyBuf) > 1 ) ? 'E' : me->sBodyBuf[0];		// 'E' 代表出错了
	me->nBytes = 0;

	SPRINTF(sLogBuf,"FleUpd_ReadBody, me->cSendSts=%c",me->cSendSts);
	uf_RecLog(me->m_pParent,sLogBuf);

	// 根据返回的结果处理
	switch (me->cSendSts)
	{
	case '1':
		if (me->nPartNum == me->nPartsTotal) {	// 已经完成
			uf_RecLog(me->m_pParent,"Send file finish");
			if ( (me->cType == '1') || (me->cType == '4') )  {		// 2012-10-08
				// 删除传送成功的文件
				FleUpd_DelFile(me);
				FleUpd_SendFileResult(me);
			}
			///// add by 2015-9-14  start  这个不能再这里删除    因为一天只有最新的一次才被保存  ///////  
			else if (me->cType == '2')  // 说明是发送的是配置文件
			{
//				FleUpd_DelFile(me);
//				FleUpd_SendFileResult(me);
			}

			else if (me->cType == '3')  // 说明是日志文件  删除日志文件  之所以分开 可能到时候不需要删除
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
			me->cSendSts = '1';			// 发下一段
			uf_RecLog(me->m_pParent,"send next leg 2");
	//		ISHELL_PostEvent( me->a.m_pIShell,me->a.clsID, FLDUPD_EVT_SEND,0, 0);  //发送事件没有起到作用
			FleUpd_Send(me);
		}
		break;
	case '0':
		me->cSendSts = '0';			// 重发
	//	ISHELL_PostEvent( me->a.m_pIShell,me->a.clsID, FLDUPD_EVT_SEND,0, 0);  //发送事件没有起到作用
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
	监控文件发送是否超时
**********************************************************************************************/
void FleUpd_MonitorSendFile(FileUpload* me)
{
	char sLogBuf[128];
	AECHAR szText[6];
    AECHAR szFailure[] = {0x4E2D,0x5FC3,0x65E0,0x5E94,0x7B54,0x0000};	// 中心无应答

	SPRINTF(sLogBuf,"FleUpd_MonitorSendFile in,dwPostOvertime=%d",me->dwPostOvertime);
	uf_RecLog(me->m_pParent,sLogBuf);

	if ( me->dwPostOvertime == 0)
		return;
	
	// 如果超过60秒没收到，则出现，主动断掉
	WSTRCPY(szText,szFailure);

	if ( (me->cType == '1') || (me->cType == '4') ) {				// 2012-10-16
		 IDISPLAY_ClearScreen(me->a.m_pIDisplay);
		 IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
			 8, 2,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
		 IDISPLAY_Update(me->a.m_pIDisplay);

		 // 发送失败，振铃提示
		 ISHELL_Beep(me->a.m_pIShell, BEEP_MSG, TRUE);
		 ISHELL_Beep(me->a.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
		 ISHELL_SetTimer(me->a.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*)me->m_pParent );

		 ISHELL_SetTimer(me->a.m_pIShell, 55*100, (PFNNOTIFY)FleUpd_Afterpost, me->m_pParent);
	}
}



// ===================================================== //
// 设置要上传的信息
// 数据流举例之一：2012091109121300531福州ABCmp415***************
// 代表: 文件创建时间 2012-09-11 09:12:13，5个Uunicode字符“福州ABC”，文件类型占3个字节，mp4格式, 段信息占1个字节，第1段，总共5段， ***************代表文件的内容
// 
// 数据流举例之二：2012091109121300031mp425***************
// 代表: 文件创建时间 2012-09-11 09:12:13，0个Uunicode字符，文件类型占3个字节，mp4格式, 段信息占1个字节，第2段，总共5段， ***************代表文件的内容
// ===================================================== //
static void FleUpd_SetData(FileUpload* me)
{
	int32 iLen = 0;
	char sLogBuf[256];
	char sFormat[50];

	SPRINTF(sLogBuf,"FleUpd_SetData in,me->cSendSts=%d[%c]",me->cSendSts,me->cSendSts);
	uf_RecLog(me->m_pParent,sLogBuf);

	// 上一段发送失败，重发
	if (me->cSendSts == '0') {		
		me->nState = SUCCESS;
		goto ExitHere;
	}
		
	// 发送成功，继续下一段
	if (me->cSendSts == '1') { // if -- 1

		FREEIF(me->pszPostData);
		me->iFileRead = 0;


// 设置第1,2部分内容
		FleUpd_SetData01(me);

//  设置第3部分内容
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
			SPRINTF(sTmp,"%c",me->pszPostData[18]);		// 第18为段的字节数
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
// 设置上传的第1,2 部内容
//   第1部分内容
//       文件创建时间(14)+Uunicode字符串长度(3)+文件类型字节数(1)+段的字节数(1) + CRC16校验值字节数(1)
//   第2部分：字符串内容+文件类型(文件扩展名)+CRC校验值 
//字符串内容是Uunicode字符    
//字符串长度是Uunicode字符长度
//  数据流举例之一：20120911091213 005 3 1 福州ABC mp4
//  代表： 文件创建时间 2012-09-11 09:12:13，5个Uunicode字符“福州ABC”，文件类型占3个字节，mp4格式，段信息占1个字节
//  数据流举例之二：2012091109121300031mp4
//  代表： 文件创建时间 2012-09-11 09:12:13，0个Uunicode字符，文件类型占3个字节，mp4格式，段信息占1个字节
// ===================================================== 
static void FleUpd_SetData01(FileUpload* me)
{
	char sLogBuf[128+TEXT_MAXSIZE*2];
	char sDateTime[15];						// 文件的创建时间
	char sExpName[10];						// 文件的扩展名	
	char sFormat[50];
	int  nPartLen = 0;						// 段的字节数
	int  nTextLen = 0;						// Uunicode字符串字节数
	int idx;


	SPRINTF(sLogBuf,"FleUpd_SetData01 in cType=%c,filename=%s",me->cType,me->oFileInfo.szName);
	uf_RecLog(me->m_pParent,sLogBuf);


	// 文件创建时间
	if (me->cType == '1')	// 视频文件
		uf_GetDate(me->m_pParent,me->oFileInfo.dwCreationDate,sDateTime,sizeof(sDateTime),'2');
	else if (me->cType == '4')		// 照片
		uf_GetDate(me->m_pParent,me->oFileInfo.dwCreationDate,sDateTime,sizeof(sDateTime),'1');
	else if (me->cType == '5')		// 录音文件	 Add by ---- 2014-04-10
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
	// 文件扩展名
	MEMSET(sExpName,0,sizeof(sExpName));
	SPRINTF(sExpName,"%s",(me->oFileInfo.szName+idx+1));

	// 段的字节数
	SPRINTF(sLogBuf,"%d",me->nPartsTotal);
	nPartLen = STRLEN(sLogBuf);

	// Uunicode字符串字节数

	//ITEXTCTL_GetText(me->textCtl,me->pText,256);

	nTextLen = WSTRLEN(me->pText) * sizeof(AECHAR);

	// 第1部分内容  文件创建时间(14)+Uunicode字符串长度(3)+文件类型字节数(1)+段的字节数(1)
	MEMSET(sLogBuf,0,sizeof(sLogBuf));
	MEMSET(sFormat,0,sizeof(sFormat));
//	SPRINTF(sLogBuf,"%d",SEGMENT_LEN);
	STRCPY(sFormat,"%s%03d%01d%01d");// STRCPY(sFormat,"%s3d%01d%01d");  
	SPRINTF(sLogBuf,sFormat,sDateTime,nTextLen,STRLEN(sExpName),nPartLen);//	SPRINTF(sLogBuf,"%s000%01d%0"SEGMENT_LENd"%s;",sDateTime,STRLEN(sExpName),nPartLen,sExpName);

	me->iFileRead = STRLEN(sLogBuf);
	uf_RecLog(me->m_pParent, sLogBuf);

	// 第2部分内容 字符串内容+文件类型(文件扩展名) ,字符串内容是Uunicode字符    
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
// 找出符合条件的录音文件
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
		me->pAudRecRecordsA->nRecordCount = 0;		// 手机中没有录音文件
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
					char sDateTime[15] = {0};  // 文件的创建时间, yyyymmddhhmmss
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
// 发送下一个录音文件
// ===================================================== 
void FleUpd_SendNextAudioRec(FileUpload* me)
{
	newgimis *pMe = me->m_pParent;
	
	int idx = me->pAudRecRecordsA->nSelRec;

	me->pAudRecRecordsA->pAudRecFieldA[idx].cSendStatus = 10;
	if ( (idx+1) >= me->pAudRecRecordsA->nRecordCount )  {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_SendNextAudioRec in,send audio recorder file finish,nSelRec=%d,nRecordCount=%d",idx,me->pAudRecRecordsA->nRecordCount);
		FleUpd_ReleaseRec(me);
		return;		// 已无文件
	}

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"FleUpd_SendNextAudioRec in,nSelRec=%d,nRecordCount=%d",idx,me->pAudRecRecordsA->nRecordCount);

	// 传下一个文件
	ISHELL_SetTimer(me->a.m_pIShell, 90 * 1000, (PFNNOTIFY)FleUpd_Upload, me);
	return;

}

