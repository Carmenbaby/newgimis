#ifndef GM_TASK_H
#define GM_TASK_H

//  ========== 有关串口 ==========
#include "AEESio.h"							
#include "AEEPort.h"
typedef struct _newgimis newgimis;


#define LINEHEIGHT   16	
#define MENULIST_HEIGHT 2

#define  CONTENT_WITH   20						// 内容的留边



// 任务派发字段列表
typedef struct  
{
	char	*pTaskId;					// 任务派发ID
	char	*pTaskName;					// 任务标题
	char    *pTaskStatus;				// 任务状态
	uint32  nDateTime;					// 派发时间, 1980 年 1 月 6 日午夜 (GMT) 至今的秒数
} ITaskFieldA;

// 任务派发记录列表
typedef struct  
{
	uint16 			nRecordCount;								// 记录的总数
	uint16 			nSelRec;									// 选中的任务号, nSelRec >= nRecordCount 未选中
	ITaskFieldA		*pTaskFieldA;								// 记录
} ITaskRecordsA;

// 任务详情-反馈项
typedef struct  
{
	char	*pFeedbackId;				// 反馈项ID
	char	*pFeedbackName;				// 反馈项名称
	uint16  nSequence;					// 反馈项顺序
} ITaskFieldB;

// 任务详情记录
typedef struct  
{
	char		*pTaskId;					// 任务派发ID
	char		*pTaskName;					// 任务标题
	char		*pTaskContent;				// 任务内容
	char		*pTaskStatus;				// 任务状态
	char		exp[512];					// 备注
	boolean		bFlow;						// 是否流程	
	uint16		nFeedbackCount;				// 反馈项个数
	ITaskFieldB *pTaskFieldB;				// 反馈项
	uint16      nSelRec;					// 选中的反馈项, 反馈项索引不可能大于TASK_DETAIL_RETURN,
											// 大于TASK_DETAIL_FIRST时， 减 TASK_DETAIL_FIRST >= 0 才是选中的项, =nFeedbackCount 代表完成

	//2016-1-11添加
	//## 要释放资源 ##
	AECHAR  *aeStr;							// ## 要释放资源 ## 保存显示的静态文本内容（标题 内容 状态） 
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
	char	cTaskStatus;					// 任务的处理状态，基数见宏定义, 第1位的含义  0 空闲, 1 正在获取任务派发列表
											// 2 : 数据分解到任务列表  3 显示任务派发界面, 4 下载任务详情, 5 数据分解到任务详情
											// 6 : 显示任务详情界面, 7 往中心端发送用户选择的任务详情
											// 第2位数含义：1: 按power键转到后台， 2 EVT_APP_SUSPEND 键到后台, 3 按 1# 退出

	uint32	nPassTm;						// 控制超时
	int     nPageCount;						// 每页显示多少条任务列表，
// 2014-01-04 好像没用	int     nTaskCount;						// 总任务数
	
	// 与HTTP有关
	char	sUrl[250];						// http的地址
	IWeb*        pWeb;
	AEECallback  webcb;
	IWebResp *   pWebResp;					// the answer I get from IWeb
	ISource*     pSource;					// the body of the response		用这个也行	IGetLine *   piGetLine;				// the body of the response, if any
	char		 cTimeout;					// http 超时标示, 0 没有超时,   1 超时	
	char         *pRecData;					//  保存收到的数据
	int          nBytes;					// count of bytes in response
	int			 nBodyHasRead;				// 通过回调函数已经读取的数据长度


	AEECallback cbGen;						// 通用回调, 适用于串行的工作
	IImage	*pImage;						// 图像资源

	ITaskRecordsA   *pTaskRecsA;			// 任务派发记录
	IMenuCtl		*pMenuTask;				// 任务派发菜单
	ITaskRecordB    *pTaskRecB;				// 任务详情
	IMenuCtl		*pMenuList;				// 任务详情列表

	IMenuCtl		*pMenuList_T;			//任务内容
	IMenuCtl		*pMenuDetail;			// 任务详情命令菜单
	ITextCtl		*pTxtExp;				// 备注
	AEERect			rectTxt;				// 备注控件的显示位置

	pList p_head; // 存放任务链表头部指针


	// 与详情的HTTP有关
	char	sUrl_xq[250];						// http的地址
	IWeb*        pWeb_xq;
	AEECallback  webcb_xq;
	IWebResp *   pWebResp_xq;					// the answer I get from IWeb
	ISource*     pSource_xq;					// the body of the response		用这个也行	IGetLine *   piGetLine;				// the body of the response, if any
	char		 cTimeout_xq;					// http 超时标示, 0 没有超时,   1 超时	
	char         *pRecData_xq;					//  保存收到的数据
	int          nBytes_xq;					// count of bytes in response
	int			 nBodyHasRead_xq;				// 通过回调函数已经读取的数据长度

	AEECallback cbGen_xq;						// 通用回调, 适用于串行的工作


	// 与详情的HTTP有关
	char	sUrl_fk[250];						// http的地址
	IWeb*        pWeb_fk;
	AEECallback  webcb_fk;
	IWebResp *   pWebResp_fk;					// the answer I get from IWeb
	ISource*     pSource_fk;					// the body of the response		用这个也行	IGetLine *   piGetLine;				// the body of the response, if any
	char		 cTimeout_fk;					// http 超时标示, 0 没有超时,   1 超时	
	char         *pRecData_fk;					//  保存收到的数据
	int          nBytes_fk;					// count of bytes in response
	int			 nBodyHasRead_fk;				// 通过回调函数已经读取的数据长度

	AEECallback cbGen_fk;						// 通用回调, 适用于串行的工作

	int FinshorXQ; //用来标记是完成还是详情反馈 == 1 则是详情反馈  == 2 则是完成
	//##新添
	int m_nState;    //子任务发送后，返回的几种情况   1-成功接收到返回数据  

	IStatic *m_pIStatic;

	AECHAR  pContentText[606];/*202*///1616


	IPeek        *pipPostData;				// POST stream, 
	char         *pszPostData;				// POST string
	int32		 iFileRead;					// post len

} cTaskApp;



// ========================= 有关公开的函数 ========================
boolean cTaskApp_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);




pList LIST_Init(void *pData);

pList LIST_Add(pList head,void *pData);
pList LIST_Add_front(pList head,void *pData);   //添加链表头后插入

pList LIST_Add_Head(pList head,void *pData);

int LIST_Print(pList head,void (*pFun)(void *pData));

pList LIST_Del(pList head,char cNum[],int (*STAFF_Del)(void *,char *));

int LIST_Free(pList head);

int LIST_iCount(pList head);

pList LIST_GetNode(pList head,int nod);			//返回查询的节点);

int LIST_Print1(pList head,char cNum[],void (*pFun)(void *pData));  //打印通用链表匹配数据域

int LIST_Print2(pList head,char cName,void (*pFun)(void *pData));  //打印通用链表数据域(根据字符)

int LIST_Print3(pList head,void (*pFun)(void *pData,pList head2),pList head1,pList head2);  

pList LIST_Change(pList head,char cNum[],char cState,int (*Table_Change)(void *,char *,char));  //改变cState 的状态

int LIST_RealDel2(pList head,int (*ORDERS_RealDel)(void *));  //符串真删除

int LIST_Print4(char acData1[],char acData2[],pList head,void (*pFun)(char acData1[],char acData2[],void *pData));  

pList LIST_GetNode1(pList head,int iNum,int (*pNode)(void *,int));		

pList LIST_Find(pList head,char *pTaskID); // 

int LIST_Toatal(pList head); // 返回当前总的记录数 也就是任务数

int LIST_GetTaskID(newgimis *pMe, pList head , int nid); // 获取第nid项中的TaskID

int LIST_RealDel(newgimis *pMe,pList head,char *pTaskID);  //符串真删除



// 与中心端交互任务的信息(任务派发、任务详情、详情反馈)
void cTaskApp_TaskInteract(cTaskApp *me);

// 以下与详情有关
void cTaskApp_TaskInteract_xq(cTaskApp *me);

// 以下与反馈有关
void cTaskApp_TaskInteract_fk(cTaskApp *me);

// 显示 asc
AEERect Task_Print_asc( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, const char *szFormat, ... );
// 显示unicode信息
AEERect Task_Print_unicode( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *asText);

//UTF8TOWSTR
void  UTF8_WCHAR( newgimis *pMe,const char *szFormat,AECHAR *szAechar);


/////////////////////////////////###任务###2015-12-28/////////////////////////////////
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
//任务部分，每次重绘屏幕
void Task_RedrawScreen(newgimis *pMe);
//f非流程
void uf_Return_Task2_xq(newgimis * pMe);// 反馈成功，返回非流程详情界面

//#后台模式任务提醒#	后台运行时，有更新任务，弹窗提示是否开始做任务
boolean SamplePosDet_TaskNotify_HandleEvent( newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam );

void Timer_Task_Notify(newgimis *pMe);
#endif