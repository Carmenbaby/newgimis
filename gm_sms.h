#ifndef GM_SMS_H
#define GM_SMS_H

#include "AEESMS.h"

#define SMS_EVT_START     EVT_USER + 2050		// �������ŵĶ���
#define SMS_EVT_STOP	  EVT_USER + 2051		// ע������
#define SMS_EVT_DOWNLOAD  EVT_USER + 2052		// ���ض�����Ϣ
#define EVT_SAVE_SMS	  EVT_USER + 2053		// ������� 

typedef struct _cSms cSms;
typedef struct _newgimis newgimis;

/* ======================================================
�������ƶ�����Ϣ�����ݰ��ṹ
======================================================== */ 
#define  ITEM_NAME_LEN			2   // ���������Ƴ���,2���ֽ�
#define  CONTENT_BYTE_LEN		2	// �������ݳ����ֽ���
#define  CALL_NO_LEN			64	// �ֻ�����
#define  MAX_REC_COUNT			20  // ÿ�����صĶ������20����������صĶ�����=20�������ٴ�����ֱ�����Ķ����صĶ�����������20�����ű�ʾ���ս���
// �ֶ��б�
typedef struct  
{
	char			**ppNam;									// �ֶε���������
	char			**ppVal;									// �ֶε���������
} IItems;
// ��¼�б�
typedef struct  
{
	uint16 			nRecordCount;								// ��¼������
	uint16 			nFieldCount;								// �ֶε�����
	IItems			*pItems;									// ��¼
} ISmsRecords;

// ���ű�����
typedef struct  
{
	char			sCALLNO[CALL_NO_LEN+1];						// ��������
	uint16 			nContentLen;								// �����ַ����ֽڳ���
	AECHAR			*aeContent;									// ����
} ISMSInfo;
// �����б�
typedef struct  
{
	uint16		 count;						// ���� 
	ISMSInfo	*pSmsInfo;					// 
	uint8        nCurId;					// ��ǰ�ȴ����������,  nCurId >= count ��ʾû��Ҫ�������Ϣ
}ISMSList;

struct _cSms
{
//	AEEApplet    a;
	newgimis	*m_pParent;
	uint8		 nState;					// ״̬
											// 0  ����,  1 : ���ڽ������� �� 5 �����յ������ݣ���������б��� ,  6 ���ڱ������
											// 2014-05-04 ����״̬�� 7: ���ڲ�ѯ��������Ĵ洢�ռ� 
	IWeb*        pWeb;
	AEECallback  webcb;
	IWebResp *   pWebResp;					// the answer I get from IWeb
	ISource*     pSource;					// the body of the response		�����Ҳ��	IGetLine *   piGetLine;				// the body of the response, if any
	
	char		 sUrl[250];
	int          nBytes;					// count of bytes in response
	int			 nBodyHasRead;
//	char         sBodyBuf[10];				// ֻҪӦ��ĵ�0���ַ�	
//	IPeek        *pipPostData;				// POST stream, 
	char         *pRecData;					//  �����յ�������

	// ���ŵı�׼�ӿ�
	ISMSMsg			  *pISmsMsg;
	ISMSStorage       *pISmsStorage;	
	AEESMSStorageStatus SMSStorageStatus;				// ��ѯ���ŵĴ洢״̬   2014-05-04			
	uint32            nStorageStatusErr;				// ��ѯ���ŵĴ洢״̬ʱ�õ���״̬   2014-05-01
//	unsigned short	  nInd;
	AEECallback		  cbMsg;
    ISMS			  *pISMS;				// 2013-10-10
	uint32			  nMessageSendRet;		// 2013-10-10
	AECHAR			  aeSendContent[71];	// ���͵�����


	// �йض����Զ���ӿ�
	ISMSList		   m_SMSList;

	uint32		 Overtime;		// ��¼�����ļ������Ӧ���ʱ�䣬��λ�����ʱ�����涨ʱ�䣬�ر����ӣ�����ʾʧ��

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

