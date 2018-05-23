#ifndef GM_SMS_H
#define GM_SMS_H

#include "AEESMS.h"

#define SMS_EVT_START     EVT_USER + 2050		// 启动短信的对象
#define SMS_EVT_STOP	  EVT_USER + 2051		// 注销短信
#define SMS_EVT_DOWNLOAD  EVT_USER + 2052		// 下载短信信息
#define EVT_SAVE_SMS	  EVT_USER + 2053		// 保存短信 

typedef struct _cSms cSms;
typedef struct _newgimis newgimis;

/* ======================================================
解析类似短信信息的数据包结构
======================================================== */ 
#define  ITEM_NAME_LEN			2   // 配置项名称长度,2个字节
#define  CONTENT_BYTE_LEN		2	// 保存内容长度字节数
#define  CALL_NO_LEN			64	// 手机号码
#define  MAX_REC_COUNT			20  // 每次下载的短信最多20条，如果下载的短信数=20条，需再次请求，直到中心端下载的短信条数少于20条，才表示接收结束
// 字段列表
typedef struct  
{
	char			**ppNam;									// 字段的名称数组
	char			**ppVal;									// 字段的内容数组
} IItems;
// 记录列表
typedef struct  
{
	uint16 			nRecordCount;								// 记录的总数
	uint16 			nFieldCount;								// 字段的总数
	IItems			*pItems;									// 记录
} ISmsRecords;

// 短信保存项
typedef struct  
{
	char			sCALLNO[CALL_NO_LEN+1];						// 发布号码
	uint16 			nContentLen;								// 内容字符串字节长度
	AECHAR			*aeContent;									// 内容
} ISMSInfo;
// 短信列表
typedef struct  
{
	uint16		 count;						// 总数 
	ISMSInfo	*pSmsInfo;					// 
	uint8        nCurId;					// 当前等待处理的索引,  nCurId >= count 表示没有要处理的信息
}ISMSList;

struct _cSms
{
//	AEEApplet    a;
	newgimis	*m_pParent;
	uint8		 nState;					// 状态
											// 0  空闲,  1 : 正在接收数据 ， 5 解析收到的数据，存入短信列表中 ,  6 正在保存短信
											// 2014-05-04 增加状态： 7: 正在查询短信邮箱的存储空间 
	IWeb*        pWeb;
	AEECallback  webcb;
	IWebResp *   pWebResp;					// the answer I get from IWeb
	ISource*     pSource;					// the body of the response		用这个也行	IGetLine *   piGetLine;				// the body of the response, if any
	
	char		 sUrl[250];
	int          nBytes;					// count of bytes in response
	int			 nBodyHasRead;
//	char         sBodyBuf[10];				// 只要应答的第0个字符	
//	IPeek        *pipPostData;				// POST stream, 
	char         *pRecData;					//  保存收到的数据

	// 短信的标准接口
	ISMSMsg			  *pISmsMsg;
	ISMSStorage       *pISmsStorage;	
	AEESMSStorageStatus SMSStorageStatus;				// 查询短信的存储状态   2014-05-04			
	uint32            nStorageStatusErr;				// 查询短信的存储状态时用到的状态   2014-05-01
//	unsigned short	  nInd;
	AEECallback		  cbMsg;
    ISMS			  *pISMS;				// 2013-10-10
	uint32			  nMessageSendRet;		// 2013-10-10
	AECHAR			  aeSendContent[71];	// 发送的内容


	// 有关短信自定义接口
	ISMSList		   m_SMSList;

	uint32		 Overtime;		// 记录发送文件后接收应答的时间，单位妙，接收时超过规定时间，关闭链接，并提示失败

};

boolean Sms_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);
static int Sms_Init(cSms* me);
static int SMS_CreateInterface(cSms * me);
static void Sms_DownLoad(cSms* me);


void Sms_MonitorSendFile(cSms *me);
static void Sms_Status(void *p, WebStatus ws, void *pVal);

static void Sms_GotResp(void *p);
static void Sms_ReadBody(cSms *me);
static void Sms_CloseWeb(cSms *me);
static void Sms_ReleaseBuf(cSms *me);
static void Sms_trf(cSms * me);
static ISmsRecords *gm_SMSformat(char *sBuf);
static void Sms_SaveSms(cSms * me);
static void SMS_StoreMsgCb(cSms * me);
static void Sms_Close(cSms *me);
static void Sms_CloseSMS(cSms *me);



#endif /* #ifndef GM_SMS_H */

