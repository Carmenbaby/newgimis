/*==========================================================================================
  gmTools.c
      brew开发用到的公用函数
===========================================================================================*/
#include "gmTools.h"

#define ARRAYSIZE(a)   (sizeof(a) / sizeof((a)[0]))
#define min(a,b)  ((a) < (b) ? (a) : (b))
#define max(a,b)  ((a) > (b) ? (a) : (b))

// =================================================================
// 将宽字符串转换为单字节字符串 
// 传入参数
// pIn： 指向必须转换为单字节字符串并以 null 结尾的宽字符串的指针 
// pszDest： 指向要接收单字节字符串的目标缓冲区的指针 
// nSize： pDest 缓冲区的大小（字节）。 如果该值为 0，函数将不进行任何转换
void gm_WSTRTOSTR(AECHAR *pIn, char *pszDest, int nSize,IShell *pShell)
{
	uint16 idx = 0;
	uint16 nAELen = 0;			// 宽字符串的长度
	uint16 ndx = 0;			    // 单字节字符串的填充位置		
	char *sCopyStr = NULL;
	uint32 nInd = 1000;
	char cWrtLog = '1';

	if (nSize < 1) return;
	nAELen = WSTRLEN(pIn) * sizeof(AECHAR);
	if (nSize < nAELen ) return;

     ud_RecLog(pShell,cWrtLog,&(nInd),"nAELen = %d",nAELen);

	sCopyStr = (char *)MALLOC(nAELen+1);
	MEMSET(sCopyStr,0,(nAELen+1));
	MEMCPY(sCopyStr,pIn,nAELen);

	MEMSET(pszDest,0,nSize);
	for(idx = 0; idx < nAELen; idx=(idx+2))
	{
		int i = 1;
		ud_RecLog(pShell,cWrtLog,&(nInd),"idx = %d",idx);
		for (i = 1; i>=0; i--) {		// for -- 1
			if (sCopyStr[idx+i] != 0) {
				pszDest[ndx] = sCopyStr[idx+i];
				ud_RecLog(pShell,cWrtLog,&(nInd),"i=%d,pszDest[%d]=%d",i,ndx,pszDest[ndx]);

				ndx++;
			}
		}		// for -- 1
	}

	FREEIF(sCopyStr);
}

// ================================================
// 在屏幕上显示字符串
static void xDrawTextA(IDisplay * pd,AEEFont fnt, const char * pszText, int nChars,
                       int x,int y,const AEERect * prcBackground,uint32 dwFlags)
{
	AECHAR wcBuf[MAX_SCREEN_CHARS*2+2];				// 
	
	if (nChars < 0)
		nChars = STRLEN(pszText);
	nChars = min(nChars, ARRAYSIZE(wcBuf));
	
	STR_TO_WSTR((char*)pszText, wcBuf, sizeof(wcBuf));
	
	IDISPLAY_DrawText(pd, fnt, wcBuf, nChars, x, y, prcBackground, dwFlags);
}
/*===========================================================================
// 在屏幕上显示信息
===========================================================================*/
void gm_Printf( cFont oFont, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, char *szBuf )
{

	AEEDeviceInfo di;
	AEERect rc;
	int nMaxLines;
	int nCPixel;		// 每个字符横向占的像素
	
	if (oFont.iFontHeight == 0)  oFont.iFontHeight = 16;
	if  (oFont.iFontWidth == 0)  oFont.iFontWidth = 22;
	
	// Change by ---- 2011-10-20	ISHELL_GetDeviceInfo(pMe->m_pIShell,&di);
	ISHELL_GetDeviceInfo(oFont.theApp.m_pIShell ,&di);
	nMaxLines = (di.cyScreen / oFont.iFontHeight) - 2;
	if (nMaxLines < 1)
		nMaxLines = 1;
	
	nCPixel = di.cxScreen / oFont.iFontWidth;
	rc.x = nCol * nCPixel;
	rc.dx = di.cxScreen - nCol * nCPixel;
	rc.y = nLine * oFont.iFontHeight;
	
	if( dwFlags & IDF_ALIGNVERT_MASK ) {
		rc.dy = di.cyScreen - rc.y;
	}
	else {
		rc.dy = oFont.iFontHeight;
	}
	
	xDrawTextA(oFont.theApp.m_pIDisplay,fnt,szBuf, -1, rc.x, rc.y, &rc, dwFlags);
	IDISPLAY_Update(oFont.theApp.m_pIDisplay);
	
}

/*===========================================================================
// 在屏幕上显示双字节信息
===========================================================================*/
void gm_WPrintf( cFont oFont, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *szText,RGBVAL DataColor)
{
	AEEDeviceInfo di;
	AEERect rc;
	int nMaxLines;
	RGBVAL iColor;
	
	int nAEChars;
	int nCPixel;		// 每个字符横向占的像素

	if (oFont.iFontHeight == 0)  oFont.iFontHeight = 16;
	if  (oFont.iFontWidth == 0)  oFont.iFontWidth = 22;
	
	ISHELL_GetDeviceInfo(oFont.theApp.m_pIShell,&di);
	nMaxLines = (di.cyScreen / oFont.iFontHeight) - 2;
	if (nMaxLines < 1)	nMaxLines = 1;
	
	nCPixel = di.cxScreen / oFont.iFontWidth;
	rc.x = nCol * nCPixel;
	rc.dx = di.cxScreen - nCol * nCPixel;
	rc.y = nLine * oFont.iFontHeight;
	
	
	if( dwFlags & IDF_ALIGNVERT_MASK ) {
		rc.dy = di.cyScreen - rc.y;
	}
	else {
		rc.dy = oFont.iFontHeight;
	}
	
	nAEChars = WSTRLEN(szText);
	iColor = IDISPLAY_SetColor(oFont.theApp.m_pIDisplay,CLR_USER_BACKGROUND,DataColor);		//  CONTENT_COLOR
	IDISPLAY_DrawText(oFont.theApp.m_pIDisplay, fnt, szText, nAEChars, rc.x, rc.y, &rc, dwFlags);
	IDISPLAY_SetColor(oFont.theApp.m_pIDisplay,CLR_USER_BACKGROUND,iColor);
	IDISPLAY_Update(oFont.theApp.m_pIDisplay);
	
}


/********************************************************************************************
 在本地记录日志
传入参数
  cgimis * pMe
  IShell *pShell  :  
  cWrtLog         :   1  写入日志， 非1 仅供qxdm调试
  uint16 *pIndex  :   计数器，表示日志中程序开始执行的位置，由调用的程序保存此值
  const char *szFormat, ...  ： 需记录的字符串
********************************************************************************************/
void ud_RecLog(IShell *pShell,char cWrtLog, uint32 *pIndex, const char *szFormat, ... )
{
	char sDateTime[20];
	int lInt_Len;
	char    lStr_msg[MAX_MESSAGE_LEN+sizeof(sDateTime)+4+5];
	JulianType dDate;				// 记录收到定位数据的时间
	
	IFileMgr		*pFileMgr;
	IFile			*pTempFile;
    FileInfo fiInfo;		// 文件信息

	va_list args;

// 放在下面DBGPRINTF之后 还是有信息利大于弊 	if ( cWrtLog == '0' ) return;		// 2013-04-27 改在此处

   // 记录写日志的时间
   GETJULIANDATE(0,&dDate);		//	取出日期	
   MEMSET(sDateTime,0,sizeof(sDateTime));
   // yyyy-mm-dd hh:MM:ss    共19个字节
   SPRINTF(sDateTime,"%04d-%02d-%02d %02d:%02d:%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
   
   MEMSET(lStr_msg,0,sizeof(lStr_msg));
   SPRINTF(lStr_msg,"%d[%s]:",*pIndex,sDateTime);
   lInt_Len = STRLEN(lStr_msg);

	va_start( args, szFormat );
	(void)VSNPRINTF( lStr_msg+lInt_Len, MAX_MESSAGE_LEN, szFormat, args );
	va_end( args );

	DBGPRINTF("----=%s",lStr_msg);

	if ( cWrtLog == '0' ) return;		// 2013-10-09  还是有信息利大于弊

	 STRCAT(lStr_msg,"\r\n");

	// 创建文件接口
	if (ISHELL_CreateInstance(pShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
	{
		if (IFILEMGR_Test(pFileMgr, LOG_FILENAME) == SUCCESS)
			pTempFile = IFILEMGR_OpenFile(pFileMgr, LOG_FILENAME, _OFM_READWRITE);
		else
			pTempFile = IFILEMGR_OpenFile(pFileMgr, LOG_FILENAME, _OFM_CREATE);
	}
	else
		return;

	if (pTempFile != NULL)
	{
		if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) {
// Change by ---- 2013-01-01			if ( fiInfo.dwSize > 1024*500 ) {
			if ( (fiInfo.dwSize > 1024*600) || (cWrtLog == '4') ) {
				IFILE_Truncate(pTempFile, 0);
			}
		}

	   IFILE_Seek(pTempFile,_SEEK_END,0);
	   lInt_Len = STRLEN(lStr_msg);
	   IFILE_Write(pTempFile, lStr_msg, lInt_Len);
	   *pIndex = ( *pIndex > 50000 ) ? 1 : (*pIndex)+1;
	}


   // Free the IFileMgr and IFile instances
	if (pTempFile != NULL)
	   IFILE_Release( pTempFile );

	if (pFileMgr != NULL)
	   IFILEMGR_Release( pFileMgr );
}



/********************************************************************************************
 在本地记录日志
传入参数
  cgimis * pMe
  IShell *pShell  :  
  cWrtLog         :   1  写入日志， 非1 仅供qxdm调试
  uint16 *pIndex  :   计数器，表示日志中程序开始执行的位置，由调用的程序保存此值
  const char *szFormat, ...  ： 需记录的字符串
********************************************************************************************/
void ud_RecLog_c(IShell *pShell,char cWrtLog, uint32 *pIndex, const char *szFormat, ... )
{
	char sDateTime[20];
	int lInt_Len;
	char    lStr_msg[MAX_MESSAGE_LEN+sizeof(sDateTime)+4+5];
	JulianType dDate;				// 记录收到定位数据的时间
	
	IFileMgr		*pFileMgr;
	IFile			*pTempFile;
    FileInfo fiInfo;		// 文件信息

	va_list args;

// 放在下面DBGPRINTF之后 还是有信息利大于弊 	if ( cWrtLog == '0' ) return;		// 2013-04-27 改在此处

   // 记录写日志的时间
   GETJULIANDATE(0,&dDate);		//	取出日期	
   MEMSET(sDateTime,0,sizeof(sDateTime));
   // yyyy-mm-dd hh:MM:ss    共19个字节
   SPRINTF(sDateTime,"%04d-%02d-%02d %02d:%02d:%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
   
   MEMSET(lStr_msg,0,sizeof(lStr_msg));
   SPRINTF(lStr_msg,"%d[%s]:",*pIndex,sDateTime);
   lInt_Len = STRLEN(lStr_msg);

	va_start( args, szFormat );
	(void)VSNPRINTF( lStr_msg+lInt_Len, MAX_MESSAGE_LEN, szFormat, args );
	va_end( args );

	DBGPRINTF("----=%s",lStr_msg);

	if ( cWrtLog == '0' ) return;		// 2013-10-09  还是有信息利大于弊

	 STRCAT(lStr_msg,"\r\n");

	// 创建文件接口
	if (ISHELL_CreateInstance(pShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
	{
		if (IFILEMGR_Test(pFileMgr, LOG_FILENAME_C) == SUCCESS)
			pTempFile = IFILEMGR_OpenFile(pFileMgr, LOG_FILENAME_C, _OFM_READWRITE);
		else
			pTempFile = IFILEMGR_OpenFile(pFileMgr, LOG_FILENAME_C, _OFM_CREATE);
	}
	else
		return;

	if (pTempFile != NULL)
	{
		if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) {
// Change by ---- 2013-01-01			if ( fiInfo.dwSize > 1024*500 ) {
			if ( (fiInfo.dwSize > 1024*600) || (cWrtLog == '4') ) {
				IFILE_Truncate(pTempFile, 0);
			}
		}

	   IFILE_Seek(pTempFile,_SEEK_END,0);
	   lInt_Len = STRLEN(lStr_msg);
	   IFILE_Write(pTempFile, lStr_msg, lInt_Len);
	   *pIndex = ( *pIndex > 50000 ) ? 1 : (*pIndex)+1;
	}


   // Free the IFileMgr and IFile instances
	if (pTempFile != NULL)
	   IFILE_Release( pTempFile );

	if (pFileMgr != NULL)
	   IFILEMGR_Release( pFileMgr );
}


/***************************************************************************
' 功能描述:
'	清除字符串前后的空格
'
'传入参数
'   rStr_Src  待清除的字符串
'
' 传出参数
'   rStr_Src  清除空格后的字符串
*****************************************************************************/
void ud_Trim(char *rStr_Src)
{
	int lInt_Len;
	int iL,lInt_b,lInt_e;
	char *lpStr_Trg;
	
	lInt_Len = STRLEN(rStr_Src);
	if (lInt_Len == 0) return;
	
	lpStr_Trg = (char *)MALLOC(lInt_Len+1);
	MEMSET(lpStr_Trg,0, (lInt_Len+1) );
	STRCPY(lpStr_Trg,rStr_Src);
	lInt_b = 0;
	// 开始位置
	for (iL = 0; iL < lInt_Len; iL++) {
		if (lpStr_Trg[iL] != 32) {
			lInt_b = iL;
			break;
		}
	}
	lInt_e = -1;
	// 结束位置
	for ( iL = (lInt_Len-1); iL >=0; iL--) {
		if (lpStr_Trg[iL] != 32) {
			lInt_e = iL;
			break;
		}
	}
	
	MEMSET(rStr_Src,0,lInt_Len);
	if ( (lInt_e - lInt_b) >=0 )
		MEMCPY(rStr_Src, lpStr_Trg+lInt_b, (lInt_e - lInt_b+1));
	
	FREE(lpStr_Trg);
	
}

// ==========================================================
// 字符串中指定字符第一次出现的位置,从0开始计数, -1 没找到 
int ud_DistToChar(const char * pszStr, char cChar)
{
	int nCount = 0;
	
	if ( !pszStr ) { 
		return -1;
	}
	
	while ( *pszStr != 0 ) {
		if ( *pszStr == cChar ) {
			return nCount;
		}
		else {
			nCount++;
			pszStr++;
		}
	}
	
	return -1;
}

// ==========================================================
// 字符串中指定字符最后出现的位置,位置从0开始计数, -1 没找到 
// ==========================================================
int ud_rDistToChar(const char * pszStr, char cChar)
{
	int nLen;
	int idx = 0;
	
	if ( !pszStr ) { 
		return -1;
	}
	
	nLen = STRLEN(pszStr);
	
	for (idx = nLen; idx > 0; idx-- )
	{
		if ( *(pszStr+idx-1) == cChar )
			return (idx-1);
	}
	
	return -1;
	
}

// ======================================================
// 把无符号字符串转成ASC码的值显示(打印)
//  空间应该传入前分配
//  const char * psSrc : 源字符串
//  int nSrcLen ： 源字符串长度
//  char *psDestr : 目标字符串
//  int nDestLen : 目标字符串长度
void ud_ucharToAscFmt(const uint8 * psSrc, int nSrcLen, char *psDestr, int nDestLen)
{
	int idx;
	int nLoc = 0, nLen;
	
	MEMSET(psDestr,0,nDestLen);
	for(idx=0;idx<nSrcLen;idx++)
	{
		char sTmp[5];
		SPRINTF(sTmp,"%d,",psSrc[idx]);
		nLen = STRLEN(sTmp);
		MEMCPY(psDestr+nLoc,sTmp,nLen);
		nLoc += nLen;
		if (nLoc >= nDestLen) 
			break;
	}
	
	if (nLoc > 0)
		psDestr[nLoc-1] = 0;
	
	return;
}


// ======================================================
// 把字符串转成ASC码的值显示(打印,带,分隔)
//  空间应该传入前分配
//  const char * psSrc : 源字符串
//  int nSrcLen ： 源字符串长度
//  char *psDestr : 目标字符串
//  int nDestLen : 目标字符串长度
void ud_ToAscFmt(const char * psSrc, int nSrcLen, char *psDestr, int nDestLen)
{
	int idx;
	int nLoc = 0, nLen;

	MEMSET(psDestr,0,nDestLen);
	for(idx=0;idx<nSrcLen;idx++)
	{
		char sTmp[5];
		SPRINTF(sTmp,"%02x,",psSrc[idx]);
		nLen = STRLEN(sTmp);
		MEMCPY(psDestr+nLoc,sTmp,nLen);
		nLoc += nLen;
		if (nLoc >= nDestLen) 
			break;
	}

	if (nLoc > 0)
		psDestr[nLoc-1] = 0;
	
	return;
}

// ======================================================
// 把字符串转成16进制ASC码的值
//  空间应该传入前分配
//  const char * psSrc : 源字符串
//  int nSrcLen ： 源字符串长度
//  char *psDestr : 目标字符串
//  int nDestLen : 目标字符串长度
void ud_ToAscFmtWithNone(const char * psSrc, int nSrcLen, char *psDestr, int nDestLen)
{
	int idx;
	int nLoc = 0, nLen;
	
	MEMSET(psDestr,0,nDestLen);
	for(idx=0;idx<nSrcLen;idx++)
	{
		char sTmp[5];
		SPRINTF(sTmp,"%02x",psSrc[idx]);
		nLen = STRLEN(sTmp);
		MEMCPY(psDestr+nLoc,sTmp,nLen);
		nLoc += nLen;
		if (nLoc >= nDestLen) 
			break;
	}
	
	return;
}

/* ======================================================================= 
到分割符(或字符串尾)的字符数，
=======================================================================*/
int ud_DistToSplit(const char * pszStr,const char *cSplit)
{
	int nCount = 0;
	
	if ( !pszStr ) { 
		return -1;
	}
	
	while ( *pszStr != 0 ) {
		if ( *pszStr == *cSplit ) {
			return nCount;
		}
		else {
			nCount++;
			pszStr++;
		}
	}
	
	return nCount;	
}

/* ======================================================================= 
// 从传入的字符串中取出子字符串，并把指针移到分隔符的下一个位置
//  char *sSource : 源字符串
//  int nSrcLen   : 源字符串的长度(不含结束字符0)
//  char *sSub    : 子字符串
//  int nSubLen   : 子字符串的最大长度
//  char cSplit   : 分隔字符
//  返回 : 分隔符的下一个位置
=======================================================================*/
char* uf_GetSubStr(char *sSource,int nSrcLen,char *sSub, int nSubLen,const char *sSplit)
{
	char *pStr = NULL;
	int nResult = ud_DistToSplit( sSource, sSplit );
	
    MEMSET(sSub,0,nSubLen);
	if (nResult > 0) {
		if (nSubLen > nResult)
			MEMCPY(sSub,sSource,nResult);
		else
			MEMCPY(sSub,sSource,nSubLen);
		pStr = sSource + nResult+1;
		
	}
	else {	// 没有分隔符
		if (nSubLen > nSrcLen)
			MEMCPY(sSub,sSource,nSrcLen);
		else
			MEMCPY(sSub,sSource,nSubLen);
		pStr = sSource + nSrcLen+1;
		
	}
	
		//	ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"uf_GetSubStr_1 out, sSource=%s,nResult=%d,nSrcLen=%d,nSubLen=%d,sSub=%s",sSource,nResult,nSrcLen,nSubLen,sSub);
	return pStr;
}

/* ======================================================================= 
//计算2个日期之间的天数
=======================================================================*/
static boolean IsLeapYear(uint16 ayear)
{
	boolean Is_LeapYear;
	Is_LeapYear=((ayear%4==0&&ayear%100!=0)||(ayear%400==0));
	return Is_LeapYear;
}

int month_fate(JulianType date3)
{
	int nRtn;
	switch(date3.wMonth)
	{
	case 1: 
	case 3: 
	case 5:
	case 7: 
	case 8: 
	case 10: 
	case 12: nRtn = 31;
		break;
	case 4: 
	case 6: 
	case 9: 
	case 11: nRtn = 30;
		break;
	case 2: 
		nRtn = IsLeapYear(date3.wYear) ? 29 : 28;
		break;
	}
	return nRtn;
}

//用来返回起始年到终止年年份上相差的天数
static int return_yearDay(JulianType date1,JulianType date2)
{
	int i=0;
	int yearDay=0;
	int sum_year=0;

	for(i=date1.wYear+1;i<date2.wYear;i++)
	{
		if(IsLeapYear((uint16)i))
		{
			yearDay=366;
		}
		else
			yearDay=365;
		sum_year+=yearDay;
	}
	return sum_year;
}

//用来返回起始月份到年终所剩的天数
static int return_toEnd(JulianType date1)
{
	int i=0;
	int monthday=0;
	int fate=0;
	int fateday=0;
	JulianType dDate;

	dDate = date1;

	for(i=date1.wMonth;i<=12;i++)
	{
		dDate.wMonth = i;
		fate=month_fate(dDate);
		monthday+=fate;
	}

	fateday=monthday-date1.wDay;
	return fateday;
	
}

//用来返回终止日期超过当年1月1日的天数
static int return_fromBegin(JulianType date2)
{
	int i=0;
	int monthday=0;
	int fate=0;
	int overday=0;
	JulianType dDate;
	
	dDate = date2;
	
	for(i=1;i<date2.wMonth;i++)
	{
		dDate.wMonth = i;
		fate=month_fate(date2);
		monthday+=fate;
	}
	overday=monthday+date2.wDay;
	return overday;
}
//计算2个日期之间的天数
int gm_countDay(JulianType date1,JulianType date2)
{
	int nRtn = 1;
	JulianType nDate1, nDate2;
	
	nDate1 = date1;
	nDate2 = date2;

	if ( (date1.wYear>date2.wYear) || 
		 ( (date1.wYear==date2.wYear)&&(date1.wMonth>date2.wMonth) ) || 
		 ( (date1.wYear==date2.wYear)&&(date1.wMonth==date2.wMonth)&&(date1.wDay>date2.wDay) ) )
	{
		nDate2 = date1;
		nDate1 = date2;
		nRtn = -1;
	}


	if (nDate1.wYear == nDate2.wYear)
		nRtn *= return_fromBegin(nDate2) - return_fromBegin(nDate1);
	else
		nRtn *= return_yearDay(nDate1,nDate2)+return_toEnd(nDate1)+return_fromBegin(nDate2);
	return nRtn;
}

// 根据当日午夜开始的毫秒数，推算出当时的日期时间
JulianType gm_GetDateFromTimems(uint32 nCurTm)
{

	JulianType dRtnDate;			  // 返回的日期时间
	uint32 nTimes = GETTIMEMS();
	uint32 nSeconds = GETTIMESECONDS();			// 1980 年 1 月 6 日午夜 (GMT) 至今的秒数（已调整为当地时间）。 
	uint32 nTmp = 0;

	if (nTimes < nCurTm) 	// 过了午夜
		nTimes += (24*60*60*1000);
	
	nTmp = (nTimes - nCurTm) / 1000;				// 相差多少秒 
	nSeconds -= nTmp;

	GETJULIANDATE(nSeconds, &dRtnDate);

	return dRtnDate;
}

