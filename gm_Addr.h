/*======================================================
FILE:  gm_Addr.h
    处理有关地址簿的问题
   

 修改日志：
 2012-11-08  zjie  


=====================================================*/
#ifndef __GM_ADDR_H__
#define __GM_ADDR_H__

#include "AEEAddrBook.h" 

typedef struct _newgimis newgimis;

#define EVT_ADDR_ENUMCATEGORY		EVT_USER+2100				// 枚举地址记录类别, 测试用,发布时要取消
#define EVT_ADDR_CREATE				EVT_USER+2101				// 更新通信录
#define EVT_ADDR_DELETE				EVT_USER+2102				// 删除通信录中记录
#define MAX_MOBILENO_SIZE			30

// 保存用户电话号码簿的结构
typedef struct
{
	AECHAR			sName[30+1];							// 姓名
	AECHAR          sJobtitle[50+1];						// 职务名称
	AECHAR          sMobileNo[MAX_MOBILENO_SIZE+1];			// 手机号码
	AECHAR          sPttNo[MAX_MOBILENO_SIZE+1];			// ptt号码
	int				wID;									// 记录在地址簿中的ID,	-1 表示没有确定位置
} IUsrAddrBook;

// 用户电话号码簿列表信息，从中心端下载得到
typedef struct
{
	uint16			count;					// 号码簿总数
	IUsrAddrBook	*AddrBookInfo;			// 号码簿的结构
} IAddrBookList;


typedef struct					
{
	void			*m_pParent;
	IAddrBook 		*m_pIAddrBook;

	char			*pstrRecU;				 // 下载的通信记录字符串(添加用)
	char			*pstrRecD;				 // 下载的通信记录字符串(删除用)
	AECHAR			*asKeyName;				 // 通信录唯一键的字段名
//	uint8			nRecLen;				 //  
	IAddrBookList	*pAddrBookList;			 // 自定义的预定义组列表


	
	
} crichAddr;


/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/

// 将下载的通信录信息传入通讯录模块中处理
void Addr_GetAddrInfo(newgimis *pMe,char *sCmdInfo);

// 处理消息
boolean Addr_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

#endif /* !__GM_ADDR_H__ */
