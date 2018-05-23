#ifndef GPSONEXTRA_H
#define GPSONEXTRA_H

#include "AEEWeb.h"
#include "AEESource.h"
#include "AEESockPort.h"

#include "AEEIXTRA.h"

#define XTRA_EVT_USER_INIT_DWNLD     EVT_USER + 1021
#define XTRA_EVT_USER_DATA_VALIDITY  EVT_USER + 1023


#define XTRA_USER_SUCCESS        1
#define XTRA_USER_FAIL           2
#define XTRA_USER_TIMEOUT        3
#define XTRA_USER_UNSUPPORTED    4

#define XTRA_DWNLD_MODE_NET 0
#define XTRA_DWNLD_MODE_EFS 1

#define NTP_SERV_PORT 123

#define XTRA_TIMEOUT_VALUE_MSEC    180000    //180000					//  Change by zjie 2013-07-31 15000 改成 180000

#define PSNTPURL "xtra1.gpsonextra.net"
#define SSNTPURL "xtra2.gpsonextra.net"
#define TSNTPURL "xtra3.gpsonextra.net"

#define XTRA_TMP_FILE "fs:/~/xtratmp.bin"
#define MAX_READ_XTRA 10240

typedef struct _XtraDcApp XtraDcApp;
typedef struct _newgimis newgimis;

typedef struct NtpPacket
{
	uint32 dwInt;
	uint32 dwFract;
} NtpPacket;

typedef struct NtpBasicInfo
{
	char   mMode;
	char   mStartum;
	char   mPoll;
	char   mPrecision;
	long   mRootDelay;
	long   mRootDisp;
	char   mRefId[4];
	NtpPacket mRefTimestamp;
	NtpPacket mOrigTimestamp;
	NtpPacket mRecvTimestamp;
	NtpPacket mTxTimestamp;
} NtpBasicInfo;

typedef struct NtpAuth
{
	char buf[20];
} NtpAuth;

typedef struct NtpResp
{
	NtpBasicInfo nBasic;
	NtpAuth nAuth;
	char buf[20];
} NtpResp;

struct _XtraDcApp					
{
	AEEApplet    a;
	IFileMgr*    pFileMgr;
	AEECallback  m_evtTimer;
	boolean      bBkgState;
	int          nCmdReq;
	uint32       dwDwnldMode;
	IXTRA*       pIXtra;
	boolean      bState;				// TRUE 正常, FALSE 异常
	
	// web related.
	ISource*   pSource;
	uint32     tStart;
	uint32     tEnd;
	uint32     nBytes;
	char       szUrl[256]; 
	AEECallback  m_webcb;
	IWeb*        pWeb;
	IWebResp*    pWebResp;
	char         cPart;
	char         cPartTotal;
	
	// time related
	ISockPort*			pSockPort;
	AEECallback			m_readcb;
	AEECallback			m_writecb;
	AEESockAddrStorage	sockAddr;
	NtpBasicInfo		ntpInfo;
	int32				nBytesW;								// zjie 功能未知 - 	 已经发送了几个字节
	int32				nBytesR;								// zjie 功能未知 - 好像是读的字节
	INetMgr*			m_pINetMgr;
	AEECallback			m_cb;
	AEEDNSResult		m_dnsr; 
	char				m_pszHost[30];
	uint32				dwSntpRequestTimeMs;


	// url's for server
	char pUrl[3][128];
	char pXtraUrl[128];
	char sXtraUrl[128];
	char tXtraUrl[128];

	// server parameter
	boolean bAutoDwnld;
	uint32  dwDwnldInterval;
	uint32 dwHttpSesStarted;

	// url's for sntp server
	char pSntpUrl[128];
	char sSntpUrl[128];
	char tSntpUrl[128];
	
	char pSntpUrlFromCfg[128];
	char sSntpUrlFromCfg[128];
	char tSntpUrlFromCfg[128];
	
	// delay threshold
//	uint32 dwDelayThreshold;
	
	// force flag
//	boolean bForceFlag;
	
	// tmp file.
	IFile* pTmpFile;
	uint64 dwTimeToSet;
	boolean bGetStatus;					// 获取星历状态(有效的时间)
	AEEXTRAStatusInfo sTimeStatus;
	int32 iSocketCount;				 // 用于注入时间	
	uint32 dwSntpSesStarted;
								//	uint8 iDebugLogLevel;
	uint8 iLifeOfSocket;
	boolean bDnsErrorFlag;			// dns是否可用标志，用于注入时间
	AEEXTRADataInfo dataInfo;
	IFile* pIXTRAFile;
	uint8 nSetDataPartNum;
	char *pSetDataBuf;
	int nSetDataTotalParts;

	char      cDwnState;				// 1 星历下载成功, 2 用local模式做了一定位以此代替星历下载,0 星历不成功, // 2013-04-12
	char      cTimeOut;					// 超时标志     1 : 超时  2013-07-31
	newgimis	  *m_pParent;

};

boolean XtraDc_HandleEvent(newgimis *pMe, AEEEvent eCode,uint16 wParam, uint32 dwParam);


#endif /* #ifndef GPSONEXTRA_H */
