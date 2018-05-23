/*===============================================================================
 处理有关任务
=============================================================================== */

#include "newgimis.bid"
#include "newgimis.h"

#include "gm_Task.h"
#include "gmTools.h"

/*
int m_Total ;//当前任务的总数量
int m_Page_c ;//当前任务需要的总页数
int m_Cur_Page;// 当前显示第几页
*/


// 任务派发菜单项中使用到的命令ID
typedef enum 
{
	TASK_ITEM_EXIT    = 100,			// 退出
	TASK_ITEM_FIRST						// 任务派发列表的第1个菜单项ID
}TaskCmdId;
// 任务详情菜单项中使用到的命令ID
typedef enum 
{
	TASK_DETAIL_RETURN    = 1000,			// 返回到任务派发
	TASK_DETAIL_SEND,					// 发送任务详情
	TASK_DETAIL_FINISH,					// 完成
	TASK_DETAIL_FIRST					// 任务派发列表的第1个菜单项ID
}DetailCmdId;

typedef enum 
{
	TASK_DISPLAY_CONTENT			// 点击显示内容

}DetailId;

#define CTL_TAB_LEFT                    0  // The wParam value of a left key press in an EVT_CTL_TAB event
#define CTL_TAB_RIGHT                   1  // The wParam value of a right key press in an EVT_CTL_TAB event

//任务提醒弹出界面
#define MENU_TASK_BEGIN					0
#define MENU_TASK_DELEY					1

// ==================== 有关宏 ============================================
// 设置铃声
#define cTaskApp_Alert(pMe) {	\
	ISHELL_Beep(pMe->a.m_pIShell, BEEP_VIBRATE_REMIND, TRUE);		\
	ISHELL_SetTimer(pMe->a.m_pIShell, 3 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*)pMe );	\
}

//设置HTTP status回调函数
static void cTaskApp_Status(void *p, WebStatus ws, void *pVal);

//设置HTTP_xq status回调函数
static void cTaskApp_Status_xq(void *p, WebStatus ws, void *pVal);

//设置HTTP_fk status回调函数
static void cTaskApp_Status_fk(void *p, WebStatus ws, void *pVal);

// 设置HTTP超时的回调函数
void cTaskApp_SetWebOverTimer(cTaskApp *me);

// 设置HTTP超时的回调函数
void cTaskApp_SetWebOverTimer_xq(cTaskApp *me);

// 设置HTTP超时的回调函数  反馈
void cTaskApp_SetWebOverTimer_fk(cTaskApp *me);

// 初始化
void cTaskApp_InitAppData(newgimis *pMe);

// 应答http返回
void cTaskApp_GotResp(void *p);

// 应答http返回
void cTaskApp_GotResp_xq(void *p);

// 应答http返回
void cTaskApp_GotResp_fk(void *p);

// 读取中心端返回任务列表
void cTaskApp_ReadTaskBody(cTaskApp *me);

// 读取中心端返回任务详情
void cTaskApp_ReadTaskBody_xq(cTaskApp *me);

// 读取中心端返回内容
void cTaskApp_ReadTaskBody_fk(cTaskApp *me);

// 销毁数据
void cTaskApp_FreeAppData(cTaskApp* me,char cMode);

// 释放m_TaskRecsA中资源
void cTaskApp_ReleaseMasterRec(cTaskApp *me);

// 释放m_TaskRecsB中资源
void cTaskApp_ReleaseDetailRec(cTaskApp *me);
// 停止的应用
void cTaskApp_StopApp(newgimis *pMe);

//  HTTP 接口有关的
void cTaskApp_CloseWeb(cTaskApp *me);

//  HTTP 接口有关的
void cTaskApp_CloseWeb_xq(cTaskApp *me);

//  HTTP 接口有关的 反馈
void cTaskApp_CloseWeb_fk(cTaskApp *me);

//解析任务列表的数据包结构
void cTaskApp_RecordsaFormat(cTaskApp *me, char *sBuf);

//解析任务详情的数据包结构
void cTaskApp_RecordsbFormat(cTaskApp *me, char *sBuf);

static int GetTotalPage(int task,int pagec); //获取当前总的页数


// static void    myiweb_Stop_Task(cTaskApp *me);


// =======================  通用 ==========================
// 从8位的时间戳(精度：毫秒)字符串转为北京日期时间和4位整数时间戳(精度：秒)
JulianType cTaskApp_DateFromInt64(char* sDateBuf, uint32 *rDateTime)
{
	JulianType dDate;
	int nContentLen = 8;
	uint64 nDateTime = 0;
	unsigned char usBuf[8] = {0};
	
	nContentLen = 8;
	MEMCPY(usBuf,sDateBuf,nContentLen);
	nDateTime =  ((uint64)usBuf[0])<<56|((uint64)usBuf[1])<<48|((uint64)usBuf[2])<<40|((uint64)usBuf[3])<<32|((uint64)usBuf[4])<<24|((uint64)usBuf[5])<<16|((uint64)usBuf[6])<<8|((uint64)usBuf[7]);
	*rDateTime = (uint32)(nDateTime /1000 - (3657*24*60*60)+8*60*60);
	GETJULIANDATE(*rDateTime, &dDate);

	return dDate;
}



pList LIST_Init(void *pData)   //初始化节点
{
	pList pNew;
	
	pNew = (pList) MALLOC(LEN);
	if (pNew == NULL)
	{
//		printf("Memory error!");
		return NULL;
	}
	MEMSET(pNew,0,LEN);
	pNew->pData = pData;
	return pNew;
}

pList LIST_Add(pList head,void *pData)   //添加通用链表节点 
{
	pList pNew;  //指向新节点
	pList pTmp;
	if (head == NULL || pData == NULL)
	{
//		printf("参数错误！\n");
		return NULL;
	}
	pNew = LIST_Init(pData);  //创建新节点
	pTmp = head;
	while (pTmp ->pNext != NULL)		//找到尾节点
	{
		pTmp = pTmp->pNext;
	}
	pTmp ->pNext = pNew;

	return pNew;	
}

pList LIST_Add_front(pList head,void *pData)   //添加链表头后插入
{
	pList pNew;  //指向新节点
	pList pTmp;
	if (head == NULL || pData == NULL)
	{
		//		printf("参数错误！\n");
		return NULL;
	}
	pNew = LIST_Init(pData);  //创建新节点
	pTmp = head;

	if (pTmp->pNext != NULL)
	{
		pNew->pNext = pTmp->pNext;
	}
	pTmp ->pNext = pNew;
	
	return pNew;	
}

pList LIST_Add_Head(pList head,void *pData)   //添加通用链表节点 
{
	pList pNew;  //指向新节点

	if (head == NULL || pData == NULL)
	{
//		printf("参数错误！\n");
		return NULL;
	}
	pNew = LIST_Init(pData);  //创建新节点

	if (pNew)		//找到尾节点
	{
		pNew->pNext = head;	
	}

	return pNew;	
	
}

int LIST_Print(pList head,void (*pFun)(void *pData))
{
	pList pTmp ;
	int iCount = 0;		//统计条数

	pTmp = head->pNext;
	while (pTmp != NULL)
	{
		pFun(pTmp ->pData);
		pTmp = pTmp->pNext;
		iCount ++;
	}
	return iCount;
	
}


int LIST_Print1(pList head,char cName[],void (*pFun)(void *pData)) 
{
	/*
	pList pTmp ;
	struct staff *staff1;
	int iCount = 0;	//用于标记有几条符合条件的

	pTmp = head->pNext;
	
	

	while (pTmp != NULL)
	{
		staff1 = (struct staff *)pTmp->pData;
	//	if (strstr(((struct staff *)(pTmp->pData))->acID,cNum) != NULL);
		if (strstr(staff1->cName,cName) != NULL)
		{
			pFun(pTmp ->pData);
			iCount++;
		}
		pTmp = pTmp->pNext;
	}
	if (iCount == 0)
	{
		printf("没有找到符合要求的！\n");
	}
	
	return iCount;
	*/
	return 0;
}


int LIST_Print2(pList head,char cName,void (*pFun)(void *pData))  
{
	/*
	pList pTmp ;
	struct Table *Table1;
	int iCount = 0;	//用于标记有几条符合条件的
	
	
	pTmp = head->pNext;
	
	
	
	while (pTmp != NULL)
	{
		Table1 = (struct Table *)pTmp->pData;
		//	if (strstr(((struct staff *)(pTmp->pData))->acID,cNum) != NULL);
		if (Table1->cState == cName)
		{
			pFun(pTmp ->pData);
			iCount++;
		}
		pTmp = pTmp->pNext;
	}
	if (iCount == 0)
	{
		printf("没有找到符合要求的！\n");
	}
	return iCount;
	*/
	return 0;
}


int LIST_Print3(pList head,void (*pFun)(void *pData,pList head2),pList head1,pList head2)  
{
	pList pTmp ;
	int iCount = 0;		//统计条数
	
	pTmp = head->pNext;
	while (pTmp != NULL)
	{
		pFun(pTmp ->pData,head2);
		pTmp = pTmp->pNext;
		iCount ++;
	}
	
	return iCount;
	
}


int LIST_Print4(char acData1[],char acData2[],pList head,void (*pFun)(char acData1[],char acData2[],void *pData))  
{
	pList pTmp ;
	int iCount = 0;		//统计条数
	
	pTmp = head->pNext;
	while (pTmp != NULL)
	{
		pFun(acData1,acData2,pTmp ->pData);
		pTmp = pTmp->pNext;
		iCount ++;
	}
	return iCount;
	
}




/*
int LIST_RealDel2(pList head,int (*ORDERS_RealDel)(void *))  //符串真删除
{
	pList pTmp;
	pList pFront = head;
	pTmp = pFront->pNext;
	
	while (pTmp != NULL)
	{
		if (ORDERS_RealDel(pTmp->pData))
		{
			pFront->pNext = pTmp->pNext;
			//			free(pTmp->pData);
			FREE(pTmp);
			pTmp = NULL;
			return 1;
		}
		pFront = pFront->pNext;
		pTmp = pFront->pNext;
	}
//	printf("NO fOUND!\n");
	return 0;
}
*/

int LIST_RealDel(newgimis *pMe,pList head,char *pTaskID)  //真删除
{

	pList pTmp = NULL;
	ITaskFieldA *s_tmpT = NULL;
	pList s_tmpF = NULL;
	cTaskApp *me = NULL;
	me = pMe->pTaskApp;

	pTmp = head;
	s_tmpF = head;
	while (pTmp != NULL)
	{
		s_tmpT = (ITaskFieldA *)pTmp->pData;
		if (STRCMP(pTaskID,s_tmpT->pTaskId) == 0)
		{
			// 说明有找到
			if (pTmp == head)
			{
				me->p_head = pTmp->pNext;
			}
			else
			{
				s_tmpF->pNext = pTmp->pNext;
			}
			FREEIF(s_tmpT->pTaskStatus);
			FREEIF(s_tmpT->pTaskName);
			FREEIF(s_tmpT->pTaskId);

			FREEIF(pTmp->pData);
			FREEIF(pTmp);
			return 1;
		} 
		s_tmpF = pTmp;
		pTmp = pTmp->pNext;
	}

	return 0;
}


pList LIST_Del(pList head,char cNum[],int (*STAFF_Del)(void *,char *))  //假删除
{
	pList pTmp;
	pTmp = head->pNext;
	
	while (pTmp != NULL)
	{
		if (STAFF_Del(pTmp->pData,cNum))
		{

			return pTmp;
		}
		pTmp = pTmp->pNext;
	}
//	printf("NO fOUND!\n");
	return NULL;
}

/*
pList LIST_GetNode(pList head,char cNum[],int (*pNode)(void *,char *))			//返回查询的节点
{
	pList pTmp;
	pTmp = head->pNext;

	while (pTmp != NULL)
	{
		if (pNode(pTmp->pData,cNum))              
		{
			return pTmp;
		}
		pTmp = pTmp->pNext;
	}
//	printf("NOT FOUND!");
	return NULL;
}
*/

pList LIST_GetNode(pList head,int nod)			//返回查询的节点
{
	pList pTmp = NULL;
	int i_Count = 1;
	pTmp = head->pNext;
	if (nod == 0) // 说明是第一页
	{
		return head;
	}

	while (pTmp != NULL && i_Count < nod)
	{
		pTmp = pTmp->pNext;
		i_Count++;
	}
//	printf("NOT FOUND!");
	return pTmp;
}


int LIST_Free(pList head)    //释放空间
{
	pList pTmp = head ->pNext;
	while (pTmp != NULL)
	{
	//	free(pTmp->pData);
		head ->pNext = pTmp ->pNext;
		FREE(pTmp);
		pTmp = NULL;
		pTmp = head ->pNext;
	}
	return 1;
}



int LIST_iCount111111111(pList head)  //统计节点个数
{
	pList pTmp;
	int iCount = 0;

	pTmp = head->pNext;
	
	while (pTmp != NULL)
	{
		iCount++;
		pTmp = pTmp->pNext;
	}
	return iCount;
}

int LIST_iCount(pList head)  //统计节点个数
{
	pList pTmp;
	int iCount = 0;
	
	//pTmp = head->pNext;  //第一个节点放了数据  全删除了  head ==NULL ;    head.next == null

	pTmp = head;
	while (pTmp != NULL)
	{
		iCount++;
		pTmp = pTmp->pNext;
	}
	return iCount;
}


pList LIST_Change(pList head,char cNum[],char cState,int (*Table_Change)(void *,char *,char))  //改变cState 的状态
{
	pList pTmp;
	pTmp = head->pNext;
	
	while (pTmp != NULL)
	{
		if (Table_Change(pTmp->pData,cNum,cState))
		{
			return pTmp;
		}
		pTmp = pTmp->pNext;
	}

	return NULL;
}



pList LIST_GetNode1(pList head,int iNum,int (*pNode)(void *,int))
{
	pList pTmp;
	pTmp = head->pNext;
	
	while (pTmp != NULL)
	{
		pNode(pTmp->pData,iNum);              
		pTmp = pTmp->pNext;
	}
	//	printf("NOT FOUND!");
	return NULL;
}


pList LIST_Find(pList head,char *pTaskID) // 查找任务是否已经存在，因为终端没有做任务的话，平台还有再次下发下来
{
	pList pTmp = NULL;
	ITaskFieldA *s_tmpT = NULL;

	pTmp = head;
	while (pTmp != NULL)
	{
		s_tmpT = (ITaskFieldA *)pTmp->pData;
		if (STRCMP(pTaskID,s_tmpT->pTaskId) == 0)
		{
			// 说明有找到
			return pTmp;
		}           
		pTmp = pTmp->pNext;
	}
	return NULL;
}

int LIST_Toatal(pList head) // 返回当前总的记录数 也就是任务数
{
	int si_Ret = 1; //进来说明就有一条记录了  
	pList pTmp = NULL;


	//////////////////////////////////////////////////////////////////////////
	if (NULL == head)   //链表无节点， head == null
	{
		return 0;
	}
	//////////////////////////// 2016-1-4 yubin  ////////////////////////////

	pTmp = head->pNext;

	while (pTmp != NULL)
	{
		si_Ret++;        
		pTmp = pTmp->pNext;
	}
	//	printf("NOT FOUND!");
	pTmp = NULL;
	return si_Ret;
}


int LIST_GetTaskID(newgimis *pMe,pList head , int nid) // 获取第nid项中的TaskID
{
	pList pTmp = NULL;
	int i_Count = 0;

	cTaskApp *me = NULL;
	me = pMe->pTaskApp;

	pTmp = head;
	if (nid == 0) // 说明是第一页
	{
		MEMSET(pMe->m_Cur_TID,0x00,100);
		STRCPY(pMe->m_Cur_TID,((ITaskFieldA *)(head->pData))->pTaskId);

		//  [3/22/2016 Administrator]
		pMe->m_Cur_nDateTime = 0;
		pMe->m_Cur_nDateTime = ((ITaskFieldA *)(head->pData))->nDateTime;	

		return 1;
	}

	while (pTmp != NULL && i_Count < nid)
	{
		pTmp = pTmp->pNext;
		i_Count++;
	}
//	printf("NOT FOUND!");
	MEMSET(pMe->m_Cur_TID,0x00,100);
	STRCPY(pMe->m_Cur_TID,((ITaskFieldA *)(pTmp->pData))->pTaskId);

	//  [3/22/2016 Administrator]
	pMe->m_Cur_nDateTime = 0;
	pMe->m_Cur_nDateTime = ((ITaskFieldA *)(pTmp->pData))->nDateTime;
	
	uf_RecLog(pMe, "写入获取的TASKid");
	uf_RecLog(pMe, pMe->m_Cur_TID);
	return 1;
}

///////////////
///////////////
///////////////
/*===========================================================================
                   M A I N   M E N U
===========================================================================*/
boolean SamplePosDet_Tasklist_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam )
{
   boolean bHandled = FALSE;
   IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );
   IMenuCtl *pMenuCtl_T = testgps_GetScreenData_task( pMe );
   int si_Ret = 0;
   int i_nID = 0;
   int i_nItemID = 0;

   switch( eCode )
   {
   case EVT_SCREEN:
      if( wParam == SCREEN_PARAM_INIT ) 
	  {
         if( pMenuCtl ) 
		 {  /* This is not expected to happen */
		    ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"#### Tasklist_HandleEvent :SCREEN_PARAM_INIT: This is not expected to happen  ### ");
            testgps_DrawScreen( pMe, 0 );
            bHandled = TRUE;
         }
         else 
		 {
				AECHAR aeItem[512] = {0};
				cTaskApp *me;
				ITaskFieldA *s_tmpT = NULL;
				pList s_list = NULL;
				int nLen = 0;
				AEERect rRect;
				int s_i = 0;
				char s_t[15] = {0};
				AECHAR aes_tmp[30] ={0};
				
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," Tasklist_HandleEvent :SCREEN_PARAM_INIT  ");
				me = pMe->pTaskApp;
				
				i_nID = 0;
				s_list = me->p_head;
				if (me->p_head)
				{
					
					pMe->m_TTotal = LIST_Toatal(me->p_head); // 返回当前总的记录数 也就是任务数
					
					pMe->m_Page_c = GetTotalPage(pMe->m_TTotal,pMe->m_TaskC);

					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," Tasklist_HandleEvent : m_TaskC = %d",pMe->m_TaskC);
				
					if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_MENUCTL, (void **)&pMenuCtl ) == SUCCESS &&
									ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&pMenuCtl_T ) == SUCCESS ) 
					{
						IMENUCTL_SetProperties( pMenuCtl_T, MP_MAXSOFTKEYITEMS | MP_NO_ARROWS );

						// pMe->m_TaskC 任务个数 的变量 
																//	SETAEERECT(&rRect, 0, pMe->m_SYSFBH * 5 + 5, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - (pMe->m_SYSFBH * 5) - 5);
	//					SETAEERECT(&rRect, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - pMe->m_MenuH - 10);修改
						SETAEERECT(&rRect, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - pMe->m_MenuH - 1);
						IMENUCTL_SetRect(pMenuCtl,&rRect);

						//底部上一页，下一页
						IMENUCTL_AddItem( pMenuCtl_T, NEWGIMIS_RES_FILE, IDS_FRONT_PIC,0, NULL, (uint32)pMenuCtl_T );
						IMENUCTL_AddItem( pMenuCtl_T, NEWGIMIS_RES_FILE, IDS_NEXT_PIC, 1, NULL, (uint32)pMenuCtl_T );
						SETAEERECT(&rRect, 0, pMe->DeviceInfo.cyScreen - pMe->m_MenuH, pMe->DeviceInfo.cxScreen, pMe->m_MenuH);
						IMENUCTL_SetRect(pMenuCtl_T,&rRect);
						
						pMe->m_TaskID = 0;

					
					while(s_list)
					{
						// 说明里面有内容
						
						for (s_i = 0;s_i <512;s_i++)
						{
							aeItem[s_i] = 0;
						}
						s_tmpT = (ITaskFieldA *)(s_list->pData);
						nLen = STRLEN(s_tmpT->pTaskName);
						

						UTF8TOWSTR((byte *)(s_tmpT->pTaskName),nLen,aeItem,sizeof(aeItem));

						// IMENUCTL_AddItem( me->pMenuTask, NULL, NULL, (uint16)(TASK_ITEM_FIRST+idx), aeItem, (uint32)me );
						IMENUCTL_AddItem( pMenuCtl, NULL, NULL, i_nID, aeItem, (uint32)pMenuCtl);
						i_nID++;
						
						s_list = (pList )s_list->pNext;

						if (i_nID % pMe->m_TaskC == 0)
						{
							// 说明一页已经存满了
							s_list = NULL;
							break;
						}
					}

				   /////////   end ////
					MEMSET(s_t,0x0,15);
					SPRINTF(s_t,"task %d/%d",pMe->m_Cur_Page,pMe->m_Page_c);
					STRTOWSTR(s_t,aes_tmp,sizeof(aes_tmp));
					IMENUCTL_SetTitle( pMenuCtl, NULL, NULL, aes_tmp );
			//	   IMENUCTL_SetTitle( pMenuCtl, NEWGIMIS_RES_FILE, IDS_MORE, NULL );
					}

					testgps_SetScreenData( pMe, (void *)pMenuCtl );
					testgps_SetScreenData_task(pMe, (void *)pMenuCtl_T);
					testgps_DrawScreen( pMe, 0 );
					bHandled = TRUE;
				   
				}
				
         }
      }
      else if( wParam == SCREEN_PARAM_CLOSE ) 
	  {
		  if (pMenuCtl)
		  {
			  IMENUCTL_Release( pMenuCtl );
		  }
		  
		  testgps_SetScreenData( pMe, 0 );
		  
		  if (pMenuCtl_T)
		  {
			  IMENUCTL_Release( pMenuCtl_T );
		  }
		  
		  pMenuCtl_T = NULL;
		  
		  testgps_SetScreenData_task( pMe, 0 );
		  
		  pMe->currentHandler = 0;
		  bHandled = TRUE;
      }

	  /*
	  else if( wParam == SCREEN_PARAM_PAINT ) {
		  IMENUCTL_SetActive( pMenuCtl, TRUE );

        testgps_GetGPSInfo_Paint( pMe, (GetGPSInfo_PaintRegions)dwParam );
		IMENUCTL_Redraw( pMenuCtl );
		 
         bHandled = TRUE;
      }
	  */
		else if( wParam == SCREEN_PARAM_PAINT ) 
		{
			
			IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);

			if( IMENUCTL_IsActive( pMenuCtl_T ) )
			{
				IMENUCTL_Redraw( pMenuCtl );
			}
			else
			{
				IMENUCTL_SetActive( pMenuCtl, TRUE );  /* Also causes a menu draw */			 
			}
			IMENUCTL_Redraw( pMenuCtl );
			IMENUCTL_Redraw(pMenuCtl_T);
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," Tasklist_HandleEvent :SCREEN_PARAM_PAINT out ");
		}
		else if (wParam == TASKLIST_EVT_USER_DISPLAY)
		{		  
			//任务列表界面，点击任务，1289等错误code

			uf_RecLog(pMe, "taskList TASKLIST_EVT_USER_DISPLAY)");
			//pMe->m_bMiss = 1;
			Display_NetworkErr(pMe);
			ISHELL_SetTimer(pMe->a.m_pIShell, 3500, (PFNNOTIFY)uf_ReturnMenu_TaskList, pMe);//停止震动 返回任务列表界面	
		}
	  
	  
	break;

   case EVT_KEY:
	   //uf_RecLog(pMe,"taskList: EVT_KEY");
		//任务界面task x/x  监听按键
//	   bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
//	   i_nItemID = IMENUCTL_GetSel(pMenuCtl);


	   if( IMENUCTL_IsActive( pMenuCtl ) ) 
	   {
		  // ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"########## task..EVT_KEY:1##########");
		   bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
		   i_nItemID = IMENUCTL_GetSel(pMenuCtl);

		   if (wParam == AVK_UP || wParam == AVK_DOWN)
		   {
			   if (pMe->m_TaskID == i_nItemID)
			   {
					IMENUCTL_SetActive( pMenuCtl, FALSE );
					IMENUCTL_SetActive( pMenuCtl_T, TRUE );
			   }
			   pMe->m_TaskID = i_nItemID;
		   }
	   }

	   else if( IMENUCTL_IsActive( pMenuCtl_T ) ) 
	   {
		   //ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"########## task..EVT_KEY:2##########");
		   bHandled = IMENUCTL_HandleEvent( pMenuCtl_T, eCode, wParam, dwParam );
//		   i_nItemID = IMENUCTL_GetSel(pMenuCtl_T);
		   if (wParam == AVK_UP || wParam == AVK_DOWN)
		   {
//			   if (pMe->m_TaskID == i_nItemID)
//			   {
				IMENUCTL_SetActive( pMenuCtl_T, FALSE );
				IMENUCTL_SetActive( pMenuCtl, TRUE );
//			   }
//			   pMe->m_TaskID = i_nItemID;
		   }

	   }
		//第一页第五个任务  1 4      
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "菜单 bHandled = %d",bHandled);    //没有激活任务Item 0    激活了1 
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "菜单 i_nItemID = %d",i_nItemID);   //0-4
	   // AVK_POWER   AVK_END

	   
	   if (wParam == AVK_SOFT2)	
	   {
		   if ( pMe->m_Right_Key == '1' )
		   {
			   if (pMenuCtl)
			   {
				   IMENUCTL_Release( pMenuCtl );
			   }
			   
			   pMenuCtl = NULL;
			   
			   
			   pMe->currentHandler = 0;
			   testgps_SetScreenData( pMe, 0 );
			   
			   if (pMenuCtl_T)
			   {
				   IMENUCTL_Release( pMenuCtl_T );
			   }
			   
			   pMenuCtl_T = NULL;
			   testgps_SetScreenData_task( pMe, 0 );
			   
			   
			   //	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
			   pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
			   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
			   pMe->m_FOBFlag = TRUE; //在后台运行
			   pMe->m_WitchPag = 0;
			   //		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"后台运行 6:%d",pMe->m_WitchPag);
			   bHandled = TRUE;
				   return TRUE;
		   }
	   }
	   	//////////////////////////////////////
		//////////////////////////////////////
		if (wParam == AVK_END)
		{
			if (STRCMP(pMe->sMobileType,"55") == 0  || (STRCMP(pMe->sMobileType,"03") == 0)  || (STRCMP(pMe->sMobileType,"58") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0)  || (STRCMP(pMe->sMobileType,"57") == 0))
			{
				/*
				if (pMenuCtl)
				  {
					  IMENUCTL_Release( pMenuCtl );
				  }
		 
				*/
				// 这里直接回到后台
				  if (pMenuCtl)
				  {
					  IMENUCTL_Release( pMenuCtl );
				  }
				   
				   pMenuCtl = NULL;
				   

				   pMe->currentHandler = 0;
				   testgps_SetScreenData( pMe, 0 );
				   
				   if (pMenuCtl_T)
				   {
					   IMENUCTL_Release( pMenuCtl_T );
				   }
				   
				   pMenuCtl_T = NULL;
				   testgps_SetScreenData_task( pMe, 0 );


				   //	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
				   pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
				   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
				   pMe->m_FOBFlag = TRUE; //在后台运行
				   pMe->m_WitchPag = 0;
		//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"后台运行 6:%d",pMe->m_WitchPag);
				   bHandled = TRUE;
				   return TRUE;
			}
		}
		//////////////////////////////////////
		//////////////////////////////////////





       if( wParam == AVK_END  || wParam == AVK_CLR ) 
	   {

		   pMe->m_WitchPag = 4; // 返回更多的那个界面
		   testgps_GotoScreen( pMe, SCREENID_MORE, 0 );

		   pMe->m_WitchPag = 4;
//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"更多界面 页返回主界面 2:%d",pMe->m_WitchPag);
		   bHandled = TRUE;
	   }

/*
		 if( wParam == AVK_END  || wParam == AVK_CLR ) {

		   pMe->m_WitchPag = 4; // 返回更多的那个界面
		   testgps_GotoScreen( pMe, SCREENID_MORE, 0 );

		   pMe->m_WitchPag = 4;
		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"更多界面 页返回主界面 2:%d",pMe->m_WitchPag);
		   bHandled = TRUE;
	   }
*/

		
	   break;
	   /*
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
		  bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
		  if( wParam == AVK_END  || wParam == AVK_CLR ) {
			  ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
			  pMe->m_FOBFlag = TRUE; //在后台运行
			  pMe->m_WitchPag = 0;
			  bHandled = TRUE;
	   }
      break;
	  */

/*
	int i = me->pTaskRecsA->nSelRec;
	pTaskId =  me->pTaskRecsA->pTaskFieldA[i].pTaskId;
	if (STRLEN(pMe->MyMobileNo) > 0)   // 2013-09-23
		 SPRINTF(me->sUrl,"http://%s/task/detail?tel=%s&imsi=%s&dispid=%s&ver=1.0",sFormat,pMe->MyMobileNo,pMe->szMobileID,pTaskId); 
	else
		SPRINTF(me->sUrl,"http://%s/task/detail?dispid=%s&ver=1.0",sFormat,pTaskId); 
*/

   case EVT_COMMAND:
	   
	   //Menu里面选中了一个任务
	   if( dwParam == (uint32)pMenuCtl ) 
	   {
		   cTaskApp *me;
		   me = pMe->pTaskApp;
		  // uf_RecLog(pMe,"taskList: EVT_COMMAND  test 1");
		   
		   //pMe->m_Cur_TID   <-
		   uf_RecLog(pMe,"taskList: EVT_COMMAND select taskID");
		   LIST_GetTaskID(pMe, me->p_head , (pMe->m_Cur_Page - 1) * pMe->m_TaskC + wParam);
		  // uf_RecLog(pMe,"taskList: EVT_COMMAND  test 2");
		   //选中某个任务，根据具体任务ID与中心交互
		   cTaskApp_TaskInteract_xq(me);
		  // uf_RecLog(pMe,"taskList: EVT_COMMAND  test 3");
		   //	  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"########## task..EVT_COMMAND(pMenuCtl)..cTaskApp_TaskInteract_xq##########");
		   
	   }

	  else if( dwParam == (uint32)pMenuCtl_T ) 
	  {
		// ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"########## task..EVT_COMMAND(pMenuCtl_T)..page up or down ##########");
         switch( wParam ) 
		 {
         case 0:  //上一页
			 {
				 if (pMe->m_Cur_Page > 1)
				 {

					 cTaskApp *me;
					 pList s_list = NULL;
					 
					 
					 AECHAR aeItem[512] = {0};
					 
					 ITaskFieldA *s_tmpT = NULL;
					 int nLen = 0;
					 AEERect rRect;
					 int s_i = 0;
					 char s_t[15] = {0};
					 AECHAR aes_tmp[30] ={0};

					 uf_RecLog(pMe,"taskList: EVT_COMMAND left page");
					 me = pMe->pTaskApp;
					 pMe->m_TaskID = 0;
					 s_list = LIST_GetNode(me->p_head , (pMe->m_Cur_Page-2) * pMe->m_TaskC);
					 pMe->m_Cur_Page--;
					 
					 // 删除原来的 的
					 for (s_i = pMe->m_TaskC ; s_i > 0;s_i--)
					 {
						 IMENUCTL_DeleteItem(pMenuCtl,s_i-1);
						
					 }
					 
					
					
					
					while(s_list)
					{
						// 说明里面有内容
						
						for (s_i = 0;s_i <512;s_i++)
						{
							aeItem[s_i] = 0;
						}
						s_tmpT = (ITaskFieldA *)(s_list->pData);
						nLen = STRLEN(s_tmpT->pTaskName);
						

						UTF8TOWSTR((byte *)(s_tmpT->pTaskName),nLen,aeItem,sizeof(aeItem));

						// IMENUCTL_AddItem( me->pMenuTask, NULL, NULL, (uint16)(TASK_ITEM_FIRST+idx), aeItem, (uint32)me );
						IMENUCTL_AddItem( pMenuCtl, NULL, NULL, i_nID, aeItem, (uint32)pMenuCtl);
						i_nID++;
						
						s_list = (pList )s_list->pNext;

						if (i_nID % pMe->m_TaskC == 0)
						{
							// 说明一页已经存满了
							s_list = NULL;
							break;
						}
					}

				   /////////   end ////
					MEMSET(s_t,0x0,15);
					SPRINTF(s_t,"task %d/%d",pMe->m_Cur_Page,pMe->m_Page_c);
					STRTOWSTR(s_t,aes_tmp,sizeof(aes_tmp));
					IMENUCTL_SetTitle( pMenuCtl, NULL, NULL, aes_tmp );
					IMENUCTL_SetSel(pMenuCtl,0);
			//	   IMENUCTL_SetTitle( pMenuCtl, NEWGIMIS_RES_FILE, IDS_MORE, NULL );
				}

					testgps_DrawScreen( pMe, 0 );
					bHandled = TRUE;
				 
			 }
			 return TRUE;
		 case 1: // 下一页

		//	 pMe->m_Cur_Page,pMe->m_Page_c
			 {
				 if (pMe->m_Cur_Page < pMe->m_Page_c)
				 {

					 cTaskApp *me;
					 pList s_list = NULL;
					 
					 
					 AECHAR aeItem[512] = {0};
					 
					 ITaskFieldA *s_tmpT = NULL;
					 int nLen = 0;
					 AEERect rRect;
					 int s_i = 0;
					 char s_t[15] = {0};
					 AECHAR aes_tmp[30] ={0};
					// uf_RecLog(pMe,"按下下一页按键");
					 
					 uf_RecLog(pMe,"taskList: EVT_COMMAND right page");
					 me = pMe->pTaskApp;
					 pMe->m_TaskID = 0;
					 s_list = LIST_GetNode(me->p_head , pMe->m_Cur_Page * pMe->m_TaskC);
					 pMe->m_Cur_Page++;
					// uf_RecLog(pMe,"按下下一页按键 1");
					 // 删除原来的 的
					 for (s_i = pMe->m_TaskC ; s_i > 0;s_i--)
					 {
						 IMENUCTL_DeleteItem(pMenuCtl,s_i-1);
						 
					 }
					 //uf_RecLog(pMe,"按下下一页按键 2");
					
					
					
					while(s_list)
					{
						// 说明里面有内容
						
						for (s_i = 0;s_i <512;s_i++)
						{
							aeItem[s_i] = 0;
						}
						s_tmpT = (ITaskFieldA *)(s_list->pData);
						nLen = STRLEN(s_tmpT->pTaskName);
						

						UTF8TOWSTR((byte *)(s_tmpT->pTaskName),nLen,aeItem,sizeof(aeItem));

						// IMENUCTL_AddItem( me->pMenuTask, NULL, NULL, (uint16)(TASK_ITEM_FIRST+idx), aeItem, (uint32)me );
						IMENUCTL_AddItem( pMenuCtl, NULL, NULL, i_nID, aeItem, (uint32)pMenuCtl);
						i_nID++;
						
						s_list = (pList )s_list->pNext;

						if (i_nID % pMe->m_TaskC == 0)
						{
							// 说明一页已经存满了
							s_list = NULL;
							break;
						}
						//uf_RecLog(pMe,"循环");
					}

					//uf_RecLog(pMe,"按下下一页按键 3");
				   /////////   end ////
					MEMSET(s_t,0x0,15);
					SPRINTF(s_t,"task %d/%d",pMe->m_Cur_Page,pMe->m_Page_c);
					STRTOWSTR(s_t,aes_tmp,sizeof(aes_tmp));
					IMENUCTL_SetTitle( pMenuCtl, NULL, NULL, aes_tmp );
					IMENUCTL_SetSel(pMenuCtl,0);
			//	   IMENUCTL_SetTitle( pMenuCtl, NEWGIMIS_RES_FILE, IDS_MORE, NULL );
					}

					//uf_RecLog(pMe,"按下下一页按键 4");
					testgps_DrawScreen( pMe, 0 );
					//uf_RecLog(pMe,"按下下一页按键 5");
					bHandled = TRUE;
				 }
			 }
			 return TRUE;
		 }
      break;
   }
   return bHandled;
}




// =================================  以下为函数实体 ======================================================
boolean cTaskApp_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	cTaskApp* me;
	
	if (!pMe) return FALSE;
	if (pMe->pTaskApp != NULL)
		me = (cTaskApp *)pMe->pTaskApp;
	else if (eCode != EVT_APP_START)
		return FALSE;
	
	switch (eCode)
	{
    case EVT_APP_START:
		{
			
		
			if (pMe->pTaskApp == NULL) 
				cTaskApp_InitAppData(pMe);

			return TRUE;
		}
		
		break;
    case EVT_APP_STOP:
		// 此处是立即退出，可能读卡器没有进入休眠状态(耗电), 因此是在紧急情况下使用，正常应通过消息EVT_NFC_OVER退出
		cTaskApp_StopApp(pMe);		
        break;
        // App is being suspended 
	case EVT_APP_SUSPEND:
		break;
        // App is being resumed
	case EVT_APP_RESUME:
		break;
/*		
	case EVT_KEY_PRESS:		// 257
		ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_KEY_PRESS in pTxtExp=%d",me->pTxtExp);
		
		if (me->pTxtExp) {
			boolean bRtn;
			bRtn = ITEXTCTL_IsActive(me->pTxtExp);
			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_KEY_PRESS,HandleEvent pTxtExp active=%d",bRtn);
			if (bRtn)
				if (ITEXTCTL_HandleEvent(me->pTxtExp,eCode, wParam, dwParam)) {
					ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_KEY_PRESS,ITEXTCTL_HandleEvent");
					return TRUE;
				}
		}
		break;
*/	
/*	
	case EVT_KEY:
		if (pMe->iPressSts != 60)
			return FALSE;

		if(me->pMenuTask) {
			boolean bRtn;
			bRtn = IMENUCTL_IsActive(me->pMenuTask);
			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_KEY,HandleEvent pMenuTask active=%d",bRtn);

			if (bRtn) 
				if (IMENUCTL_HandleEvent(me->pMenuTask, EVT_KEY, wParam, 0)) {
					ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_KEY,HandleEvent pMenuTask");
					return TRUE;
				}
		} 
		if (me->pMenuDetail) {
			boolean bRtn;
			bRtn = IMENUCTL_IsActive(me->pMenuDetail);
			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_KEY,HandleEvent pMenuDetail active=%d",bRtn);
			if (bRtn) 
				if (IMENUCTL_HandleEvent(me->pMenuDetail, EVT_KEY, wParam, 0)) {
					return TRUE;
				}
		}
		if (me->pMenuList) {
			boolean bRtn;
			bRtn = IMENUCTL_IsActive(me->pMenuList);
			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_KEY,HandleEvent pMenuList active=%d",bRtn);
			if (bRtn) 
				if (IMENUCTL_HandleEvent(me->pMenuList, EVT_KEY, wParam, 0)) {
					return TRUE;
				}
		}
		if (me->pTxtExp) {
			boolean bRtn;
			bRtn = ITEXTCTL_IsActive(me->pTxtExp);
			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_KEY,HandleEvent pTxtExp active=%d",bRtn);
			if (bRtn)
				if (ITEXTCTL_HandleEvent(me->pTxtExp,eCode, wParam, dwParam)) {
					ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_KEY,ITEXTCTL_HandleEvent");
					return TRUE;
				}
		}

		break;
*/	
/*	
	case EVT_KEY_RELEASE:		// 258
		ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_KEY_RELEASE in pTxtExp=%d",me->pTxtExp);

		if (me->pTxtExp) {
			boolean bRtn;
			bRtn = ITEXTCTL_IsActive(me->pTxtExp);
			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_KEY_RELEASE,HandleEvent pTxtExp active=%d",bRtn);
			if (bRtn)
				if (ITEXTCTL_HandleEvent(me->pTxtExp,eCode, wParam, dwParam)) {
					ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_KEY_RELEASE,ITEXTCTL_HandleEvent");
					return TRUE;
				}
		}
		break;
	case EVT_CTL_TAB:
		ISHELL_PostEvent(pMe->theApp.m_pIShell,pMe->theApp.clsID, USER_TAB_EVENT,wParam,dwParam);
		return TRUE;
*/	
/*
	case EVT_COMMAND:   
		if (cTaskApp_HandleEventCommand(me, eCode, wParam, dwParam))
			return TRUE;
		break;
*/	
/*
	case EVT_TASK_GETMASTER:
		cTaskApp_SetStatus(me,INT_TASK_STATUS_BASE_0,1);		// 下发任务
		cTaskApp_TaskInteract(me);
		return TRUE;	// 	break;
*/	
/*
	case EVT_TASK_GETDETAIL:				// 下载任务详情
		me->pTaskRecsA->nSelRec = wParam;	
		cTaskApp_SetStatus(me,INT_TASK_STATUS_BASE_0,4);
		cTaskApp_ShowImage(me,IDI_TASK_01);	// 显示正在加载中图片
		cTaskApp_TaskInteract(me);
		break;
*/
/*	
	case EVT_TASK_SNDFB:		// //  往中心端发送用户选择的任务详情
		ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_TASK_SNDFB,wParam=%d", wParam);
		cTaskApp_SetStatus(me,INT_TASK_STATUS_BASE_0,7);
		cTaskApp_ShowImage(me,IDI_SENDING);	// 显示正在发送中
        me->pTaskRecB->nSelRec = wParam;
		cTaskApp_TaskInteract(me);
		break;
*/	
/*	
	case EVT_TASK_SETSTRUCT:
		switch (wParam)
		{
		case 1:		// 任务列表
			cTaskApp_SetStatus(me,INT_TASK_STATUS_BASE_0,2);		// 分解到任务列表
			cTaskApp_RecordsaFormat(me,me->pRecData);
 			if (me->pTaskRecsA != NULL) {
				cTaskApp_FreeTaskMenu(me);
 				cTaskApp_ForgeGroundEvt(pMe,0);
				cTaskApp_Alert(pMe);
				return TRUE;
 			}
			break;
		case 2:		// 任务详情
			cTaskApp_SetStatus(me,INT_TASK_STATUS_BASE_0,5);		// 分解到任务详情
			cTaskApp_RecordsbFormat(me,me->pRecData);
			cTaskApp_SetStatus(me,INT_TASK_STATUS_BASE_0,6);		// 显示任务详情界面
			if (me->pTaskRecB->bFlow)
				cTaskApp_ShowDetail(me,TRUE,1);
			else
				cTaskApp_ShowDetail(me,TRUE,0);
			break;
		default:
			break;
		}
		break;
*/	
/*	case EVT_TASK_FORM:
		if ( me->pTaskRecsA != NULL ) {
			cTaskApp_SetStatus(me,INT_TASK_STATUS_BASE_0,3);		// 显示任务派发界面
			if (wParam == 0) 
				me->pTaskRecsA->nSelRec = me->pTaskRecsA->nRecordCount;
			cTaskApp_InitTaskMenu(me);
			return TRUE;
		}
		break;
*/	
/*	case EVT_TASK_OVER:	
		{
			int iPressSts = (int)dwParam;
			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_HandleEvent EVT_TASK_OVER in iPressSts=%d[%d],wParam=%d",pMe->iPressSts,iPressSts,wParam);
			if (iPressSts != 60)  return FALSE;		// 不在前台

			cTaskApp_FreeTaskMenu(me);
			cTaskApp_FreeDetailMenu(me);
			cTaskApp_FreeFeedbackLIst(me);
			cTaskApp_FreeFeedbackExp(me);

			if (pMe->iPressSts == 60) {		// 转到后台的动作
				pMe->iPressSts = 0;
				return TRUE;
			}
			
		}
		break;
*/	
//	case USER_TAB_EVENT:
//		if ( cTaskApp_HandleCtlTab(me,eCode, wParam, dwParam) ) 
//			return TRUE;
//		break;
	default:
			break;
    }
	
    return FALSE;
}



// =================================================================
// 与中心端交互任务的信息(任务派发)
void cTaskApp_TaskInteract(cTaskApp *me)
{
	newgimis *pMe;
	char cStatus;
	WebOpt zWebOpts[7];

	if (!me) return;
	pMe = me->m_pParent;

// 	pMe->m_bMiss = 1;
// 	uf_AEPrint_Sending(pMe,FALSE);  // 显示发送中
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_TaskInteract in,cTaskStatus=%d",me->cTaskStatus);

	cTaskApp_CloseWeb(me);

	if (STRLEN(pMe->MyMobileNo) > 0)		// 2013-09-23
	{
		SPRINTF(me->sUrl,"http://%s/task/list?tel=%s&imsi=%s",pMe->m_IP,pMe->MyMobileNo,pMe->szMobileID);
	}
    else
	{
		uf_RecLog(pMe, "##### Err: no MyMobileNo ! can not connect center ! ##### ");
		SPRINTF(me->sUrl,"http://%s/task/list",pMe->strCenterIp);   //		SPRINTF(me->sUrl,"http://%s/message/getMessage?ver=1.0",sFormat); 
	}

	uf_RecLog(pMe,me->sUrl);


	//2016-1-12注释
	
//	uf_RecLog(pMe,"测试用");
//	SPRINTF(me->sUrl,"http://cn.richtalk.cn:80/task/list?tel=%s&imsi=%s",pMe->MyMobileNo,pMe->szMobileID);
//	uf_RecLog(pMe,me->sUrl);

	//任务超时标志 在这里对超时标志位清0
	me->cTimeout = 0;

	{
		int i = 0;
		
		zWebOpts[i].nId  = WEBOPT_CONNECTTIMEOUT;
		zWebOpts[i].pVal = (void *)20000;
		i++;
		zWebOpts[i].nId  = WEBOPT_PROXYSPEC;
		zWebOpts[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		zWebOpts[i].nId  = WEBOPT_END;
		
	}
	
// 	cTaskApp_SetWebOverTimer(me);
 	CALLBACK_Init(&me->webcb, cTaskApp_GotResp, (void*)me);
// 	IWEB_GetResponseV(me->pWeb,	&me->pWebResp,	&me->webcb, me->sUrl,	zWebOpts);


	IWEB_GetResponse(me->pWeb,
		(me->pWeb, &me->pWebResp, &me->webcb, me->sUrl, 
		WEBOPT_HANDLERDATA, me,
		WEBOPT_HEADER, "X-Method: GET \r\n",
		WEBOPT_STATUSHANDLER, cTaskApp_Status,  
			WEBOPT_END));
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_TaskInteract out");

	return;

}


//==============================================================
//设置status http回馈消息

static void cTaskApp_Status(void *p, WebStatus ws, void *pVal)
{
	char lStr_Sts[51];
	cTaskApp  *me;
	newgimis *pMe;
	
	if (p == NULL)
		return;
	
	me = (cTaskApp *)p;
	//    pMe = pwa->m_pParent;
	
	pMe = me->m_pParent;

	MEMSET(lStr_Sts,0,sizeof(lStr_Sts));
	switch (ws) 
	{
	case WEBS_CANCELLED:
		STRCPY(lStr_Sts, "** cancelled...");
		break;
	case WEBS_GETHOSTBYNAME:
		STRCPY(lStr_Sts, "** finding host...");
		break;          
	case WEBS_CONNECT:
		STRCPY(lStr_Sts, "** connecting...");
		break;
	case WEBS_SENDREQUEST:
		STRCPY(lStr_Sts, "** sending...");
		break;
	case WEBS_READRESPONSE: 
		me->cTimeout = GETTIMEMS();
		ISHELL_SetTimer(pMe->a.m_pIShell, TASK_SET_TIMER*1000, (PFNNOTIFY)cTaskApp_SetWebOverTimer, me);
		STRCPY(lStr_Sts, "** receiving...");
		break;
	case WEBS_GOTREDIRECT:
		STRCPY(lStr_Sts, "** redirect...");
		break;
	case WEBS_CACHEHIT:
		STRCPY(lStr_Sts, "** cache hit...");
		break;
	default:
		if (me->cTimeout == 0)
		{			//  Add 2013-09-23
			me->cTimeout = GETTIMEMS();														
			ISHELL_SetTimer(pMe->a.m_pIShell, TASK_SET_TIMER*1000, (PFNNOTIFY)cTaskApp_SetWebOverTimer, me);		
		}		// Add End
		SPRINTF(lStr_Sts, "** other info=%d",ws);
		break;
	}
	
//	uf_RecLog(me->m_pParent,lStr_Sts);
}

// =================================================================
// 设置HTTP超时的回调函数
void cTaskApp_SetWebOverTimer(cTaskApp *me)
{
	newgimis *pMe;
	char sLogBuf[128];

	if (!me) return;
	pMe = me->m_pParent;
// 
// 	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_SetWebOverTimer in");
// 
// 	me->cTimeout = 0;
// 	CALLBACK_Cancel(&me->cbGen);
//	CALLBACK_Init(&me->cbGen,  cTaskApp_WebTimeOut,  (void*)me);  这个待完善
//	ISHELL_SetTimerEx(pMe->theApp.m_pIShell, TASK_HTTP_TIMEOUT, &me->cbGen);  这个待完善
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_SetWebOverTimer out");
	
	SPRINTF(sLogBuf,"cTaskApp_SetWebOverTimer in,Overtime=%d",me->cTimeout);
	uf_RecLog(me->m_pParent,sLogBuf);
	
	if ( me->cTimeout == 0)
	{
	//	uf_RecLog(me->m_pParent,"cTaskApp_SetWebOverTimer never overtime");
		return;
	}
	
	cTaskApp_CloseWeb(me);	

	//cTaskApp_Alert(pMe);



}

//==============================================================
//设置status http回馈消息

static void cTaskApp_Status_xq(void *p, WebStatus ws, void *pVal)
{
	char lStr_Sts[51];
	cTaskApp  *me;
	newgimis *pMe;
	
	if (p == NULL)
		return;
	
	me = (cTaskApp *)p;
	//    pMe = pwa->m_pParent;
	
	pMe = me->m_pParent;
	
	MEMSET(lStr_Sts,0,sizeof(lStr_Sts));
	switch (ws) 
	{
	case WEBS_CANCELLED:
		STRCPY(lStr_Sts, "** cancelled...");
		break;
	case WEBS_GETHOSTBYNAME:
		STRCPY(lStr_Sts, "** finding host...");
		break;          
	case WEBS_CONNECT:
		STRCPY(lStr_Sts, "** connecting...");
		break;
	case WEBS_SENDREQUEST:
		STRCPY(lStr_Sts, "** sending...");
		break;
	case WEBS_READRESPONSE: 
		me->cTimeout_xq = GETTIMEMS();
		ISHELL_SetTimer(pMe->a.m_pIShell, TASK_SET_TIMER*1000, (PFNNOTIFY)cTaskApp_SetWebOverTimer_xq, me);
		STRCPY(lStr_Sts, "** receiving...");
		break;
	case WEBS_GOTREDIRECT:
		STRCPY(lStr_Sts, "** redirect...");
		break;
	case WEBS_CACHEHIT:
		STRCPY(lStr_Sts, "** cache hit...");
		break;
	default:
		if (me->cTimeout_xq == 0)
		{			//  Add 2013-09-23
			me->cTimeout_xq = GETTIMEMS();														
			ISHELL_SetTimer(pMe->a.m_pIShell, TASK_SET_TIMER*1000, (PFNNOTIFY)cTaskApp_SetWebOverTimer_xq, me);		
		}		// Add End
		SPRINTF(lStr_Sts, "** other info=%d",ws);
		break;
	}
	
	//uf_RecLog(me->m_pParent,lStr_Sts);
}


// =================================================================
// 设置HTTP超时的回调函数
void cTaskApp_SetWebOverTimer_xq(cTaskApp *me)
{
	newgimis *pMe;
	char sLogBuf[128];

	if (!me) return;
	pMe = me->m_pParent;
// 
// 	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_SetWebOverTimer in");
// 
// 	me->cTimeout = 0;
// 	CALLBACK_Cancel(&me->cbGen);
//	CALLBACK_Init(&me->cbGen,  cTaskApp_WebTimeOut,  (void*)me);  这个待完善
//	ISHELL_SetTimerEx(pMe->theApp.m_pIShell, TASK_HTTP_TIMEOUT, &me->cbGen);  这个待完善
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_SetWebOverTimer out");
	
	SPRINTF(sLogBuf,"cTaskApp_SetWebOverTimer_xq in,Overtime=%d",me->cTimeout_xq);
	uf_RecLog(me->m_pParent,sLogBuf);
	
	if ( me->cTimeout_xq == 0)
	{
		//uf_RecLog(me->m_pParent,"cTaskApp_SetWebOverTimer_xq never overtime");
		return;
	}
	
	cTaskApp_CloseWeb_xq(me);	

	pMe->m_bMiss = 1;	//锁定界面 保持显示 Display_getTaskTimeout
	
// 	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_SetWebOverTimer_fk pMe->m_Task_fs = %d ",pMe->m_Task_fs);
// 	
// 	if (1 == pMe->m_Task_fs)   
// 	{
// 		pMe->m_Task_fs = 0;			
// 
// 		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_SetWebOverTimer_xq fk get xq timeout");
// 	
// 		ISHELL_SetTimer(pMe->a.m_pIShell, 100, (PFNNOTIFY)Display_getTaskTimeout, pMe);
// 		ISHELL_SetTimer(pMe->a.m_pIShell, 2000 , (PFNNOTIFY)uf_ReturnMenu_xq, pMe);//停止震动 返回任务详情界面
// 	}
// 	else
// 	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_SetWebOverTimer_xq get xq timeout");
		//获取子任务超时
		Display_getTaskTimeout(pMe);
		//ISHELL_SetTimer(pMe->a.m_pIShell, 100, (PFNNOTIFY)Display_getTaskTimeout, pMe);
		ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)uf_ReturnMenu_TaskList, pMe);//停止震动 返回任务列表界面	
//	}
}


//==============================================================
//设置status http回馈消息

static void cTaskApp_Status_fk(void *p, WebStatus ws, void *pVal)
{
	char lStr_Sts[51];
	cTaskApp  *me;
	newgimis *pMe;
	
	if (p == NULL)
		return;
	
	me = (cTaskApp *)p;
	//    pMe = pwa->m_pParent;
	
	pMe = me->m_pParent;
	
	MEMSET(lStr_Sts,0,sizeof(lStr_Sts));
	switch (ws) 
	{
	case WEBS_CANCELLED:
		STRCPY(lStr_Sts, "** cancelled...");
		break;
	case WEBS_GETHOSTBYNAME:
		STRCPY(lStr_Sts, "** finding host...");
		break;          
	case WEBS_CONNECT:
		STRCPY(lStr_Sts, "** connecting...");
		break;
	case WEBS_SENDREQUEST:
		STRCPY(lStr_Sts, "** sending...");
		break;
	case WEBS_READRESPONSE: 
		me->cTimeout_fk = GETTIMEMS();
		ISHELL_SetTimer(pMe->a.m_pIShell, TASK_SET_TIMER*1000, (PFNNOTIFY)cTaskApp_SetWebOverTimer_fk, me);
		STRCPY(lStr_Sts, "** receiving...");
		break;
	case WEBS_GOTREDIRECT:
		STRCPY(lStr_Sts, "** redirect...");
		break;
	case WEBS_CACHEHIT:
		STRCPY(lStr_Sts, "** cache hit...");
		break;
	default:
		if (me->cTimeout_fk == 0)
		{			//  Add 2013-09-23
			me->cTimeout_fk = GETTIMEMS();														
			ISHELL_SetTimer(pMe->a.m_pIShell, TASK_SET_TIMER*1000, (PFNNOTIFY)cTaskApp_SetWebOverTimer_fk, me);		
		}		// Add End
		SPRINTF(lStr_Sts, "** other info=%d",ws);
		break;
	}
	
	//uf_RecLog(me->m_pParent,lStr_Sts);
}

// =================================================================
// 设置HTTP超时的回调函数 反馈
void cTaskApp_SetWebOverTimer_fk(cTaskApp *me)
{
	newgimis *pMe;
	char sLogBuf[128];

	if (!me) return;
	pMe = me->m_pParent;
// 
// 	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_SetWebOverTimer in");
// 
// 	me->cTimeout = 0;
// 	CALLBACK_Cancel(&me->cbGen);
//	CALLBACK_Init(&me->cbGen,  cTaskApp_WebTimeOut,  (void*)me);  这个待完善
//	ISHELL_SetTimerEx(pMe->theApp.m_pIShell, TASK_HTTP_TIMEOUT, &me->cbGen);  这个待完善
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_SetWebOverTimer out");
	
	SPRINTF(sLogBuf,"cTaskApp_SetWebOverTimer_fk in,Overtime=%d",me->cTimeout_fk);
	uf_RecLog(me->m_pParent,sLogBuf);
	
	if ( me->cTimeout_fk == 0)
	{
		//uf_RecLog(me->m_pParent,"cTaskApp_SetWebOverTimer_fk never overtime");
		return;
	}
	
	cTaskApp_CloseWeb_fk(me);	

	pMe->m_Task_fs = 0;		// 如果超时 没有0的话  会以为是反馈时候的详情进入（针对xq有两种） [3/4/2016 Administrator]

	pMe->m_bMiss = 1;	//锁定界面 保持显示 Display_timeout	反馈超时

// 	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_SetWebOverTimer_fk pMe->m_Task_fs = %d ",pMe->m_Task_fs);
// 	if (1 == pMe->m_Task_fs)   
// 	{
// 		pMe->m_Task_fs = 0;			

		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_SetWebOverTimer_fk  timeout");
	
		Display_timeout(pMe);
		//ISHELL_SetTimer(pMe->a.m_pIShell, 100, (PFNNOTIFY)Display_timeout, pMe);
		ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)uf_ReturnMenu_xq, pMe);//停止震动 返回任务详情界面
/*	}*/
	
	//cTaskApp_Alert(pMe);
}

//==============================================================================
// 初始化
//==============================================================================
void cTaskApp_InitAppData(newgimis *pMe)
{
	cTaskApp* me;
	int nErr;
	if (!pMe)
	{
		return;
	}

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_InitAppData in,cUseLoad=%d,cPicMode=%d",pMe->cUseLoad,pMe->cPicMode);
	
	if (pMe->pTaskApp != NULL)
	{
		return;
	}

	me = (cTaskApp *)MALLOC(sizeof(cTaskApp));
	me->m_pParent = (void *)pMe;
	pMe->pTaskApp = (void *)me;
	me->pMenuTask = NULL;
	me->pMenuDetail = NULL;
	me->pMenuList = NULL;
	me->pTxtExp = NULL;								// 说明
	me->p_head = NULL; //链表头

	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)&me->pWeb);	
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "cTaskApp_InitAppData IWeb err=%d", nErr);
		nErr = EFAILED;
		goto ExitHere;
	}

	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)&me->pWeb_xq);	
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "cTaskApp_InitAppData IWeb_xq err=%d", nErr);
		nErr = EFAILED;
		goto ExitHere;
	}

	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)&me->pWeb_fk);	
	if (SUCCESS != nErr) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "cTaskApp_InitAppData IWeb_fk err=%d", nErr);
		nErr = EFAILED;
		goto ExitHere;
	}

//	CALLBACK_Init(&me->cbGen,  cTaskApp_WebTimeOut,  (void*)me);

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_InitAppData out,nErr=%d",nErr);

ExitHere:
	return;
}



/*********************** 应答http返回 *******************************************************/
void cTaskApp_GotResp(void *p)
{
	
	WebRespInfo *pInfo;
	char cStatus;
	cTaskApp *me = (cTaskApp *)p;
	newgimis * pMe = me->m_pParent; 
	
// 	if (me->cTimeout == 1) 
// 	{
// 		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_GotResp response, Timeout");
// 		return;
// 	}

	//超时清零
	me->cTimeout = 0;


	pInfo = IWEBRESP_GetInfo(me->pWebResp);
	if (pInfo == NULL) {
		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_GotResp IWEBRESP_GetInfo return null");
		return;
	}
	else 
		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_GotResp response, code:%d,lContentLength=%d,cTaskStatus=%d",pInfo->nCode,pInfo->lContentLength,me->cTaskStatus);
	
	if (!WEB_ERROR_SUCCEEDED(pInfo->nCode)) 
		return;

	if(0 != pInfo->pisMessage) 
	{
		if(0 == pInfo->lContentLength) 		return; 	      

 		//超时清零
 		me->cTimeout = 0;

		me->nBodyHasRead = 0;
		FREEIF(me->pRecData);
		me->nBytes = pInfo->lContentLength;
//		if ( (cStatus == 1) || (cStatus == 4) || (cStatus == 7) )    // 任务派发 ||  任务详情 || 详情反馈
//		{
			me->pSource = pInfo->pisMessage;
			ISOURCE_AddRef(me->pSource);
//		}
		IWEBRESP_Release(me->pWebResp);
		me->pWebResp = 0;
//		if ( (cStatus == 1) || (cStatus == 4) || (cStatus == 7) )    // 任务派发 ||  任务详情 || 详情反馈
//		{
			CALLBACK_Cancel(&me->webcb);
			CALLBACK_Init(&me->webcb, cTaskApp_ReadTaskBody, me);
			cTaskApp_ReadTaskBody(me);
//		}
	}
	else
		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_GotResp response, pisMessage=%d",pInfo->pisMessage);


	return;
}


/*********************** 应答http返回 *******************************************************/
void cTaskApp_GotResp_xq(void *p)
{
	
	WebRespInfo *pInfo;
	char cStatus;
	cTaskApp *me = (cTaskApp *)p;
	newgimis * pMe = me->m_pParent; 
	
	ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_GotResp_xq");

// 	if (me->cTimeout_xq == 1) 
// 	{
// 		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_GotResp response, xiang qing Timeout");
// 
// 
// 		
// 		//##任务 ----界面显示 “获取子任务超时”
// 		if (1 == pMe->m_Task_fs)   
// 		{
// 			pMe->m_Task_fs = 0;		
// 			me->m_nState = 5; 
// 			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"*** xiang qing Timeout..display get task timeout..***");
// 			ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
// 		}
// 
// 		return;
// 	}
	//超时清零
	me->cTimeout_xq = 0;

	pInfo = IWEBRESP_GetInfo(me->pWebResp_xq);
	if (pInfo == NULL) {
		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_GotResp xiang qing in, IWEBRESP_GetInfo return null");



		//##任务 ----界面显示 “获取子任务失败”
		if (1 == pMe->m_Task_fs)   
		{
			pMe->m_Task_fs = 0;		
			me->m_nState = 5; 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"*** display: get task err1..***");
			ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
		}
		return;
	}
	else 
		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_GotResp response, code:%d,lContentLength=%d,cTaskStatus=%d",pInfo->nCode,pInfo->lContentLength,me->cTaskStatus);
	
	if (!WEB_ERROR_SUCCEEDED(pInfo->nCode)) 
	{
		//##任务 ----界面显示 “获取子任务失败”
		if (1 == pMe->m_Task_fs)   //反馈
		{
			pMe->m_Task_fs = 0;		
			me->m_nState = 4; 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"*** display: get task err2..***");
			ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
		}
		else
		{
			//任务列表界面选中一个任务，Code错误，比如-1289，弹窗提示网络异常，重启手机。
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR: PostEvent  TASKLIST_EVT_USER_DISPLAY  networkErr.");
			ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, TASKLIST_EVT_USER_DISPLAY,  0); //显示网络异常弹窗
		}


		return;
	}
	if(0 != pInfo->pisMessage) 
	{
		if(0 == pInfo->lContentLength) 	
		{
			//##任务 ----界面显示 “获取子任务失败”
			if (1 == pMe->m_Task_fs)   
			{
				pMe->m_Task_fs = 0;		
				me->m_nState = 4; 
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"*** display: get task err3..***");
				ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
			}
			
			return; 	      
		}
		
 		//超时清零
 		me->cTimeout_xq = 0;


		me->nBodyHasRead_xq = 0;
		FREEIF(me->pRecData_xq);
		me->nBytes_xq = pInfo->lContentLength;
//		if ( (cStatus == 1) || (cStatus == 4) || (cStatus == 7) )    // 任务派发 ||  任务详情 || 详情反馈
//		{
			me->pSource_xq = pInfo->pisMessage;
			ISOURCE_AddRef(me->pSource_xq);
//		}
		IWEBRESP_Release(me->pWebResp_xq);
		me->pWebResp_xq = 0;
//		if ( (cStatus == 1) || (cStatus == 4) || (cStatus == 7) )    // 任务派发 ||  任务详情 || 详情反馈
//		{
			CALLBACK_Cancel(&me->webcb_xq);
			CALLBACK_Init(&me->webcb_xq, cTaskApp_ReadTaskBody_xq, me);
			cTaskApp_ReadTaskBody_xq(me);
//		}
	}
	else
	{
		//##任务 ----界面显示 “获取子任务失败”
		if (1 == pMe->m_Task_fs)   
		{
			pMe->m_Task_fs = 0;		
			me->m_nState = 4; 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"*** display: get task err4..***");
			ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
		}
		
		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_GotResp response, pisMessage=%d",pInfo->pisMessage);
	}

	return;
}



// ==========================================================
// 读取中心端返回任务列表
// ==========================================================
void cTaskApp_ReadTaskBody(cTaskApp *me)
{
	newgimis *pMe;
	int32 nRead;
	char cStatus = 0;
	int nReceiveLen = 0;
	char sBuf[MAX_MESSAGE_LEN];

	pMe = me->m_pParent;


// 	if (me->cTimeout == 1) 
// 	{
// 		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody response, Timeout,cTaskStatus=%d",me->cTaskStatus);
// 		return;
// 	}
// 	else
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody in,cTaskStatus=%d",me->cTaskStatus);

	nRead = ISOURCE_Read(me->pSource, sBuf, sizeof(sBuf));
	if(ISOURCE_WAIT == nRead) 
	{
//		cTaskApp_SetWebOverTimer(me);
		ISOURCE_Readable(me->pSource, &me->webcb);
		return;
	}
	
	if (nRead > 0)		
	{  // -- ISOURCE_ERROR
		
		if (me->pRecData == NULL) {
			if (me->nBytes > 0)	
				me->pRecData = (char *)MALLOC( (me->nBytes+1)*sizeof(char) );
			else
				me->pRecData = (char *)MALLOC( (nRead+1)*sizeof(char));
			
			MEMCPY(me->pRecData,sBuf,nRead);
			me->nBodyHasRead = nRead; 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody, dataLen=%d, recevie=%d",me->nBytes,nRead);    
		}
		else {
			if ( me->nBytes == -1) {		
				me->pRecData = (char *)REALLOC( me->pRecData, (me->nBodyHasRead + nRead+1)*sizeof(char) );
				MEMSET(me->pRecData+me->nBodyHasRead,0,(nRead+1)*sizeof(char));
			}	
			
			MEMCPY(me->pRecData+me->nBodyHasRead,sBuf,nRead);
			me->nBodyHasRead += nRead;	
		}
		
	}  // -- ISOURCE_ERROR  
	
	if (!ISOURCE_Exhausted(nRead))			// 检查数据是否已经全部获取
	{
		ISOURCE_Readable(me->pSource, &me->webcb);
		return;
	}

//	cTaskApp_CloseWeb(me);

	// 判断接收数据是否正确	
 	MEMSET(sBuf,0,sizeof(sBuf));
 	if (me->nBodyHasRead <300)
 		ud_ToAscFmt(me->pRecData,me->nBodyHasRead,sBuf,sizeof(sBuf));
 	else
 		ud_ToAscFmt(me->pRecData,100,sBuf,sizeof(sBuf));
	{
		unsigned char usBuf[4] = {0};
		MEMCPY(usBuf,me->pRecData,4);
		nReceiveLen = ((int)usBuf[0])<<24|((int)usBuf[1])<<16|((int)usBuf[2])<<8|((int)usBuf[3]);
	}
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody data[%d]=%s",nReceiveLen,sBuf);
	if (nReceiveLen != me->nBodyHasRead) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody receive data error,nReceiveLen=%d, nBodyHasRead=%d",nReceiveLen,me->nBodyHasRead);
//		cTaskApp_ShowImage(me,IDI_NFC_05);	// 失败
//		cTaskApp_Alert(pMe);
//		ISHELL_SetTimer(pMe->a.m_pIShell, 3500, (PFNNOTIFY)cTaskApp_WebTimeOutA, me);
		return;
	}

	cStatus = 1;
	switch (cStatus)
	{
	case 1:		// 任务派发
		{
			int nRecCount;
			nRecCount = *(me->pRecData+8)<<8 | *(me->pRecData+9);			// 总任务数
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody in, Task item count=%d",nRecCount);
			
//			if ( nRecCount == 0)  {		// 没有数据
//				cTaskApp_SetStatus(me,INT_TASK_STATUS_BASE_0,0);
//				cTaskApp_ReleaseMasterRec(me);
//				FREEIF(me->pRecData);
//			}
//			else
//				ISHELL_PostEvent(pMe->a.m_pIShell,pMe->theApp.clsID,EVT_TASK_SETSTRUCT,1,0);
			
			cTaskApp_RecordsaFormat(me,me->pRecData);

		}
		break;
	case 4:				// 任务详情
//		ISHELL_PostEvent(pMe->a.m_pIShell,pMe->theApp.clsID,EVT_TASK_SETSTRUCT,2,0);	

		break;
	case 7:
		{
			int nResut;
			unsigned char usBuf[4] = {0};
			MEMCPY(usBuf,me->pRecData+4,4);
			nResut = ((int)usBuf[0])<<24|((int)usBuf[1])<<16|((int)usBuf[2])<<8|((int)usBuf[3]);		// 返回结果
//			if (nResut == 1)
//				cTaskApp_ShowImage(me,IDI_NFC_04);	// 成功
//			else
//				cTaskApp_ShowImage(me,IDI_NFC_05);	// 失败
//			cTaskApp_Alert(pMe);
//			ISHELL_SetTimer(pMe->theApp.m_pIShell, 3500, (PFNNOTIFY)cTaskApp_WebTimeOutA, me);

		}
		break;
	}

	return;
}




// ==========================================================
// 读取中心端返回任务列表
// ==========================================================
void cTaskApp_ReadTaskBody_xq(cTaskApp *me)
{
	newgimis *pMe;
	int32 nRead;
	char cStatus = 0;
	int nReceiveLen = 0;
	char sBuf[MAX_MESSAGE_LEN];

	pMe = me->m_pParent;


// 	if (me->cTimeout_xq == 1)
// 	{
// 		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody_xq response, Timeout,cTaskStatus=%d",me->cTaskStatus);
// 		
// 		//##任务 ----界面显示 “获取子任务超时”
// 		if (1 == pMe->m_Task_fs)   
// 		{
// 			pMe->m_Task_fs = 0;		
// 			me->m_nState = 5; 
// 			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"***cTaskApp_ReadTaskBody_xq   display: get task Timeout..***");
// 			ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
// 		}
// 		
// 		return;
// 	}
// 	else
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody_xq  in,cTaskStatus=%d",me->cTaskStatus);

	nRead = ISOURCE_Read(me->pSource_xq, sBuf, sizeof(sBuf));
	if(ISOURCE_WAIT == nRead) 
	{
//		cTaskApp_SetWebOverTimer(me);
		ISOURCE_Readable(me->pSource_xq, &me->webcb_xq);
		return;
	}
	
	if (nRead > 0)		
	{  // -- ISOURCE_ERROR
		
		if (me->pRecData_xq == NULL) 
		{
			if (me->nBytes_xq > 0)	
				me->pRecData_xq = (char *)MALLOC( (me->nBytes_xq+1)*sizeof(char) );
			else
				me->pRecData_xq = (char *)MALLOC( (nRead+1)*sizeof(char));
			
			MEMCPY(me->pRecData_xq,sBuf,nRead);
			me->nBodyHasRead_xq = nRead; 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody_xq, dataLen=%d, recevie=%d",me->nBytes_xq,nRead);    
		}
		else 
		{
			if ( me->nBytes_xq == -1) 
			{		
				me->pRecData_xq = (char *)REALLOC( me->pRecData_xq, (me->nBodyHasRead_xq + nRead+1)*sizeof(char) );
				MEMSET(me->pRecData_xq+me->nBodyHasRead_xq,0,(nRead+1)*sizeof(char));
			}	
			
			MEMCPY(me->pRecData_xq+me->nBodyHasRead_xq,sBuf,nRead);
			me->nBodyHasRead_xq += nRead;	
		}
		
	}  // -- ISOURCE_ERROR  
	
	if (!ISOURCE_Exhausted(nRead))			// 检查数据是否已经全部获取
	{
		ISOURCE_Readable(me->pSource_xq, &me->webcb_xq);
		return;
	}

//	cTaskApp_CloseWeb(me);

	// 判断接收数据是否正确	
 	MEMSET(sBuf,0,sizeof(sBuf));
 	if (me->nBodyHasRead_xq <300)
 		ud_ToAscFmt(me->pRecData_xq,me->nBodyHasRead_xq,sBuf,sizeof(sBuf));
 	else
 		ud_ToAscFmt(me->pRecData_xq,100,sBuf,sizeof(sBuf));
	{
		unsigned char usBuf[4] = {0};
		MEMCPY(usBuf,me->pRecData_xq,4);
		nReceiveLen = ((int)usBuf[0])<<24|((int)usBuf[1])<<16|((int)usBuf[2])<<8|((int)usBuf[3]);
	}
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody_xq data[%d]=%s",nReceiveLen,sBuf);
	if (nReceiveLen != me->nBodyHasRead_xq)
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody_xq receive data error,nReceiveLen=%d, nBodyHasRead=%d",nReceiveLen,me->nBodyHasRead);
//		cTaskApp_ShowImage(me,IDI_NFC_05);	// 失败
//		cTaskApp_Alert(pMe);
//		ISHELL_SetTimer(pMe->a.m_pIShell, 3500, (PFNNOTIFY)cTaskApp_WebTimeOutA, me);


		//##任务 ----界面显示 “获取子任务失败”
		if (1 == pMe->m_Task_fs)   
		{
			pMe->m_Task_fs = 0;		
			me->m_nState = 4; 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"***cTaskApp_ReadTaskBody_xq   display: get task err1..***");
			ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
		}

		return;
	}

	cStatus = 4;
	switch (cStatus)
	{

	case 4:				// 任务详情
//		ISHELL_PostEvent(pMe->a.m_pIShell,pMe->theApp.clsID,EVT_TASK_SETSTRUCT,2,0);
		{
			cTaskApp_RecordsbFormat(me,me->pRecData_xq);

			pMe->m_bMiss = 0;

			if (me->pTaskRecB->bFlow)
			{
			//	pMe->m_WitchPag = 17;
				//###任务###2015-12-28	
				//任务发送
	
				if (1 == pMe->m_Task_fs)   
				{
					//uf_RecLog(pMe, "#################跳到XTRA_EVT_USER_DISPLAY");

					pMe->m_Task_fs = 0;
					me->m_nState = 1;
					ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
					
					//ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu_xq, pMe);//停止震动 返回任务详情界面
					//testgps_GotoScreen( pMe, SCREENID_DETAIL1, 0 );			
				}
				else
				{
					//uf_RecLog(pMe, "#################跳到SCREENID_DETAIL1");
					pMe->m_WitchPag = 17;
					testgps_GotoScreen( pMe, SCREENID_DETAIL1, 0 );  
				}
//				cTaskApp_ShowDetail(me,TRUE,1);
			}
			else
			{
				pMe->m_WitchPag = 18; 
				//uf_RecLog(pMe, "### fei liu cheng begin..");
				testgps_GotoScreen( pMe, SCREENID_DETAIL2, 0 );
//				cTaskApp_ShowDetail(me,TRUE,0);
			}

		}
		break;
	case 7:
		{
			int nResut;
			unsigned char usBuf[4] = {0};
			MEMCPY(usBuf,me->pRecData+4,4);
			nResut = ((int)usBuf[0])<<24|((int)usBuf[1])<<16|((int)usBuf[2])<<8|((int)usBuf[3]);		// 返回结果
//			if (nResut == 1)
//				cTaskApp_ShowImage(me,IDI_NFC_04);	// 成功
//			else
//				cTaskApp_ShowImage(me,IDI_NFC_05);	// 失败
//			cTaskApp_Alert(pMe);
//			ISHELL_SetTimer(pMe->theApp.m_pIShell, 3500, (PFNNOTIFY)cTaskApp_WebTimeOutA, me);

		}
		break;
	}

	return;
}


// =================================================================================
// 销毁数据
// cMode:  1 退出(转到后台)之前销毁,  0 非退出的销毁
// =================================================================================
void cTaskApp_FreeAppData(cTaskApp* me,char cMode)
{
	newgimis *pMe;
//	int nErr;
	
	if (!me) return;
	pMe = me->m_pParent;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_FreeAppData in,iPressSts=%d",pMe->iPressSts);
	cTaskApp_ReleaseMasterRec(me);
	cTaskApp_ReleaseDetailRec(me);
	MEMSET(me->sUrl,0,sizeof(me->sUrl));
	FREEIF(me->pRecData);

	MEMSET(me->sUrl_xq,0,sizeof(me->sUrl_xq));
	FREEIF(me->pRecData_xq);

	MEMSET(me->sUrl_fk,0,sizeof(me->sUrl_fk));
	FREEIF(me->pRecData_fk);

	if (cMode == 1) {
		if (pMe->iPressSts == 60)
			pMe->iPressSts = 0;
	}
	
	return;
	
}


// 释放m_TaskRecsA中资源
void cTaskApp_ReleaseMasterRec(cTaskApp *me)
{

	if (me->pTaskRecsA != NULL) {
		int i = 0;
		for(i=0;i<me->pTaskRecsA->nRecordCount;i++) {
			FREEIF(me->pTaskRecsA->pTaskFieldA[i].pTaskId);
			FREEIF(me->pTaskRecsA->pTaskFieldA[i].pTaskName);
			FREEIF(me->pTaskRecsA->pTaskFieldA[i].pTaskStatus);
		}
		FREEIF(me->pTaskRecsA->pTaskFieldA);
		me->pTaskRecsA->nRecordCount = 0;
		FREEIF(me->pTaskRecsA);
	}
	
	return;
}

// 释放m_TaskRecsB中资源
void cTaskApp_ReleaseDetailRec(cTaskApp *me)
{

	if (me->pTaskRecB != NULL) 
	{
		int i = 0;
		uf_RecLog(me->m_pParent,"cTaskApp_ReleaseDetailRec");
		for(i=0;i<me->pTaskRecB->nFeedbackCount;i++) 
		{
			FREEIF(me->pTaskRecB->pTaskFieldB[i].pFeedbackId);
			FREEIF(me->pTaskRecB->pTaskFieldB[i].pFeedbackName);
			me->pTaskRecB->pTaskFieldB[i].nSequence = 0;
		}
		FREEIF(me->pTaskRecB->pTaskFieldB);
		FREEIF(me->pTaskRecB->pTaskId);
		FREEIF(me->pTaskRecB->pTaskName);
		FREEIF(me->pTaskRecB->pTaskContent);
		FREEIF(me->pTaskRecB->pTaskStatus);
		MEMSET(me->pTaskRecB->exp,0,sizeof(me->pTaskRecB->exp));
		FREEIF(me->pTaskRecB);
	}
	
	me->rectTxt.x = 0;
	me->rectTxt.dx = 0;
	me->rectTxt.y = 0;
	me->rectTxt.dy = 0;

	return;
}


// 停止的应用
void cTaskApp_StopApp(newgimis *pMe)
{
	cTaskApp* me = pMe->pTaskApp;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_StopApp in");

	cTaskApp_FreeAppData(me,1);
//	cTaskApp_FreeTaskMenu(me);
//	cTaskApp_FreeDetailMenu(me);
//	cTaskApp_FreeFeedbackLIst(me);
//	cTaskApp_FreeFeedbackExp(me);

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cPortApp_StopApp in-0");

	cTaskApp_CloseWeb(me);
	if(me->pWeb)	{
		int i,n;					// 2.6.4D2
		n = 0;
		i=IWEB_Release(me->pWeb);
		if (i>0) {
			n++;
			i=IWEB_Release(me->pWeb);
		}
		
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_StopApp, IWEB_Release=%d,n=%d",i,n);
		me->pWeb = 0;
	}

	CALLBACK_Cancel(&me->cbGen);
	me->m_pParent = 0;
	FREEIF(pMe->pTaskApp);
	
	return;
}


// ==================================================================
// 关闭http接口
void cTaskApp_CloseWeb(cTaskApp *me)
{
	newgimis *pMe;
	//	int nErr = SUCCESS;
	
	pMe = me->m_pParent;
	
	//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_CloseWeb in");
	
	if (me->pSource != NULL)
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_Close in,pSource != NULL");
		ISOURCE_Release(me->pSource);
		me->pSource = NULL;
	}
	
	if (me->pWebResp != NULL)
	{
		int n=0;		
		uint32 nErr = IWEBRESP_Release(me->pWebResp);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_Close in,clear piWResp,return=%d,n=%d",nErr,n);
		me->pWebResp = NULL;
	}
	
	/*me->cTimeout = 0;*/
	CALLBACK_Cancel(&me->cbGen);
	CALLBACK_Cancel(&me->webcb);
	
	//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_CloseWeb out");
	return;
}


// ==================================================================
// 关闭http接口
void cTaskApp_CloseWeb_xq(cTaskApp *me)
{
	newgimis *pMe;
	//	int nErr = SUCCESS;
	
	pMe = me->m_pParent;
	
	//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_CloseWeb xiang qing in ");
	
	if (me->pSource_xq != NULL)
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_Close xiang qing in,pSource != NULL");
		ISOURCE_Release(me->pSource_xq);
		me->pSource_xq = NULL;
	}
	
	if (me->pWebResp_xq != NULL)
	{
		int n=0;		
		uint32 nErr = IWEBRESP_Release(me->pWebResp_xq);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_Close xiang qing in,clear piWResp,return=%d,n=%d",nErr,n);
		me->pWebResp_xq = NULL;
	}
	
	/*me->cTimeout_xq = 0;*/
	CALLBACK_Cancel(&me->cbGen_xq);
	CALLBACK_Cancel(&me->webcb_xq);
	
	//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_CloseWeb xiang qing out");
	return;
}

// ==================================================================
// 关闭http接口 反馈
void cTaskApp_CloseWeb_fk(cTaskApp *me)
{
	newgimis *pMe;
	//	int nErr = SUCCESS;
	
	pMe = me->m_pParent;
	
	//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_CloseWeb fang kui in ");
	
	if (me->pSource_fk != NULL)
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_Close fang kui in,pSource != NULL");
		ISOURCE_Release(me->pSource_fk);
		me->pSource_fk = NULL;
	}
	
	if (me->pWebResp_fk != NULL)
	{
		int n=0;		
		uint32 nErr = IWEBRESP_Release(me->pWebResp_fk);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_Close fang kui in,clear piWResp,return=%d,n=%d",nErr,n);
		me->pWebResp_fk = NULL;
	}
	
	me->cTimeout_fk = 0;
	CALLBACK_Cancel(&me->cbGen_fk);
	CALLBACK_Cancel(&me->webcb_fk);
	
	//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_CloseWeb fang kui out");
	return;
}

/* ======================================================
解析任务列表的数据包结构
传入参数：
	char *sBuf : 数据包
传出参数： 解析后的记录
======================================================== */ 
void cTaskApp_RecordsaFormat(cTaskApp *me, char *sBuf)
{
	newgimis *pMe;
	ITaskRecordsA *pRecords = NULL;
	uint16  nRecCount = 0;						// 记录条数
	uint16  nContentLen=0;
	int nLoc = 10;	// 配置项的内容是从第8位开始
	int i = 0;
	pList s_tmpL =NULL;
	
	if (!me) return;
	pMe = me->m_pParent;

	cTaskApp_ReleaseMasterRec(me);

	nRecCount = sBuf[8]<<8 | sBuf[9];			// 总任务数		
	if (nRecCount == 0) goto ExitHere;

	pRecords = (ITaskRecordsA *)MALLOC(sizeof(ITaskRecordsA));
	pRecords->nRecordCount = nRecCount;
	pRecords->nSelRec = pRecords->nRecordCount;
	pRecords->pTaskFieldA = (ITaskFieldA *)MALLOC(nRecCount*sizeof(ITaskFieldA));

	for (i = 0; i<nRecCount;i++)
	{
		// ITaskFieldA 接收任务要存放到结构体中的
		ITaskFieldA *s_tmpT = NULL;
		s_tmpT = (ITaskFieldA *)MALLOC(nRecCount*sizeof(ITaskFieldA));
		pRecords->pTaskFieldA[i].nDateTime = 0;
		// 任务ID
		nContentLen = sBuf[nLoc]<<8|sBuf[nLoc+1];
		pRecords->pTaskFieldA[i].pTaskId = (char *)MALLOC((nContentLen+1)*sizeof(char));
		s_tmpT->pTaskId = (char *)MALLOC((nContentLen+1)*sizeof(char));
		MEMCPY(pRecords->pTaskFieldA[i].pTaskId,sBuf+nLoc+2,nContentLen);
		MEMCPY(s_tmpT->pTaskId,sBuf+nLoc+2,nContentLen);
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"TaskId[%d]=%s",nContentLen,pRecords->pTaskFieldA[i].pTaskId);
		nLoc += (2+nContentLen);
		// 任务标题
		nContentLen = sBuf[nLoc]<<8|sBuf[nLoc+1];
		pRecords->pTaskFieldA[i].pTaskName = (char *)MALLOC((nContentLen+1)*sizeof(char));
		s_tmpT->pTaskName = (char *)MALLOC((nContentLen+1)*sizeof(char));
		MEMCPY(pRecords->pTaskFieldA[i].pTaskName,sBuf+nLoc+2,nContentLen);
		MEMCPY(s_tmpT->pTaskName,sBuf+nLoc+2,nContentLen);
		nLoc += (2+nContentLen);
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Task Title[%d]=%s",nContentLen,pRecords->pTaskFieldA[i].pTaskName);
		// 任务当前状态
		nContentLen = sBuf[nLoc]<<8|sBuf[nLoc+1];
		pRecords->pTaskFieldA[i].pTaskStatus = (char *)MALLOC((nContentLen+1)*sizeof(char));
		s_tmpT->pTaskStatus = (char *)MALLOC((nContentLen+1)*sizeof(char));
		MEMCPY(pRecords->pTaskFieldA[i].pTaskStatus,sBuf+nLoc+2,nContentLen);
		MEMCPY(s_tmpT->pTaskStatus,sBuf+nLoc+2,nContentLen);
		nLoc += (2+nContentLen);
		// 任务派发时间
		{
			JulianType dDate;
			uint32 nDateTime = 0;
			char *sTmp = sBuf + nLoc;
			nContentLen = 8;
			dDate = cTaskApp_DateFromInt64(sBuf+nLoc,&nDateTime);
			pRecords->pTaskFieldA[i].nDateTime = nDateTime;
			s_tmpT->nDateTime = nDateTime;
//			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"date=%04d-%02d-%02d %02d:%02d:%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
//			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_RecordsaFormat timestamp=0[%d],1[%d],2[%d],3[%d],4[%d],5[%d],6[%d],7[%d]"
//			 							 ,sTmp[0],sTmp[1],sTmp[2],sTmp[3],sTmp[4],sTmp[5],sTmp[6],sTmp[7]);
			nLoc += nContentLen;
		}

		// 添加掉链表
		
		if (me->p_head == NULL)
		{
			me->p_head = LIST_Init(s_tmpT);
		}
		else
		{
			s_tmpL = LIST_Find(me->p_head,s_tmpT->pTaskId); // 查找任务是否已经存在，因为终端没有做任务的话，平台还有再次下发下来
			if (!s_tmpL)
			{
				pList s_tmp = NULL;

				if (pMe->m_Task_F < 2)
				{
					s_tmp = LIST_Add(me->p_head,s_tmpT);
				}
				else
				{
					s_tmp = LIST_Add_Head(me->p_head,s_tmpT);
					me->p_head = s_tmp;
				}
				
				
			}
			
		}
		
	}

ExitHere:

	me->pTaskRecsA = pRecords;
	return;
}


/*
static void myiweb_Stop_Task(cTaskApp *me)
{
   // this cancels any pending web transaction, or readable on the 
   // response body.  if nothing is pending, this has no effect
   CALLBACK_Cancel(&pwa->cb_xt);
   
   // then clean up, if necessary
   if ((char *)0 != pwa->pszPostData_xt) {

	   FREE(pwa->pszPostData_xt);
		pwa->pszPostData_xt = NULL;
//		pwa->iFileRead = 0;

//      *pwa->pszPostData_xt = 1; // write delimiter back in, if any
 //     pwa->pszPostData_xt = 0;
   }
 // 新增  
   if (pwa->pipPostData_xt != NULL)
	{
		IPEEK_Release(pwa->pipPostData_xt);
		// Add by zjie 2011-01-30
		pwa->pipPostData_xt = NULL;

	}
 // 新增  
	if (pwa->pSource_xt != NULL) {
		uint32 nErr = ISOURCE_Release(pwa->pSource_xt);	// 2013-04-12 加 nErr
		pwa->pSource_xt = NULL;
	}

   
	if (pwa->piWResp_xt)
	{
		IWEBRESP_Release(pwa->piWResp_xt);
		pwa->piWResp_xt= NULL;
	}

	if (pwa->m_pIWeb_xt) 
	{
//		IWEB_Release(pwa->m_pIWeb_xt);
//		pwa->m_pIWeb_xt = NULL;
		int i,n;			// 2.6.4D2

		//////////////  9-25  暂时先不考虑这个  start //////////////
		/*
		int n_optid = 0;
		int n_index = 0;
		int si_R = 0;
		WebOpt opt;
		uf_RecLog(pwa,"comeing here movept");
		si_R = IWEB_GetOpt(pwa->m_pIWeb_xt, n_optid, n_index, &opt);
		ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"movept %d",n_index);
		if (si_R == SUCCESS)
		{
			si_R = IWEB_RemoveOpt(pwa->m_pIWeb_xt, n_optid, n_index);
			if (si_R == SUCCESS)
			{
				ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"movept SUCCESS");
			}
			else
			{
				ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"movept FAIL");
			}
		}
		*//////////////  9-25     end  //////////////



/*		
		n = 0;
		i=IWEB_Release(pwa->m_pIWeb_xt);
		while (i>0) {
			n++;
			i=IWEB_Release(pwa->m_pIWeb_xt);
		}

		pwa->m_pIWeb_xt = NULL;
	}

	if (pwa->m_BodyBuffer_xt)
	{
		FREE(pwa->m_BodyBuffer_xt);
		pwa->m_BodyBuffer_xt = NULL;
	}
	pwa->m_BodySize_xt = 0;
	pwa->m_BodyAllocSize_xt = 0;

}

  */


  static int GetTotalPage(int task,int pagec) //获取当前总的页数
  {
	  int i_Ret = 0;
	  i_Ret = task % pagec;
	  if (i_Ret > 0)
	  {
		  i_Ret = task / pagec + 1;
	  }
	  else
	  {
		  i_Ret = task / pagec;
	  }
	  return i_Ret ;
  }




  /// 以下与详情有光

  // =================================================================
// 与中心端交互任务的详情
void cTaskApp_TaskInteract_xq(cTaskApp *me)
{
	newgimis *pMe;
	char cStatus;
	WebOpt zWebOpts[7];

	if (!me) return;
	pMe = me->m_pParent;

	pMe->m_bMiss = 1;
	if (1 == pMe->m_Task_fs) 
	{
		uf_AEPrint_Sending(pMe,FALSE);  // 显示发送中
	}
	else
	{
		uf_AEPrint_xq(pMe,FALSE);	//显示获取详情中
	}
	
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_TaskInteract in xiang qing ,cTaskStatus=%d",me->cTaskStatus);
/*
	if (STRLEN(pMe->MyMobileNo) > 0)   // 2013-09-23
		SPRINTF(me->sUrl,"http://%s/task/detail?tel=%s&imsi=%s&dispid=%s&ver=1.0",sFormat,pMe->MyMobileNo,pMe->szMobileID,pTaskId); 
	else
		SPRINTF(me->sUrl,"http://%s/task/detail?dispid=%s&ver=1.0",sFormat,pTaskId); 

	uf_RecLog(pMe,me->sUrl);
*/

	cTaskApp_CloseWeb_xq(me);

	if (STRLEN(pMe->MyMobileNo) > 0)		// 2013-09-23
	{
		SPRINTF(me->sUrl_xq,"http://%s/task/detail?tel=%s&imsi=%s&dispid=%s&ver=1.0",pMe->m_IP,pMe->MyMobileNo,pMe->szMobileID, pMe->m_Cur_TID );
	}
    else
	{
		uf_RecLog(pMe, "##### Warning: no MyMobileNo ! can not connect center ! ##### ");
		SPRINTF(me->sUrl_xq,"http://%s/task/detail",pMe->strCenterIp);    
	}
//	uf_RecLog(pMe,"测试用 1");
//	SPRINTF(me->sUrl_xq,"http://cn.richtalk.cn:80/task/detail?tel=%s&imsi=%s&dispid=%s&ver=1.0",pMe->MyMobileNo,pMe->szMobileID,pMe->m_Cur_TID);
	uf_RecLog(pMe,me->sUrl_xq);

	//清空超时标志位
	me->cTimeout_xq = 0;

	{
		int i = 0;
		
		zWebOpts[i].nId  = WEBOPT_CONNECTTIMEOUT;
		zWebOpts[i].pVal = (void *)20000;
		i++;
		zWebOpts[i].nId  = WEBOPT_PROXYSPEC;
		zWebOpts[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		zWebOpts[i].nId  = WEBOPT_END;
		
	}
	
// 	cTaskApp_SetWebOverTimer_xq(me);
 	CALLBACK_Init(&me->webcb_xq, cTaskApp_GotResp_xq, (void*)me);
// 	IWEB_GetResponseV(me->pWeb_xq,	&me->pWebResp_xq,	&me->webcb_xq, me->sUrl_xq,	zWebOpts);
	
	IWEB_GetResponse(me->pWeb_xq,
		(me->pWeb_xq, &me->pWebResp_xq, &me->webcb_xq, me->sUrl_xq, 
		WEBOPT_HANDLERDATA, me,
		WEBOPT_HEADER, "X-Method: GET \r\n",
		WEBOPT_STATUSHANDLER, cTaskApp_Status_xq,  
			WEBOPT_END));

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_TaskInteract_xq xiang qing out");

	return;

}


/* ======================================================
解析任务详情的数据包结构
传入参数：
	char *sBuf : 数据包
传出参数： 解析后的记录
======================================================== */ 
void cTaskApp_RecordsbFormat(cTaskApp *me, char *sBuf)
{
	newgimis *pMe;
	ITaskRecordB *pRecords = NULL;
	uint16  nRecCount = 0;						// 反馈项记录条数
	uint16  nContentLen=0;
	int nLoc = 0;	
	int i = 0;
	
	char cTaskName[] = "Name: " ;
	char cTaskContent[] = "Content: " ;
	char cTaskStatus[] = "Status: " ;

	char *str = NULL;
	
	if (!me) return;
	pMe = me->m_pParent;

	cTaskApp_ReleaseDetailRec(me);

	uf_RecLog(pMe,"cTaskApp_RecordsbFormat in");

	// 根据任务派发ID，判断接收数据是否正确
	nLoc = 4;	
	nContentLen = sBuf[nLoc]<< 8|sBuf[nLoc+1];

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"xiang qing nContentLen=%d",nContentLen);

	pRecords = (ITaskRecordB *)MALLOC(sizeof(ITaskRecordB));
	// 任务ID
	pRecords->pTaskId = (char *)MALLOC((nContentLen+1)*sizeof(char));
	MEMCPY(pRecords->pTaskId,sBuf+nLoc+2,nContentLen);
	nLoc += (2+nContentLen);
	//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"xiang qing pTaskId=%s",pRecords->pTaskId);

	nContentLen = sBuf[nLoc]<<8|sBuf[nLoc+1];
	pRecords->pTaskName = (char *)MALLOC((nContentLen+1)*sizeof(char)+ sizeof(cTaskName));

	str = (char *)MALLOC((nContentLen+1)*sizeof(char) );
	ZEROAT(str);

	MEMCPY(str,sBuf+nLoc+2,nContentLen);

	STRCPY(pRecords->pTaskName, cTaskName); 

	STRCAT(pRecords->pTaskName, str);  //崩了
	FREE(str);
	nLoc += (2+nContentLen);

	nContentLen = sBuf[nLoc]<<8|sBuf[nLoc+1];
	pRecords->pTaskContent = (char *)MALLOC((nContentLen+1)*sizeof(char) +sizeof(cTaskContent)+ sizeof(cTaskContent));

	str = (char *)MALLOC((nContentLen+1)*sizeof(char) );
	ZEROAT(str);
	
	MEMCPY(str,sBuf+nLoc+2,nContentLen);
	
	STRCPY(pRecords->pTaskContent, cTaskContent); 
	
	STRCAT(pRecords->pTaskContent, str);  //崩了
	FREE(str);
	nLoc += (2+nContentLen);

	nContentLen = sBuf[nLoc]<<8|sBuf[nLoc+1];
	pRecords->pTaskStatus = (char *)MALLOC((nContentLen+1)*sizeof(char) + sizeof(cTaskStatus));

	str = (char *)MALLOC((nContentLen+1)*sizeof(char));
	ZEROAT(str);
	
	MEMCPY(str,sBuf+nLoc+2,nContentLen);
	
	STRCPY(pRecords->pTaskStatus, cTaskStatus); 
	
	STRCAT(pRecords->pTaskStatus, str);  //崩了
	FREE(str);
	nLoc += (2+nContentLen);

	// 是否流程项
	pRecords->bFlow = sBuf[nLoc];
	nLoc += 1;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"xiang qing bFlow=%d",pRecords->bFlow);
	// 反馈项个数
	nRecCount = sBuf[nLoc]<<8|sBuf[nLoc+1];
	pRecords->nFeedbackCount = nRecCount;
	nLoc += 2;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"xiang qing nFeedbackCount=%d",nRecCount);

	pRecords->pTaskFieldB= (ITaskFieldB *)MALLOC(nRecCount*sizeof(ITaskFieldB));

	for (i = 0; i<nRecCount;i++)
	{
		// 反馈项ID
		nContentLen = sBuf[nLoc]<<8|sBuf[nLoc+1];
		pRecords->pTaskFieldB[i].pFeedbackId = (char *)MALLOC((nContentLen+1)*sizeof(char));
		MEMCPY(pRecords->pTaskFieldB[i].pFeedbackId,sBuf+nLoc+2,nContentLen);
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_RecordsbFormat FeedbackId[%d]=%s",nContentLen,pRecords->pTaskFieldB[i].pFeedbackId);
		nLoc += (2+nContentLen);
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"xiang qing pFeedbackId=%s",pRecords->pTaskFieldB[i].pFeedbackId);
		// 反馈项名称
		nContentLen = sBuf[nLoc]<<8|sBuf[nLoc+1];
		pRecords->pTaskFieldB[i].pFeedbackName = (char *)MALLOC((nContentLen+1)*sizeof(char));
		MEMCPY(pRecords->pTaskFieldB[i].pFeedbackName,sBuf+nLoc+2,nContentLen);
		nLoc += (2+nContentLen);
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"xiang qing pFeedbackName=%s",pRecords->pTaskFieldB[i].pFeedbackName);
		// 反馈项顺序
		pRecords->pTaskFieldB[i].nSequence = sBuf[nLoc]<<8|sBuf[nLoc+1];
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"xiang qing nSequence=%d",pRecords->pTaskFieldB[i].nSequence);
		nLoc += 2;
	}

	me->pTaskRecB = pRecords;
	return;
}

//任务内容界面
boolean SamplePosDet_Content_HandleEvent( newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam )
{
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );
	
	boolean bHandled = FALSE;

	AEERect rRect;

	cTaskApp *me = NULL;
	me = pMe->pTaskApp;
	
// 	rRect = (AEERect *)MALLOC(sizeof(rRect)); 
// 	ZEROAT(rRect);

	switch( eCode ) 
	{
	case EVT_SCREEN:
		if( wParam == SCREEN_PARAM_INIT ) 
		{
			if(pMenuCtl) 
			{
				testgps_DrawScreen( pMe, 0 );
				bHandled = TRUE;
			}
			else
			{
				int nErr = 0;
				//静态文本       ///////////////////////////////////////
				nErr = ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_STATIC,(void**)(&me->m_pIStatic));
				if ( nErr != SUCCESS)   {
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_Content_HandleEvent m_pIStatic create err=%d  ##### ",nErr);
					return TRUE;
				}
				//设置静态文本框
				rRect.x = pMe->m_SYSFBH;
				rRect.y = 0;
				rRect.dx = pMe->DeviceInfo.cxScreen - pMe->m_SYSFBH;
				rRect.dy = pMe->DeviceInfo.cyScreen - 2* pMe->m_SYSFBH;
				ISTATIC_SetRect(me->m_pIStatic, &rRect);

				//文本属性
				ISTATIC_SetProperties(me->m_pIStatic, ST_CENTERTITLE);
				
				MEMSET(me->pContentText,0x00,404);

				//设置文本内容
				{
					AECHAR* psTextBuf = NULL;
					uint32  bufSize = 0;
					uint32  nRtn;
					//标签："任务内容"
					ISHELL_GetResSize(pMe->a.m_pIShell,NEWGIMIS_RES_FILE, IDS_TASK_CHINAESE,RESTYPE_STRING,&bufSize);
					psTextBuf = (AECHAR*)MALLOC((bufSize+1)*sizeof(AECHAR));
					nRtn = ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE,IDS_TASK_CHINAESE, psTextBuf,bufSize);
					
					if (nRtn >0 )
					{
						if (me->pTaskRecB->pTaskContent != NULL)
						{
							char    szBuf[1212] = {0};/*101*///300zi
							int iLen = 0;
							va_list args;
							
							va_start( args, me->pTaskRecB->pTaskContent );
							(void)VSNPRINTF( szBuf, 3060, me->pTaskRecB->pTaskContent, args );/*255*/
							va_end( args );
							iLen = STRLEN(szBuf);
							
							UTF8TOWSTR( (byte *)szBuf,iLen,me->pContentText,sizeof(me->pContentText));
							nErr = ISTATIC_SetText(me->m_pIStatic, psTextBuf, me->pContentText, AEE_FONT_BOLD,AEE_FONT_BOLD);
						}
					}

					FREE(psTextBuf);
				}

				//底部的soft菜单控件   返回///////////////////////////////////////////////////////////////////////////		
				nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void **) (&(pMenuCtl)));
				if ( nErr != SUCCESS)   {
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_Content_HandleEvent soft menu create err=%d  ##### ",nErr);
					return TRUE;
				}
				// "返回"
				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IMENU_RTN, 0, NULL, (uint32)pMenuCtl);
				testgps_SetScreenData( pMe, (void *)pMenuCtl);
				testgps_DrawScreen( pMe, 0 );
				bHandled = TRUE;
			}
		}
		else if( wParam == SCREEN_PARAM_CLOSE ) 
		{
			ISTATIC_Reset(me->m_pIStatic);

			if (me->m_pIStatic != NULL)
			{
				ISTATIC_Release(me->m_pIStatic );
				me->m_pIStatic = NULL;
			}
			
			if( pMenuCtl != NULL)	
			{								
				if ( IMENUCTL_GetItemCount(pMenuCtl)>0 )	
				{
					IMENUCTL_DeleteAll(pMenuCtl);  
					IMENUCTL_Release(pMenuCtl);    
					pMenuCtl = NULL;	
				}
			}	
			
			testgps_SetScreenData( pMe, 0 );
			pMe->currentHandler = 0;
			bHandled = TRUE;
		}
		else if( wParam == SCREEN_PARAM_PAINT ) 
		{
			IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
			IDISPLAY_Update (pMe->a.m_pIDisplay);

			//ISTATIC_SetActive(me->m_pIStatic, TRUE);
			ISTATIC_Redraw(me->m_pIStatic);
			IMENUCTL_Redraw(pMenuCtl);
			IMENUCTL_SetActive(pMenuCtl,TRUE);

		}
		break;
	case EVT_KEY:
		{
			if ( !IMENUCTL_IsActive(pMenuCtl) )
			{
				if (wParam == AVK_SELECT)
				{
					ISTATIC_SetActive(me->m_pIStatic, FALSE);
					IMENUCTL_SetActive(pMenuCtl, TRUE);
					return TRUE;
				}
			}

			
			if (wParam == AVK_SOFT2)	
			{
				if ( pMe->m_Right_Key == '1' )
				{
					ISTATIC_Reset(me->m_pIStatic);
					
					// 这里直接回到后台
					if (me->m_pIStatic != NULL)
					{
						ISTATIC_Release(me->m_pIStatic );
						me->m_pIStatic = NULL;
					}
					
					if( pMenuCtl != NULL)	
					{								
						if ( IMENUCTL_GetItemCount(pMenuCtl)>0 )	
						{
							IMENUCTL_DeleteAll(pMenuCtl);  
							IMENUCTL_Release(pMenuCtl);    
							pMenuCtl = NULL;	
						}
					}	
					
					pMe->currentHandler = 0;
					testgps_SetScreenData( pMe, 0 );
					
					pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
					ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
					pMe->m_FOBFlag = TRUE; //在后台运行
					pMe->m_WitchPag = 0;
					bHandled = TRUE;
					return TRUE;
				}
			}

			
			if (wParam == AVK_END)
			{
				if (STRCMP(pMe->sMobileType,"55") == 0 || (STRCMP(pMe->sMobileType,"03") == 0)  || (STRCMP(pMe->sMobileType,"58") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0) || (STRCMP(pMe->sMobileType,"57") == 0) )
				{
					ISTATIC_Reset(me->m_pIStatic);

					// 这里直接回到后台
					if (me->m_pIStatic != NULL)
					{
						ISTATIC_Release(me->m_pIStatic );
						me->m_pIStatic = NULL;
					}
					
					if( pMenuCtl != NULL)	
					{								
						if ( IMENUCTL_GetItemCount(pMenuCtl)>0 )	
						{
							IMENUCTL_DeleteAll(pMenuCtl);  
							IMENUCTL_Release(pMenuCtl);    
							pMenuCtl = NULL;	
						}
					}	
			
					pMe->currentHandler = 0;
					testgps_SetScreenData( pMe, 0 );
					
					pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
					ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
					pMe->m_FOBFlag = TRUE; //在后台运行
					pMe->m_WitchPag = 0;
					bHandled = TRUE;
					return TRUE;
				}
			}
			
			
			if( wParam == AVK_END  || wParam == AVK_CLR ) 
			{	
				if (me->pTaskRecB->bFlow)
				{
					pMe->m_WitchPag = 17; // 返回流程任务
					testgps_GotoScreen( pMe, SCREENID_DETAIL1, 0 );
				}
				else
				{
					pMe->m_WitchPag = 18; // 返回非流程任务
					testgps_GotoScreen( pMe, SCREENID_DETAIL2, 0 );
				}

				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"task content  :%d",pMe->m_WitchPag);
				bHandled = TRUE;
			}


			if (IMENUCTL_IsActive(pMenuCtl))
			{
				if (wParam == AVK_DOWN || wParam == AVK_UP )
				{
					ISTATIC_SetActive(me->m_pIStatic, TRUE);
					IMENUCTL_SetActive(pMenuCtl, FALSE);
					return TRUE;
				}
				IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam);
				//return TRUE;
			}
			if (ISTATIC_IsActive(me->m_pIStatic))
			{
				ISTATIC_HandleEvent(me->m_pIStatic, eCode, wParam, dwParam);
				//return TRUE;
			}

		}
	break;
	case EVT_COMMAND:
		{
			if( dwParam == (uint32)pMenuCtl) 
			{
				if (wParam == 0)
				{
					if (me->pTaskRecB->bFlow)
					{
						pMe->m_WitchPag = 17; // 返回流程任务
						testgps_GotoScreen( pMe, SCREENID_DETAIL1, 0 );
					}
					else
					{
						pMe->m_WitchPag = 18; // 返回非流程任务
						testgps_GotoScreen( pMe, SCREENID_DETAIL2, 0 );
					}
				}
			}
		}
	break;
	}

	return bHandled;
}


// 非流程界面

boolean SamplePosDet_Detail2_HandleEvent( newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam )
{
	AEERect rRect;
	AEERect rRect_cln;

	CtlAddItem rMenuItem;

	int  m_nFontHeight = 0;
	int i = 0;
	cTaskApp *me = NULL;

	boolean bHandled = FALSE;
	
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );
	char s_buf[100];//  测试用

	if (!pMe) return TRUE;

	me = pMe->pTaskApp;
	
	if (me->pTaskRecB->bFlow)  
		return TRUE;

	if(pMe->m_bSendPF == TRUE )//用来标记是否正在发送图片的标志位 TRUE -- 表示正在发送状态  FALSE -- 表示空闲
	{
		// 说明正在发送照片 
		return TRUE;
	}

	//任务修改 
	if((pMe->textCtl_T!=NULL) 
			&& (ITEXTCTL_IsActive( pMe->textCtl_T))
				&&ITEXTCTL_HandleEvent( pMe->textCtl_T, eCode, wParam, dwParam ))
	{
	//	ITEXTCTL_Redraw( pMe->textCtl_T );
		uf_RecLog(pMe, " ## task 2 ## handle gettext ");
		ITEXTCTL_GetText(pMe->textCtl_T,pMe->pText,100);
		WSTRTOSTR(pMe->pText,s_buf,100);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"## task2: s_buf = %s ",s_buf);
	
		//uf_RecLog(pMe,s_buf );
	//	IDISPLAY_Update(pMe->a.m_pIDisplay); 

		return TRUE;
	}


	switch( eCode ) 
	{
	case EVT_SCREEN:
		if( wParam == SCREEN_PARAM_INIT ) 
		{
			uf_RecLog(pMe,"task2: SCREEN_PARAM_INIT" );

			if(pMenuCtl) 
			{  /* This is not expected to happen */
					
				boolean sb_tmp = FALSE;
				uf_RecLog(pMe, "##### task2:  This is not expected to happen.. :if( pMenuCtl)  ##### ");
				sb_tmp = IMENUCTL_DeleteAll(pMenuCtl);
				testgps_DrawScreen( pMe, 0 );
				bHandled = TRUE;
			}
			else 
			{//pMenuCtl == NULL
				int nErr = 0;
				//底部的soft菜单控件   发送/完成/返回///////////////////////////////////////////////////////////////////////////		
				nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void **) (&(pMenuCtl)));
				if ( nErr != SUCCESS)   {
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"###### task2 soft menu create err=%d  ##### ",nErr);
					return TRUE;
				}
				
				{	// 有关菜单的美工部分
					AEEMenuColors stColors;
					stColors.wMask = CLR_SYS_ITEM|CLR_SYS_ITEM_TEXT|CLR_SYS_ITEM_SEL|CLR_SYS_ITEM_SEL_TEXT;
					stColors.cBack = MAKE_RGB(199,230,248);
					stColors.cText = MAKE_RGB(9,90,136);
					stColors.cSelBack = MAKE_RGB(25,137,199);
					stColors.cSelText = MAKE_RGB(255,255,255);
					IMENUCTL_SetColors(pMenuCtl,&stColors);
				}  // 有关菜单的美工部分


				//"发送"
				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_STRING_FK, TASK_DETAIL_SEND, NULL, (uint32)pMenuCtl);		
				// "完成"
				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_FINISH, TASK_DETAIL_FINISH, NULL, (uint32)pMenuCtl );
				// "返回"
				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IMENU_RTN, TASK_DETAIL_RETURN, NULL, (uint32)pMenuCtl);
	
			//	IMENUCTL_SetActive( me->pMenuDetail, TRUE );
			//	IMENUCTL_SetActive( pMenuCtl, TRUE );

				testgps_SetScreenData( pMe, (void *)pMenuCtl);
				
				//LIST菜单控件：任务内容/////////////////////////////////////////////////////////////////////////////////////////////////
				if (me->pMenuList_T == NULL) {
					nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_LISTCTL, (void **) (&(me->pMenuList_T)));
					if ( nErr != SUCCESS)   {
						//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##### task2 AEECLSID_LISTCTL create pMenuList err=%d ##### ",nErr);
						return TRUE;
					}
				}
				IMENUCTL_AddItem( me->pMenuList_T, NEWGIMIS_RES_FILE, IDS_TASK_IN, TASK_DISPLAY_CONTENT, NULL, (uint32)me->pMenuList_T);
				IMENUCTL_EnableCommand( me->pMenuList_T, TRUE );
				
				{	// 列表控件内容
					AEEMenuColors stColors;
					stColors.wMask = CLR_SYS_ITEM|CLR_SYS_ITEM_TEXT|CLR_SYS_ITEM_SEL|CLR_SYS_ITEM_SEL_TEXT;
					stColors.cBack = MAKE_RGB(199,230,248);
					stColors.cText = MAKE_RGB(9,90,136);
					stColors.cSelBack = MAKE_RGB(25,137,199);
					stColors.cSelText = MAKE_RGB(255,255,255);
					IMENUCTL_SetColors(me->pMenuList_T,&stColors);			
				}  // 列表控件内容
				

				//LIST菜单控件：反馈项/////////////////////////////////////////////////////////////////////////////////////////////////
				if (me->pMenuList == NULL) {
					nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_LISTCTL, (void **) (&(me->pMenuList)));
					if ( nErr != SUCCESS)   {
					//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##### task2 AEECLSID_LISTCTL create pMenuList err=%d ##### ",nErr);
						return TRUE;
					}
				}
				
				for (i=0; i<me->pTaskRecB->nFeedbackCount;i++) {	// for -- 1
					AECHAR aeItem[30] = {0};
					int nLen = STRLEN(me->pTaskRecB->pTaskFieldB[i].pFeedbackName);
					UTF8TOWSTR((byte *)(me->pTaskRecB->pTaskFieldB+i)->pFeedbackName,nLen,aeItem,sizeof(aeItem));
					IMENUCTL_AddItem( me->pMenuList, NULL, NULL, (uint16)i, aeItem, (uint32)me );  
				}	// -- 1
				IMENUCTL_EnableCommand( me->pMenuList, TRUE );

				{	// 列表控件内容
					AEEMenuColors stColors;
					stColors.wMask = CLR_SYS_ITEM|CLR_SYS_ITEM_TEXT|CLR_SYS_ITEM_SEL|CLR_SYS_ITEM_SEL_TEXT;
					stColors.cBack = MAKE_RGB(199,230,248);
					stColors.cText = MAKE_RGB(9,90,136);
					stColors.cSelBack = MAKE_RGB(25,137,199);
					stColors.cSelText = MAKE_RGB(255,255,255);
					IMENUCTL_SetColors(me->pMenuList,&stColors);			
				}  // 列表控件内容


				//文本输入 (备注)//////////////////////////////////////////////////////////////////////////////////
				if (!pMe->textCtl_T) 
				{		
					//##创建任务text输入编辑框
					if (ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_TEXTCTL,(void**)&pMe->textCtl_T) == SUCCESS)
					{
						ITEXTCTL_SetMaxSize(pMe->textCtl_T, TEXT_MAXSIZE);
						//设置属性,有边框
						ITEXTCTL_SetProperties(pMe->textCtl_T,TP_FRAME|TP_MULTILINE);
						
						//设置显示矩形区域
						

						//q3设置
						//SETAEERECT(&rRect, 0, (2 * pMe->m_SYSFBH) + 5*pMe->m_SYSFBH + 10, pMe->DeviceInfo.cxScreen, 2 * pMe->m_SYSFBH); 
						
						//update: 2016-1-28
						//SETAEERECT(&rRect, 0, (2 * pMe->m_SYSFBH) + 5*pMe->m_SYSFBH + 10, pMe->DeviceInfo.cxScreen,5* pMe->m_SYSFBH);  // m_nFontHeight+10
					
						// "33" SETAEERECT(&rRect, 0, (2 * pMe->m_SYSFBH) + 6*pMe->m_SYSFBH + 10, pMe->DeviceInfo.cxScreen,4* pMe->m_SYSFBH);

						//X3输入法
// 						if (STRCMP(pMe->sMobileType,"33") == 0)
// 						{
// 							SETAEERECT(&rRect, 0, (2 * pMe->m_SYSFBH) + 6*pMe->m_SYSFBH + 10, pMe->DeviceInfo.cxScreen,4* pMe->m_SYSFBH);
// 						}else{
// 							SETAEERECT(&rRect, 0, (2 * pMe->m_SYSFBH) + 5*pMe->m_SYSFBH + 10, pMe->DeviceInfo.cxScreen,5* pMe->m_SYSFBH); 
// 						}

						//2016-4-15
						//X3输入法
 						if (STRCMP(pMe->sMobileType,"33") == 0)
 						{
 							SETAEERECT(&rRect, 0, (2 * pMe->m_SYSFBH) + 6*pMe->m_SYSFBH + 10, pMe->DeviceInfo.cxScreen,4* pMe->m_SYSFBH);
 						}else{
 							SETAEERECT(&rRect, 0, 7*pMe->m_SYSFBH + 16, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - (7*pMe->m_SYSFBH+16+2*pMe->m_SYSFBH) );
						}

						
						
						ITEXTCTL_SetRect(pMe->textCtl_T, &rRect);						
						ITEXTCTL_SetInputMode(pMe->textCtl_T, AEE_TM_PINYIN);			// 拼音AEE_TM_PINYIN  AEE_TM_NUMBERS
					}
				}
				//IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
				//IDISPLAY_Update (pMe->a.m_pIDisplay);

				testgps_DrawScreen( pMe, 0 );
				bHandled = TRUE;
			}
		}


		else if( wParam == SCREEN_PARAM_CLOSE ) 
		{
			uf_RecLog(pMe, "task2: SCREEN_PARAM_CLOSE ");

			if (pMe->m_pIStatic_Task != NULL)
			{
				ISTATIC_Release(pMe->m_pIStatic_Task);
			}

			//list Menu菜单
			if( me->pMenuList != NULL)	
			{								
				if ( IMENUCTL_GetItemCount(me->pMenuList)>0 ){	
					IMENUCTL_DeleteAll(me->pMenuList);  
					IMENUCTL_Release(me->pMenuList);    
					me->pMenuList = NULL;
				}
			}	

			//list_T 内容菜单
			if( me->pMenuList_T != NULL)	
			{								
				if ( IMENUCTL_GetItemCount(me->pMenuList_T)>0 ){	
					IMENUCTL_DeleteAll(me->pMenuList_T);  
					IMENUCTL_Release(me->pMenuList_T);    
					me->pMenuList_T = NULL;
				}
			}

			if( pMenuCtl != NULL)	
			{								
				if ( IMENUCTL_GetItemCount(pMenuCtl)>0 )	
				{
					IMENUCTL_DeleteAll(pMenuCtl);  
					IMENUCTL_Release(pMenuCtl);    
					pMenuCtl = NULL;	
				}
			}	
		
			if (pMe->textCtl_T)
			{
				ITEXTCTL_Release(pMe->textCtl_T);
			}
			testgps_SetScreenData( pMe, 0 );

			pMe->currentHandler = 0;
			pMe->textCtl_T = NULL;
			bHandled = TRUE;
		}

		else if( wParam == SCREEN_PARAM_PAINT ) 
		{
			uf_RecLog(pMe, "### wParam == SCREEN_PARAM_PAINT");

			IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
			IDISPLAY_Update (pMe->a.m_pIDisplay);

 			{
				//关于任务派发时间
 				JulianType dDate;
				AECHAR  asText[50] = {0};
				char    szBuf[25] = {0};
 				GETJULIANDATE(pMe->m_Cur_nDateTime, &dDate);
				SPRINTF(szBuf,"time: %04d-%02d-%02d %02d:%02d%",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute);
				STRTOWSTR(szBuf, asText, sizeof(asText));
 				Task_Print_unicode(pMe,0,1,AEE_FONT_BOLD,IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL,asText);
 			}

			Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
			//Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
			Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);

			{// 列表控件标签+列表控件内容 显示	文本内容
				AECHAR* psTextBuf = NULL;
				uint32  bufSize = 0;
				uint32  nRtn;
				AEERect rRectList;
				//标签："点击显示内容"
				ISHELL_GetResSize(pMe->a.m_pIShell,NEWGIMIS_RES_FILE, IDS_TASK_CONTENT,RESTYPE_STRING,&bufSize);
				psTextBuf = (AECHAR*)MALLOC((bufSize+1)*sizeof(AECHAR));
				nRtn = ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE,IDS_TASK_CONTENT, psTextBuf,bufSize);
				
				if (nRtn >0 )
					rRectList = Task_Print_unicode( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,psTextBuf);
				else
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_DisplayCtlList in LoadResString nRtn=%d",nRtn);
				FREE(psTextBuf);
				
				//菜单list控件显示
				rRectList.x += rRectList.dx + 3;
				rRectList.dx = pMe->DeviceInfo.cxScreen - rRectList.x - 5;
				
				rRectList.dy = pMe->m_SYSFBH +MENULIST_HEIGHT; //update: 2016-2-2
				IMENUCTL_SetRect( me->pMenuList_T, &rRectList );
				//IMENUCTL_Redraw( me->pMenuList );
			}

			
			{// 列表控件标签+列表控件内容 显示	反馈项
				AECHAR* psTextBuf = NULL;
				uint32  bufSize = 0;
				uint32  nRtn;
				AEERect rRectList;
				//标签："反馈项"
				ISHELL_GetResSize(pMe->a.m_pIShell,NEWGIMIS_RES_FILE, IDS_TASK_02,RESTYPE_STRING,&bufSize);
				psTextBuf = (AECHAR*)MALLOC((bufSize+1)*sizeof(AECHAR));
				nRtn = ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE,IDS_TASK_02, psTextBuf,bufSize);
			
				if (nRtn >0 )
					rRectList = Task_Print_unicode( pMe, 4, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,psTextBuf);
				else
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_DisplayCtlList in LoadResString nRtn=%d",nRtn);
				FREE(psTextBuf);
				
				//菜单list控件显示
				rRectList.x += rRectList.dx + 3;
				rRectList.dx = pMe->DeviceInfo.cxScreen - rRectList.x - 5;

				rRectList.dy = pMe->m_SYSFBH + MENULIST_HEIGHT; //update: 2016-2-2
				IMENUCTL_SetRect( me->pMenuList, &rRectList );
				//IMENUCTL_Redraw( me->pMenuList );
			}


		
			{
				int nRtn;
				int nLine;
				int nExpLen;
				AECHAR szText[128];

				AECHAR temp[10] = {0x2e, 0x2e, 0x2e,0x0000};


				//nLine = (me->pTaskRecB->bFlow) ? 4 : 5;
				nRtn = ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_TASK_BZ, szText,sizeof(szText));
				if (nRtn > 0)
				{
					Task_Print_unicode( pMe, 5, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,szText);
				}
			}
		

			{
				//界面提示输入法输入模式
				AEETextInputModeInfo CurIme;											
				ITEXTCTL_GetInputMode(pMe->textCtl_T,&CurIme);									
				yb_ShowInputMode(pMe,CurIme.tmMode,2);
			}

			//刷到界面

			IMENUCTL_Redraw(me->pMenuList);
			IMENUCTL_Redraw(me->pMenuList_T);
			IMENUCTL_Redraw(pMenuCtl);
			ITEXTCTL_Redraw(pMe->textCtl_T);

			//焦点
			IMENUCTL_SetActive(me->pMenuList_T,TRUE); 
		}

		else if (wParam == TASK2_EVT_USER_DISPLAY)
		{
			//uf_RecLog(pMe, "## task2 XTRA_EVT_USER_DISPLAY ) ");

			//###任务###2015-12-28
			//任务发送 用上面的标志位 me->FinshorXQ
			//pMe->m_bMiss = 1;
			//pMe->m_Task_fs = 1;
			uf_AEPrint_Sending(pMe,FALSE);  // 显示发送中


			if (1 == me->m_nState)//详情成功，反馈成功
			{
				//1.5秒后显示"发送成功" & 震动
				//	uf_RecLog(pMe, " ### 1.5s later display..success");
				uf_RecLog(pMe, "## task2 TASK2_EVT_USER_DISPLAY 1 :Display_Success) ");
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)Display_Success, pMe);
				me->m_nState = 0;
			}else if (2 == me->m_nState) //反馈失败
			{
				uf_RecLog(pMe, "## task2 TASK2_EVT_USER_DISPLAY 2 :Display_false) ");
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)Display_false, pMe);
				me->m_nState = 0;
			}else if (3 == me->m_nState)//反馈超时
			{
				uf_RecLog(pMe, "## task2 TASK2_EVT_USER_DISPLAY 3 :Display_timeout) ");
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)Display_timeout, pMe);
				me->m_nState = 0;
			}else if (6 == me->m_nState)//反馈超时
			{
				uf_RecLog(pMe, "## task2 TASK2_EVT_USER_DISPLAY 6 :Display_fkweb_error ");
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)Display_fkweb_error, pMe);
				//me->m_nState = 0;
			}else{
				uf_RecLog(pMe, "##### task2 TASK2_EVT_USER_DISPLAY 7  ERR!  ##### ");
			}

			pMe->m_Task_fs = 0;

			// 反馈异常时候跳转到任务列表 [3/8/2016 Administrator]-----------------------------------
			if (6 == me->m_nState)
			{
				me->m_nState = 0;
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000 + 3500, (PFNNOTIFY)uf_ReturnMenu_TaskList, pMe);//停止震动 返回任务详情界面
			}
			else
			{
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000 + 3500, (PFNNOTIFY)uf_ReturnMenu_xq, pMe);//停止震动 返回任务详情界面
			}
			//-----------------------------------------------------------------------------------------

			//ISHELL_SetTimer(pMe->a.m_pIShell, 2000 + 3500, (PFNNOTIFY)uf_ReturnMenu_xq, pMe);//停止震动 返回任务详情界面	

			//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ISHELL_PostEvent ###");			
			//	ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0);
		}
		break;

	case EVT_KEY:
	{
		if(wParam == AVK_SELECT)
		{
			if (ITEXTCTL_IsActive( pMe->textCtl_T))
			{
				pMe->m_iText_Input = 0;
				ITEXTCTL_SetActive( pMe->textCtl_T,FALSE);			
				IMENUCTL_SetActive(pMenuCtl,TRUE);
				return TRUE;
			}

			if (IMENUCTL_IsActive( me->pMenuList))
			{
				//保存LIST 选项
				pMe->m_iTask_fkID = IMENUCTL_GetSel(me->pMenuList);
				pMe->m_iText_Input = 1;
				ITEXTCTL_SetActive( pMe->textCtl_T,TRUE);						
				IMENUCTL_SetActive(me->pMenuList,FALSE);
				
				ITEXTCTL_SetCursorPos(pMe->textCtl_T, TC_CURSOREND);//  [3/17/2016 Administrator]
				return TRUE;
			}

			if (IMENUCTL_IsActive( me->pMenuList_T) )
			{
				pMe->m_WitchPag = 19; // 任务内容界面
				testgps_GotoScreen( pMe, SCREENID_CONTENT, 0 );
				return TRUE;
			}
		}

		if (wParam == AVK_DOWN ) //在控件间切换
		{
			if (IMENUCTL_IsActive(me->pMenuList_T) )
			{
				IMENUCTL_SetActive(me->pMenuList_T,FALSE);
				IMENUCTL_SetActive(me->pMenuList,TRUE);
				return TRUE;
			}
						
			if (IMENUCTL_IsActive( me->pMenuList))
			{
				if ( (me->pTaskRecB->nFeedbackCount-1) == IMENUCTL_GetSel(me->pMenuList))
				{
					pMe->m_iText_Input = 1;
					IMENUCTL_SetActive(me->pMenuList, FALSE);
					ITEXTCTL_SetActive(pMe->textCtl_T,TRUE);
					ITEXTCTL_SetCursorPos(pMe->textCtl_T, TC_CURSOREND);
				}
				else
				{
					bHandled = IMENUCTL_HandleEvent( me->pMenuList, eCode, wParam, dwParam );
					return TRUE;
				}
			}
			else if (ITEXTCTL_IsActive(pMe->textCtl_T))
			{				
				pMe->m_iText_Input = 0;
				ITEXTCTL_SetActive( pMe->textCtl_T,FALSE);
				IMENUCTL_SetActive(pMenuCtl, TRUE);
				return TRUE;
			}
			else if (IMENUCTL_IsActive( pMenuCtl))
			{
				IMENUCTL_SetActive(pMenuCtl,FALSE);
				IMENUCTL_SetActive(me->pMenuList_T,TRUE);
				return TRUE;
			}
			
		}

		if (wParam == AVK_UP ) //在控件间切换
		{
			if (IMENUCTL_IsActive(me->pMenuList_T) )
			{
				IMENUCTL_SetActive(me->pMenuList_T,FALSE);
				IMENUCTL_SetActive(pMenuCtl,TRUE);
				return TRUE;
			}
						
			if (IMENUCTL_IsActive( me->pMenuList))
			{

				if (0 == IMENUCTL_GetSel(me->pMenuList))
				{
					IMENUCTL_SetActive(me->pMenuList, FALSE);
					IMENUCTL_SetActive(me->pMenuList_T,TRUE);
					return TRUE;
				}
				else
				{
					bHandled = IMENUCTL_HandleEvent( me->pMenuList, eCode, wParam, dwParam );
					return TRUE;
				}
			}
			else if (ITEXTCTL_IsActive(pMe->textCtl_T))
			{
				pMe->m_iText_Input = 0;
				ITEXTCTL_SetActive( pMe->textCtl_T,FALSE);
				IMENUCTL_SetActive(me->pMenuList,TRUE);
				return TRUE;
			}
			else if (IMENUCTL_IsActive( pMenuCtl))
			{
				pMe->m_iText_Input = 1;
				IMENUCTL_SetActive(pMenuCtl,FALSE);
				ITEXTCTL_SetActive( pMe->textCtl_T,TRUE);
				ITEXTCTL_SetCursorPos(pMe->textCtl_T, TC_CURSOREND); //update: 2016-2-2
			}
			return TRUE;
		}

		
		if (wParam == AVK_SOFT2)	
		{
			if ( pMe->m_Right_Key == '1' )
			{
				// 这里直接回到后台
				if (me->pMenuDetail)
				{
					IMENUCTL_Release( me->pMenuDetail );
					me->pMenuDetail = NULL;
				}
				
				if( me->pMenuList != NULL)	
				{								
					if ( IMENUCTL_GetItemCount(me->pMenuList)>0 ){	
						IMENUCTL_DeleteAll(me->pMenuList);  
						IMENUCTL_Release(me->pMenuList);    
						me->pMenuList = NULL;
					}
				}	
				
				if( me->pMenuList_T != NULL)	
				{								
					if ( IMENUCTL_GetItemCount(me->pMenuList_T)>0 ){	
						IMENUCTL_DeleteAll(me->pMenuList_T);  
						IMENUCTL_Release(me->pMenuList_T);    
						me->pMenuList_T = NULL;
					}
				}
				
				if( pMenuCtl != NULL)	
				{								
					if ( IMENUCTL_GetItemCount(pMenuCtl)>0 )	
					{
						IMENUCTL_DeleteAll(pMenuCtl);  
						IMENUCTL_Release(pMenuCtl);    
						pMenuCtl = NULL;	
					}
				}	
				
				if (pMe->textCtl_T)
				{
					ITEXTCTL_Release(pMe->textCtl_T);
					pMe->textCtl_T = NULL;
				}
				
				pMe->currentHandler = 0;
				testgps_SetScreenData( pMe, 0 );
				
				pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
				ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
				pMe->m_FOBFlag = TRUE; //在后台运行
				pMe->m_WitchPag = 0;
				bHandled = TRUE;
				
				pMe->m_Cur_Page = 1;
				   return TRUE;
			}
	   }

		if (wParam == AVK_END)
		{
			if (STRCMP(pMe->sMobileType,"55") == 0 || (STRCMP(pMe->sMobileType,"03") == 0)  || (STRCMP(pMe->sMobileType,"58") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0) || (STRCMP(pMe->sMobileType,"57") == 0) )
			{
				// 这里直接回到后台
				   if (me->pMenuDetail)
				   {
					   IMENUCTL_Release( me->pMenuDetail );
					   me->pMenuDetail = NULL;
				   }

				   if( me->pMenuList != NULL)	
				   {								
					   if ( IMENUCTL_GetItemCount(me->pMenuList)>0 ){	
						   IMENUCTL_DeleteAll(me->pMenuList);  
						   IMENUCTL_Release(me->pMenuList);    
						   me->pMenuList = NULL;
					   }
				   }	

				   if( me->pMenuList_T != NULL)	
				   {								
					   if ( IMENUCTL_GetItemCount(me->pMenuList_T)>0 ){	
						   IMENUCTL_DeleteAll(me->pMenuList_T);  
						   IMENUCTL_Release(me->pMenuList_T);    
						   me->pMenuList_T = NULL;
					   }
				   }
				   
				   if( pMenuCtl != NULL)	
				   {								
					   if ( IMENUCTL_GetItemCount(pMenuCtl)>0 )	
					   {
						   IMENUCTL_DeleteAll(pMenuCtl);  
						   IMENUCTL_Release(pMenuCtl);    
						   pMenuCtl = NULL;	
					   }
				   }	
				   
				   if (pMe->textCtl_T)
				   {
					   ITEXTCTL_Release(pMe->textCtl_T);
					   pMe->textCtl_T = NULL;
				   }

				   pMe->currentHandler = 0;
				   testgps_SetScreenData( pMe, 0 );

				   pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
				   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
				   pMe->m_FOBFlag = TRUE; //在后台运行
				   pMe->m_WitchPag = 0;
				   bHandled = TRUE;

				   pMe->m_Cur_Page = 1;
				   return TRUE;
			}
		}
			
		if( wParam == AVK_END  || wParam == AVK_CLR ) 
		{
			if (ITEXTCTL_IsActive(pMe->textCtl_T))
			{
				return TRUE;
			}
			pMe->m_Cur_Page = 1;
			pMe->m_WitchPag = 16; // 返回主界面(任务第一页)
			testgps_GotoScreen( pMe, SCREENID_TASKLIST, 0 );
			//pMe->m_WitchPag = 16;
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"task2: back to taskList   :%d",pMe->m_WitchPag);
			bHandled = TRUE;
		}

		if (IMENUCTL_IsActive(pMenuCtl))
		{
			IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam);
			//return TRUE;
		}

	}
	break;

	case EVT_COMMAND:
		if( dwParam == (uint32)pMenuCtl) 
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"当前的菜单项 = %d", wParam );

			switch( wParam ) 
			{
				case TASK_DETAIL_SEND:  // 说明是上报反馈数据
				{
					 // 设置反馈地址
					 // SPRINTF(me->sUrl,"http://cn.richtalk.cn:80/task/list?tel=%s&imsi=%s",pMe->MyMobileNo,pMe->szMobileID);
					char *pFeedid = NULL;
					boolean bRtn = FALSE;
					AECHAR asText[128] = {0};
					int  nTextLen = 0;
					//add
					char url_temp[1024]={0};
					if (me->pszPostData != NULL) {
						FREE( me->pszPostData );
						me->pszPostData = NULL;
						me->iFileRead = 0;
					}
					
					if (me->pipPostData != NULL)
					{
						IPEEK_Release(me->pipPostData);
						me->pipPostData = NULL;
					}

					//end

					//AECHAR bsText[128] = {0};
					uf_RecLog(pMe, "task2: press FK button ");
					//获取反馈项ID
					//pFeedid = me->pTaskRecB->pTaskFieldB[0].pFeedbackId;// 流程的
					//pFeedid = me->pTaskRecB->pTaskFieldB[pMe->m_iTask_fkID].pFeedbackId;// 非流程

					pFeedid = me->pTaskRecB->pTaskFieldB[pMe->m_iTask_fkID].pFeedbackId;
					//ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd), "nSequence = %d id =  %s , name =  %s ",me->pTaskRecB->pTaskFieldB[pMe->m_iTask_fkID].nSequence, me->pTaskRecB->pTaskFieldB[pMe->m_iTask_fkID].pFeedbackId, me->pTaskRecB->pTaskFieldB[pMe->m_iTask_fkID].pFeedbackName);
					
					//				if (STRLEN(pMe->MyMobileNo) > 0)   
					//					 SPRINTF(me->sUrl,"http://%s/task/feedback?tel=%s&imsi=%s&dispid=%s&feedid=%s&ver=1.0&remark=%s",sFormat,pMe->MyMobileNo,pMe->szMobileID,pTaskId,pFeedid,me->pTaskRecB->exp); 
					//				else
					//					SPRINTF(me->sUrl,"http://%s/task/feedback?dispid=%s&feedid=%s&ver=1.0&remark=%s",sFormat,pTaskId,pFeedid,me->pTaskRecB->exp); 

					bRtn = ITEXTCTL_GetText(pMe->textCtl_T, asText, sizeof(asText));

					MEMSET(me->pTaskRecB->exp,0,sizeof(me->pTaskRecB->exp));
					WSTRTOUTF8(asText,WSTRLEN(asText),(byte *)me->pTaskRecB->exp,sizeof(me->pTaskRecB->exp));
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd) ,"Task2: note information,exp=%s",me->pTaskRecB->exp);

						
//					if (STRLEN(pMe->MyMobileNo) > 0)   
//					{
//						 SPRINTF(me->sUrl_fk,"http://cn.richtalk.cn:80/task/feedback?tel=%s&imsi=%s&dispid=%s&feedid=%s&ver=1.0&remark=%s",pMe->MyMobileNo,pMe->szMobileID,pMe->m_Cur_TID,pFeedid,me->pTaskRecB->exp); 
//					}
//					else
//					{
//						SPRINTF(me->sUrl_fk,"http://cn.richtalk.cn:80/task/feedback?dispid=%s&feedid=%s&ver=1.0&remark=%s",pMe->m_Cur_TID,pFeedid,me->pTaskRecB->exp); 
//						
//					}
					MEMSET(me->sUrl_fk,0x00,250);
					if (STRLEN(pMe->MyMobileNo) > 0)		// 2016-1-11
					{
						//20170615
						uf_RecLog(pMe, "##### 1 ");
						SPRINTF(me->sUrl_fk,"http://%s/task/feedback",pMe->m_IP);
						SPRINTF(url_temp,"tel=%s&imsi=%s&dispid=%s&feedid=%s&ver=1.0&remark=%s",pMe->MyMobileNo,pMe->szMobileID,pMe->m_Cur_TID,pFeedid,me->pTaskRecB->exp);
					}
					else
					{
						uf_RecLog(pMe, "##### Warning: no MyMobileNo ! can not connect center ! ##### ");
						SPRINTF(me->sUrl_fk,"http://%s/task/feedback",pMe->strCenterIp);    
						SPRINTF(url_temp,"dispid=%s&feedid=%s&ver=1.0&remark=%s", pMe->m_Cur_TID,pFeedid,me->pTaskRecB->exp);

					}
					//uf_RecLog(pMe,me->sUrl_fk);

					me->iFileRead = STRLEN(url_temp);
					me->pszPostData = MALLOC( 1024 + 1 );
					MEMSET(me->pszPostData,0,1024+1);
					MEMCPY(me->pszPostData,url_temp,me->iFileRead);
					
					
					uf_RecLog(pMe,me->pszPostData);

					me->FinshorXQ = 1;
					//###任务###2015-12-28//任务发送 用上面的标志位 me->FinshorXQ
					pMe->m_bMiss = 1;				
					pMe->m_Task_fs = 1;
					
					cTaskApp_TaskInteract_fk(me);
					return TRUE;
 					//break;
				}
				
				case TASK_DETAIL_FINISH: // 完成
					uf_RecLog(pMe, "task2: press FINISH button ");	

				{

					boolean bRtn = FALSE;
					AECHAR asText[128] = {0};
					//add
					char url_temp[1024]={0};
					if (me->pszPostData != NULL) {
						FREE( me->pszPostData );
						me->pszPostData = NULL;
						me->iFileRead = 0;
					}
					
					if (me->pipPostData != NULL)
					{
						IPEEK_Release(me->pipPostData);
						me->pipPostData = NULL;
					}
					//end

// 					bRtn = ITEXTCTL_GetText(pMe->textCtl_T, asText, sizeof(asText));
// 					
// 					MEMSET(me->pTaskRecB->exp,0,sizeof(me->pTaskRecB->exp));
// 					WSTRTOUTF8(asText,WSTRLEN(asText),(byte *)me->pTaskRecB->exp,sizeof(me->pTaskRecB->exp));
// 					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd) ,"TASK_DETAIL_FINISH: note information,exp=%s",me->pTaskRecB->exp);

					MEMSET(me->sUrl_fk,0x00,250);
					if (STRLEN(pMe->MyMobileNo) > 0)		// 2013-09-23
					{
						uf_RecLog(pMe, "##### 2 ");
						SPRINTF(me->sUrl_fk,"http://%s/task/finish",pMe->m_IP);
						SPRINTF(url_temp,"tel=%s&imsi=%s&dispid=%s&ver=1.0&remark=%s",pMe->MyMobileNo,pMe->szMobileID,pMe->m_Cur_TID,me->pTaskRecB->exp);

					}
					else
					{
						uf_RecLog(pMe, "##### Err: no MyMobileNo ! can not connect center ! ##### ");
						SPRINTF(me->sUrl_fk,"http://%s/task/finish",pMe->strCenterIp);
						SPRINTF(url_temp,"dispid=%s&ver=1.0&remark=%s",pMe->m_Cur_TID,me->pTaskRecB->exp); 
					}


					//uf_RecLog(pMe,me->sUrl_fk);

					me->iFileRead = STRLEN(url_temp);
					me->pszPostData = MALLOC( 1024 + 1 );
					MEMSET(me->pszPostData,0,1024+1);
					MEMCPY(me->pszPostData,url_temp,me->iFileRead);
					
				
					uf_RecLog(pMe,me->pszPostData);

					cTaskApp_TaskInteract_fk(me);
					me->FinshorXQ = 2;
					//###任务###2015-12-28//任务发送 用上面的标志位 me->FinshorXQ	
					pMe->m_bMiss = 1;				
					pMe->m_Task_fs = 1;	
				}
					break;

				case TASK_DETAIL_RETURN:  //返回 		
					uf_RecLog(pMe, "task2: press return button");
					{
						pMe->m_WitchPag = 16; 
						pMe->m_Cur_Page = 1;
						testgps_GotoScreen( pMe, SCREENID_TASKLIST, 0 );
					}
					return TRUE;

				default:
					break;
			}
		}
		break;
	}
	return bHandled;
}


// 流程界面

boolean SamplePosDet_Detail1_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam )
{
	AEERect rRect;

	AEERect rRect_cln;
	CtlAddItem rMenuItem;
	//	AECHAR PP[20];
	// cUsePhoto 启用 视频和图片功能
	/*
	0 : 不启用照片和视频上传功能  
	1 ：启用照片 
	2 ：启用照片一键上传 
	4 : 发送视频，不发送照片 
	5： 发送视频，发送照片 
	7:  发送视频，照片一键上传 
	*/
	
	int  m_nFontHeight = 0;
	int i = 0;
	cTaskApp *me = NULL;

	boolean bHandled = FALSE;
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );

	char s_buf[100];//  测试用

	me = pMe->pTaskApp;

	if(pMe->m_bSendPF == TRUE )//用来标记是否正在发送图片的标志位 TRUE -- 表示正在发送状态  FALSE -- 表示空闲
	{
		// 说明正在发送照片 
		return TRUE;
	}

	//任务修改 
	if((pMe->textCtl_T!=NULL) 
			&& (ITEXTCTL_IsActive( pMe->textCtl_T))
				&&ITEXTCTL_HandleEvent( pMe->textCtl_T, eCode, wParam, dwParam ))
	{
		//uf_RecLog(pMe, "###   get text");
		uf_RecLog(pMe, " ######## task 1 ##### handle gettext ");
//		ITEXTCTL_Redraw( pMe->textCtl_T );
		ITEXTCTL_GetText(pMe->textCtl_T,pMe->pText,100);
		WSTRTOSTR(pMe->pText,s_buf,100);
		uf_RecLog(pMe,s_buf );
//		IDISPLAY_Update(pMe->a.m_pIDisplay); 

		return TRUE;
	}



   switch( eCode ) 
   {
   case EVT_SCREEN:
      if( wParam == SCREEN_PARAM_INIT ) 
	  {

         if( pMenuCtl ) 
		 {  /* This is not expected to happen */		

			boolean sb_tmp = FALSE;
			
			uf_RecLog(pMe, "###   IMENUCTL_DeleteAll");
			sb_tmp = IMENUCTL_DeleteAll(pMenuCtl);

			if (sb_tmp)
			{
				//uf_RecLog(pMe,"删除选项成功");

				uf_RecLog(pMe, "### This is not expected to happen!  task2:  pMenuCtl != NULL ");

				for (i=0; i<me->pTaskRecB->nFeedbackCount;i++) 
				{	// for -- 1
					AECHAR aeItem[30] = {0};
					int nLen = STRLEN(me->pTaskRecB->pTaskFieldB[i].pFeedbackName);
					UTF8TOWSTR((byte *)(me->pTaskRecB->pTaskFieldB+i)->pFeedbackName,nLen,aeItem,sizeof(aeItem));
//					IMENUCTL_AddItem( pMenuCtl, NULL, NULL, (uint16)(TASK_DETAIL_FIRST+i), aeItem, (uint32)pMenuCtl );
					IMENUCTL_AddItem( pMenuCtl, NULL, NULL, (uint16)i, aeItem, (uint32)pMenuCtl );
				}	// -- 1

				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_FINISH ,
                  i++, NULL, (uint32)pMenuCtl );

				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_BACK,
                  i++, NULL, (uint32)pMenuCtl );
			}

            testgps_DrawScreen( pMe, 0 );
            bHandled = TRUE;
         }
         else 
		 {//pMenuCtl == NULL

			 //##创建任务详情界面 底部的SOFTKEYCTL菜单
           if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&pMenuCtl ) == SUCCESS) 
		   {
				IMENUCTL_SetProperties( pMenuCtl, MP_MAXSOFTKEYITEMS | MP_NO_ARROWS );
				m_nFontHeight = IDISPLAY_GetFontMetrics( pMe->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL );
				rMenuItem.wFont = AEE_FONT_NORMAL;

				for (i=0; i<me->pTaskRecB->nFeedbackCount;i++) 
				{	// for -- 1
					AECHAR aeItem[30] = {0};
					int nLen = STRLEN(me->pTaskRecB->pTaskFieldB[i].pFeedbackName);
					UTF8TOWSTR((byte *)(me->pTaskRecB->pTaskFieldB+i)->pFeedbackName,nLen,aeItem,sizeof(aeItem));
//					IMENUCTL_AddItem( pMenuCtl, NULL, NULL, (uint16)(TASK_DETAIL_FIRST+i), aeItem, (uint32)pMenuCtl );
					IMENUCTL_AddItem( pMenuCtl, NULL, NULL, (uint16)i, aeItem, (uint32)pMenuCtl );
				}	// -- 1

				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_FINISH ,
                  i++, NULL, (uint32)pMenuCtl ); 

				IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_BACK,
                  i++, NULL, (uint32)pMenuCtl ); 


				// Set the dimensions and placement of the menu control
				//			  SETAEERECT(&rRect, 0, pMe->DeviceInfo.cyScreen - m_nFontHeight-10, pMe->DeviceInfo.cxScreen, m_nFontHeight+10);
				SETAEERECT(&rRect, 0, pMe->DeviceInfo.cyScreen - pMe->m_MenuH, pMe->DeviceInfo.cxScreen, pMe->m_MenuH);
				// SETAEERECT( &rRect, 0, ( pMe->m_rScreenRect.dy + pApp->m_rScreenRect.y ) - MAIN_MENU_HEIGHT, pApp->m_rScreenRect.dx, MAIN_MENU_HEIGHT );
				IMENUCTL_SetRect( pMenuCtl, &rRect );
				
//				IMENUCTL_SetActive( pMenuCtl, TRUE );

				// ##
				testgps_SetScreenData( pMe, (void *)pMenuCtl );

				//LIST菜单控件：任务内容/////////////////////////////////////////////////////////////////////////////////////////////////
				if (me->pMenuList_T == NULL) {
					int nErr = 0;
					nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_LISTCTL, (void **) (&(me->pMenuList_T)));
					if ( nErr != SUCCESS)   {
						//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##### task2 AEECLSID_LISTCTL create pMenuList err=%d ##### ",nErr);
						return TRUE;
					}
				}
				IMENUCTL_AddItem( me->pMenuList_T, NEWGIMIS_RES_FILE, IDS_TASK_IN, TASK_DISPLAY_CONTENT, NULL, (uint32)me->pMenuList_T);
				IMENUCTL_EnableCommand( me->pMenuList_T, TRUE );
				
				{	// 列表控件内容
					AEEMenuColors stColors;
					stColors.wMask = CLR_SYS_ITEM|CLR_SYS_ITEM_TEXT|CLR_SYS_ITEM_SEL|CLR_SYS_ITEM_SEL_TEXT;
					stColors.cBack = MAKE_RGB(199,230,248);
					stColors.cText = MAKE_RGB(9,90,136);
					stColors.cSelBack = MAKE_RGB(25,137,199);
					stColors.cSelText = MAKE_RGB(255,255,255);
					IMENUCTL_SetColors(me->pMenuList_T,&stColors);			
				}  // 列表控件内容

	
				if (!pMe->textCtl_T) 
				{	
					//##创建任务text输入编辑框
					if (ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_TEXTCTL,(void**)&pMe->textCtl_T) == SUCCESS)
					{
						ITEXTCTL_SetMaxSize(pMe->textCtl_T, TEXT_MAXSIZE);
						//设置属性,有边框
						ITEXTCTL_SetProperties(pMe->textCtl_T,TP_FRAME|TP_MULTILINE);
						//设置显示矩形区域
			 //			SETAEERECT(&rRect, 0, 30+pMe->iPhotoHeight, pMe->DeviceInfo.cxScreen, 50);  // m_nFontHeight+10
						//q3
						//SETAEERECT(&rRect, 0, (2 * pMe->m_SYSFBH) + 5*pMe->m_SYSFBH + 10, pMe->DeviceInfo.cxScreen, 2 * pMe->m_SYSFBH);  // m_nFontHeight+10
						//update: 2016-2-2
						//SETAEERECT(&rRect, 0, (2 * pMe->m_SYSFBH) + 5*pMe->m_SYSFBH + 10, pMe->DeviceInfo.cxScreen, 5 * pMe->m_SYSFBH);
						//2016-4-15
						if (STRCMP(pMe->sMobileType,"33") == 0)
						{
							SETAEERECT(&rRect, 0, (2 * pMe->m_SYSFBH) + 5*pMe->m_SYSFBH + 10, pMe->DeviceInfo.cxScreen, 5 * pMe->m_SYSFBH);
						}else{
							SETAEERECT(&rRect, 0, 6*pMe->m_SYSFBH + 10, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - (6*pMe->m_SYSFBH+10+2*pMe->m_SYSFBH) );
						}
						
						//X3输入法
//						if (STRCMP(pMe->sMobileType,"33") == 0)
//						{
//							SETAEERECT(&rRect, 0, (2 * pMe->m_SYSFBH) + 6*pMe->m_SYSFBH + 10, pMe->DeviceInfo.cxScreen,4* pMe->m_SYSFBH);
//						}else{
//							SETAEERECT(&rRect, 0, (2 * pMe->m_SYSFBH) + 5*pMe->m_SYSFBH + 10, pMe->DeviceInfo.cxScreen, 5 * pMe->m_SYSFBH);
//						}
						ITEXTCTL_SetRect(pMe->textCtl_T, &rRect);
						
						ITEXTCTL_SetInputMode(pMe->textCtl_T, AEE_TM_PINYIN);			// 拼音

				//		ITEXTCTL_SetSoftKeyMenu(pMe->textCtl_T, pMe->textmenu);

			//			ITEXTCTL_SetText(pMe->textCtl_T, pMe->pText, 0);
			//			ITEXTCTL_Redraw(pMe->textCtl_T);

						// 激活控件
						DBGPRINTF( "-----text 21" );
					//	ITEXTCTL_SetActive(pMe->textCtl_T,TRUE);
			//			ITEXTCTL_SetCursorPos(pMe->textCtl_T,TC_CURSOREND);
					}
				}

				///////////
//				IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
//				IDISPLAY_Update (pMe->a.m_pIDisplay);

               //##初始化创建好了 调用SCREEN_PARAM_PAINT
               testgps_DrawScreen( pMe, 0 );
               bHandled = TRUE;
            }
         }
      }
      else if( wParam == SCREEN_PARAM_CLOSE ) 
	  {
		 uf_RecLog(pMe, "### wParam == SCREEN_PARAM_CLOSE");
	
         IMENUCTL_Release( pMenuCtl );
		 if (pMe->textCtl_T)
		 {
			 ITEXTCTL_Release(pMe->textCtl_T);
		 }

		 //list_T 内容菜单
		 if( me->pMenuList_T != NULL)	
		 {								
			 if ( IMENUCTL_GetItemCount(me->pMenuList_T)>0 ){	
				 IMENUCTL_DeleteAll(me->pMenuList_T);  
				 IMENUCTL_Release(me->pMenuList_T);    
				 me->pMenuList_T = NULL;
			 }
		 }
		 
         testgps_SetScreenData( pMe, 0 );

         pMe->currentHandler = 0;
		 pMe->textCtl_T = NULL;
		 pMenuCtl = NULL;
         bHandled = TRUE;
      }
      else if( wParam == SCREEN_PARAM_PAINT ) 
	  {

		  ///////////
		  IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
		  IDISPLAY_Update (pMe->a.m_pIDisplay);
		  //uf_RecLog(pMe, "### wParam == SCREEN_PARAM_PAINT");

		  {
			  //关于任务派发时间
			  JulianType dDate;
			  AECHAR  asText[50] = {0};
			  char    szBuf[25] = {0};
			  GETJULIANDATE(pMe->m_Cur_nDateTime, &dDate);
			  SPRINTF(szBuf,"time: %04d-%02d-%02d %02d:%02d%",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute);
			  STRTOWSTR(szBuf, asText, sizeof(asText));
			  Task_Print_unicode(pMe,0,1,AEE_FONT_BOLD,IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL,asText);
		  }

		  
		  Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
		  //Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
		  Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);
		  
		  //x3照片输入法 显示当前输入法模式 update: 2016-2-2		
		  {
			  AEETextInputModeInfo CurIme;											
			  ITEXTCTL_GetInputMode( pMe->textCtl_T,&CurIme);									
			  yb_ShowInputMode(pMe,CurIme.tmMode,2);
		  }

		  {// 列表控件标签+列表控件内容 显示	文本内容
			  AECHAR* psTextBuf = NULL;
			  uint32  bufSize = 0;
			  uint32  nRtn;
			  AEERect rRectList;
			  //标签："点击显示内容"
			  ISHELL_GetResSize(pMe->a.m_pIShell,NEWGIMIS_RES_FILE, IDS_TASK_CONTENT,RESTYPE_STRING,&bufSize);
			  psTextBuf = (AECHAR*)MALLOC((bufSize+1)*sizeof(AECHAR));
			  nRtn = ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE,IDS_TASK_CONTENT, psTextBuf,bufSize);
			  
			  if (nRtn >0 )
				  rRectList = Task_Print_unicode( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,psTextBuf);
			  else
				  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_DisplayCtlList in LoadResString nRtn=%d",nRtn);
			  FREE(psTextBuf);
			  
			  //菜单list控件显示
			  rRectList.x += rRectList.dx + 3;
			  rRectList.dx = pMe->DeviceInfo.cxScreen - rRectList.x - 5;
			  
			  rRectList.dy = pMe->m_SYSFBH +MENULIST_HEIGHT; //update: 2016-2-2
			  IMENUCTL_SetRect( me->pMenuList_T, &rRectList );
			  //IMENUCTL_Redraw( me->pMenuList );
			}

		  {	//备注文字
			  int nRtn;
			  int nLine;
			  int nExpLen;
			  AECHAR szText[128];
			  
			  AECHAR temp[10] = {0x2e, 0x2e, 0x2e,0x0000};

			  
			  //nLine = (me->pTaskRecB->bFlow) ? 4 : 5;
			  nRtn = ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_TASK_BZ, szText,sizeof(szText));
			  if (nRtn > 0)
			  {
				  Task_Print_unicode( pMe, 4, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,szText);
			  }
		  }

		  
		  //ITEXTCTL_SetActive(pMe->textCtl_T,TRUE);
		  //ITEXTCTL_SetActive(pMe->textCtl_T,FALSE);
		  IMENUCTL_SetActive(me->pMenuList_T, TRUE);
		  if( IMENUCTL_IsActive( pMenuCtl ) ) 
		  {
			  IMENUCTL_SetActive( pMenuCtl, FALSE );
			  IMENUCTL_SetActive( me->pMenuList_T, FALSE);
		  }
		  else if ( ITEXTCTL_IsActive(pMe->textCtl_T) )
		  {
			  IMENUCTL_SetActive( pMenuCtl, FALSE );
			  IMENUCTL_SetActive( me->pMenuList_T, FALSE);
		  }
		  if (pMe->textCtl_T)
		  {
			  ITEXTCTL_Redraw(pMe->textCtl_T); 
		  }
		  IMENUCTL_Redraw( pMenuCtl );  // 这句后面加的
		  IMENUCTL_Redraw( me->pMenuList_T );

      }
     
	  else if (wParam == XTRA_EVT_USER_DISPLAY)
	  {
			//uf_RecLog(pMe, "## task1 XTRA_EVT_USER_DISPLAY) ##");

			//###任务###2015-12-28
			//任务发送 用上面的标志位 me->FinshorXQ
			pMe->m_bMiss = 1;
			//pMe->m_Task_fs = 1;
			uf_AEPrint_Sending(pMe,FALSE);  // 显示发送中


			if (1 == me->m_nState)//详情成功，反馈成功
			{
				//1.5秒后显示"发送成功" & 震动
				//	uf_RecLog(pMe, " ### 1.5s later display..success");
				uf_RecLog(pMe, "task1 XTRA_EVT_USER_DISPLAY: 1)");
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)Display_Success, pMe);
				me->m_nState = 0;
			}else if (2 == me->m_nState) //反馈失败
			{
				uf_RecLog(pMe, "task1 XTRA_EVT_USER_DISPLAY: 2)");
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)Display_false, pMe);
				me->m_nState = 0;
			}else if (3 == me->m_nState)//反馈超时
			{
				uf_RecLog(pMe, "task1 XTRA_EVT_USER_DISPLAY: 3)");
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)Display_timeout, pMe);
				me->m_nState = 0;
			}else if (4 == me->m_nState)//获取子任务失败
			{
				uf_RecLog(pMe, "task1 XTRA_EVT_USER_DISPLAY: 4)");
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)Display_getTaskErr, pMe);
				me->m_nState = 0;				
			}else if (5 == me->m_nState)//获取子任务超时
			{
				uf_RecLog(pMe, "task1 XTRA_EVT_USER_DISPLAY: 5)");
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)Display_getTaskTimeout, pMe);
				me->m_nState = 0;				
			}else if (6 == me->m_nState)//反馈异常
			{
				uf_RecLog(pMe, "task1 XTRA_EVT_USER_DISPLAY: 6 :Display_fkweb_error");
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)Display_fkweb_error, pMe);
				//me->m_nState = 0;				
			}else{
				uf_RecLog(pMe, "### task1 XTRA_EVT_USER_DISPLAY: 7 ERR! ###");
			}

			
			pMe->m_Task_fs = 0;
			// 反馈异常时候跳转到任务列表 [3/8/2016 Administrator]-----------------------------------
			if (6 == me->m_nState)
			{
				me->m_nState = 0;
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000 + 3500, (PFNNOTIFY)uf_ReturnMenu_TaskList, pMe);//停止震动 返回任务详情界面
			}
			else
			{
				ISHELL_SetTimer(pMe->a.m_pIShell, 2000 + 3500, (PFNNOTIFY)uf_ReturnMenu_xq, pMe);//停止震动 返回任务详情界面
			}
			//-----------------------------------------------------------------------------------------
			//ISHELL_SetTimer(pMe->a.m_pIShell, 2000 + 3500, (PFNNOTIFY)uf_ReturnMenu_xq, pMe);//停止震动 返回任务详情界面
		//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ISHELL_PostEvent ###");			
		//	ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0);
		
	  }

		
		break;

   case EVT_KEY:
	   {
		   //这里是在任务明细界面，按键控制。在text和底部menu切换 监听
			if(wParam == AVK_SELECT)
			{
				if (ITEXTCTL_IsActive( pMe->textCtl_T))
				{
					ITEXTCTL_SetActive( pMe->textCtl_T,FALSE);
					IMENUCTL_SetActive(pMenuCtl,TRUE);
					return TRUE;
				}
				else if (IMENUCTL_IsActive( me->pMenuList_T ) )
				{
					pMe->m_WitchPag = 19; // 任务内容界面
					testgps_GotoScreen( pMe, SCREENID_CONTENT, 0 );
					return TRUE;
				}
			}

			if (wParam == AVK_DOWN)
			{
				if ( IMENUCTL_IsActive(me->pMenuList_T) )
				{
					IMENUCTL_SetActive(pMenuCtl,FALSE);
					IMENUCTL_SetActive( me->pMenuList_T, FALSE);
					ITEXTCTL_SetActive( pMe->textCtl_T,TRUE);
					ITEXTCTL_SetCursorPos(pMe->textCtl_T, TC_CURSOREND);
					return TRUE;
				}
				else if ( IMENUCTL_IsActive(pMenuCtl) )
				{
					IMENUCTL_SetActive(pMenuCtl,FALSE);
					ITEXTCTL_SetActive( pMe->textCtl_T,FALSE);
					IMENUCTL_SetActive( me->pMenuList_T, TRUE);
					return TRUE;
				}
			}

			if (wParam == AVK_UP)
			{
				if ( IMENUCTL_IsActive(me->pMenuList_T) )
				{
					IMENUCTL_SetActive(pMenuCtl,TRUE);
					ITEXTCTL_SetActive( pMe->textCtl_T,FALSE);
					IMENUCTL_SetActive( me->pMenuList_T, FALSE);
					return TRUE;
				}
				else if ( ITEXTCTL_IsActive(pMe->textCtl_T) )
				{
					IMENUCTL_SetActive(pMenuCtl,FALSE);
					ITEXTCTL_SetActive( pMe->textCtl_T,FALSE);
					IMENUCTL_SetActive( me->pMenuList_T, TRUE);
					return TRUE;
				}
				else
				{
					IMENUCTL_SetActive(pMenuCtl,FALSE);
					IMENUCTL_SetActive( me->pMenuList_T, FALSE);
					ITEXTCTL_SetActive( pMe->textCtl_T,TRUE);
					ITEXTCTL_SetCursorPos(pMe->textCtl_T, TC_CURSOREND);
					return TRUE;
				}
			}
			
			if (ITEXTCTL_IsActive( pMe->textCtl_T))
			{	
				bHandled = ITEXTCTL_HandleEvent( pMe->textCtl_T, eCode, wParam, dwParam ); //这里监听不到输入法的输入  这是做什么啊？？
				//uf_RecLog(pMe, " ###### ERR:  handle gettext 监听不到 ");
				
				ITEXTCTL_Redraw( pMe->textCtl_T );	//用于指示文本控件对象刷新其内容
				ITEXTCTL_GetText(pMe->textCtl_T,pMe->pText,100);
				WSTRTOSTR(pMe->pText,s_buf,100);
				uf_RecLog(pMe,s_buf );
				//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"#####################  ITEXTCTL_GetText(pMe->textCtl_T,pMe->pText =##########");
				
				IDISPLAY_Update(pMe->a.m_pIDisplay);  //更新屏幕
				
				return TRUE;
			}
			
			if ( IMENUCTL_IsActive(pMenuCtl) )
			{
				bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
			}

			if (wParam == AVK_SOFT2)	
			{
				if ( pMe->m_Right_Key == '1' )
				{
					// 这里直接回到后台
					if (pMenuCtl)
					{
						IMENUCTL_Release( pMenuCtl );
						pMenuCtl = NULL;
					}
					if (pMe->textCtl_T)
					{
						ITEXTCTL_Release(pMe->textCtl_T);
					}
					
					pMe->textCtl_T = NULL;
					pMenuCtl = NULL;
					
					pMe->currentHandler = 0;
					testgps_SetScreenData( pMe, 0 );
					
					//	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
					pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
					ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
					pMe->m_FOBFlag = TRUE; //在后台运行
					pMe->m_WitchPag = 0;
					//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"后台运行 6:%d",pMe->m_WitchPag);
					bHandled = TRUE;
					
					pMe->m_Cur_Page = 1;
					return TRUE;
				}
			}

			if (wParam == AVK_END)
			{
				if (STRCMP(pMe->sMobileType,"55") == 0 || (STRCMP(pMe->sMobileType,"03") == 0)  || (STRCMP(pMe->sMobileType,"58") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0) || (STRCMP(pMe->sMobileType,"57") == 0) )
				{
					// 这里直接回到后台
					if (pMenuCtl)
					{
						IMENUCTL_Release( pMenuCtl );
						pMenuCtl = NULL;
					}
					if (pMe->textCtl_T)
					{
						ITEXTCTL_Release(pMe->textCtl_T);
					}
					
					pMe->textCtl_T = NULL;
					pMenuCtl = NULL;
					
					pMe->currentHandler = 0;
					testgps_SetScreenData( pMe, 0 );
					
					//	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
					pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
					ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
					pMe->m_FOBFlag = TRUE; //在后台运行
					pMe->m_WitchPag = 0;
					//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"后台运行 6:%d",pMe->m_WitchPag);
					bHandled = TRUE;
					
					pMe->m_Cur_Page = 1;
					return TRUE;
				}
			}
			
		   if( wParam == AVK_END  || wParam == AVK_CLR ) 
		   {
			   pMe->m_Cur_Page = 1;
			   pMe->m_WitchPag = 16; // 返回主界面
			   testgps_GotoScreen( pMe, SCREENID_TASKLIST, 0 );
			   pMe->m_WitchPag = 16;
			   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"返回任务界面 2:%d",pMe->m_WitchPag);
			   bHandled = TRUE;
		   }
	   }

	   
		
	   break;

   case EVT_COMMAND:

      if( dwParam == (uint32)pMenuCtl ) 
	  {
		  int si_tmp = 0; // 临时存放获取菜单的个数 如果是只有两项 则第一项为完成 第二项为返回 如果是三项则 任务 完成 返回
		  //uf_RecLog(pMe, "进来这里 1");
		  //ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"当前的菜单项 = %d", wParam );
		  si_tmp = IMENUCTL_GetItemCount( pMenuCtl );
		  switch( wParam ) 
		  {
		  case 0:  // 说明是上报反馈数据
			  {
				  //add
				  char url_temp[1024]={0};
				  int  nTextLen = 0;
				  if (me->pszPostData != NULL) {
					  FREE( me->pszPostData );
					  me->pszPostData = NULL;
					  me->iFileRead = 0;
				  }
				  
				  if (me->pipPostData != NULL)
				  {
					  IPEEK_Release(me->pipPostData);
					  me->pipPostData = NULL;
				  }
				  //end
				  
				  MEMSET(me->sUrl_fk,0x00,250);
				  if (si_tmp == 2)  // ## "完成"键
				  {
					  if (STRLEN(pMe->MyMobileNo) > 0)   // 2013-09-23
						  SPRINTF(me->sUrl_fk,"http://%s/task/finish?tel=%s&imsi=%s&dispid=%s&ver=1.0&remark=%s",pMe->m_IP,pMe->MyMobileNo,pMe->szMobileID,pMe->m_Cur_TID,me->pTaskRecB->exp); 
					  else
						  SPRINTF(me->sUrl_fk,"http://%s/task/finish?dispid=%s&ver=1.0&remark=%s",pMe->m_IP,pMe->m_Cur_TID,me->pTaskRecB->exp); 

					  me->FinshorXQ = 2;
				  }
				  else if (si_tmp == 3)  //## 子任务完成上报
				  {
					  // 设置反馈地址
					  // SPRINTF(me->sUrl,"http://cn.richtalk.cn:80/task/list?tel=%s&imsi=%s",pMe->MyMobileNo,pMe->szMobileID);
					  char *pFeedid = NULL;
					  boolean bRtn = FALSE;
					  AECHAR asText[128] = {0};
					  pFeedid = me->pTaskRecB->pTaskFieldB[0].pFeedbackId;
					  
					  bRtn = ITEXTCTL_GetText(pMe->textCtl_T, asText, sizeof(asText));
					  MEMSET(me->pTaskRecB->exp,0,sizeof(me->pTaskRecB->exp));
					  WSTRTOUTF8(asText,WSTRLEN(asText),(byte *)me->pTaskRecB->exp,sizeof(me->pTaskRecB->exp));
					  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd) ,"Task note information,exp=%s",me->pTaskRecB->exp);
					  
					  if (STRLEN(pMe->MyMobileNo) > 0)   
					  {
						  
						  SPRINTF(me->sUrl_fk,"http://%s/task/feedback", pMe->m_IP); 
						  SPRINTF(url_temp,"tel=%s&imsi=%s&dispid=%s&feedid=%s&ver=1.0&remark=%s",pMe->MyMobileNo,pMe->szMobileID,pMe->m_Cur_TID,pFeedid,me->pTaskRecB->exp); 
					  }
					  else
					  {
						  SPRINTF(me->sUrl_fk,"http://%s/task/feedback",pMe->m_IP); 
						  SPRINTF(url_temp,"dispid=%s&feedid=%s&ver=1.0&remark=%s",pMe->m_Cur_TID,pFeedid,me->pTaskRecB->exp); 
						  
					  }
					  
					  me->FinshorXQ = 1;

					  nTextLen = WSTRLEN(asText);
					  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"test nTextLen = %d", nTextLen );
					  nTextLen = WSTRLEN(asText) * sizeof(AECHAR);
				      ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"test nTextLen 2 = %d", nTextLen );
				  }


				  
				  me->iFileRead = STRLEN(url_temp);
				  me->pszPostData = MALLOC( 1024 + 1 );
				  MEMSET(me->pszPostData,0,1024+1);
				  MEMCPY(me->pszPostData,url_temp,me->iFileRead);
				  
				  uf_RecLog(pMe,me->sUrl_fk);
				  uf_RecLog(pMe,me->pszPostData);	

				  //###任务###2015-12-28//任务发送 用上面的标志位 me->FinshorXQ
				  pMe->m_bMiss = 1;				
				  pMe->m_Task_fs = 1;
				  //	 uf_RecLog(pMe, "display sending...");
				  // uf_AEPrint_Sending(pMe,FALSE);  // 显示发送中
				  
				  cTaskApp_TaskInteract_fk(me);
				  return TRUE;
			  }
			  break;
		  case 1: // 完成
			  {
				  if (si_tmp == 2)//##返回键
				  {
					  pMe->m_WitchPag = 16; 
					  pMe->m_Cur_Page = 1;
					  testgps_GotoScreen( pMe, SCREENID_TASKLIST, 0 );
				  }
				  else if (si_tmp == 3)//##完成键
				  {
					  //add
					  char url_temp[1024]={0};
					  if (me->pszPostData != NULL) {
						  FREE( me->pszPostData );
						  me->pszPostData = NULL;
						  me->iFileRead = 0;
					  }
					  
					  if (me->pipPostData != NULL)
					  {
						  IPEEK_Release(me->pipPostData);
						  me->pipPostData = NULL;
					  }
					//end	
					
					  MEMSET(me->sUrl_fk,0x00,250);
					  if (STRLEN(pMe->MyMobileNo) > 0)		
					  {
						  SPRINTF(me->sUrl_fk,"http://%s/task/finish?tel=%s&imsi=%s&dispid=%s&ver=1.0&remark=%s",pMe->m_IP,pMe->MyMobileNo,pMe->szMobileID,pMe->m_Cur_TID,me->pTaskRecB->exp);
					  }
					  else
					  {
						  uf_RecLog(pMe, "##### Err: no MyMobileNo ! can not connect center ! ##### ");
						  SPRINTF(me->sUrl_fk,"http://%s/task/finish?dispid=%s&ver=1.0&remark=%s",pMe->strCenterIp,pMe->m_Cur_TID,me->pTaskRecB->exp);   //		SPRINTF(me->sUrl,"http://%s/message/getMessage?ver=1.0",sFormat); 
					  }			
					  
					  me->iFileRead = STRLEN(url_temp);
					  me->pszPostData = MALLOC( 1024 + 1 );
					  MEMSET(me->pszPostData,0,1024+1);
					  MEMCPY(me->pszPostData,url_temp,me->iFileRead);
					  
					  uf_RecLog(pMe,me->sUrl_fk);
					 uf_RecLog(pMe,me->pszPostData);
					  

					  cTaskApp_TaskInteract_fk(me);
					  me->FinshorXQ = 2;

					  //###任务###2015-12-28//任务发送 用上面的标志位 me->FinshorXQ	
					  pMe->m_bMiss = 1;				
					  pMe->m_Task_fs = 1;	
					  
				  }
				  
			  }
			  break;
		  case 2:  //返回 
			  {
				  pMe->m_WitchPag = 16; 
				  pMe->m_Cur_Page = 1;
				  testgps_GotoScreen( pMe, SCREENID_TASKLIST, 0 );
				  
			  }
			  
			  return TRUE;
		  default:
			  break;
         }
      }
      break;
   }
   return bHandled;
}

// 非流程 这个没有用，不是这个。。。
boolean SamplePosDet_Detail222_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam )
{
   boolean bHandled = FALSE;
   IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );
   IMenuCtl *pMenuCtl_T = testgps_GetScreenData_task( pMe );
   int si_Ret = 0;
   int i_nID = 0;
   int i_nItemID = 0;

   switch( eCode ) {
   case EVT_SCREEN:
      if( wParam == SCREEN_PARAM_INIT ) {
         if( pMenuCtl ) {  /* This is not expected to happen */
            testgps_DrawScreen( pMe, 0 );
            bHandled = TRUE;
         }
         else 
		 {
				AECHAR aeItem[512] = {0};
				cTaskApp *me;
				ITaskFieldA *s_tmpT = NULL;
				pList s_list = NULL;
				int nLen = 0;
				AEERect rRect;
				int s_i = 0;
				char s_t[15] = {0};
				AECHAR aes_tmp[30] ={0};
				me = pMe->pTaskApp;
				
				i_nID = 0;
				s_list = me->p_head;
				if (me->p_head)
				{
					
					pMe->m_TTotal = LIST_Toatal(me->p_head); // 返回当前总的记录数 也就是任务数
					
					pMe->m_Page_c = GetTotalPage(pMe->m_TTotal,pMe->m_TaskC);
				
					if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_MENUCTL, (void **)&pMenuCtl ) == SUCCESS &&
						ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL, (void **)&pMenuCtl_T ) == SUCCESS ) 
					{
						IMENUCTL_SetProperties( pMenuCtl_T, MP_MAXSOFTKEYITEMS | MP_NO_ARROWS );

						// pMe->m_TaskC 任务个数 的变量 
				//	SETAEERECT(&rRect, 0, pMe->m_SYSFBH * 5 + 5, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - (pMe->m_SYSFBH * 5) - 5);
					SETAEERECT(&rRect, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - pMe->m_MenuH - 10);

			        IMENUCTL_SetRect(pMenuCtl,&rRect);

					IMENUCTL_AddItem( pMenuCtl_T, NEWGIMIS_RES_FILE, IDS_FRONT_PIC,
                  0, NULL, (uint32)pMenuCtl_T );

				IMENUCTL_AddItem( pMenuCtl_T, NEWGIMIS_RES_FILE, IDS_NEXT_PIC,
                  1, NULL, (uint32)pMenuCtl_T );
					SETAEERECT(&rRect, 0, pMe->DeviceInfo.cyScreen - pMe->m_MenuH, pMe->DeviceInfo.cxScreen, pMe->m_MenuH);
					IMENUCTL_SetRect(pMenuCtl_T,&rRect);
					
					pMe->m_TaskID = 0;

					
					while(s_list)
					{
						// 说明里面有内容
						
						for (s_i = 0;s_i <512;s_i++)
						{
							aeItem[s_i] = 0;
						}
						s_tmpT = (ITaskFieldA *)(s_list->pData);
						nLen = STRLEN(s_tmpT->pTaskName);
						

						UTF8TOWSTR((byte *)(s_tmpT->pTaskName),nLen,aeItem,sizeof(aeItem));

						// IMENUCTL_AddItem( me->pMenuTask, NULL, NULL, (uint16)(TASK_ITEM_FIRST+idx), aeItem, (uint32)me );
						IMENUCTL_AddItem( pMenuCtl, NULL, NULL, i_nID, aeItem, (uint32)pMenuCtl);
						i_nID++;
						
						s_list = (pList )s_list->pNext;

						if (i_nID % pMe->m_TaskC == 0)
						{
							// 说明一页已经存满了
							s_list = NULL;
							break;
						}
					}

				   /////////   end ////
					MEMSET(s_t,0x0,15);
					SPRINTF(s_t,"task %d/%d",pMe->m_Cur_Page,pMe->m_Page_c);
					STRTOWSTR(s_t,aes_tmp,sizeof(aes_tmp));
					IMENUCTL_SetTitle( pMenuCtl, NULL, NULL, aes_tmp );
			//	   IMENUCTL_SetTitle( pMenuCtl, NEWGIMIS_RES_FILE, IDS_MORE, NULL );
					}

					testgps_SetScreenData( pMe, (void *)pMenuCtl );
					testgps_SetScreenData_task(pMe, (void *)pMenuCtl_T);
					testgps_DrawScreen( pMe, 0 );
					bHandled = TRUE;
				   
				}
				
         }
      }
      else if( wParam == SCREEN_PARAM_CLOSE ) {
		  if (pMenuCtl)
		  {
			  IMENUCTL_Release( pMenuCtl );
		  }
         
         testgps_SetScreenData( pMe, 0 );

		 if (pMenuCtl_T)
		 {
		   IMENUCTL_Release( pMenuCtl_T );
		 }
	   
	    pMenuCtl_T = NULL;

	    testgps_SetScreenData_task( pMe, 0 );

         pMe->currentHandler = 0;
         bHandled = TRUE;
      }

	  /*
	  else if( wParam == SCREEN_PARAM_PAINT ) {
		  IMENUCTL_SetActive( pMenuCtl, TRUE );

        testgps_GetGPSInfo_Paint( pMe, (GetGPSInfo_PaintRegions)dwParam );
		IMENUCTL_Redraw( pMenuCtl );
		 
         bHandled = TRUE;
      }
	  */
      else if( wParam == SCREEN_PARAM_PAINT ) 
	  {

		  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	
         if( IMENUCTL_IsActive( pMenuCtl_T ) ) {
            IMENUCTL_Redraw( pMenuCtl );
			
         }
		 else
		 {
            IMENUCTL_SetActive( pMenuCtl, TRUE );  /* Also causes a menu draw */			 
		 }
		 IMENUCTL_Redraw( pMenuCtl );
		 IMENUCTL_Redraw(pMenuCtl_T);
      }

      break;

   case EVT_KEY:
//	   bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
//	   i_nItemID = IMENUCTL_GetSel(pMenuCtl);


	   if( IMENUCTL_IsActive( pMenuCtl ) ) 
	   {
		   bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
		   i_nItemID = IMENUCTL_GetSel(pMenuCtl);

		   if (wParam == AVK_UP || wParam == AVK_DOWN)
		   {
			   if (pMe->m_TaskID == i_nItemID)
			   {
					IMENUCTL_SetActive( pMenuCtl, FALSE );
					IMENUCTL_SetActive( pMenuCtl_T, TRUE );
			   }
			   pMe->m_TaskID = i_nItemID;
		   }
	   }

	   else if( IMENUCTL_IsActive( pMenuCtl_T ) ) 
	   {
		   bHandled = IMENUCTL_HandleEvent( pMenuCtl_T, eCode, wParam, dwParam );
//		   i_nItemID = IMENUCTL_GetSel(pMenuCtl_T);
		   if (wParam == AVK_UP || wParam == AVK_DOWN)
		   {
//			   if (pMe->m_TaskID == i_nItemID)
//			   {
				IMENUCTL_SetActive( pMenuCtl_T, FALSE );
				IMENUCTL_SetActive( pMenuCtl, TRUE );
//			   }
//			   pMe->m_TaskID = i_nItemID;
		   }

	   }

		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "二级菜单 bHandled = %d",bHandled);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "二级菜单 i_nItemID = %d",i_nItemID);
	   // AVK_POWER   AVK_END
		if (wParam == AVK_SOFT2)	
		{
			if ( pMe->m_Right_Key == '1' )
			{
				// 这里直接回到后台
				if (pMenuCtl)
				{
					IMENUCTL_Release( pMenuCtl );
				}
				
				pMenuCtl = NULL;
				
				
				pMe->currentHandler = 0;
				testgps_SetScreenData( pMe, 0 );
				
				if (pMenuCtl_T)
				{
					IMENUCTL_Release( pMenuCtl_T );
				}
				
				pMenuCtl_T = NULL;
				testgps_SetScreenData_task( pMe, 0 );
				
				
				//	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
				pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
				ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
				pMe->m_FOBFlag = TRUE; //在后台运行
				pMe->m_WitchPag = 0;
				//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"后台运行 6:%d",pMe->m_WitchPag);
				bHandled = TRUE;
				   return TRUE;
			}
	   }

	   	//////////////////////////////////////
		//////////////////////////////////////
		if (wParam == AVK_END)
		{
			if (STRCMP(pMe->sMobileType,"55") == 0  || (STRCMP(pMe->sMobileType,"03") == 0)  || (STRCMP(pMe->sMobileType,"58") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0) || (STRCMP(pMe->sMobileType,"57") == 0) )
			{
				/*
				if (pMenuCtl)
				  {
					  IMENUCTL_Release( pMenuCtl );
				  }
		 
				*/
				// 这里直接回到后台
				  if (pMenuCtl)
				  {
					  IMENUCTL_Release( pMenuCtl );
				  }
				   
				   pMenuCtl = NULL;
				   

				   pMe->currentHandler = 0;
				   testgps_SetScreenData( pMe, 0 );

				   if (pMenuCtl_T)
				  {
					  IMENUCTL_Release( pMenuCtl_T );
				  }
				   
				   pMenuCtl_T = NULL;
				   testgps_SetScreenData_task( pMe, 0 );


				   //	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
				   pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
				   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
				   pMe->m_FOBFlag = TRUE; //在后台运行
				   pMe->m_WitchPag = 0;
		//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"后台运行 6:%d",pMe->m_WitchPag);
				   bHandled = TRUE;
				   return TRUE;
			}
		}
		//////////////////////////////////////
		//////////////////////////////////////





       if( wParam == AVK_END  || wParam == AVK_CLR ) 
	   {

		   pMe->m_WitchPag = 4; // 返回更多的那个界面
		   testgps_GotoScreen( pMe, SCREENID_MORE, 0 );

		   pMe->m_WitchPag = 4;
//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"更多界面 页返回主界面 2:%d",pMe->m_WitchPag);
		   bHandled = TRUE;
	   }

/*
		 if( wParam == AVK_END  || wParam == AVK_CLR ) {

		   pMe->m_WitchPag = 4; // 返回更多的那个界面
		   testgps_GotoScreen( pMe, SCREENID_MORE, 0 );

		   pMe->m_WitchPag = 4;
		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"更多界面 页返回主界面 2:%d",pMe->m_WitchPag);
		   bHandled = TRUE;
	   }
*/

		
	   break;
	   /*
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
		  bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
		  if( wParam == AVK_END  || wParam == AVK_CLR ) {
			  ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
			  pMe->m_FOBFlag = TRUE; //在后台运行
			  pMe->m_WitchPag = 0;
			  bHandled = TRUE;
	   }
      break;
	  */

/*
	int i = me->pTaskRecsA->nSelRec;
	pTaskId =  me->pTaskRecsA->pTaskFieldA[i].pTaskId;
	if (STRLEN(pMe->MyMobileNo) > 0)   // 2013-09-23
		 SPRINTF(me->sUrl,"http://%s/task/detail?tel=%s&imsi=%s&dispid=%s&ver=1.0",sFormat,pMe->MyMobileNo,pMe->szMobileID,pTaskId); 
	else
		SPRINTF(me->sUrl,"http://%s/task/detail?dispid=%s&ver=1.0",sFormat,pTaskId); 
*/

   case EVT_COMMAND:
      if( dwParam == (uint32)pMenuCtl ) 
	  {
		  cTaskApp *me;
		  me = pMe->pTaskApp;
		  LIST_GetTaskID(pMe, me->p_head , (pMe->m_Cur_Page - 1) * pMe->m_TaskC + wParam);
//		  cTaskApp_TaskInteract_fk(me);
 
	  }

	  else if( dwParam == (uint32)pMenuCtl_T ) 
	  {
         switch( wParam ) {
         case 0:  //上一页
			 {
				 if (pMe->m_Cur_Page > 1)
				 {

					 cTaskApp *me;
					 pList s_list = NULL;


					 AECHAR aeItem[512] = {0};

					ITaskFieldA *s_tmpT = NULL;
					int nLen = 0;
					AEERect rRect;
					int s_i = 0;
					char s_t[15] = {0};
					AECHAR aes_tmp[30] ={0};


					 me = pMe->pTaskApp;
					 pMe->m_TaskID = 0;
					 s_list = LIST_GetNode(me->p_head , (pMe->m_Cur_Page-2) * pMe->m_TaskC);
					 pMe->m_Cur_Page--;
					 
					 // 删除原来的 的
					 for (s_i = pMe->m_TaskC ; s_i > 0;s_i--)
					 {
						 IMENUCTL_DeleteItem(pMenuCtl,s_i-1);
						
					 }
					 
					
					
					
					while(s_list)
					{
						// 说明里面有内容
						
						for (s_i = 0;s_i <512;s_i++)
						{
							aeItem[s_i] = 0;
						}
						s_tmpT = (ITaskFieldA *)(s_list->pData);
						nLen = STRLEN(s_tmpT->pTaskName);
						

						UTF8TOWSTR((byte *)(s_tmpT->pTaskName),nLen,aeItem,sizeof(aeItem));

						// IMENUCTL_AddItem( me->pMenuTask, NULL, NULL, (uint16)(TASK_ITEM_FIRST+idx), aeItem, (uint32)me );
						IMENUCTL_AddItem( pMenuCtl, NULL, NULL, i_nID, aeItem, (uint32)pMenuCtl);
						i_nID++;
						
						s_list = (pList )s_list->pNext;

						if (i_nID % pMe->m_TaskC == 0)
						{
							// 说明一页已经存满了
							s_list = NULL;
							break;
						}
					}

				   /////////   end ////
					MEMSET(s_t,0x0,15);
					SPRINTF(s_t,"task %d/%d",pMe->m_Cur_Page,pMe->m_Page_c);
					STRTOWSTR(s_t,aes_tmp,sizeof(aes_tmp));
					IMENUCTL_SetTitle( pMenuCtl, NULL, NULL, aes_tmp );
					IMENUCTL_SetSel(pMenuCtl,0);
			//	   IMENUCTL_SetTitle( pMenuCtl, NEWGIMIS_RES_FILE, IDS_MORE, NULL );
				}

					testgps_DrawScreen( pMe, 0 );
					bHandled = TRUE;
				 
			 }
			 return TRUE;
		 case 1: // 下一页

		//	 pMe->m_Cur_Page,pMe->m_Page_c
			 {
				 if (pMe->m_Cur_Page < pMe->m_Page_c)
				 {

					 cTaskApp *me;
					 pList s_list = NULL;


					 AECHAR aeItem[512] = {0};

					ITaskFieldA *s_tmpT = NULL;
					int nLen = 0;
					AEERect rRect;
					int s_i = 0;
					char s_t[15] = {0};
					AECHAR aes_tmp[30] ={0};


					 me = pMe->pTaskApp;
					 pMe->m_TaskID = 0;
					 s_list = LIST_GetNode(me->p_head , pMe->m_Cur_Page * pMe->m_TaskC);
					 pMe->m_Cur_Page++;
					 
					 // 删除原来的 的
					 for (s_i = pMe->m_TaskC ; s_i > 0;s_i--)
					 {
						 IMENUCTL_DeleteItem(pMenuCtl,s_i-1);
						 
					 }
					 
					
					
					
					while(s_list)
					{
						// 说明里面有内容
						
						for (s_i = 0;s_i <512;s_i++)
						{
							aeItem[s_i] = 0;
						}
						s_tmpT = (ITaskFieldA *)(s_list->pData);
						nLen = STRLEN(s_tmpT->pTaskName);
						

						UTF8TOWSTR((byte *)(s_tmpT->pTaskName),nLen,aeItem,sizeof(aeItem));

						// IMENUCTL_AddItem( me->pMenuTask, NULL, NULL, (uint16)(TASK_ITEM_FIRST+idx), aeItem, (uint32)me );
						IMENUCTL_AddItem( pMenuCtl, NULL, NULL, i_nID, aeItem, (uint32)pMenuCtl);
						i_nID++;
						
						s_list = (pList )s_list->pNext;

						if (i_nID % pMe->m_TaskC == 0)
						{
							// 说明一页已经存满了
							s_list = NULL;
							break;
						}
					}

				   /////////   end ////
					MEMSET(s_t,0x0,15);
					SPRINTF(s_t,"task %d/%d",pMe->m_Cur_Page,pMe->m_Page_c);
					STRTOWSTR(s_t,aes_tmp,sizeof(aes_tmp));
					IMENUCTL_SetTitle( pMenuCtl, NULL, NULL, aes_tmp );
					IMENUCTL_SetSel(pMenuCtl,0);
			//	   IMENUCTL_SetTitle( pMenuCtl, NEWGIMIS_RES_FILE, IDS_MORE, NULL );
					}

					testgps_DrawScreen( pMe, 0 );
					bHandled = TRUE;
				 }
			 }
			 return TRUE;
		 }
      break;
   }
   return bHandled;
}



 /// 以下与详情有光

  // =================================================================
// 与中心端交互任务的详情
void cTaskApp_TaskInteract_fk(cTaskApp *me)
{
	int itemp = 0;
	newgimis *pMe;
	char cStatus;
	int iLen = 0;
	ISourceUtil *pisu;
	WebOpt awo[4]; 
	if (!me) return;
	pMe = me->m_pParent;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_TaskInteract_fk in ,cTaskStatus=%d",me->cTaskStatus);

	cTaskApp_CloseWeb_fk(me);

	uf_AEPrint_Sending(pMe,FALSE);  // 显示发送中
	
//	SPRINTF(me->sUrl_fk,"http://cn.richtalk.cn:80/task/detail?tel=%s&imsi=%s&dispid=%s&ver=1.0",pMe->MyMobileNo,pMe->szMobileID,pMe->m_Cur_TID);
	uf_RecLog(pMe,me->sUrl_fk);

	if (me->pipPostData != NULL) {
		IPEEK_Release(me->pipPostData);
		me->pipPostData = NULL;
	}

	if ((char *)0 != me->pszPostData) {
		  
		  if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pisu)) 
		  {
			  iLen = me->iFileRead;
			  itemp = ISOURCEUTIL_PeekFromMemory(pisu, me->pszPostData, iLen, 0, 0,&me->pipPostData);
			  ISOURCEUTIL_Release(pisu);

			  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_TaskInteract_fk ISOURCEUTIL_PeekFromMemory = %d",itemp);
		  }
	}


	//超时标志位清0
	me->cTimeout_fk = 0;

    // Add Options
	awo[0].nId  = WEBOPT_CONNECTTIMEOUT;
    awo[0].pVal = (void *)20000;			// 2013-03-27  5000 
 // 2.6.2D2   awo[1].nId  = WEBOPT_FLAGS;
 // 2.6.2D2   awo[1].pVal = (void *)WEBREQUEST_NOWAITCONN;
    awo[1].nId  = WEBOPT_PROXYSPEC;
    awo[1].pVal = (void *)"http:///http://10.0.0.200:80";
    awo[2].nId  = WEBOPT_END;
	IWEB_AddOpt(me->pWeb_fk, awo);
	
	if ((IPeek *)0 != me->pipPostData) {
		
		uf_RecLog(me->m_pParent,"cTaskApp_TaskInteract_fk post  begin");
		// initialize the callback, where I'll be called when the request  completes
		CALLBACK_Init(&me->webcb_fk, cTaskApp_GotResp_fk, (void*)me);
		
		IWEB_GetResponse(me->pWeb_fk,
			(me->pWeb_fk, &me->pWebResp_fk, &me->webcb_fk, me->sUrl_fk, 
			WEBOPT_HANDLERDATA, me,
			WEBOPT_HEADER, "X-Method: POST\r\n  Content-Type: application/x-www-form-urlencoded\r\n", /* for kicks */
			WEBOPT_HEADERHANDLER, NULL,   
			WEBOPT_STATUSHANDLER, cTaskApp_Status_fk,  
			WEBOPT_METHOD, "POST",
			WEBOPT_BODY, me->pipPostData,
			WEBOPT_CONTENTLENGTH, iLen,		
			WEBOPT_END));
		
	}else{
		uf_RecLog(me->m_pParent,"cTaskApp_TaskInteract_fk get  begin");
			CALLBACK_Init(&me->webcb_fk, cTaskApp_GotResp_fk, (void*)me); //应答http返回
			IWEB_GetResponse(me->pWeb_fk,
				(me->pWeb_fk, &me->pWebResp_fk, &me->webcb_fk, me->sUrl_fk, 
				WEBOPT_HANDLERDATA, me,
				WEBOPT_HEADER, "X-Method: GET \r\n",
				WEBOPT_STATUSHANDLER, cTaskApp_Status_fk,  
					WEBOPT_END));
	}

	
//  	CALLBACK_Init(&me->webcb_fk, cTaskApp_GotResp_fk, (void*)me); //应答http返回
// 	IWEB_GetResponse(me->pWeb_fk,
// 		(me->pWeb_fk, &me->pWebResp_fk, &me->webcb_fk, me->sUrl_fk, 
// 		WEBOPT_HANDLERDATA, me,
// 		WEBOPT_HEADER, "X-Method: GET \r\n",
// 		WEBOPT_STATUSHANDLER, cTaskApp_Status_fk,  
// 			WEBOPT_END));
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_TaskInteract_fk  out");

	return;

}



/*********************** 应答http返回 *******************************************************/
void cTaskApp_GotResp_fk(void *p)
{
	WebRespInfo *pInfo;
	char cStatus;
	cTaskApp *me = (cTaskApp *)p;
	newgimis * pMe = me->m_pParent; 
	
	ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_GotResp_fk in");

	//超时清零
	me->cTimeout_fk = 0;


	pInfo = IWEBRESP_GetInfo(me->pWebResp_fk);
	if (pInfo == NULL)
	{
		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"##### ERR:  task cTaskApp_GotResp fang kui in, IWEBRESP_GetInfo return null.##### ");

		//##任务 子任务发送err
		if (1 == pMe->m_Task_fs)   
		{
			pMe->m_Task_fs = 0;		
			me->m_nState = 2;
			if (me->pTaskRecB->bFlow)
			{	
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##### ERR: task1 fang kui IWEBRESP_GetInfo return null.##### ");
				ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
			}
			else
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##### ERR: task2 fang kui IWEBRESP_GetInfo return null.##### ");
				ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, TASK2_EVT_USER_DISPLAY,  0); //显示界面
			}
		}

		return;
	}
	else 
		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_GotResp response fang kui, code:%d,lContentLength=%d,cTaskStatus=%d",pInfo->nCode,pInfo->lContentLength,me->cTaskStatus);
	
// 	if (!WEB_ERROR_SUCCEEDED(pInfo->nCode)) 
// 	{
// 		//##任务 子任务发送err
// 		if (1 == pMe->m_Task_fs)   
// 		{
// 			pMe->m_Task_fs = 0;		
// 			;
// 			if (me->pTaskRecB->bFlow)
// 			{	
// 				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##### ERR: task1 fang kui WEB_ERROR_SUCCEEDED.##### ");
// 				ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
// 			}
// 			else
// 			{
// 				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##### ERR: task2 fang kui WEB_ERROR_SUCCEEDED.##### ");
// 				ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, TASK2_EVT_USER_DISPLAY,  0); //显示界面
// 			}
// 		}
// 
// 		return;
// 	}

	if (!WEB_ERROR_SUCCEEDED(pInfo->nCode)) 
	{
		if (WEB_ERROR_BADRESPONSE != (pInfo->nCode) * (-1))
		{
			//##任务 子任务发送err
			if (1 == pMe->m_Task_fs)   
			{
				pMe->m_Task_fs = 0;		
				me->m_nState = 2;
				if (me->pTaskRecB->bFlow)
				{	
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##### ERR: task1 fang kui WEB_ERROR_SUCCEEDED.##### ");
					ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
				}
				else
				{
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##### ERR: task2 fang kui WEB_ERROR_SUCCEEDED.##### ");
					ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, TASK2_EVT_USER_DISPLAY,  0); //显示界面
				}
			}
			return;
		}
		else
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##### ERR: task fang kui WEB_ERROR_BADRESPONSE.##### ");
			//##任务 子任务发送err
			if (1 == pMe->m_Task_fs)   
			{
				pMe->m_Task_fs = 0;		
				me->m_nState = 6;
				if (me->pTaskRecB->bFlow)
				{	
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR: cTaskApp_GotResp_fk task1 code:1292..display fk web error### ");
					ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
				}
				else
				{
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR: cTaskApp_GotResp_fk task2 code:1292..display fk web error###");
					ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, TASK2_EVT_USER_DISPLAY,  0); //显示界面
				}
			}

			return;
		}
	}

	if(0 != pInfo->pisMessage) 
	{
		if(0 == pInfo->lContentLength) 
		{
			//##任务 子任务发送err
			if (1 == pMe->m_Task_fs)   
			{
				pMe->m_Task_fs = 0;		
				me->m_nState = 2;
				if (me->pTaskRecB->bFlow)
				{	
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##### ERR: task1 fang kui pInfo.##### ");
					ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
				}else{
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##### ERR: task2 fang kui pInfo.##### ");
					ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, TASK2_EVT_USER_DISPLAY,  0); //显示界面
				}
			}


			return; 	      
		}


 		//超时清零
 		me->cTimeout_fk = 0;

		

		me->nBodyHasRead_fk = 0;
		FREEIF(me->pRecData_fk);
		me->nBytes_fk = pInfo->lContentLength;
//		if ( (cStatus == 1) || (cStatus == 4) || (cStatus == 7) )    // 任务派发 ||  任务详情 || 详情反馈
//		{
			me->pSource_fk = pInfo->pisMessage;
			ISOURCE_AddRef(me->pSource_fk);
//		}
		IWEBRESP_Release(me->pWebResp_fk);
		me->pWebResp_fk = 0;
//		if ( (cStatus == 1) || (cStatus == 4) || (cStatus == 7) )    // 任务派发 ||  任务详情 || 详情反馈
//		{
			CALLBACK_Cancel(&me->webcb_fk);
			CALLBACK_Init(&me->webcb_fk, cTaskApp_ReadTaskBody_fk, me); //读取中心端返回内容
			cTaskApp_ReadTaskBody_fk(me);
//		}
	}
	else
	{


		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_GotResp response fang kui, pisMessage=%d",pInfo->pisMessage);
		//##任务 子任务发送err
		if (1 == pMe->m_Task_fs)   
		{
			pMe->m_Task_fs = 0;		
			me->m_nState = 2;

			if (me->pTaskRecB->bFlow)
			{	
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"***task1 fang kui !pInfo->pisMessage..***");
				ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
			}else{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"***task2 fang kui !pInfo->pisMessage..***");
				ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, TASK2_EVT_USER_DISPLAY,  0); //显示界面
			}
		}



	}
	return;
}




// ==========================================================
// 读取中心端返回内容
// ==========================================================
void cTaskApp_ReadTaskBody_fk(cTaskApp *me)
{
	newgimis *pMe;
	int32 nRead;
	char cStatus = 0;
	int nReceiveLen = 0;
	char sBuf[MAX_MESSAGE_LEN];

	pMe = me->m_pParent;


// 	if (me->cTimeout_fk == 1) 
// 	{
// 		ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody_fk response, fang kui Timeout,cTaskStatus=%d",me->cTaskStatus);
// 	
// 		//##任务 子任务发送超时
// 		if (1 == pMe->m_Task_fs)   
// 		{
// 			pMe->m_Task_fs = 0;		
// 			me->m_nState = 3;
// 
// 			if (me->pTaskRecB->bFlow)
// 			{	
// 				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR: cTaskApp_ReadTaskBody_fk  task1 fang kui Timeout #### ");
// 				ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
// 			}
// 			else
// 			{
// 				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR: cTaskApp_ReadTaskBody_fk  task2 fang kui Timeout ##### ");
// 				ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, TASK2_EVT_USER_DISPLAY,  0); //显示界面
// 			}
// 		}
// 
// 		return;
// 	}
// 	else
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody_fk fang kui in,cTaskStatus=%d",me->cTaskStatus);

	nRead = ISOURCE_Read(me->pSource_fk, sBuf, sizeof(sBuf));
	if(ISOURCE_WAIT == nRead) 
	{
//		cTaskApp_SetWebOverTimer(me);
		ISOURCE_Readable(me->pSource_fk, &me->webcb_fk);
		return;
	}
	
	if (nRead > 0)		
	{  // -- ISOURCE_ERROR
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody_fk 接收 sBuf=%s",sBuf);
		
		//////////////////////////////start////////////////////////////////////////////
		//  [3/4/2016 Administrator]
		if (NULL != STRSTR(sBuf, "<head>") )
		{

			//##任务 子任务发送err
			if (1 == pMe->m_Task_fs)   
			{
				pMe->m_Task_fs = 0;		
				me->m_nState = 6;
				
				if (me->pTaskRecB->bFlow)
				{	
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR: cTaskApp_ReadTaskBody_fk task1 receive data error..display fk web error### ");
					ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
				}
				else
				{
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR: cTaskApp_ReadTaskBody_fk task2 receive data error..display fk web error### ");
					ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, TASK2_EVT_USER_DISPLAY,  0); //显示界面
				}
			}
			return;
		}

		//////////////////////////////end////////////////////////////////////////////
		
		if (me->pRecData_fk == NULL) 
		{
			if (me->nBytes_fk > 0)	
				me->pRecData_fk = (char *)MALLOC( (me->nBytes_fk+1)*sizeof(char) );
			else
				me->pRecData_fk = (char *)MALLOC( (nRead+1)*sizeof(char));
			
			MEMCPY(me->pRecData_fk,sBuf,nRead);
			me->nBodyHasRead_fk = nRead; 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody_fk, dataLen=%d, recevie=%d",me->nBytes_fk,nRead);    
		}
		else 
		{
			if ( me->nBytes_fk == -1)
			{		
				me->pRecData_fk = (char *)REALLOC( me->pRecData_fk, (me->nBodyHasRead_fk + nRead+1)*sizeof(char) );
				MEMSET(me->pRecData_fk+me->nBodyHasRead_fk,0,(nRead+1)*sizeof(char));
			}	
			
			MEMCPY(me->pRecData_fk+me->nBodyHasRead_fk,sBuf,nRead);
			me->nBodyHasRead_fk += nRead;	
		}
		
	}  // -- ISOURCE_ERROR  
	
	if (!ISOURCE_Exhausted(nRead))			// 检查数据是否已经全部获取
	{
		ISOURCE_Readable(me->pSource_fk, &me->webcb_fk);
		return;
	}

//	cTaskApp_CloseWeb_fk(me);

	// 判断接收数据是否正确	
 	MEMSET(sBuf,0,sizeof(sBuf));
 	if (me->nBodyHasRead_fk <300)
 		ud_ToAscFmt(me->pRecData_fk,me->nBodyHasRead_fk,sBuf,sizeof(sBuf));
 	else
 		ud_ToAscFmt(me->pRecData_fk,100,sBuf,sizeof(sBuf));
	{
		unsigned char usBuf[4] = {0};
		MEMCPY(usBuf,me->pRecData_fk,4);
		nReceiveLen = ((int)usBuf[0])<<24|((int)usBuf[1])<<16|((int)usBuf[2])<<8|((int)usBuf[3]);
	}
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody_fk data[%d]=%s",nReceiveLen,sBuf);
	if (nReceiveLen != me->nBodyHasRead_fk)
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cTaskApp_ReadTaskBody_fk receive data error,nReceiveLen=%d, nBodyHasRead=%d",nReceiveLen,me->nBodyHasRead);
//		cTaskApp_ShowImage(me,IDI_NFC_05);	// 失败
//		cTaskApp_Alert(pMe);
//		ISHELL_SetTimer(pMe->a.m_pIShell, 3500, (PFNNOTIFY)cTaskApp_WebTimeOutA, me);


		//##任务 子任务发送err
		if (1 == pMe->m_Task_fs)   
		{
			pMe->m_Task_fs = 0;		
			me->m_nState = 2;
	
			if (me->pTaskRecB->bFlow)
			{	
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR: cTaskApp_ReadTaskBody_fk task1 receive data error..display send err### ");
				ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
			}
			else
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR: cTaskApp_ReadTaskBody_fk task2 receive data error..display send err### ");
				ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, TASK2_EVT_USER_DISPLAY,  0); //显示界面
			}
		}


		return;
	}


	cStatus = 7;
	switch (cStatus)
	{
	
	case 7:
		{
			int nResut;
			unsigned char usBuf[4] = {0};
			MEMCPY(usBuf,me->pRecData_fk+4,4);
			nResut = ((int)usBuf[0])<<24|((int)usBuf[1])<<16|((int)usBuf[2])<<8|((int)usBuf[3]);		// 返回结果
			if (nResut == 1)
			{
				//cTaskApp_ShowImage(me,IDI_NFC_04);	// 成功
				uf_RecLog(pMe, "cTaskApp_ReadTaskBody_fk fk_success");

				if (me->FinshorXQ == 1) // 说明要继续获取下一个子任务
				{
					if (me->pTaskRecB->bFlow)
					{
						me->FinshorXQ = 0;
						cTaskApp_TaskInteract_xq(me);			
					}
					else
					{
						//非流程，显示反馈成功，重新刷新界面

						//##任务 按下"完成" 显示发送成功
						pMe->m_bMiss = 1;
						
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### task2: display fk success");	
						uf_AEPrint_Sending(pMe,FALSE);  // 显示发送中
						//1.5秒后显示"发送成功" & 震动
						ISHELL_SetTimer(pMe->a.m_pIShell, 2500, (PFNNOTIFY)Display_Success, pMe);
						me->m_nState = 0;
						pMe->m_Task_fs = 0;
						ISHELL_SetTimer(pMe->a.m_pIShell, 2500+3000, (PFNNOTIFY)uf_Return_Task2_xq, pMe);//停止震动 返回任务详情界面																				//如果没任务了返回更多界面

					}
				}
				else if(me->FinshorXQ == 2) // 说明是完成 则要删除在列表中的节点
				{
					
					int nCount = 1;
					uf_RecLog(pMe, "cTaskApp_ReadTaskBody_fk  LIST_RealDel");
					me->FinshorXQ = 0;

					nCount =  LIST_RealDel(pMe, me->p_head, pMe->m_Cur_TID);  //符串真删除
					//nCount = LIST_iCount(me->p_head);
					//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"LIST_iCount task nCount  = %d",nCount);

					nCount = pMe->m_TTotal =  LIST_Toatal(me->p_head); // 返回当前总的记录数 也就是任务数
					//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"LIST_iCount task nCount  = %d",nCount);

/*					if (0 == nCount)
					{

						uf_RecLog(pMe,"#########　任务做完了");
						pMe->m_bMiss = 1;


						pMe->pTaskApp = NULL;

						//打印 任务已全部完成
						uf_AEPrint_Sending(pMe,FALSE);  // 显示发送中  //显示无任务

						//	uf_RecLog(pMe, " ### 1.5s later display..success");


						ISHELL_SetTimer(pMe->a.m_pIShell, 3500, (PFNNOTIFY)uf_ReturnMenu_gd, pMe);//停止震动 返回任务详情界面	



						//任务都做完了，直接返回上一级菜单（任务崩溃）
//						uf_RecLog(pMe, "####################  testgps_GotoScreen SCREENID_MORE");	
//						pMe->m_WitchPag = 4; // 返回更多的那个界面
//						testgps_GotoScreen ( pMe, SCREENID_MORE, 0 );
//						
//						uf_RecLog(pMe, "####################  return");
//						pMe->m_WitchPag = 4;
//						//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"更多界面 页返回主界面 2:%d",pMe->m_WitchPag);
//				
//						uf_RecLog(pMe, "return");
					  return ;

					}
*/
					uf_RecLog(pMe, "GetTotalPage");
					pMe->m_Page_c = GetTotalPage(pMe->m_TTotal,pMe->m_TaskC);
					pMe->m_Cur_Page = 1;// 当前显示第几页
					uf_RecLog(pMe,"进入 按下下一页按键");
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"按下下一页按键 m_TTotal=%d",pMe->m_TTotal);
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"按下下一页按键 m_Page_c=%d",pMe->m_Page_c);
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"按下下一页按键 m_Cur_Page=%d",pMe->m_Cur_Page);
										
					

					//##任务 按下"完成" 显示发送成功
					pMe->m_bMiss = 1;

					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### task: display send success");	
					uf_AEPrint_Sending(pMe,FALSE);  // 显示发送中
					//1.5秒后显示"发送成功" & 震动
					ISHELL_SetTimer(pMe->a.m_pIShell, 2500, (PFNNOTIFY)Display_Success, pMe);
					me->m_nState = 0;
					pMe->m_Task_fs = 0;
					ISHELL_SetTimer(pMe->a.m_pIShell, 2500+3000, (PFNNOTIFY)uf_ReturnMenu_TaskList, pMe);//停止震动 返回任务详情界面
																										//如果没任务了返回更多界面
					//testgps_GotoScreen( pMe, SCREENID_TASKLIST, 0 );
				}
			}
			else
			{
					//				cTaskApp_ShowImage(me,IDI_NFC_05);	// 失败
					uf_RecLog(pMe, "cTaskApp_ReadTaskBody_fk fk_err...");

					//##任务 子任务发送失败
					if (1 == pMe->m_Task_fs)   
					{
						pMe->m_Task_fs = 0;		
						me->m_nState = 2;

						if (me->pTaskRecB->bFlow)
						{	
							ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR: task1 nResut ### ");
							ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_DISPLAY,  0); //显示界面
						}else{
							ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR: task2 nResut ### ");
							ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, TASK2_EVT_USER_DISPLAY,  0); //显示界面
						}
					}	

					//			cTaskApp_Alert(pMe);
					//			ISHELL_SetTimer(pMe->theApp.m_pIShell, 3500, (PFNNOTIFY)cTaskApp_WebTimeOutA, me);
			}	
//			cTaskApp_Alert(pMe);
//			ISHELL_SetTimer(pMe->theApp.m_pIShell, 3500, (PFNNOTIFY)cTaskApp_WebTimeOutA, me);
		}
		break;
	}

	return;
}

// 显示asc码信息
//Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
AEERect Task_Print_asc( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, const char *szFormat, ... )
{
	AEERect rRect;
	AECHAR  asText[202] = {0};
	char    szBuf[101] = {0};
	int iLen = 0;
	va_list args;

	va_start( args, szFormat );
	(void)VSNPRINTF( szBuf, 255, szFormat, args );
	va_end( args );
	iLen = STRLEN(szBuf);
	
	UTF8TOWSTR( (byte *)szBuf,iLen,asText,sizeof(asText));
	rRect = Task_Print_unicode(pMe,nLine,nCol,fnt,dwFlags,asText);
	return rRect;

}


// 显示unicode信息
AEERect Task_Print_unicode( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *asText)
{
	RGBVAL iColor;
	uint16     cxScreen;         // 实际屏幕大小 (像素)
	uint16     cyScreen;         // 实际屏幕大小 (像素)
	AEERect    rcContent;
	int nMaxLines;
	int nCPixel;		// 每个字符横向占的像素
//	int nLineHight = LINEHEIGHT;
	int nLineHight = pMe->m_SYSFBH;
	
	cxScreen = pMe->DeviceInfo.cxScreen - CONTENT_WITH;
	cyScreen = pMe->DeviceInfo.cyScreen - CONTENT_WITH;

	if (STRCMP(pMe->sMobileType,"72") == 0) 
		nLineHight = pMe->m_SYSFBH+4;
	else if (STRCMP(pMe->sMobileType,"01") == 0 /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/) 
		nLineHight = pMe->m_SYSFBH+2;
	else
		nLineHight = /*pMe->iFontHeight*/pMe->m_SYSFBH;			
	nMaxLines = (cyScreen / nLineHight) - 2;
	if (nMaxLines < 1) 	nMaxLines = 1;

	nCPixel = cxScreen / /*pMe->iFontWidth*/pMe->m_SYSFNH;

	rcContent.x = nCol * nCPixel + 3;
	rcContent.dx = IDISPLAY_MeasureText( pMe->a.m_pIDisplay, AEE_FONT_BOLD, asText );             // cxScreen - nCol * nCPixel;
	rcContent.y = nLine * nLineHight;
	rcContent.dy = nLineHight;
	dwFlags = dwFlags | IDF_TEXT_TRANSPARENT;

//	iColor = IDISPLAY_SetColor(pMe->theApp.m_pIDisplay,CLR_USER_BACKGROUND,CONTENT_COLOR);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, asText, -1,    // -1 = 所有字符串的都表示
		rcContent.x,  rcContent.y, &rcContent, dwFlags);

//	IDISPLAY_SetColor(pMe->theApp.m_pIDisplay,CLR_USER_BACKGROUND,iColor);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	return rcContent;

}


void  UTF8_WCHAR( newgimis *pMe,const char *szFormat,AECHAR *szAechar)
{
	AEERect rRect;
	
	char szBuf[101] = {0};
	int iLen = 0;
	va_list args;
	
	va_start( args, szFormat );
	(void)VSNPRINTF( szBuf, 255, szFormat, args );
	va_end( args );
	iLen = STRLEN(szBuf);
	
	UTF8TOWSTR( (byte *)szBuf,iLen,szAechar,sizeof(szAechar));
}




///////////////////////////////###任务###2015-12-28/////////////////////////////////////////
void Display_Success(newgimis *me)
{
	// 显示发送结果信息
	AECHAR szSuccess[] = {0x53D1,0x9001,0x6210,0x529F,0000};	// 发送成功  \uFEFF\u53D1\u9001\u6210\u529F
	AECHAR szFailure[] = {0x53D1,0x9001,0x5931,0x8D25,0x0000};	// 发送失败
	//	AECHAR szNoFile[] = {0x5DF2,0x65E0,0x89C6,0x9891,0x6587,0x4EF6,0x0000}; // 已无视频文件 \uFEFF\u5DF2\u65E0\u89C6\u9891\u6587\u4EF6
	
				// 显示传视频文件成功 
	
	//uf_RecLog(me, " ### Display_Success");
	
	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szSuccess, -1, 8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);// -1 = 所有字符串的都表示
	
	//	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1, 8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);// -1 = 所有字符串的都表示
	
	IDISPLAY_Update(me->a.m_pIDisplay);
				
	ISHELL_Beep(me->a.m_pIShell, BEEP_MSG, TRUE);
	ISHELL_Beep(me->a.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
	//	ISHELL_SetTimer(me->a.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*) me->m_pParent);
	//	ISHELL_SetTimer(me->a.m_pIShell, 3 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*) me);
	//	ISHELL_SetTimer(me->a.m_pIShell, 55 * 100, (PFNNOTIFY)FleUpd_Afterpost, me->m_pParent); 
	
	return;
}

void Display_false(newgimis *me)  
{
	// 显示发送结果信息

	AECHAR szFailure[] = {0x53D1,0x9001,0x5931,0x8D25,0x0000};	// 发送失败
	//	AECHAR szNoFile[] = {0x5DF2,0x65E0,0x89C6,0x9891,0x6587,0x4EF6,0x0000}; // 已无视频文件 \uFEFF\u5DF2\u65E0\u89C6\u9891\u6587\u4EF6
	
				// 显示传视频文件成功 

	
	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	
	//IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szSuccess, -1, 8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);// -1 = 所有字符串的都表示
	
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1, 8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);// -1 = 所有字符串的都表示
	
	IDISPLAY_Update(me->a.m_pIDisplay);
				
//	ISHELL_Beep(me->a.m_pIShell, BEEP_MSG, TRUE);
//	ISHELL_Beep(me->a.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
	//	ISHELL_SetTimer(me->a.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*) me->m_pParent);
	//	ISHELL_SetTimer(me->a.m_pIShell, 3 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*) me);
	//	ISHELL_SetTimer(me->a.m_pIShell, 55 * 100, (PFNNOTIFY)FleUpd_Afterpost, me->m_pParent); 
	
	return;
}
void Display_NetworkErr(newgimis *me)  
{
	// 显示发送结果信息
	AECHAR strerr[] = {0x5f53,0x524d,0x7f51,0x7edc,0xff1a,0x5f02,0x5e38,0x0000};//  网络异常	
	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, strerr, -1, 8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);// -1 = 所有字符串的都表示	
	IDISPLAY_Update(me->a.m_pIDisplay);
	return;
}

void Display_timeout(newgimis *me)  
{
	AECHAR szFailure[] = {0x53D1,0x9001,0x8D85,0x65F6,0x0000};	// 发送超时
	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1, 8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);// -1 = 所有字符串的都表示
	IDISPLAY_Update(me->a.m_pIDisplay);
	return;
}

void Display_fkweb_error(newgimis *me)  
{
	//\53cd\9988\ff1a\7f51\7edc\5f02\5e38
	AECHAR szFailure[] = {0x53cd,0x9988,0xff1a,0x7f51,0x7edc,0x5f02,0x5e38,0x0000};	// 反馈：网络异常
	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1, 8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);// -1 = 所有字符串的都表示
	IDISPLAY_Update(me->a.m_pIDisplay);
	return;
}


void Display_getTaskErr(newgimis *me)  
{
	AECHAR szFailure[] = {0x83b7,0x53d6,0x5b50,0x4efb,  0x52a1,  0x5931,  0x8d25,  0x0000};	// 获取子任务失败
	//\u83b7\u53d6\u5b50\u4efb   \u52a1\u5931\u8d25
	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1, 8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);// -1 = 所有字符串的都表示
	IDISPLAY_Update(me->a.m_pIDisplay);
	return;
}


void Display_getTaskTimeout(newgimis *me)  
{
	AECHAR szFailure[] = {0x83b7,0x53d6,0x5b50,0x4efb,  0x52a1,  0x8d85,  0x65f6,  0x0000};	// 获取子任务超时
	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1, 8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);// -1 = 所有字符串的都表示
	IDISPLAY_Update(me->a.m_pIDisplay);
	return;
}


// uf_ReturnMenu  返回任务详情界面
void uf_ReturnMenu_xq(newgimis * pMe)
{
	cTaskApp *me = pMe->pTaskApp;
	//pMe->m_bSendPF = FALSE; //用来标记是否正在发送图片的标志位 TRUE -- 表示正在发送状态  FALSE -- 表示空闲
	//ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)uf_ReturnMenu,pMe);
	//testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### uf_ReturnMenu_xq");
	uf_StopBeep(pMe);
//	pMe->m_WitchPag = 17;
	if(me->pTaskRecB->bFlow)
	{
		pMe->m_WitchPag = 17;
		testgps_GotoScreen( pMe, SCREENID_DETAIL1, 0 );
	}
	else
	{
		pMe->m_WitchPag = 18;
		testgps_GotoScreen( pMe, SCREENID_DETAIL2, 0 );
	}
	
	pMe->m_bMiss = 0;
}
// uf_ReturnMenu  返回任务列表
void uf_ReturnMenu_TaskList(newgimis * pMe)
{

	cTaskApp *me = pMe->pTaskApp;
	uf_StopBeep(pMe);

	//if (pMe->m_Page_c >0)
	if (me->p_head)
	{
		pMe->m_Cur_Page = 1;
		pMe->m_WitchPag = 16; 
		testgps_GotoScreen( pMe, SCREENID_TASKLIST, 0 );	
	}
	//else if (0 == pMe->m_Page_c) //没任务了
	else
	{

		uf_RecLog(pMe, "####################  testgps_GotoScreen SCREENID_MORE");	
		pMe->m_WitchPag = 4; // 返回更多的那个界面
		testgps_GotoScreen ( pMe, SCREENID_MORE, 0 );
	}

	//pMe->m_WitchPag = 4;

	pMe->m_bMiss = 0;
}



void uf_Return_Task2_xq(newgimis * pMe)// 反馈成功，返回非流程详情界面
{
	cTaskApp *me = pMe->pTaskApp;
	uf_StopBeep(pMe);
	uf_RecLog(pMe, "uf_Return_Task2_xq: task2 FK success..GotoScreen SCREENID_DETAIL2");
	//pMe->m_WitchPag = 4; // 返回更多的那个界面
	pMe->m_WitchPag = 18;
	testgps_GotoScreen ( pMe, SCREENID_DETAIL2, 0 );	
	//pMe->m_WitchPag = 4;	
	pMe->m_bMiss = 0;
}


// uf_ReturnMenu 显示 任务都完成了， 返回更多界面
void uf_ReturnMenu_gd(newgimis * pMe)
{
	//pMe->m_bSendPF = FALSE; //用来标记是否正在发送图片的标志位 TRUE -- 表示正在发送状态  FALSE -- 表示空闲
	//ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)uf_ReturnMenu,pMe);
	//testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
	//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### uf_ReturnMenu_xq");
	uf_StopBeep(pMe);
	
	
	
	//任务都做完了，直接返回上一级菜单（任务崩溃）
	uf_RecLog(pMe, "####################  testgps_GotoScreen SCREENID_MORE");	
	pMe->m_WitchPag = 4; // 返回更多的那个界面
	testgps_GotoScreen ( pMe, SCREENID_MORE, 0 );
	
	
	pMe->m_WitchPag = 4;
	
	pMe->m_bMiss = 0;	
}

//任务部分，每次重绘屏幕
void Task_RedrawScreen(newgimis *pMe)
{
	// 列表控件标签+列表控件内容 显示
	cTaskApp *me;

//	uf_RecLog(pMe, "Task_RedrawScreen in");
	if(pMe->pTaskApp)
	{
		me = pMe->pTaskApp;
		if (me->pTaskRecB->bFlow)
		{
			{
				AECHAR* psTextBuf = NULL;
				uint32  bufSize = 0;
				uint32  nRtn;
				AEERect rRectList;
				//标签："点击显示内容"
				ISHELL_GetResSize(pMe->a.m_pIShell,NEWGIMIS_RES_FILE, IDS_TASK_CONTENT,RESTYPE_STRING,&bufSize);
				psTextBuf = (AECHAR*)MALLOC((bufSize+1)*sizeof(AECHAR));
				nRtn = ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE,IDS_TASK_CONTENT, psTextBuf,bufSize);
				
				if (nRtn >0 )
					rRectList = Task_Print_unicode( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,psTextBuf);
				FREE(psTextBuf);
			}
			
			//反馈项list
			IMENUCTL_Redraw(me->pMenuList_T);

			{
				//关于任务派发时间
				JulianType dDate;
				AECHAR  asText[50] = {0};
				char    szBuf[25] = {0};
				GETJULIANDATE(pMe->m_Cur_nDateTime, &dDate);
				SPRINTF(szBuf,"time: %04d-%02d-%02d %02d:%02d%",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute);
				STRTOWSTR(szBuf, asText, sizeof(asText));
				Task_Print_unicode(pMe,0,1,AEE_FONT_BOLD,IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL,asText);
			}


			{	//备注文字
				int nRtn;
				int nLine;
				int nExpLen;
				AECHAR szText[128];
				
				AECHAR temp[10] = {0x2e, 0x2e, 0x2e,0x0000};

				
				//nLine = (me->pTaskRecB->bFlow) ? 4 : 5;
				nRtn = ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_TASK_BZ, szText,sizeof(szText));
				if (nRtn > 0)
				{
					Task_Print_unicode( pMe, 4, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,szText);
				}
			}

			return;
		}
	}

	if (!(me->pTaskRecB->bFlow)) //非流程
	{
		AECHAR* psTextBuf = NULL;
		uint32  bufSize = 0;
		uint32  nRtn;
		AEERect rRectList;
		
		//标签："反馈项："
		ISHELL_GetResSize(pMe->a.m_pIShell,NEWGIMIS_RES_FILE, IDS_TASK_02,RESTYPE_STRING,&bufSize);
		psTextBuf = (AECHAR*)MALLOC((bufSize+1)*sizeof(AECHAR));
		nRtn = ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE,IDS_TASK_02, psTextBuf,bufSize);
		
		if (nRtn >0 )
			rRectList = Task_Print_unicode( pMe, 4, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,psTextBuf);
		FREE(psTextBuf);
		
		
		//反馈项list
		IMENUCTL_Redraw(me->pMenuList);	

		{
			AECHAR* psTextBuf = NULL;
			uint32  bufSize = 0;
			uint32  nRtn;
			AEERect rRectList;
			//标签："点击显示内容"
			ISHELL_GetResSize(pMe->a.m_pIShell,NEWGIMIS_RES_FILE, IDS_TASK_CONTENT,RESTYPE_STRING,&bufSize);
			psTextBuf = (AECHAR*)MALLOC((bufSize+1)*sizeof(AECHAR));
			nRtn = ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE,IDS_TASK_CONTENT, psTextBuf,bufSize);
			
			if (nRtn >0 )
				rRectList = Task_Print_unicode( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,psTextBuf);
			FREE(psTextBuf);
		}
		
		//反馈项list
		IMENUCTL_Redraw(me->pMenuList_T);
	}
	
	{
		//关于任务派发时间
		JulianType dDate;
		AECHAR  asText[50] = {0};
		char    szBuf[25] = {0};
		GETJULIANDATE(pMe->m_Cur_nDateTime, &dDate);
		SPRINTF(szBuf,"time: %04d-%02d-%02d %02d:%02d%",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute);
		STRTOWSTR(szBuf, asText, sizeof(asText));
		Task_Print_unicode(pMe,0,1,AEE_FONT_BOLD,IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL,asText);
 	}



	{
		int nRtn;
		int nLine;
		int nExpLen;
		AECHAR szText[128];

		AECHAR temp[10] = {0x2e, 0x2e, 0x2e,0x0000};


		//nLine = (me->pTaskRecB->bFlow) ? 4 : 5;
		nRtn = ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_TASK_BZ, szText,sizeof(szText));
		if (nRtn > 0)
		{
			Task_Print_unicode( pMe, 5, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,szText);
		}
	}
			
}



//#后台模式任务提醒#	后台运行时，有更新任务，弹窗提示是否开始做任务
boolean SamplePosDet_TaskNotify_HandleEvent( newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam )
{
	//AEERect rRect;
	boolean bHandled = FALSE;
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );
	//uf_RecLog(pMe, " ### SamplePosDet_TaskNotify_HandleEvent:  ###");
	
	switch( eCode )
	{
	case EVT_SCREEN:
		if( wParam == SCREEN_PARAM_INIT )
		{
			if( pMenuCtl ) 
			{  /* This is not expected to happen */
				uf_RecLog(pMe, " ### SamplePosDet_TaskNotify_HandleEvent: SCREEN_PARAM_INIT.  This is not expected to happen ###");
				testgps_DrawScreen( pMe, 0 );
				bHandled = TRUE;
			}
			else
			{
				uf_RecLog(pMe,"SamplePosDet_TaskNotify_HandleEvent: SCREEN_PARAM_INIT" );
				if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_MENUCTL, (void **)&pMenuCtl ) == SUCCESS )
				{	//IDS_TASK_BEGIN 开始任务
					//IDS_TASK_DELEY 稍后开始
					//IDS_TASK_NEW	 收到新任务	
					IMENUCTL_SetTitle( pMenuCtl, NEWGIMIS_RES_FILE, IDS_TASK_NEW, NULL );
					IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_TASK_BEGIN, MENU_TASK_BEGIN, NULL, (uint32)pMenuCtl );
						IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_TASK_DELEY, MENU_TASK_DELEY, NULL, (uint32)pMenuCtl );
						
						//SETAEERECT(&rRect, 0, pMe->m_SYSFBH * 5 + 5, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - (pMe->m_SYSFBH * 5) - 5);
						//IMENUCTL_SetRect(pMenuCtl,&rRect);
						testgps_SetScreenData( pMe, (void *)pMenuCtl );
						testgps_DrawScreen( pMe, 0 );
						bHandled = TRUE;
					}
				}
			}
			else if( wParam == SCREEN_PARAM_CLOSE )
			{
				IMENUCTL_Release( pMenuCtl );
				testgps_SetScreenData( pMe, 0 );
				pMe->currentHandler = 0;
				bHandled = TRUE;
			}


			else if( wParam == SCREEN_PARAM_PAINT ) 
			{

				IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);	 
				if( IMENUCTL_IsActive( pMenuCtl ) )
				{
					IMENUCTL_Redraw( pMenuCtl );			
				}
				else
				{
					//IMENUCTL_Redraw( pMenuCtl );
					IMENUCTL_SetActive( pMenuCtl, TRUE );  /* Also causes a menu draw */			 
				}
				bHandled = TRUE;
			}	
			break;

		case EVT_KEY:
			
//			ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)Timer_Close_BackLight,pMe);//#后台模式任务提醒#

			bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );

			// AVK_POWER   AVK_END
			if( wParam == AVK_END  || wParam == AVK_CLR )
			{
				if (pMenuCtl)
				{
					IMENUCTL_Release( pMenuCtl );
					pMenuCtl = NULL;
				}

				pMe->currentHandler = 0;
				testgps_SetScreenData( pMe, 0 );

				pMe->m_RelyExit = FALSE;//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
				ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //转后台
				pMe->m_FOBFlag = TRUE; //在后台运行
				pMe->m_WitchPag = 0;
				bHandled = TRUE;
			}
 
			break;

		case EVT_COMMAND:
			if( dwParam == (uint32)pMenuCtl ) 
			{
				switch( wParam )
				{
					case MENU_TASK_BEGIN:  // 开始任务
					{
										
						cTaskApp *me = pMe->pTaskApp;

						if (pMe->pTaskApp)
						{
							if(me->p_head) 
							{		
								ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_TaskNotify_HandleEvent: press button, do task right now. ");					
								pMe->m_WitchPag = 16; 
								pMe->m_Cur_Page = 1 ;// 表示每次进去都是第一页
								testgps_GotoScreen( pMe, SCREENID_TASKLIST, wParam );
							}
							else
							{
								ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR:1 no task. This is not expected to happen ## ");
								uf_AEPrint_RWLB_Back_Two(pMe,FALSE);	
							}
						}
						else
						{
							pMe->m_bMiss = 1;
							ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"### ERR:2 no task. This is not expected to happen ## ");
							uf_AEPrint_RWLB_Back_Two(pMe,FALSE);	
						}
						
					}		
					return TRUE;

					case MENU_TASK_DELEY:  //稍后开始任务
					{
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"SamplePosDet_TaskNotify_HandleEvent: press button, do task later. ");
						//切回后台
						pMe->currentHandler = 0;
						testgps_SetScreenData( pMe, 0 );
						
						pMe->m_RelyExit = FALSE;						//标记是否真正退出  TRUE -真正退出 FALSE -- 转入后台
						ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE);		//转后台
						pMe->m_FOBFlag = TRUE;							//在后台运行
						pMe->m_WitchPag = 0;
						bHandled = TRUE;
					}
					return TRUE;

					default:
						break;
				}
			}
		break;
		
	}
	return bHandled;
}

//定时器，有任务弹窗提醒
void Timer_Task_Notify(newgimis *pMe)
{
	cTaskApp *me = pMe->pTaskApp;
	//#后台模式任务提醒#	后台运行时，有更新任务，弹窗提示是否开始做任务 2016-1-21
	if (pMe->pTaskApp)
	{
		
		cTaskApp *me = pMe->pTaskApp;
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"new task ####ISHELL_StartApplet  m_FOBFlag = %d", pMe->m_FOBFlag);
		
		if(me->p_head &&  1 == pMe->m_FOBFlag  && (0 == me->cTimeout) ) //有未完成任务 并且在后台
		{					 
			//后台切到前台
			
			pMe->m_bDoTask = 1;										//标志位
			ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
						
		}
		else
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ERR: Timer_Task_Notify 1	m_FOBFlag = %d  me->cTimeout = %d", pMe->m_FOBFlag, me->cTimeout);
			//uf_RecLog(pMe,"ERR: Timer_Task_Notify 1	m_FOBFlag = %d  me->cTimeout = %d", pMe->m_FOBFlag, me->cTimeout);
		}
	}
	else
	{
		uf_RecLog(pMe,"ERR: Timer_Task_Notify 2 ");	
	}
}

//定时器，5S关闭屏幕背光
void Timer_Close_BackLight(newgimis *pMe)
{
	uf_RecLog(pMe,"Timer_Close_BackLight");
	IDisplay_Backlight(pMe->a.m_pIDisplay, FALSE);
}