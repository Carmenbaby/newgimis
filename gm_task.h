#ifndef GM_TASK_H
#define GM_TASK_H

//  ========== �йش��� ==========
#include "AEESio.h"							
#include "AEEPort.h"
typedef struct _newgimis newgimis;


#define LINEHEIGHT   16	
#define MENULIST_HEIGHT 2

#define  CONTENT_WITH   20						// ���ݵ�����



// �����ɷ��ֶ��б�
typedef struct  
{
	char	*pTaskId;					// �����ɷ�ID
	char	*pTaskName;					// �������
	char    *pTaskStatus;				// ����״̬
	uint32  nDateTime;					// �ɷ�ʱ��, 1980 �� 1 �� 6 ����ҹ (GMT) ���������
} ITaskFieldA;

// �����ɷ���¼�б�
typedef struct  
{
	uint16 			nRecordCount;								// ��¼������
	uint16 			nSelRec;									// ѡ�е������, nSelRec >= nRecordCount δѡ��
	ITaskFieldA		*pTaskFieldA;								// ��¼
} ITaskRecordsA;

// ��������-������
typedef struct  
{
	char	*pFeedbackId;				// ������ID
	char	*pFeedbackName;				// ����������
	uint16  nSequence;					// ������˳��
} ITaskFieldB;

// ���������¼
typedef struct  
{
	char		*pTaskId;					// �����ɷ�ID
	char		*pTaskName;					// �������
	char		*pTaskContent;				// ��������
	char		*pTaskStatus;				// ����״̬
	char		exp[512];					// ��ע
	boolean		bFlow;						// �Ƿ�����	
	uint16		nFeedbackCount;				// ���������
	ITaskFieldB *pTaskFieldB;				// ������
	uint16      nSelRec;					// ѡ�еķ�����, ���������������ܴ���TASK_DETAIL_RETURN,
											// ����TASK_DETAIL_FIRSTʱ�� �� TASK_DETAIL_FIRST >= 0 ����ѡ�е���, =nFeedbackCount �������

	//2016-1-11���
	//## Ҫ�ͷ���Դ ##
	AECHAR  *aeStr;							// ## Ҫ�ͷ���Դ ## ������ʾ�ľ�̬�ı����ݣ����� ���� ״̬�� 
} ITaskRecordB;





struct List_T
{
	void *pData;
	struct List_T *pNext;
};

typedef struct List_T LIST_T;
typedef LIST_T *pList;
#define LEN sizeof(LIST_T)





typedef struct _cTaskApp
{	
	
	void   *m_pParent;						// 
	char	cTaskStatus;					// ����Ĵ���״̬���������궨��, ��1λ�ĺ���  0 ����, 1 ���ڻ�ȡ�����ɷ��б�
											// 2 : ���ݷֽ⵽�����б�  3 ��ʾ�����ɷ�����, 4 ������������, 5 ���ݷֽ⵽��������
											// 6 : ��ʾ�����������, 7 �����Ķ˷����û�ѡ�����������
											// ��2λ�����壺1: ��power��ת����̨�� 2 EVT_APP_SUSPEND ������̨, 3 �� 1# �˳�

	uint32	nPassTm;						// ���Ƴ�ʱ
	int     nPageCount;						// ÿҳ��ʾ�����������б�
// 2014-01-04 ����û��	int     nTaskCount;						// ��������
	
	// ��HTTP�й�
	char	sUrl[250];						// http�ĵ�ַ
	IWeb*        pWeb;
	AEECallback  webcb;
	IWebResp *   pWebResp;					// the answer I get from IWeb
	ISource*     pSource;					// the body of the response		�����Ҳ��	IGetLine *   piGetLine;				// the body of the response, if any
	char		 cTimeout;					// http ��ʱ��ʾ, 0 û�г�ʱ,   1 ��ʱ	
	char         *pRecData;					//  �����յ�������
	int          nBytes;					// count of bytes in response
	int			 nBodyHasRead;				// ͨ���ص������Ѿ���ȡ�����ݳ���


	AEECallback cbGen;						// ͨ�ûص�, �����ڴ��еĹ���
	IImage	*pImage;						// ͼ����Դ

	ITaskRecordsA   *pTaskRecsA;			// �����ɷ���¼
	IMenuCtl		*pMenuTask;				// �����ɷ��˵�
	ITaskRecordB    *pTaskRecB;				// ��������
	IMenuCtl		*pMenuList;				// ���������б�

	IMenuCtl		*pMenuList_T;			//��������
	IMenuCtl		*pMenuDetail;			// ������������˵�
	ITextCtl		*pTxtExp;				// ��ע
	AEERect			rectTxt;				// ��ע�ؼ�����ʾλ��

	pList p_head; // �����������ͷ��ָ��


	// �������HTTP�й�
	char	sUrl_xq[250];						// http�ĵ�ַ
	IWeb*        pWeb_xq;
	AEECallback  webcb_xq;
	IWebResp *   pWebResp_xq;					// the answer I get from IWeb
	ISource*     pSource_xq;					// the body of the response		�����Ҳ��	IGetLine *   piGetLine;				// the body of the response, if any
	char		 cTimeout_xq;					// http ��ʱ��ʾ, 0 û�г�ʱ,   1 ��ʱ	
	char         *pRecData_xq;					//  �����յ�������
	int          nBytes_xq;					// count of bytes in response
	int			 nBodyHasRead_xq;				// ͨ���ص������Ѿ���ȡ�����ݳ���

	AEECallback cbGen_xq;						// ͨ�ûص�, �����ڴ��еĹ���


	// �������HTTP�й�
	char	sUrl_fk[250];						// http�ĵ�ַ
	IWeb*        pWeb_fk;
	AEECallback  webcb_fk;
	IWebResp *   pWebResp_fk;					// the answer I get from IWeb
	ISource*     pSource_fk;					// the body of the response		�����Ҳ��	IGetLine *   piGetLine;				// the body of the response, if any
	char		 cTimeout_fk;					// http ��ʱ��ʾ, 0 û�г�ʱ,   1 ��ʱ	
	char         *pRecData_fk;					//  �����յ�������
	int          nBytes_fk;					// count of bytes in response
	int			 nBodyHasRead_fk;				// ͨ���ص������Ѿ���ȡ�����ݳ���

	AEECallback cbGen_fk;						// ͨ�ûص�, �����ڴ��еĹ���

	int FinshorXQ; //�����������ɻ������鷴�� == 1 �������鷴��  == 2 �������
	//##����
	int m_nState;    //�������ͺ󣬷��صļ������   1-�ɹ����յ���������  

	IStatic *m_pIStatic;

	AECHAR  pContentText[606];/*202*///1616


	IPeek        *pipPostData;				// POST stream, 
	char         *pszPostData;				// POST string
	int32		 iFileRead;					// post len

} cTaskApp;



// ========================= �йع����ĺ��� ========================
boolean cTaskApp_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);




pList LIST_Init(void *pData);

pList LIST_Add(pList head,void *pData);
pList LIST_Add_front(pList head,void *pData);   //�������ͷ�����

pList LIST_Add_Head(pList head,void *pData);

int LIST_Print(pList head,void (*pFun)(void *pData));

pList LIST_Del(pList head,char cNum[],int (*STAFF_Del)(void *,char *));

int LIST_Free(pList head);

int LIST_iCount(pList head);

pList LIST_GetNode(pList head,int nod);			//���ز�ѯ�Ľڵ�);

int LIST_Print1(pList head,char cNum[],void (*pFun)(void *pData));  //��ӡͨ������ƥ��������

int LIST_Print2(pList head,char cName,void (*pFun)(void *pData));  //��ӡͨ������������(�����ַ�)

int LIST_Print3(pList head,void (*pFun)(void *pData,pList head2),pList head1,pList head2);  

pList LIST_Change(pList head,char cNum[],char cState,int (*Table_Change)(void *,char *,char));  //�ı�cState ��״̬

int LIST_RealDel2(pList head,int (*ORDERS_RealDel)(void *));  //������ɾ��

int LIST_Print4(char acData1[],char acData2[],pList head,void (*pFun)(char acData1[],char acData2[],void *pData));  

pList LIST_GetNode1(pList head,int iNum,int (*pNode)(void *,int));		

pList LIST_Find(pList head,char *pTaskID); // 

int LIST_Toatal(pList head); // ���ص�ǰ�ܵļ�¼�� Ҳ����������

int LIST_GetTaskID(newgimis *pMe, pList head , int nid); // ��ȡ��nid���е�TaskID

int LIST_RealDel(newgimis *pMe,pList head,char *pTaskID);  //������ɾ��



// �����Ķ˽����������Ϣ(�����ɷ����������顢���鷴��)
void cTaskApp_TaskInteract(cTaskApp *me);

// �����������й�
void cTaskApp_TaskInteract_xq(cTaskApp *me);

// �����뷴���й�
void cTaskApp_TaskInteract_fk(cTaskApp *me);

// ��ʾ asc
AEERect Task_Print_asc( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, const char *szFormat, ... );
// ��ʾunicode��Ϣ
AEERect Task_Print_unicode( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *asText);

//UTF8TOWSTR
void  UTF8_WCHAR( newgimis *pMe,const char *szFormat,AECHAR *szAechar);


/////////////////////////////////###����###2015-12-28/////////////////////////////////
void Display_Success(newgimis *pMe);
void uf_ReturnMenu_xq(newgimis * pMe);
void Display_false(newgimis *me)  ;
void Display_timeout(newgimis *me) ;

void Display_fkweb_error(newgimis *me);
void Display_getTaskErr(newgimis *me) ;
void Display_getTaskTimeout(newgimis *me);
void uf_ReturnMenu_TaskList(newgimis * pMe);
void uf_ReturnMenu_gd(newgimis * pMe);
void Display_NetworkErr(newgimis *me) ;
//���񲿷֣�ÿ���ػ���Ļ
void Task_RedrawScreen(newgimis *pMe);
//f������
void uf_Return_Task2_xq(newgimis * pMe);// �����ɹ������ط������������

//#��̨ģʽ��������#	��̨����ʱ���и������񣬵�����ʾ�Ƿ�ʼ������
boolean SamplePosDet_TaskNotify_HandleEvent( newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam );

void Timer_Task_Notify(newgimis *pMe);
#endif