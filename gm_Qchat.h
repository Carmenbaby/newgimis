/*======================================================
FILE:  gm_Qchat.h

�޸���־��
2013-01-01  zjie
            �й�QCHAT
=====================================================*/
#ifndef GM_QCHAT_H
#define GM_QCHAT_H

#include "./PKCommon/inc/aeeiqdk30provision.h" 
#include "./PKCommon/inc/aeeiqdk30notifier.h"	
#include "./PKCommon/inc/aeeiqdknotifier.bid"	
#include "./PKCommon/inc/aeeqdkdatatypes.h"
#include "./PKCommon/inc/AEEIQDK30Manager.h"       // IQDKManager Interface definitions
#include "./PKCommon/inc/AEEIQDK30Call.h"     // IQDKPresence Interface definitions
#include "./PKCommon/inc/AEEQDKAddress.h"
#include "./PKCommon/inc/aeeiqdk30membership.h"
#include "./PKCommon/inc/aeeiqdkmembership.h"
#include "./PKCommon/inc/aeeiqdk30callrestriction.h" 

#include "./PKCommon/inc/qchatui.bid"				// Qchat ͼ�ν����CLASSID

#define QDK_CALL_NAME_LEN		IQDK_MAX_ADDRNAME_SIZE	// Qchat�������ƣ�Ԥ�����飬�ֻ��ŵ�)����, ����ͬ�����ṩ��ͷ�ļ�
#define QDK_GROUP_PRIORITY		2						// ���ȼ�
#define DOMAIN_LEN				IQDK_MAX_DOMAIN_SIZE	// domain�ĳ���, ����ͬ�����ṩ��ͷ�ļ�

#define EVT_STARTCALL_EVENT		EVT_USER+2000			// �������
#define EVT_ENDCALL_EVENT		EVT_USER+2001			// ��������
#define EVT_RELEASEFLOOR_EVENT	EVT_USER+2002			// �ͷŻ���Ȩ
#define EVT_REQUESTFLOOR_EVENT	EVT_USER+2003			// ���뻰��Ȩ
#define EVT_CALLAGAIN_EVENT		EVT_USER+2004			// (���)ͬʱ�������ʱ����ͻ���ٷ���һ��, ����Ĵ�����nMyCallCnt����
#define EVT_SETPTTCALL_EVENT	EVT_USER+2005			// ����һ��������Ϣ
#define EVT_PTTCALL_EVENT		EVT_USER+2006			// ����һ������
#define EVT_UPDREST_EVENT		EVT_USER+2007			// ���º������ƣ��ڰ�������

#define QCHAT_CONFIG_FILE          "qdkconfig.txt"
#define CONFIG_GROUP_DOMAIN		   "GROUP_DOMAIN = "
#define CONFIG_DIRECTCALL_DOMAIN   "DIRECTCALL_DOMAIN = "

#define RICH_PTT_DOMAIN				"ptt.189.cn"
#define RICH_GROUP_DOMAIN			"group.ptt.189.cn"
#define RICH_CALL_INTERVAL			4000						// ��������֮�����Сʱ����, ��λ1΢��

typedef struct _newgimis newgimis;

// Qchat״̬��Ϣ����
#define QCHAT_NMASK_CALL			 0x8				// 2^3:   1 �� Qchat ���ڶԽ�
#define QCHAT_NMASK_CONNECT			 0x10				// 2^4:   1 �� Qchat ���ӳɹ�
							// #define QCHAT_NMASK_PTT				 0x32				// 2^5:   1 �� ����һ������
// #define QCHAT_NMASK_ASKEND			 0x64				// 2^6:   1 �� �յ������ȼ��� ???

#define richptt_HasStatus(me,nMask)     (me->cStatus & nMask)    	// ��������ȡ��ĳһλ�õ�ֵ
// ������������ĳһλ�õ�ֵΪ1
#define richptt_StatusOn(me, nMask)  { me->cStatus = (nMask<0x8) ? (me->cStatus & 0xF8) | nMask : me->cStatus | nMask;	}
// ������������ĳһλ�õ�ֵΪ0
#define richptt_StatusOff(me, nMask)  { me->cStatus = (nMask<0x8) ? (me->cStatus & 0xF8) | nMask : me->cStatus & (~nMask);	}


// �Զ����Ԥ��������Ϣ���ƻ������Ķ����صõ�
typedef struct
{
	char			sName[QDK_CALL_NAME_LEN+1];			// Ԥ�����������
	IQDKConfIdType  ConfId;								// Ԥ�������Ӧ��confId
	char	   originator[QDK_CALL_NAME_LEN+1];			// ���ԭʼ����������
    char			cStatus;							// Ԥ������ĺ���״̬,     // 0 ���У�1 ���ӳɹ�, 2 ����ʧ��
	unsigned char	npriority;							// ���ȼ������ִ�����ȼ���  
} IUsrPredGroup;

// �Զ����Ԥ�������б���Ϣ
typedef struct
{
	uint16			count;				// �Զ����Ԥ��������Ϣ����
	// Add by zjie 2013-03-20
	//  1 ��Ϊ���з���1�η������ʧ�ܣ�2 ���з���2�η���3 ���з����гɹ� 
	char		    cGrpCallSts;		// ״̬��Ϣ  0 δ���壬 1 ��Ϊ���з�����ʧ�ܺ�����У�2 ��ʼ����ͨ��ʱ��
	uint32          iGrpCallTime;		// ��¼��Ϊ���з�����ʧ�ܺ��ٴη�������ҳɹ����ʱ�� �����û�˽��������ӳɹ�������3�����˳���ɹ� 
	IUsrPredGroup	*GroupInfo;			// �Զ����Ԥ��������Ϣ
} IUsrPredGroupList;

// һ�����е��ֻ�����Ϣ
typedef struct  
{
	char			CallNo[QDK_CALL_NAME_LEN+1];			// �ֻ�����
	char			cCallSts;								// ���е�״̬��0:����, 1:���ں���, 3: ����ʧ��
} ICallNoGroup;
// һ�����е��б���Ϣ
typedef struct  
{
	char		 cPttCallSts;				//  ״̬��Ϣ  0 û�а����� 1 �û�����, 2 �û������ﵽ�涨ʱ��
											//   3 �����ҿ��е��ֻ�����,  4 ��û�п��е��ֻ��Ź�����   5 ���ں���, 6 ���гɹ�
											//   7 ���ٻ�ù�һ�λ���Ȩ, 8 δ��û���Ȩ�͹Ҷ�
	uint32       iLastCallTime;				//  ��¼�ϴκ��е�ʱ��, ���κ���֮��Ӧ��һ��ʱ����
    uint32		 iGetkey;					// ��¼������ʱ�䳤��
	uint8		 count;						// �ֻ���������
	uint8		 nCallId;					// ��ǰ���ں��е�����,nCallId == count����û�к���
	ICallNoGroup *PttCallNoGroup;			// һ�����е��ֻ�����Ϣ
}IcallGroupList;
// Add by zjie 2013-03-15
// �ڰ������ṹ
typedef struct  
{
	char			sName[QDK_CALL_NAME_LEN+1];			// ���ƺ��е�ID
    IQDK_ServicesType                service;			// ��������(2��������4�����)
	IQDK_CallRestrictionDirection    direction;			// ���Ʒ���(0�����룬1��������2�������)
	IQDK_CallRestrictionType         type;				// ��������	= IQDK_ALWAYS_ALLOW,IQDK_ALLOW,IQDK_DENY   
} ICallRestriction;

// �ڰ������б���Ϣ
typedef struct
{
	uint16			  count;				// �Զ����Ԥ��������Ϣ����
	ICallRestriction  *CallRestInfo;		// �Զ����Ԥ��������Ϣ
} ICallRestrictionList;
// Add End


typedef struct 
{
	newgimis		*m_pParent;

	AEECallback cbGen;												// �ص�����
	char        cStatus;											// Qchat ״̬ 1 ~ 7 ����(���ڻ�ȡ�ֻ�����)
	char		MyMobileNo[QDK_CALL_NAME_LEN+1];					// �����ĵ绰����,�������qchat�ֻ�����"0"

	AECHAR sGrpDomain[DOMAIN_LEN+1];								// Ԥ�������Domain
	AECHAR sPttDomain[DOMAIN_LEN+1];								// Direct call ��Domain
	

	IQDK30Provision		*pIQDKProvision;	// �ɻ�õı�������
	IQDK30Membership	*pIQDK30Membership;	// ϣ��ͨ���˽ӿڻ��confId��Ӧ��GroupName 	
	// QDK interfaces
	IQDK30Manager		*pIQDKManager;
	IQDK30Call			*pIQDKCall;
	
	// �йغ�������
	IQDK30CallRestriction  *pIQDK30CallRestriction;

	// user's address
	IQDKAddressListType *addrList;
	IQDKAddressData *addrData;
	
	// Call Preferences
	IQDK_CommonCallPrefs commonPrefs;
	IQDK_DirectCallPrefs directPref; 
	
	IQDKConfIdType  ConnConfID;					// �����������ӵ�confID	

	IUsrPredGroupList *pGroupList;			 // �Զ����Ԥ�������б�
    // ��ǰ��������
	char     nMyCallCnt;					 // ������Ϊ�����ߣ�����ʧ�ܺ��ظ����еĴ���
	int      idxCalling;					 //  >=0  ���ڶԽ����� , -1 û�жԽ�����
	int      idxCallWait;					 //  �Ѿ�������У��ȴ����н��Ԥ����������
	int		 idxCallReq;				     //  ������е��Զ����Ԥ����������, -1 ��ʾû��
	uint8	 nCallCnt;				         //  �ȴ����е�Ԥ�����鷢����еĴ���
											 //  �ݶ���ʧ�ܣ���5�뷢��һ�Σ��������ʧ�ܣ�������� ������3��

	char	originatorName[QDK_CALL_NAME_LEN+1];	// Ԥ�����鴥�����ӵķ���������

	// �й�һ�����е����
	IcallGroupList   *pPttCallGroup;		//  һ����������  
	ICallRestrictionList *pCallRestList;		//  �ڰ������б�

} crichptt;

/*-------------------------------------------------------------------
function prototypes
-------------------------------------------------------------------*/
boolean richptt_HandleEvent(newgimis *pMe,  AEEEvent eCode, uint16 wParam, uint32 dwParam);
// ����Ԥ���������ȼ�
void richptt_SetUsrGroup(crichptt *me, char *sGroupInfo);
// ��������
void richptt_SetDomain(crichptt *me, char *sGroupInfo);
// �������ƺ���		2013-03-15
void richPtt_SetCallRestInfo(crichptt *me,char *CallRestInfo);	


#endif /* #ifndef GM_QCHAT_H */
