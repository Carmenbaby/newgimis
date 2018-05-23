#ifndef _MY_FUNCTION_H_
#define _MY_FUNCTION_H_

#include "newgimis.h"



#define OFFLINE_GPS_DATA				 "offline_gps.txt"

//我的日志
#define  MAX_MESSAGE_LEN				1024						// 日志每条记录的最大长度
//#define  MAX_SCREEN_CHARS	50										// 屏幕上每次最多显示的字符数

//文件
#define FILE_NAME						"OfflineData_gps.txt"
//#define OFFLINEDATA_GPS_SAVE_MAX		5							//gps离线数据存储条数最大值(2-999)

//任务
#define TASK_COUNT_EVERYPAGE			5							//开机查询任务列表 默认每页显示5个

//help function
#define MAXCOLUMNS    22            // 每行最多22个字符

//离线数据保存的天数


//////////////////////////////////////////////////////////////////////////

typedef struct
{
	int year;
	int month;
	int day;

}calDate;

uint16 Leap_Year(uint16 m_Year);
uint16 Judge(calDate d1);
void Now_To_Fronter(char *file_name,JulianType m_Date, int gap);

void delete_overGpsData(newgimis* pMe);

void write_GPSData_7Day(newgimis *pMe, char *strData);

int Upload_Init_7Day(newgimis *pMe);

void Upload_Start_7Day(newgimis* pMe);
void Upload_GotResp_7Day(void *p);		
void Upload_Status_7Day(void *p, WebStatus ws, void *pVal);
void Upload_ReadBody_7Day(newgimis *pMe);
void Upload_Stop_7Day(newgimis *pMe);
void Upload_Header_7Day(void *p, const char *cpszName, GetLine *pglVal);


void uf_MonitorPost_7Day(newgimis *pMe);
// 监控文件发送是否超时
void Upload_MonitorSendFile_7Day(newgimis *pMe);  // uf_MonitorSendFile(FileUpload *me);
// 传送完成后的动作
void Upload_Afterpost_7Day(newgimis *pMe);

void send_GPSData_7Day(newgimis *pMe);

//创建存放离线gps数据的目录
void init_gpsDir(newgimis *pMe);

//初始化的时候，检查gpsData文件
void init_check_gpsData(newgimis *pMe);
///////////////////////////////////////////////////////////////////////////
//通用删除文件，根据文件名
void Delete_file(newgimis *pMe,char* file_name);

////////////////////////////////GPS离线数据包/////////////////////////////////////////////////////////////
//写入文件
void File_Write_OfflineData_Gps(newgimis *pMe, char* strData);
//读取文件发送
int File_Get_OfflineData_Gps(newgimis *pMe, char **strData);




//help function
int My_StrTOint(const char *str);
void My_Log(IShell *pShell,char cWrtLog, uint16 *pIndex, const char *szFormat, ... );
boolean MP_AddMenuItem(IMenuCtl * pMenu, uint16 wTextID, AECHAR * pText, uint16 wImageID, uint16 wItemID, uint32 dwData);
// 显示屏幕上显示当前输入法信息
void yb_ShowInputMode(newgimis * pMe, AEETextInputMode eInputMode, int iMode);
// 在屏幕上显示双字节信息
void yb_WPrintf( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *szText);
//newgimis_InitAppData 
//ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)TimetoTrack_cbInterval,s_pts);

//Send_Offline_GPSData
	//myiweb_GotResp_E

//XTRA_EVT_USER_XT_CG
		//发送离线数据
//SendGPSData(pMe);
//static void myiweb_GotResp_gps(void *p) 
//SendErrorGPSData_0
//SendErrorGPSData
	//myiweb_GotResp_E
//ISHELL_SetTimer(
//bug
//读取数据正确了 才能标志读取了
//初始化值 init里面做可以？
//授权到未授权 myiweb_GotResp_E会关闭程序 还能发送离线包吗？
//stop iweb  mutex = 0
//文件不能同时打开
#endif
