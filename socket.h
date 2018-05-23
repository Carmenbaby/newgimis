#ifndef SOCKET_H
#define SOCKET_H

#include "newgimis.h"
#define PACKET_TCP_ID_HEAD "CARI"
//tcp包
typedef struct 
{
	char  heater[4];	//数据头 "CARI"
	char  type[4];     // 1 心跳包.数据包
	int   Length;	//数据长度，包含 包头长度 + 数据长度
}TCP_HEAD;


//UDP包
typedef struct 
{
	int cPageType;//包类型
	int index; //传送当前的包
	int total; //总包数
	int bsize; //该次发送内容长度大小
	char buffer[4096]; //传输内容
}UDP_INFO;


//TCP
void Scoket_init_TCP(newgimis *pMe);	//初始化TCP资源设置以及connect操作
// 
void Socket_ConnectCB_TCP(void* p,int error);	//connect连接后的回调根据回调通知结果操作。
// 
void Socket_ReadCB(void *p);	//tcp接收操作。
void Socket_WriteCB(void *p);

void Socket_Write_Answer_CB(void *p);//应答包

void Socket_Listen_Heart(void *p);
// 
void Socket_Clean_TCP(newgimis *pMe);	//清除TCP资源。


void initModule_yyxf(newgimis *pMe);
void coreModule_yyxf(newgimis *pMe);

void setTimerHttp_yyxf(newgimis *pMe);
//http
void myiweb_start_yyxf(newgimis *pMe, char *pszUrl,int iPage);
static void myiweb_GotResp_yyxf(void *p);
static void myiweb_ReadBody_yyxf(newgimis *pMe);
static void myiweb_Header_yyxf(void *p, const char *cpszName, GetLine *pglVal);
static void myiweb_Status_yyxf(void *p, WebStatus ws, void *pVal);
static void uf_MonitorPost_yyxf(newgimis *pMe);
static void myiweb_Stop_yyxf(newgimis *pwa);
	
//UDP
void Scoket_init_UDP(newgimis *pMe);   //初始化UDP资源设置以及发送请求命令

static void Socket_sendPack_UDP(void *p);	//UDP发送操作

static void Socket_readPack_UDP(void *p);	//UDP接收操作

void Socket_Clean_UDP(newgimis *pMe);      //清除UDP资源。

void write_UDP_Date(newgimis *pMe, char *strData,int ilen);	//UDP包写入文件操作

void write_udp_thread(newgimis *pMe);	//遍历UDP缓存包-写入文件

void SoundPlay_YYXF(newgimis* pMe);//播放

void SoundStatus_YYXF(newgimis* pMe);

void CBSoundPlay_YYXF(void *p,AEESoundPlayerCmd eCBType,AEESoundPlayerStatus eSPStatus,uint32 dwParam);//控制播放回调

void removeFile_YYXF(newgimis *pMe,char *filename);

int SMSInit_YYXF(newgimis *pMe);
void SmsSave_YYXF(newgimis * pMe);
void SMSCB_YYXF(newgimis * pMe);

void readOvertime_yyxf(newgimis *pMe);

void Settimer_ReadUdpData(newgimis *pMe);


#endif

