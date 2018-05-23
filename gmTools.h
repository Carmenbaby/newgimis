/*======================================================
FILE:  gmTools.h
    brew开发用到的公用函数
=====================================================*/
#ifndef __GMTOOLS_H__
#define __GMTOOLS_H__

#include "AEE.h"
#include "AEEAppGen.h"
#include "AEEStdLib.h"
#include "AEEShell.h"           // Shell interface definitions
#include "AEEFile.h"			// File interface definitions

#include "AEEStdLib.h"

#define  LOG_FILENAME		"Rec_Info.log"				// 日志文件名
#define  LOG_FILENAME_C		"Total.log"				// 日志文件名

#define  MAX_MESSAGE_LEN	1024						// 日志每条记录的最大长度
#define  MAX_SCREEN_CHARS	50							// 屏幕上每次最多显示的字符数

typedef struct _gmFont cFont;


// 有关屏幕上字体的信息
struct _gmFont
{
	 AEEApplet theApp;
	 int iFontHeight;			//  字体的高度
	 int iFontWidth;		    //  字体的宽度
};



/*********************************************************
//	清除字符串前后的空格
//
// 传入参数
//   rStr_Src  待清除的字符串
//
// 传出参数
//   rStr_Src  清除空格后的字符串
*****************************************************************************/
void ud_Trim(char *rStr_Src);

/*********************************************************
// 字符串中指定字符第一次出现的位置,从0开始计数, -1 没找到 
// 传入参数
//   pszStr  待查找的源字符串
//   cChar   待查找的字符
// 传出参数
//   字符位置 ,从0开始计数, -1 没找到 
*****************************************************************************/
int ud_DistToChar(const char * pszStr, char cChar);

// ==========================================================
// 字符串中指定字符最后出现的位置,位置从0开始计数, -1 没找到 
// 传入参数
//   pszStr  待查找的源字符串
//   cChar   待查找的字符
// 传出参数
//   字符位置 ,从0开始计数, -1 没找到 
// ==========================================================
int ud_rDistToChar(const char * pszStr, char cChar);

//到分割符(或字符串尾)的字符数，
int ud_DistToSplit(const char * pszStr,const char *cSplit);

/* ======================================================================= 
// 从传入的字符串中取出子字符串，并把指针移到分隔符的下一个位置
//  char *sSource : 源字符串
//  int nSrcLen   : 源字符串的长度(不含结束字符0)
//  char *sSub    : 子字符串
//  int nSubLen   : 子字符串的最大长度
//  char cSplit   : 分隔字符
//  返回 : 分隔符的下一个位置
=======================================================================*/
char* uf_GetSubStr(char *sSource,int nSrcLen,char *sSub, int nSubLen,const char *sSplit);

/********************************************************************************************
 在本地记录日志
传入参数
  cgimis * pMe
  IShell *pShell  :  
  cWrtLog         :   1  写入日志， 非1 仅供qxdm调试
  int *pIndex		  :   计数器，表示日志中程序开始执行的位置，由调用的程序保存此值
  const char *szFormat, ...  ： 需记录的字符串
********************************************************************************************/
void ud_RecLog(IShell *pShell,char cWrtLog, uint32 *pIndex, const char *szFormat, ... );
// ================================================================================================
// 在屏幕上显示单字节信息
void gm_Printf( cFont oFont, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, char *szBuf );
// 在屏幕上显示双字节信息
void gm_WPrintf( cFont oFont, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *szText,RGBVAL DataColor);

// 将宽字符串转换为单字节字符串 
void gm_WSTRTOSTR(AECHAR *pIn, char *pszDest, int nSize,IShell *pShell);
// 把字符串转成ASC码的值显示(打印,带,分隔)
void ud_ToAscFmt(const char * psSrc, int nSrcLen, char *psDestr, int nDestLen);
// 把字符串转成16进制ASC码的值
void ud_ToAscFmtWithNone(const char * psSrc, int nSrcLen, char *psDestr, int nDestLen);

// 把无符号字符串转成ASC码的值显示(打印)
void ud_ucharToAscFmt(const uint8 * psSrc, int nSrcLen, char *psDestr, int nDestLen);


//计算2个日期之间的天数
int gm_countDay(JulianType date1,JulianType date2);
// 根据当日午夜开始的毫秒数，推算出当时的日期时间
JulianType gm_GetDateFromTimems(uint32 nCurTm);

#endif /* !__GMTOOLS_H__ */
