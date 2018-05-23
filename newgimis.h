#ifndef _NEWGIMIS_H_
#define _NEWGIMIS_H_

/*===============================================================================
INCLUDES AND VARIABLE DEFINITIONS
=============================================================================== */

#include "AEE.h"
#include "AEEPosDet.h"          // Position Determination interface definitions.
#include "AEEStdLib.h"
#include "AEEModGen.h"          // Module interface definitions
#include "AEEAppGen.h"          // Applet interface definitions
#include "AEEShell.h"           // Shell interface definitions
#include "AEEFile.h"
#include "AEENet.h"				// Socket interface definitions
#include "AEETapi.h"			// TAPI Interface definitions

#include "AEEText.h"
#include "AEEMenu.h"

#include "AEENet.h"				// Socket interface definitions
#include "AEEHtmlViewer.h"

#include "AEEWeb.h"
#include "AEEBattery.h"

#include "newgimis.bid"
#include "newgimis.brh"

#include "AEETelephone.h"

#include "AEESoundPlayer.h"
#include "AEESound.h"			// Sound Interface definitions

#include "gm_task.h"

#include "OEMTTSExt_Gps.h"
#include "OEMTTSExt_Gps.bid"

#include "OEMTTSExt.h"
#include "OEMTTSEXT.bid"

#include "AEESMS.h"

#include "socket.h"



typedef enum {
   GETGPSINFO_PAINT_ALL = 0,
   GETGPSINFO_PAINT_FIXCOUNT,
   GETGPSINFO_PAINT_FIXDATA,
   GETGPSINFO_PAINT_FIXANIM,
   GETGPSINFO_PAINT_ERROR
} GetGPSInfo_PaintRegions;



#define DEFAULT_VERSION				    "V3.1.2PAYY"			// �汾��   
#define D_VERSION				    "3.1.2PAYY"			// �汾�� 
#define SPD_MOBILE_NO			"MOBILE_NO = "		// �ֻ�����	
#define SPD_MOBILE_IMEI         "MOBILE_IMEI = "		// �ֻ�imei
#define DEFAULT_FIELD_SPLIT			";"					// �������ݵĲ�ͬ�ֶ��ַ���֮��ķָ��
#define SPD_CONFIG_CALLNOS_STRING    "QCHAT_CALLNO = "		// �Խ�����
#define SPD_CONFIG_LOG			    "WRITE_LOG = "			// д��־, '0' ��д��'1' д
#define USR_NMASK_TRANSMITPHOTO		0x32			// ����һ���ϴ���Ƭ������
#define SPD_CONFIG_SOT			    "SpaceOfTime = "		// ����GPS��ʱ����(��)
#define DEFAULT_MAX_ARRAY_SIZE		16					// 	�ڷ���GPS��Ϣ֮ǰ��������鶨λ����
#define SPD_CONFIG_RSG			    "ReStartGps = "			// ���ֹ���ʱ�Ƿ�����GPS��mInt_ReStartGps ˵��
#define SPD_CONFIG_USRGROUP_STRING    "QCHAT_GROUP = "		// Ԥ�������ȼ�
#define SPD_CONFIG_VERSION_STRING      "VERSION = "			// 

// �йض�λ��Ȩ  Add by zjie 2012-10-08
#define SPD_CONFIG_CT_AUTH		"CT_AUTH = "		// �Ƿ�ʹ�ü�Ȩ
#define DEFAULT_CT_AUTH          '0'					// ȱʡ�ǲ�ʹ��
#define SPD_CONFIG_CUSID		"CT_SID = "			// ��Ȩ���˻�
#define DEFAULT_CT_CUSID        "35110129"				
#define SPD_CONFIG_CUPWD		"CT_PWD = "			// ��Ȩ�˻�������
#define DEFAULT_CT_CUPWD        "123456"				
// Add End	
	
#define SPD_PHOTO_TYPE			    "MOBILE_TYPE = "		// �ֻ��ͺ�
#define DEFAULT_PHOTO_TYPE			"66"					//  66 =  gota 660�� 68 = gota 680��55 = qchat holleycomm E550B , 01 = viewSonic (Q3,Q3+)
 														    // 55 = holleycomm Q660, 72 ��Ϊ7266 

#define FONT_WIDTH			        "FONT_WIDTH = "			//  �ַ����,�˴�Ϊʵ��ÿ�ж��ٸ�����
#define DEFAULT_FONT_WIDTH			22
#define FONT_HEIGHT			        "FONT_HEIGHT = "		//  �ַ��߶�
#define DEFAULT_FONT_HEIGHT			16
#define ADD_HOUR			        "ADD_HOUR = "			//  ʱ������
#define DEFAULT_ADD_HOUR            0	

#define DEFAULT_ReStartGps          2						// ��mInt_ReStartGps˵��
#define DEFAULT_sot					60						// ��mInt_sot˵��		2013-07-19 ��25�ĳ�60



#define SPD_CONFIG_PULSATION        "PULSATION = "			// ��������
#define DEFAULT_PULSATION			60						// ȱʡ60��
#define DEFAULT_INTERVAL			60								// ȱʡʱ����

#define EVT_SCREEN             (EVT_USER+1)

#define SCREEN_PARAM_GOTO      0       /* Private event */
#define SCREEN_PARAM_INIT      1
#define SCREEN_PARAM_CLOSE     2
#define SCREEN_PARAM_PAINT     3

#define SPD_BOOT_FILE             "PowerBoot.txt"
#define SPD_BOOT_OPT_STRING       "FLAG = "

#define SPD_MOD_FILE             "newgimis.mod"

#define SPD_CONFIG_FILE             "configer.txt"
#define SPD_CONFIG_OPT_STRING       "GPS_OPTIMIZATION_MODE = "
#define SPD_CONFIG_QOS_STRING       "GPS_QOS = "
#define SPD_CONFIG_SVR_TYPE_STRING  "GPS_SERVER_TYPE = "
#define SPD_CONFIG_SVR_IP_STRING    "GPS_SERVER_IP = "
#define SPD_CONFIG_SVR_PORT_STRING  "GPS_SERVER_PORT = "

// GPS��������������� [3/9/2016 Administrator]
#define SPD_CONFIG_OFFLINNEDATA_MAX_STRING   "GPS_OFFLINNEDATA_MAX = "
#define DEFAULT_OFFLINNEDATA		5

//����1289�󲥷�cari.mp3�ı�־λ
#define SPD_CONFIG_PLAY_ERROR_MP3_STRING     "CONNECT_ERROR = "
#define DEFAULT_CONNECT_ERROR       0


// ���������������ݵĺ궨��
#define SPD_OFFLINE_1             "offline1.txt"
#define SPD_OFFLINE_2             "offline2.txt"
#define SPD_OFFLINE_3             "offline3.txt"
#define SPD_OFFLINE_4             "offline4.txt"
#define SPD_OFFLINE_5             "offline5.txt"
#define SPD_OFFLINE_6             "offline6.txt"
#define SPD_OFFLINE_7             "offline7.txt"
#define SPD_OFFLINE_8             "offline8.txt"
#define SPD_OFFLINE_9             "offline9.txt"

//��������
#define SPD_CONFIG_OFFLINE_DAY  "OFFLINE_DAY = "
#define DEFAULT_OFFLINE_DAY  7 //Ĭ����7��

//�����Ѿ����ɼ��������ļ������������ļ�ʱ��ȡ��ʹ�ã�
#define SPD_CONFIG_OFFLINE_FILE_C "OFFLINE_COUNT = "
#define DEFAULT_OFFLINE_COUNT  1 //Ĭ����7��


#define SPD_CONFIG_ACCURACY_CHAR        "ACCURACY_TYPE = "			// ����  '1'��'2'��'3'��'4'��'5'��'6'
#define DEFAULT_AccType				'3'								// ��λ���ȣ�ȱʡ3��

#define SPD_CONFIG_WATCHDOG_CHAR         "WATCHDOG = "				// ���Ź�  0 ��Ҫ���Ź���  1 Ҫ���Ź�
#define DEFAULT_WatchDog			'0'				// ȱʡ 0 ��Ҫ���Ź�

// �رվɰ汾 #define SPD_CONFIG_START_CHAR			 "START_MODE = "			// ����ģʽ  '1' ���������� , '0' ����������
// �رվɰ汾  #define SPD_CONFIG_AUTHORIZE        "IF_AUTHORIZE = "			// �Ƿ���Ȩ '1' ��Ȩ '0' δ��Ȩ

#define SPD_CONFIG_START_CHAR		"MODE_START = "			// ����ģʽ  '1' ���������� , '0' ����������
#define DEFAULT_START				'0'				// ȱʡ		

#define SPD_CONFIG_AUTHORIZE        "AUTHORIZE_STS = "			// �Ƿ���Ȩ '1' ��Ȩ '0' δ��Ȩ
#define DEFAULT_AUTHORIZE			'0'			   // ȱʡδ��Ȩ

#define SPD_CONFIG_GPS_MODE			 "GPS_MODE = "			    // ���Ķ�Ҫ���GPS״̬  '1' ���� , '0' ������
#define DEFAULT_GpsMode				'0'				// ȱʡ������		


#define SPD_CONFIG_PIC_CHAR			 "PIC_MODE = "	  // ������ʱʹ�õ�ͼƬ
#define DEFAULT_PIC				     '1'			   // ȱʡ���á�O����������  ����'0','1'	
#define DEFAULT_ARRAY_SIZE			1					// 	ȱʡ���͵ĸ���

// �ֻ����մ�ŵ�·��
#define SPD_PHOTO_PATH			    "PHOTO_PATH = "		//  �ֻ����մ�ŵ�·��: root/photo/gallery/
#define DEFAULT_PATH				"root/photo/gallery/"		// ȱʡgota G660 ��·��, Qchat·��"md/__Images/"
// G680: filemgr/image/My Photo/                    

// �ֻ���Ƶ��ŵ�·��   Add by zjie 2012-09-21
#define SPD_VIDEO_PATH			    "VIDEO_PATH = "		
#define SPD_AUDREC_PATH			    "AUDREC_PATH = "		// ¼���ļ���ŵ�·��   Add by zjie 2014-04-15

#define SPD_CONFIG_USE_LOAD			 "USE_LOAD = "	// �Ƿ�ʹ�ð���װ��ģʽ  '1' ���� , '0' ������
#define DEFAULT_UseLoad				'0'				// ȱʡ������

//�ҹ��ܼ��Ƿ��������� 0 1������
#define SPD_CONFIG_RIGHT_KEY_FLAG   "RIGHT_KEY_FLAG = "
#define DEFAULT_Right_Key           '0' // ȱʡ������

//�Ƿ�����Զ�¼���ϴ����� 1��
#define SPD_CONFIG_ENABLE_AUDIO    "Auto_Upload_Audio = "
#define DEFAULT_Enable_Audio              0

#define SPD_CONFIG_ENABLE_MD5    "PICTURE_ENABLE_MD5 = "
#define DEFAULT_Enable_MD5              0


// ����������Ƭ
#define SPD_CONFIG_PHOTO			 "IF_PHOTO = "	// �Ƿ�ʹ�ð���������Ƭ,������˵��  '0' ������, '1' ���� , '2' һ���ϴ���Ƭ
#define DEFAULT_PHOTO				'0'				// ȱʡ������


#define SPD_PHOTO_WIDTH			    "PHOTO_WIDTH = "		//  ���
#define DEFAULT_PHOTO_WIDTH			176						//  ȱʡ���
#define SPD_PHOTO_HEIGHT			"PHOTO_HEIGHT = "		//  �߶�
#define DEFAULT_PHOTO_HEIGHT		132						//  ȱʡ�߶�

#define SPD_PHOTO_SEND			    "PHOTO_SEND = "			//  �Ƿ�����������Ƭ���� cPhotoSend ˵��
#define DEFAULT_PHOTO_SEND			'1'						//  ȱʡ����

#define SPD_CONFIG_POSDET_ATIMES      "POSDET_ATIME = "		// ������λ����
#define DEFAULT_POSDET_ATIMES		  10					// ȱʡ������λ����

#define DEFAULT_LOG					'0'						// ȱʡ��д			test 

#define SPD_CONFIG_PTT_KEY			 "PTT_KEY = "			// һ�����еİ���ֵ, ��ֵ����PTT��57421
#define DEFAULT_PTTKEY				 AVK_PTT				// ȱʡ
#define SPD_CONFIG_EXIT				 "IF_EXIT = "			// �Ƿ�����1#�˳��� '0'  ������   '1' ����
#define DEFAULT_EXIT				  '0'					// ȱʡ������

#define SPD_CONFIG_CHECKON_KEEP    "ATTENDANCE_KEEP_DAY = "		// ������Ϣ��ౣ������
#define DEFAULT_KEEP_DAY			7							// ȱʡ gps��������

#define DEFAULT_SPLIT_LEN			2					//  ����ָ����ַ����ռ�

#define SPD_CONF_TA				"CONN_ERR_TIME_A = "			// ���ò���TA(-1289�����೤ʱ�������ֻ�)
#define DEFAULT_TA				 20					// ȱʡ30����
#define SPD_CONF_TB				"CONN_ERR_TIME_B = "			// ���ò���TB(ÿ���ĸ�ʱ�������ֻ�)
#define DEFAULT_TB				 "25"					// 25 �������ֻ�, ������˵��

#define SPD_CONFIG_INTERVAL_GET_STRING    "GPS_INTERVAL_GET = "		// ����GPS��Ϣ��ʱ����(�ɼ�����)
#define DEFAULT_INTERVAL			60								// ȱʡʱ����

#define SPD_CONFIG_ASSIST_GPS_INTERVAL    "ASSIST_GPS_INTERVAL = "		// �����ɼ�����,ȱʡ���ǲɼ�����

#define SPD_CONFIG_CENTER_IP_STRING      "CENTER_IP = "				// GPS���Ķ�IP��ַ
#define SPD_DEFAULT_CENTER_IP          ""						// ȱʡ�����Ķ�IP��ַ�գ���Ȩ�������ṩ�ȴ�

#define SPD_CONFIG_POSDET_MODE_CHAR      "POSDET_MODE = "			// ��λ��ʽ 0 OneShot ��ʽ, 1 Local��ʽ
#define DEFAULT_PosDetMode			0				// ȱʡ��λ��ʽ	

#define SPD_CONFIG_ARRAY_SIZE		"ARRAY_SIZE = "

#define SPD_QOS_DEFAULT       127


// �ֻ�״̬��Ϣ���� ��ʼ
#define USR_NMASK_SS_TAPI			0x7				// 2^0 + 2^1 + 2^2:  TAPI ������ 
#define USR_NMASK_SS_RSSI			0x8				// 2^3:  1x �Ѿ����� 
#define USR_NMASK_SS_HDR_RSSI		0x10			// 2^4:  evdo �Ѿ����� 
#define USR_NMASK_WAIT_IDLE			0x20            // �ȴ�������״̬תΪIDLE״̬  2^5
#define USR_NMASK_GPS_ONE			0x40            // ��һ��ʹ�����Ƕ�λ  2^6			// Add  2013-04-27
#define gimis_HasStatus(pMe,nMask)     (pMe->cPhoneSts & nMask)    	// ��������ȡ��ĳһλ�õ�ֵ
//������������ĳһλ�õ�ֵ
#define gimis_SetStatus(pMe, nMask)  { pMe->cPhoneSts = (nMask<0x8) ? (pMe->cPhoneSts & 0xF8) | nMask : pMe->cPhoneSts | nMask;	}
// �ֻ�״̬��Ϣ���� ����

#define EVT_CHECKIN_EVENT			EVT_USER+1010				// ����ָ��������Ϳ�����Ϣ

#define  MAXTEXTLEN 22

#define GPSGROUP 3

/////// ��HTTP�йصĶ���//////////////////////////////////////////////////////////////////////////
#define FIND_STRING(a, b) if (STRISTR(a, b) != NULL)
#define TEST_HOST  "http://192.168.84.40/" //"http://10.10.10.84/"





#define XTRA_EVT_USER_QQSQ_CG EVT_USER + 2100  //������Ȩ�ɹ�
#define XTRA_EVT_USER_QQSQ_SB EVT_USER + 2101  //������Ȩʧ��
#define XTRA_EVT_USER_QQSQ_SB_WLYC EVT_USER + 2102  //������Ȩʧ��
#define XTRA_EVT_USER_XT_CG EVT_USER + 2103  //������Ȩ�ɹ�
#define XTRA_EVT_USER_PRINT EVT_USER + 2104  //��ӡ ���� �� 554 1289 ��
#define XTRA_EVT_USER_ERR_GPS EVT_USER + 2105 // ����10004
#define XTRA_EVT_USER_NAL_GPS EVT_USER + 2106 // ����10004


//###����###2015-12-28
#define XTRA_EVT_USER_DISPLAY		EVT_USER + 2107 //���������׳������ػ�����¼�
//���������񣬷��ͽ���ĸ��ֵ�����ʾ
#define  TASK2_EVT_USER_DISPLAY		EVT_USER + 2108 //���������������׳������ػ�����¼�
//����list���棬���ֵ�����ʾ�¼�
#define  TASKLIST_EVT_USER_DISPLAY  EVT_USER + 2109


//����
#define XTRA_EVT_USER_NAL_BD EVT_USER + 2110 // ����BD

#define XTRA_EVT_USER_ERR_BD EVT_USER + 2111 // ����BD


// 
// #define  EVT_MESSAGE_B		EVT_USER + 2010

#define IMG_FILE_FAILURE			"kqsb.bmp"				// ����ʧ��״̬ͼƬ�ļ���
#define IMG_FILE_SUCCESS			"kqcg.bmp"				// ���ڳɹ�̬ͼƬ�ļ���
#define IMG_FILE_SENDING			"sending.bmp"			// ���ڷ���״̬ͼƬ�ļ���
#define IMG_FILE_SENDFAILURE		"kqfssb.bmp"			// ���ڷ���ʧ��״̬ͼƬ�ļ���
#define IMG_FILE_KEEP				"kqbccg.bmp"			// ����״̬�ѱ���,��δ����
#define IMG_FILE_NOSTS				"mykqsj.bmp"			// û�п��ڶ�λ����
#define IMG_FILE_SENDOK				"kqfscg.bmp"			// ���ڷ��ͳɹ�
#define IMG_FILE_NONET				"wlbnsy.bmp"			// ���粻��ʹ��
#define IMG_FILE_ZTBSL				"ztbsl.bmp"				// û�ж�λ����״̬������
#define IMG_FILE_YWWJ				"ywwj.bmp"				// �����ļ�Ҫ����		
#define IMG_FILE_WLCSH				"wlcsh.bmp"				// �����ʼ�����������ֻ�		// Add by zjie 2013-04-12
#define IMG_FILE_SCLYWJ				"zzsclywj.bmp"			// �����ϴ�¼���ļ�		// Add by zjie 2014-04-10

//////  ����Ƭ�йصĺ궨��
// ��Ƭ�ĺ�׺
#define PHOTO_FILE_EXTENSION        "jpg"               // photo file extension
#define TEXT_MAXSIZE				100					 // ��Ƭ������������ַ���


#define DEFAULT_SPLIT				","					// �������ݵ�ͬһ�ֶ��ַ���֮��ķָ��   


#define LOG_STR_LEN			  256 					//  д����־�ļ�һ�е���󳤶�,�糬���˳������õ��ô˶���
#define NUM_VERSION				        2061000				// ��ֵ�Ͱ汾�ţ������汾���жϰ汾��������


//�����ļ��м��ֻ��ͺţ�ת���ַ�
#define  ESC_EQ             "ZA="                    // '=' ת��� ' '
#define  ESC_SP             " "                      
#define  ESC_SUB            "ZA-"                    // '-' ת��� '_'
#define  ESC_ZA_             "_"                      
// Add by zjie 2013-03-15 ת���ַ�
#define  ESC_for_asterisk         "ZA:"                    // ':' ת��� '*'
#define  ESC_ASTERISK             "*"        

///////////////////////   2015-10-26 ����ͨѶ¼�� ��ʼ   //////////////////
#define COLON_SPLIT					":"					        // �������ݵ�ͬһ�ֶ��ַ���֮��ķָ��,����ͨ��¼�ֶμ�ָ�
///////////////////////     END  ����ͨѶ¼�Ľ���        //////////////////


#define  FILE_OFFDATA 1024  //������ǵ����ļ������ֵ
#define  ITEM_OFFDATA 180  //������ǵ����ļ������ֵ

#define  LX_FILE_OFFDATA 1200  //������ǵ����ļ������ֵ

#define DEFAULT_KEYTIME	3			// �û��������ﵽ�涨��ʱ��(��),�ŷ����û�����

#define TASK_SET_TIMER  30	//���ں�̨ʱ,��������ʱ�������  �Լ�����http�йصĳ�ʱʱ��[3/4/2016 Administrator]

//¼���ļ�����Ϣ�궨��
#define Record_Info_Name          "fs:/ftl1/Audio/Qchat/car/info.txt"

#define Record_Info_Name_NoDel	  "fs:/card0/Audio/Qchat/car/info.txt"

#define Record_Info_Size          50000


/* An enumeration for different screens in the applet. */
typedef enum _ScreenId {
		SCREENID_MAINMENU,
		SCREENID_SHOWGETGPSINFO,
		SCREENID_YJKQ,
		SCREENID_SENDEMPTY,
		SCREENID_FULL,
		SCREENID_SENDPHOTO,
		SCREENID_SENDAUDIO,
		SCREENID_TASKLIST,
		SCREENID_NFC,
		SCREENID_CONFIG,
		SCREENID_GETGPSINFO,
		SCREENID_MORE,
		SCREENID_DETAIL1,
		SCREENID_DETAIL2,
		SCREENID_CONTENT,
		SCREENID_NOTIFY,     //#��̨ģʽ��������#
		SCREENID_MAX  
		
		/* Insert new screens above this entry
		
		** and its association in gAppScreens */
} ScreenId;

typedef struct _CTextCtlData {
	ITextCtl *pTextCtl;
	IMenuCtl *pSoftKey;
} CTextCtlData;

/* Maintains the screen to its handler association */
typedef struct _CScreen {
	AEEHANDLER theHandler;
} CScreen;

typedef struct _CSettings {
	AEEGPSOpt      optim;
	AEEGPSQos      qos;
	AEEGPSServer   server;
} CSettings;

//�����Ƭ�Ľṹ��
typedef struct _SavePhoto
{
	int i_dex; //����
	char photoname[65];//��Ƭ����
}SavePhoto;
/*
typedef enum {
	MAINMENU_ITEM_ONE_SHOT,
		MAINMENU_ITEM_TRACK_LOCAL,
		MAINMENU_ITEM_TRACK_NETWORK,
		MAINMENU_ITEM_DLOAD_FIRST,
		MAINMENU_ITEM_TRACK_AUTO,
		MAINMENU_ITEM_CONFIG
}MainMenuItems;
*/
typedef enum {
	    MAINMENU_ITEM_YJDK,
		MAINMENU_ITEM_YJKQ,
		MAINMENU_ITEM_PHOTO,
		MAINMENU_ITEM_AUDIO,
		MAINMENU_ITEM_MORE,
		MAINMENU_ITEM_GPS,

		// ����Ƭ�й�
  Menu_write,		// ��������	
  Menu_right,		// ��һ��
  Menu_left,			// ��һ��
  Menu_del,			// ɾ��һ����Ƭ
  Menu_rtn,			// ���ض�λ����
  Menu_input		// �л��������
// Add End
}MainMenuItems;

typedef enum {
		SUBMENU_LOCAL_INFO,
		SUBMENU_NFC,
		SUBMENU_FULL,
		SUBMENU_EMPTY,
		SUBMENU_TASK_LIST,
		SUBMENU_SEND_LOG,
		SUBMENU_SPEAK_MANAGER	
// Add End
}MainMenuItems1;


typedef struct
{
	uint8  nPartsTotal;				// �ܶ���
	uint8  nPartNum;					// ���ڴ��ڼ��Σ�1 ��1�Σ�2 ��2�Σ�����������  
	char   cStatus;					// 0: û���յ��µ��ϴ�ָ��, 1: �յ��µ��ϴ�ָ��
	
	uint16 recordId;	            //¼���ļ���� ������ҳ��λ
	uint32 recordSize;	            //¼���ļ����ܴ�С
    char   recordName[AEE_MAX_FILE_NAME];	//¼���ļ���
	
	char   recordInfo[1998];
}UploadRecord;

//��Ƶ�·�http���յ�����
typedef struct 
{
	int icount;//���Ŵ���
	char cName[128];//��������
	char cTel[16];   // �ֻ�tel
}YYXF_INFO;

//��Ƶ���нṹ�嶨��  
typedef struct  
{  
    YYXF_INFO *pBase;    //���ڶ�̬�����ڴ�,pBase����������׵�ַ  
    int front;      //ָ��ͷ���  
    int rear;       //ָ�����һ��Ԫ�ص���һ���  
}QUEUE;

  


/*-------------------------------------------------------------------
Applet structure. All variables in here are reference via "pMe->"
-------------------------------------------------------------------*/
// create an applet structure that's passed around. All variables in
// here will be able to be referenced as static.
typedef struct _newgimis {
	AEEApplet      a ;	       // First element of this structure must be AEEApplet
    AEEDeviceInfo  DeviceInfo; // always have access to the hardware device information
    IDisplay      *pIDisplay;  // give a standard way to access the Display interface
    IShell        *pIShell;    // give a standard way to access the Shell interface

    // add your own variables here...
	char         cWrtLog;											//  �Ƿ�д��־�� '0' ��д, '1' д
	uint32		nInd;												// ��¼��־�Ĵ���	iInd
	uint16		mInt_StartRun;		//  ��������������: 1 : �յ�init��Ϣ, 4 : 1x ����, 5 : evdo ����
	// 5/35 ����������, 6/36 �ֶ�����,  7/37�������� ,8/38 ��̨��������,9/39 ǰ̨��������
	//   20 ~ 30 �� ���ڽ����������صȳ�ʼ������
									//  ԭ�� 0/20 �ֶ�����, 11/1/21 ����������, 3/23�������� ,2/22,4/24 ��������	

	// Add by zjie 2012-10-08
	char         cCTAuth;				// �Ƿ�ʹ�õ��Ŷ�λ��Ȩ�� 0 ��ʹ�ã� 1 ʹ��
	char		    sCusID[17];					// ���Ŷ�λ��Ȩ���˻�
	char		    sPassword[13];				// ���Ŷ�λ��Ȩ�˻������� 
	// Add End

	CSettings   gpsSettings;
	uint32		ipulsation;				// �������ڣ���λ��
    uint32		iInterval;				// ����GPS��ʱ��������λ��
	uint32		iAssInterval;			//  ʹ�ø�����λʱ�Ĳɼ�����	
	char         strCenterIp[151];		// GPS���Ķ�IP��ַ
	uint16		cPosDetMode;			// ��λ��ʽ '0' OneShot ��ʽ, '1' Local��ʽ, '2'.... ���ĵ�˵��
	
	char        m_cPosDetMode[10];
	
	char			cAccType;				// ��λ���� '1'��'2'��'3'��'4'��'5'��'6',ȱʡΪ'3'
	int		    m_Acc ; //�ȼ�����   ��λ���� '1'��'2'��'3'��'4'��'5'��'6',ȱʡΪ'3'

	char			cWatchDog;				// ���Ź�  '0' ��Ҫ���Ź���  '1' Ҫ���Ź�, '2' �����ֻ�
	char		    cStartMode;			   // �Ƿ񿪻�������, '1' ���������� , '0' ����������
    char			cAuthorize;			   // �Ƿ���Ȩ,'1' ��Ȩ '0' δ��Ȩ
	char		    cGpsMode;			   // ���Ķ�Ҫ��Ŀ�����GPS״̬  '1' ����GPS , '0' �ر�GPS
	// '2' ���Ķ�Ҫ�󱾴ιرճ��򣬵���д�뵽�����ļ��У�����������Ч��
	char		    cUseLoad;			   // �Ƿ�ʹ�ð���װ��ģʽ  '1' ʹ�� , '0' ��ʹ��

	char         cPicMode;			   // ������ʱʹ�õ�ͼƬ '0'- �á��ճ��������س��� ����״̬'0','1'  1 -����״̬ 0 1   2 -����  3 -NFC ����
	//                    '1'  �á�O����������  ����'0','1' 	
	//	                 '2'  ���Ϳ�����Ϣ 
									   //                    '3'  ����NFC������Ϣ   2013-09-01	

	int			nArySize;					// �ڷ������ݵ�֮ǰ���漸�����ݣ��ò��������ñ���

	char         sAudRecPath[MAX_FILE_NAME+1];						// �ֻ�¼���ļ���ŵ�·��     Add by zjie  2014-04-10
	char         sVideoPath[MAX_FILE_NAME+1];						// �ֻ���Ƶ�ļ���ŵ�·��
   char         sPhotoPath[MAX_FILE_NAME+1];						// �ֻ���Ƭ��·��	  ԭ���ֽ���512�� 2012-09-21

   char		    cUsePhoto;											// �Ƿ�����������Ƭ  '1' ���� , '0' ������
   // Change by zjie 2012-09-21 �����������
   // '0'  ��������Ƶ����Ƭ
   // '1'  ��������Ƶ������Ƭ, ��ASC���0���ֽ�λ��
   // '4'  ������Ƶ����������Ƭ, ��ASC���2���ֽ�λ��
   // '5'  ������Ƶ��������Ƭ
   // '7'  ������Ƶ��һ���ϴ���Ƭ

   int			iPhotoWidth;										// ��ʾ��Ƭ�Ŀ��
   int			iPhotoHeight;										// ��ʾ��Ƭ�ĸ߶�

   char         sMobileType[3];										// �ֻ��ͺ�

   uint32  iFontWidth;				// �ַ����,�˴�Ϊʵ��ÿ�ж��ٸ�����
   uint32  iFontHeight;				// �ַ��߶�
   uint32  iAddHour;				// ʱ������
   // ���ֶκ����������޸�  Change by zjie 2012-09-21 
									// ��λ+ʮλ����ֵΪ��Ƭʱ������, ��λ+ǧλ����ֵΪ��Ƶ�ļ�ʱ������

   int  mInt_ReStartGps;			// 0 ������GPS�� 1 ��1�γ��ֹ���ʱ����gps, 2 ÿ���յ�����ʱ������GPS
   int  mInt_sot;					//  ����GPS ʱʱ����(��) 

   char		    cPhotoSend;											// �Ƿ�����������Ƭ  '1' ������������������֣� , '0' ���أ�����������֣�
   uint32		iPosDetAtime;										//  ������λ����
   // Add by zjie 2013-01-01
   uint32		nPttkey;											//  һ�����еİ���ֵ, ��ֵ����PTT��57421
   char		    cCanExit;											//  �Ƿ�����1#�˳��� '0'  ������   '1' ����

   char    *sUsrGroup;							// ���ȼ�����Ϣ
   char	   *sPttCallNo;							// �����ļ���һ�����еĶԽ�����	
   int     nKeepDays;								// �������ݱ��������

   int      iErrConTa;							// WEB_ERROR_CONNECT����ʱ��
   char     sRestTb[11];						// �����ֻ�ʱ���, ��1-2λ��ʾʱ��Σ���3-10������������yyyymmdd
   int		iRestTb ;// ����ʱ��  == 25 ��ʱ��˵������ʱ���ǹرյ�
   int		iTBCT ;  // ������ŵ�ǰ��ǰ��ʱ��  ������ �����ǰ������ʱ��Ϊ 5�� ������ʱ����4�㣬��ô����Ҫ������
   char		sVer[11];				// �汾��
   char		sVer_1[11];				// �汾��  û��V ��
   char		MyMobileNo[64+1];					// �����ĵ绰����,  Add 2013-09-23
   char		MobileNoDJ[64+1];					// �����ĵ绰���루�Խ���,  Add 2013-09-23


	char         cPhoneSts;				// ��ǰ�ֻ�(������)״̬  �����ֻ�״̬��Ϣ���롱����  
	//   ��������ֻ�״̬��Ϣ���롱���崦��
										//  ��   '0' �ȴ�������״̬����ΪIDLE״̬ , '2' ����,
	char     szMobileID[MOBILE_ID_LEN + 1];		// 2013-09-23   �ֻ����Ĵ���

    char     szMobileID_Configer[MOBILE_ID_LEN + 1];		// 2013-09-23   �ֻ����Ĵ���


	//  ================ û�����������(�����߿��ڣ����濼����Ϣ =======================
   int	   nSavInd;									// 0 û�����߿������ݣ�>0 �����߿�������



   void *pXtrxDcApp;							// ��������Ӧ�õ��ĽṹXtraDcApp


   //// ������GPS�йصĿ�ʼ ��
   void        *pScreenData;
   void        *pScreenData_gps;
   void        *pScreenData_Audio;
   void  *Old_ScreenData;  // �ɵ�ҳ��ָ�� ���ڱ���GPSҳ���ֵ
   void  *New_ScreenData; //�µ�ҳ��ָ�� ���ڱ��浱ǰҳ���ָ�� �Ⱦɵ��ͷź����¸� currentHandler
   AEEHANDLER  currentHandler;
   CScreen     appScreens[SCREENID_MAX];
   boolean GPSFlag;		//	FALSE ��ʾ�ر� TRUE ��ʾ����
   char	   cPosModused;				// ��ǰ��λʹ�õ�ģʽ

   uint32		iAssLastTime;			//  �ϴ�ʹ�ø�����λ��ʱ��		Add by zjie 2013-04-12	
   // ������GPS�йصĽ���������

   IStatic *		m_pIStatic;



   /////////  �� ������Ȩ�йص� ��ʼ
   IWeb *   m_pIWeb_qqsq; //����������
   int					m_iAppStatus_qqsq;
   char                 *pszPostData_qqsq;  // POST string
   IPeek                *pipPostData_qqsq;  // POST stream, if any
   AEECallback          cb_qqsq;            // how to call me back
   uint32               uStart_qqsq;
   IWebResp *           piWResp_qqsq;       // the answer I get from IWeb
   ISource*   pSource_qqsq;
   byte *		m_BodyBuffer_qqsq;		// Buffer to hold HTML data
   int			m_BodySize_qqsq;			// Size of HTML data in buffer
   int			m_BodyAllocSize_qqsq;	// Allocated size of HTML data buffer

   char m_SQ1[250]; // ��Ȩ������ַ ������ ���������л����ڶ�����ַ
   char m_SQ2[250]; // ��Ȩ�ĵڶ�����ַ �������ַ�����ã����������ַ
   int m_SQ_C ;// ��Ȩ��ַѡ�� ���Ϊ 1 ��Ϊ����ַ��Ȩ ���Ϊ 2 ��Ϊ�ڶ���ַ��Ȩ
   //////////  �������йصĽ���  ///////



   /////////////////////    ������HTTP�йص� ��ʼ ////////////
   int					m_iAppStatus;
   char                 *pszPostData;  // POST string
   IPeek                *pipPostData;  // POST stream, if any
   AEECallback          cb;            // how to call me back
   uint32               uStart;
   IWeb*	m_pIWeb;
   IWeb*	m_pIWeb_s;  //�����ϱ�GPSֵ��
   /////////  �� �����йص� ��ʼ
   IWeb *   m_pIWeb_xt; //����������
   int					m_iAppStatus_xt;
   char                 *pszPostData_xt;  // POST string
   IPeek                *pipPostData_xt;  // POST stream, if any
   AEECallback          cb_xt;            // how to call me back
   uint32               uStart_xt;
   IWebResp *           piWResp_xt;       // the answer I get from IWeb
   ISource*   pSource_xt;
   byte *		m_BodyBuffer_xt;		// Buffer to hold HTML data
   int			m_BodySize_xt;			// Size of HTML data in buffer
   int			m_BodyAllocSize_xt;	// Allocated size of HTML data buffer

   char Url_st[250]; //������ַ
   //////////  �������йصĽ���  ///////

    /////////  �� �ϱ��������� ��ʼ
   IWeb *   m_pIWeb_lx; //����������
   int					m_iAppStatus_lx;
   char                 *pszPostData_lx;  // POST string
   IPeek                *pipPostData_lx;  // POST stream, if any
   AEECallback          cb_lx;            // how to call me back
   uint32               uStart_lx;
   IWebResp *           piWResp_lx;       // the answer I get from IWeb
   ISource*   pSource_lx;
   byte *		m_BodyBuffer_lx;		// Buffer to hold HTML data
   int			m_BodySize_lx;			// Size of HTML data in buffer
   int			m_BodyAllocSize_lx;	// Allocated size of HTML data buffer
   //////////  ���뷢�������й� ����  ///////


    /////////  �� �ϱ��������� ��ʼ
   IWeb *   m_pIWeb_slx; //����������
   int					m_iAppStatus_slx;
   char                 *pszPostData_slx;  // POST string   //��ȡ�����������ݣ�׼������ȥ
   IPeek                *pipPostData_slx;  // POST stream, if any
   AEECallback          cb_slx;            // how to call me back
   uint32               uStart_slx;
   IWebResp *           piWResp_slx;       // the answer I get from IWeb
   ISource*   pSource_slx;
   byte *		m_BodyBuffer_slx;		// Buffer to hold HTML data
   int			m_BodySize_slx;			// Size of HTML data in buffer
   int			m_BodyAllocSize_slx;	// Allocated size of HTML data buffer
   //////////  ���뷢�������й� ����  ///////


   /////////  �� �ϱ� ����GPS ���� ���� ��ʼ  �������Ҫ���������س������ʱ�� _s ����ӿڵ�HTTP�ͻ���Ӱ��
   IWeb *   m_pIWeb_gps; //����������
   int					m_iAppStatus_gps;
   char                 *pszPostData_gps;  // POST string
   IPeek                *pipPostData_gps;  // POST stream, if any
   AEECallback          cb_gps;            // how to call me back
   uint32               uStart_gps;
   IWebResp *           piWResp_gps;       // the answer I get from IWeb
   ISource*   pSource_gps;
   byte *		m_BodyBuffer_gps;		// Buffer to hold HTML data
   int			m_BodySize_gps;			// Size of HTML data in buffer
   int			m_BodyAllocSize_gps;	// Allocated size of HTML data buffer
   //////////  ���뷢�������й� ����  ///////


   //////////////   ���ϱ����� GPS�����йغ� ��λ�쳣�ϱ��йص�  ///////////////
   IWeb *   m_pIWeb_E; //����������
   int					m_iAppStatus_E;
   char                 *pszPostData_E;  // POST string
   IPeek                *pipPostData_E;  // POST stream, if any
   AEECallback          cb_E;            // how to call me back
   uint32               uStart_E;
   IWebResp *           piWResp_E;       // the answer I get from IWeb
   ISource*   pSource_E;
   byte *		m_BodyBuffer_E;		// Buffer to hold HTML data
   int			m_BodySize_E;			// Size of HTML data in buffer
   int			m_BodyAllocSize_E;	// Allocated size of HTML data buffer
   int iPostPhTm_E; //��ǿ�ʼ�����ʱ�� �ϱ���ʱ HTTP����
   ////////     END   ///////////////



   IWebResp *           piWResp;       // the answer I get from IWeb
   IHtmlViewer *        m_pHTMLViewer;  
   ISource*   pSource;
   int			m_BodySize;			// Size of HTML data in buffer
   int			m_BodyAllocSize;	// Allocated size of HTML data buffer
   byte *		m_BodyBuffer;		// Buffer to hold HTML data
   IFileMgr * m_pIFileMgr;
   int m_iMediaType;
   char	* m_pszFileName;
   char targetUrl[128];
   AEERect              m_rc; 

   int i_HttpMod; //HTTP���շ�������ģʽ ����	1 - ������ջ�������������Ȩ�� 2 - ���ջ�������GPS�����ϴ�����������
   //////////////////////   ��HTTP�йص� ����   //////////////

   char		 sUrl[1024];  //�������͵���ַ ��ʱ���ڷ��� GPS����

   // GPS ���յ� ��γ�� �Լ� ˮƽ�ʹ�ֱ�������ֵ
   double lat;       /* latitude on WGS-84 Geoid */
   double lon;       /* longitude on WGS-84 Geoid */
   uint16 dVelHor;	/*  ˮƽ�����ٶ� ��������0.25 meters/second Ϊ��λ  */
   int8   dVelVer;	/*  ��ֱ�����ٶ� ��������0.5 meters/second Ϊ��λ*/	

   //���� ��λ�ľ�γ�Ⱥ�ˮƽ�ٶ�
	char   latStr_BD[MAXTEXTLEN];
	char   lonStr_BD[MAXTEXTLEN];
	
	uint16 dVelHor_BD;

	int m_PM_BD;

   //  ���������ͼ�������
   int m_GPSGroup; // ��ʾ���ͼ���GPS����


	char sDateTime1[20];
	char sDateTime2[20];
	char sDateTime3[20];

	char sDateTime4[20];
	char sDateTime5[20];
	char sDateTime6[20];
	char sDateTime7[20];
	char sDateTime8[20];


	
	char   latStr1[MAXTEXTLEN];
	char   lonStr1[MAXTEXTLEN];
	char   latStr2[MAXTEXTLEN];
	char   lonStr2[MAXTEXTLEN];
	char   latStr3[MAXTEXTLEN];
	char   lonStr3[MAXTEXTLEN];
	char   latStr4[MAXTEXTLEN];
	char   lonStr4[MAXTEXTLEN];
	char   latStr5[MAXTEXTLEN];
	char   lonStr5[MAXTEXTLEN];
	char   latStr6[MAXTEXTLEN];
	char   lonStr6[MAXTEXTLEN];
	char   latStr7[MAXTEXTLEN];
	char   lonStr7[MAXTEXTLEN];
	char   latStr8[MAXTEXTLEN];
	char   lonStr8[MAXTEXTLEN];

	uint16 dVelHor1;
	uint16 dVelHor2;
	uint16 dVelHor3;
	uint16 dVelHor4;
	uint16 dVelHor5;
	uint16 dVelHor6;
	uint16 dVelHor7;
	uint16 dVelHor8;
	
	char m_PM;//���ڵõ���ǰ�Ķ�λģʽ��
	char m_PM1;
	char m_PM2;
	char m_PM3;

	char m_PM4;
	char m_PM5;
	char m_PM6;
	char m_PM7;
	char m_PM8;


	char m_ycPM; //�����ʱ��λ��ʽ



	//////////////  ��������Ƭ�йصı���
	int			iPressSts;											// �Գ������Ĵ���״̬:
																	// 0 û�а����� 1 �û�����
																	// ���´���״̬:
																	// 2 �û������ﵽ�涨ʱ�� 3 ׼�����Ͱ���ֵ
																	// 4 �Ѿ����Ͱ���ֵ  5 ����splash screen 
																	// ���´�����Ƭ
																	//  12 �û������ﵽ�涨ʱ��
																	//  13 ����Ƿ������µ���Ƭ
																	//  14 ������ʾͼ��
																	//  15 ���ڴ���ͼ��
																	// ���´�����ʾ��ʾͼƬ
																	//   20 ׼����ʾͼƬ
																	//   21 ����splash screen
																    //   25 ��ʾ��ϢͼƬ(������)
																	//   26 ��ʾͼƬ(�����ļ�Ҫ����)
																	//   27 �����ϴ�¼���ļ� 
																	// ���´��������µ�����( 30~ 39)  Add by zjie 2012-05-10 
																	//	 30 ׼����ʾ�����༭����
																	//   31 ���������༭����
																	// Add End 2012-05-10
																	// ������ʾ�����ʼ����Ϣ
																	//   35 ׼����ʾ�����ʼ����Ϣ
																	//  Add 2013-04-27
																	//   40 ׼����ʾ��ѯ���ڵ���Ϣ
																	//  Add 2013-09-01 
																	//   50 ��ʾNFCˢ������
																	//  Add 2013-10-01
																	//    60 ��ʾ����Ľ���	

																	//    70 �й������Ĵ���			Add 2014-01-04

	int          iShowText;											// 1 �ڻ�ͼƬ��ͬʱ���»�������Ϣ�ؼ�
																	// 2 �ڻ�ͼƬ��ͬʱ�����»�������Ϣ�ؼ�
																	// 3 ����ѡ�����뷨
																	// 4 ��ˢ����Ļ  
																	// 0 δ���� 

	int			iSendPhoto;											// 0 û�з�����Ƭ��1 ׼�����ͣ� 2 ���ڷ���
																	// 3 ���ͳɹ��� 4 ����ʧ��
	IImage		*pi;												// ͼ����Դ
	IImage		*pi_copy;											// ͼ����Դ ���ݵ�
	int			imgidx;												// ����ʾ����Ƭ������, 1 ��1�ţ�2 ��2��
	char		sFileName[MAX_FILE_NAME+1];							// ���ϴ���Ƭ���ļ������ļ��������� 2012-10-16  p+yyyymmddhhmmss.jpg
	char         sFile[MAX_FILE_NAME+1];	// �ļ�������·����	
	void *FleUpdApp;								// �ϴ��ļ�Ӧ�õ��Ľṹ
   
	ITextCtl		*textCtl;											// ����Ƭ��ص��ı���
    IMenuCtl		*textmenu;											// ���ı���ؼ����
	AECHAR		pText[TEXT_MAXSIZE+1];								// ����Ƭ�йص�����,���100��˫�ֽ��ַ�

	int m_gpsFlag;		// �������GPS��ģʽѡ��  0-������   1- �ֻ���ѡ 2-��վ���� 3--����վ 4--���� 5--����

	AEERect                 m_rScreenRect;  // Screen Rect

	boolean m_FOBFlag ;//ǰ��̨�ı�־λ  0-��ǰ̨����  1-�ں�̨����
	int m_WitchPag; //������¼���ĸ�ҳ��  0-��ҳ�� 1-��GPS���Ǹ�ҳ��   4-������Ǹ����� 12-�������?	21-��������������ѽ���
											//16 �����б�SCREENID_TASKLIST


	// /// ����Ƭ�й�
	IStatic*                m_PhotoTextCtl; 
	SavePhoto m_photo[5];
	int m_i_photo;//����ָʾ��ǰ��ʾ�ĵڼ�����Ƭ
	boolean m_bSendPF ;//��������Ƿ����ڷ���ͼƬ�ı�־λ TRUE -- ��ʾ���ڷ���״̬  FALSE -- ��ʾ����

	// ��һ�������й�
	char	sUrl_yjkq[200];  //�������͵���ַ ��ʱ���ڷ��� GPS����
	char	sUrl_Cyjkq[200];  //�������͵���ַ ��ʱ���ڷ��� GPS����

	char sUrl_lx[251]; //�������ߵ���ַ

	// ����һ�������йص�
	char   lx_lon[MAXTEXTLEN];  // LO ����
	char   lx_lat[MAXTEXTLEN];	// LA γ��
	char   lx_vv[MAXTEXTLEN];	// VV ˮƽ�ٶ�
	char   lx_ep;				// EP ��Դ
	// KS	��ť ����Ĭ�϶��� 2
	char   lx_tt[15];			// TT ʱ��
	char   lx_pm;				// PM ��λģʽ


	// ����Ƶ�йصı���
	SavePhoto m_Audio[5];



	/////  ϵͳ��Ļ��һЩ����  //////////
	int m_SYSFNH;//ϵͳ��׼������߶�
	int m_SYSFBH;//ϵͳ�������߶�
	int m_SYSCX;//ϵͳ��Ļ���  //��Ƭ���
	int m_SYSCY;//ϵͳ��Ļ�߶�
	int m_MenuH;//�˵��ؼ��ĸ߶�  

//	int m_PhotoW; //��Ƭ���
	int m_PhotoH; //��Ƭ�߶�

	boolean m_bGetGPSF;//������Ǵ�ʱ��ȡGPS��״̬  TRUE -- ��ʾ��ȡ�쳣 FLASE -- ��ȡ����

	
// ������ϼ���ʹ��
	uint16       clsKey[2];				// ������ϼ���ʵ������ϼ��ػ�,ֻ�������յ�2��������ϼ����ܹػ��������򲻹ػ�
	boolean m_RelyExit;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨

	int	   m_OffData;									// 0 -- û�����߿������ݣ�1 -- �����߿�������
	int	   m_Mutex_LX; //�������ߵĻ��� ��Ҫ��������Ȩ�ɹ� �� �����ɹ� ��ȥ������û����������  0 -- ��ʾ����������  1 -- ��ʾ�����ɹ�����ȥ����
	int	   m_WitchLX ;//������ǵ�ǰ���͵��ǵڼ��������ļ�

	int	   m_Flag_F; // ������ǵ�ǰ������������ʲô����  1 -- һ������      2 -- ���Ϳճ�     3 --  �����س�  4 -- ����״̬ 0   5 -- ����״̬1  6 -- һ���򿨣�����ת����

	int	   m_Flag_Qqsq ; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����

	int    m_F_R_O;   // ��������ǲ��������Ŀ���  // 1 - ��ʾ��������  0 - ����  �����ʾλ ��  AEECLSID_PHONENOTIFIER 
   
	int	  m_T_L; //��ʾ�ܺ���  m_total_line ����д����ʾ



	char m_PowerFlag;// ������������ܷ�����  FLASE -- ������  TRUE -- ����

	uint32 m_Error ;// ������� HTTPͨ�ŵĴ�����ȷ���صĴ��� �����Ƕ�λ�쳣�Ĵ������
	char show_Time[10]; // �����ڽ�������ʾ��ʱ��
	char show_LatLon[30]; // �����ڽ�������ʾ��γ��


	//  �����ǰ���ϼ������ǳ��������йر���
	
	int m_Flag_Key;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
	int m_Witch_Key; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ   3 -- �س�  4 -- �ճ�  9 -- ״̬ 1   10 -״̬ 0   
					 // 11- һ����-�������ŵ�������ת��				

	int	m_PressSts;  //������Ǵ��������İ���״̬  1 - ����  0 - û�а���	
	boolean zuhe_loong ;// �����������ϼ������ǳ�����  0 - ������  1 - ��ϼ�
	char m_zuhe[5]; // �����ϼ� * # 9 9 9  ���ڿ��ٽ�������
	uint32       iGetkey;			// ��¼������ʱ�䳤�ȣ��ж��Ƿ񳣰�һ�������ں�̨��ǰ̨),�û����¼���ʱ��,����������3��󷢳�״̬
	uint16 wParam;					// ������Ϣ�¼��Ĳ���,   ����������ʱ�����õ�
									// ����״̬��,���水��ֵ
	int m_Key_Count ;// ������ǵ�ǰ�ж��ټ�������
	

	//  ������������Ȩ��ʱ��ʱ���õ�
	int iPostPhTm; //��ǿ�ʼ�����ʱ��
	int iPostPhTm_xt; //��ǿ�ʼ�����ʱ�� ��������HTTP����
	int iPostPhTm_gps; //��ǿ�ʼ�����ʱ��  gps ��HTTP����

	int iPos_kq_zt; //��ǿ�ʼ����һ�����ں�״̬���͵� ���ճ� �س� ״̬1 ״̬0��

	int m_F_QQSQ_CS; //��������ǲ���������Ȩ��ʱ  1 -- ��ʱ  0 -- û�г�ʱ 

	




	//////  �����ǹ���GPS�Ż������� START ////////
	int m_F_G_C; //����ͳ����û����ѯ��һ��  ������� ��һ�� 1000004  m_F_G_C == 6 ��˵����ѯ��һ��  �����ȡ����  ������ �� 0
	int m_F_M; //���������ʲôģʽ��   ���� 0 ��Ϊ����ѯ�������  ���Ϊ 1 ����ƽ̨�·���ģʽ�� 
	uint8 m_F_P_V; //������Ƕ�λ����  AEEGPS_ACCURACY_LEVEL1 -- 1  �� AEEGPS_ACCURACY_LEVEL6 -- 6 ��1-6����ȼ����ȣ�
	boolean m_F_Pos;  // ������ǵȼ��Ƿ�Ҫ�� �������֮�� �ֱ�ɲ����ӵȼ� TRUE == ��ʾ���ӵȼ�  FLASE == ��ʾ�����ӵȼ�
	boolean m_F_P_F ;// ��������ǲ���track_stop ���һ����track_start  TRUE == ��ʾ  ��  FLASE = ��ʾ����
	boolean m_Q_P; //��������Ƿ��ǳ�����ʱ���ٶ�λ  TRUE -- ��  FLASE -- ����
	int m_POS_M;// ��ȡ��ǰ�ֻ��ܶ�λ����ģʽ
	int m_NOMAINPOS; //��ʾ�Ƿ���Ҫ����ѯģʽ�¶�λ����ʱ���л�����ģʽ  == 0 ����Ҫ  == 1 ����Ҫ  == 0 ��ʱ����һ���÷� ����ƽ̨�·���λģʽ��ʱ���л����µ�ƽ̨��λģʽ
	boolean m_PosMode ;// ��������Ƿ���ֻ���ô����Ƕ�λ TRUE -- ��ʾֻ���ô�����  FLASH -- ���� �����Ա�ķ�ʽ
	boolean m_F_START; //��������ǲ��ǲŷ���Track_Start   TRUE --  ��ʾ�շ���      FALSE -- ���ǲ��Ǹշ���  
	///////          END        //////////////////


	//////  �����ǹ��� ����״̬�Թ���Ҫ��Щ����� ���� START //////////////
	boolean m_F_NONET; //���������û������  TRUE -- ������ʱ��û������  FALSE -- ������
	boolean m_has_QQSQ; //��������Ƿ�������Ȩ��  TRUE == ��ʾ������Ȩ�� FALSE == ��ʾ��û��������Ȩ
	boolean m_Rely_XT; //��������Ƿ�������Ҫ��������  TRUE = Ҫ��������  FALSE = ��ʾ�������Ҫ���ͣ�ֻ�ǽ�����ʱ
	boolean m_F_QF;  // ����ֻ�Ƿ��  TRUE - �ֻ�Ƿ��  FALSE - �ֻ�û��Ƿ�� 
	//////////////////////////////       END      /////////////////////////

	uint32 m_POS_T; //������Ƕ�λ����
	uint32 m_Fix_C; //����ͳ�ƻ�ȡ�����ܶ�λ��  ���ڻ�ȡ����һ�鷢�����ݺ������ϴ�


	///////////////  �����õ� ����HTTP �����ʱ��
	int m_MUTEX; // 1 = ˵�����ܽ��з���HTTP �Ѿ���ռ����http����Դ  0= ˵������ʹ��http��Դ  
				 // (������������ == 1 ˵��������ռ����Դ == 2˵���Ƿ���GPS����ռ����Դ  ��������Ŀ�����Լ�����)



	uint32 m_FAIL_T; //������Ž�����ȡGPS�쳣�����


	boolean m_E_1289 ; // TRUE - ��ʾ���� 1289 FALSE - ��ʾû�г���1289

	char m_ConText[256];//  �����洢��Ƭ����༭���еı༭����
	int m_ConT_FH;//������Ǵ�ʱ�ǲ����ڷ��ŵ�״̬��  1-��  0-����

	uint32 m_T_H_G; //���ʱ���Ǽ�¼ �ϱ�HTTP ���� ����GPS��λ��ʱ�� ���ǵĲ�ֵҪ�ǲ��������� ���������GPS ��ʱ���� �����HTTP������˴�����

	int m_T_E_C; // ����ͳ���������� ��ȡʧ�ܼ��� ���������������ͣ��ȡ����ͣʱ��Ϊ200�룬Ȼ�������ȡ ����������������ϴ����������
	int m_W_E; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�

	boolean mb_MsgLog ;// ��������Ƿ��Ƕ��ŷ�����־  TRUE -- ��   FALSE - ����

	// �����ص�
	int m_F_R_C;// ����Ƿ��ȡHTTP������Ȩ��ʱ������ȴ�  ISOURCE_WAIT  == 1  ��  == 0 ����
	char * m_HTTP_C;//HTTP�ص����ص����ݳ��ֵȴ���ʱ�� ��ʱ��������  ��̬����ռ�  �ռ��С����Ϊ 102400

	// GPS�ص�
	int m_F_R_GPS;// ����Ƿ��ȡHTTP������Ȩ��ʱ������ȴ�  ISOURCE_WAIT  == 1  ��  == 0 ����  
	char * m_H_GPS;//HTTP�ص����ص����ݳ��ֵȴ���ʱ�� ��ʱ��������  ��̬����ռ�  �ռ��С����Ϊ 102400

	// ������Ȩ�ص�
	int m_F_R_SQ;// ����Ƿ��ȡHTTP������Ȩ��ʱ������ȴ�  ISOURCE_WAIT  == 1  ��  == 0 ����  
	char * m_H_SQ;//HTTP�ص����ص����ݳ��ֵȴ���ʱ�� ��ʱ��������  ��̬����ռ�  �ռ��С����Ϊ 102400

	// ���ڻص�
	int m_F_R_LX;// ����Ƿ��ȡHTTP������Ȩ��ʱ������ȴ�  ISOURCE_WAIT  == 1  ��  == 0 ����  
	char * m_H_LX;//HTTP�ص����ص����ݳ��ֵȴ���ʱ�� ��ʱ��������  ��̬����ռ�  �ռ��С����Ϊ 102400

	// �����ص�  �����쳣���ݻص�
	int m_F_R_E;// ����Ƿ��ȡHTTP������Ȩ��ʱ������ȴ�  ISOURCE_WAIT  == 1  ��  == 0 ����
	char * m_HTTP_E;//HTTP�ص����ص����ݳ��ֵȴ���ʱ�� ��ʱ��������  ��̬����ռ�  �ռ��С����Ϊ 102400

	//////////  �����ǹ��� ��˼��������뷨�õ��ı���  start  /////////
	int m_ys_code ; //���������˼����� ecode ����ֵ  // ��������ֵ��519 ��˵�����ڱ�д����  ������� 272 ���� 

	//////////          END      //////////////////////////////////////



	////////////////  2015-10-26 START �������й���ͨѶ¼�ṹ��
	void	*pRichAddr;								// �й�ͨѶ¼�Ľṹ
	void *pRichptt;								// QCHATӦ�õ��Ľṹ
	char *sc_sub; // ���������ͨѶ¼���棬�ж�����ֻ���û����ͨѶ¼����
	//////////////////    END    ///////�й�ͨѶ¼�ṹ

	uint32	m_GPS_E_T ;  // ������ŷ���GPS error data  ��������Ŀ���Ƿ�ֹ �л�GPSģʽ��ʱ�����Ͼͻ� 100004  ���ϱ�ƽ̨ ���������̾Ͳ��ϱ�

	int m_F_DJ; //��������ֻ��Ƿ������ڶԽ�״̬  ͨ���Խ��¼� evtCode=28930 �����Խ�  evtCode=28929 �Խ����� ��һ���궨���ǶԽ��Ƿ���Ҫ����
	// = 1�����ڶԽ� ���ֵ���Ǵ��ڶԽ�״̬ 

	int m_temp_FOBFlag;//��ֹ�ٴν���ǰ̨�ĳ�ʼ����ǰ̨�ĳ�ʼ��ֻ��һ�Ρ�

	///////  �����������йص� ///////////
	void *pTaskApp;								// �������й�
	void        *pScreen_Task; // ����������б�����һҳ ��һҳ�Ŀؼ�
	int m_TaskC; //���� һ��������ʾ���ٵĵ���������
	int m_TaskID ;// ��������ڵ�ǰҳ���µĵ�ǰ��Ŀ¼�ڵڼ���
	int m_Task_F ;// task_first ��������ǲ��ǳ����������һ�ν�����������ǵĻ���������ǰ������û��������ǰ��
	int m_TTotal ;//��ǰ�����������
	int m_Page_c ;//��ǰ������Ҫ����ҳ��
	int m_Cur_Page;// ��ǰ��ʾ�ڼ�ҳ
	boolean m_bDoTask;  //���������ѣ�������̿�ʼ������1  ��ʼ��0
	char m_IP[256]; // ��Ӧ��IP ��ַ ������ ip �˿� ��������
	char m_Cur_TID[100]; //��ʱ���TaskID����

	uint32  m_Cur_nDateTime;	//  [3/22/2016 Administrator]��ʱ���TASK_DATATIME

	ITextCtl		*textCtl_T;											// ��������ص��ı���

	///////////////////////GPS�������ݰ�/////////////////////////
	//##����
	boolean m_bMiss;   //�ڵ�����ʾ����"����"�Ľ���ʱ��  ���Ը��ְ���   1-���ΰ���  2-������

	boolean m_offlineData_gps;  //1-����������  0-ô��
	boolean m_Mutex_offgps; //�������ߵĻ��� Http��û�б�ռ�� 1-ռ��   0-����ռ��
	boolean m_bFlg_errG;    //�Ƿ����ڵȴ�����errGPSdata    1-��; 0��

	int m_KJZQ_C ;// ͳ�ƿ�����������Ĵ���

	int m_F_CD; //����Ƿ�Ϊ��� == 1 ��  == 0 ����

	int		m_E_SC;				// ���ڴ���������ٴ�1289֮�󲥷�һ������
					
	//////////////////////////////////////////////////////////////////////////

	// ����������������ڲ���¼����
	int m_W_E_P; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�


	ISoundPlayer * pISoundPlayer;

	int m_SQ_again; //���������һ����Ȩ ����Ҫ�ǿ��ܴ�cnƽ̨������һ��ƽ̨  = 1 ��һ����Ȩ= 0 ����

    //  [2/4/2016 Administrator] ##������й�
	void *pSms;	
	
	//###����###2015-12-28//
	boolean m_Task_fs;     //���񣬰��·��������� ���� ���� "���" ʱ��1, 
	boolean m_Task_fh;		//�������뷨����*�� ��1; 
	IStatic	 *m_pIStatic_Task;

	//����������
	int m_iTask_fkID;  //Ҫ������ �������ǵڼ�����
	int m_iText_Input;	    //1-������������༭��   2-������Ƭ�༭��(��ʱû����);

	//��gps�����ļ��ǿյ�ʱ��Ž����µ��������� m_RofflineData_Num = m_WofflineData_Num
	int m_WofflineData_Num; //gps��������������� ר������д�������ļ�  [3/9/2016 Administrator]
	
	int m_RofflineData_Num;//gps���������������  ר�����ڴ������ļ��������ͳ�������ʹ��[3/9/2016 Administrator]

	//�������qchat�ĳ�ʼ��ʧ�ܣ������ظ���ʼ���Ĵ��������ӵ�ԭ��ĵ�ʱ��ʼ��ʧ�����¿�ʼҲ�����Ȩ�������ظ���Ȩ����
	int m_WaitForQChat_Num;//    WaitForQChatʧ��3�κ�Ͳ���WaitForQChat��[3/14/2016 Administrator]

    char m_Right_Key;	//���ֻ����ҹ��ܼ��ڶ����˵���ֱ������Ӱ��ǰ��̨��־λ��������������ͣ����start��

	//  [5/9/2016 Administrator] ���������ӿ� 
	ITTSEXT *pIgpsext;
    char m_Con[30];

	//¼��
	void *precord; //
	UploadRecord *rData ;
	int m_flag_Audio;//��ֹ¼���ϴ��� ��ʱ��ػ�û�����꣬�ֿ�ʼ�µ�һ�η��͡��µ�һ�η��ͻ��ͷſռ䵼�³�ʱ��ش���
	int m_enable_Audio;//����¼���ϴ���־λ

	char  szinfotxtPath[128];
	char  szinfotxtPath_2[128];
	
	//7��GPS��������
	char file_Name_7Day[128] ;
	IWeb *m_pIWeb_7Day; //����������
	IWebResp *     piWResp_7Day;       // the answer I get from IWeb
	ISource*       pSource_7Day;
	ISourceUtil* pisu_7Day;
	char  *pszPostData_7Day;  // POST string
	IPeek          *pipPostData_7Day;  // POST stream, if any
	AEECallback    cb_7Day;            // how to call me back
	int PostOvertime_7Day;
	uint32 fileRead_7Day;
	char  pURL_7Day[200];
	uint8 nState_7Day;

	//char del_file_Name[7][64];
	char (*pfile_Name)[64];
	//������˼�����������
	/*uint8 */

	//�ϴ��������ݺ�д�������ݵĻ���ӿ�
	int mux_7Day;
	
	//s958GPS�ӿ�
	ITTSEXTGPS *pS958gps;

	//�������ֵ
	uint8 m_min_Accuracy;

	//�Ƿ�Ҫɾ���ϴ��õ�¼���ı�־λ
	uint8 m_del_Audio;
	
	//unsigned short us_CRC16_result;
	//MD5У��
	char s_MD5[33]; 
	uint8 m_enable_MD5;//�Ƿ�У�顣

	//evdo���źţ��ſ�ʼ��ʼ����������
	uint8 m_enable_evdo;

	//---------------start
	//��Ƶ�·�ģ��
	//��Ȩʱ��õ�HTTP����IP��ַ��UDPIP��port
	char sHttp_YYXF[64]; //HTTP��ַIP
	char sUDPIP_YYXF[32];
	uint16 iUDPPORT_YYXF;
	char sUrl_YYXF[256];

	char sTCPIP_YYXF[32];
	uint16 iTCPPORT_YYXF;

	//socket
	//tcp
	int TCP_Status_yyxf;	//TCP�Ƿ�����������������ʾ���ֻ�����
	INetMgr *pINetMgr_TCP_yyxf;
	ISocket *pISocket_TCP_yyxf;
	char m_readMsg_tcp[5120];	//��ȡrtp
	int	 m_nRecvSize;	//�ۼ�δ������ֽ���
	char* m_DataBuf;	//����������ݰ�
	int m_Tcphead_flag;	//�Ƿ��յ������� ���ݴ��ж��Ƿ�Ҫ�ظ�ƽ̨��
	int m_Listen_Heart;//���������Ƿ��յ������û���յ�����3����������
	//udp
	INetMgr *pINetMgr_UDP;
	ISocket *pISocket_UDP;
	char m_readMsg[10240];//UDP����
	pList list_head_UDP;	//udp���洢����
	int iflag_YYXF;//ֻ����һ��ͷ��
	int iCount_YYXF;	//�����յ�����UDP��

	int overTime_YYXF;	//���ͳ�ʱ
	int readUdpTime_YYXF;//���ճ�ʱ

	//�����·�HTTP
	IWeb *m_pIWeb_yyxf; //����������
	char                 *pszPostData_yyxf;  // POST string
	IPeek                *pipPostData_yyxf;  // POST stream, if any
	AEECallback          cb_yyxf;            // how to call me back
	uint32               uStart_yyxf;
	IWebResp *           piWResp_yyxf;       // the answer I get from IWeb
	ISource*   pSource_yyxf;
	byte *		m_BodyBuffer_yyxf;		// Buffer to hold HTML data
	int			m_BodySize_yyxf;			// Size of HTML data in buffer
	int			m_BodyAllocSize_yyxf;	// Allocated size of HTML data buffer
	int iPostPhTm_yyxf; //

	//�����·����кͿ���
	QUEUE pQueue;
	YYXF_INFO t_httpInfo;//���http���ܵ���Ϣ������UDP����
	int m_audioCount;	//�� �·���Ƶ�貥�ż���	//���������ĿǰĬ��Ϊ1
	int gi_yyxfCount;//ȡ �·���Ƶ�貥�ż���	//���������ĿǰĬ��Ϊ1
	char m_audioName[128];	//���� ��Ƶ����
	char gs_yyxfAudioName[128];//ȡ�� ��Ƶ����
	char gs_yyxfPlayName[128];//���ŵ���ʱ����
	//����������
	ISoundPlayer *pIPlayer;//UPD���պ�İ�����

	//����д��
	ISMS			  *pISMS_yyxf;
	ISMSMsg			  *pISmsMsg_yyxf;
	ISMSStorage       *pISmsStorage_yyxf;
	AEECallback		  cbMsg_yyxf;
	
	AECHAR		      *aeContent_yyxf;
	//---------------end

	int m_evdo_Num;//  ��ʼΪ0,�ۼƵ�8�Σ�һ��5�롣40���Ҳ�ܹ����ź�m_enable_evdo������1
	
} newgimis;


typedef struct
{
	AEEApplet    a;
	char		 pURL[128];
	IFileMgr*    pFileMgr;
	IFile*		 pFileUpload;				// ���ϴ����ļ����
    FileInfo	 oFileInfo;					// �ļ���Ϣ
	char         sFile[MAX_FILE_NAME + 1];	// �ļ�������·����
	
	IWeb*        pWeb;
	AEECallback  webcb;
	IWebResp *   piWResp;		// the answer I get from IWeb
	ISource*     pSource;		// the body of the response		�����Ҳ��	IGetLine *   piGetLine;				// the body of the response, if any
	ISourceUtil* pisu;
	char         sBodyBuf[10];				// ֻҪӦ��ĵ�0���ַ�
	int          nBytes;					// count of bytes in response
	
	newgimis	*m_pParent;
	
	uint8		cSelsts;		//0 ��û���ļ���ѡ�з���
	//1�� ���ļ���ѡ�з��ͣ���ѡ�н�����
	
	char         cSendSts;		// ���͵�״̬
	//	'1':  ���ν��ճɹ�������������һ�Σ����ֻ��˻����������һ�Σ���������һ�β��ҷ��͵�����Ƶ�ļ������ֻ��˻�ɾ����Ƶ�ļ�����ʾ�ֻ��û������ͳɹ�����
	//	'0':  ���ν���ʧ�ܣ����ֻ��˻��ط�����
	//	'A': ��ʾ���ճ����쳣���ֻ�����ʾ������ʧ�ܡ���Ϣ���ֻ��û���������͵�����Ƶ�ļ����򲻻�ɾ����Ƶ�ļ�
	uint8		nPartNum;		// ���ڴ��ڼ��Σ�1 ��1�Σ�2 ��2�Σ�����������  
	uint8	    nPartsTotal;	// �ܶ���
	uint16 		nSelRec;		// ѡ�е������
	
	int32		iFileRead;					// �����ļ����ֽ���  
	uint32      nHasRead;		// �Ѿ��ϴ���ȡ���ֽ���
	
	/*	pList ptemp;//���ڱ��汻ѡ�е����ݡ�*/
	uint8		 nState;					// ״̬
	// 0(SUCCESS)    �ɹ�
	// 1(EFAILED)    ʧ�� 
	IPeek        *pipPostData;				// POST stream, 
	char         *pszPostData;				// POST string
	uint32		 dwPostOvertime;		// ��¼�����ļ������Ӧ���ʱ�䣬��λ�����ʱ�����涨ʱ�䣬�ر����ӣ�����ʾʧ��
	
	AECHAR		pText[999];		// ���ϴ��ļ��йص�����,���100��˫�ֽ��ַ�

	
	
}Record_information;


uint32 gimis_TEST_MOD_FILE( newgimis *pMe ); //���MOD�ļ���û�д���

uint32 gimis_Init_Pram_Open( newgimis *pMe ); //��ȡ����txt�ı�־λ �������Ҫ��Ҫ�Զ�����


uint32 gimis_ReadPowerBoot( newgimis *pMe, IFile * pIFile );
uint32 gimis_WritePowerBoot( newgimis *pMe, IFile * pIFile );
// ���� �����Ƿ�������
uint32 gimis_SetPram_Open(newgimis *pMe);

// ���� ����������Ȩ�ɹ� �����Ƿ�������
uint32 gimis_SetPram_Open_QQSQ_CG(newgimis *pMe);

uint32 gimis_InitGPSSettings( newgimis *pMe );
uint32 gimis_ReadGPSSettings( newgimis *pMe, IFile * pIFile );
uint32 gimis_WriteGPSSettings( newgimis *pMe, IFile * pIFile );

uint32 gimis_WriteHTEL(newgimis *pMe);

// �������������ļ�
uint32 gimis_SetGPSPara(newgimis *pMe);

// ��ȡ���������ļ�
uint32 gimis_GetGPSPara(newgimis *pMe);

// �ж���û����������
uint32 gimis_judgedata(newgimis *pMe);


/*-------------------------------------------------------------------
�����봦�����й�
-------------------------------------------------------------------*/
// Add by zjie 2012-09-21 �ж��Ƿ�������Ƭ(�����ļ�)�ȹ���
boolean uf_HasFun(newgimis *pMe, int nMask);


/*-------------------------------------------------------------------
�����������й�  Add by zjie 2011-11-18
-------------------------------------------------------------------*/
boolean uf_CheckNetSts(newgimis * pMe);			// ��������Ƿ����
void uf_GetNetSts(newgimis * pMe);				// ��ȡ����״̬
void uf_GetNetSts_imsi(newgimis * pMe);				// ��ȡ����״̬


// �����������״̬תΪ����״̬ʱ����������״̬
void uf_SetNetSts(newgimis *pMe);


// �ڱ��ؼ�¼����(������)
void uf_RecLog(newgimis * pMe,char *vpStr_msg);


//////  ������GPS�йص� ��ʼ ///////
extern void *testgps_GetScreenData( newgimis *pMe );


extern void testgps_DrawScreen( newgimis *pMe, uint32 dwParam );
extern void testgps_Printf( newgimis *pMe, int nLine, int nCol, AEEFont fnt, 
                                uint32 dwFlags, const char *szFormat, ... );

extern void testgps_Printf_hanzi( newgimis *pMe, int nLine, int nCol, AEEFont fnt, 
                                uint32 dwFlags, AECHAR *szFormat, ... ); // ��ʾ����

extern void *testgps_SetScreenData( newgimis *pMe, void *screenData );
extern boolean testgps_GotoScreen( newgimis *pMe, ScreenId id, uint32 dwParam );
extern boolean testgps_GotoScreentogps( newgimis *pMe, ScreenId id, uint32 dwParam );
uint32 testgps_InitGPSSettings( newgimis *pMe );
uint32 testgps_ReadGPSSettings( newgimis *pMe, IFile * pIFile );
uint32 testgps_WriteGPSSettings( newgimis *pMe, IFile * pIFile );
extern boolean testgps_MainMenu_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );

extern boolean testgps_GetGPSInfo_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );
extern boolean testgps_ShowGetGPSInfo_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );

// SamplePosDet_More_HandleEvent
extern boolean SamplePosDet_More_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );

// SamplePosDet_SendPhoto_HandleEvent ������Ƭ
extern boolean SamplePosDet_SendPhoto_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );

// SamplePosDet_SendAudio_HandleEvent ������Ƶ
extern boolean SamplePosDet_SendAudio_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );


// SamplePosDet_More_HandleEvent
extern boolean SamplePosDet_Tasklist_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );

extern boolean SamplePosDet_Detail1_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );

extern boolean SamplePosDet_Detail2_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );

extern boolean SamplePosDet_Content_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );

///////////////////  ������ӵĹ��� ��ȡ��Ļ������ gps��ʾ��ҳ�� /////////
extern void *testgps_GetScreenData_gps( newgimis *pMe );
extern void *testgps_SetScreenData_gps( newgimis *pMe, void *screenData );
//////////////////////////////////////////////////////////////////////////
// 
// ����Ƶ�й� ///
extern void *testgps_GetScreenData_Audio( newgimis *pMe );
extern void *testgps_SetScreenData_Audio( newgimis *pMe, void *screenData );
/////////////////
///////  ��GPS�йصĽ���  //////////



// ����������Ȩ�ķ��ص�ֵ��д�������ļ�
// ����ֵ˵�� ������� 1 - ��ɹ�����������Ȩ  0 - �򷵻ص��Ǵ�����Ϣ
int ParseStringandWriteToConfigure( newgimis *pMe,char *pString); // ���������������Ȩ���ص�ֵ

int ParseStringandWriteToConfigure_xintiao_GPS( newgimis *pMe,char *pString); // ���������������Ȩ���ص�ֵ

int ParseStringandWriteToConfigure_xintiao_GPS_FAIL( newgimis *pMe,char *pString); // ���������������Ȩ���ص�ֵ


// ����GPS����
boolean SendGPSData( newgimis *pMe );

// ���ʹ����GPS 0x100001
boolean SendErrorGPSData_0( newgimis *pMe );

// ���ʹ����GPS 0x100004
boolean SendErrorGPSData( newgimis *pMe );


// ��ʾ�ֻ�״̬
void uf_PhoneSts(newgimis *pMe);


// ��һ�������й�
void newgimis_forYJKQ(newgimis * pMe,boolean bGet);

// �뷢�Ϳճ��й�
void newgimis_SendEmpty(newgimis * pMe,boolean bGet);

// �뷢���س��й�
void newgimis_SendFull(newgimis * pMe,boolean bGet);

// �뷢��״̬0�й�
void newgimis_Send_ZT_0(newgimis * pMe,boolean bGet);

// �뷢��״̬ 1 �й�
void newgimis_Send_ZT_1(newgimis * pMe,boolean bGet);

//��һ���򿨣�����ת���� �й�
void newgimis_forYJDK(newgimis * pMe,boolean bGet);

// ��ʾ�ճ�����ʧ��
void uf_AEPrint_EMPTY_FAIL_Back(newgimis * pMe);

// ��ʾ�س�����ʧ��
void uf_AEPrint_FULL_FAIL_Back(newgimis * pMe);

// ��ʾ״̬1����ʧ��
void uf_AEPrint_ZT_1_FAIL_Back(newgimis * pMe);

// ��ʾ״̬0����ʧ��
void uf_AEPrint_ZT_0_FAIL_Back(newgimis * pMe);

//��ʾ��ʧ��
void uf_AEPrint_YJDK_FAIL_Back(newgimis * pMe);

// ��ʾ״̬1���ͳɹ�
void uf_AEPrint_ZT_1_SUCC_Back(newgimis * pMe);

// ��ʾ״̬0���ͳɹ�
void uf_AEPrint_ZT_0_SUCC_Back(newgimis * pMe);

// ��ʾ�ճ����ͳɹ�
void uf_AEPrint_EMPTY_SUCC_Back(newgimis * pMe);

// ��ʾ�س����ͳɹ�
void uf_AEPrint_FULL_SUCC_Back(newgimis * pMe);

//��ʾ�򿨳ɹ�
void uf_AEPrint_YJDK_SUCC_Back(newgimis * pMe);

/*-------------------------------------------------------------------
��������Ƭ�й�
-------------------------------------------------------------------*/
void newgimis_forphoto(newgimis * pMe,boolean bGet);
void newgimis_GetPhote(newgimis * pMe);


// ��������Ƶ�й�
void newgimis_forAudio(newgimis * pMe,boolean bGet);


void uf_forphoto(newgimis * pMe,boolean bGet);
// ����Ƭ��ʾ����Ļ�ϣ����û�ȷ��
void uf_SetPhote(newgimis * pMe);
// �ҵ��������͵�������Ƭ, �ҵ������pMe->imgidx��, ���������������Ƭ����������
void uf_GetNewestPhotoForSend(newgimis * pMe);   //uf_GetPhotoForSendImmediately
// ��ʾ�ı����ѡ����Ƭ�˵�
void uf_ShowText(newgimis * pMe);
// �ͷ����ı������йصĿؼ�
void uf_ReleaseCtl(newgimis * pMe,boolean bClean);


void uf_GetDate(newgimis * pMe,uint32 dwCreationDate,char* strDateTime,int iStrLen,char cType);

// ��������ģʽ������ʾ����Ļ��
boolean uf_TextModechange(newgimis *pMe);
// ��ʾ��Ļ����ʾ��ǰ���뷨
void uf_ShowInputMode(newgimis * pMe,AEETextInputMode eInputMode);

// ֹͣ����  Add by zjie 2011-10-26
void uf_StopBeep(newgimis *pMe);



// �ڷ�����Ƭʱ������Ļ��ʾͼƬ�������С�����
void uf_DrawSendPic(newgimis *pMe,IImage *pIImage, AEEImageInfo * pi, int nErr );

// �����ļ�
void uf_CopyFile(newgimis * pMe,IFileMgr* pFileMgr,char *pStr_SrcName,char *pStr_DstName);

// ��ʾ����
void uf_AEPrint(newgimis * pMe,boolean bCallBack);

// ��ʾ����û����ƵҪ��ʾ
void uf_AEPrint_Back(newgimis * pMe,boolean bCallBack);

// ��ʾ����û����ƬҪ��ʾ
void uf_AEPrintPhoto_Back(newgimis * pMe,boolean bCallBack);
// ��Ƭ����δ����
void uf_AEPrint_Photo_Back(newgimis * pMe,boolean bCallBack);

// ��Ƭ�����������������
void uf_Afterpost(newgimis *pMe);


//��̨תǰ̨�Ķ��� Add by zjie 2011-11-18
boolean uf_ForgeGroundEvt(newgimis *pMe);		// void 

// ����Ļ����ʾ˫�ֽ���Ϣ
void gimis_WPrintf( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *szText);


// Add by zjie 2012-09-21, ��gm_photo.c�����˴� 
//   �ڷ����ļ�ʱ������Ļ��ʾͼƬ�������С�����Add by zjie 2012-05-29
void uf_ShowSendPic(newgimis *pMe);

// uf_ReturnMenu �������˵� �ص�����
void uf_ReturnMenu(newgimis * pMe);

// uf_ReturnMenu ���ظ���˵� �ص�����
void uf_ReturnMenu_Two(newgimis * pMe);


// ��һ�������йص�
// ��ʾ����û��GPS����
void uf_AEPrint_YJKQ_Back(newgimis * pMe,boolean bCallBack);

// ��ʾ����û��GPS����  �ڶ�������
void uf_AEPrint_YJKQ_Back_Two(newgimis * pMe,boolean bCallBack);

// ��ʾ����û��GPS����  �ڶ�������
void uf_AEPrint_RWLB_Back_Two(newgimis * pMe,boolean bCallBack);

// ��ʾ����û��GPS����  ��һ������
void uf_AEPrint_YJDK_Back(newgimis * pMe,boolean bCallBack);

// ��ʾû�иù��� 
void without_this_feature(newgimis * pMe,boolean bCallBack);


// ��ʾ������
void uf_AEPrint_Sending(newgimis * pMe,boolean bCallBack);

//��ʾ��ȡ������
void uf_AEPrint_xq(newgimis * pMe,boolean bCallBack);

// ��ʾ����������
void uf_AEPrint_NO_NET_Back(newgimis * pMe,boolean bCallBack);

// ��ʾ����������  ���ظ���Ľ���
void uf_AEPrint_NO_NET_Back_Two(newgimis * pMe,boolean bCallBack);

// ����Ƶ�й�
// ��Ƭ����δ����
void uf_AEPrint_Audio_Back(newgimis * pMe,boolean bCallBack);
void newgimis_GetAudio(newgimis * pMe);



//���� / �ر�GPS��λ
void newgimis_OpenGPS(newgimis * pMe);
void newgimis_CloseGPS(newgimis * pMe);

// ��ʱ ���� �� �ر� GPS��λ
void newgimis_OpenGPS_tmp(newgimis * pMe);
void newgimis_CloseGPS_tmp(newgimis * pMe);


// ���ַ�����ԭ�ַ�ת��Ϊת���ַ�
void uf_toESC(char *rStr_source,char *rStr_Esc);
// ���ַ�����ת���ַ�ת��Ϊԭ�ַ�
// ����˵�� ��һ�������ǻ�û��ת����  �ڶ���������ת�����
void uf_fromESC(char *rStr_Esc,char *rStr_source);

// ���ַ������滻���ַ���
void uf_trfESC(char *rStr_source,char **rStr_Esc,const char *vStr_sESC, const char *vStr_tESC);


// �������ڻ���ʱ���õĽ���  ���µķ���ֵ�� 1 -- ����ɹ�����  0 -- ��û���ҵ��ؼ�ָ�� û�л���
// ��ʾ������
int Show_Main_Fun(newgimis *pnewgimis);
// ��ʾ�鿴GPS���ý���
int Show_GPS_Fun(newgimis *pnewgimis);
// ��ʾ��Ƭ����
int Show_Photo_Fun(newgimis *pnewgimis);
// ��ʾ��Ƶ����
int Show_Audio_Fun(newgimis *pnewgimis);

// ��ʾ��Ƶ �������
int Show_Audio_Fun_Sup(newgimis *pnewgimis);

// ��ʾ������Ǹ�����
int Show_MorePag_Fun(newgimis *pnewgimis);

//��ʾ��̨�������ѽ���
int show_Notify(newgimis *pnewgimis);

//��ʾ�����б����
int Show_Task_List(newgimis *pnewgimis);

//��ʾ���������������
int Show_Task_Detail1(newgimis *pnewgimis);

//��ʾ�������ݽ���
int Show_Task_Content(newgimis *pnewgimis);

//��ʾ����������������
int Show_Task_Detail2(newgimis *pnewgimis);

// ����ϼ��˳�����
boolean newgimis_ClosseApplet( newgimis *pMe );

// �ر�GPS���˳�����
void newgimis_ufStopApp( newgimis *pMe );


void Save_GPSData(newgimis *pMe);

void Save_GPSData_had(newgimis *pMe);  //���Ѿ����������ݵ�����¼���������������

void Save_GPSData_All(newgimis *pMe);



//��post��ʽ������������
void newgimis_PostOffData(newgimis * pMe);

int Get_LX_GPSData(newgimis *pMe);  //��ȡ�������� �ɹ� ���� > 0  ʧ�ܷ��� 0

// ɾ�����߿����ļ�
int uf_DelOff_File(newgimis *pMe);

//   �����ã�ɾ�������ļ�
int uf_Del_ConfigFile(newgimis *pMe);

// ����ע��İ���	Add by zjie 2011-11-18
static boolean gimis_NotifyShellKey(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);


// ���HTTP�Ƿ�ʱ
void uf_MonitorPost(newgimis *pMe);	 // ������Ȩ��ʱ������������ ˵�� ���ܺ�ƽ̨���� ���е����ݶ������ߵ� ��λҲֻ�������Ƕ�λ

// ���HTTP�Ƿ�ʱ
void uf_MonitorPost_xt(newgimis *pMe);	 // ������Ȩ��ʱ������������ ˵�� ���ܺ�ƽ̨���� ���е����ݶ������ߵ� ��λҲֻ�������Ƕ�λ

// ���HTTP�Ƿ�ʱ
void uf_MonitorPost_E(newgimis *pMe);	 // ������Ȩ��ʱ������������ ˵�� ���ܺ�ƽ̨���� ���е����ݶ������ߵ� ��λҲֻ�������Ƕ�λ


// ���HTTP�Ƿ�ʱ һ������ �� ״̬
void uf_MonitorPost_kq_zt(newgimis *pMe);	 // ������Ȩ��ʱ������������ ˵�� ���ܺ�ƽ̨���� ���е����ݶ������ߵ� ��λҲֻ�������Ƕ�λ



// ���HTTP�Ƿ�ʱ
void uf_MonitorPost_gps(newgimis *pMe);	 // ������Ȩ��ʱ������������ ˵�� ���ܺ�ƽ̨���� ���е����ݶ������ߵ� ��λҲֻ�������Ƕ�λ


// �ж���û����־���� 
/*
	����ֵ 0 -- ��ʾ������ļ�������������  > 0 ˵�����ش���
*/
uint32 juage_logfile( newgimis *pMe ); //�ж���û����־����


//  ��ȡ���͵�ģʽ
void Send_PM(newgimis *pMe, int pi);


/////////////  �� GPS��λ �Ż��йص�  START             //////////////
// uf_Pos_Track_stopstart  GPS��λֹͣ��ʼ  ��������ڳ�ʱ��ʱ��
void uf_Pos_Track_stopstart(newgimis * pMe);  

// ����������Ѿ���λ��Ҫ�л�����ģʽ��
void uf_Pos_Track_stopstart_has(newgimis * pMe);  

// �ڽ��յ� 100004 ��ʱ�� ���¹ر����� Ȼ���ٿ�������
void uf_Pos_Track_stopstart_error(newgimis * pMe);  

// ����ģʽ�� ���յ� 100004 ��ʱ�� ��ʱ 10���Ӻ��ٴγ�����ģʽ�ܷ�λ��
void uf_Pos_Track_stopstart_time(newgimis * pMe);  
//////////////////                    END               //////////////



// 2���Ӻ�����GPS
 void GPS_Restart_fun(newgimis *pMe);

//void GPS_Restart_fun(void *pMe);

// ����ͼƬ����
void Load_Image_fun(newgimis *pMe);	 // 



// Track_cbInterval_copy  GPS��λֹͣ��ʼ  ��������ڳ�ʱ��ʱ��
void Track_cbInterval_copy(void * pMe);  

void Track_Notify_Copy(void *pts);

// ��ʱ������������GPS ��λ
void TimetoTrack_cbInterval(void *pts);

// �ж��Ƿ��� myiweb_start_xt ����ȡ������   1 - ��ʾ����   0 - ��ʾ������
int judgeSendorCancel(newgimis *pMe); 

void judge_reset_TB(newgimis *pMe); // �ж��Ƿ���Ҫ���� ��������ʱ��


/********************************************************************************************
 �ڱ��ؼ�¼��־
�������
  cgimis * pMe
  IShell *pShell  :  
  cWrtLog         :   1  д����־�� ��1 ����qxdm����
  int *pIndex		  :   ����������ʾ��־�г���ʼִ�е�λ�ã��ɵ��õĳ��򱣴��ֵ
  const char *szFormat, ...  �� ���¼���ַ���
********************************************************************************************/
void ud_RecLog_c(IShell *pShell,char cWrtLog, uint32 *pIndex, const char *szFormat, ... );



void IPOSDET_Release_fun(newgimis *pMe);

// ���Ͷ��������ֻ�ָ��
void uf_RestartPhone(newgimis *pMe);

// �ж�GPSmode ʧ���� ����վ ������ ������ ��վ���� 
//  ����ֵ ��� == TRUE ��    == FALSE  ���� 
int Judge_GPS_Mode(newgimis *pMe);

//  ��ȡGPS��Ӧ��ģʽ
int Get_GPS_Mode(newgimis *pMe);


//////////  һ����ͨѶ¼��     /////////
// ���ڼ�����Ķ�ָ��ĺ���
char *uf_STRSTR(const char *pszHaystack, const char *pszNeedle, const char *sSplit );

int addbook_init(newgimis *pMe);
int addbook_free(newgimis *pMe);
int Add_addbook(newgimis *pMe);
int Del_addbook(newgimis *pMe);
int Del_all_addbook(newgimis *pMe);


//////////   ������QCHAT �Խ���صĺ���  ////////////////
void newgimis_WaitForQChat(newgimis *pMe);

void call_richptt_PttCall(newgimis *pMe);


//////  �����������йص� ��ʼ ///////
extern void *testgps_GetScreenData_task( newgimis *pMe );
extern void *testgps_SetScreenData_task( newgimis *pMe, void *screenData );

//��������GPS���� //2015-12-18 �޸�
boolean Send_Offline_GPSData( newgimis *pMe);
void myiweb_start_E_cb(void *p);


void newgimis_NotifyShellKey_33( newgimis *pMe ) ; // ���ճ��������� �ֻ���ʶ��Ϊ 33 �� ��ֻ�ᴥ��������Ϣ 257 256 258 �������ֻ���һֱ����256
boolean InputMode33(newgimis *pMe, ITextCtl	*textCtl,AEEEvent eCode, uint16 wParam, uint32 dwParam );
void Timer_Close_BackLight(newgimis *pMe);

//������λ

int newgimis_Init_UKBD(newgimis * pMe);
void newgimis_OpenGPS_UKBD(newgimis * pMe);
void newgimis_CloseGPS_UKBD(newgimis * pMe);

boolean SendBDData(newgimis *pMe);
boolean SendErrorBDData( newgimis *pMe );

//�Զ��ϴ�¼��
int newgimis_GetRecordInfo_2(newgimis* pMe);
void newgimis_GetRecordInfo(newgimis* pMe);
void newgimis_GetRecord(newgimis* pMe);

//s958GPS
int newgimis_Init_S958GPS(newgimis * pMe);
void newgimis_OpenGPS_S958GPS(newgimis * pMe);
void newgimis_CloseGPS_S958GPS(newgimis * pMe);

//�ָ�HTTP ����wait�ı�־λ

void uf_MonitorWait_HTTP(newgimis *pMe);
void uf_MonitorWait_GPS(newgimis *pMe);
void uf_MonitorWait_E(newgimis *pMe);

//����  
void initQueue(newgimis *pMe);                 //���г�ʼ���ĺ���  
int isEmpty(newgimis *pMe);                   //�ж϶����Ƿ�Ϊ�յĺ���  
int isFull(newgimis *pMe);                    //�ж϶����Ƿ����ĺ���  
int inQueue(newgimis *pMe, YYXF_INFO t_info);        //��ӵĺ���   
int outQueue(newgimis *pMe, YYXF_INFO* t_info);    //���ӵĺ���,ͬʱ������ӵ�Ԫ��  
/*void traverseQueue(QUEUE * pQueue);*/



#endif
