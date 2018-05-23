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



#define DEFAULT_VERSION				    "V3.1.2PAYY"			// 版本号   
#define D_VERSION				    "3.1.2PAYY"			// 版本号 
#define SPD_MOBILE_NO			"MOBILE_NO = "		// 手机号码	
#define SPD_MOBILE_IMEI         "MOBILE_IMEI = "		// 手机imei
#define DEFAULT_FIELD_SPLIT			";"					// 发送数据的不同字段字符串之间的分割符
#define SPD_CONFIG_CALLNOS_STRING    "QCHAT_CALLNO = "		// 对讲号码
#define SPD_CONFIG_LOG			    "WRITE_LOG = "			// 写日志, '0' 不写，'1' 写
#define USR_NMASK_TRANSMITPHOTO		0x32			// 启用一键上传照片的掩码
#define SPD_CONFIG_SOT			    "SpaceOfTime = "		// 重启GPS的时间间隔(秒)
#define DEFAULT_MAX_ARRAY_SIZE		16					// 	在发送GPS信息之前保存最多组定位数据
#define SPD_CONFIG_RSG			    "ReStartGps = "			// 出现故障时是否重启GPS，mInt_ReStartGps 说明
#define SPD_CONFIG_USRGROUP_STRING    "QCHAT_GROUP = "		// 预定义优先级
#define SPD_CONFIG_VERSION_STRING      "VERSION = "			// 

// 有关定位鉴权  Add by zjie 2012-10-08
#define SPD_CONFIG_CT_AUTH		"CT_AUTH = "		// 是否使用鉴权
#define DEFAULT_CT_AUTH          '0'					// 缺省是不使用
#define SPD_CONFIG_CUSID		"CT_SID = "			// 鉴权的账户
#define DEFAULT_CT_CUSID        "35110129"				
#define SPD_CONFIG_CUPWD		"CT_PWD = "			// 鉴权账户的密码
#define DEFAULT_CT_CUPWD        "123456"				
// Add End	
	
#define SPD_PHOTO_TYPE			    "MOBILE_TYPE = "		// 手机型号
#define DEFAULT_PHOTO_TYPE			"66"					//  66 =  gota 660， 68 = gota 680，55 = qchat holleycomm E550B , 01 = viewSonic (Q3,Q3+)
 														    // 55 = holleycomm Q660, 72 华为7266 

#define FONT_WIDTH			        "FONT_WIDTH = "			//  字符宽度,此处为实际每行多少个汉字
#define DEFAULT_FONT_WIDTH			22
#define FONT_HEIGHT			        "FONT_HEIGHT = "		//  字符高度
#define DEFAULT_FONT_HEIGHT			16
#define ADD_HOUR			        "ADD_HOUR = "			//  时间修正
#define DEFAULT_ADD_HOUR            0	

#define DEFAULT_ReStartGps          2						// 见mInt_ReStartGps说明
#define DEFAULT_sot					60						// 见mInt_sot说明		2013-07-19 由25改成60



#define SPD_CONFIG_PULSATION        "PULSATION = "			// 心跳周期
#define DEFAULT_PULSATION			60						// 缺省60秒
#define DEFAULT_INTERVAL			60								// 缺省时间间隔

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

// GPS离线数据最大条数 [3/9/2016 Administrator]
#define SPD_CONFIG_OFFLINNEDATA_MAX_STRING   "GPS_OFFLINNEDATA_MAX = "
#define DEFAULT_OFFLINNEDATA		5

//几次1289后播放cari.mp3的标志位
#define SPD_CONFIG_PLAY_ERROR_MP3_STRING     "CONNECT_ERROR = "
#define DEFAULT_CONNECT_ERROR       0


// 以下用于离线数据的宏定义
#define SPD_OFFLINE_1             "offline1.txt"
#define SPD_OFFLINE_2             "offline2.txt"
#define SPD_OFFLINE_3             "offline3.txt"
#define SPD_OFFLINE_4             "offline4.txt"
#define SPD_OFFLINE_5             "offline5.txt"
#define SPD_OFFLINE_6             "offline6.txt"
#define SPD_OFFLINE_7             "offline7.txt"
#define SPD_OFFLINE_8             "offline8.txt"
#define SPD_OFFLINE_9             "offline9.txt"

//离线天数
#define SPD_CONFIG_OFFLINE_DAY  "OFFLINE_DAY = "
#define DEFAULT_OFFLINE_DAY  7 //默认是7天

//当天已经生成几份离线文件（用于生成文件时候取名使用）
#define SPD_CONFIG_OFFLINE_FILE_C "OFFLINE_COUNT = "
#define DEFAULT_OFFLINE_COUNT  1 //默认是7天


#define SPD_CONFIG_ACCURACY_CHAR        "ACCURACY_TYPE = "			// 精度  '1'，'2'，'3'，'4'，'5'，'6'
#define DEFAULT_AccType				'3'								// 定位精度，缺省3级

#define SPD_CONFIG_WATCHDOG_CHAR         "WATCHDOG = "				// 看门狗  0 不要看门狗，  1 要看门狗
#define DEFAULT_WatchDog			'0'				// 缺省 0 不要看门狗

// 关闭旧版本 #define SPD_CONFIG_START_CHAR			 "START_MODE = "			// 启动模式  '1' 开机自启动 , '0' 开机不启动
// 关闭旧版本  #define SPD_CONFIG_AUTHORIZE        "IF_AUTHORIZE = "			// 是否授权 '1' 授权 '0' 未授权

#define SPD_CONFIG_START_CHAR		"MODE_START = "			// 启动模式  '1' 开机自启动 , '0' 开机不启动
#define DEFAULT_START				'0'				// 缺省		

#define SPD_CONFIG_AUTHORIZE        "AUTHORIZE_STS = "			// 是否授权 '1' 授权 '0' 未授权
#define DEFAULT_AUTHORIZE			'0'			   // 缺省未授权

#define SPD_CONFIG_GPS_MODE			 "GPS_MODE = "			    // 中心端要求的GPS状态  '1' 启动 , '0' 不启动
#define DEFAULT_GpsMode				'0'				// 缺省不启动		


#define SPD_CONFIG_PIC_CHAR			 "PIC_MODE = "	  // 长按键时使用的图片
#define DEFAULT_PIC				     '1'			   // 缺省，用“O”，“—”  代表'0','1'	
#define DEFAULT_ARRAY_SIZE			1					// 	缺省发送的个数

// 手机拍照存放的路径
#define SPD_PHOTO_PATH			    "PHOTO_PATH = "		//  手机拍照存放的路径: root/photo/gallery/
#define DEFAULT_PATH				"root/photo/gallery/"		// 缺省gota G660 的路径, Qchat路径"md/__Images/"
// G680: filemgr/image/My Photo/                    

// 手机视频存放的路径   Add by zjie 2012-09-21
#define SPD_VIDEO_PATH			    "VIDEO_PATH = "		
#define SPD_AUDREC_PATH			    "AUDREC_PATH = "		// 录音文件存放的路径   Add by zjie 2014-04-15

#define SPD_CONFIG_USE_LOAD			 "USE_LOAD = "	// 是否使用按键装载模式  '1' 启动 , '0' 不启动
#define DEFAULT_UseLoad				'0'				// 缺省不启动

//右功能键是否跳出界面 0 1：跳出
#define SPD_CONFIG_RIGHT_KEY_FLAG   "RIGHT_KEY_FLAG = "
#define DEFAULT_Right_Key           '0' // 缺省不启动

//是否具有自动录音上传功能 1有
#define SPD_CONFIG_ENABLE_AUDIO    "Auto_Upload_Audio = "
#define DEFAULT_Enable_Audio              0

#define SPD_CONFIG_ENABLE_MD5    "PICTURE_ENABLE_MD5 = "
#define DEFAULT_Enable_MD5              0


// 启动发送照片
#define SPD_CONFIG_PHOTO			 "IF_PHOTO = "	// 是否使用按键发送照片,见定义说明  '0' 不启动, '1' 启动 , '2' 一键上传照片
#define DEFAULT_PHOTO				'0'				// 缺省不启动


#define SPD_PHOTO_WIDTH			    "PHOTO_WIDTH = "		//  宽度
#define DEFAULT_PHOTO_WIDTH			176						//  缺省宽度
#define SPD_PHOTO_HEIGHT			"PHOTO_HEIGHT = "		//  高度
#define DEFAULT_PHOTO_HEIGHT		132						//  缺省高度

#define SPD_PHOTO_SEND			    "PHOTO_SEND = "			//  是否立即发送照片，见 cPhotoSend 说明
#define DEFAULT_PHOTO_SEND			'1'						//  缺省，是

#define SPD_CONFIG_POSDET_ATIMES      "POSDET_ATIME = "		// 辅助定位次数
#define DEFAULT_POSDET_ATIMES		  10					// 缺省辅助定位次数

#define DEFAULT_LOG					'0'						// 缺省不写			test 

#define SPD_CONFIG_PTT_KEY			 "PTT_KEY = "			// 一键呼叫的按键值, 数值，如PTT是57421
#define DEFAULT_PTTKEY				 AVK_PTT				// 缺省
#define SPD_CONFIG_EXIT				 "IF_EXIT = "			// 是否允许按1#退出， '0'  不允许，   '1' 允许
#define DEFAULT_EXIT				  '0'					// 缺省不允许

#define SPD_CONFIG_CHECKON_KEEP    "ATTENDANCE_KEEP_DAY = "		// 考勤信息最多保存天数
#define DEFAULT_KEEP_DAY			7							// 缺省 gps离线数据

#define DEFAULT_SPLIT_LEN			2					//  分配分隔符字符串空间

#define SPD_CONF_TA				"CONN_ERR_TIME_A = "			// 配置参数TA(-1289持续多长时间重启手机)
#define DEFAULT_TA				 20					// 缺省30分钟
#define SPD_CONF_TB				"CONN_ERR_TIME_B = "			// 配置参数TB(每天哪个时段重启手机)
#define DEFAULT_TB				 "25"					// 25 不重启手机, 见下面说明

#define SPD_CONFIG_INTERVAL_GET_STRING    "GPS_INTERVAL_GET = "		// 接收GPS信息的时间间隔(采集周期)
#define DEFAULT_INTERVAL			60								// 缺省时间间隔

#define SPD_CONFIG_ASSIST_GPS_INTERVAL    "ASSIST_GPS_INTERVAL = "		// 辅助采集周期,缺省就是采集周期

#define SPD_CONFIG_CENTER_IP_STRING      "CENTER_IP = "				// GPS中心端IP地址
#define SPD_DEFAULT_CENTER_IP          ""						// 缺省的中心端IP地址空，授权后中心提供等待

#define SPD_CONFIG_POSDET_MODE_CHAR      "POSDET_MODE = "			// 定位方式 0 OneShot 方式, 1 Local方式
#define DEFAULT_PosDetMode			0				// 缺省定位方式	

#define SPD_CONFIG_ARRAY_SIZE		"ARRAY_SIZE = "

#define SPD_QOS_DEFAULT       127


// 手机状态信息掩码 开始
#define USR_NMASK_SS_TAPI			0x7				// 2^0 + 2^1 + 2^2:  TAPI 的掩码 
#define USR_NMASK_SS_RSSI			0x8				// 2^3:  1x 已经可用 
#define USR_NMASK_SS_HDR_RSSI		0x10			// 2^4:  evdo 已经可用 
#define USR_NMASK_WAIT_IDLE			0x20            // 等待从离线状态转为IDLE状态  2^5
#define USR_NMASK_GPS_ONE			0x40            // 第一次使用卫星定位  2^6			// Add  2013-04-27
#define gimis_HasStatus(pMe,nMask)     (pMe->cPhoneSts & nMask)    	// 根据掩码取出某一位置的值
//根据掩码设置某一位置的值
#define gimis_SetStatus(pMe, nMask)  { pMe->cPhoneSts = (nMask<0x8) ? (pMe->cPhoneSts & 0xF8) | nMask : pMe->cPhoneSts | nMask;	}
// 手机状态信息掩码 结束

#define EVT_CHECKIN_EVENT			EVT_USER+1010				// 网络恢复正常后发送考勤信息

#define  MAXTEXTLEN 22

#define GPSGROUP 3

/////// 与HTTP有关的定义//////////////////////////////////////////////////////////////////////////
#define FIND_STRING(a, b) if (STRISTR(a, b) != NULL)
#define TEST_HOST  "http://192.168.84.40/" //"http://10.10.10.84/"





#define XTRA_EVT_USER_QQSQ_CG EVT_USER + 2100  //请求授权成功
#define XTRA_EVT_USER_QQSQ_SB EVT_USER + 2101  //请求授权失败
#define XTRA_EVT_USER_QQSQ_SB_WLYC EVT_USER + 2102  //请求授权失败
#define XTRA_EVT_USER_XT_CG EVT_USER + 2103  //请求授权成功
#define XTRA_EVT_USER_PRINT EVT_USER + 2104  //打印 代码 如 554 1289 等
#define XTRA_EVT_USER_ERR_GPS EVT_USER + 2105 // 发送10004
#define XTRA_EVT_USER_NAL_GPS EVT_USER + 2106 // 发送10004


//###任务###2015-12-28
#define XTRA_EVT_USER_DISPLAY		EVT_USER + 2107 //任务里面抛出各种重绘界面事件
//非流程任务，发送结果的各种弹窗提示
#define  TASK2_EVT_USER_DISPLAY		EVT_USER + 2108 //非流程任务里面抛出各种重绘界面事件
//任务list界面，各种弹窗提示事件
#define  TASKLIST_EVT_USER_DISPLAY  EVT_USER + 2109


//北斗
#define XTRA_EVT_USER_NAL_BD EVT_USER + 2110 // 发送BD

#define XTRA_EVT_USER_ERR_BD EVT_USER + 2111 // 发送BD


// 
// #define  EVT_MESSAGE_B		EVT_USER + 2010

#define IMG_FILE_FAILURE			"kqsb.bmp"				// 考勤失败状态图片文件名
#define IMG_FILE_SUCCESS			"kqcg.bmp"				// 考勤成功态图片文件名
#define IMG_FILE_SENDING			"sending.bmp"			// 正在发送状态图片文件名
#define IMG_FILE_SENDFAILURE		"kqfssb.bmp"			// 考勤发送失败状态图片文件名
#define IMG_FILE_KEEP				"kqbccg.bmp"			// 考勤状态已保存,但未发送
#define IMG_FILE_NOSTS				"mykqsj.bmp"			// 没有考勤定位数据
#define IMG_FILE_SENDOK				"kqfscg.bmp"			// 考勤发送成功
#define IMG_FILE_NONET				"wlbnsy.bmp"			// 网络不能使用
#define IMG_FILE_ZTBSL				"ztbsl.bmp"				// 没有定位数据状态不受理
#define IMG_FILE_YWWJ				"ywwj.bmp"				// 已无文件要发送		
#define IMG_FILE_WLCSH				"wlcsh.bmp"				// 网络初始化，需重启手机		// Add by zjie 2013-04-12
#define IMG_FILE_SCLYWJ				"zzsclywj.bmp"			// 正在上传录音文件		// Add by zjie 2014-04-10

//////  与照片有关的宏定义
// 照片的后缀
#define PHOTO_FILE_EXTENSION        "jpg"               // photo file extension
#define TEXT_MAXSIZE				100					 // 照片关联内容最大字符数


#define DEFAULT_SPLIT				","					// 发送数据的同一字段字符串之间的分割符   


#define LOG_STR_LEN			  256 					//  写入日志文件一行的最大长度,如超过此长度则不用掉用此定义
#define NUM_VERSION				        2061000				// 数值型版本号，升级版本后，判断版本兼容性用


//配置文件中加手机型号，转义字符
#define  ESC_EQ             "ZA="                    // '=' 转义成 ' '
#define  ESC_SP             " "                      
#define  ESC_SUB            "ZA-"                    // '-' 转义成 '_'
#define  ESC_ZA_             "_"                      
// Add by zjie 2013-03-15 转义字符
#define  ESC_for_asterisk         "ZA:"                    // ':' 转义成 '*'
#define  ESC_ASTERISK             "*"        

///////////////////////   2015-10-26 关于通讯录的 开始   //////////////////
#define COLON_SPLIT					":"					        // 发送数据的同一字段字符串之间的分割符,用于通信录字段间分隔
///////////////////////     END  关于通讯录的结束        //////////////////


#define  FILE_OFFDATA 1024  //用来标记单个文件的最大值
#define  ITEM_OFFDATA 180  //用来标记单个文件的最大值

#define  LX_FILE_OFFDATA 1200  //用来标记单个文件的最大值

#define DEFAULT_KEYTIME	3			// 用户长按键达到规定的时间(秒),才发送用户按键

#define TASK_SET_TIMER  30	//处于后台时,任务来后定时多久启动  以及任务http有关的超时时长[3/4/2016 Administrator]

//录音文件的信息宏定义
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
		SCREENID_NOTIFY,     //#后台模式任务提醒#
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

//存放照片的结构体
typedef struct _SavePhoto
{
	int i_dex; //索引
	char photoname[65];//照片名字
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

		// 与照片有关
  Menu_write,		// 输入内容	
  Menu_right,		// 下一张
  Menu_left,			// 上一张
  Menu_del,			// 删除一张照片
  Menu_rtn,			// 返回定位界面
  Menu_input		// 切换到输入框
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
	uint8  nPartsTotal;				// 总段数
	uint8  nPartNum;					// 正在传第几段，1 第1段，2 第2段，。。。。。  
	char   cStatus;					// 0: 没有收到新的上传指令, 1: 收到新的上传指令
	
	uint16 recordId;	            //录音文件标号 用来翻页定位
	uint32 recordSize;	            //录音文件的总大小
    char   recordName[AEE_MAX_FILE_NAME];	//录音文件名
	
	char   recordInfo[1998];
}UploadRecord;

//音频下发http接收的数据
typedef struct 
{
	int icount;//播放次数
	char cName[128];//播放名称
	char cTel[16];   // 手机tel
}YYXF_INFO;

//音频队列结构体定义  
typedef struct  
{  
    YYXF_INFO *pBase;    //用于动态分配内存,pBase保存数组的首地址  
    int front;      //指向头结点  
    int rear;       //指向最后一个元素的下一结点  
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
	char         cWrtLog;											//  是否写日志， '0' 不写, '1' 写
	uint32		nInd;												// 记录日志的次数	iInd
	uint16		mInt_StartRun;		//  开机自启动过程: 1 : 收到init消息, 4 : 1x 可用, 5 : evdo 可用
	// 5/35 开机自启动, 6/36 手动启动,  7/37短信启动 ,8/38 后台闹钟启动,9/39 前台闹钟启动
	//   20 ~ 30 ： 正在进行星历下载等初始化动作
									//  原来 0/20 手动启动, 11/1/21 开机自启动, 3/23短信启动 ,2/22,4/24 闹钟启动	

	// Add by zjie 2012-10-08
	char         cCTAuth;				// 是否使用电信定位鉴权， 0 不使用， 1 使用
	char		    sCusID[17];					// 电信定位鉴权的账户
	char		    sPassword[13];				// 电信定位鉴权账户的密码 
	// Add End

	CSettings   gpsSettings;
	uint32		ipulsation;				// 心跳周期，单位妙
    uint32		iInterval;				// 接收GPS的时间间隔，单位妙
	uint32		iAssInterval;			//  使用辅助定位时的采集周期	
	char         strCenterIp[151];		// GPS中心端IP地址
	uint16		cPosDetMode;			// 定位方式 '0' OneShot 方式, '1' Local方式, '2'.... 见文档说明
	
	char        m_cPosDetMode[10];
	
	char			cAccType;				// 定位精度 '1'，'2'，'3'，'4'，'5'，'6',缺省为'3'
	int		    m_Acc ; //等级精度   定位精度 '1'，'2'，'3'，'4'，'5'，'6',缺省为'3'

	char			cWatchDog;				// 看门狗  '0' 不要看门狗，  '1' 要看门狗, '2' 重启手机
	char		    cStartMode;			   // 是否开机自启动, '1' 开机自启动 , '0' 开机不启动
    char			cAuthorize;			   // 是否授权,'1' 授权 '0' 未授权
	char		    cGpsMode;			   // 中心端要求的开机后GPS状态  '1' 启动GPS , '0' 关闭GPS
	// '2' 中心端要求本次关闭程序，但不写入到配置文件中（即仅本次生效）
	char		    cUseLoad;			   // 是否使用按键装载模式  '1' 使用 , '0' 不使用

	char         cPicMode;			   // 长按键时使用的图片 '0'- 用“空车”，“重车” 代表状态'0','1'  1 -代表状态 0 1   2 -考勤  3 -NFC 功能
	//                    '1'  用“O”，“—”  代表'0','1' 	
	//	                 '2'  发送考勤信息 
									   //                    '3'  发送NFC考勤信息   2013-09-01	

	int			nArySize;					// 在发送数据的之前保存几组数据，该参数由配置表定义

	char         sAudRecPath[MAX_FILE_NAME+1];						// 手机录音文件存放的路径     Add by zjie  2014-04-10
	char         sVideoPath[MAX_FILE_NAME+1];						// 手机视频文件存放的路径
   char         sPhotoPath[MAX_FILE_NAME+1];						// 手机照片的路径	  原来字节是512， 2012-09-21

   char		    cUsePhoto;											// 是否启动发送照片  '1' 启动 , '0' 不启动
   // Change by zjie 2012-09-21 含义更改如下
   // '0'  不发送视频和照片
   // '1'  不发送视频，发照片, 即ASC码第0个字节位置
   // '4'  发送视频，不发送照片, 即ASC码第2个字节位置
   // '5'  发送视频，发送照片
   // '7'  发送视频，一键上传照片

   int			iPhotoWidth;										// 显示照片的宽度
   int			iPhotoHeight;										// 显示照片的高度

   char         sMobileType[3];										// 手机型号

   uint32  iFontWidth;				// 字符宽度,此处为实际每行多少个汉字
   uint32  iFontHeight;				// 字符高度
   uint32  iAddHour;				// 时间修正
   // 该字段含义做如下修改  Change by zjie 2012-09-21 
									// 个位+十位数的值为照片时间修正, 百位+千位数的值为视频文件时间修正

   int  mInt_ReStartGps;			// 0 不重启GPS， 1 第1次出现故障时重启gps, 2 每次收到故障时都重启GPS
   int  mInt_sot;					//  重启GPS 时时间间隔(秒) 

   char		    cPhotoSend;											// 是否立即发送照片  '1' 立即（即不需添加文字） , '0' 不必（即可添加文字）
   uint32		iPosDetAtime;										//  辅助定位次数
   // Add by zjie 2013-01-01
   uint32		nPttkey;											//  一键呼叫的按键值, 数值，如PTT是57421
   char		    cCanExit;											//  是否允许按1#退出， '0'  不允许，   '1' 允许

   char    *sUsrGroup;							// 优先级的信息
   char	   *sPttCallNo;							// 配置文件中一键呼叫的对讲号码	
   int     nKeepDays;								// 考勤数据保存的天数

   int      iErrConTa;							// WEB_ERROR_CONNECT持续时间
   char     sRestTb[11];						// 重启手机时间段, 第1-2位表示时间段，第3-10被重启的日期yyyymmdd
   int		iRestTb ;// 重启时段  == 25 的时候说明重启时段是关闭的
   int		iTBCT ;  // 用来存放当前当前的时间  （比如 如果当前的重启时间为 5点 如果这个时候是4点，那么就需要重启）
   char		sVer[11];				// 版本号
   char		sVer_1[11];				// 版本号  没有V 的
   char		MyMobileNo[64+1];					// 本机的电话号码,  Add 2013-09-23
   char		MobileNoDJ[64+1];					// 本机的电话号码（对讲）,  Add 2013-09-23


	char         cPhoneSts;				// 当前手机(含网络)状态  见“手机状态信息掩码”定义  
	//   其余见“手机状态信息掩码”定义处。
										//  旧   '0' 等待从离线状态设置为IDLE状态 , '2' 不详,
	char     szMobileID[MOBILE_ID_LEN + 1];		// 2013-09-23   手机卡的串号

    char     szMobileID_Configer[MOBILE_ID_LEN + 1];		// 2013-09-23   手机卡的串号


	//  ================ 没有网络情况下(即离线考勤，保存考勤信息 =======================
   int	   nSavInd;									// 0 没有离线考勤数据，>0 有离线考勤数据



   void *pXtrxDcApp;							// 星历下载应用到的结构XtraDcApp


   //// 以下与GPS有关的开始 、
   void        *pScreenData;
   void        *pScreenData_gps;
   void        *pScreenData_Audio;
   void  *Old_ScreenData;  // 旧的页面指针 用于保存GPS页面的值
   void  *New_ScreenData; //新的页面指针 用于保存当前页面的指针 等旧的释放后重新给 currentHandler
   AEEHANDLER  currentHandler;
   CScreen     appScreens[SCREENID_MAX];
   boolean GPSFlag;		//	FALSE 表示关闭 TRUE 表示开启
   char	   cPosModused;				// 当前定位使用的模式

   uint32		iAssLastTime;			//  上次使用辅助定位的时间		Add by zjie 2013-04-12	
   // 以下与GPS有关的结束、、、

   IStatic *		m_pIStatic;



   /////////  与 请求授权有关的 开始
   IWeb *   m_pIWeb_qqsq; //用来心跳的
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

   char m_SQ1[250]; // 授权的主地址 如果这个 不能用则切换到第二个地址
   char m_SQ2[250]; // 授权的第二个地址 如果主地址不能用，则用这个地址
   int m_SQ_C ;// 授权地址选择 如果为 1 则为主地址授权 如果为 2 则为第二地址授权
   //////////  与心跳有关的结束  ///////



   /////////////////////    以下与HTTP有关的 开始 ////////////
   int					m_iAppStatus;
   char                 *pszPostData;  // POST string
   IPeek                *pipPostData;  // POST stream, if any
   AEECallback          cb;            // how to call me back
   uint32               uStart;
   IWeb*	m_pIWeb;
   IWeb*	m_pIWeb_s;  //用于上报GPS值的
   /////////  与 心跳有关的 开始
   IWeb *   m_pIWeb_xt; //用来心跳的
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

   char Url_st[250]; //心跳地址
   //////////  与心跳有关的结束  ///////

    /////////  与 上报离线数据 开始
   IWeb *   m_pIWeb_lx; //用来心跳的
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
   //////////  与与发送数据有关 结束  ///////


    /////////  与 上报离线数据 开始
   IWeb *   m_pIWeb_slx; //用来心跳的
   int					m_iAppStatus_slx;
   char                 *pszPostData_slx;  // POST string   //读取到的离线数据，准备发出去
   IPeek                *pipPostData_slx;  // POST stream, if any
   AEECallback          cb_slx;            // how to call me back
   uint32               uStart_slx;
   IWebResp *           piWResp_slx;       // the answer I get from IWeb
   ISource*   pSource_slx;
   byte *		m_BodyBuffer_slx;		// Buffer to hold HTML data
   int			m_BodySize_slx;			// Size of HTML data in buffer
   int			m_BodyAllocSize_slx;	// Allocated size of HTML data buffer
   //////////  与与发送数据有关 结束  ///////


   /////////  与 上报 三组GPS 数据 数据 开始  加这个主要是星历下载出问题的时候， _s 这个接口的HTTP就会受影响
   IWeb *   m_pIWeb_gps; //用来心跳的
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
   //////////  与与发送数据有关 结束  ///////


   //////////////   与上报缓冲 GPS数据有关和 定位异常上报有关的  ///////////////
   IWeb *   m_pIWeb_E; //用来心跳的
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
   int iPostPhTm_E; //标记开始请求的时间 上报超时 HTTP请求
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

   int i_HttpMod; //HTTP接收返回来的模式 数据	1 - 代表接收回来的是请求授权的 2 - 接收回来的是GPS数据上传回来的数据
   //////////////////////   与HTTP有关的 结束   //////////////

   char		 sUrl[1024];  //用来发送的网址 暂时用于发送 GPS数据

   // GPS 接收的 经纬度 以及 水平和垂直方向的数值
   double lat;       /* latitude on WGS-84 Geoid */
   double lon;       /* longitude on WGS-84 Geoid */
   uint16 dVelHor;	/*  水平方向速度 估计是以0.25 meters/second 为单位  */
   int8   dVelVer;	/*  垂直方向速度 估计是以0.5 meters/second 为单位*/	

   //北斗 定位的经纬度和水平速度
	char   latStr_BD[MAXTEXTLEN];
	char   lonStr_BD[MAXTEXTLEN];
	
	uint16 dVelHor_BD;

	int m_PM_BD;

   //  用来处理发送几组数据
   int m_GPSGroup; // 表示发送几组GPS数据


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
	
	char m_PM;//用于得到当前的定位模式。
	char m_PM1;
	char m_PM2;
	char m_PM3;

	char m_PM4;
	char m_PM5;
	char m_PM6;
	char m_PM7;
	char m_PM8;


	char m_ycPM; //出错的时候定位方式



	//////////////  以下与照片有关的变量
	int			iPressSts;											// 对长按键的处理状态:
																	// 0 没有按键， 1 用户按键
																	// 以下处理状态:
																	// 2 用户按键达到规定时间 3 准备发送按键值
																	// 4 已经发送按键值  5 弹出splash screen 
																	// 以下处理照片
																	//  12 用户按键达到规定时间
																	//  13 检查是否有最新的照片
																	//  14 可以显示图像
																	//  15 正在处理图像
																	// 以下处理显示提示图片
																	//   20 准备显示图片
																	//   21 弹出splash screen
																    //   25 显示信息图片(发送中)
																	//   26 显示图片(已无文件要传送)
																	//   27 正在上传录音文件 
																	// 以下处理物流下单窗体( 30~ 39)  Add by zjie 2012-05-10 
																	//	 30 准备显示物流编辑界面
																	//   31 弹出物流编辑界面
																	// Add End 2012-05-10
																	// 以下显示网络初始化信息
																	//   35 准备显示网络初始化信息
																	//  Add 2013-04-27
																	//   40 准备显示查询考勤的信息
																	//  Add 2013-09-01 
																	//   50 显示NFC刷卡界面
																	//  Add 2013-10-01
																	//    60 显示任务的界面	

																	//    70 有关语音的处理			Add 2014-01-04

	int          iShowText;											// 1 在画图片的同时重新画描述信息控件
																	// 2 在画图片的同时不重新画描述信息控件
																	// 3 正在选择输入法
																	// 4 仅刷新屏幕  
																	// 0 未设置 

	int			iSendPhoto;											// 0 没有发送照片，1 准备发送， 2 正在发送
																	// 3 发送成功， 4 发送失败
	IImage		*pi;												// 图像资源
	IImage		*pi_copy;											// 图像资源 备份的
	int			imgidx;												// 待显示的相片的索引, 1 第1张，2 第2张
	char		sFileName[MAX_FILE_NAME+1];							// 待上传照片的文件名：文件名不受限 2012-10-16  p+yyyymmddhhmmss.jpg
	char         sFile[MAX_FILE_NAME+1];	// 文件名（含路径）	
	void *FleUpdApp;								// 上传文件应用到的结构
   
	ITextCtl		*textCtl;											// 与照片相关的文本框
    IMenuCtl		*textmenu;											// 与文本框控件相关
	AECHAR		pText[TEXT_MAXSIZE+1];								// 与照片有关的内容,最多100个双字节字符

	int m_gpsFlag;		// 用来标记GPS的模式选择  0-纯网络   1- 手机自选 2-基站辅助 3--纯基站 4--首先 5--卫星

	AEERect                 m_rScreenRect;  // Screen Rect

	boolean m_FOBFlag ;//前后台的标志位  0-在前台运行  1-在后台运行
	int m_WitchPag; //用来记录在哪个页面  0-主页面 1-在GPS的那个页面   4-更多的那个界面 12-更多界面?	21-弹出任务更新提醒界面
											//16 任务列表SCREENID_TASKLIST


	// /// 与照片有关
	IStatic*                m_PhotoTextCtl; 
	SavePhoto m_photo[5];
	int m_i_photo;//用来指示当前显示的第几张照片
	boolean m_bSendPF ;//用来标记是否正在发送图片的标志位 TRUE -- 表示正在发送状态  FALSE -- 表示空闲

	// 与一键考勤有关
	char	sUrl_yjkq[200];  //用来发送的网址 暂时用于发送 GPS数据
	char	sUrl_Cyjkq[200];  //用来发送的网址 暂时用于发送 GPS数据

	char sUrl_lx[251]; //设置离线的网址

	// 保存一键考勤有关的
	char   lx_lon[MAXTEXTLEN];  // LO 经度
	char   lx_lat[MAXTEXTLEN];	// LA 纬度
	char   lx_vv[MAXTEXTLEN];	// VV 水平速度
	char   lx_ep;				// EP 电源
	// KS	按钮 这里默认都用 2
	char   lx_tt[15];			// TT 时间
	char   lx_pm;				// PM 定位模式


	// 与视频有关的变量
	SavePhoto m_Audio[5];



	/////  系统屏幕的一些参数  //////////
	int m_SYSFNH;//系统标准的字体高度
	int m_SYSFBH;//系统字体粗体高度
	int m_SYSCX;//系统屏幕宽度  //照片宽度
	int m_SYSCY;//系统屏幕高度
	int m_MenuH;//菜单控件的高度  

//	int m_PhotoW; //照片宽度
	int m_PhotoH; //照片高度

	boolean m_bGetGPSF;//用来标记此时获取GPS的状态  TRUE -- 表示获取异常 FLASE -- 获取正常

	
// 按键组合键的使用
	uint16       clsKey[2];				// 保存组合键，实现用组合键关机,只能连续收到2个键的组合键才能关机，其它则不关机
	boolean m_RelyExit;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台

	int	   m_OffData;									// 0 -- 没有离线考勤数据，1 -- 有离线考勤数据
	int	   m_Mutex_LX; //用于离线的互斥 主要是请求授权成功 和 心跳成功 会去访问有没有离线数据  0 -- 表示心跳可以用  1 -- 表示心跳成功后不能去访问
	int	   m_WitchLX ;//用来标记当前发送的是第几个离线文件

	int	   m_Flag_F; // 用来标记当前按键触发的是什么功能  1 -- 一键考勤      2 -- 发送空车     3 --  发送重车  4 -- 发送状态 0   5 -- 发送状态1  6 -- 一键打卡（数据转发）

	int	   m_Flag_Qqsq ; //用来标记是否正在请求授权  1 -- 是  0-- 不是

	int    m_F_R_O;   // 用来标记是不是真正的开机  // 1 - 表示真正开机  0 - 则不是  这个表示位 在  AEECLSID_PHONENOTIFIER 
   
	int	  m_T_L; //显示总函数  m_total_line 用缩写来表示



	char m_PowerFlag;// 标记用来开机受否自启  FLASE -- 不自启  TRUE -- 自启

	uint32 m_Error ;// 用来存放 HTTP通信的错误正确返回的代码 或者是定位异常的错误代码
	char show_Time[10]; // 用来在界面上显示的时间
	char show_LatLon[30]; // 用来在界面上显示经纬度


	//  以下是按组合键或者是长按键的有关变量
	
	int m_Flag_Key;//用来标记长按键触发进入  0 -- 不是长按键    1 -- 长按键进入
	int m_Witch_Key; // 用来标记是触发什么功能的  2 -- 一键考勤  5 -- 发送照片  8 -- 发送视频   3 -- 重车  4 -- 空车  9 -- 状态 1   10 -状态 0   
					 // 11- 一键打卡-关于厦门电力数据转发				

	int	m_PressSts;  //用来标记触发按键的按键状态  1 - 按下  0 - 没有按下	
	boolean zuhe_loong ;// 用来标记是组合键或者是长按键  0 - 长按键  1 - 组合键
	char m_zuhe[5]; // 存放组合键 * # 9 9 9  用于快速进入界面的
	uint32       iGetkey;			// 记录按键的时间长度，判断是否常按一个键（在后台或前台),用户按下键的时间,当按键大于3秒后发出状态
	uint16 wParam;					// 传递消息事件的参数,   触发按键的时候有用到
									// 考勤状态下,保存按键值
	int m_Key_Count ;// 用来标记当前有多少键被按下
	

	//  以下是请求授权超时的时候用的
	int iPostPhTm; //标记开始请求的时间
	int iPostPhTm_xt; //标记开始请求的时间 心跳包的HTTP请求
	int iPostPhTm_gps; //标记开始请求的时间  gps 的HTTP请求

	int iPos_kq_zt; //标记开始请求一键考勤和状态发送的 （空车 重车 状态1 状态0）

	int m_F_QQSQ_CS; //用来标记是不是请求授权超时  1 -- 超时  0 -- 没有超时 

	




	//////  以下是关于GPS优化的问题 START ////////
	int m_F_G_C; //用来统计有没有轮询完一遍  如果有则 报一次 1000004  m_F_G_C == 6 则说明轮询完一遍  如果获取到了  则重新 清 0
	int m_F_M; //用来标记在什么模式下   等于 0 则为在轮询的情况下  如果为 1 则在平台下发的模式下 
	uint8 m_F_P_V; //用来标记定位精度  AEEGPS_ACCURACY_LEVEL1 -- 1  到 AEEGPS_ACCURACY_LEVEL6 -- 6 （1-6代表等级精度）
	boolean m_F_Pos;  // 用来标记等级是否要加 如果加完之后 又变成不增加等级 TRUE == 表示增加等级  FLASE == 表示不增加等级
	boolean m_F_P_F ;// 用来标记是不是track_stop 后第一次用track_start  TRUE == 表示  是  FLASE = 表示不是
	boolean m_Q_P; //用来标记是否是程序开启时快速定位  TRUE -- 是  FLASE -- 不是
	int m_POS_M;// 获取当前手机能定位到的模式
	int m_NOMAINPOS; //表示是否需要在轮询模式下定位到的时候切换到主模式  == 0 则需要  == 1 则不需要  == 0 的时候还有一层用法 就是平台下发定位模式的时候切换到新的平台定位模式
	boolean m_PosMode ;// 用来标记是否是只能用纯卫星定位 TRUE -- 表示只能用纯卫星  FLASH -- 不是 还可以别的方式
	boolean m_F_START; //用来标记是不是才发起Track_Start   TRUE --  表示刚发起      FALSE -- 不是不是刚发起  
	///////          END        //////////////////


	//////  以下是关于 网络状态对功能要做些处理的 变量 START //////////////
	boolean m_F_NONET; //用来标记有没有网络  TRUE -- 开机的时候没有网络  FALSE -- 有网络
	boolean m_has_QQSQ; //用来标记是否请求授权过  TRUE == 表示请求授权过 FALSE == 表示还没有请求授权
	boolean m_Rely_XT; //用来标记是否真正的要发送心跳  TRUE = 要发送心跳  FALSE = 表示不是真的要发送，只是进来定时
	boolean m_F_QF;  // 标记手机欠费  TRUE - 手机欠费  FALSE - 手机没有欠费 
	//////////////////////////////       END      /////////////////////////

	uint32 m_POS_T; //用来标记定位周期
	uint32 m_Fix_C; //用来统计获取到的总定位数  用于获取到第一组发送数据后马上上传


	///////////////  互斥用的 用于HTTP 请求的时候
	int m_MUTEX; // 1 = 说明不能进行发送HTTP 已经有占用了http的资源  0= 说明可以使用http资源  
				 // (这里做个更改 == 1 说明是心跳占用资源 == 2说明是发送GPS数据占用资源  这样做的目的是自己解锁)



	uint32 m_FAIL_T; //用来存放进来获取GPS异常的情况


	boolean m_E_1289 ; // TRUE - 表示出现 1289 FALSE - 表示没有出现1289

	char m_ConText[256];//  用来存储照片里面编辑框中的编辑内容
	int m_ConT_FH;//用来标记此时是不是在符号的状态下  1-是  0-不是

	uint32 m_T_H_G; //这个时间是记录 上报HTTP 或者 请求GPS定位的时间 他们的差值要是不超过两秒 如果是请求GPS 则定时两秒 如果是HTTP则放弃此次请求

	int m_T_E_C; // 用来统计连续进来 获取失败几次 如果超过三次则暂停获取，暂停时间为200秒，然后继续获取 这个计数在心跳包上传函数中清除
	int m_W_E; // 用来统计1289连续出现的次数  当大于 3 的时候则重启手机

	boolean mb_MsgLog ;// 用来标记是否是短信发送日志  TRUE -- 是   FALSE - 不是

	// 心跳回调
	int m_F_R_C;// 标记是否读取HTTP请求授权的时候请求等待  ISOURCE_WAIT  == 1  是  == 0 不是
	char * m_HTTP_C;//HTTP回调返回的内容出现等待的时候 临时保存数据  动态申请空间  空间大小申请为 102400

	// GPS回调
	int m_F_R_GPS;// 标记是否读取HTTP请求授权的时候请求等待  ISOURCE_WAIT  == 1  是  == 0 不是  
	char * m_H_GPS;//HTTP回调返回的内容出现等待的时候 临时保存数据  动态申请空间  空间大小申请为 102400

	// 请求授权回调
	int m_F_R_SQ;// 标记是否读取HTTP请求授权的时候请求等待  ISOURCE_WAIT  == 1  是  == 0 不是  
	char * m_H_SQ;//HTTP回调返回的内容出现等待的时候 临时保存数据  动态申请空间  空间大小申请为 102400

	// 考勤回调
	int m_F_R_LX;// 标记是否读取HTTP请求授权的时候请求等待  ISOURCE_WAIT  == 1  是  == 0 不是  
	char * m_H_LX;//HTTP回调返回的内容出现等待的时候 临时保存数据  动态申请空间  空间大小申请为 102400

	// 心跳回调  发送异常数据回调
	int m_F_R_E;// 标记是否读取HTTP请求授权的时候请求等待  ISOURCE_WAIT  == 1  是  == 0 不是
	char * m_HTTP_E;//HTTP回调返回的内容出现等待的时候 临时保存数据  动态申请空间  空间大小申请为 102400

	//////////  以下是关于 优思主板的输入法用到的变量  start  /////////
	int m_ys_code ; //用来存放优思主板的 ecode 的数值  // 如果这个数值的519 则说明还在编写内容  如果不是 272 则不是 

	//////////          END      //////////////////////////////////////



	////////////////  2015-10-26 START 以下是有关于通讯录结构的
	void	*pRichAddr;								// 有关通讯录的结构
	void *pRichptt;								// QCHAT应用到的结构
	char *sc_sub; // 这个是用于通讯录里面，判断这个手机有没有在通讯录里面
	//////////////////    END    ///////有关通讯录结构

	uint32	m_GPS_E_T ;  // 用来存放发送GPS error data  这样做的目的是防止 切换GPS模式的时候马上就回 100004  有上报平台 如果间隔过短就不上报

	int m_F_DJ; //用来标记手机是否正处于对讲状态  通过对讲事件 evtCode=28930 结束对讲  evtCode=28929 对讲连接 用一个宏定义标记对讲是否需要开启
	// = 1正处于对讲 别的值则不是处于对讲状态 

	int m_temp_FOBFlag;//防止再次进入前台的初始化，前台的初始化只有一次。

	///////  以下与任务有关的 ///////////
	void *pTaskApp;								// 与任务有关
	void        *pScreen_Task; // 这个是任务列表中上一页 下一页的控件
	int m_TaskC; //用于 一个界面显示多少的的任务数量
	int m_TaskID ;// 用来存放在当前页面下的当前的目录在第几项
	int m_Task_F ;// task_first 用来标记是不是程序起来后第一次接收任务，如果是的话，可能以前的任务没错，会排在前面
	int m_TTotal ;//当前任务的总数量
	int m_Page_c ;//当前任务需要的总页数
	int m_Cur_Page;// 当前显示第几页
	boolean m_bDoTask;  //新任务提醒，点击立刻开始任务置1  初始化0
	char m_IP[256]; // 对应的IP 地址 （包括 ip 端口 工程名）
	char m_Cur_TID[100]; //临时存放TaskID名称

	uint32  m_Cur_nDateTime;	//  [3/22/2016 Administrator]临时存放TASK_DATATIME

	ITextCtl		*textCtl_T;											// 与任务相关的文本框

	///////////////////////GPS离线数据包/////////////////////////
	//##公用
	boolean m_bMiss;   //在弹出显示各种"发送"的界面时候  忽略各种按键   1-屏蔽按键  2-不屏蔽

	boolean m_offlineData_gps;  //1-有离线数据  0-么有
	boolean m_Mutex_offgps; //用于离线的互斥 Http有没有被占用 1-占用   0-不被占用
	boolean m_bFlg_errG;    //是否正在等待发送errGPSdata    1-是; 0否

	int m_KJZQ_C ;// 统计开机自启进入的次数

	int m_F_CD; //标记是否为充电 == 1 是  == 0 不是

	int		m_E_SC;				// 用于存放连续多少次1289之后播放一次语音
					
	//////////////////////////////////////////////////////////////////////////

	// 以下这个变量是用于播放录音的
	int m_W_E_P; // 用来统计1289连续出现的次数  当大于 3 的时候则重启手机


	ISoundPlayer * pISoundPlayer;

	int m_SQ_again; //用来标记在一次授权 ，主要是可能从cn平台跳到另一个平台  = 1 在一次授权= 0 不用

    //  [2/4/2016 Administrator] ##与短信有关
	void *pSms;	
	
	//###任务###2015-12-28//
	boolean m_Task_fs;     //任务，按下反馈子任务 或者 按下 "完成" 时置1, 
	boolean m_Task_fh;		//任务输入法按下*键 置1; 
	IStatic	 *m_pIStatic_Task;

	//非流程任务
	int m_iTask_fkID;  //要反馈的 反馈项是第几个。
	int m_iText_Input;	    //1-正在任务输入编辑框   2-发送照片编辑框(暂时没设置);

	//当gps缓存文件是空的时候才将最新的条数更新 m_RofflineData_Num = m_WofflineData_Num
	int m_WofflineData_Num; //gps缓存最大数据条数 专门用于写入配置文件  [3/9/2016 Administrator]
	
	int m_RofflineData_Num;//gps缓存最大数据条数  专门用于从配置文件读出，和程序运行使用[3/9/2016 Administrator]

	//关于如果qchat的初始化失败，继续重复初始化的次数：增加的原因的当时初始化失败重新开始也会进授权，导致重复授权。。
	int m_WaitForQChat_Num;//    WaitForQChat失败3次后就不再WaitForQChat；[3/14/2016 Administrator]

    char m_Right_Key;	//部分机型右功能键在二级菜单会直接跳出影响前后台标志位，进而产生开启停留在start。

	//  [5/9/2016 Administrator] 友恺北斗接口 
	ITTSEXT *pIgpsext;
    char m_Con[30];

	//录音
	void *precord; //
	UploadRecord *rData ;
	int m_flag_Audio;//防止录音上传的 超时监控还没有走完，又开始新的一次发送。新的一次发送回释放空间导致超时监控错误
	int m_enable_Audio;//设置录音上传标志位

	char  szinfotxtPath[128];
	char  szinfotxtPath_2[128];
	
	//7天GPS离线数据
	char file_Name_7Day[128] ;
	IWeb *m_pIWeb_7Day; //用来心跳的
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
	//当天存了几份离线数据
	/*uint8 */

	//上传离线数据和写离线数据的互斥接口
	int mux_7Day;
	
	//s958GPS接口
	ITTSEXTGPS *pS958gps;

	//精度最低值
	uint8 m_min_Accuracy;

	//是否要删除上传好的录音的标志位
	uint8 m_del_Audio;
	
	//unsigned short us_CRC16_result;
	//MD5校验
	char s_MD5[33]; 
	uint8 m_enable_MD5;//是否校验。

	//evdo有信号，才开始初始化启动流程
	uint8 m_enable_evdo;

	//---------------start
	//音频下发模块
	//授权时候得到HTTP请求IP地址和UDPIP和port
	char sHttp_YYXF[64]; //HTTP地址IP
	char sUDPIP_YYXF[32];
	uint16 iUDPPORT_YYXF;
	char sUrl_YYXF[256];

	char sTCPIP_YYXF[32];
	uint16 iTCPPORT_YYXF;

	//socket
	//tcp
	int TCP_Status_yyxf;	//TCP是否连接正常，用于显示在手机界面
	INetMgr *pINetMgr_TCP_yyxf;
	ISocket *pISocket_TCP_yyxf;
	char m_readMsg_tcp[5120];	//读取rtp
	int	 m_nRecvSize;	//累计未处理的字节数
	char* m_DataBuf;	//解析后的数据包
	int m_Tcphead_flag;	//是否收到心跳包 根据此判断是否要回复平台。
	int m_Listen_Heart;//监听心跳是否收到，如果没有收到超过3分钟则重连
	//udp
	INetMgr *pINetMgr_UDP;
	ISocket *pISocket_UDP;
	char m_readMsg[10240];//UDP接收
	pList list_head_UDP;	//udp包存储链表
	int iflag_YYXF;//只保存一次头包
	int iCount_YYXF;	//计算收到几个UDP包

	int overTime_YYXF;	//发送超时
	int readUdpTime_YYXF;//接收超时

	//语音下发HTTP
	IWeb *m_pIWeb_yyxf; //用来心跳的
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

	//语音下发队列和控制
	QUEUE pQueue;
	YYXF_INFO t_httpInfo;//存放http接受的信息，用于UDP发送
	int m_audioCount;	//存 下发音频需播放几次	//参数服务端目前默认为1
	int gi_yyxfCount;//取 下发音频需播放几次	//参数服务端目前默认为1
	char m_audioName[128];	//存入 音频名称
	char gs_yyxfAudioName[128];//取出 音频名称
	char gs_yyxfPlayName[128];//播放的临时变量
	//语音包播放
	ISoundPlayer *pIPlayer;//UPD接收后的包播放

	//短信写入
	ISMS			  *pISMS_yyxf;
	ISMSMsg			  *pISmsMsg_yyxf;
	ISMSStorage       *pISmsStorage_yyxf;
	AEECallback		  cbMsg_yyxf;
	
	AECHAR		      *aeContent_yyxf;
	//---------------end

	int m_evdo_Num;//  初始为0,累计到8次，一次5秒。40秒后也能够将信号m_enable_evdo变量至1
	
} newgimis;


typedef struct
{
	AEEApplet    a;
	char		 pURL[128];
	IFileMgr*    pFileMgr;
	IFile*		 pFileUpload;				// 待上传的文件句柄
    FileInfo	 oFileInfo;					// 文件信息
	char         sFile[MAX_FILE_NAME + 1];	// 文件名（含路径）
	
	IWeb*        pWeb;
	AEECallback  webcb;
	IWebResp *   piWResp;		// the answer I get from IWeb
	ISource*     pSource;		// the body of the response		用这个也行	IGetLine *   piGetLine;				// the body of the response, if any
	ISourceUtil* pisu;
	char         sBodyBuf[10];				// 只要应答的第0个字符
	int          nBytes;					// count of bytes in response
	
	newgimis	*m_pParent;
	
	uint8		cSelsts;		//0 ：没有文件被选中发送
	//1： 有文件被选中发送，在选中进不来
	
	char         cSendSts;		// 发送的状态
	//	'1':  本段接收成功，如果不是最后一段，则手机端会继续发送下一段；如果是最后一段并且发送的是视频文件，则手机端会删除视频文件，提示手机用户“发送成功”。
	//	'0':  本段接收失败，则手机端会重发本段
	//	'A': 表示接收出现异常，手机端显示“发送失败”信息给手机用户。如果发送的是视频文件，则不会删除视频文件
	uint8		nPartNum;		// 正在传第几段，1 第1段，2 第2段，。。。。。  
	uint8	    nPartsTotal;	// 总段数
	uint16 		nSelRec;		// 选中的任务号
	
	int32		iFileRead;					// 读入文件的字节数  
	uint32      nHasRead;		// 已经上传读取的字节数
	
	/*	pList ptemp;//用于保存被选中的数据。*/
	uint8		 nState;					// 状态
	// 0(SUCCESS)    成功
	// 1(EFAILED)    失败 
	IPeek        *pipPostData;				// POST stream, 
	char         *pszPostData;				// POST string
	uint32		 dwPostOvertime;		// 记录发送文件后接收应答的时间，单位妙，接收时超过规定时间，关闭链接，并提示失败
	
	AECHAR		pText[999];		// 与上传文件有关的内容,最多100个双字节字符

	
	
}Record_information;


uint32 gimis_TEST_MOD_FILE( newgimis *pMe ); //监测MOD文件有没有存在

uint32 gimis_Init_Pram_Open( newgimis *pMe ); //读取存在txt的标志位 用来标记要不要自动启动


uint32 gimis_ReadPowerBoot( newgimis *pMe, IFile * pIFile );
uint32 gimis_WritePowerBoot( newgimis *pMe, IFile * pIFile );
// 设置 启动是否开启程序
uint32 gimis_SetPram_Open(newgimis *pMe);

// 设置 用于请求授权成功 启动是否开启程序
uint32 gimis_SetPram_Open_QQSQ_CG(newgimis *pMe);

uint32 gimis_InitGPSSettings( newgimis *pMe );
uint32 gimis_ReadGPSSettings( newgimis *pMe, IFile * pIFile );
uint32 gimis_WriteGPSSettings( newgimis *pMe, IFile * pIFile );

uint32 gimis_WriteHTEL(newgimis *pMe);

// 设置整个配置文件
uint32 gimis_SetGPSPara(newgimis *pMe);

// 获取整个配置文件
uint32 gimis_GetGPSPara(newgimis *pMe);

// 判断有没有离线数据
uint32 gimis_judgedata(newgimis *pMe);


/*-------------------------------------------------------------------
以下与处理按键有关
-------------------------------------------------------------------*/
// Add by zjie 2012-09-21 判断是否启用照片(或发送文件)等功能
boolean uf_HasFun(newgimis *pMe, int nMask);


/*-------------------------------------------------------------------
以下与网络有关  Add by zjie 2011-11-18
-------------------------------------------------------------------*/
boolean uf_CheckNetSts(newgimis * pMe);			// 检查网络是否可用
void uf_GetNetSts(newgimis * pMe);				// 获取网络状态
void uf_GetNetSts_imsi(newgimis * pMe);				// 获取网络状态


// 当网络从离线状态转为在线状态时，设置网络状态
void uf_SetNetSts(newgimis *pMe);


// 在本地记录数据(测试用)
void uf_RecLog(newgimis * pMe,char *vpStr_msg);


//////  以下与GPS有关的 开始 ///////
extern void *testgps_GetScreenData( newgimis *pMe );


extern void testgps_DrawScreen( newgimis *pMe, uint32 dwParam );
extern void testgps_Printf( newgimis *pMe, int nLine, int nCol, AEEFont fnt, 
                                uint32 dwFlags, const char *szFormat, ... );

extern void testgps_Printf_hanzi( newgimis *pMe, int nLine, int nCol, AEEFont fnt, 
                                uint32 dwFlags, AECHAR *szFormat, ... ); // 显示汉子

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

// SamplePosDet_SendPhoto_HandleEvent 发送照片
extern boolean SamplePosDet_SendPhoto_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam );

// SamplePosDet_SendAudio_HandleEvent 发送视频
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

///////////////////  后面添加的关于 获取屏幕的数据 gps显示的页面 /////////
extern void *testgps_GetScreenData_gps( newgimis *pMe );
extern void *testgps_SetScreenData_gps( newgimis *pMe, void *screenData );
//////////////////////////////////////////////////////////////////////////
// 
// 与视频有关 ///
extern void *testgps_GetScreenData_Audio( newgimis *pMe );
extern void *testgps_SetScreenData_Audio( newgimis *pMe, void *screenData );
/////////////////
///////  与GPS有关的结束  //////////



// 解析请求授权的返回的值并写入配置文件
// 返回值说明 如果返回 1 - 则成功接收请求授权  0 - 则返回的是错误信息
int ParseStringandWriteToConfigure( newgimis *pMe,char *pString); // 这个是用于请求授权返回的值

int ParseStringandWriteToConfigure_xintiao_GPS( newgimis *pMe,char *pString); // 这个是用于请求授权返回的值

int ParseStringandWriteToConfigure_xintiao_GPS_FAIL( newgimis *pMe,char *pString); // 这个是用于请求授权返回的值


// 发送GPS数据
boolean SendGPSData( newgimis *pMe );

// 发送错误的GPS 0x100001
boolean SendErrorGPSData_0( newgimis *pMe );

// 发送错误的GPS 0x100004
boolean SendErrorGPSData( newgimis *pMe );


// 显示手机状态
void uf_PhoneSts(newgimis *pMe);


// 与一键考勤有关
void newgimis_forYJKQ(newgimis * pMe,boolean bGet);

// 与发送空车有关
void newgimis_SendEmpty(newgimis * pMe,boolean bGet);

// 与发送重车有关
void newgimis_SendFull(newgimis * pMe,boolean bGet);

// 与发送状态0有关
void newgimis_Send_ZT_0(newgimis * pMe,boolean bGet);

// 与发送状态 1 有关
void newgimis_Send_ZT_1(newgimis * pMe,boolean bGet);

//与一键打卡（数据转发） 有关
void newgimis_forYJDK(newgimis * pMe,boolean bGet);

// 显示空车发送失败
void uf_AEPrint_EMPTY_FAIL_Back(newgimis * pMe);

// 显示重车发送失败
void uf_AEPrint_FULL_FAIL_Back(newgimis * pMe);

// 显示状态1发送失败
void uf_AEPrint_ZT_1_FAIL_Back(newgimis * pMe);

// 显示状态0发送失败
void uf_AEPrint_ZT_0_FAIL_Back(newgimis * pMe);

//显示打卡失败
void uf_AEPrint_YJDK_FAIL_Back(newgimis * pMe);

// 显示状态1发送成功
void uf_AEPrint_ZT_1_SUCC_Back(newgimis * pMe);

// 显示状态0发送成功
void uf_AEPrint_ZT_0_SUCC_Back(newgimis * pMe);

// 显示空车发送成功
void uf_AEPrint_EMPTY_SUCC_Back(newgimis * pMe);

// 显示重车发送成功
void uf_AEPrint_FULL_SUCC_Back(newgimis * pMe);

//显示打卡成功
void uf_AEPrint_YJDK_SUCC_Back(newgimis * pMe);

/*-------------------------------------------------------------------
以下与照片有关
-------------------------------------------------------------------*/
void newgimis_forphoto(newgimis * pMe,boolean bGet);
void newgimis_GetPhote(newgimis * pMe);


// 以下与视频有关
void newgimis_forAudio(newgimis * pMe,boolean bGet);


void uf_forphoto(newgimis * pMe,boolean bGet);
// 把照片显示在屏幕上，供用户确认
void uf_SetPhote(newgimis * pMe);
// 找到立即发送的那张照片, 找到后放在pMe->imgidx中, 如果是立即发送照片则立即发送
void uf_GetNewestPhotoForSend(newgimis * pMe);   //uf_GetPhotoForSendImmediately
// 显示文本框和选择照片菜单
void uf_ShowText(newgimis * pMe);
// 释放与文本输入有关的控件
void uf_ReleaseCtl(newgimis * pMe,boolean bClean);


void uf_GetDate(newgimis * pMe,uint32 dwCreationDate,char* strDateTime,int iStrLen,char cType);

// 测试输入模式，并显示在屏幕上
boolean uf_TextModechange(newgimis *pMe);
// 显示屏幕上显示当前输入法
void uf_ShowInputMode(newgimis * pMe,AEETextInputMode eInputMode);

// 停止振铃  Add by zjie 2011-10-26
void uf_StopBeep(newgimis *pMe);



// 在发送照片时，在屏幕显示图片“发送中。。”
void uf_DrawSendPic(newgimis *pMe,IImage *pIImage, AEEImageInfo * pi, int nErr );

// 复制文件
void uf_CopyFile(newgimis * pMe,IFileMgr* pFileMgr,char *pStr_SrcName,char *pStr_DstName);

// 显示汉字
void uf_AEPrint(newgimis * pMe,boolean bCallBack);

// 显示汉字没有视频要显示
void uf_AEPrint_Back(newgimis * pMe,boolean bCallBack);

// 显示汉字没有照片要显示
void uf_AEPrintPhoto_Back(newgimis * pMe,boolean bCallBack);
// 照片功能未开启
void uf_AEPrint_Photo_Back(newgimis * pMe,boolean bCallBack);

// 照片处理结束后启动清屏
void uf_Afterpost(newgimis *pMe);


//后台转前台的动作 Add by zjie 2011-11-18
boolean uf_ForgeGroundEvt(newgimis *pMe);		// void 

// 在屏幕上显示双字节信息
void gimis_WPrintf( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *szText);


// Add by zjie 2012-09-21, 由gm_photo.c移来此处 
//   在发送文件时，在屏幕显示图片“发送中。。”Add by zjie 2012-05-29
void uf_ShowSendPic(newgimis *pMe);

// uf_ReturnMenu 返回主菜单 回调函数
void uf_ReturnMenu(newgimis * pMe);

// uf_ReturnMenu 返回更多菜单 回调函数
void uf_ReturnMenu_Two(newgimis * pMe);


// 与一键考勤有关的
// 显示汉字没有GPS数据
void uf_AEPrint_YJKQ_Back(newgimis * pMe,boolean bCallBack);

// 显示汉字没有GPS数据  第二个界面
void uf_AEPrint_YJKQ_Back_Two(newgimis * pMe,boolean bCallBack);

// 显示汉字没有GPS数据  第二个界面
void uf_AEPrint_RWLB_Back_Two(newgimis * pMe,boolean bCallBack);

// 显示汉字没有GPS数据  第一个界面
void uf_AEPrint_YJDK_Back(newgimis * pMe,boolean bCallBack);

// 显示没有该功能 
void without_this_feature(newgimis * pMe,boolean bCallBack);


// 显示发送中
void uf_AEPrint_Sending(newgimis * pMe,boolean bCallBack);

//显示获取详情中
void uf_AEPrint_xq(newgimis * pMe,boolean bCallBack);

// 显示汉字无网络
void uf_AEPrint_NO_NET_Back(newgimis * pMe,boolean bCallBack);

// 显示汉字无网络  返回更多的界面
void uf_AEPrint_NO_NET_Back_Two(newgimis * pMe,boolean bCallBack);

// 与视频有关
// 照片功能未开启
void uf_AEPrint_Audio_Back(newgimis * pMe,boolean bCallBack);
void newgimis_GetAudio(newgimis * pMe);



//开启 / 关闭GPS定位
void newgimis_OpenGPS(newgimis * pMe);
void newgimis_CloseGPS(newgimis * pMe);

// 临时 开启 和 关闭 GPS定位
void newgimis_OpenGPS_tmp(newgimis * pMe);
void newgimis_CloseGPS_tmp(newgimis * pMe);


// 把字符串中原字符转换为转义字符
void uf_toESC(char *rStr_source,char *rStr_Esc);
// 把字符串中转义字符转换为原字符
// 参数说明 第一个参数是还没有转换的  第二个参数是转换后的
void uf_fromESC(char *rStr_Esc,char *rStr_source);

// 在字符串中替换子字符串
void uf_trfESC(char *rStr_source,char **rStr_Esc,const char *vStr_sESC, const char *vStr_tESC);


// 以下是在唤醒时调用的界面  以下的返回值是 1 -- 代表成功绘制  0 -- 则没有找到控件指针 没有绘制
// 显示主界面
int Show_Main_Fun(newgimis *pnewgimis);
// 显示查看GPS配置界面
int Show_GPS_Fun(newgimis *pnewgimis);
// 显示照片界面
int Show_Photo_Fun(newgimis *pnewgimis);
// 显示视频界面
int Show_Audio_Fun(newgimis *pnewgimis);

// 显示视频 挂起界面
int Show_Audio_Fun_Sup(newgimis *pnewgimis);

// 显示更多的那个界面
int Show_MorePag_Fun(newgimis *pnewgimis);

//显示后台任务提醒界面
int show_Notify(newgimis *pnewgimis);

//显示任务列表界面
int Show_Task_List(newgimis *pnewgimis);

//显示任务流程详情界面
int Show_Task_Detail1(newgimis *pnewgimis);

//显示任务内容界面
int Show_Task_Content(newgimis *pnewgimis);

//显示任务非流程详情界面
int Show_Task_Detail2(newgimis *pnewgimis);

// 按组合键退出程序
boolean newgimis_ClosseApplet( newgimis *pMe );

// 关闭GPS，退出程序
void newgimis_ufStopApp( newgimis *pMe );


void Save_GPSData(newgimis *pMe);

void Save_GPSData_had(newgimis *pMe);  //在已经有离线数据的情况下继续保存离线数据

void Save_GPSData_All(newgimis *pMe);



//用post方式发送离线数据
void newgimis_PostOffData(newgimis * pMe);

int Get_LX_GPSData(newgimis *pMe);  //获取离线数据 成功 返回 > 0  失败返回 0

// 删除离线考勤文件
int uf_DelOff_File(newgimis *pMe);

//   测试用，删除配置文件
int uf_Del_ConfigFile(newgimis *pMe);

// 处理注册的按键	Add by zjie 2011-11-18
static boolean gimis_NotifyShellKey(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);


// 监控HTTP是否超时
void uf_MonitorPost(newgimis *pMe);	 // 请求授权的时候进来这个参数 说明 不能和平台交互 所有的数据都是离线的 定位也只能是卫星定位

// 监控HTTP是否超时
void uf_MonitorPost_xt(newgimis *pMe);	 // 请求授权的时候进来这个参数 说明 不能和平台交互 所有的数据都是离线的 定位也只能是卫星定位

// 监控HTTP是否超时
void uf_MonitorPost_E(newgimis *pMe);	 // 请求授权的时候进来这个参数 说明 不能和平台交互 所有的数据都是离线的 定位也只能是卫星定位


// 监控HTTP是否超时 一键考勤 和 状态
void uf_MonitorPost_kq_zt(newgimis *pMe);	 // 请求授权的时候进来这个参数 说明 不能和平台交互 所有的数据都是离线的 定位也只能是卫星定位



// 监控HTTP是否超时
void uf_MonitorPost_gps(newgimis *pMe);	 // 请求授权的时候进来这个参数 说明 不能和平台交互 所有的数据都是离线的 定位也只能是卫星定位


// 判断有没有日志数据 
/*
	返回值 0 -- 表示有这个文件且里面有内容  > 0 说明返回错误
*/
uint32 juage_logfile( newgimis *pMe ); //判断有没有日志数据


//  获取发送的模式
void Send_PM(newgimis *pMe, int pi);


/////////////  与 GPS定位 优化有关的  START             //////////////
// uf_Pos_Track_stopstart  GPS定位停止后开始  这个是用于超时的时候
void uf_Pos_Track_stopstart(newgimis * pMe);  

// 这个是用于已经定位到要切换到主模式的
void uf_Pos_Track_stopstart_has(newgimis * pMe);  

// 在接收到 100004 的时候 重新关闭请求 然后再开启请求
void uf_Pos_Track_stopstart_error(newgimis * pMe);  

// 在主模式下 接收到 100004 的时候 则定时 10分钟后再次尝试主模式能否定位到
void uf_Pos_Track_stopstart_time(newgimis * pMe);  
//////////////////                    END               //////////////



// 2分钟后重启GPS
 void GPS_Restart_fun(newgimis *pMe);

//void GPS_Restart_fun(void *pMe);

// 加载图片内容
void Load_Image_fun(newgimis *pMe);	 // 



// Track_cbInterval_copy  GPS定位停止后开始  这个是用于超时的时候
void Track_cbInterval_copy(void * pMe);  

void Track_Notify_Copy(void *pts);

// 定时进来继续开启GPS 定位
void TimetoTrack_cbInterval(void *pts);

// 判断是否发送 myiweb_start_xt 或者取消发送   1 - 表示发送   0 - 表示不发送
int judgeSendorCancel(newgimis *pMe); 

void judge_reset_TB(newgimis *pMe); // 判断是否需要重启 用于重启时段


/********************************************************************************************
 在本地记录日志
传入参数
  cgimis * pMe
  IShell *pShell  :  
  cWrtLog         :   1  写入日志， 非1 仅供qxdm调试
  int *pIndex		  :   计数器，表示日志中程序开始执行的位置，由调用的程序保存此值
  const char *szFormat, ...  ： 需记录的字符串
********************************************************************************************/
void ud_RecLog_c(IShell *pShell,char cWrtLog, uint32 *pIndex, const char *szFormat, ... );



void IPOSDET_Release_fun(newgimis *pMe);

// 发送短信重启手机指令
void uf_RestartPhone(newgimis *pMe);

// 判断GPSmode 失败是 纯基站 纯卫星 纯网络 基站辅助 
//  返回值 如果 == TRUE 是    == FALSE  不是 
int Judge_GPS_Mode(newgimis *pMe);

//  获取GPS对应的模式
int Get_GPS_Mode(newgimis *pMe);


//////////  一下是通讯录的     /////////
// 用于检查中心端指令的函数
char *uf_STRSTR(const char *pszHaystack, const char *pszNeedle, const char *sSplit );

int addbook_init(newgimis *pMe);
int addbook_free(newgimis *pMe);
int Add_addbook(newgimis *pMe);
int Del_addbook(newgimis *pMe);
int Del_all_addbook(newgimis *pMe);


//////////   以下是QCHAT 对讲相关的函数  ////////////////
void newgimis_WaitForQChat(newgimis *pMe);

void call_richptt_PttCall(newgimis *pMe);


//////  以下与任务有关的 开始 ///////
extern void *testgps_GetScreenData_task( newgimis *pMe );
extern void *testgps_SetScreenData_task( newgimis *pMe, void *screenData );

//发送离线GPS数据 //2015-12-18 修改
boolean Send_Offline_GPSData( newgimis *pMe);
void myiweb_start_E_cb(void *p);


void newgimis_NotifyShellKey_33( newgimis *pMe ) ; // 接收长按键触发 手机标识码为 33 的 它只会触发三次消息 257 256 258 不像别的手机会一直触发256
boolean InputMode33(newgimis *pMe, ITextCtl	*textCtl,AEEEvent eCode, uint16 wParam, uint32 dwParam );
void Timer_Close_BackLight(newgimis *pMe);

//北斗定位

int newgimis_Init_UKBD(newgimis * pMe);
void newgimis_OpenGPS_UKBD(newgimis * pMe);
void newgimis_CloseGPS_UKBD(newgimis * pMe);

boolean SendBDData(newgimis *pMe);
boolean SendErrorBDData( newgimis *pMe );

//自动上传录音
int newgimis_GetRecordInfo_2(newgimis* pMe);
void newgimis_GetRecordInfo(newgimis* pMe);
void newgimis_GetRecord(newgimis* pMe);

//s958GPS
int newgimis_Init_S958GPS(newgimis * pMe);
void newgimis_OpenGPS_S958GPS(newgimis * pMe);
void newgimis_CloseGPS_S958GPS(newgimis * pMe);

//恢复HTTP 关于wait的标志位

void uf_MonitorWait_HTTP(newgimis *pMe);
void uf_MonitorWait_GPS(newgimis *pMe);
void uf_MonitorWait_E(newgimis *pMe);

//队列  
void initQueue(newgimis *pMe);                 //队列初始化的函数  
int isEmpty(newgimis *pMe);                   //判断队列是否为空的函数  
int isFull(newgimis *pMe);                    //判断队列是否满的函数  
int inQueue(newgimis *pMe, YYXF_INFO t_info);        //入队的函数   
int outQueue(newgimis *pMe, YYXF_INFO* t_info);    //出队的函数,同时保存出队的元素  
/*void traverseQueue(QUEUE * pQueue);*/



#endif
