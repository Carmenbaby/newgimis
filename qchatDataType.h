#ifndef QCHATDATATYPES_H_
#define QCHATDATATYPES_H_

// ��ͷ�ļ��Ǹ��ݺ����ṩ����������
#define QCHAT_FLDLEN_PTT_LEN  (26)

/**********************************************************************
       ���������ú��ŵĽӿڷ�������й�
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
    QCHATCALL_EVENT_DRAWCALLNOTIFYUI,               /*�滭���ں��н���*/
    QCHATCALL_EVENT_DRAWCONVERSATIONUI,         /*�滭ͨ���н���*/
    QCHATCALL_EVENT_DRAWCALLENDUI,                     /*�滭ͨ����������*/
    QCHATCALL_EVENT_DRAWINVITERECMSG,              /*�滭���뵽����Ϣ*/
    QCHATCALL_EVENT_DRAWMEMBERLISTUI,              /*�滭���Ա�б����*/
    QCHATCALL_EVENT_TIMER_UPDATE,                        /*����ͨ����ʱ����ʾ*/
    QCHATCALL_EVENT_FLOORTIME_UPDATE,                        /*����ͨ���л�Ȩʱ����ʾ*/
    QCHATCALL_EVENT_CONNECTED_UPDATE,              /*����ͨ����ͨ������ʾ*/
    QCHATCALL_EVENT_STATUS_UPDATE,                    /*���»���Ȩ״̬��ʾ*/
    QCHATCALL_EVENT_ENDCALL_UPDATE,                  /*����ͨ������������ʾ*/
    QCHATCALL_EVENT_CALLTYPE_UPDATE,                  /*���º���������ʾ*/
    QCHATCALL_EVENT_CALLNAME_UPDATE,                  /*���º���������ʾ*/
    QCHATCALL_EVENT_MISSCALL,                                  /*δ������*/
    QCHATCALL_EVENT_DRAWCALLLOGUI,                      /*�滭ͨ����¼����*/
    QCHATCALL_EVENT_CALLENDUI_CLOSE,                    /*ͨ����������ر�*/
    QCHATCALL_EVENT_CLEAR_PENDING_TARGETS,        /*  ����Ż���ʽ�ĵ���*/
    QCHATCALL_EVENT_START_DIRECT_CALL_BY_ADDRESS,  /* �����̰�PTT ��ֱ�Ӻ���*/
    QCHATCALL_EVENT_START_ADHOC_BY_ADDRESS_NORMAL,   /* ��ʼ��ͨ����ʱ���*/
    QCHATCALL_EVENT_START_ADHOC_BY_ADDRESS_INVITE,   /* ��ʼ��������ʱ���*/
    QCHATCALL_EVENT_START_ADHOC_BY_CONFERENCEID,   /* ��ʼ��ʱ���*/
    QCHATCALL_EVENT_START_GROUP_CALL_BY_CONFERENCEID, /* ���Ż���ʽ��Ԥ�������*/
    QCHATCALL_EVENT_START_CHATROOM_CALL_BY_CONFERENCEID,  /* ���Ż���ʽ��Ԥ�����������*/
    QCHATCALL_EVENT_START_OPT_DIRECT_CALL_BY_ADDRESS, /* �Ż���ʽ�ĵ���*/
    QCHATCALL_EVENT_START_OPT_ADHOC_BY_CONFERENCEID,   /* �Ż���ʽ��ʼ��ʱ���*/
    QCHATCALL_EVENT_START_OPT_GROUP_CALL_BY_CONFERENCEID,  /* �Ż���ʽ��Ԥ�������*/
    QCHATCALL_EVENT_START_OPT_CHATROOM_CALL_BY_CONFERENCEID, /* �Ż���ʽ��Ԥ�����������*/
    QCHATCALL_EVENT_START_SHORTCUT_CALL,              /* ��ݺ���*/
    QCHATCALL_EVENT_DRAWNOSERVICEMSG,                        /* �滭�����������Ϣ��*/
    QCHATCALL_EVENT_DRAWNEEDONLINEMSG,                 /* �滭�Ƿ�ע����Ϣ��*/
    QCHATCALL_EVENT_DRAWUNABLDEMAKECALLMSG,                  /* �滭�Ƿ����޷����������Ϣ��*/
    QCHATCALL_EVENT_DRAWSHORTCUTOFFMSG,     /* �滭δ������ݺ���󶨹�����Ϣ��*/
    QCHATCALL_EVENT_DRAWNOSHORTCUTNUMBERMSG,     /* �滭�޿�ݺ�����Ϣ��*/
    QCHATCALL_EVENT_START_DIRECT_CALL_BY_NAME, /*����name ���� �ĵ���*/
    QCHATCALL_EVENT_START_GROUP_CALL_BY_NAME, /*����name ����� ��Ԥ�������*/
    QCHATCALL_EVENT_START_CHATROOM_CALL_BY_NAME, /* ����name ������������*/
    QCHATCALL_EVENT_START_GROUPCALL_RESTRICT,
    QCHATCALL_EVENT_START_OPTDIRECT_CALL_BY_ADDRESS,
    QCHATCALL_EVENT_START_CALL_USE_PCDATA	//����PC����������������ݷ������
}EQchatCallEvt;



#endif /* GWQCHATDATATYPES_H_ */

