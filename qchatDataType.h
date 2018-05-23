#ifndef QCHATDATATYPES_H_
#define QCHATDATATYPES_H_

// 本头文件是根据海信提供的资料整理
#define QCHAT_FLDLEN_PTT_LEN  (26)

/**********************************************************************
       以下与利用海信的接口发起呼叫有关
***********************************************************************/
#define FLDLEN_NUMBER 32
typedef enum _SpeedDialType
{
    SPEEDDIAL_NOTYPE= 0,
	SPEEDDIAL_VOICE_CALL ,
	SPEEDDIAL_QCHAT_DIRECTCALL,
	SPEEDDIAL_QCHAT_PREDEFINEDCALL,
	SPEEDDIAL_QCHAT_CHATCALL,
	SPEEDDIAL_QCHAT_ADHOCCALL,
	SPEEDDIAL_TYPEMAX
} SpeedDialType;

/* User defined events for internal use. */
typedef enum _QCHATUI_EVENT
{
    EVT_QCHAT_LAUNCH = EVT_USER+0x500+0xF0,		//	EVT_USER+1520
	EVT_QCHAT_CALL,
	EVT_QCHAT_ALERT,
	EVT_QCHAT_PRESENCEOD,
	EVT_QCHAT_AVAILABILITY,
	EVT_QCHAT_COMMON
}QCHATUI_EVENT;

typedef struct SpeedDialNum
{
    AECHAR pSpeedNum[FLDLEN_NUMBER + 1];
    SpeedDialType speeddial_type;
    uint16 AdHoc_id;
}SpeedDialNum;


/**********************************************************************
                                         QChat Call DataTypes
***********************************************************************/
typedef enum
{
    QCHATCALL_EVENT_DRAWCALLNOTIFYUI,               /*绘画正在呼叫界面*/
    QCHATCALL_EVENT_DRAWCONVERSATIONUI,         /*绘画通话中界面*/
    QCHATCALL_EVENT_DRAWCALLENDUI,                     /*绘画通话结束界面*/
    QCHATCALL_EVENT_DRAWINVITERECMSG,              /*绘画邀请到来消息*/
    QCHATCALL_EVENT_DRAWMEMBERLISTUI,              /*绘画组成员列表界面*/
    QCHATCALL_EVENT_TIMER_UPDATE,                        /*更新通话中时间显示*/
    QCHATCALL_EVENT_FLOORTIME_UPDATE,                        /*更新通话中话权时间显示*/
    QCHATCALL_EVENT_CONNECTED_UPDATE,              /*更新通话接通界面显示*/
    QCHATCALL_EVENT_STATUS_UPDATE,                    /*更新话语权状态显示*/
    QCHATCALL_EVENT_ENDCALL_UPDATE,                  /*更新通话结束界面显示*/
    QCHATCALL_EVENT_CALLTYPE_UPDATE,                  /*更新呼叫类型显示*/
    QCHATCALL_EVENT_CALLNAME_UPDATE,                  /*更新呼叫姓名显示*/
    QCHATCALL_EVENT_MISSCALL,                                  /*未接来电*/
    QCHATCALL_EVENT_DRAWCALLLOGUI,                      /*绘画通话记录界面*/
    QCHATCALL_EVENT_CALLENDUI_CLOSE,                    /*通话结束界面关闭*/
    QCHATCALL_EVENT_CLEAR_PENDING_TARGETS,        /*  清除优化方式的单呼*/
    QCHATCALL_EVENT_START_DIRECT_CALL_BY_ADDRESS,  /* 拨号盘按PTT 键直接呼叫*/
    QCHATCALL_EVENT_START_ADHOC_BY_ADDRESS_NORMAL,   /* 开始普通型临时组呼*/
    QCHATCALL_EVENT_START_ADHOC_BY_ADDRESS_INVITE,   /* 开始邀请型临时组呼*/
    QCHATCALL_EVENT_START_ADHOC_BY_CONFERENCEID,   /* 开始临时组呼*/
    QCHATCALL_EVENT_START_GROUP_CALL_BY_CONFERENCEID, /* 非优化方式的预定义组呼*/
    QCHATCALL_EVENT_START_CHATROOM_CALL_BY_CONFERENCEID,  /* 非优化方式的预定义聊天组呼*/
    QCHATCALL_EVENT_START_OPT_DIRECT_CALL_BY_ADDRESS, /* 优化方式的单呼*/
    QCHATCALL_EVENT_START_OPT_ADHOC_BY_CONFERENCEID,   /* 优化方式开始临时组呼*/
    QCHATCALL_EVENT_START_OPT_GROUP_CALL_BY_CONFERENCEID,  /* 优化方式的预定义组呼*/
    QCHATCALL_EVENT_START_OPT_CHATROOM_CALL_BY_CONFERENCEID, /* 优化方式的预定义聊天组呼*/
    QCHATCALL_EVENT_START_SHORTCUT_CALL,              /* 快捷呼叫*/
    QCHATCALL_EVENT_DRAWNOSERVICEMSG,                        /* 绘画无网络服务消息框*/
    QCHATCALL_EVENT_DRAWNEEDONLINEMSG,                 /* 绘画是否注册消息框*/
    QCHATCALL_EVENT_DRAWUNABLDEMAKECALLMSG,                  /* 绘画非法卡无法发起呼叫消息框*/
    QCHATCALL_EVENT_DRAWSHORTCUTOFFMSG,     /* 绘画未开启快捷号码绑定功能消息框*/
    QCHATCALL_EVENT_DRAWNOSHORTCUTNUMBERMSG,     /* 绘画无快捷号码消息框*/
    QCHATCALL_EVENT_START_DIRECT_CALL_BY_NAME, /*仅有name 发起 的单呼*/
    QCHATCALL_EVENT_START_GROUP_CALL_BY_NAME, /*仅有name 发起的 的预定义组呼*/
    QCHATCALL_EVENT_START_CHATROOM_CALL_BY_NAME, /* 仅有name 发起的聊天组呼*/
    QCHATCALL_EVENT_START_GROUPCALL_RESTRICT,
    QCHATCALL_EVENT_START_OPTDIRECT_CALL_BY_ADDRESS,
    QCHATCALL_EVENT_START_CALL_USE_PCDATA	//利用PC发过来解析后的数据发起呼叫
}EQchatCallEvt;



#endif /* GWQCHATDATATYPES_H_ */

