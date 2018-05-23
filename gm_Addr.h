/*======================================================
FILE:  gm_Addr.h
    �����йص�ַ��������
   

 �޸���־��
 2012-11-08  zjie  


=====================================================*/
#ifndef __GM_ADDR_H__
#define __GM_ADDR_H__

#include "AEEAddrBook.h" 

typedef struct _newgimis newgimis;

#define EVT_ADDR_ENUMCATEGORY		EVT_USER+2100				// ö�ٵ�ַ��¼���, ������,����ʱҪȡ��
#define EVT_ADDR_CREATE				EVT_USER+2101				// ����ͨ��¼
#define EVT_ADDR_DELETE				EVT_USER+2102				// ɾ��ͨ��¼�м�¼
#define MAX_MOBILENO_SIZE			30

// �����û��绰���벾�Ľṹ
typedef struct
{
	AECHAR			sName[30+1];							// ����
	AECHAR          sJobtitle[50+1];						// ְ������
	AECHAR          sMobileNo[MAX_MOBILENO_SIZE+1];			// �ֻ�����
	AECHAR          sPttNo[MAX_MOBILENO_SIZE+1];			// ptt����
	int				wID;									// ��¼�ڵ�ַ���е�ID,	-1 ��ʾû��ȷ��λ��
} IUsrAddrBook;

// �û��绰���벾�б���Ϣ�������Ķ����صõ�
typedef struct
{
	uint16			count;					// ���벾����
	IUsrAddrBook	*AddrBookInfo;			// ���벾�Ľṹ
} IAddrBookList;


typedef struct					
{
	void			*m_pParent;
	IAddrBook 		*m_pIAddrBook;

	char			*pstrRecU;				 // ���ص�ͨ�ż�¼�ַ���(�����)
	char			*pstrRecD;				 // ���ص�ͨ�ż�¼�ַ���(ɾ����)
	AECHAR			*asKeyName;				 // ͨ��¼Ψһ�����ֶ���
//	uint8			nRecLen;				 //  
	IAddrBookList	*pAddrBookList;			 // �Զ����Ԥ�������б�


	
	
} crichAddr;


/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/

// �����ص�ͨ��¼��Ϣ����ͨѶ¼ģ���д���
void Addr_GetAddrInfo(newgimis *pMe,char *sCmdInfo);

// ������Ϣ
boolean Addr_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam);

#endif /* !__GM_ADDR_H__ */
