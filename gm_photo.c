/*===============================================================================
 处理有关相片
=============================================================================== */

#include "newgimis.bid"
#include "newgimis.h"

#include "gm_FileUpload.h"  // Add 2012-09-21
#include "gmTools.h"


// 发送中...
void Send_ing(newgimis *me);

// 回调函数显示图像
void uf_ImageNotify(newgimis * pMe,IImage *pIImage, AEEImageInfo * pi, int nErr);


/*===========================================================================
// 2013-05-09
// 设置照片的根路径
 =========================================================================== */
#define photo_SetRootDir(pMe,strRoot) {	\
	if ( (STRCMP(pMe->sMobileType,"16") == 0) ) {	\
		char sDate[11] = {0};						\
		JulianType dDate;							\
		int nLen;									\
		GETJULIANDATE(0,&dDate);					\
			SPRINTF(sDate,"%04d-%02d-%02d",dDate.wYear,dDate.wMonth,dDate.wDay);	\
		nLen = STRLEN(strRoot);														\
		if (strRoot[nLen-1] == '/')													\
			SPRINTF(strRoot, "%s%s/", strRoot,sDate);								\
		else																		\
			SPRINTF(strRoot, "%s/%s/", strRoot,sDate);								\
	}																				\
}																					




// ==========================================================
// 显示 发送中..
// ==========================================================
void Send_ing(newgimis *me)
{
	// 显示发送结果信息  发送中...
	// \u53d1\u9001\u4e2d\u2e\u2e\u2e
	AECHAR szFailure[] = {0x53d1,0x9001,0x4e2d,0x2e,0x2e,0x2e,0x0000};	// 发送成功


	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1,    // -1 = 所有字符串的都表示
	8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(me->a.m_pIDisplay);
	return;
}






// Add by ---- 2012-05-29
/*===========================================================================
// 在发送文件时，在屏幕显示图片“发送中。。”
===========================================================================*/
void uf_ShowSendPic(newgimis *pMe)
{
	IImage * pi;
	char sFileSending[MAX_FILE_NAME];		// 正在发生图片

	if (pMe->iPressSts == 26)	{		
			// 已无文件要发送
		SPRINTF(sFileSending,"%s", IMG_FILE_YWWJ);
	}	
	else if (pMe->iPressSts == 25)	{	// 显示信息图片(发送中)							
	    SPRINTF(sFileSending,"%s",IMG_FILE_SENDING);
	}
	else if (pMe->iPressSts == 27) {
		SPRINTF(sFileSending,"%s",IMG_FILE_SCLYWJ);
	}
	else
		return;
	pi = ISHELL_LoadImage(pMe->a.m_pIShell, sFileSending);
	IImage_Notify(pi, (PFNIMAGEINFO)uf_DrawSendPic,pMe);  //注册回调

}

/*===========================================================================
说明
   在屏幕显示图片
=========================================================================== */
void uf_DrawSendPic(newgimis *pMe,IImage *pIImage, AEEImageInfo * pi, int nErr )
{
	int            x;
	int            y;
	
	AEERect  pRect;
	SETAEERECT(&pRect, 0, 0,  pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen);
	
	// Do not display if image does not fit in the allocated rectangle.
	if (pi->cx > pMe->DeviceInfo.cxScreen || pi->cy > pMe->DeviceInfo.cyScreen) {
		IIMAGE_Release(pIImage); 
		return;
	}
	
	x = (pMe->DeviceInfo.cxScreen / 2) - (pi->cxFrame / 2);
	y = (pMe->DeviceInfo.cyScreen / 2) - (pi->cy / 2);
	
	//   IIMAGE_Start(pImage, x, y);
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IImage_Draw(pIImage,x,y); 
	IDISPLAY_Update(pMe->a.m_pIDisplay); 
	IIMAGE_Release(pIImage); 
	pIImage = NULL;	
}
// Add End

// =====================================================================
// 显示屏幕上显示当前输入法信息
// =====================================================================
void uf_ShowInputMode(newgimis * pMe, AEETextInputMode eInputMode)
{

   char  szBuf[10];
   AECHAR szText[10];

   DBGPRINTF("-----ShowInputMode");

   if ( (STRCMP(pMe->sMobileType,"68") != 0) )
	   return;

   switch(eInputMode)
   {
	case AEE_TM_PINYIN:
		DBGPRINTF("-----AEE_TM_PINYIN");
		ISHELL_LoadResString(pMe->a.m_pIShell,NEWGIMIS_RES_FILE,IDS_TM_PINYIN,szText,sizeof(szText));
		break;
	case AEE_TM_NUMBERS:
		DBGPRINTF("-----AEE_TM_LETTERS");
		ISHELL_LoadResString(pMe->a.m_pIShell,NEWGIMIS_RES_FILE,IDS_TM_NUMBER,szText,sizeof(szText));
		break;
	case AEE_TM_LETTERS:
	case AEE_TM_EZTEXT:
	case 86:
		DBGPRINTF("-----AEE_TM_LETTERS");
		ISHELL_LoadResString(pMe->a.m_pIShell,NEWGIMIS_RES_FILE,IDS_TM_LETTERS,szText,sizeof(szText));
		break;
	case AEE_TM_STROKE:
		DBGPRINTF("-----AEE_TM_STROKE");
		ISHELL_LoadResString(pMe->a.m_pIShell,NEWGIMIS_RES_FILE,IDS_TM_STROKE,szText,sizeof(szText));
		break;
	case 85:
		DBGPRINTF("-----AEE_TM_EZTEXT");
		ISHELL_LoadResString(pMe->a.m_pIShell,NEWGIMIS_RES_FILE,IDS_TM_ENGLISH,szText,sizeof(szText));
		break;
	default:
		SPRINTF(szBuf,"%d",eInputMode);
		DBGPRINTF("-----InputMode=%d",eInputMode);
        STRTOWSTR((char*)szBuf, szText, sizeof(szText));
		break;
   }

	gimis_WPrintf(pMe,14,2,AEE_FONT_BOLD,IDF_ALIGN_LEFT | IDF_RECT_FILL,szText);

}

// =====================================================================
// 得到相片目录下的相片信息
// =====================================================================
void uf_GetPhote(newgimis * pMe)
{
   FileInfo* pFileInfo = NULL;	

   IFileMgr* pFileMgr = NULL;
   char* pzFileName = NULL;				// 系统照片目录下文件名

   char lStr_Log[LOG_STR_LEN];
   char strRoot[512];
   char strDateTime[15];
   char strNewFileName[15+4+4];		// 待上传的文件名：p+yyyymmddhhmmss+999.jpg
   int idx;

   // Add by ---- 2012-05-29  
   char strCopyDate[15];		// yyyymmddhhmmss   保存最新一张照片的日期
   MEMSET(strCopyDate,0,sizeof(strCopyDate));
   // Add End

   // QCHAT	 AEEFS_ROOT_DIR"md/__Images/"	
   MEMSET(strRoot,0,sizeof(strRoot));
 
   SPRINTF(strRoot, AEEFS_ROOT_DIR"%s", pMe->sPhotoPath);		//   SPRINTF(strRoot,AEEFS_ROOT_DIR"root/photo/gallery/");
   photo_SetRootDir(pMe,strRoot);	// Add 2013-05-09
   uf_RecLog(pMe,"photo_path=");
   uf_RecLog(pMe,strRoot);

  // Create the text buffers on the heap.
  pzFileName = (char*) MALLOC( MAX_FILE_NAME * sizeof( char ) );

  if( !pzFileName)
  {
    FREEIF( pzFileName );
    return;
  }

  // Create an IFileMgr object.
  if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
  {
    FREEIF( pzFileName );
    return;
  }

	DBGPRINTF( "-----photo 3" );

  // Initialize the file manager's file index in the application's root directory
  if ( IFILEMGR_EnumInit( pFileMgr, strRoot, FALSE ) )
  {
	  int iAccess;
		 DBGPRINTF( "-----pFileInfo 9");
		 iAccess = IFILEMGR_CheckPathAccess(pFileMgr,strRoot,AEEFP_READ,NULL);
		 SPRINTF(lStr_Log,"-----access=%d",iAccess);
		 DBGPRINTF(lStr_Log);	 
		 uf_RecLog(pMe,lStr_Log);
  }

  // Get each file name in the aplication's root directory.
  pFileInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
  
  if( pFileInfo )
  {
	  
	 DBGPRINTF( "-----pFileInfo 10");
	 uf_RecLog(pMe,"-----pFileInfo 10");
	 idx = 0;

    while( IFILEMGR_EnumNext( pFileMgr, pFileInfo ) )
    {
		char *psTmp = NULL;

	   DBGPRINTF( "-----pFileInfo 11");
        uf_RecLog(pMe,"-----pFileInfo 11");

	  // 根据文件的后缀判断是否为照片	
	  // Change by ---- 2011-10-20, 把文件名转为小写来比较，适配华为
		// if( STRENDS( PHOTO_FILE_EXTENSION, pFileInfo->szName ) ) 
		psTmp = (char *) MALLOC( STRLEN( pFileInfo->szName )+1 );
        MEMSET(psTmp,0,STRLEN( pFileInfo->szName )+1);
		STRCPY(psTmp,pFileInfo->szName);
		STRLOWER(psTmp);
        uf_RecLog(pMe,psTmp);
      if( STRENDS( PHOTO_FILE_EXTENSION, psTmp ) )   {
		   FileInfo * pInfo;	

		   DBGPRINTF( "-----pFileInfo 12");
		   uf_RecLog(pMe,"-----pFileInfo 12");
	       pInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );

		  if ( IFILEMGR_GetInfo(pFileMgr, pFileInfo->szName, pInfo) == SUCCESS )
		  {
			  if (pInfo->dwCreationDate == 0)  continue;

			DBGPRINTF( "-----FileName0 = %s", pInfo->szName);
			DBGPRINTF( "-----dwCreationDate0 = %d", pInfo->dwCreationDate);
			SPRINTF(lStr_Log, "-----FileName0=%s,CreationDate0=%d",pInfo->szName,pInfo->dwCreationDate);
			uf_RecLog(pMe,lStr_Log);
			uf_GetDate(pMe,pInfo->dwCreationDate,strDateTime,sizeof(strDateTime),'1');

			if (pMe->cPhotoSend == '1')	
			{	// Add by ---- 2012-05-29
				if ( STRCMP(strDateTime,strCopyDate) > 0 ) {
					STRCPY(strCopyDate,strDateTime);
					STRCPY( pzFileName, pFileInfo->szName );
				}
			}	// Add by ---- 2012-05-29
			else {
				SPRINTF(strNewFileName,"p%s.jpg",strDateTime);	//		  SPRINTF(strNewFileName,"p%s%03d.jpg",strDateTime,++idx);
				DBGPRINTF( "-----photoname = %s", strNewFileName);
				SPRINTF(lStr_Log, "-----photoname = %s", strNewFileName);
				uf_RecLog(pMe,lStr_Log);
 				STRCPY( pzFileName, pFileInfo->szName );
				uf_CopyFile(pMe,pFileMgr,pzFileName,strNewFileName);	// 在viewSonic机型下不能用此语句复制 IFILEMGR_Rename(pFileMgr,pzFileName,strNewFileName);
			}

		  }

  	     FREEIF( pInfo );

      }
	  if (psTmp != NULL)	FREEIF(psTmp);

    }  // while -- 1

	// Add by ---- 2012-05-29, 只把最新的照片复制到工作目录
	if (pMe->cPhotoSend == '1') {
		if ( STRLEN(strCopyDate) > 0 ) {
			SPRINTF(strNewFileName,"p%s.jpg",strCopyDate);
			uf_CopyFile(pMe,pFileMgr,pzFileName,strNewFileName);
		}
	}
	// Add End


  }
  IFILEMGR_Release( pFileMgr );
  FREEIF( pFileInfo );
  FREEIF( pzFileName );
	
}

// =============================================================
// 复制文件
// 传入参数
// IFileMgr* pFileMgr  文件接口
// char *pStr_SrcName  源文件名
// char *pStr_DstName  目标文件名
// =============================================================
void uf_CopyFile(newgimis * pMe,IFileMgr* pFileMgr,char *pStr_SrcName,char *pStr_DstName)
{

	IFile	*psrcIFile = NULL;		// 源文件
	IFile	*pdstIFile = NULL;		// 目标文件
    FileInfo fiInfo;		// 源文件的信息
	int32 iRtn = 0;		// 0 复制失败, 1 复制成功

	// 打开源文件
	if (IFILEMGR_Test(pFileMgr, pStr_SrcName) == SUCCESS)
		psrcIFile = IFILEMGR_OpenFile(pFileMgr, pStr_SrcName, _OFM_READ);
	else
		goto ExitHere;

	if ( SUCCESS != IFILE_GetInfo( psrcIFile, &fiInfo ) )
		goto ExitHere;

    if ( fiInfo.dwSize == 0 )
		goto ExitHere;


	// 产生目标文件
	if (IFILEMGR_Test(pFileMgr, pStr_DstName) == SUCCESS) {
		pdstIFile = IFILEMGR_OpenFile(pFileMgr, pStr_DstName, _OFM_READWRITE);
		IFILE_Truncate(pdstIFile, 0);
	}
	else
		pdstIFile = IFILEMGR_OpenFile(pFileMgr, pStr_DstName, _OFM_CREATE);

    if (!pMe->pszPostData) {
		FREE(pMe->pszPostData);
		pMe->pszPostData = NULL;
    }

	// Allocate enough memory to read the full text into memory
	pMe->pszPostData = MALLOC( fiInfo.dwSize + 1 );
	MEMSET(pMe->pszPostData,0,(fiInfo.dwSize + 1));

	IFILE_Seek(psrcIFile,_SEEK_CURRENT ,0);
    iRtn = IFILE_Read( psrcIFile, pMe->pszPostData, fiInfo.dwSize );    
	if ((uint32)iRtn != fiInfo.dwSize) 	{
		iRtn = 0;
		goto ExitHere;
	}

	if ( IFILE_Write(pdstIFile, pMe->pszPostData, iRtn ) > 0 ) {
		
		iRtn = 1;
	}

ExitHere:

	if (psrcIFile != NULL)
		IFILE_Release( psrcIFile );

	if (iRtn == 1) 
	  IFILEMGR_Remove(pFileMgr,pStr_SrcName);

	if (pdstIFile != NULL)
		IFILE_Release( pdstIFile );

    if (!pMe->pszPostData) {
		FREE(pMe->pszPostData);
		pMe->pszPostData = NULL;
    }
	
}
// =============================================================
// 删除指定照片
// =============================================================
void uf_DelPhotoFile(newgimis *pMe)
{
	IFileMgr* pFileMgr = NULL;

    if ( STRLEN(pMe->sFileName) <= 0 )	// 不存在照片
		return;

	// Create an IFileMgr object.
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
		return;

	if ( IFILEMGR_Remove(pFileMgr,pMe->sFileName) == SUCCESS ) {
		MEMSET(pMe->sFileName,0,sizeof(pMe->sFileName));
		pMe->imgidx = ( pMe->imgidx > 0 ) ? pMe->imgidx-1:0;
	}

	if (pFileMgr)
	{
		IFILEMGR_Release(pFileMgr);
		pFileMgr = NULL;
	}
}

// ===============================================================
// 找到立即发送的那张照片, 找到后放在pMe->imgidx中, 如果是立即发送照片则立即发送
// ===============================================================
void uf_GetNewestPhotoForSend(newgimis * pMe)
{
   FileInfo* pFileInfo = NULL;	
   IFileMgr* pFileMgr = NULL;
   char strCopyDate[15];		// yyyymmddhhmmss   保存最新一张照片的日期
   char strDateTime[15];
   char strRoot[512];
   int idx;

   char lStr_Log[LOG_STR_LEN];

//    if (pMe->cPhotoSend != '1')	
// 	   return;
   
   MEMSET(strCopyDate,0,sizeof(strCopyDate));
   MEMSET(strRoot,0,sizeof(strRoot));			
   SPRINTF(strRoot, AEEFS_ROOT_DIR"%s", pMe->sPhotoPath);
   photo_SetRootDir(pMe,strRoot);	// Add 2013-05-09
   SPRINTF(lStr_Log,"uf_GetNewestPhotoForSend in, photo_path=%s",strRoot);
   uf_RecLog(pMe,lStr_Log);

  if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
  {
	uf_RecLog(pMe, "ISHELL_CreateInstance err");
	pMe->imgidx = 0;		// 手机中没有照片
    return;
  }

  // Initialize the file manager's file index in the application's root directory
  if ( IFILEMGR_EnumInit( pFileMgr, strRoot, FALSE ) )   {  //初始化 IFILEMGR接口
	  int iAccess;
	  iAccess = IFILEMGR_CheckPathAccess(pFileMgr,strRoot,AEEFP_READ,NULL);
	  SPRINTF(lStr_Log,"-----access=%d",iAccess);
	  DBGPRINTF(lStr_Log);	 
	  uf_RecLog(pMe,lStr_Log);
  }

   MEMSET(pMe->sFileName,0,sizeof(pMe->sFileName));

  // Get each file name in the aplication's root directory.
  pFileInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
  if( pFileInfo )   {		// if -- pFileInfo
	  
	 uf_RecLog(pMe,"-----pFileInfo 10");
	 idx = 0;

    while( IFILEMGR_EnumNext( pFileMgr, pFileInfo ) )
    {
		FileInfo * pInfo;	
		if ( (STRCMP(pMe->sMobileType,"33") == 0) ) {
			MEMSET(lStr_Log,0,STRLEN( pFileInfo->szName )+1);
			STRCPY(lStr_Log,pFileInfo->szName);
			STRLOWER(lStr_Log);
			if ( STRENDS( "sys", lStr_Log ) )
				continue;
		}

        uf_RecLog(pMe,"-----pFileInfo 11111");
	    pInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
		if ( IFILEMGR_GetInfo(pFileMgr, pFileInfo->szName, pInfo) == SUCCESS )  {
			if (pInfo->dwCreationDate != 0) {
				idx++;
				
				SPRINTF(lStr_Log, "-----FileName0=%s,CreationDate0=%d",pInfo->szName,pInfo->dwCreationDate);
				uf_RecLog(pMe,lStr_Log);
				uf_GetDate(pMe,pInfo->dwCreationDate,strDateTime,sizeof(strDateTime),'1');
				
				if ( STRCMP(strDateTime,strCopyDate) > 0 ) {
					STRCPY(strCopyDate,strDateTime);
					STRCPY( pMe->sFileName, pFileInfo->szName );
					pMe->imgidx = idx;
				}
			}
		}

  	    FREEIF( pInfo );
    }  // while -- 1
	
	if ( STRLEN(strCopyDate) > 0 ) {
		uf_RecLog(pMe,"Find Send photo" );
		if (pMe->cPhotoSend == '1') {
			pMe->pi = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);
			if (pMe->pi) 		{
				IImage_Notify(pMe->pi, (PFNIMAGEINFO)uf_ImageNotify,pMe);  //注册回调
			}
		}
	}
	else {
		uf_RecLog(pMe,"can not Find Send photo" );
		pMe->imgidx = 0;		   // 手机中没有照片
		uf_AEPrint(pMe,FALSE);		// 一张都没有没有
	}

	FREEIF( pFileInfo );

  }	// if -- pFileInfo

  IFILEMGR_Release( pFileMgr );

}

// ==================================================================
// 1.把照片显示在屏幕上，供用户确认
//  2012-10-16  改为直接在照片原来的目录上读取照片
// ==================================================================
void uf_SetPhote(newgimis * pMe)
{
    char strRoot[512];				// 2012-10-16

	FileInfo* pFileInfo = NULL;	
	IFileMgr* pFileMgr = NULL;
	int iCurImg = 0;		// 当前浏览的图片
	char lStr_Log[LOG_STR_LEN];

	SPRINTF(lStr_Log,"_SetPhote iPressSts=%d,iShowText=%d,iSendPhoto=%d,pi=%d",pMe->iPressSts,pMe->iShowText,pMe->iSendPhoto,pMe->pi);
	uf_RecLog(pMe,lStr_Log);

	// 可能正在处理图像，需等待释放资源
	if (pMe->iPressSts != 14)
		return;

	// 资源还未释放，退出
	if (pMe->pi) 	return;

	// Add by ---- 2012-10-16
	MEMSET(strRoot,0,sizeof(strRoot));
	SPRINTF(strRoot, AEEFS_ROOT_DIR"%s", pMe->sPhotoPath);
	photo_SetRootDir(pMe,strRoot);	// Add 2013-05-09
	SPRINTF(lStr_Log,"PhotoPath=%s",strRoot);
	uf_RecLog(pMe,lStr_Log);
	// Add End

	DBGPRINTF( "-----photo 10" );
	pMe->iPressSts = 15;	// 表示正在处理照片
	
//
//	if (pMe->cPhotoSend == '1')	 {	// 立即发送照片, 在找到时就发送了; 不是立即发送，则首先
  	   uf_GetNewestPhotoForSend(pMe);
	   return;
//	}
//

	if (pMe->cPhotoSend == '1')	 {	// 立即发送照片, 在找到时就发送了; 不是立即发送，则首先
  	   uf_GetNewestPhotoForSend(pMe);
	   return;
	}

  // Create an IFileMgr object.	pMe->theApp.m_pIShell
  if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
  {
	pMe->iPressSts = 14;
    return;
  }

  // Change by ---- 2012-10-16
  // IFILEMGR_EnumInit( pFileMgr, "", FALSE );
  if ( IFILEMGR_EnumInit( pFileMgr, strRoot, FALSE ) )  {
	  int iAccess;
	  iAccess = IFILEMGR_CheckPathAccess(pFileMgr,strRoot,AEEFP_READ,NULL);
	  SPRINTF(lStr_Log,"-----access=%d",iAccess);
	  DBGPRINTF(lStr_Log);	 
	  uf_RecLog(pMe,lStr_Log);
  }
  // Change End 2012-10-16	

  // Get each file name in the aplication's root directory.
  pFileInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
  
  if( pFileInfo )
  {

	DBGPRINTF( "-----photo 4" );

	while( IFILEMGR_EnumNext( pFileMgr, pFileInfo ) )
    {	// while -- 1

		if ( (STRCMP(pMe->sMobileType,"33") == 0) ) {
			MEMSET(lStr_Log,0,STRLEN( pFileInfo->szName )+1);
			STRCPY(lStr_Log,pFileInfo->szName);
			STRLOWER(lStr_Log);
			if ( STRENDS( "sys", lStr_Log ) )
				continue;
		}

//lStr_Log
			iCurImg++;
			if (iCurImg == pMe->imgidx)		// 找到要显示的照片
			{	// if -- 1
				pMe->pi = ISHELL_LoadImage(pMe->a.m_pIShell, pFileInfo->szName);
				DBGPRINTF( "-----photo 6 = %s", pFileInfo->szName);
				MEMSET(pMe->sFileName,0,sizeof(pMe->sFileName));
			    STRCPY(pMe->sFileName,pFileInfo->szName);

				if (pMe->pi)
				{
					DBGPRINTF( "-----photo 7" );
					IImage_Notify(pMe->pi, (PFNIMAGEINFO)uf_ImageNotify,pMe);  //注册回调
					break;
				}
			}	// if -- 1

	}	// while -- 1

	// 没有找到合适的照片
	if (iCurImg != pMe->imgidx)
	{

		DBGPRINTF( "-----photo 11" );
		if (pMe->pi)
		{
			IIMAGE_Release(pMe->pi);	// 放在此处释放
			pMe->pi = NULL;
			DBGPRINTF( "-----photo 12" );
		}
		pMe->iPressSts = 14;

		if ( iCurImg == 0) {
			DBGPRINTF( "-----photo 20" );
			uf_AEPrint(pMe,FALSE);		// 一张都没有没有
		}
		else
		{
			uf_AEPrint(pMe,TRUE);
			DBGPRINTF( "-----photo 21" );
		}
	}

	DBGPRINTF( "-----photo 22" );
	FREEIF( pFileInfo );

  }

  IFILEMGR_Release( pFileMgr );

}


// =============================================================
// 显示文本框和选择照片菜单
// =============================================================
void uf_ShowText(newgimis * pMe)
{
	AEERect rect;

    if ( (STRCMP(pMe->sMobileType,"01") == 0) /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/)
	{
		rect.x = 0;
		rect.y = pMe->DeviceInfo.cyScreen - 160;
		rect.dy = 30;
		rect.dx = pMe->DeviceInfo.cxScreen - 25;
	}
	else {
		rect.x = 10;
		rect.dy = 30;
		rect.y = pMe->iPhotoHeight + 5;
		rect.dx = pMe->DeviceInfo.cxScreen - 20;
	}

	DBGPRINTF( "-----text 00" );
	if ( (pMe->textCtl) && (pMe->textmenu) )	{		// if -- 0
		DBGPRINTF( "-----text01=%d",pMe->iShowText );
		switch  (pMe->iShowText) {		// case 
			case  1:
				uf_ReleaseCtl(pMe,FALSE);
				pMe->iShowText = 0;
				break;
			case 2:
			case 3:							// Add 2011-09-28
				{
					DBGPRINTF( "-----text iShowText == 2" );
					
					if ( (STRCMP(pMe->sMobileType,"01") == 0) /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/)  {      // viewSonic需如此
						ITEXTCTL_SetText(pMe->textCtl, pMe->pText, 0);
						ITEXTCTL_Redraw(pMe->textCtl);
						
						ITEXTCTL_SetInputMode(pMe->textCtl, AEE_TM_PINYIN);			// 拼音
						ITEXTCTL_SetActive(pMe->textCtl,TRUE);
						ITEXTCTL_SetCursorPos(pMe->textCtl,TC_CURSOREND);
					}
					else {     
						
						if( ITEXTCTL_IsActive( pMe->textCtl ) ) {
							ITEXTCTL_Redraw( pMe->textCtl );
							DBGPRINTF( "-----ITEXTCTL_Redraw" );
						}
						ITEXTCTL_SetActive( pMe->textCtl, TRUE );  /* Also causes a menu draw */
						ITEXTCTL_SetCursorPos(pMe->textCtl,TC_CURSOREND);
						DBGPRINTF( "-----SetActive 09" );
						
						uf_TextModechange(pMe);
					}
					
					pMe->iShowText = 0;
					return;
				}
				break;
		}	// case 
	}	// if -- 0
	
	//create the text pointer
	if (!pMe->textCtl) 
		if (ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_TEXTCTL,(void**)&pMe->textCtl) != SUCCESS) 
			return;
	
	DBGPRINTF( "-----text 20" );
	if (!pMe->textmenu)
		if (ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOFTKEYCTL,(void**)&pMe->textmenu) != SUCCESS)
			return;

	IMENUCTL_SetTitle(pMe->textmenu,NEWGIMIS_RES_FILE,ITEXT_TITLE,NULL);
	IMENUCTL_AddItem(pMe->textmenu,NEWGIMIS_RES_FILE,IMENU_WRITE,Menu_write,NULL,0);
	IMENUCTL_AddItem(pMe->textmenu,NEWGIMIS_RES_FILE,IMENU_RIGHT,Menu_right,NULL,0);
	IMENUCTL_AddItem(pMe->textmenu,NEWGIMIS_RES_FILE,IMENU_LEFT,Menu_left,NULL,0);
	IMENUCTL_AddItem(pMe->textmenu,NEWGIMIS_RES_FILE,IMENU_DEL,Menu_del,NULL,0);
	IMENUCTL_AddItem(pMe->textmenu,NEWGIMIS_RES_FILE,IMENU_RTN,Menu_rtn,NULL,0);

	// G680需要切换到输入框的菜单项
	if ( (STRCMP(pMe->sMobileType,"68") == 0) )
	{
		IMENUCTL_AddItem(pMe->textmenu,NEWGIMIS_RES_FILE,IDS_INPUT,Menu_input,NULL,0);
	}

    //设置属性,有边框
	ITEXTCTL_SetProperties(pMe->textCtl,TP_FRAME);
	//设置显示矩形区域
	ITEXTCTL_SetRect(pMe->textCtl, &rect);
	ITEXTCTL_SetMaxSize(pMe->textCtl, TEXT_MAXSIZE);
	ITEXTCTL_SetInputMode(pMe->textCtl, AEE_TM_PINYIN);			// 拼音
	DBGPRINTF( "-----SetInputMode 0" );

	ITEXTCTL_SetSoftKeyMenu(pMe->textCtl, pMe->textmenu);

	ITEXTCTL_SetText(pMe->textCtl, pMe->pText, 0);
	ITEXTCTL_Redraw(pMe->textCtl);

	// 激活控件
	DBGPRINTF( "-----text 21" );
	ITEXTCTL_SetActive(pMe->textCtl,TRUE);
	ITEXTCTL_SetCursorPos(pMe->textCtl,TC_CURSOREND);

	uf_ShowInputMode(pMe,AEE_TM_PINYIN);

}

// 释放与文本输入有关的控件
// bClean  true 清屏
void uf_ReleaseCtl(newgimis * pMe,boolean bClean)
{
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"uf_ReleaseCtl in");
	if (pMe->textCtl) {
		ITEXTCTL_Release(pMe->textCtl);
		pMe->textCtl = NULL;
	}

	if (pMe->textmenu) {
		IMENUCTL_Release(pMe->textmenu);
		pMe->textmenu = NULL;
	}

	// 清屏
	if (bClean) {
		IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );		
//		gimis_Printf( pMe, 0, 0, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_RECT_FILL, "GiMis" );
		IDISPLAY_Update(pMe->a.m_pIDisplay);
		MEMSET(pMe->pText,0,(TEXT_MAXSIZE+1)*sizeof(AECHAR));
	}

}




// ==================================================================
// 得到日期时间 yyyymmddhhmmss
//  dwCreationDate 从January 6, 1980 00:00:00 GMT开始经过的时间, UTC时间
//  strDateTime 转换后的时间
//  iStrLen : strDateTime的长度
// Change by ---- 2012-09-21
//   cType  :  取出时间修正的方式, '0' 不做时间修正  '1': 取个位+十位数的值, '2': 取百位+千位数的值
// void uf_GetDate(cgimis * pMe,uint32 dwCreationDate,char* strDateTime,int iStrLen,char cType)
// ==================================================================
void uf_GetDate(newgimis * pMe,uint32 dwCreationDate,char* strDateTime,int iStrLen,char cType)
{
	JulianType dDate;				// 记录收到照片的时间
	{
      int nHours;
	  if (NUM_VERSION >= 2050900 )
	  {
		  switch (cType)
		  {
		  case '0':
			  nHours = 0;
			  break;
		  case '2':				//  取视频文件等的时间修正
			  nHours = pMe->iAddHour / 100;
			  break;
		  default:		//  '1' 作为缺省值  case '1':				// 取照片文件等的时间修正
			  nHours = pMe->iAddHour % 100;	
			  break;
		  }
	  }
	  dwCreationDate += (nHours*3600);
	}

	// 记录收到定位数据的时间
//  系统当前时间 	GETJULIANDATE(0,&pDate);		//	取出日期	
	GETJULIANDATE(dwCreationDate,&dDate);		//	取出日期	
	MEMSET(strDateTime,0,iStrLen);
	SPRINTF(strDateTime,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);

}

// ================================================== //
// 显示汉字
// boolean bCallBack  TRUE 回调, FALSE 不用回调
// ================================================== //
void uf_AEPrint(newgimis * pMe,boolean bCallBack)
{
	AECHAR szText[] = {0x5DF2,0x65E0,0x7167,0x7247,0x8981,0x663E,0x793A,0x0000}; // L"已无照片要显示";
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	pMe->imgidx = ( pMe->imgidx <= 1)?1 : --pMe->imgidx;
	DBGPRINTF( "-----imgidx = %d",pMe->imgidx );

	if (bCallBack)
		ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_SetPhote, pMe);
	else {
		DBGPRINTF( "-----err c221");
		ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_Afterpost, pMe);	// 返回到定位模块
	}
}

// 照片处理结束后启动清屏
void uf_Afterpost(newgimis *pMe)
{
	char lStr_Log[LOG_STR_LEN];
	SPRINTF(lStr_Log,"-----Afterpost,iSendPhoto=%d,iPressSts=%d",pMe->iSendPhoto,pMe->iPressSts);
	uf_RecLog(pMe,lStr_Log);

	pMe->imgidx = 0;
	pMe->iShowText = 0;

	// 发送照片已处理完
	pMe->iPressSts = 0;
	pMe->iSendPhoto = 0;
	uf_ReleaseCtl(pMe,TRUE);
	// 原来是在后台，回到后台
/*
	if (pMe->m_bOldGoBg) {
		pMe->m_bGoBg = TRUE;
		ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE);
	}
*/
}


///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////

/***************************************************************************
SamplePosDet_SendPhoto_HandleEvent 发送照片
****************************************************************************/
boolean SamplePosDet_SendPhoto_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam )
{
	AEERect rRect;
	CtlAddItem rMenuItem;
//	AECHAR PP[20];
	// cUsePhoto 启用 视频和图片功能
	/*
	0 : 不启用照片和视频上传功能  
	1 ：启用照片 
	2 ：启用照片一键上传 
	4 : 发送视频，不发送照片 
	5： 发送视频，发送照片 
	7:  发送视频，照片一键上传 
	*/


   int  m_nFontHeight = 0;
	
   boolean bHandled = FALSE;
   IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );

	char s_buf[100];//  测试用

	if(pMe->m_bSendPF == TRUE )//用来标记是否正在发送图片的标志位 TRUE -- 表示正在发送状态  FALSE -- 表示空闲
	{
		// 说明正在发送照片 
		return TRUE;
	}

	if((pMe->textCtl!=NULL) && 
      (ITEXTCTL_IsActive( pMe->textCtl))&&
      ITEXTCTL_HandleEvent( pMe->textCtl, eCode, wParam, dwParam ))
	{
		ITEXTCTL_Redraw( pMe->textCtl );
		   ITEXTCTL_GetText(pMe->textCtl,pMe->pText,100);
		   WSTRTOSTR(pMe->pText,s_buf,100);
		   uf_RecLog(pMe,s_buf );
		   IDISPLAY_Update(pMe->a.m_pIDisplay); 

		   return TRUE;
	}

	

   switch( eCode ) {
   case EVT_SCREEN:
      if( wParam == SCREEN_PARAM_INIT ) 
	  {
         if( pMenuCtl ) {  /* This is not expected to happen */
            testgps_DrawScreen( pMe, 0 );
            bHandled = TRUE;
         }
         else {
            if( (ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&pMenuCtl ) == SUCCESS) && 
				(ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_STATIC, (void**)( &pMe->m_PhotoTextCtl )) == SUCCESS)) {
				IMENUCTL_SetProperties( pMenuCtl, MP_MAXSOFTKEYITEMS | MP_NO_ARROWS );
				m_nFontHeight = IDISPLAY_GetFontMetrics( pMe->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL );
				rMenuItem.wFont = AEE_FONT_NORMAL;

				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_FRONT_PIC,
                  0, NULL, (uint32)pMenuCtl );

				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_SEND,
                  1, NULL, (uint32)pMenuCtl );

				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_NEXT_PIC,
                  2, NULL, (uint32)pMenuCtl );


//				SETAEERECT(&rRect, 0, 0, pMe->DeviceInfo.cxScreen, m_nFontHeight+10);
				SETAEERECT(&rRect, 0, 0, pMe->DeviceInfo.cxScreen, pMe->m_SYSFBH);
				ISTATIC_SetRect(pMe->m_PhotoTextCtl, &rRect);
//				STR_TO_WSTR("1我1",PP,20);
//				ISTATIC_SetText( pMe->m_PhotoTextCtl, NULL, PP, AEE_FONT_NORMAL, AEE_FONT_NORMAL );

				  // Set the dimensions and placement of the menu control

	//			  SETAEERECT(&rRect, 0, pMe->DeviceInfo.cyScreen - m_nFontHeight-10, pMe->DeviceInfo.cxScreen, m_nFontHeight+10);
				SETAEERECT(&rRect, 0, pMe->DeviceInfo.cyScreen - pMe->m_MenuH, pMe->DeviceInfo.cxScreen, pMe->m_MenuH);

				 // SETAEERECT( &rRect, 0, ( pMe->m_rScreenRect.dy + pApp->m_rScreenRect.y ) - MAIN_MENU_HEIGHT, pApp->m_rScreenRect.dx, MAIN_MENU_HEIGHT );
				  IMENUCTL_SetRect( pMenuCtl, &rRect );

				  // Activate the control and draw it on the screen

				IMENUCTL_SetActive( pMenuCtl, TRUE );

				testgps_SetScreenData( pMe, (void *)pMenuCtl );


				///////////
				if (pMe->cUsePhoto == '1' || pMe->cUsePhoto == '5')
				{
//					if (pMe->m_T_L >  7)
//					{
					
						if (!pMe->textCtl) 
						{
								if (ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_TEXTCTL,(void**)&pMe->textCtl) == SUCCESS)
								{
									ITEXTCTL_SetMaxSize(pMe->textCtl, TEXT_MAXSIZE);
									//设置属性,有边框
									ITEXTCTL_SetProperties(pMe->textCtl,TP_FRAME);
									
									//设置显示矩形区域
						 //			SETAEERECT(&rRect, 0, 30+pMe->iPhotoHeight, pMe->DeviceInfo.cxScreen, 50);  // m_nFontHeight+10
									SETAEERECT(&rRect, 0, (2 * pMe->m_SYSFBH) + pMe->m_PhotoH + 10, pMe->DeviceInfo.cxScreen, 2 * pMe->m_SYSFBH);  // m_nFontHeight+10
									ITEXTCTL_SetRect(pMe->textCtl, &rRect);
									
									ITEXTCTL_SetInputMode(pMe->textCtl, AEE_TM_PINYIN);			// 拼音
						/*			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "创建文本框 1= %d",rRect.x);
									ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "创建文本框2 = %d",rRect.y);
									ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "创建文本框3 = %d",rRect.dx);
									ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "创建文本框4 = %d",rRect.dy);
						*/
									uf_RecLog(pMe,"create text contrl" );

							//		ITEXTCTL_SetSoftKeyMenu(pMe->textCtl, pMe->textmenu);

						//			ITEXTCTL_SetText(pMe->textCtl, pMe->pText, 0);
									ITEXTCTL_Redraw(pMe->textCtl);

									// 激活控件
									DBGPRINTF( "-----text 21" );
									ITEXTCTL_SetActive(pMe->textCtl,TRUE);
						//			ITEXTCTL_SetCursorPos(pMe->textCtl,TC_CURSOREND);
								}
						}
//					}
				}
				///////////
				IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
				IDISPLAY_Update (pMe->a.m_pIDisplay);

               
               testgps_DrawScreen( pMe, 0 );
               bHandled = TRUE;
            }
         }
      }
      else if( wParam == SCREEN_PARAM_CLOSE ) {
         IMENUCTL_Release( pMenuCtl );
		 if (pMe->textCtl)
		 {
			 ITEXTCTL_Release(pMe->textCtl);
		 }
		 ISTATIC_Release( pMe->m_PhotoTextCtl );
         testgps_SetScreenData( pMe, 0 );

         pMe->currentHandler = 0;
		 pMe->m_PhotoTextCtl = NULL;
		 pMe->textCtl = NULL;
		 pMenuCtl = NULL;
         bHandled = TRUE;
      }
      else if( wParam == SCREEN_PARAM_PAINT )
	  {
		  if (pMe->cUsePhoto == '1' || pMe->cUsePhoto == '5')
		  {
			  ITEXTCTL_SetActive(pMe->textCtl,TRUE);// by wh [2/18/2016 Administrator]
			  
			  if( IMENUCTL_IsActive( pMenuCtl ) ) 
			  {
				  ISTATIC_Redraw(pMe->m_PhotoTextCtl);
				  if (pMe->cUsePhoto == '1' || pMe->cUsePhoto == '5')
				  {
					  ITEXTCTL_Redraw(pMe->textCtl);
				  }
				  IMENUCTL_Redraw( pMenuCtl );
				  IMENUCTL_SetActive( pMenuCtl, FALSE );
			  }
			  else 
			  {
				  IMENUCTL_SetActive( pMenuCtl, FALSE );
				  //IMENUCTL_SetActive( pMenuCtl, TRUE );  /* Also causes a menu draw */
				  IMENUCTL_Redraw( pMenuCtl );  // 这句后面加的
			  }
		  }
		  else
		  {
			  IMENUCTL_SetActive( pMenuCtl, TRUE );
			  IMENUCTL_Redraw( pMenuCtl );  
		  }
	
      }
      break;

   case EVT_KEY:
	   {
		   if (pMe->cUsePhoto == '1' || pMe->cUsePhoto == '5')
		   {
			   if(wParam == AVK_SELECT)
			   {
				   if (ITEXTCTL_IsActive( pMe->textCtl))
				   {
					   ITEXTCTL_SetActive( pMe->textCtl,FALSE);
					   IMENUCTL_SetActive(pMenuCtl,TRUE);
					   return TRUE;
				   }
			   }
		   }

	   
		   if (pMe->cUsePhoto == '1' || pMe->cUsePhoto == '5') // 说明有照片的备注框
		   {
			   if (wParam == AVK_UP || wParam == AVK_DOWN) //在控件间切换
			   {
				   if (ITEXTCTL_IsActive( pMe->textCtl))
				   {
					   ITEXTCTL_SetActive( pMe->textCtl,FALSE);
					   IMENUCTL_SetActive(pMenuCtl,TRUE);
					   
				   }
				   else
				   {
					   IMENUCTL_SetActive(pMenuCtl,FALSE);
					   ITEXTCTL_SetActive( pMe->textCtl,TRUE);
					   ITEXTCTL_SetCursorPos(pMe->textCtl, TC_CURSOREND);//  [3/17/2016 Administrator]
				   }
				   break;
			   }

			   
		
			
				if (ITEXTCTL_IsActive( pMe->textCtl))
				{
					
					bHandled = ITEXTCTL_HandleEvent( pMe->textCtl, eCode, wParam, dwParam );
					ITEXTCTL_Redraw( pMe->textCtl );
					ITEXTCTL_GetText(pMe->textCtl,pMe->pText,100);
					WSTRTOSTR(pMe->pText,s_buf,100);
					uf_RecLog(pMe,s_buf );
					IDISPLAY_Update(pMe->a.m_pIDisplay); 

					return TRUE;
				}
				else
				{
					
					bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );		   
				}
			}
			else
			{
				bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
			}

		   

	//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "触发 EVT_KEY_RELEASE");

			if (wParam == AVK_SOFT2)	
			{
				if ( pMe->m_Right_Key == '1' )
				{
					if (pMe->pi_copy)
					{
						IIMAGE_Release(pMe->pi_copy); 
						pMe->pi_copy = NULL;
					}
					
					if (pMe->pi)
					{
						IIMAGE_Release(pMe->pi);
						pMe->pi = NULL;
					}
					
					if (pMenuCtl)
					{
						IMENUCTL_Release( pMenuCtl );
						pMenuCtl = NULL;
					}
					if (pMe->m_PhotoTextCtl)
					{
						ISTATIC_Release( pMe->m_PhotoTextCtl );
					}
					if (pMe->textCtl)
					{
						ITEXTCTL_Release(pMe->textCtl);
					}
					
					pMe->m_PhotoTextCtl = NULL;
					pMe->textCtl = NULL;
					pMenuCtl = NULL;
					
					pMe->currentHandler = 0;
					testgps_SetScreenData( pMe, 0 );
					
					//	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
					pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
					ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
					pMe->m_FOBFlag = TRUE; //在后台运行
					pMe->m_WitchPag = 0;
					//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"后台运行 6:%d",pMe->m_WitchPag);
					bHandled = TRUE;
					return TRUE;
				}
			}
		   
		   
		   // AVK_POWER   AVK_END
		
			if (wParam == AVK_END)
			{
				if (STRCMP(pMe->sMobileType,"55") == 0 || (STRCMP(pMe->sMobileType,"03") == 0)  || (STRCMP(pMe->sMobileType,"58") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0) || (STRCMP(pMe->sMobileType,"57") == 0) )
				{
					/*
					IMENUCTL_Release( pMenuCtl );
			 if (pMe->textCtl)
			 {
				 ITEXTCTL_Release(pMe->textCtl);
			 }
			 ISTATIC_Release( pMe->m_PhotoTextCtl );
			 testgps_SetScreenData( pMe, 0 );

			 pMe->currentHandler = 0;
			 pMe->m_PhotoTextCtl = NULL;
			 pMe->textCtl = NULL;
			 pMenuCtl = NULL;
			 
					*/
					// 这里直接回到后台

					if (pMe->pi_copy)
					{
						IIMAGE_Release(pMe->pi_copy); 
						pMe->pi_copy = NULL;
					}

					if (pMe->pi)
					{
						IIMAGE_Release(pMe->pi);
						pMe->pi = NULL;
					}

					   if (pMenuCtl)
					   {
						   IMENUCTL_Release( pMenuCtl );
						   pMenuCtl = NULL;
					   }
					   if (pMe->m_PhotoTextCtl)
					   {
						   ISTATIC_Release( pMe->m_PhotoTextCtl );
					   }
					   if (pMe->textCtl)
					   {
						   ITEXTCTL_Release(pMe->textCtl);
					   }
					   
					   pMe->m_PhotoTextCtl = NULL;
					   pMe->textCtl = NULL;
					   pMenuCtl = NULL;

					   pMe->currentHandler = 0;
					   testgps_SetScreenData( pMe, 0 );

					   //	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
					   pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
					   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
					   pMe->m_FOBFlag = TRUE; //在后台运行
					   pMe->m_WitchPag = 0;
			//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"后台运行 6:%d",pMe->m_WitchPag);
					   bHandled = TRUE;
					   return TRUE;
				}
			}
			
		   if( wParam == AVK_END  || wParam == AVK_CLR ) {
	//		   IMENUCTL_Release( pMe->m_PhotoTextCtl );
	//		   IMENUCTL_Release( pMenuCtl );
	 //          pMe->currentHandler = 0;
	//		   testgps_SetScreenData( pMe, 0 );

			   if (pMe->pi_copy)
			   {
				   IIMAGE_Release(pMe->pi_copy); 
				   pMe->pi_copy = NULL;
			   }

			   if (pMe->pi)
			   {
				   IIMAGE_Release(pMe->pi);
				   pMe->pi = NULL;
			   }

			   pMe->m_WitchPag = 10; // 返回主界面
			   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
	//		   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
	//		   pMe->m_FOBFlag = TRUE; //在后台运行

			   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"页返回主界面 2:%d",pMe->m_WitchPag);
			   bHandled = TRUE;
		   }
	   }

	   
		
	   break;
	   /*
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
		  bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
		  if( wParam == AVK_END  || wParam == AVK_CLR ) {
			  ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
			  pMe->m_FOBFlag = TRUE; //在后台运行
			  pMe->m_WitchPag = 0;
			  bHandled = TRUE;
	   }
      break;
	  */

   case EVT_COMMAND:
      if( dwParam == (uint32)pMenuCtl ) {
         switch( wParam ) {
         case 0:  //上一页

			 /******
			 // 显示照片 
				if ( STRLEN(pMe->m_photo[0].photoname) > 0 ) 
				{
					uf_RecLog(pMe,"Find Send photo" );
					STRCPY(pMe->sFileName,pMe->m_photo[0].photoname);
					pMe->m_i_photo = pMe->m_photo[0].i_dex;
					if (pMe->cPhotoSend == '1') 
					{
						pMe->pi = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);
						if (pMe->pi) 		
						{
							IImage_Notify(pMe->pi, (PFNIMAGEINFO)uf_ImageNotify,pMe);  //注册回调
						}
					}
				}
			 ******/
			 {
				 
				 if (pMe->m_i_photo > 1)//说明不是第一张
				 {
					 pMe->m_i_photo--;
					 
					 if ( STRLEN(pMe->m_photo[pMe->m_i_photo-1].photoname) > 0 ) 
					{
							uf_RecLog(pMe,"Find Send photo" );
							STRCPY(pMe->sFileName,pMe->m_photo[pMe->m_i_photo-1].photoname);
							
							if (STRCMP(pMe->sMobileType,"01") == 0 || STRCMP(pMe->sMobileType,"15") == 0 /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/)
							{
							//	ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY)Load_Image_fun, pMe);

								Load_Image_fun(pMe);
								if (pMe->pi_copy) 		
								{
									IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //注册回调
								}
								return TRUE;
								//			pMe->pi_copy = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);
							}
							
							pMe->pi = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);

							if (pMe->pi) 		
							{
								IImage_Notify(pMe->pi, (PFNIMAGEINFO)uf_ImageNotify,pMe);  //注册回调
							}
					}
				 }
				 return TRUE;
				 
				 
				
			 }
            break;
		case 1: // 发送
			{
			//	if (pMe->iSendPhoto== 1)
			//	{	// 发送照片
					pMe->m_bSendPF = TRUE ;//用来标记是否正在发送图片的标志位 TRUE -- 表示正在发送状态  FALSE -- 表示空闲

					pMe->iSendPhoto = 2;
					DBGPRINTF( "-----send 0" );
					
					pMe->m_WitchPag = 25;// 锁屏 [3/16/2016 Administrator]
					uf_AEPrint_Sending(pMe,FALSE);

					
					if (pMe->pi_copy)
					{
						IIMAGE_Release(pMe->pi_copy); 
						pMe->pi_copy = NULL;
					}
					
					if (pMe->pi)
					{
						IIMAGE_Release(pMe->pi);
						pMe->pi = NULL;
					}

					// 释放与选择照片有关的控件资源
					uf_ReleaseCtl(pMe,FALSE);
					// 发送照片
				//  Change by ----  2012-10-16			uf_SendPhoto(pMe);
					if ( FleUpd_HandleEvent(pMe,FLDUPD_EVT_START,0,0) )	 {
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"shang  chuan jin lai");
						ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,FLDUPD_EVT_UPLOAD,52,0); // 52这个数值就是代表 ‘4’
					}
				// Change End
		//		}
			}
				break;
		 case 2:  //下一页
			 {
				if (pMe->m_i_photo < 5)//说明不是最后一张
				 {
					 pMe->m_i_photo++;
					 
					 if ( STRLEN(pMe->m_photo[pMe->m_i_photo-1].photoname) > 0 ) 
					 {
							uf_RecLog(pMe,"Find Send photo" );
							STRCPY(pMe->sFileName,pMe->m_photo[pMe->m_i_photo-1].photoname);
							
							if (STRCMP(pMe->sMobileType,"01") == 0 || STRCMP(pMe->sMobileType,"15") == 0 /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/)
							{
							//	ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY)Load_Image_fun, pMe);

								Load_Image_fun(pMe);
								if (pMe->pi_copy) 		
								{
									IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //注册回调
								}
								return TRUE;
				//			pMe->pi_copy = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);
							}

							pMe->pi = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);
							if (pMe->pi) 		
							{
								IImage_Notify(pMe->pi, (PFNIMAGEINFO)uf_ImageNotify,pMe);  //注册回调
							}
					 }
					 else  //这种情况是在照片总数没有达到5张的情况下
					 {
						 pMe->m_i_photo--; //说明没有照片了
					 }
				 }
				 
			 }
			
			return TRUE;
         default:
            break;
         }
      }
      break;
   }
   return bHandled;
}


// ==================================================================
// 1.把照片显示在屏幕上，供用户确认
//  2012-10-16  改为直接在照片原来的目录上读取照片
//  获取照片 //最多获取最近的5张照片
// ==================================================================
void newgimis_GetPhote(newgimis * pMe)
{
    char strRoot[512];				// 2012-10-16

	FileInfo* pFileInfo = NULL;	
	IFileMgr* pFileMgr = NULL;
	int iCurImg = 0;		// 当前浏览的图片
	char lStr_Log[LOG_STR_LEN];

	char strCopyDate[15];		// yyyymmddhhmmss   保存最新一张照片的日期
    char strDateTime[15];
	char strEveryDate[5][15];		// yyyymmddhhmmss   保存最新一张照片的日期
    int idx;
	int s_itmp = 0;


	MEMSET(pMe->m_photo,0x00,sizeof(SavePhoto)*5);
	MEMSET(strEveryDate,0,sizeof(strEveryDate)*5);
   
   MEMSET(strCopyDate,0,sizeof(strCopyDate));
   MEMSET(strRoot,0,sizeof(strRoot));			
   SPRINTF(strRoot, AEEFS_ROOT_DIR"%s", pMe->sPhotoPath);
   photo_SetRootDir(pMe,strRoot);	// Add 2013-05-09
   SPRINTF(lStr_Log,"uf_GetNewestPhotoForSend in, photo_path=%s",strRoot);
   uf_RecLog(pMe,lStr_Log);

  if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
  {
	uf_RecLog(pMe, "ISHELL_CreateInstance err");
	pMe->imgidx = 0;		// 手机中没有照片
    return;
  }

  // Initialize the file manager's file index in the application's root directory
  if ( IFILEMGR_EnumInit( pFileMgr, strRoot, FALSE ) )   {  //初始化 IFILEMGR接口
	  int iAccess;
	  iAccess = IFILEMGR_CheckPathAccess(pFileMgr,strRoot,AEEFP_READ,NULL);
	  SPRINTF(lStr_Log,"-----access=%d",iAccess);
	  DBGPRINTF(lStr_Log);	 
	  uf_RecLog(pMe,lStr_Log);
  }

   MEMSET(pMe->sFileName,0,sizeof(pMe->sFileName));

  // Get each file name in the aplication's root directory.
  pFileInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
  if( pFileInfo )   {		// if -- pFileInfo
	  

	 idx = 0;

    while( IFILEMGR_EnumNext( pFileMgr, pFileInfo ) )
    {
		FileInfo * pInfo;	
		if ( (STRCMP(pMe->sMobileType,"33") == 0) ) {
			MEMSET(lStr_Log,0,STRLEN( pFileInfo->szName )+1);
			STRCPY(lStr_Log,pFileInfo->szName);
			STRLOWER(lStr_Log);
			if ( STRENDS( "sys", lStr_Log ) )
				continue;
		}

        uf_RecLog(pMe,"-----pFileInfo 1111111111");
	    pInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
		if ( IFILEMGR_GetInfo(pFileMgr, pFileInfo->szName, pInfo) == SUCCESS )  {
			if (pInfo->dwCreationDate != 0) {
				idx++;
	//			uf_RecLog(pMe,"获取照片 进来");
				SPRINTF(lStr_Log, "-----FileName0=%s,CreationDate0=%d",pInfo->szName,pInfo->dwCreationDate);
				uf_RecLog(pMe,lStr_Log);
				uf_GetDate(pMe,pInfo->dwCreationDate,strDateTime,sizeof(strDateTime),'1');
				
				if ( STRCMP(strDateTime,strCopyDate) > 0 ) {
					STRCPY(strCopyDate,strDateTime);
					STRCPY( pMe->sFileName, pFileInfo->szName );
					pMe->imgidx = idx;
		//			uf_RecLog(pMe,"获取照片 大于 进来这里进来");
		//			if (pMe->m_photo[4].i_dex > 0 ) 
		//			{
						for (s_itmp = 3; s_itmp >= 0; s_itmp-- )
						{
							pMe->m_photo[s_itmp+1].i_dex = pMe->m_photo[s_itmp].i_dex;
							STRCPY(pMe->m_photo[s_itmp+1].photoname,pMe->m_photo[s_itmp].photoname);
							STRCPY(strEveryDate[s_itmp+1], strEveryDate[s_itmp] );
							
						}
						pMe->m_photo[0].i_dex = pMe->imgidx;
						STRCPY(pMe->m_photo[0].photoname,pFileInfo->szName);
						STRCPY(strEveryDate[0], strCopyDate );
		//			}
				}
				else
				{
//					uf_RecLog(pMe,"获取照片 小于 进来这里进来");
					uf_RecLog(pMe,"get photo  less than come there come");
					// 有可能先获取到最近的，后面获取到比较前面的 但是又没有达到5张
					if ( STRCMP(strDateTime,strEveryDate[1]) > 0 )
					{
						for (s_itmp = 3; s_itmp > 0; s_itmp-- )
						{
							pMe->m_photo[s_itmp+1].i_dex = pMe->m_photo[s_itmp].i_dex;
							STRCPY(pMe->m_photo[s_itmp+1].photoname,pMe->m_photo[s_itmp].photoname);
							STRCPY(strEveryDate[s_itmp+1], strEveryDate[s_itmp] );
							
						}
						pMe->m_photo[1].i_dex = pMe->imgidx;
						STRCPY(pMe->m_photo[1].photoname,pFileInfo->szName);
						STRCPY(strEveryDate[1], strCopyDate );
					}
					else if ( STRCMP(strDateTime,strEveryDate[2]) > 0 )
					{
						for (s_itmp = 3; s_itmp > 1; s_itmp-- )
						{
							pMe->m_photo[s_itmp+1].i_dex = pMe->m_photo[s_itmp].i_dex;
							STRCPY(pMe->m_photo[s_itmp+1].photoname,pMe->m_photo[s_itmp].photoname);
							STRCPY(strEveryDate[s_itmp+1], strEveryDate[s_itmp] );
							
						}
						pMe->m_photo[2].i_dex = pMe->imgidx;
						STRCPY(pMe->m_photo[2].photoname,pFileInfo->szName);
						STRCPY(strEveryDate[2], strCopyDate );
					}
					else if ( STRCMP(strDateTime,strEveryDate[3]) > 0 )
					{
						for (s_itmp = 3; s_itmp > 2; s_itmp-- )
						{
							pMe->m_photo[s_itmp+1].i_dex = pMe->m_photo[s_itmp].i_dex;
							STRCPY(pMe->m_photo[s_itmp+1].photoname,pMe->m_photo[s_itmp].photoname);
							STRCPY(strEveryDate[s_itmp+1], strEveryDate[s_itmp] );
							
						}
						pMe->m_photo[3].i_dex = pMe->imgidx;
						STRCPY(pMe->m_photo[3].photoname,pFileInfo->szName);
						STRCPY(strEveryDate[3], strCopyDate );
					}
					else if ( STRCMP(strDateTime,strEveryDate[4]) > 0 )
					{
						pMe->m_photo[4].i_dex = pMe->imgidx;
						STRCPY(pMe->m_photo[4].photoname,pFileInfo->szName);
						STRCPY(strEveryDate[4], strCopyDate );
					}
					
				}
			}
		}
		
  	    FREEIF( pInfo );
    }  // while -- 1
	//给照片重新编号
	pMe->m_photo[0].i_dex = 1;
	pMe->m_photo[1].i_dex = 2;
	pMe->m_photo[2].i_dex = 3;
	pMe->m_photo[3].i_dex = 4;
	pMe->m_photo[4].i_dex = 5;
	/*
	uf_RecLog(pMe,"获取照片");
	uf_RecLog(pMe,pMe->m_photo[0].photoname);
	uf_RecLog(pMe,pMe->m_photo[1].photoname);
	uf_RecLog(pMe,pMe->m_photo[2].photoname);
	uf_RecLog(pMe,pMe->m_photo[3].photoname);
	uf_RecLog(pMe,pMe->m_photo[4].photoname);
	uf_RecLog(pMe,pMe->sFileName);
	*/
	
	/*
	if ( STRLEN(strCopyDate) > 0 ) {
		uf_RecLog(pMe,"Find Send photo" );
		if (pMe->cPhotoSend == '1') {
			pMe->pi = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);
			if (pMe->pi) 		{
				IImage_Notify(pMe->pi, (PFNIMAGEINFO)uf_ImageNotify,pMe);  //注册回调
			}
		}
	}
	else {
		uf_RecLog(pMe,"can not Find Send photo" );
		pMe->imgidx = 0;		   // 手机中没有照片
		uf_AEPrint(pMe,FALSE);		// 一张都没有没有
	}
	*/

	FREEIF( pFileInfo );

  }	// if -- pFileInfo

  IFILEMGR_Release( pFileMgr );


}



// uf_ReturnMenu  返回主菜单
void uf_ReturnMenu(newgimis * pMe)
{
	pMe->m_bSendPF = FALSE; //用来标记是否正在发送图片的标志位 TRUE -- 表示正在发送状态  FALSE -- 表示空闲
	ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)uf_ReturnMenu,pMe);
	pMe->m_WitchPag = 10;
	testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
	
}








//uf_AEPrint_Photo_Back
// ================================================== //
// 显示汉字   照片上传功能未开启
// boolean bCallBack  TRUE 回调, FALSE 不用回调
// ================================================== //
void uf_AEPrint_Photo_Back(newgimis * pMe,boolean bCallBack)
{   // \u7167\u7247\u4e0a\u4f20\u529f\u80fd\u672a\u5f00\u542f
	AECHAR szText[] = {0x7167,0x7247,0x4e0a ,0x4f20,0x529f,0x80fd,0x672a,0x5f00,0x542f,0x0000}; // L"照片上传功能未开启";
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu, pMe);

}


// ================================================== //
// 显示汉字
// boolean bCallBack  TRUE 回调, FALSE 不用回调
// ================================================== //
void uf_AEPrint_Back(newgimis * pMe,boolean bCallBack)
{   // \u65e0\u89c6\u9891
	AECHAR szText[] = {0x65e0,0x89c6,0x9891,0x0000}; // L"无视频";
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu, pMe);

}


// ================================================== //
// 显示汉字
// boolean bCallBack  TRUE 回调, FALSE 不用回调
// ================================================== //
void uf_AEPrintPhoto_Back(newgimis * pMe,boolean bCallBack)
{   // \u65e0\u7167\u7247
	AECHAR szText[] = {0x65e0,0x7167,0x7247,0x0000}; // L"无照片";
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu, pMe);

}

//
// ================================================== //
// 显示汉字  没有一键考勤数据
// boolean bCallBack  TRUE 回调, FALSE 不用回调
// ================================================== //
void uf_AEPrint_YJKQ_Back(newgimis * pMe,boolean bCallBack)
{   // \u6682\u65e0\u47\u50\u53\u8003\u52e4\u6570\u636e  暂无GPS考勤数据
	AECHAR szText[] = {0x6682,0x65e0,0x47,0x50,0x53,0x8003,0x52e4,0x6570,0x636e,0x0000}; // L"无照片";
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu, pMe);

}

//
// ================================================== //
// 显示汉字  没有一键打卡数据
// boolean bCallBack  TRUE 回调, FALSE 不用回调
// ================================================== //
void uf_AEPrint_YJDK_Back(newgimis * pMe,boolean bCallBack)
{   // \u6682\u65e0\u47\u50\u53\u6570\u636e  暂无GPS数据
	AECHAR szText[] = {0x6682,0x65e0,0x47,0x50,0x53,0x6570,0x636e,0x0000}; // L"暂无GPS数据";
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu, pMe);
	
}


// uf_AEPrint_Sending  显示发送中
void uf_AEPrint_Sending(newgimis * pMe,boolean bCallBack)
{   // 发送中  \u53d1\u9001\u4e2d
	AECHAR szText[] = {0x53d1,0x9001,0x4e2d,0x0000}; // L"发送中";
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}

//  [3/4/2016 Administrator]
// uf_AEPrint_Sending  显示 获取详情中
void uf_AEPrint_xq(newgimis * pMe,boolean bCallBack)
{   // 获取详情中  \83b7\53d6\8be6\60c5\4e2d
	AECHAR szText[] = {0x83b7,0x53d6,0x8be6,0x60c5,0x4e2d,0x0000}; // 获取详情中";
	
	
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
		8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}




//
// ================================================== //
// 显示汉字 无网络
// boolean bCallBack  TRUE 回调, FALSE 不用回调
// ================================================== //
void uf_AEPrint_NO_NET_Back(newgimis * pMe,boolean bCallBack)
{   // \u65e0\u7f51\u7edc  无网络
	AECHAR szText[] = {0x65e0,0x7f51,0x7edc,0x0000}; // L"无网络";
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu, pMe);

}


// 显示空车发送失败
void uf_AEPrint_EMPTY_FAIL_Back(newgimis * pMe)
{	// \u53d1\u9001\u7a7a\u8f66\u5931\u8d25    发送空车失败
	AECHAR szText[] = {0x53d1,0x9001,0x7a7a,0x8f66,0x5931,0x8d25,0x0000}; // L"无网络";
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}

// 显示重车发送失败
void uf_AEPrint_FULL_FAIL_Back(newgimis * pMe)
{// \u53d1\u9001\u91cd\u8f66\u5931\u8d25    发送重车失败
	AECHAR szText[] = {0x53d1,0x9001,0x91cd,0x8f66,0x5931,0x8d25,0x0000}; // L"无网络";
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}

// 显示状态1 发送失败
void uf_AEPrint_ZT_1_FAIL_Back(newgimis * pMe)
{// \u53d1\u9001\u72b6\u6001\u31\u5931\u8d25    发送状态1失败
	AECHAR szText[] = {0x53d1,0x9001,0x72b6,0x6001,0x31,0x5931,0x8d25,0x0000}; 
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}

// 显示状态0 发送失败
void uf_AEPrint_ZT_0_FAIL_Back(newgimis * pMe)
{// \u53d1\u9001\u72b6\u6001\u31\u5931\u8d25    发送状态1失败
	AECHAR szText[] = {0x53d1,0x9001,0x72b6,0x6001,0x30,0x5931,0x8d25,0x0000}; 
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}

// 一键打卡 发送失败
void uf_AEPrint_YJDK_FAIL_Back(newgimis * pMe)
{// \u6253\u5361\u5931\u8d25    打卡失败
	AECHAR szText[] = {0x6253,0x5361,0x5931,0x8d25,0x0000}; 
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
		8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}

// 显示状态1 发送成功
void uf_AEPrint_ZT_1_SUCC_Back(newgimis * pMe)
{// \u53d1\u9001\u72b6\u6001\u31   发送状态1成功
	AECHAR szText[] = {0x53d1,0x9001,0x72b6,0x6001,0x31,0x6210,0x529f,0x0000}; 
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}

// 显示状态0 发送成功
void uf_AEPrint_ZT_0_SUCC_Back(newgimis * pMe)
{// \u53d1\u9001\u72b6\u6001\u31   发送状态0成功
	AECHAR szText[] = {0x53d1,0x9001,0x72b6,0x6001,0x30,0x6210,0x529f,0x0000}; 
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}

// 显示空车发送成功
void uf_AEPrint_EMPTY_SUCC_Back(newgimis * pMe)
{	// \u53d1\u9001\u7a7a\u8f66\u6210\u529f     发送空车成功
	AECHAR szText[] = {0x53d1,0x9001,0x7a7a,0x8f66,0x6210,0x529f,0x0000}; // L"无网络";
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}

// 显示重车发送成功
void uf_AEPrint_FULL_SUCC_Back(newgimis * pMe)
{	// \u53d1\u9001\u91cd\u8f66\u6210\u529f     发送重车成功
	AECHAR szText[] = {0x53d1,0x9001,0x91cd,0x8f66,0x6210,0x529f,0x0000}; // L"无网络";
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}

// 一键打卡 发送成功
void uf_AEPrint_YJDK_SUCC_Back(newgimis * pMe)
{// \u6253\u5361\u6210\u529f    打卡成功
	AECHAR szText[] = {0x6253,0x5361,0x6210,0x529f,0x0000}; 
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
		8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);
}



//uf_AEPrint_Photo_Back
// ================================================== //
// 显示汉字  视频上传功能未开启
// boolean bCallBack  TRUE 回调, FALSE 不用回调
// ================================================== //
void uf_AEPrint_Audio_Back(newgimis * pMe,boolean bCallBack)
{   // \u89c6\u9891\u4e0a\u4f20\u529f\u80fd\u672a\u5f00\u542f
	AECHAR szText[] = {0x89c6,0x9891,0x4e0a ,0x4f20,0x529f,0x80fd,0x672a,0x5f00,0x542f,0x0000}; // L"视频上传功能未开启";
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = 所有字符串的都表示
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu, pMe);

}




// ==================================================================
// 1.把照片显示在屏幕上，供用户确认
//  2012-10-16  改为直接在照片原来的目录上读取照片
//  获取照片 //最多获取最近的5张照片
// ==================================================================
void newgimis_GetAudio(newgimis * pMe)
{
    char strRoot[512];				// 2012-10-16

	FileInfo* pFileInfo = NULL;	
	IFileMgr* pFileMgr = NULL;
	int iCurImg = 0;		// 当前浏览的图片
	char lStr_Log[LOG_STR_LEN];

	char strCopyDate[15];		// yyyymmddhhmmss   保存最新一张照片的日期
    char strDateTime[15];
	char strEveryDate[5][15];		// yyyymmddhhmmss   保存最新一张照片的日期
    int idx;
	int s_itmp = 0;


	MEMSET(pMe->m_Audio,0x00,sizeof(SavePhoto)*5);
	MEMSET(strEveryDate,0,sizeof(strEveryDate)*5);
   
   MEMSET(strCopyDate,0,sizeof(strCopyDate));
   MEMSET(strRoot,0,sizeof(strRoot));			
   SPRINTF(strRoot, AEEFS_ROOT_DIR"%s", pMe->sVideoPath);
   photo_SetRootDir(pMe,strRoot);	// Add 2013-05-09
   SPRINTF(lStr_Log,"uf_GetNewestPhotoForSend in, photo_path=%s",strRoot);
   uf_RecLog(pMe,lStr_Log);

  if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
  {
	uf_RecLog(pMe, "ISHELL_CreateInstance err");
	pMe->imgidx = 0;		// 手机中没有视频
    return;
  }

  // Initialize the file manager's file index in the application's root directory
  if ( IFILEMGR_EnumInit( pFileMgr, strRoot, FALSE ) )   {  //初始化 IFILEMGR接口
	  int iAccess;
	  iAccess = IFILEMGR_CheckPathAccess(pFileMgr,strRoot,AEEFP_READ,NULL);
	  SPRINTF(lStr_Log,"-----access=%d",iAccess);
	  DBGPRINTF(lStr_Log);	 
	  uf_RecLog(pMe,lStr_Log);
  }

   MEMSET(pMe->sFileName,0,sizeof(pMe->sFileName));

  // Get each file name in the aplication's root directory.
  pFileInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
  if( pFileInfo )   {		// if -- pFileInfo
	  

	 idx = 0;

    while( IFILEMGR_EnumNext( pFileMgr, pFileInfo ) )
    {
		FileInfo * pInfo;	
		if ( (STRCMP(pMe->sMobileType,"33") == 0) ) {
			MEMSET(lStr_Log,0,STRLEN( pFileInfo->szName )+1);
			STRCPY(lStr_Log,pFileInfo->szName);
			STRLOWER(lStr_Log);
			if ( STRENDS( "sys", lStr_Log ) )
				continue;
		}

        uf_RecLog(pMe,"-----pFileInfo 111");
	    pInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
		if ( IFILEMGR_GetInfo(pFileMgr, pFileInfo->szName, pInfo) == SUCCESS )  {
			if (pInfo->dwCreationDate != 0) {
				idx++;
	//			uf_RecLog(pMe,"获取照片 进来");
				SPRINTF(lStr_Log, "-----FileName0=%s,CreationDate0=%d",pInfo->szName,pInfo->dwCreationDate);
				uf_RecLog(pMe,lStr_Log);
				uf_GetDate(pMe,pInfo->dwCreationDate,strDateTime,sizeof(strDateTime),'1');
				
				if ( STRCMP(strDateTime,strCopyDate) > 0 ) {
					STRCPY(strCopyDate,strDateTime);
			//		STRCPY( pMe->sFileName, pFileInfo->szName );
					SPRINTF(pMe->sFile, "%s", pFileInfo->szName);
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"will send video file=%s",pMe->sFile);
			//		pMe->nState = SUCCESS;

					pMe->imgidx = idx;
		//			uf_RecLog(pMe,"获取照片 大于 进来这里进来");
		//			if (pMe->m_Audio[4].i_dex > 0 ) 
		//			{
						for (s_itmp = 3; s_itmp >= 0; s_itmp-- )
						{
							pMe->m_Audio[s_itmp+1].i_dex = pMe->m_Audio[s_itmp].i_dex;
							STRCPY(pMe->m_Audio[s_itmp+1].photoname,pMe->m_Audio[s_itmp].photoname);
							STRCPY(strEveryDate[s_itmp+1], strEveryDate[s_itmp] );
							
						}
						pMe->m_Audio[0].i_dex = pMe->imgidx;
						STRCPY(pMe->m_Audio[0].photoname,pFileInfo->szName);
						STRCPY(strEveryDate[0], strCopyDate );
		//			}
				}
				else
				{
//					uf_RecLog(pMe,"获取照片 小于 进来这里进来");
					uf_RecLog(pMe,"get photo less than come there come 2");
					// 有可能先获取到最近的，后面获取到比较前面的 但是又没有达到5张
					if ( STRCMP(strDateTime,strEveryDate[1]) > 0 )
					{
						for (s_itmp = 3; s_itmp > 0; s_itmp-- )
						{
							pMe->m_Audio[s_itmp+1].i_dex = pMe->m_Audio[s_itmp].i_dex;
							STRCPY(pMe->m_Audio[s_itmp+1].photoname,pMe->m_Audio[s_itmp].photoname);
							STRCPY(strEveryDate[s_itmp+1], strEveryDate[s_itmp] );
							
						}
						pMe->m_Audio[1].i_dex = pMe->imgidx;
						STRCPY(pMe->m_Audio[1].photoname,pFileInfo->szName);
						STRCPY(strEveryDate[1], strCopyDate );
					}
					else if ( STRCMP(strDateTime,strEveryDate[2]) > 0 )
					{
						for (s_itmp = 3; s_itmp > 1; s_itmp-- )
						{
							pMe->m_Audio[s_itmp+1].i_dex = pMe->m_Audio[s_itmp].i_dex;
							STRCPY(pMe->m_Audio[s_itmp+1].photoname,pMe->m_Audio[s_itmp].photoname);
							STRCPY(strEveryDate[s_itmp+1], strEveryDate[s_itmp] );
							
						}
						pMe->m_Audio[2].i_dex = pMe->imgidx;
						STRCPY(pMe->m_Audio[2].photoname,pFileInfo->szName);
						STRCPY(strEveryDate[2], strCopyDate );
					}
					else if ( STRCMP(strDateTime,strEveryDate[3]) > 0 )
					{
						for (s_itmp = 3; s_itmp > 2; s_itmp-- )
						{
							pMe->m_Audio[s_itmp+1].i_dex = pMe->m_Audio[s_itmp].i_dex;
							STRCPY(pMe->m_Audio[s_itmp+1].photoname,pMe->m_Audio[s_itmp].photoname);
							STRCPY(strEveryDate[s_itmp+1], strEveryDate[s_itmp] );
							
						}
						pMe->m_Audio[3].i_dex = pMe->imgidx;
						STRCPY(pMe->m_Audio[3].photoname,pFileInfo->szName);
						STRCPY(strEveryDate[3], strCopyDate );
					}
					else if ( STRCMP(strDateTime,strEveryDate[4]) > 0 )
					{
						pMe->m_Audio[4].i_dex = pMe->imgidx;
						STRCPY(pMe->m_Audio[4].photoname,pFileInfo->szName);
						STRCPY(strEveryDate[4], strCopyDate );
					}
					
				}
			}
		}
		
  	    FREEIF( pInfo );
    }  // while -- 1
	//给照片重新编号
	pMe->m_Audio[0].i_dex = 1;
	pMe->m_Audio[1].i_dex = 2;
	pMe->m_Audio[2].i_dex = 3;
	pMe->m_Audio[3].i_dex = 4;
	pMe->m_Audio[4].i_dex = 5;
	
	uf_RecLog(pMe,"get audio");
//	uf_RecLog(pMe,pMe->m_Audio[0].photoname);
//	uf_RecLog(pMe,pMe->m_Audio[1].photoname);
//	uf_RecLog(pMe,pMe->m_Audio[2].photoname);
//	uf_RecLog(pMe,pMe->m_Audio[3].photoname);
//	uf_RecLog(pMe,pMe->m_Audio[4].photoname);
	uf_RecLog(pMe,pMe->sFileName);
	
	
	/*
	if ( STRLEN(strCopyDate) > 0 ) {
		uf_RecLog(pMe,"Find Send photo" );
		if (pMe->cPhotoSend == '1') {
			pMe->pi = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);
			if (pMe->pi) 		{
				IImage_Notify(pMe->pi, (PFNIMAGEINFO)uf_ImageNotify,pMe);  //注册回调
			}
		}
	}
	else {
		uf_RecLog(pMe,"can not Find Send photo" );
		pMe->imgidx = 0;		   // 手机中没有照片
		uf_AEPrint(pMe,FALSE);		// 一张都没有没有
	}
	*/

	FREEIF( pFileInfo );

  }	// if -- pFileInfo

  IFILEMGR_Release( pFileMgr );


}






/***************************************************************************
SamplePosDet_SendAudio_HandleEvent 发送视频
****************************************************************************/
boolean SamplePosDet_SendAudio_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam )
{
	AEERect rRect;
	CtlAddItem rMenuItem;

	int  m_nFontHeight = 0;
	AECHAR c_Tmp[512];
	char *sc_tmp = NULL;
	int si_tmp = 0;

	boolean bHandled = FALSE;
	IMenuCtl *pMenuCtl = testgps_GetScreenData_Audio( pMe );
	IMenuCtl *pMenuCtl_N = testgps_GetScreenData( pMe );

 //  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "进来视频 1");
	if(pMe->m_bSendPF == TRUE )//用来标记是否正在发送图片的标志位 TRUE -- 表示正在发送状态  FALSE -- 表示空闲
	{
		// 说明正在发送照片 
//		return TRUE;  暂时关闭
	}
	
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "进来视频 2");
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
				IMENUCTL_SetProperties( pMenuCtl, MP_MAXSOFTKEYITEMS | MP_NO_ARROWS );
				m_nFontHeight = IDISPLAY_GetFontMetrics( pMe->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL );
				rMenuItem.wFont = AEE_FONT_NORMAL;

			
				

			   IMENUCTL_SetTitle( pMenuCtl_N, NEWGIMIS_RES_FILE, IDS_AUDIO_FILE, NULL );
			   MEMSET(c_Tmp,0x00,512);

			   sc_tmp = (char *)MALLOC(512);
			   MEMSET(sc_tmp,0x00,512);


			   /*////////////////////////////////////////////////////////////////////////
			   把路径去了
			   	sc_tmp = (char *)MALLOC(100);
				MEMSET(sc_tmp,0x00,100);
				si_tmp = ud_rDistToChar(pMe->m_photo[pMe->m_i_photo-1].photoname,'/');
				if (si_tmp >= 0)
				{
				STRCPY(sc_tmp,pMe->m_photo[pMe->m_i_photo-1].photoname+si_tmp+1);
				STR_TO_WSTR(sc_tmp,s_Tmp,100);
				}
				else
				{
				STR_TO_WSTR(pMe->m_photo[pMe->m_i_photo-1].photoname,s_Tmp,100);
				}

			   ////////////////////////////////////////////////////////////////////////*/

			   if (STRLEN(pMe->m_Audio[0].photoname) > 0)
			   {
				   MEMSET(c_Tmp,0x00,512);
				   MEMSET(sc_tmp,0x00,512);

					si_tmp = ud_rDistToChar(pMe->m_Audio[0].photoname,'/');
					if (si_tmp >= 0)
					{
						STRCPY(sc_tmp,pMe->m_Audio[0].photoname+si_tmp+1);
						STR_TO_WSTR(sc_tmp,c_Tmp,512);
					}
					else
					{
						STR_TO_WSTR(pMe->m_photo[0].photoname,c_Tmp,512);
					}

				   // STRTOWSTR(pMe->m_Audio[0].photoname,c_Tmp,512);

				   
				   IMENUCTL_AddItem( pMenuCtl_N, NULL, NULL, 0, c_Tmp, (uint32)pMenuCtl_N );
			   }
			   
			   if (STRLEN(pMe->m_Audio[1].photoname) > 0)
			   {
				   MEMSET(c_Tmp,0x00,512);
				   MEMSET(sc_tmp,0x00,512);

				   si_tmp = ud_rDistToChar(pMe->m_Audio[1].photoname,'/');
					if (si_tmp >= 0)
					{
						STRCPY(sc_tmp,pMe->m_Audio[1].photoname+si_tmp+1);
						STR_TO_WSTR(sc_tmp,c_Tmp,512);
					}
					else
					{
						STR_TO_WSTR(pMe->m_photo[1].photoname,c_Tmp,512);
					}


			//	   STRTOWSTR(pMe->m_Audio[1].photoname,c_Tmp,512);
				   IMENUCTL_AddItem( pMenuCtl_N, NULL, NULL, 1, c_Tmp, (uint32)pMenuCtl_N );
			   }

			   if (STRLEN(pMe->m_Audio[2].photoname) > 0)
			   {
				   MEMSET(c_Tmp,0x00,512);
				   MEMSET(sc_tmp,0x00,512);
				   
				   si_tmp = ud_rDistToChar(pMe->m_Audio[2].photoname,'/');
					if (si_tmp >= 0)
					{
						STRCPY(sc_tmp,pMe->m_Audio[2].photoname+si_tmp+1);
						STR_TO_WSTR(sc_tmp,c_Tmp,512);
					}
					else
					{
						STR_TO_WSTR(pMe->m_photo[2].photoname,c_Tmp,512);
					}

			//	   STRTOWSTR(pMe->m_Audio[2].photoname,c_Tmp,512);
				   IMENUCTL_AddItem( pMenuCtl_N, NULL, NULL, 2, c_Tmp, (uint32)pMenuCtl_N );
			   }

			   if (STRLEN(pMe->m_Audio[3].photoname) > 0)
			   {
				   MEMSET(c_Tmp,0x00,512);
				   MEMSET(sc_tmp,0x00,512);

				   si_tmp = ud_rDistToChar(pMe->m_Audio[3].photoname,'/');
					if (si_tmp >= 0)
					{
						STRCPY(sc_tmp,pMe->m_Audio[3].photoname+si_tmp+1);
						STR_TO_WSTR(sc_tmp,c_Tmp,512);
					}
					else
					{
						STR_TO_WSTR(pMe->m_photo[3].photoname,c_Tmp,512);
					}

		//		   STRTOWSTR(pMe->m_Audio[3].photoname,c_Tmp,512);
				   IMENUCTL_AddItem( pMenuCtl_N, NULL, NULL, 3, c_Tmp, (uint32)pMenuCtl_N );
			   }

			   if (STRLEN(pMe->m_Audio[4].photoname) > 0)
			   {
				   MEMSET(c_Tmp,0x00,512);
				   MEMSET(sc_tmp,0x00,512);

				   si_tmp = ud_rDistToChar(pMe->m_Audio[4].photoname,'/');
					if (si_tmp >= 0)
					{
						STRCPY(sc_tmp,pMe->m_Audio[4].photoname+si_tmp+1);
						STR_TO_WSTR(sc_tmp,c_Tmp,512);
					}
					else
					{
						STR_TO_WSTR(pMe->m_photo[4].photoname,c_Tmp,512);
					}

					
			//	   STRTOWSTR(pMe->m_Audio[4].photoname,c_Tmp,512);
				   IMENUCTL_AddItem( pMenuCtl_N, NULL, NULL, 4, c_Tmp, (uint32)pMenuCtl_N );
			   }

			   FREE(sc_tmp);
			   sc_tmp = NULL;

			   SETAEERECT(&rRect, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - m_nFontHeight-10);

				 // SETAEERECT( &rRect, 0, ( pMe->m_rScreenRect.dy + pApp->m_rScreenRect.y ) - MAIN_MENU_HEIGHT, pApp->m_rScreenRect.dx, MAIN_MENU_HEIGHT );
				  IMENUCTL_SetRect( pMenuCtl_N, &rRect );
			
			   /////////   end ////

				

               	IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_SEND,0, NULL, (uint32)pMenuCtl );

			  SETAEERECT(&rRect, 0, pMe->DeviceInfo.cyScreen - m_nFontHeight-10, pMe->DeviceInfo.cxScreen, m_nFontHeight+10);

				 // SETAEERECT( &rRect, 0, ( pMe->m_rScreenRect.dy + pApp->m_rScreenRect.y ) - MAIN_MENU_HEIGHT, pApp->m_rScreenRect.dx, MAIN_MENU_HEIGHT );
				  IMENUCTL_SetRect( pMenuCtl, &rRect );



				  // Activate the control and draw it on the screen

			   IMENUCTL_SetActive( pMenuCtl_N, TRUE );
			   IMENUCTL_SetActive( pMenuCtl, FALSE );



				IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
				IDISPLAY_Update (pMe->a.m_pIDisplay);
				testgps_SetScreenData( pMe, (void *)pMenuCtl_N );
               testgps_SetScreenData_Audio( pMe, (void *)pMenuCtl );
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

         testgps_SetScreenData( pMe, 0 );
		 testgps_SetScreenData_Audio( pMe, 0 );
         pMe->currentHandler = 0;

		 pMenuCtl = NULL;
         bHandled = TRUE;
      }
      else if( wParam == SCREEN_PARAM_PAINT ) {

		  IMENUCTL_SetActive( pMenuCtl_N, TRUE );
          
		  IMENUCTL_Redraw( pMenuCtl_N );
		  IMENUCTL_Redraw( pMenuCtl );
         }

      
	  
      break;

   case EVT_KEY:
	   {
		   
	   

			bHandled = IMENUCTL_HandleEvent( pMenuCtl_N, eCode, wParam, dwParam );		   
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "视频 触发 EVT_KEY_RELEASE");
	//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "触发 EVT_KEY_RELEASE");

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
					pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
					ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
					pMe->m_FOBFlag = TRUE; //在后台运行
					pMe->m_WitchPag = 0;
					//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"后台运行 6:%d",pMe->m_WitchPag);
					bHandled = TRUE;
				    return TRUE;
				}
			}
			//////////////////////////////////////
			//////////////////////////////////////
			if (wParam == AVK_END)
			{
				if (STRCMP(pMe->sMobileType,"55") == 0 || (STRCMP(pMe->sMobileType,"03") == 0) || (STRCMP(pMe->sMobileType,"58") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0) || (STRCMP(pMe->sMobileType,"57") == 0) )
				{
					/*
						if (pMenuCtl)
					  {
						  IMENUCTL_Release( pMenuCtl );
					  }
					 if (pMenuCtl_N)
					  {
						  IMENUCTL_Release( pMenuCtl_N );
					  }
			 
					*/
					// 这里直接回到后台
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
					   pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
					   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
					   pMe->m_FOBFlag = TRUE; //在后台运行
					   pMe->m_WitchPag = 0;
			//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"后台运行 6:%d",pMe->m_WitchPag);
					   bHandled = TRUE;
					   return TRUE;
				}
			}
			//////////////////////////////////////
			//////////////////////////////////////
		   
		   
		   // AVK_POWER   AVK_END
		   if( wParam == AVK_END  || wParam == AVK_CLR ) 
		   {
	//		   IMENUCTL_Release( pMe->m_PhotoTextCtl );
	//		   IMENUCTL_Release( pMenuCtl );
	 //          pMe->currentHandler = 0;
	//		   testgps_SetScreenData( pMe, 0 );

			   pMe->m_WitchPag = 10; // 返回主界面
			   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
	//		   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
	//		   pMe->m_FOBFlag = TRUE; //在后台运行
			   pMe->m_WitchPag = 10;
			   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"返回主界面 3:%d",pMe->m_WitchPag);
			   bHandled = TRUE;
		   }

	   }
	   
		
	   break;
	   /*
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
		  bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
		  if( wParam == AVK_END  || wParam == AVK_CLR ) {
			  ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
			  pMe->m_FOBFlag = TRUE; //在后台运行
			  pMe->m_WitchPag = 0;
			  bHandled = TRUE;
	   }
      break;
	  */

   case EVT_COMMAND:
      if( dwParam == (uint32)pMenuCtl_N ) {
         switch( wParam ) {

		case 0: // 发送 第一条
		case 1:	
		case 2:
		case 3:
		case 4:			{
			//	if (pMe->iSendPhoto== 1)
			//	{	// 
					pMe->m_bSendPF = TRUE ;//用来标记是否正在发送图片的标志位 TRUE -- 表示正在发送状态  FALSE -- 表示空闲

					pMe->iSendPhoto = 2;
					DBGPRINTF( "zjie-send 0" );

					// 释放与选择照片有关的控件资源
	//				uf_ReleaseCtl(pMe,FALSE);
					// 发送照片

					////  IMENUCTL_SetActive( pMenuCtl_N, TRUE );
					IMENUCTL_SetActive( pMenuCtl_N, FALSE );

					pMe->m_WitchPag = 25;// 锁屏 [3/16/2016 Administrator]
					Send_ing(pMe);

				//	me->m_pParent->sFile
					
					STRCPY(pMe->sFile,pMe->m_Audio[wParam].photoname);
		


				//  Change by ----  2012-10-16			uf_SendPhoto(pMe);
					if ( FleUpd_HandleEvent(pMe,FLDUPD_EVT_START,0,0) )	 {
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"shang  chuan jin lai Audio");
						ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,FLDUPD_EVT_UPLOAD,49,0); // 52这个数值就是代表 ‘4’
					}
				// Change End
		//		}
			}
			return TRUE;
			break;
         default:
            break;
         }
      }
      break;
   }
   return bHandled;
}






