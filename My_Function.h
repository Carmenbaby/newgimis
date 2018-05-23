#ifndef _MY_FUNCTION_H_
#define _MY_FUNCTION_H_

#include "newgimis.h"



#define OFFLINE_GPS_DATA				 "offline_gps.txt"

//�ҵ���־
#define  MAX_MESSAGE_LEN				1024						// ��־ÿ����¼����󳤶�
//#define  MAX_SCREEN_CHARS	50										// ��Ļ��ÿ�������ʾ���ַ���

//�ļ�
#define FILE_NAME						"OfflineData_gps.txt"
//#define OFFLINEDATA_GPS_SAVE_MAX		5							//gps�������ݴ洢�������ֵ(2-999)

//����
#define TASK_COUNT_EVERYPAGE			5							//������ѯ�����б� Ĭ��ÿҳ��ʾ5��

//help function
#define MAXCOLUMNS    22            // ÿ�����22���ַ�

//�������ݱ��������


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
// ����ļ������Ƿ�ʱ
void Upload_MonitorSendFile_7Day(newgimis *pMe);  // uf_MonitorSendFile(FileUpload *me);
// ������ɺ�Ķ���
void Upload_Afterpost_7Day(newgimis *pMe);

void send_GPSData_7Day(newgimis *pMe);

//�����������gps���ݵ�Ŀ¼
void init_gpsDir(newgimis *pMe);

//��ʼ����ʱ�򣬼��gpsData�ļ�
void init_check_gpsData(newgimis *pMe);
///////////////////////////////////////////////////////////////////////////
//ͨ��ɾ���ļ��������ļ���
void Delete_file(newgimis *pMe,char* file_name);

////////////////////////////////GPS�������ݰ�/////////////////////////////////////////////////////////////
//д���ļ�
void File_Write_OfflineData_Gps(newgimis *pMe, char* strData);
//��ȡ�ļ�����
int File_Get_OfflineData_Gps(newgimis *pMe, char **strData);




//help function
int My_StrTOint(const char *str);
void My_Log(IShell *pShell,char cWrtLog, uint16 *pIndex, const char *szFormat, ... );
boolean MP_AddMenuItem(IMenuCtl * pMenu, uint16 wTextID, AECHAR * pText, uint16 wImageID, uint16 wItemID, uint32 dwData);
// ��ʾ��Ļ����ʾ��ǰ���뷨��Ϣ
void yb_ShowInputMode(newgimis * pMe, AEETextInputMode eInputMode, int iMode);
// ����Ļ����ʾ˫�ֽ���Ϣ
void yb_WPrintf( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *szText);
//newgimis_InitAppData 
//ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)TimetoTrack_cbInterval,s_pts);

//Send_Offline_GPSData
	//myiweb_GotResp_E

//XTRA_EVT_USER_XT_CG
		//������������
//SendGPSData(pMe);
//static void myiweb_GotResp_gps(void *p) 
//SendErrorGPSData_0
//SendErrorGPSData
	//myiweb_GotResp_E
//ISHELL_SetTimer(
//bug
//��ȡ������ȷ�� ���ܱ�־��ȡ��
//��ʼ��ֵ init���������ԣ�
//��Ȩ��δ��Ȩ myiweb_GotResp_E��رճ��� ���ܷ������߰���
//stop iweb  mutex = 0
//�ļ�����ͬʱ��
#endif
