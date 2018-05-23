/*======================================================
FILE:  gm_Qchat.h

修改日志：
2013-01-01  zjie
            有关QCHAT
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

#include "./PKCommon/inc/qchatui.bid"				// Qchat 图形界面的CLASSID

#define QDK_CALL_NAME_LEN		IQDK_MAX_ADDRNAME_SIZE	// Qchat呼叫名称（预定义组，手机号等)长度, 定义同海信提供的头文件
#define QDK_GROUP_PRIORITY		2						// 优先级
#define DOMAIN_LEN				IQDK_MAX_DOMAIN_SIZE	// domain的长度, 定义同海信提供的头文件

#define EVT_STARTCALL_EVENT		EVT_USER+2000			// 发起呼叫
#define EVT_ENDCALL_EVENT		EVT_USER+2001			// 结束呼叫
#define EVT_RELEASEFLOOR_EVENT	EVT_USER+2002			// 释放话语权
#define EVT_REQUESTFLOOR_EVENT	EVT_USER+2003			// 申请话语权
#define EVT_CALLAGAIN_EVENT		EVT_USER+2004			// (组呼)同时发起呼叫时，冲突，再发起一遍, 发起的次数由nMyCallCnt计数
#define EVT_SETPTTCALL_EVENT	EVT_USER+2005			// 设置一键呼叫信息
#define EVT_PTTCALL_EVENT		EVT_USER+2006			// 发起一键呼叫
#define EVT_UPDREST_EVENT		EVT_USER+2007			// 更新呼叫限制（黑白名单）

#define QCHAT_CONFIG_FILE          "qdkconfig.txt"
#define CONFIG_GROUP_DOMAIN		   "GROUP_DOMAIN = "
#define CONFIG_DIRECTCALL_DOMAIN   "DIRECTCALL_DOMAIN = "

#define RICH_PTT_DOMAIN				"ptt.189.cn"
#define RICH_GROUP_DOMAIN			"group.ptt.189.cn"
#define RICH_CALL_INTERVAL			4000						// 两个呼叫之间的最小时间间隔, 单位1微妙

typedef struct _newgimis newgimis;

// Qchat状态信息掩码
#define QCHAT_NMASK_CALL			 0x8				// 2^3:   1 ： Qchat 正在对讲
#define QCHAT_NMASK_CONNECT			 0x10				// 2^4:   1 ： Qchat 连接成功
							// #define QCHAT_NMASK_PTT				 0x32				// 2^5:   1 ： 正在一键呼叫
// #define QCHAT_NMASK_ASKEND			 0x64				// 2^6:   1 ： 收到高优先级后 ???

#define richptt_HasStatus(me,nMask)     (me->cStatus & nMask)    	// 根据掩码取出某一位置的值
// 根据掩码设置某一位置的值为1
#define richptt_StatusOn(me, nMask)  { me->cStatus = (nMask<0x8) ? (me->cStatus & 0xF8) | nMask : me->cStatus | nMask;	}
// 根据掩码设置某一位置的值为0
#define richptt_StatusOff(me, nMask)  { me->cStatus = (nMask<0x8) ? (me->cStatus & 0xF8) | nMask : me->cStatus & (~nMask);	}


// 自定义的预定义组信息，计划从中心端下载得到
typedef struct
{
	char			sName[QDK_CALL_NAME_LEN+1];			// 预定义组的组名
	IQDKConfIdType  ConfId;								// 预定义组对应的confId
	char	   originator[QDK_CALL_NAME_LEN+1];			// 组的原始发起者姓名
    char			cStatus;							// 预定义组的呼叫状态,     // 0 空闲，1 连接成功, 2 连接失败
	unsigned char	npriority;							// 优先级，数字大的优先级高  
} IUsrPredGroup;

// 自定义的预定义组列表信息
typedef struct
{
	uint16			count;				// 自定义的预定义组信息总数
	// Add by zjie 2013-03-20
	//  1 作为主叫方第1次发起呼叫失败，2 主叫方第2次发起，3 主叫方呼叫成功 
	char		    cGrpCallSts;		// 状态信息  0 未定义， 1 作为主叫方呼叫失败后发起呼叫，2 开始计算通话时间
	uint32          iGrpCallTime;		// 记录作为主叫方呼叫失败后再次发起呼叫且成功后的时间 如果都没人讲话，连接成功后至少3才能退出算成功 
	IUsrPredGroup	*GroupInfo;			// 自定义的预定义组信息
} IUsrPredGroupList;

// 一键呼叫的手机号信息
typedef struct  
{
	char			CallNo[QDK_CALL_NAME_LEN+1];			// 手机号码
	char			cCallSts;								// 呼叫的状态，0:空闲, 1:正在呼叫, 3: 呼叫失败
} ICallNoGroup;
// 一键呼叫的列表信息
typedef struct  
{
	char		 cPttCallSts;				//  状态信息  0 没有按键， 1 用户按键, 2 用户按键达到规定时间
											//   3 正在找空闲的手机号码,  4 已没有空闲的手机号供呼叫   5 正在呼叫, 6 呼叫成功
											//   7 至少获得过一次话语权, 8 未获得话语权就挂断
	uint32       iLastCallTime;				//  记录上次呼叫的时间, 两次呼叫之间应个一定时间间隔
    uint32		 iGetkey;					// 记录按键的时间长度
	uint8		 count;						// 手机号码总数
	uint8		 nCallId;					// 当前正在呼叫的索引,nCallId == count代表没有呼叫
	ICallNoGroup *PttCallNoGroup;			// 一键呼叫的手机号信息
}IcallGroupList;
// Add by zjie 2013-03-15
// 黑白名单结构
typedef struct  
{
	char			sName[QDK_CALL_NAME_LEN+1];			// 限制呼叫的ID
    IQDK_ServicesType                service;			// 呼叫类型(2：单呼，4：组呼)
	IQDK_CallRestrictionDirection    direction;			// 限制方向(0：呼入，1：呼出，2呼入呼出)
	IQDK_CallRestrictionType         type;				// 限制类型	= IQDK_ALWAYS_ALLOW,IQDK_ALLOW,IQDK_DENY   
} ICallRestriction;

// 黑白名单列表信息
typedef struct
{
	uint16			  count;				// 自定义的预定义组信息总数
	ICallRestriction  *CallRestInfo;		// 自定义的预定义组信息
} ICallRestrictionList;
// Add End


typedef struct 
{
	newgimis		*m_pParent;

	AEECallback cbGen;												// 回调函数
	char        cStatus;											// Qchat 状态 1 ~ 7 计数(用于获取手机号码)
	char		MyMobileNo[QDK_CALL_NAME_LEN+1];					// 本机的电话号码,如果不是qchat手机，存"0"

	AECHAR sGrpDomain[DOMAIN_LEN+1];								// 预定义组的Domain
	AECHAR sPttDomain[DOMAIN_LEN+1];								// Direct call 的Domain
	

	IQDK30Provision		*pIQDKProvision;	// 可获得的本机号码
	IQDK30Membership	*pIQDK30Membership;	// 希望通过此接口获得confId对应的GroupName 	
	// QDK interfaces
	IQDK30Manager		*pIQDKManager;
	IQDK30Call			*pIQDKCall;
	
	// 有关呼叫限制
	IQDK30CallRestriction  *pIQDK30CallRestriction;

	// user's address
	IQDKAddressListType *addrList;
	IQDKAddressData *addrData;
	
	// Call Preferences
	IQDK_CommonCallPrefs commonPrefs;
	IQDK_DirectCallPrefs directPref; 
	
	IQDKConfIdType  ConnConfID;					// 正在请求连接的confID	

	IUsrPredGroupList *pGroupList;			 // 自定义的预定义组列表
    // 当前请求的情况
	char     nMyCallCnt;					 // 本机作为发起者，呼叫失败后重复呼叫的次数
	int      idxCalling;					 //  >=0  正在对讲的组 , -1 没有对讲的组
	int      idxCallWait;					 //  已经发起呼叫，等待呼叫结果预定义组索引
	int		 idxCallReq;				     //  请求呼叫的自定义的预定义组索引, -1 表示没有
	uint8	 nCallCnt;				         //  等待呼叫的预定义组发起呼叫的次数
											 //  暂定如失败，隔5秒发起一次，如果还是失败，则放弃。 即允许3次

	char	originatorName[QDK_CALL_NAME_LEN+1];	// 预定义组触发连接的发起者姓名

	// 有关一键呼叫的情况
	IcallGroupList   *pPttCallGroup;		//  一键呼叫数据  
	ICallRestrictionList *pCallRestList;		//  黑白名单列表

} crichptt;

/*-------------------------------------------------------------------
function prototypes
-------------------------------------------------------------------*/
boolean richptt_HandleEvent(newgimis *pMe,  AEEEvent eCode, uint16 wParam, uint32 dwParam);
// 设置预定义组优先级
void richptt_SetUsrGroup(crichptt *me, char *sGroupInfo);
// 设置域名
void richptt_SetDomain(crichptt *me, char *sGroupInfo);
// 设置限制呼叫		2013-03-15
void richPtt_SetCallRestInfo(crichptt *me,char *CallRestInfo);	


#endif /* #ifndef GM_QCHAT_H */
