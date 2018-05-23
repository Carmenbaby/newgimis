#ifndef SOCKET_H
#define SOCKET_H

#include "newgimis.h"
#define PACKET_TCP_ID_HEAD "CARI"
//tcp��
typedef struct 
{
	char  heater[4];	//����ͷ "CARI"
	char  type[4];     // 1 ������.���ݰ�
	int   Length;	//���ݳ��ȣ����� ��ͷ���� + ���ݳ���
}TCP_HEAD;


//UDP��
typedef struct 
{
	int cPageType;//������
	int index; //���͵�ǰ�İ�
	int total; //�ܰ���
	int bsize; //�ôη������ݳ��ȴ�С
	char buffer[4096]; //��������
}UDP_INFO;


//TCP
void Scoket_init_TCP(newgimis *pMe);	//��ʼ��TCP��Դ�����Լ�connect����
// 
void Socket_ConnectCB_TCP(void* p,int error);	//connect���Ӻ�Ļص����ݻص�֪ͨ���������
// 
void Socket_ReadCB(void *p);	//tcp���ղ�����
void Socket_WriteCB(void *p);

void Socket_Write_Answer_CB(void *p);//Ӧ���

void Socket_Listen_Heart(void *p);
// 
void Socket_Clean_TCP(newgimis *pMe);	//���TCP��Դ��


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
void Scoket_init_UDP(newgimis *pMe);   //��ʼ��UDP��Դ�����Լ�������������

static void Socket_sendPack_UDP(void *p);	//UDP���Ͳ���

static void Socket_readPack_UDP(void *p);	//UDP���ղ���

void Socket_Clean_UDP(newgimis *pMe);      //���UDP��Դ��

void write_UDP_Date(newgimis *pMe, char *strData,int ilen);	//UDP��д���ļ�����

void write_udp_thread(newgimis *pMe);	//����UDP�����-д���ļ�

void SoundPlay_YYXF(newgimis* pMe);//����

void SoundStatus_YYXF(newgimis* pMe);

void CBSoundPlay_YYXF(void *p,AEESoundPlayerCmd eCBType,AEESoundPlayerStatus eSPStatus,uint32 dwParam);//���Ʋ��Żص�

void removeFile_YYXF(newgimis *pMe,char *filename);

int SMSInit_YYXF(newgimis *pMe);
void SmsSave_YYXF(newgimis * pMe);
void SMSCB_YYXF(newgimis * pMe);

void readOvertime_yyxf(newgimis *pMe);

void Settimer_ReadUdpData(newgimis *pMe);


#endif

