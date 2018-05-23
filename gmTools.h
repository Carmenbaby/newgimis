/*======================================================
FILE:  gmTools.h
    brew�����õ��Ĺ��ú���
=====================================================*/
#ifndef __GMTOOLS_H__
#define __GMTOOLS_H__

#include "AEE.h"
#include "AEEAppGen.h"
#include "AEEStdLib.h"
#include "AEEShell.h"           // Shell interface definitions
#include "AEEFile.h"			// File interface definitions

#include "AEEStdLib.h"

#define  LOG_FILENAME		"Rec_Info.log"				// ��־�ļ���
#define  LOG_FILENAME_C		"Total.log"				// ��־�ļ���

#define  MAX_MESSAGE_LEN	1024						// ��־ÿ����¼����󳤶�
#define  MAX_SCREEN_CHARS	50							// ��Ļ��ÿ�������ʾ���ַ���

typedef struct _gmFont cFont;


// �й���Ļ���������Ϣ
struct _gmFont
{
	 AEEApplet theApp;
	 int iFontHeight;			//  ����ĸ߶�
	 int iFontWidth;		    //  ����Ŀ��
};



/*********************************************************
//	����ַ���ǰ��Ŀո�
//
// �������
//   rStr_Src  ��������ַ���
//
// ��������
//   rStr_Src  ����ո����ַ���
*****************************************************************************/
void ud_Trim(char *rStr_Src);

/*********************************************************
// �ַ�����ָ���ַ���һ�γ��ֵ�λ��,��0��ʼ����, -1 û�ҵ� 
// �������
//   pszStr  �����ҵ�Դ�ַ���
//   cChar   �����ҵ��ַ�
// ��������
//   �ַ�λ�� ,��0��ʼ����, -1 û�ҵ� 
*****************************************************************************/
int ud_DistToChar(const char * pszStr, char cChar);

// ==========================================================
// �ַ�����ָ���ַ������ֵ�λ��,λ�ô�0��ʼ����, -1 û�ҵ� 
// �������
//   pszStr  �����ҵ�Դ�ַ���
//   cChar   �����ҵ��ַ�
// ��������
//   �ַ�λ�� ,��0��ʼ����, -1 û�ҵ� 
// ==========================================================
int ud_rDistToChar(const char * pszStr, char cChar);

//���ָ��(���ַ���β)���ַ�����
int ud_DistToSplit(const char * pszStr,const char *cSplit);

/* ======================================================================= 
// �Ӵ�����ַ�����ȡ�����ַ���������ָ���Ƶ��ָ�������һ��λ��
//  char *sSource : Դ�ַ���
//  int nSrcLen   : Դ�ַ����ĳ���(���������ַ�0)
//  char *sSub    : ���ַ���
//  int nSubLen   : ���ַ�������󳤶�
//  char cSplit   : �ָ��ַ�
//  ���� : �ָ�������һ��λ��
=======================================================================*/
char* uf_GetSubStr(char *sSource,int nSrcLen,char *sSub, int nSubLen,const char *sSplit);

/********************************************************************************************
 �ڱ��ؼ�¼��־
�������
  cgimis * pMe
  IShell *pShell  :  
  cWrtLog         :   1  д����־�� ��1 ����qxdm����
  int *pIndex		  :   ����������ʾ��־�г���ʼִ�е�λ�ã��ɵ��õĳ��򱣴��ֵ
  const char *szFormat, ...  �� ���¼���ַ���
********************************************************************************************/
void ud_RecLog(IShell *pShell,char cWrtLog, uint32 *pIndex, const char *szFormat, ... );
// ================================================================================================
// ����Ļ����ʾ���ֽ���Ϣ
void gm_Printf( cFont oFont, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, char *szBuf );
// ����Ļ����ʾ˫�ֽ���Ϣ
void gm_WPrintf( cFont oFont, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *szText,RGBVAL DataColor);

// �����ַ���ת��Ϊ���ֽ��ַ��� 
void gm_WSTRTOSTR(AECHAR *pIn, char *pszDest, int nSize,IShell *pShell);
// ���ַ���ת��ASC���ֵ��ʾ(��ӡ,��,�ָ�)
void ud_ToAscFmt(const char * psSrc, int nSrcLen, char *psDestr, int nDestLen);
// ���ַ���ת��16����ASC���ֵ
void ud_ToAscFmtWithNone(const char * psSrc, int nSrcLen, char *psDestr, int nDestLen);

// ���޷����ַ���ת��ASC���ֵ��ʾ(��ӡ)
void ud_ucharToAscFmt(const uint8 * psSrc, int nSrcLen, char *psDestr, int nDestLen);


//����2������֮�������
int gm_countDay(JulianType date1,JulianType date2);
// ���ݵ�����ҹ��ʼ�ĺ��������������ʱ������ʱ��
JulianType gm_GetDateFromTimems(uint32 nCurTm);

#endif /* !__GMTOOLS_H__ */
