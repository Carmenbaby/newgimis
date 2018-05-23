/*==========================================================================================
  gmTools.c
      brew�����õ��Ĺ��ú���
===========================================================================================*/
#include "gmTools.h"

#define ARRAYSIZE(a)   (sizeof(a) / sizeof((a)[0]))
#define min(a,b)  ((a) < (b) ? (a) : (b))
#define max(a,b)  ((a) > (b) ? (a) : (b))

// =================================================================
// �����ַ���ת��Ϊ���ֽ��ַ��� 
// �������
// pIn�� ָ�����ת��Ϊ���ֽ��ַ������� null ��β�Ŀ��ַ�����ָ�� 
// pszDest�� ָ��Ҫ���յ��ֽ��ַ�����Ŀ�껺������ָ�� 
// nSize�� pDest �������Ĵ�С���ֽڣ��� �����ֵΪ 0���������������κ�ת��
void gm_WSTRTOSTR(AECHAR *pIn, char *pszDest, int nSize,IShell *pShell)
{
	uint16 idx = 0;
	uint16 nAELen = 0;			// ���ַ����ĳ���
	uint16 ndx = 0;			    // ���ֽ��ַ��������λ��		
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
// ����Ļ����ʾ�ַ���
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
// ����Ļ����ʾ��Ϣ
===========================================================================*/
void gm_Printf( cFont oFont, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, char *szBuf )
{

	AEEDeviceInfo di;
	AEERect rc;
	int nMaxLines;
	int nCPixel;		// ÿ���ַ�����ռ������
	
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
// ����Ļ����ʾ˫�ֽ���Ϣ
===========================================================================*/
void gm_WPrintf( cFont oFont, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *szText,RGBVAL DataColor)
{
	AEEDeviceInfo di;
	AEERect rc;
	int nMaxLines;
	RGBVAL iColor;
	
	int nAEChars;
	int nCPixel;		// ÿ���ַ�����ռ������

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
 �ڱ��ؼ�¼��־
�������
  cgimis * pMe
  IShell *pShell  :  
  cWrtLog         :   1  д����־�� ��1 ����qxdm����
  uint16 *pIndex  :   ����������ʾ��־�г���ʼִ�е�λ�ã��ɵ��õĳ��򱣴��ֵ
  const char *szFormat, ...  �� ���¼���ַ���
********************************************************************************************/
void ud_RecLog(IShell *pShell,char cWrtLog, uint32 *pIndex, const char *szFormat, ... )
{
	char sDateTime[20];
	int lInt_Len;
	char    lStr_msg[MAX_MESSAGE_LEN+sizeof(sDateTime)+4+5];
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	
	IFileMgr		*pFileMgr;
	IFile			*pTempFile;
    FileInfo fiInfo;		// �ļ���Ϣ

	va_list args;

// ��������DBGPRINTF֮�� ��������Ϣ�����ڱ� 	if ( cWrtLog == '0' ) return;		// 2013-04-27 ���ڴ˴�

   // ��¼д��־��ʱ��
   GETJULIANDATE(0,&dDate);		//	ȡ������	
   MEMSET(sDateTime,0,sizeof(sDateTime));
   // yyyy-mm-dd hh:MM:ss    ��19���ֽ�
   SPRINTF(sDateTime,"%04d-%02d-%02d %02d:%02d:%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
   
   MEMSET(lStr_msg,0,sizeof(lStr_msg));
   SPRINTF(lStr_msg,"%d[%s]:",*pIndex,sDateTime);
   lInt_Len = STRLEN(lStr_msg);

	va_start( args, szFormat );
	(void)VSNPRINTF( lStr_msg+lInt_Len, MAX_MESSAGE_LEN, szFormat, args );
	va_end( args );

	DBGPRINTF("----=%s",lStr_msg);

	if ( cWrtLog == '0' ) return;		// 2013-10-09  ��������Ϣ�����ڱ�

	 STRCAT(lStr_msg,"\r\n");

	// �����ļ��ӿ�
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
 �ڱ��ؼ�¼��־
�������
  cgimis * pMe
  IShell *pShell  :  
  cWrtLog         :   1  д����־�� ��1 ����qxdm����
  uint16 *pIndex  :   ����������ʾ��־�г���ʼִ�е�λ�ã��ɵ��õĳ��򱣴��ֵ
  const char *szFormat, ...  �� ���¼���ַ���
********************************************************************************************/
void ud_RecLog_c(IShell *pShell,char cWrtLog, uint32 *pIndex, const char *szFormat, ... )
{
	char sDateTime[20];
	int lInt_Len;
	char    lStr_msg[MAX_MESSAGE_LEN+sizeof(sDateTime)+4+5];
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	
	IFileMgr		*pFileMgr;
	IFile			*pTempFile;
    FileInfo fiInfo;		// �ļ���Ϣ

	va_list args;

// ��������DBGPRINTF֮�� ��������Ϣ�����ڱ� 	if ( cWrtLog == '0' ) return;		// 2013-04-27 ���ڴ˴�

   // ��¼д��־��ʱ��
   GETJULIANDATE(0,&dDate);		//	ȡ������	
   MEMSET(sDateTime,0,sizeof(sDateTime));
   // yyyy-mm-dd hh:MM:ss    ��19���ֽ�
   SPRINTF(sDateTime,"%04d-%02d-%02d %02d:%02d:%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
   
   MEMSET(lStr_msg,0,sizeof(lStr_msg));
   SPRINTF(lStr_msg,"%d[%s]:",*pIndex,sDateTime);
   lInt_Len = STRLEN(lStr_msg);

	va_start( args, szFormat );
	(void)VSNPRINTF( lStr_msg+lInt_Len, MAX_MESSAGE_LEN, szFormat, args );
	va_end( args );

	DBGPRINTF("----=%s",lStr_msg);

	if ( cWrtLog == '0' ) return;		// 2013-10-09  ��������Ϣ�����ڱ�

	 STRCAT(lStr_msg,"\r\n");

	// �����ļ��ӿ�
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
' ��������:
'	����ַ���ǰ��Ŀո�
'
'�������
'   rStr_Src  ��������ַ���
'
' ��������
'   rStr_Src  ����ո����ַ���
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
	// ��ʼλ��
	for (iL = 0; iL < lInt_Len; iL++) {
		if (lpStr_Trg[iL] != 32) {
			lInt_b = iL;
			break;
		}
	}
	lInt_e = -1;
	// ����λ��
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
// �ַ�����ָ���ַ���һ�γ��ֵ�λ��,��0��ʼ����, -1 û�ҵ� 
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
// �ַ�����ָ���ַ������ֵ�λ��,λ�ô�0��ʼ����, -1 û�ҵ� 
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
// ���޷����ַ���ת��ASC���ֵ��ʾ(��ӡ)
//  �ռ�Ӧ�ô���ǰ����
//  const char * psSrc : Դ�ַ���
//  int nSrcLen �� Դ�ַ�������
//  char *psDestr : Ŀ���ַ���
//  int nDestLen : Ŀ���ַ�������
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
// ���ַ���ת��ASC���ֵ��ʾ(��ӡ,��,�ָ�)
//  �ռ�Ӧ�ô���ǰ����
//  const char * psSrc : Դ�ַ���
//  int nSrcLen �� Դ�ַ�������
//  char *psDestr : Ŀ���ַ���
//  int nDestLen : Ŀ���ַ�������
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
// ���ַ���ת��16����ASC���ֵ
//  �ռ�Ӧ�ô���ǰ����
//  const char * psSrc : Դ�ַ���
//  int nSrcLen �� Դ�ַ�������
//  char *psDestr : Ŀ���ַ���
//  int nDestLen : Ŀ���ַ�������
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
���ָ��(���ַ���β)���ַ�����
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
// �Ӵ�����ַ�����ȡ�����ַ���������ָ���Ƶ��ָ�������һ��λ��
//  char *sSource : Դ�ַ���
//  int nSrcLen   : Դ�ַ����ĳ���(���������ַ�0)
//  char *sSub    : ���ַ���
//  int nSubLen   : ���ַ�������󳤶�
//  char cSplit   : �ָ��ַ�
//  ���� : �ָ�������һ��λ��
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
	else {	// û�зָ���
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
//����2������֮�������
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

//����������ʼ�굽��ֹ���������������
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

//����������ʼ�·ݵ�������ʣ������
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

//����������ֹ���ڳ�������1��1�յ�����
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
//����2������֮�������
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

// ���ݵ�����ҹ��ʼ�ĺ��������������ʱ������ʱ��
JulianType gm_GetDateFromTimems(uint32 nCurTm)
{

	JulianType dRtnDate;			  // ���ص�����ʱ��
	uint32 nTimes = GETTIMEMS();
	uint32 nSeconds = GETTIMESECONDS();			// 1980 �� 1 �� 6 ����ҹ (GMT) ������������ѵ���Ϊ����ʱ�䣩�� 
	uint32 nTmp = 0;

	if (nTimes < nCurTm) 	// ������ҹ
		nTimes += (24*60*60*1000);
	
	nTmp = (nTimes - nCurTm) / 1000;				// �������� 
	nSeconds -= nTmp;

	GETJULIANDATE(nSeconds, &dRtnDate);

	return dRtnDate;
}

