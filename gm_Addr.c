/*===============================================================================
 richAddr.c
    �����йص�ַ��
=============================================================================== */
#include "AEEStdLib.h"

#include "newgimis.h"
#include "gm_Addr.h"
#include "gm_Qchat.h"
#include "gmTools.h"

#define  PHONE_LOC_01	1		// �����ֻ�������ͨ��¼��λ��
#define  ID_LOC_01	9			// �����ֻ���¼ID��ͨ��¼��λ��


// ͨ��¼����
#define richptt_SetAddrCat(pMe,nAddrCat)  {\
	if ( (STRCMP(pMe->sMobileType,"55") == 0) || (STRCMP(pMe->sMobileType,"58") == 0) || (STRCMP(pMe->sMobileType,"63") == 0)  || (STRCMP(pMe->sMobileType,"03") == 0) || STRCMP(pMe->sMobileType,"57")==0 )\
		nAddrCat = AEE_ADDR_CAT_PERSONAL;\
    else if (STRCMP(pMe->sMobileType,"33") == 0)\
        nAddrCat = AEE_ADDR_CAT_NONE;\
	else if (STRCMP(pMe->sMobileType,"15") == 0)\
        nAddrCat = AEE_ADDR_CAT_NONE;\
	else\
		nAddrCat = AEE_ADDR_CAT_BUSINESS;\
}

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
// ��ʼ��
void Addr_InitAppData(newgimis *pMe);
// �˳�֮ǰ������
static void Addr_FreeAppData(crichAddr* me);
// ֹͣͨ��¼��Ӧ��
static void Addr_StopApp(newgimis *pMe);
//����ע��İ���
static boolean Addr_NotifyShellKey(crichAddr *me, AEEEvent eCode, uint16 wParam, uint32 dwParam);

// ���������ص�ͨ��¼��Ϣ���뵽�û��绰���벾(�ṹ)��
static void Addr_SetUsrAddr(crichAddr* me);
// 						//�Ժ����ֻ���ͨѶ¼��¼�ŵ����ã��ڸ��µ�����£�
// 						static void Addr_SetAddrIdx_Upd(crichAddr* me,AECHAR *asPnoneNo,word nRecId);
// 						//�Ժ����ֻ���ͨѶ¼��¼�ŵ����ã�������������£�
// 						static void Addr_SetAddrIdx_Add(crichAddr* me,word nBeginId);

// ö�ٵ�ַ��¼���
static int Addr_EnumCategory(crichAddr* me);
// ö�ټ�¼���ֶ���Ϣ
static int Addr_EnumFieldsInfo(crichAddr* me,AEEAddrCat oBusiness,boolean bEnumAll);
// ö�ټ�¼����Ϣ
static int Addr_EnumRecInfo(crichAddr* me);
// ��Ӽ�¼ 
static int Addr_CreateRec_01(crichAddr* me);	// �йغ��ŵ�
static int Addr_CreateRec_58(crichAddr* me);	// �й����Ƶ�
static int Addr_CreateRec_33(crichAddr* me);	// �й�������Ԫ

// ɾ��ͨ��¼����Ϣ
static void Addr_DelAddress(crichAddr *me,char *sAddrInfo);
// ������¼֮ǰɾ��ͨ��¼����Ϣ(�йغ���)
static void Addr_DelAddressForAdd_01(crichAddr *me,char *sAddrInfo);

void Addr_DelAddressForAdd(crichAddr* me);

// �ͷ��û��绰���벾
static void Addr_FreeUsrAddrBook(crichAddr *me);


////////  add  2015-10-29  start ////////////////
// ����ָ���绰�����Ƿ����
// ����ֵ  = 1 ˵�����ҵ��绰����  = 0 ˵��û���ҵ��绰����
static int Addr_Find_Number(crichAddr *me,char *sAddrInfo,newgimis *pwa);
static int Addr_Find_Number_01(crichAddr *me,char *sAddrInfo,newgimis *pwa);
// ����˵�� ���һ������ pAddrbook ������Ż�ȡ���ĵ绰����
static int Addr_Find_Number_haixin(crichAddr *me,newgimis *pwa,char *pAddrbook);

// ����˵�� ���һ������ pAddrbook ������Ż�ȡ���ĵ绰����
static int Addr_Find_Number_xinyun(crichAddr *me,newgimis *pwa,char *pAddrbook);


////////  add  2015-11-14  start ////////////////
static int Addr_Find_Number_03(crichAddr *me,char *sAddrInfo,newgimis *pwa);
// ��ȡû����ӵ�ͨѶ¼�ĺ���
////////  add  2015-11-14  start ////////////////
static void do_PhoneNumber_03(newgimis *pMe);


// ��ȡû����ӵ�ͨѶ¼�ĺ���
static void do_PhoneNumber(newgimis *pMe);

static void do_PhoneNumber_01(newgimis *pMe);
////////       end       ////////////////////////

//========================
// (����ͨ��¼) ��word�������ֵתΪ�ַ�����ֵ
static char* hesense_wordtostr(word *wInt)
{
	char *sInt = NULL;
	int nLen = WSTRLEN(wInt);
	
	sInt = (char *)MALLOC( (nLen+1)*sizeof(char) );
	MEMSET(sInt,0,(nLen+1));
	
	WSTRTOUTF8(wInt,nLen,(byte *)sInt, nLen);
	
	return sInt;
}

// (����ͨ��¼) ���ַ�����ֵתΪword�������ֵ
static word* hesense_strtoword(char *sInt)
{
	word *wInt = NULL;
	int nLen = STRLEN(sInt);
	
	wInt = (word *)MALLOC( (nLen+1)*sizeof(word) ); 
	MEMSET(wInt,0,(nLen+1)*sizeof(word));
	
	UTF8TOWSTR((byte *)sInt,nLen,wInt,(nLen+1)*sizeof(word));

	return wInt;
}


//==============================================================================
// ��ʼ��
//==============================================================================
void Addr_InitAppData(newgimis *pMe)
{
	crichAddr* me;
	int nErr;

	if (!pMe) return;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_InitAppData in");

	if (pMe->pRichAddr != NULL)
		return;

	me = (crichAddr *)MALLOC(sizeof(crichAddr));
	MEMSET(me,0,sizeof(crichAddr));
	me->m_pParent = (void *)pMe;
	pMe->pRichAddr = (void *)me;

	// ע��	RUIM��  AEECLSID_ADDRBOOK_RUIM
	nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_ADDRBOOK , (void**)(&(me->m_pIAddrBook)));
	if ( nErr != SUCCESS) {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ISHELL_CreateInstance for AEECLSID_ADDRBOOK error=%d",nErr);
		goto ExitHere;
	}

		
ExitHere:
	return;
}
// �˳�֮ǰ������
static void Addr_FreeAppData(crichAddr* me)
{

	IADDRBOOK_Release(me->m_pIAddrBook);
	FREEIF(me->pAddrBookList);
	return;

}

// ֹͣͨ��¼��Ӧ��
static void Addr_StopApp(newgimis *pMe)
{
	crichAddr* me = pMe->pRichAddr;
	
	Addr_FreeAppData(me);
	me->m_pParent = 0;
	FREEIF(pMe->pRichAddr);
	
}

// =======================================================================================
boolean Addr_HandleEvent(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	crichAddr* me;

	if (!pMe) return FALSE;
 	if (pMe->pRichAddr != NULL)
 		me = (crichAddr *)pMe->pRichAddr;
 	else if (eCode != EVT_APP_START)
 		return FALSE;


//	ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_HandleEvent in");
	
	switch (eCode)
	{
    case EVT_APP_START:
		if (pMe->pRichAddr == NULL) {
			// ����ͨ��¼
			Addr_InitAppData(pMe);
		}

		break;
    case EVT_APP_STOP:							
		DBGPRINTF("zjie-Addr_EVT_APP_STOP");
		Addr_StopApp(pMe);
	
        break;

	case EVT_KEY:

// 		if (wParam == AVK_2) {
// 			richppt_printf( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_RECT_FILL, "key AVK_2");
// 			Addr_EnumCategory(me);
// 			return TRUE;
// 		}
// 		
// 		if (wParam == AVK_3) {
// 			richppt_printf( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_RECT_FILL, "key AVK_3");
// //			Addr_EnumFieldsInfo(me,AEE_ADDR_CAT_BUSINESS,TRUE);
// 			Addr_EnumRecInfo(me);
// 			return TRUE;
// 		}

		break;
	case EVT_NOTIFY:
		{	
			AEENotify* pNotify = (AEENotify*) dwParam;
			
			if (pNotify && (pNotify->cls == AEECLSID_SHELL)) // event sender
			{	// -- shell
				// ����������
				if ((pNotify->dwMask & NMASK_SHELL_KEY ) == NMASK_SHELL_KEY )  
				{	// if -- KEY
					if ( Addr_NotifyShellKey( me, eCode, wParam, dwParam ) )
						return TRUE;
				}	// if -- KEY
			}   // -- shell
			
		}
		break;

	case EVT_ADDR_DELETE:
		Addr_DelAddress(me,me->pstrRecD);
		FREEIF(me->pstrRecD);

		if (me->pstrRecU != NULL) {
			ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID, EVT_ADDR_CREATE,0,0);
		}

		break;
	case EVT_ADDR_CREATE:
		Addr_SetUsrAddr(me);		// ����ɾ��ǰ�棬Ϊ�˵õ�����ֵ
		if ( (STRCMP(pMe->sMobileType,"01") == 0) /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/) 
		{
			Addr_DelAddressForAdd_01(me,me->pstrRecU);
			Addr_CreateRec_01(me);
		}
		else {
			Addr_DelAddress(me,me->pstrRecU);
			if ( (STRCMP(pMe->sMobileType,"33") == 0) )
				Addr_CreateRec_33(me);
			else
				Addr_CreateRec_58(me);
		}

		FREEIF(me->pstrRecU);
		break;
	case EVT_ADDR_ENUMCATEGORY:			// �����ã�����ʱҪȡ��
// 		Addr_EnumCategory(me);
// 		Addr_EnumFieldsInfo(me,AEE_ADDR_CAT_BUSINESS,TRUE);
// 		Addr_EnumFieldsInfo(me,AEE_ADDR_CAT_PERSONAL,TRUE);
// 		Addr_EnumRecInfo(me);
// ɾ��
//		Addr_DelAddress(me,"A");
//		Addr_GetAddrInfo(pMe,"QCD18960882574,18960882566;");
// ����
/*
  		me->pstrRecU = (char *)MALLOC(500);		// 60
		STRCPY(me->pstrRecU,"15359144504:15359144775:Abcd:Bcde,15359144508:15359144779:Xbcd:Ycde,18965901037:18965901038:Cdef:Defg");
//  	//	STRCPY(me->pstrRecU,"15359144504:15359144775:Abcd:Bcde");
		Addr_SetUsrAddr(me);	
// 			Addr_DelAddressForAdd_01(me,me->pstrRecU);
		Addr_CreateRec_33(me);
		Addr_EnumRecInfo(me);
		FREEIF(me->pstrRecU);
*/
		break;
    default:
		break;
    }
	
    return FALSE;
}

/* ===========================================================================================
˵��			
   ����ע��İ���
�������
   ͬ richptt_HandleEvent

��������
   TRUE ������ؼ��Ѿ���������Ϣ�� FALSE ����ؼ�û�д�����Ϣ
============================================================================================== */
static boolean Addr_NotifyShellKey(crichAddr *me, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	newgimis *pMe;
	AEENotify* pNotify = (AEENotify*) dwParam;
	NotifyKeyEvent *pNKeyEvn = (NotifyKeyEvent*) pNotify->pData;

	if (!me) return FALSE;
	pMe = me->m_pParent;

//	ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richptt_NotifyShellKey in, wParam=%d",pNKeyEvn->wParam);

	if ( (pNKeyEvn->wParam == AVK_3) && (pNKeyEvn->eCode == EVT_KEY_RELEASE) )
	{
		ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_ADDR_ENUMCATEGORY,0, 0);
	}

	return FALSE;

}

// ====================================================================
// �����ص�ͨ��¼��Ϣ����ͨѶ¼ģ���д���
void Addr_GetAddrInfo(newgimis *pMe,char *sCmdInfo)
{
	char *pszTok = NULL;
	int nResult = 0;
	crichAddr* me;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_GetAddrInfo in");

	if (pMe->pRichAddr == NULL) return;
	me = pMe->pRichAddr;

	FREEIF(me->pstrRecD);
	FREEIF(me->pstrRecU);

	// ɾ��ͨ��¼�м�¼, *******ע�⣺QCDһ��Ҫ����QCAǰ�棬�Է���ɾ��ӵ����
	pszTok = uf_STRSTR(sCmdInfo,"QCD",DEFAULT_FIELD_SPLIT);		
	if (pszTok) {
		nResult = ud_DistToSplit( pszTok, DEFAULT_FIELD_SPLIT );
		if (nResult > 0)	{
			me->pstrRecD = (char *)MALLOC(nResult+1);		
			MEMCPY(me->pstrRecD,pszTok, nResult);
		}
	}
	// �������޸�ͨ��¼�м�¼
	pszTok = uf_STRSTR(sCmdInfo,"QCA",DEFAULT_FIELD_SPLIT);
	if (pszTok) {
		nResult = ud_DistToSplit( pszTok, DEFAULT_FIELD_SPLIT );
		if (nResult > 0)	{
			me->pstrRecU = (char *)MALLOC(nResult+1);		
			MEMCPY(me->pstrRecU,pszTok,nResult);

			if (me->pstrRecD != NULL) {
				int iLen = nResult + STRLEN(me->pstrRecD) + 1 ;
				me->pstrRecD = (char *)REALLOC(me->pstrRecD,(iLen+1));
				STRCAT(me->pstrRecD,DEFAULT_SPLIT);
				STRCAT(me->pstrRecD,me->pstrRecU);
			}
			

		}
	}
	
	// ע����ɾ���������
	if (me->pstrRecD != NULL) {  
		ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID, EVT_ADDR_DELETE,0,0);
	}
	else if (me->pstrRecU != NULL) {
		ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID, EVT_ADDR_CREATE,0,0);
	}

	return;
}


// ɾ��ȫ����¼
static void Addr_RemoveAll(crichAddr* me)
{
	int nErr = AEE_SUCCESS;
	newgimis *pMe;
	AEEAddrCat nAddrCat;

	IAddrRec *pAddRec = NULL;
	int nRecId = 0;
	
	if (!me) return;
	pMe = (newgimis *)me->m_pParent;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_RemoveAll in");

// 	if ( (STRCMP(pMe->sMobileType,"55") == 0) || (STRCMP(pMe->sMobileType,"58") == 0) )
// 		nAddrCat = AEE_ADDR_CAT_PERSONAL;
// 	else
// 		nAddrCat = AEE_ADDR_CAT_BUSINESS;
	richptt_SetAddrCat(pMe,nAddrCat);

	// ö��ȫ�� 
    nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, nAddrCat , AEE_ADDRFIELD_NONE, NULL,0 ); 
	if ( nErr != AEE_SUCCESS ) 	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_RemoveAll, IADDRBOOK_EnumRecInit error=%d",nErr);
		goto ExitHere;
	}

	nRecId = 0;
	while( TRUE )   {		// while -- 1
//		int idx = 0;
//		int ndx = 0;
		
		pAddRec = IADDRBOOK_EnumNextRec(me->m_pIAddrBook);
		if (pAddRec == NULL)
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"nRecId=%d,pAddRec == NULL",nRecId);
			break;
		}

		nErr = IADDRREC_RemoveRec(pAddRec);
		if ( AEE_SUCCESS != nErr )
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRREC_RemoveRec return error");
			break;
		}

		nRecId++;

	}	// while -- 1
ExitHere:
	return;
}

// ======================================================================
// ɾ��ͨ��¼����Ϣ
// char *sAddrInfo �� Ҫɾ����ͨ��¼��Ϣ���ֻ�����,
//					  �������"A"���ʾȫ��ɾ��
static void Addr_DelAddress(crichAddr *me, char *sAddrInfo)
{
	int nErr = AEE_SUCCESS;
	newgimis *pMe;
	
	AEEAddrCat nAddrCat;
	IAddrRec *pAddRec = NULL;
	int nRecId = 0;
	AEEAddrField *pAddrField = NULL;

	if (!me) return;
	pMe = (newgimis *)me->m_pParent;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddress in");

	if (STRCMP(sAddrInfo,"A") == 0) {		// ɾ��ȫ����Ϣ
		Addr_RemoveAll(me);
		goto ExitHere;
	}	// ɾ��ȫ����Ϣ

// 	if ( (STRCMP(pMe->sMobileType,"55") == 0) || (STRCMP(pMe->sMobileType,"58") == 0) )
// 		nAddrCat = AEE_ADDR_CAT_PERSONAL;
// 	else
// 		nAddrCat = AEE_ADDR_CAT_BUSINESS;
	richptt_SetAddrCat(pMe,nAddrCat);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumRecInit nAddrCat=%d",nAddrCat);

	// �����ֻ�����ɾ����¼								AEE_ADDR_CAT_NONE,AEE_ADDR_CAT_PERSONAL
    nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, nAddrCat , AEE_ADDRFIELD_NONE, NULL,0 );	// AEE_ADDR_CAT_BUSINESS
	if ( nErr != AEE_SUCCESS ) 	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddr,IADDRBOOK_EnumRecInit error=%d",nErr);
		goto ExitHere;
	}
	
	nRecId = -1;
	while( TRUE )   {		// while -- 1
		int ndx = 0;
//		int nResult = 0;
		char sPhoneNo[QDK_CALL_NAME_LEN+1];
//		int nSize = sizeof(sPhoneNo);  
		nRecId++;
		
		pAddRec = IADDRBOOK_EnumNextRec(me->m_pIAddrBook);
		if (pAddRec == NULL)
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddr,nRecId=%d,pAddRec == NULL",nRecId);
			break;
		}
		// �ֻ�������ͨ��¼��λ��
		if (STRCMP(pMe->sMobileType,"01") == 0 /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/)
			ndx = PHONE_LOC_01;
		else if ( (STRCMP(pMe->sMobileType,"55") == 0) || (STRCMP(pMe->sMobileType,"58") == 0) || (STRCMP(pMe->sMobileType,"03") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0) || STRCMP(pMe->sMobileType,"57")==0 )
			ndx = 2;
		else
			ndx = 1;
		
		pAddrField = IADDRREC_GetField(pAddRec,ndx);
		if (pAddrField == NULL) {
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRREC_GetField,nRecId=%d,ndx=%d,pAddrField == NULL",nRecId,ndx);
			continue;
		}

		MEMSET(sPhoneNo,0,sizeof(sPhoneNo));
		if (pAddrField->wDataLen > 0) {	// if -- 12
			WSTRTOUTF8(pAddrField->pBuffer,WSTRLEN(pAddrField->pBuffer),(byte *)sPhoneNo,pAddrField->wDataLen );
//			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddr,Phone=%s,sAddrInfo=%s",sPhoneNo,sAddrInfo);
			if ( uf_STRSTR(sAddrInfo, sPhoneNo,DEFAULT_SPLIT) != NULL) {	
				nErr = IADDRREC_RemoveRec(pAddRec);
				if ( AEE_SUCCESS != nErr )
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddr,IADDRREC_RemoveRec error");
				else
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddr,IADDRREC_RemoveRec=%s",sPhoneNo);

			}
		}	// if -- 12

	}	// while -- 1


ExitHere:
	return;
}

// ======================================================================
// ������¼֮ǰɾ��ͨ��¼����Ϣ(�йغ���)
// char *sAddrInfo �� Ҫɾ����ͨ��¼��Ϣ���ֻ�����,
//					  �������"A"���ʾȫ��ɾ��
void Addr_DelAddressForAdd_01(crichAddr *me, char *sAddrInfo)
{
	int nErr = AEE_SUCCESS;
	newgimis *pMe;
	AEEAddrCat nAddrCat;	
	IAddrRec *pAddRec = NULL;
//	int nRecId = 0;
	AEEAddrField *pAddrField = NULL;				// �ֻ�����
	AEEAddrField *pAddrFieldId = NULL;				// ��¼id
	int nUsedCnt;					// ����(�����Ѿ�ʹ�õ�id)����
	int16 *naUsedId = NULL;		// ���飬�����Ѿ�ʹ�õ�id
	int16 nMaxId;					// ��ǰ��¼�����ļ�¼��
	int16 *naIdArray = NULL;			// ���飬������ʹ�ú�δʹ�õ�id��������������Ǽ�¼id��-1 �����Ѿ�ʹ��
	word nLoc;						// ��¼id�����е�һ����������Ӽ�¼��id��λ��
	
	if (!me) return;
	pMe = (newgimis *)me->m_pParent;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01 in");
	richptt_SetAddrCat(pMe,nAddrCat);

	nUsedCnt = IADDRBOOK_GetNumRecs(me->m_pIAddrBook);		// ��¼��
	naUsedId = (int16 *)MALLOC(nUsedCnt * sizeof(int16));

	// �����ֻ�����ɾ����¼
    nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_NONE , AEE_ADDRFIELD_NONE, NULL,0 );	// AEE_ADDR_CAT_BUSINESS
	if ( nErr != AEE_SUCCESS ) 	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01,IADDRBOOK_EnumRecInit error=%d",nErr);
		goto ExitHere;
	}

	nLoc = -1;
	nMaxId = -1;
	while( TRUE )   {		// while -- 1
		int i = 0;	
		int nResult = 0;
		char *sInt = NULL;
		nLoc++;

//		ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01 in,IADDRBOOK_EnumNextRec,nLoc=%d",nLoc);
		pAddRec = IADDRBOOK_EnumNextRec(me->m_pIAddrBook);
		if (pAddRec == NULL)
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01,IADDRBOOK_EnumNextRec return NULL");
			// һ����ûö�ٵ����ƺ� IADDRBOOK_GetNumRecs ��ʱ�����
			if (nLoc == 0) 	nUsedCnt = 0;
			FREEIF(sInt);
			break;
		}
		
		pAddrFieldId = IADDRREC_GetField(pAddRec,ID_LOC_01);
		if (pAddrFieldId == NULL) {
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01,IADDRREC_GetField,pAddrFieldId == NULL");
			continue;
		}
//		ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01 in,IADDRREC_GetField,ID_LOC_01");

		pAddrField = IADDRREC_GetField(pAddRec,PHONE_LOC_01);
		if (pAddrField == NULL) {
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01,IADDRREC_GetField,pAddrField == NULL");
			break;
		}
		
		FREEIF(sInt);
		sInt = hesense_wordtostr( (word *)pAddrFieldId->pBuffer );
		nResult = ATOI(sInt);
		nMaxId = (nResult > nMaxId) ? nResult : nMaxId;
		naUsedId[nLoc] = nResult;
//		ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01 in,IADDRREC_GetField,PHONE_LOC_01,wDataLen=%d,recid=%d",pAddrField->wDataLen,nResult);

		if (pAddrField->wDataLen > 0) {	// if -- 12
//			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01 in,IADDRREC_GetField,PHONE_LOC_01, bookInfo count=%d",me->pAddrBookList->count);
			nResult = 0;
			for(i = 0; i < me->pAddrBookList->count; i++ ) {	// for-1
				if (WSTRCMP(pAddrField->pBuffer,me->pAddrBookList->AddrBookInfo[i].sMobileNo) == 0)	{ 
					me->pAddrBookList->AddrBookInfo[i].wID = ATOI(sInt);
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01,find rec wid=%s",sInt);
					nResult = 1;
					break;
				}
			}	// for-1

			if (nResult == 1) {	 
				nErr = IADDRREC_RemoveRec(pAddRec);
				if ( AEE_SUCCESS != nErr )
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01,IADDRREC_RemoveRec error");
 				else
 					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01,IADDRREC_RemoveRec ok");
			}
		}	// if -- 12

	}	// while -- 1
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01 in,IADDRBOOK_EnumNextRec,nMaxId=%d,nUsedCnt=%d,nLoc=%d",nMaxId,nUsedCnt,nLoc);

	// Ѱ�ҿ��Եļ�¼��
	{	// if -- 2
		int i;
		naIdArray = (int16 *)MALLOC( (nMaxId+1)*sizeof(int16));
		for (i=0;i<=nMaxId;i++) 
			naIdArray[i] = i;
		for (i=0;i<nUsedCnt;i++) 
			if (naUsedId[i] <= nMaxId)
				naIdArray[naUsedId[i]] = -1;

		nLoc = 0;	// �����ļ�¼�л�δ���¼�ŵļ�¼����
		for (i=0;i<=nMaxId;i++) 
		{
		   int j=nLoc;
		   if (naIdArray[i] != -1)	// �ҵ����õļ�¼��
			   for (j=nLoc;j<me->pAddrBookList->count;j++) {	// for-2
					if (me->pAddrBookList->AddrBookInfo[j].wID == -1) {
						me->pAddrBookList->AddrBookInfo[j].wID = naIdArray[i];
						nLoc = j+1;
						break;
					}
			   }	// for-2
			   if (j == (me->pAddrBookList->count-1)) {
				   if ( (nLoc == 0) || (nLoc == me->pAddrBookList->count) )
					   goto ExitHere;
			   }
		}
		 
	}	// if -- 2

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01 in, last");
	
	{	// nMaxId֮ǰû��δ�õļ�¼��
		int i;
		nMaxId = ( nMaxId<nUsedCnt ) ? nUsedCnt : (nMaxId+1);
		for(i=0;i<me->pAddrBookList->count;i++) {
			if (me->pAddrBookList->AddrBookInfo[i].wID == -1) {
				me->pAddrBookList->AddrBookInfo[i].wID = nMaxId;
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd_01 in,i=%d,nMaxId=%d,wID=%d",i,nMaxId,me->pAddrBookList->AddrBookInfo[i].wID);
				nMaxId++;
			}
		}
	}

	
ExitHere:
	FREEIF(naUsedId);
	FREEIF(naIdArray);

	return;
}

// =======================================================================================
// ö�ٵ�ַ��¼���
static int Addr_EnumCategory(crichAddr* me)
{
	int nErr = SUCCESS;
	int idx;
	AEEAddrCat oPc;

	newgimis *pMe;
	
	if (!me) return -1;
	pMe = (newgimis *)me->m_pParent;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_EnumCategory in");

    nErr = IADDRBOOK_EnumCategoryInit(me->m_pIAddrBook);
	if ( nErr != AEE_SUCCESS ) 	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumCategoryInit error=%d",nErr);
		goto ExitHere;
	}

    idx = 0;
	while( IADDRBOOK_EnumNextCategory ( me->m_pIAddrBook, &oPc ) )   {		// while -- 1
		nErr = SUCCESS;
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"per cat=%d",oPc);  // cat= 1 ���˼�¼
		switch (oPc)
		{
		case AEE_ADDR_CAT_NONE:
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CAT_NONE");
			break;
		case AEE_ADDR_CAT_PERSONAL:
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CAT_PERSONAL");   // ȡ�� 2013-02-01
			break;
		case AEE_ADDR_CAT_BUSINESS:
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CAT_BUSINESS");	 // ȡ�� 2013-02-01	
			break;
		case AEE_ADDR_CAT_USER_DEFINED:
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"USER_DEFINED");
			break;
		case AEE_ADDR_CAT_ERR:
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CAT_ERR");
			break;
		default:
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"other");
			break;
		}
		idx++;
	}	// while 
	

ExitHere:
	return nErr;

}


// ======================================================
// ö�ټ�¼���ֶ���Ϣ
// AEEAddrCat oBusiness = AEE_ADDR_CAT_BUSINESS;
// bEnumAll �� true  ö��ȫ�� �� false �ҶԽ�����
// return :  ���ҶԽ�����ʱ 1000 , �ҵ��Խ�����
static int Addr_EnumFieldsInfo(crichAddr* me,AEEAddrCat oBusiness,boolean bEnumAll)
{
//	int IADDRBOOK_EnumFieldsInfoInit (IAddrBook * pIAddrBook, AEEAddrCat c 	) 
	int nErr = AEE_SUCCESS;
	int idx;
	AEEAddrFieldInfo oAddrFldInfo;

	newgimis *pMe;

	if (!me) return -1;
	pMe= (newgimis *)me->m_pParent;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_EnumFieldsInfo in");
	
    nErr = IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook,oBusiness);
	if ( nErr != AEE_SUCCESS ) 	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_EnumFieldsInfo error=%d",nErr);
		goto ExitHere;
	}
	
    idx = 0;
	while( IADDRBOOK_EnumNextFieldsInfo ( me->m_pIAddrBook, &oAddrFldInfo ) )   {		// while -- 1
		AECHAR pAECName[50];
		int nSize = 50; 
		char sLogBuf[256];

		nErr = SUCCESS;
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumNextFieldsInfo ok, idx=%d",idx);
		MEMSET(sLogBuf,0,sizeof(sLogBuf));
		SPRINTF(sLogBuf,"fieldID=%d,nMaxItems=%d,fType=%d,bFieldSearch=%d,bFieldCatSearch=%d"
			   ,oAddrFldInfo.fieldID,oAddrFldInfo.nMaxItems,oAddrFldInfo.fType,oAddrFldInfo.bFieldSearch,oAddrFldInfo.bFieldCatSearch);
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),sLogBuf);

		if (!bEnumAll) {
			if ( (STRCMP(pMe->sMobileType,"33") == 0) ) {
				if (oAddrFldInfo.fieldID == AEE_ADDRFIELD_PHONE_VOICE) {	// X3 �ҵ��Խ������ֶ�
					nErr = 1000;
					break;
				}
			}
		}
			IADDRBOOK_GetFieldName(me->m_pIAddrBook,oAddrFldInfo.fieldID,pAECName,&nSize);
			MEMSET(sLogBuf,0,sizeof(sLogBuf));
			 WSTRTOSTR(pAECName,sLogBuf,sizeof(sLogBuf));
//			 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),sLogBuf);
		
		idx++;
	}	// while 
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "IADDRBOOK_EnumNextFieldsInfo ok,nErr=%d",nErr);
	
ExitHere:
	return nErr;
}

// ====================================================================
// �ͷ��û��绰���벾
static void Addr_FreeUsrAddrBook(crichAddr *me)
{
	newgimis *pMe;
	
	if (!me) return;
	pMe = (newgimis *)me->m_pParent;
	
	if (me->pAddrBookList != NULL) {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_FreeUsrAddrBook in, AdrBook count=%d",me->pAddrBookList->count);
		if (me->pAddrBookList->count >0)
		{
			FREEIF(me->pAddrBookList->AddrBookInfo);
			me->pAddrBookList->count = 0;
		}
	}
	
}

// =======================================================================================
// ���������ص�ͨ��¼��Ϣ���뵽�û��绰���벾(�ṹ)��
// ������ַ������磺�ֻ�����:ptt����:����:ְ��,�ֻ�����:ptt����:����:ְ��; 
//  ��¼֮����","���ֶ�֮����":"
//  ��������(���30���ֽ�,��bom utf-8)��ְ��(���50���ֽ�,��bom utf-8)
// ������18965901047:18965901048:����:�Ƴ�,18965901037:18965901038:����:��Ա QCA
// =======================================================================================
static void Addr_SetUsrAddr(crichAddr* me)
{
	int iLocNextRec, iLenRec;
	int iLocNextFld, iLenFld;
	int ndx;			// �ֶε�����
	char sc_tmp[128] = {0};

	newgimis *pMe;
	


	if (!me) return;
	pMe = (newgimis *)me->m_pParent;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_SetUsrAddr in");
	
	Addr_FreeUsrAddrBook(me);
	if (me->pAddrBookList == NULL) {
		me->pAddrBookList = (IAddrBookList *)MALLOC(sizeof(IAddrBookList));
		MEMSET(me->pAddrBookList,0,sizeof(IAddrBookList));
	}
	iLenRec = 0;
	iLocNextRec = -1;
	while(TRUE)	
	{		// record
		char *pRecord = NULL;
		// ȡ����¼
		iLocNextRec += iLenRec + 1;	// ��','�ĳ���
		if (iLocNextRec >= (int)STRLEN(me->pstrRecU) )
			break;
		iLenRec = ud_DistToSplit( me->pstrRecU+iLocNextRec, DEFAULT_SPLIT );
		if (iLenRec <= 0) break;

		pRecord = me->pstrRecU+iLocNextRec;		// ��¼�Ŀ�ʼλ��
		me->pAddrBookList->count += 1;
		
		if (me->pAddrBookList->count == 1)
			me->pAddrBookList->AddrBookInfo = (IUsrAddrBook *)MALLOC(sizeof(IUsrAddrBook));
		else
			me->pAddrBookList->AddrBookInfo = (IUsrAddrBook *)REALLOC( me->pAddrBookList->AddrBookInfo, me->pAddrBookList->count*sizeof(IUsrAddrBook) );
		MEMSET(me->pAddrBookList->AddrBookInfo+(me->pAddrBookList->count-1),0,sizeof(IUsrAddrBook));
		me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].wID = -1;

		iLenFld = 0;
		iLocNextFld = -1;
		ndx = -1;
		while(TRUE)
		{	// field
			char sTmp[128] = {0};
			
			// ȡ����¼
			iLocNextFld += iLenFld + 1;	// ��':'�ĳ���
			if (iLocNextFld >= iLenRec )
				break;
			iLenFld = ud_DistToSplit( pRecord+iLocNextFld, COLON_SPLIT );
			if (iLenFld <= 0) break;
			if ( (iLocNextFld +iLenFld) > iLenRec )
				iLenFld = iLenRec - iLocNextFld;
			ndx++;
							//			ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"in Addr_SetUsrAddr,ndx=%d,iLocNextFld=%d,iLenFld=%d",ndx,iLocNextFld,iLenFld);
			switch (ndx)
			{
			case 0:			// �ֻ���
				MEMCPY(sTmp,pRecord+iLocNextFld, iLenFld);
				MEMSET(sc_tmp,0x00,128);
				MEMCPY(sc_tmp,pRecord+iLocNextFld, iLenFld);
				UTF8TOWSTR((byte *)sTmp,STRLEN(sTmp),
					        me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sMobileNo,
							sizeof(me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sMobileNo));
				break;
			case 1:			// �Խ�����
				MEMCPY(sTmp,pRecord+iLocNextFld, iLenFld);
				UTF8TOWSTR((byte *)sTmp,STRLEN(sTmp),
					me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sPttNo,
					sizeof(me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sPttNo));
				break;
			case 2:		  // ����
				{
					MEMCPY(sTmp,pRecord+iLocNextFld, iLenFld);

//					UTF8TOWSTR((byte *)sTmp,STRLEN(sTmp),
//						me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sName,
//						sizeof(me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sName));	
					
					//  add 2015-10-29  //
//					uf_RecLog(pMe,"sTmp");
//					uf_RecLog(pMe,sTmp);
//					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"sTmp = %d",STRLEN(sTmp));
					if (STRCMP(sTmp," ") == 0)
					{
//						uf_RecLog(pMe,"����1");
//						uf_RecLog(pMe,sc_tmp);
//						ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"sc_tmp 222 = %d",STRLEN(sc_tmp));
						UTF8TOWSTR((byte *)sc_tmp,STRLEN(sc_tmp),	
						me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sName,
						sizeof(me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sName));	
						MEMSET(sc_tmp,0x00,128);
						WSTRTOSTR(me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sName,sc_tmp,sizeof(me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sName));

					}
					else
					{
//						uf_RecLog(pMe,"����2");
						UTF8TOWSTR((byte *)sTmp,STRLEN(sTmp),
						me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sName,
						sizeof(me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sName));	
					}
				}

				break;
			case 3:			// ְ��
				MEMCPY(sTmp,pRecord+iLocNextFld, iLenFld);
				UTF8TOWSTR((byte *)sTmp,STRLEN(sTmp),
					me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sJobtitle,
					sizeof(me->pAddrBookList->AddrBookInfo[me->pAddrBookList->count-1].sJobtitle));
				break;
			}
		}	// field

	}	// record

	return;
}


// =======================================================================================
// ��Ӽ�¼
/*
Hi, 

  I'm facing a problem where my device hang when I insert contacts to my application.
 The following codes have no problem running in Nokia handset. However when run in Hua Wei handset, the line 
  "pRec = IADDRBOOK_CreateRec(pMe->pAddrBook,AEE_ADDR_CAT_USER+20,(AEEAddrField*)field,1);" 
	cause the handset to hang. May I know what is the problem?
		
		  The following is part of my coding of insert contacts, anyone can help me? Thanks
		  
			if(STREQ(pMe->vName," "))
			{
			field[0].fID = AEE_ADDRFIELD_LASTNAME;
			field[0].fType = AEEDB_FT_STRING;
			STRCPY(pMe->szName, "");
			STRTOWSTR(pMe->szName,pMe->aszName,268);
			field[0].pBuffer= pMe->aszName;
			field[0].wDataLen = (WSTRLEN(pMe->aszName)+1)*sizeof(AECHAR);
			}
			
			  if(ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_ADDRBOOK, (void **)&pMe->pAddrBook)==SUCCESS)
			  {
			  IAddrRec *pRec;
			  
				//insert values into handset address book
				pRec = IADDRBOOK_CreateRec(pMe->pAddrBook,AEE_ADDR_CAT_USER+20,(AEEAddrField*)field,1);
				
				  if(pRec)
				  {
				  IADDRREC_Release(pRec);
				  }
				  else
				  {
				  int error = IADDRBOOK_GetLastError(pMe->pAddrBook);
				  DBGPRINTF("Error: %d", error);
				  }
				  
					IADDRBOOK_Release(pMe->pAddrBook);
					}

*/
// =======================================================================================
// ���ͨѶ¼��¼(�йغ���)
static int Addr_CreateRec_01(crichAddr* me)
{
	newgimis *pMe;
	AEEAddrCat nAddrCat;
	AEEAddrField *pItems = NULL;		// 	
	IAddrRec *pAddRec = NULL;
	int nItemCount = 18;			 //  ��18���ֶ�
	AECHAR asDefault[8] = L"default";
	word *nItemIdx = NULL;
	
	if (!me) return -1;
	pMe = (newgimis *)me->m_pParent;
	
	richptt_SetAddrCat(pMe,nAddrCat);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_CreateRec_01 in,count=%d,AddrCat=%d",me->pAddrBookList->count,nAddrCat);


	{	// for
		int i,j;
		for (i = 0; i<me->pAddrBookList->count; i++)
		{
			pItems = (AEEAddrField *)MALLOC(nItemCount * sizeof(AEEAddrField));
			// ����
			j = 0;
			pItems[j].fID = AEE_ADDRFIELD_NAME;
			pItems[j].fType = AEEDB_FT_STRING;
			pItems[j].wDataLen = (WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sName)+1) * 2;
			pItems[j].pBuffer = (void *)me->pAddrBookList->AddrBookInfo[i].sName;
			{
				char  sTmp[128] = {0};
				WSTRTOUTF8( me->pAddrBookList->AddrBookInfo[i].sName,WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sName),(byte *)sTmp,sizeof(sTmp) );   
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,name=%s,wDataLen=%d",
//					j,sTmp,pItems[j].wDataLen);
			}
			
			// �绰����
			j = 1;
			pItems[j].fID = AEE_ADDRFIELD_PHONE_GENERIC;
			pItems[j].fType = AEEDB_FT_PHONE;
			pItems[j].wDataLen = (WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sMobileNo)+1) * 2;
			pItems[j].pBuffer = (void *)me->pAddrBookList->AddrBookInfo[i].sMobileNo;
			{
				char  sTmp[128] = {0};
				WSTRTOUTF8( me->pAddrBookList->AddrBookInfo[i].sMobileNo,WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sMobileNo),(byte *)sTmp,sizeof(sTmp) );   
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,mobileNo=%s,wDataLen=%d",
//					j,sTmp,pItems[j].wDataLen);
			}
 		
  			j = 2;	// Home
  			pItems[j].fID = AEE_ADDRFIELD_PHONE_HOME;
  			pItems[j].fType = AEEDB_FT_STRING;
  			pItems[j].wDataLen = 0;
  			pItems[j].pBuffer = NULL;
  			j = 3;	// Work
  			pItems[j].fID = AEE_ADDRFIELD_PHONE_WORK;
  			pItems[j].fType = AEEDB_FT_STRING;
  			pItems[j].wDataLen = 0;
  			pItems[j].pBuffer = NULL;
  			j = 4;	// Email
  			pItems[j].fID = AEE_ADDRFIELD_EMAIL;
  			pItems[j].fType = AEEDB_FT_STRING;
  			pItems[j].wDataLen = 0;
  			pItems[j].pBuffer = NULL;
 	
			// ְ�� 
			j = 5;
			pItems[j].fID =  AEE_ADDRFIELD_NOTES;			//  AEE_ADDRFIELD_JOBTITLE		AEE_ADDRFIELD_ORG
			pItems[j].fType = AEEDB_FT_STRING;
			pItems[j].wDataLen = (WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sJobtitle)+1) * 2;
			pItems[j].pBuffer = (void *)me->pAddrBookList->AddrBookInfo[i].sJobtitle;
			{
				char  sTmp[128] = {0};
				WSTRTOUTF8( me->pAddrBookList->AddrBookInfo[i].sJobtitle,WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sJobtitle),(byte *)sTmp,sizeof(sTmp) );   
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,Jobtitle=%s,wDataLen=%d",
//					j,sTmp,pItems[j].wDataLen);
			}

 			j = 6;	// URL
 			pItems[j].fID = AEE_ADDRFIELD_URL;
 			pItems[j].fType = AEEDB_FT_STRING;
 			pItems[j].wDataLen = 0;
 			pItems[j].pBuffer = NULL;

  			j = 7;	// URL  default = Ĭ������
  			pItems[j].fID = 50;
  			pItems[j].fType = AEEDB_FT_STRING;
  			pItems[j].wDataLen = 16;
  			pItems[j].pBuffer = (void *)asDefault;
  			{
  				char  sTmp[128] = {0};
  				WSTRTOUTF8( pItems[j].pBuffer,WSTRLEN(pItems[j].pBuffer),(byte *)sTmp,sizeof(sTmp) );   
//  				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,default=%s,wDataLen=%d",
//  					j,sTmp,pItems[j].wDataLen);
  			}
 
  			j = 8;	// URL 
  			pItems[j].fID = 48;
  			pItems[j].fType = AEEDB_FT_PHONE;
  			pItems[j].wDataLen = 0;
  			pItems[j].pBuffer = NULL;
 
 			j = 9;	// URL  ��ʵ������
 			pItems[j].fID = 49;
 			pItems[j].fType = AEEDB_FT_WORD;		// AEEDB_FT_WORD;
			{
				char sInt[6];		// �ɴ�5λ��id���㹻����
				MEMSET(sInt,0,sizeof(sInt));
				SPRINTF(sInt,"%d",me->pAddrBookList->AddrBookInfo[i].wID);
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"word data[sInt=%s]",sInt);
				FREEIF(nItemIdx);
				nItemIdx = hesense_strtoword(sInt);
  				pItems[j].wDataLen = (STRLEN(sInt)+1)*sizeof(word);
  				pItems[j].pBuffer = nItemIdx;
			}

 			j = 10;	// URL 
 			pItems[j].fID = AEE_ADDRFIELD_PHOTOURL;
  			pItems[j].fType = AEEDB_FT_STRING;
  			pItems[j].wDataLen = 0;
  			pItems[j].pBuffer = NULL;
  			j = 11;	// URL 
  			pItems[j].fID = AEE_ADDRFIELD_ADDRESS;
  			pItems[j].fType = AEEDB_FT_STRING;
  			pItems[j].wDataLen = 0;
  			pItems[j].pBuffer = NULL;
 			j = 12;	// URL  ����
			pItems[j].fID = AEE_ADDRFIELD_BDAY;
 			pItems[j].fType = AEEDB_FT_DWORD;
 			pItems[j].wDataLen = 0;
 			pItems[j].pBuffer = NULL;
 			j = 13;	// URL 
 			pItems[j].fID = AEE_ADDRFIELD_CITY;
 			pItems[j].fType = AEEDB_FT_STRING;
 			pItems[j].wDataLen = 0;
 			pItems[j].pBuffer = NULL;
 			j = 14;	// URL 
 			pItems[j].fID = AEE_ADDRFIELD_STATE;
 			pItems[j].fType = AEEDB_FT_STRING;
 			pItems[j].wDataLen = 0;
 			pItems[j].pBuffer = NULL;
 			j = 15;	// URL 
 			pItems[j].fID = AEE_ADDRFIELD_ZIPCODE;
 			pItems[j].fType = AEEDB_FT_STRING;
 			pItems[j].wDataLen = 0;
 			pItems[j].pBuffer = NULL;
 			j = 16;	// URL 
 			pItems[j].fID = AEE_ADDRFIELD_COUNTRY;
 			pItems[j].fType = AEEDB_FT_STRING;
			pItems[j].wDataLen = 0;
 			pItems[j].pBuffer = NULL;

 			// ptt����
   			j = 17;		// Ӧ��17
   			pItems[j].fID = 60;
   			pItems[j].fType = AEEDB_FT_STRING;
   			pItems[j].wDataLen = (WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sPttNo)+1) * 2;
   			pItems[j].pBuffer =  (void *)me->pAddrBookList->AddrBookInfo[i].sPttNo;
   			{
   				char  sTmp[128] = {0};
   				WSTRTOUTF8( me->pAddrBookList->AddrBookInfo[i].sPttNo,WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sPttNo),(byte *)sTmp,sizeof(sTmp) );   
//   				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,PttNo=%s,wDataLen=%d",
//   					j,sTmp,pItems[j].wDataLen);
   			}

			pAddRec = IADDRBOOK_CreateRec(me->m_pIAddrBook,nAddrCat ,pItems,nItemCount);	// AEE_ADDR_CAT_PERSONAL,AEE_ADDR_CAT_BUSINESS,  AEE_ADDR_CAT_NONE
			if (pAddRec != NULL)
			{
				int nErr = AEE_SUCCESS;
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CreateRec out haixin, ok, nErr=%d",nErr);
			}
			else {	
				int err= IADDRBOOK_GetLastError(me->m_pIAddrBook); 
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CreateRec out haixin, err code=%d",err);
			}

			FREEIF(pItems);
		}
	}	// for

	FREEIF(nItemIdx);

	return 1;
}
// ====================================================================
// ���ͨѶ¼��¼(�й�������Ԫ��)
static int Addr_CreateRec_33(crichAddr* me)
{
	newgimis *pMe;
	AEEAddrCat nAddrCat;
	AEEAddrField *pItems = NULL;		// 	
	IAddrRec *pAddRec = NULL;
	int nItemCount = 4;		
	int nChkQchatFldRtn = 0;		//  1000 �Ƿ���ڶԽ�����
	
	if (!me) return -1;
	pMe = (newgimis *)me->m_pParent;

	nChkQchatFldRtn = Addr_EnumFieldsInfo(me,AEE_ADDR_CAT_PERSONAL,FALSE);	

	if (nChkQchatFldRtn == 0)   // ��ʾ PhoneMenu is Empty, PhoneBook is Empty
		nItemCount = 3;

	richptt_SetAddrCat(pMe,nAddrCat);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_CreateRec_33 in,count=%d,AddrCat=%d,CheckkQchatFieldReturn=%d",me->pAddrBookList->count,nAddrCat,nChkQchatFldRtn);

	
	{	// for
		int i,j;
		for (i = 0; i<me->pAddrBookList->count; i++)
		{
			pItems = (AEEAddrField *)MALLOC(nItemCount * sizeof(AEEAddrField));
			// ����
			j = 0;
			pItems[j].fID = AEE_ADDRFIELD_NAME;
			pItems[j].fType = AEEDB_FT_STRING;
			pItems[j].wDataLen = (WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sName)+1) * 2;
			pItems[j].pBuffer = (void *)me->pAddrBookList->AddrBookInfo[i].sName;
			{
				char  sTmp[128] = {0};
				WSTRTOUTF8( me->pAddrBookList->AddrBookInfo[i].sName,WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sName),(byte *)sTmp,sizeof(sTmp) );   
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,name=%s,wDataLen=%d",
//					j,sTmp,pItems[j].wDataLen);
			}
	
			// �绰����
			j += 1;
			pItems[j].fID = AEE_ADDRFIELD_PHONE_CELL;
			pItems[j].fType = AEEDB_FT_STRING;
			pItems[j].wDataLen = (WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sMobileNo)+1) * 2;
			pItems[j].pBuffer = (void *)me->pAddrBookList->AddrBookInfo[i].sMobileNo;
			{
				char  sTmp[128] = {0};
				WSTRTOUTF8( me->pAddrBookList->AddrBookInfo[i].sMobileNo,WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sMobileNo),(byte *)sTmp,sizeof(sTmp) );   
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,mobileNo=%s,wDataLen=%d",
//					j,sTmp,pItems[j].wDataLen);
			}

			// ptt����
			if (nChkQchatFldRtn == 1000) {
				j += 1;		
				pItems[j].fID = AEE_ADDRFIELD_PHONE_VOICE;
				pItems[j].fType = AEEDB_FT_STRING;
				pItems[j].wDataLen = (WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sPttNo)+1) * 2;
				pItems[j].pBuffer =  (void *)me->pAddrBookList->AddrBookInfo[i].sPttNo;
				{
					char  sTmp[128] = {0};
					WSTRTOUTF8( me->pAddrBookList->AddrBookInfo[i].sPttNo,WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sPttNo),(byte *)sTmp,sizeof(sTmp) );   
//					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,PttNo=%s,wDataLen=%d",
//						j,sTmp,pItems[j].wDataLen);
				}
			}

			// ְ�� 
			j += 1;
			pItems[j].fID =  AEE_ADDRFIELD_NOTES;			//  AEE_ADDRFIELD_JOBTITLE		AEE_ADDRFIELD_ORG
			pItems[j].fType = AEEDB_FT_STRING;
			pItems[j].wDataLen = (WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sJobtitle)+1) * 2;
			pItems[j].pBuffer = (void *)me->pAddrBookList->AddrBookInfo[i].sJobtitle;
			{
				char  sTmp[128] = {0};
				WSTRTOUTF8( me->pAddrBookList->AddrBookInfo[i].sJobtitle,WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sJobtitle),(byte *)sTmp,sizeof(sTmp) );   
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,Jobtitle=%s,wDataLen=%d",
//					j,sTmp,pItems[j].wDataLen);
			}
			
			pAddRec = IADDRBOOK_CreateRec(me->m_pIAddrBook,nAddrCat ,pItems,nItemCount);	// AEE_ADDR_CAT_PERSONAL,AEE_ADDR_CAT_BUSINESS,  AEE_ADDR_CAT_NONE
			if (pAddRec != NULL) 		{
				int nErr = AEE_SUCCESS;
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CreateRec out, ok, nErr=%d,nItemCount=%d",nErr,nItemCount);
			}
			else {	
				int err= IADDRBOOK_GetLastError(me->m_pIAddrBook); 
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CreateRec out, err code=%d,nItemCount=%d",err,nItemCount);
			}
			
			FREEIF(pItems);
		}
	}	// for
	
	
	return 1;
}

// ====================================================================
// ���ͨѶ¼��¼(�й����Ƶ�)
static int Addr_CreateRec_58(crichAddr* me)
{
	newgimis *pMe;
	AEEAddrCat nAddrCat;
	AEEAddrField *pItems = NULL;		// 	
	IAddrRec *pAddRec = NULL;
	int nItemCount = 4;		
	
	if (!me) return -1;
	pMe = (newgimis *)me->m_pParent;
	
	richptt_SetAddrCat(pMe,nAddrCat);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_CreateRec_58 in,count=%d,AddrCat=%d",me->pAddrBookList->count,nAddrCat);

	{	// for
		int i,j;
		for (i = 0; i<me->pAddrBookList->count; i++)
		{
			pItems = (AEEAddrField *)MALLOC(nItemCount * sizeof(AEEAddrField));
			// ����

			j = 0;
			pItems[j].fID = AEE_ADDRFIELD_NAME;
			pItems[j].fType = AEEDB_FT_STRING;
			pItems[j].wDataLen = (WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sName)+1) * 2;
			pItems[j].pBuffer = (void *)me->pAddrBookList->AddrBookInfo[i].sName;
			{
				char  sTmp[128] = {0};
				WSTRTOUTF8( me->pAddrBookList->AddrBookInfo[i].sName,WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sName),(byte *)sTmp,sizeof(sTmp) );   
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,name=%s,wDataLen=%d",
//					j,sTmp,pItems[j].wDataLen);
			}
			
			// ptt����
			j = 1;		
			pItems[j].fID = AEE_ADDRFIELD_PHONE_OTHER;
			pItems[j].fType = AEEDB_FT_PHONE;
			pItems[j].wDataLen = (WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sPttNo)+1) * 2;
			pItems[j].pBuffer =  (void *)me->pAddrBookList->AddrBookInfo[i].sPttNo;
			{
				char  sTmp[128] = {0};
				WSTRTOUTF8( me->pAddrBookList->AddrBookInfo[i].sPttNo,WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sPttNo),(byte *)sTmp,sizeof(sTmp) );   
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,PttNo=%s,wDataLen=%d",
//					j,sTmp,pItems[j].wDataLen);
			}

			// �绰����
			j = 2;
			pItems[j].fID = AEE_ADDRFIELD_PHONE_CELL;
			pItems[j].fType = AEEDB_FT_PHONE;
			pItems[j].wDataLen = (WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sMobileNo)+1) * 2;
			pItems[j].pBuffer = (void *)me->pAddrBookList->AddrBookInfo[i].sMobileNo;
			{
				char  sTmp[128] = {0};
				WSTRTOUTF8( me->pAddrBookList->AddrBookInfo[i].sMobileNo,WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sMobileNo),(byte *)sTmp,sizeof(sTmp) );   
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,mobileNo=%s,wDataLen=%d",
//					j,sTmp,pItems[j].wDataLen);
			}

 			// ְ�� 
			j = 3;
			pItems[j].fID =  AEE_ADDRFIELD_ADDRESS;			//  AEE_ADDRFIELD_JOBTITLE		AEE_ADDRFIELD_ORG
			pItems[j].fType = AEEDB_FT_STRING;
			pItems[j].wDataLen = (WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sJobtitle)+1) * 2;
			pItems[j].pBuffer = (void *)me->pAddrBookList->AddrBookInfo[i].sJobtitle;
			{
				char  sTmp[128] = {0};
				WSTRTOUTF8( me->pAddrBookList->AddrBookInfo[i].sJobtitle,WSTRLEN(me->pAddrBookList->AddrBookInfo[i].sJobtitle),(byte *)sTmp,sizeof(sTmp) );   
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pItems index=%d,Jobtitle=%s,wDataLen=%d",
//					j,sTmp,pItems[j].wDataLen);
			}

			pAddRec = IADDRBOOK_CreateRec(me->m_pIAddrBook,nAddrCat ,pItems,nItemCount);	// AEE_ADDR_CAT_PERSONAL,AEE_ADDR_CAT_BUSINESS,  AEE_ADDR_CAT_NONE
			if (pAddRec != NULL) 		{
				int nErr = AEE_SUCCESS;
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CreateRec out xinyun, ok, nErr=%d",nErr);
			}
			else {	
				int err= IADDRBOOK_GetLastError(me->m_pIAddrBook); 
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"CreateRec out xinyun, err code=%d",err);
			}

			FREEIF(pItems);
		}
	}	// for

	
	return 1;
}

// ======================================================
// ö�ټ�¼����Ϣ
static int Addr_EnumRecInfo(crichAddr* me)
{
	int nErr = AEE_SUCCESS;
	newgimis *pMe;

	AEEAddrCat nAddrCat;
	IAddrRec *pAddRec = NULL;
	AEEAddrField *pAddrField = NULL;
	int nFieldCount = 0;
	int nRecId = 0;

	if (!me) return -1;

	pMe = (newgimis *)me->m_pParent;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_EnumRecInfo in");

	nRecId = IADDRBOOK_GetNumRecs(me->m_pIAddrBook); 
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_GetNumRecs return=%d",nRecId);  // ��¼��
	
	richptt_SetAddrCat(pMe,nAddrCat);
	// ö��ȫ��										  AEE_ADDR_CAT_NONE,AEE_ADDR_CAT_BUSINESS,AEE_ADDR_CAT_PERSONAL 
    nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_NONE, AEE_ADDRFIELD_NONE, NULL,0 ); 
	if ( nErr != AEE_SUCCESS ) 	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_EnumRecInfo error=%d",nErr);
		goto ExitHere;
	}

	nRecId = 0;
	while( TRUE )   {		// while -- 1
		char sLogBuf[1001];
		int idx = 0;
		int ndx = 0;

 		pAddRec = IADDRBOOK_EnumNextRec(me->m_pIAddrBook);
 		if (pAddRec == NULL)
 		{
 			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"nRecId=%d,pAddRec == NULL",nRecId);
			break;
 		}

		nFieldCount = IADDRREC_GetFieldCount(pAddRec);   // �ֶ���
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"nRecId=%d,nFieldCount=%d",nRecId,nFieldCount);

		idx = 0;
		MEMSET(sLogBuf,0,sizeof(sLogBuf));
		STRCPY(sLogBuf,"Recval=");
		for (ndx = 0; ndx < nFieldCount; ndx++)
		{
			char sTmp[126];
			char sFldName[50];
			AECHAR pAECFldName[50];
			int nSize = sizeof(sFldName); 

			pAddrField = IADDRREC_GetField(pAddRec,ndx);
			if (pAddrField == NULL)
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ndx=%d,pAddrField == NULL",ndx);
				break;
			}

			MEMSET(pAECFldName,0,sizeof(pAECFldName));
			IADDRBOOK_GetFieldName(me->m_pIAddrBook,pAddrField->fID,pAECFldName,&nSize);
			MEMSET(sFldName,0,sizeof(sFldName));
			WSTRTOUTF8( pAECFldName,WSTRLEN(pAECFldName),(byte *)sFldName,sizeof(sFldName) );   

			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ndx=%d,nSize=%d,name=%s,fID=%d,fType=%d",ndx,nSize,sFldName,pAddrField->fID,pAddrField->fType);

			if (STRLEN(sFldName) > 0)
			   SPRINTF(sLogBuf, "%s[%s]=",sLogBuf,sFldName);

			MEMSET(sTmp,0,sizeof(sTmp));
			switch (pAddrField->fType)
			{
				case AEEDB_FT_STRING:
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEEDB_FT_STRING,WSTRLEN=%d, wDataLen=%d",WSTRLEN(pAddrField->pBuffer),pAddrField->wDataLen);
 					if (pAddrField->wDataLen > 0) {
 						 WSTRTOUTF8(pAddrField->pBuffer,WSTRLEN(pAddrField->pBuffer),(byte *)sTmp, sizeof(sTmp));  // ���Ƶĳ���Ҫȫ�������Ų�ͬ��pAddrField->wDataLen+2
						 SPRINTF(sLogBuf,"%s%s,",sLogBuf,sTmp);
 						 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"STRING,sTmp=%s",sTmp);
 					}
					else
						SPRINTF(sLogBuf,"%sSTRING null,",sLogBuf);
					break;
				case AEEDB_FT_PHONE:
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEEDB_FT_PHONE wDataLen=%d",pAddrField->wDataLen);
					if (pAddrField->wDataLen > 0) {
						WSTRTOUTF8(pAddrField->pBuffer,WSTRLEN(pAddrField->pBuffer),(byte *)sTmp,pAddrField->wDataLen );
						SPRINTF(sLogBuf,"%s%s,",sLogBuf,sTmp);
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"PHONE,sTmp=%s",sTmp);
					}
					else
						SPRINTF(sLogBuf,"%sPHONE null,",sLogBuf);
					break;
				case AEEDB_FT_WORD:
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEEDB_FT_WORD wDataLen=%d",pAddrField->wDataLen);
					if (pAddrField->wDataLen > 0) {
						word nItemIdx = 0;
						nItemIdx = *((word *)pAddrField->pBuffer);

						{
							char sAscWord[20+1];
							ud_ToAscFmt((char *)pAddrField->pBuffer,pAddrField->wDataLen,sAscWord,sizeof(sAscWord));
							ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"WORD asc=%s,buffer len=%d",sAscWord,WSTRLEN(pAddrField->pBuffer) );

						}

						WSTRTOUTF8(pAddrField->pBuffer,WSTRLEN(pAddrField->pBuffer),(byte *)sTmp,pAddrField->wDataLen );
						SPRINTF(sLogBuf,"%s%s,",sLogBuf,sTmp);
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"WORD,sTmp=%s[%d]",sTmp,nItemIdx);
					}
					else
						SPRINTF(sLogBuf,"%sWORD null,",sLogBuf);
					break;
				case AEEDB_FT_DWORD:
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEEDB_FT_DWORD wDataLen=%d",pAddrField->wDataLen);
					if (pAddrField->wDataLen > 0) {
//						WSTRTOUTF8(pAddrField->pBuffer,WSTRLEN(pAddrField->pBuffer),(byte *)sTmp,pAddrField->wDataLen );
						SPRINTF(sLogBuf,"%s%u,",sLogBuf,pAddrField->pBuffer);
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"DWORD,sTmp=%u",pAddrField->pBuffer);
					}
					else
						SPRINTF(sLogBuf,"%sDWORD null,",sLogBuf);

					break;
				default:
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"other type=%d, wDataLen=%d",pAddrField->fType,pAddrField->wDataLen);
					break;
			}

			// ���ó��򲻿��ͷŻ���Ĵ˺������ص�ָ�� FREE(pAddrField);
		}

		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),sLogBuf);

// ����Ҳ��Ҫ�ͷ�		FREE(pAddRec);
		nRecId++;

	} // while -- 1

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_EnumRecInfo finish,nRecId=%d",nRecId);


ExitHere:
	return nErr;
}

/*
// MEMSET(sLogBuf,0,sizeof(sLogBuf));
// STRCPY(sLogBuf,"pMe->pText AECHAR=");
// {
// 	char sTemp[(TEXT_MAXSIZE+1)*2];
// 	int nTextLen;
// 	int idx;
// 	MEMSET(sTemp,0,sizeof(sTemp));
// 	nTextLen = WSTRLEN(me->m_pParent->pText) * sizeof(AECHAR);
// 	MEMCPY(sTemp,me->m_pParent->pText,nTextLen);
// 	for(idx=0;idx<nTextLen;idx++)
// 	{
// 		char cTmp = sTemp[idx];
// 		char sTmp[5];
// 		SPRINTF(sTmp,"%d,",cTmp);
// 		STRCAT(sLogBuf,sTmp);
// 	}
// }
// uf_RecLog(me->m_pParent,sLogBuf);

*/


// IADDRBOOK_EnumRecInit , IAddrRec * IADDRBOOK_EnumNextRec ,IADDRREC_GetFieldCount(),IADDRREC_GetField()
/* typedef struct
{
	AEEAddrFieldID       fID;
	AEEAddrFieldType     fType;
	void *               pBuffer;
	uint16               wDataLen;
} AEEAddrField;

  typedef enum {
  AEEDB_FT_NONE,
  AEEDB_FT_BYTE,
  AEEDB_FT_WORD,
  AEEDB_FT_DWORD,
  AEEDB_FT_STRING,
  AEEDB_FT_BINARY,
  AEEDB_FT_PHONE,
  AEEDB_FT_BITMAP,
  AEEDB_FT_MAX
  } AEEDBFieldType;
  
		��Ա��
	AEEDB_FT_BYTE�� ���� 8 λֵ���ֶ� 
	AEEDB_FT_WORD�� ���� 16 λֵ���ֶ� 
	AEEDB_FT_DWORD�� ���� 32 λֵ���ֶ� 
	AEEDB_FT_STRING�� ���� AECHAR ���飨�� 0 ��β�����ֶ� 
	AEEDB_FT_BINARY�� ����������ֵ���ֶ� 
	AEEDB_FT_PHONE�� �����绰������ֶ� 
	AEEDB_FT_BITMAP�� ���� .BMP ��ʽλͼ���ֶ� 

*/

int addbook_init(newgimis *pMe)
{
	Addr_InitAppData(pMe);
	return 0;
}
int addbook_free(newgimis *pMe)
{
	crichAddr *me;
	me = pMe->pRichAddr;
	Addr_FreeAppData( me );
	return 0;
}



int Add_addbook(newgimis *pMe)
{
	crichAddr *me;

	me = pMe->pRichAddr;


// 	if ( STRCMP(pMe->sMobileType,"01") == 0 /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/) 
// 	{
// 		uf_RecLog(pMe,"do_PhoneNumber_01 come");
// 		Addr_EnumCategory(me);
// 		do_PhoneNumber_01( pMe);
// 
// 		
// 	}
// 	else if ((STRCMP(pMe->sMobileType,"03") == 0) || (STRCMP(pMe->sMobileType,"58") == 0)  || (STRCMP(pMe->sMobileType,"55") == 0) || (STRCMP(pMe->sMobileType,"63") == 0)  || (STRCMP(pMe->sMobileType,"33") == 0) || STRCMP(pMe->sMobileType,"57")==0 )  // 03 �Ǳ����ֻ���  33����һ���ֻ�
// 	{
// 		do_PhoneNumber_03(pMe);
// 	}
// 	else
// 	{
// 
// 		do_PhoneNumber( pMe);
// 	}
	


	Addr_SetUsrAddr(me);		// ����ɾ��ǰ�棬Ϊ�˵õ�����ֵ

	if ( (STRCMP(pMe->sMobileType,"01") == 0)  /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/) 
	{
		Addr_DelAddressForAdd_01(me,me->pstrRecU);
		Addr_CreateRec_01(me);
	}
	else {
//		Addr_DelAddress(me,me->pstrRecU);
		if ( (STRCMP(pMe->sMobileType,"33") == 0) )
		{
			Addr_DelAddressForAdd(me);
			Addr_CreateRec_33(me);
		}

		else if ( (STRCMP(pMe->sMobileType,"15") == 0) )  // �����ֻ�������������� 
		{
			Addr_DelAddressForAdd(me);
			Addr_CreateRec_33(me);
		}

		else
		{
			Addr_DelAddressForAdd(me);
			Addr_CreateRec_58(me);
		}

	}

	FREEIF(me->pstrRecU);

	return 0;
}

void Addr_DelAddressForAdd(crichAddr* me)
{
	int nErr = AEE_SUCCESS;
	newgimis *pMe;
	AEEAddrCat nAddrCat;
	
	IAddrRec *pAddRec = NULL;
	int nRecId = 0;
	int i = 0;
	AEEAddrField *pAddrField = NULL;
	
	if (!me) return;
	pMe = (newgimis *)me->m_pParent;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd in");
	
	// 	if ( (STRCMP(pMe->sMobileType,"55") == 0) || (STRCMP(pMe->sMobileType,"58") == 0) )
	// 		nAddrCat = AEE_ADDR_CAT_PERSONAL;
	// 	else
	// 		nAddrCat = AEE_ADDR_CAT_BUSINESS;
	richptt_SetAddrCat(pMe,nAddrCat);


	// �����ֻ�����ɾ����¼								AEE_ADDR_CAT_NONE,AEE_ADDR_CAT_PERSONAL
    nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, nAddrCat , AEE_ADDRFIELD_NONE, NULL,0 );	// AEE_ADDR_CAT_BUSINESS
	if ( nErr != AEE_SUCCESS ) 	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd,IADDRBOOK_EnumRecInit error=%d",nErr);
		goto ExitHere;
	}
	
	nRecId = -1;
	while( TRUE )   {		// while -- 1
		int ndx = 0;
		//		int nResult = 0;
		char sPhoneNo[QDK_CALL_NAME_LEN+1];
		//		int nSize = sizeof(sPhoneNo);  
		nRecId++;
		
		pAddRec = IADDRBOOK_EnumNextRec(me->m_pIAddrBook);
		if (pAddRec == NULL)
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd,nRecId=%d,pAddRec == NULL",nRecId);
			break;
		}
		// �ֻ�������ͨ��¼��λ��
		if (STRCMP(pMe->sMobileType,"01") == 0 /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/)
			ndx = PHONE_LOC_01;
		else if ( (STRCMP(pMe->sMobileType,"55") == 0) || (STRCMP(pMe->sMobileType,"58") == 0) || (STRCMP(pMe->sMobileType,"03") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0) || STRCMP(pMe->sMobileType,"57")==0 )
			ndx = 2;
		else
			ndx = 1;
		
		pAddrField = IADDRREC_GetField(pAddRec,ndx);
		if (pAddrField == NULL) {
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd,nRecId=%d,ndx=%d,pAddrField == NULL",nRecId,ndx);
			continue;
		}
		
		MEMSET(sPhoneNo,0,sizeof(sPhoneNo));
		if (pAddrField->wDataLen > 0) {	// if -- 12
			WSTRTOUTF8(pAddrField->pBuffer,WSTRLEN(pAddrField->pBuffer),(byte *)sPhoneNo,pAddrField->wDataLen );
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd, sPhoneNo=%s",sPhoneNo);
			if ( STRSTR(me->pstrRecU,sPhoneNo) != NULL) {	
				nErr = IADDRREC_RemoveRec(pAddRec);
				if ( AEE_SUCCESS != nErr )
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd,IADDRREC_RemoveRec error");
				else
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_DelAddressForAdd,IADDRREC_RemoveRec=%s",sPhoneNo);
				
			}
		}	// if -- 12
		
	}	// while -- 1

ExitHere:
	return;
}


int Del_addbook(newgimis *pMe)
{
	crichAddr *me;
	me = pMe->pRichAddr;
	Addr_DelAddress(me,me->pstrRecD);
	FREEIF(me->pstrRecD);
	return 0;
}
int Del_all_addbook(newgimis *pMe)
{
	crichAddr *me;
	me = pMe->pRichAddr;
	Addr_RemoveAll(me);
	return 0;
}



////////  add  2015-11-14  start ////////////////
static int Addr_Find_Number_03(crichAddr *me,char *sAddrInfo,newgimis *pwa)
{
	int nErr = AEE_SUCCESS;
	newgimis *pMe = (newgimis *)pwa;
	
	AEEAddrCat nAddrCat;
	IAddrRec *pAddRec = NULL;
	int nRecId = 0;
	AEEAddrField *pAddrField = NULL;

	AECHAR szNum[64];

	if (!me) return -1;
	pMe = (newgimis *)me->m_pParent;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_Find_Number in 03");


// 	if ( (STRCMP(pMe->sMobileType,"55") == 0) || (STRCMP(pMe->sMobileType,"58") == 0) )
// 		nAddrCat = AEE_ADDR_CAT_PERSONAL;
// 	else
// 		nAddrCat = AEE_ADDR_CAT_BUSINESS;


//	richptt_SetAddrCat(pMe,nAddrCat);
	nAddrCat = 2;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumRecInit nAddrCat=%d",nAddrCat);

	// �����ֻ�����ɾ����¼								AEE_ADDR_CAT_NONE,AEE_ADDR_CAT_PERSONAL
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumRecInit nAddrCat=%s",sAddrInfo);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumRecInit nAddrCat=%d",STRLEN(sAddrInfo));
	STR_TO_WSTR(sAddrInfo,szNum,64);
 //   nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_NONE , AEE_ADDRFIELD_NONE, sAddrInfo,STRLEN(sAddrInfo) );	// AEE_ADDR_CAT_BUSINESS
    nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_NONE , AEE_ADDRFIELD_NONE, szNum,64 );	// AEE_ADDR_CAT_BUSINESS

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumRecInit nErr 1=%d",nErr);
	if ( nErr != AEE_SUCCESS ) 	
	{ // û���ҵ� 
		return  0;
	}

	pAddRec = IADDRBOOK_EnumNextRec(me->m_pIAddrBook);
	if (!pAddRec) //˵��û���ҵ�
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumNextRec pAddRec=%d",pAddRec);
		return  0;
	}
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumNextRec pAddRec=%d",pAddRec);

	// IADDRBOOK_Release(me->m_pIAddrBook);
	return 1;
}



////////  add  2015-10-29  start ////////////////
// ����ָ���绰�����Ƿ����
// ����ֵ  = 1 ˵�����ҵ��绰����  = 0 ˵��û���ҵ��绰����
static int Addr_Find_Number(crichAddr *me,char *sAddrInfo,newgimis *pwa)
{
	int nErr = AEE_SUCCESS;
	newgimis *pMe = (newgimis *)pwa;
	
	AEEAddrCat nAddrCat;
	IAddrRec *pAddRec = NULL;
	int nRecId = 0;
	AEEAddrField *pAddrField = NULL;

	AECHAR szNum[64];

	if (!me) return -1;
	pMe = (newgimis *)me->m_pParent;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Addr_Find_Number in");


// 	if ( (STRCMP(pMe->sMobileType,"55") == 0) || (STRCMP(pMe->sMobileType,"58") == 0) )
// 		nAddrCat = AEE_ADDR_CAT_PERSONAL;
// 	else
// 		nAddrCat = AEE_ADDR_CAT_BUSINESS;
	richptt_SetAddrCat(pMe,nAddrCat);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumRecInit nAddrCat=%d",nAddrCat);

	// �����ֻ�����ɾ����¼								AEE_ADDR_CAT_NONE,AEE_ADDR_CAT_PERSONAL
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumRecInit nAddrCat=%s",sAddrInfo);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumRecInit nAddrCat=%d",STRLEN(sAddrInfo));
	STR_TO_WSTR(sAddrInfo,szNum,64);
 //   nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_NONE , AEE_ADDRFIELD_NONE, sAddrInfo,STRLEN(sAddrInfo) );	// AEE_ADDR_CAT_BUSINESS
    nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_NONE , AEE_ADDRFIELD_NONE, szNum,64 );	// AEE_ADDR_CAT_BUSINESS

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumRecInit nErr 1=%d",nErr);
	if ( nErr != AEE_SUCCESS ) 	
	{ // û���ҵ� 
		return  0;
	}

	pAddRec = IADDRBOOK_EnumNextRec(me->m_pIAddrBook);
	if (!pAddRec) //˵��û���ҵ�
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumNextRec pAddRec=%d",pAddRec);
		return  0;
	}
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumNextRec pAddRec=%d",pAddRec);

	// IADDRBOOK_Release(me->m_pIAddrBook);
	return 1;
}


// ����˵�� ���һ������ pAddrbook ������Ż�ȡ���ĵ绰����
static int Addr_Find_Number_haixin(crichAddr *me,newgimis *pwa,char *pAddrbook)
{
	int nErr = AEE_SUCCESS;
	newgimis *pMe = (newgimis *)pwa;
	
	AEEAddrCat nAddrCat;
	IAddrRec *pAddRec = NULL;
	int nRecId = 0;
	AEEAddrField *pAddrField = NULL;


	if (!me) return -1;
	pMe = (newgimis *)me->m_pParent;

	//////////////////               /////////////////////           ////////////////////////////
	nErr =IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook, 0);
	if (nErr == AEE_ADDRBOOK_NOCATSUPPORT)
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_ADDRBOOK_NOCATSUPPORT 0=%d",AEE_ADDRBOOK_NOCATSUPPORT);
	}
	else
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumFieldsInfoInit nErr 0 =%d",nErr);
	}

	nErr =IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook, 1);
	if (nErr == AEE_ADDRBOOK_NOCATSUPPORT)
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_ADDRBOOK_NOCATSUPPORT 1 =%d",AEE_ADDRBOOK_NOCATSUPPORT);
	}
	else
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumFieldsInfoInit nErr 1 =%d",nErr);
	}

	nErr =IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook, 2);
	if (nErr == AEE_ADDRBOOK_NOCATSUPPORT)
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_ADDRBOOK_NOCATSUPPORT 2 =%d",AEE_ADDRBOOK_NOCATSUPPORT);
	}
	else
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumFieldsInfoInit nErr 2 =%d",nErr);
	}

	//////////////////               /////////////////////           ////////////////////////////

	nErr =IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook, AEE_ADDRFIELD_NAME);
	if (nErr == AEE_ADDRBOOK_NOCATSUPPORT)
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_ADDRBOOK_NOCATSUPPORT =%d",AEE_ADDRBOOK_NOCATSUPPORT);
	}
	else
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumFieldsInfoInit nErr =%d",nErr);
	}

	// �����ֻ�����ɾ����¼								AEE_ADDR_CAT_NONE,AEE_ADDR_CAT_PERSONAL

 //   nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_NONE , AEE_ADDRFIELD_NONE, sAddrInfo,STRLEN(sAddrInfo) );	// AEE_ADDR_CAT_BUSINESS
	//  AEE_ADDR_CAT_NONE	AEE_ADDR_CAT_PERSONAL	AEE_ADDR_CAT_BUSINESS

//	nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_BUSINESS , AEE_ADDRFIELD_NONE, szNum,64 );	// AEE_ADDR_CAT_BUSINESS
	nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_BUSINESS , AEE_ADDRFIELD_NONE, NULL,0 );	// AEE_ADDR_CAT_BUSINESS

	if ( nErr != AEE_SUCCESS ) 	
	{ // û���ҵ� 
		return  0;
	}

	while(TRUE)
	{
		char sPhoneNo[QDK_CALL_NAME_LEN+1];
		pAddRec = IADDRBOOK_EnumNextRec(me->m_pIAddrBook);
		if (pAddRec) 
		{
			pAddrField = IADDRREC_GetField(pAddRec,1);
			if (pAddrField == NULL) 
			{
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRREC_GetField,nRecId=%d,ndx=%d,pAddrField == NULL",nRecId,1);
				continue;
			}

			MEMSET(sPhoneNo,0,sizeof(sPhoneNo));
			if (pAddrField->wDataLen > 0) 
			{	// if -- 12
				WSTRTOUTF8(pAddrField->pBuffer,WSTRLEN(pAddrField->pBuffer),(byte *)sPhoneNo,pAddrField->wDataLen );

//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumNextRec sPhoneNo 1 =%s",sPhoneNo);


				STRCAT(pAddrbook,sPhoneNo);
				STRCAT(pAddrbook,",");
/*				if (STRCMP(sPhoneNo,sAddrInfo) == 0) //˵���ҵ���
				{
					pAddRec = NULL;
					pAddrField = NULL;
					return 1;
				}
*/
			}	

		}
		else // û���ҵ�
		{
			pAddRec = NULL;
			pAddrField = NULL;
			return 0;
		}
	}
	pAddRec = NULL;
	pAddrField = NULL;

	return 1;
}


// ����˵�� ���һ������ pAddrbook ������Ż�ȡ���ĵ绰����
static int Addr_Find_Number_xinyun(crichAddr *me,newgimis *pwa,char *pAddrbook)
{
	int nErr = AEE_SUCCESS;
	newgimis *pMe = (newgimis *)pwa;
	
	AEEAddrCat nAddrCat;
	IAddrRec *pAddRec = NULL;
	int nRecId = 0;
	AEEAddrField *pAddrField = NULL;


	if (!me) return -1;
	pMe = (newgimis *)me->m_pParent;

	//////////////////               /////////////////////           ////////////////////////////
	nErr =IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook, 0);
	if (nErr == AEE_ADDRBOOK_NOCATSUPPORT)
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_ADDRBOOK_NOCATSUPPORT 0=%d",AEE_ADDRBOOK_NOCATSUPPORT);
	}
	else
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumFieldsInfoInit nErr 0 =%d",nErr);
	}

	nErr =IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook, 1);
	if (nErr == AEE_ADDRBOOK_NOCATSUPPORT)
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_ADDRBOOK_NOCATSUPPORT 1 =%d",AEE_ADDRBOOK_NOCATSUPPORT);
	}
	else
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumFieldsInfoInit nErr 1 =%d",nErr);
	}

	nErr =IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook, 2);
	if (nErr == AEE_ADDRBOOK_NOCATSUPPORT)
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_ADDRBOOK_NOCATSUPPORT 2 =%d",AEE_ADDRBOOK_NOCATSUPPORT);
	}
	else
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumFieldsInfoInit nErr 2 =%d",nErr);
	}

	//////////////////               /////////////////////           ////////////////////////////

	nErr =IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook, AEE_ADDRFIELD_NAME);
	if (nErr == AEE_ADDRBOOK_NOCATSUPPORT)
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_ADDRBOOK_NOCATSUPPORT =%d",AEE_ADDRBOOK_NOCATSUPPORT);
	}
	else
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumFieldsInfoInit nErr =%d",nErr);
	}

	// �����ֻ�����ɾ����¼								AEE_ADDR_CAT_NONE,AEE_ADDR_CAT_PERSONAL

 //   nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_NONE , AEE_ADDRFIELD_NONE, sAddrInfo,STRLEN(sAddrInfo) );	// AEE_ADDR_CAT_BUSINESS
	//  AEE_ADDR_CAT_NONE	AEE_ADDR_CAT_PERSONAL	AEE_ADDR_CAT_BUSINESS

//	nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_BUSINESS , AEE_ADDRFIELD_NONE, szNum,64 );	// AEE_ADDR_CAT_BUSINESS
	nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_NONE , AEE_ADDRFIELD_NONE, NULL,0 );	// AEE_ADDR_CAT_BUSINESS

	if ( nErr != AEE_SUCCESS ) 	
	{ // û���ҵ� 
		return  0;
	}

	while(TRUE)
	{
		char sPhoneNo[QDK_CALL_NAME_LEN+1];
		pAddRec = IADDRBOOK_EnumNextRec(me->m_pIAddrBook);
		if (pAddRec) 
		{
			pAddrField = IADDRREC_GetField(pAddRec,1);
			if (pAddrField == NULL) 
			{
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRREC_GetField,nRecId=%d,ndx=%d,pAddrField == NULL",nRecId,1);
				continue;
			}

			MEMSET(sPhoneNo,0,sizeof(sPhoneNo));
			if (pAddrField->wDataLen > 0) 
			{	// if -- 12
				WSTRTOUTF8(pAddrField->pBuffer,WSTRLEN(pAddrField->pBuffer),(byte *)sPhoneNo,pAddrField->wDataLen );

//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumNextRec sPhoneNo 1 =%s",sPhoneNo);


				STRCAT(pAddrbook,sPhoneNo);
				STRCAT(pAddrbook,",");
/*				if (STRCMP(sPhoneNo,sAddrInfo) == 0) //˵���ҵ���
				{
					pAddRec = NULL;
					pAddrField = NULL;
					return 1;
				}
*/
			}	

		}
		else // û���ҵ�
		{
			pAddRec = NULL;
			pAddrField = NULL;
			return 0;
		}
	}
	pAddRec = NULL;
	pAddrField = NULL;

	return 1;
}




static int Addr_Find_Number_01(crichAddr *me,char *sAddrInfo,newgimis *pwa)
{
	int nErr = AEE_SUCCESS;
	newgimis *pMe = (newgimis *)pwa;
	
	AEEAddrCat nAddrCat;
	IAddrRec *pAddRec = NULL;
	int nRecId = 0;
	AEEAddrField *pAddrField = NULL;

	AECHAR szNum[64];

	if (!me) return -1;
	pMe = (newgimis *)me->m_pParent;

	//////////////////               /////////////////////           ////////////////////////////
	nErr =IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook, 0);
	if (nErr == AEE_ADDRBOOK_NOCATSUPPORT)
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_ADDRBOOK_NOCATSUPPORT 0=%d",AEE_ADDRBOOK_NOCATSUPPORT);
	}
	else
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumFieldsInfoInit nErr 0 =%d",nErr);
	}

	nErr =IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook, 1);
	if (nErr == AEE_ADDRBOOK_NOCATSUPPORT)
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_ADDRBOOK_NOCATSUPPORT 1 =%d",AEE_ADDRBOOK_NOCATSUPPORT);
	}
	else
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumFieldsInfoInit nErr 1 =%d",nErr);
	}

	nErr =IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook, 2);
	if (nErr == AEE_ADDRBOOK_NOCATSUPPORT)
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_ADDRBOOK_NOCATSUPPORT 2 =%d",AEE_ADDRBOOK_NOCATSUPPORT);
	}
	else
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumFieldsInfoInit nErr 2 =%d",nErr);
	}

	//////////////////               /////////////////////           ////////////////////////////

	nErr =IADDRBOOK_EnumFieldsInfoInit(me->m_pIAddrBook, AEE_ADDRFIELD_NAME);
	if (nErr == AEE_ADDRBOOK_NOCATSUPPORT)
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_ADDRBOOK_NOCATSUPPORT =%d",AEE_ADDRBOOK_NOCATSUPPORT);
	}
	else
	{
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumFieldsInfoInit nErr =%d",nErr);
	}

	// �����ֻ�����ɾ����¼								AEE_ADDR_CAT_NONE,AEE_ADDR_CAT_PERSONAL

	STR_TO_WSTR(sAddrInfo,szNum,64);
 //   nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_NONE , AEE_ADDRFIELD_NONE, sAddrInfo,STRLEN(sAddrInfo) );	// AEE_ADDR_CAT_BUSINESS
	//  AEE_ADDR_CAT_NONE	AEE_ADDR_CAT_PERSONAL	AEE_ADDR_CAT_BUSINESS

//	nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_BUSINESS , AEE_ADDRFIELD_NONE, szNum,64 );	// AEE_ADDR_CAT_BUSINESS
	nErr = IADDRBOOK_EnumRecInit ( me->m_pIAddrBook, AEE_ADDR_CAT_BUSINESS , AEE_ADDRFIELD_NONE, NULL,0 );	// AEE_ADDR_CAT_BUSINESS

	if ( nErr != AEE_SUCCESS ) 	
	{ // û���ҵ� 
		return  0;
	}

	while(TRUE)
	{
		char sPhoneNo[QDK_CALL_NAME_LEN+1];
		pAddRec = IADDRBOOK_EnumNextRec(me->m_pIAddrBook);
		if (pAddRec) 
		{
			pAddrField = IADDRREC_GetField(pAddRec,1);
			if (pAddrField == NULL) 
			{
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRREC_GetField,nRecId=%d,ndx=%d,pAddrField == NULL",nRecId,1);
				continue;
			}

			MEMSET(sPhoneNo,0,sizeof(sPhoneNo));
			if (pAddrField->wDataLen > 0) 
			{	// if -- 12
				WSTRTOUTF8(pAddrField->pBuffer,WSTRLEN(pAddrField->pBuffer),(byte *)sPhoneNo,pAddrField->wDataLen );

//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IADDRBOOK_EnumNextRec sPhoneNo 1 =%s",sPhoneNo);

				if (STRCMP(sPhoneNo,sAddrInfo) == 0) //˵���ҵ���
				{
					pAddRec = NULL;
					pAddrField = NULL;
					return 1;
				}
			}	

		}
		else // û���ҵ�
		{
			pAddRec = NULL;
			pAddrField = NULL;
			return 0;
		}
	}
	pAddRec = NULL;
	pAddrField = NULL;

	return 1;
}


static void do_PhoneNumber_03(newgimis *pMe)
{  // COLON_SPLIT  DEFAULT_SPLIT
	char *ss_tmp = NULL;
	char *sc_Start = NULL;
	char *sc_End = NULL;
	char *sc_Start1 = NULL;
	char *sc_End1 = NULL;
	char *sc_Content = NULL;  //������ս��
	char *sc_tmpP = NULL; //����������洢�����￪ʼ��ȡ��ָ��
	char *sc_Item = NULL;  // ������ŵ������ֵ
	crichAddr *me;
	int si_Ret = 0;
	char *sc_tmp_t = NULL;
	char *sc_tmp_f = NULL; //ָ�������ҵ��ַ�����ַ
	me = pMe->pRichAddr;
	if (!me)
	{
		return;
	}

	//////  �Ȼ�ȡͨѶ¼�����еļ�¼  //////////////////////
	sc_tmp_t = (char *) MALLOC(15000);
	MEMSET(sc_tmp_t,0x00,15000);

	Addr_Find_Number_xinyun(me,pMe,sc_tmp_t);
//	uf_RecLog(pMe,"��ӡ���м�¼ ����");
//	uf_RecLog(pMe,sc_tmp_t);
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"sc_tmp_t = %d",STRLEN(sc_tmp_t));
//	uf_RecLog(pMe,"��ӡ���� ����");
	/////////////////////   END       //////////////////////
	ss_tmp = (char *)MALLOC(STRLEN(me->pstrRecU)+1);
	MEMSET(ss_tmp,0x00,STRLEN(me->pstrRecU)+1);

	sc_Content = (char *)MALLOC(STRLEN(me->pstrRecU)+1);
	MEMSET(sc_Content,0x00,STRLEN(me->pstrRecU)+1);

	pMe->sc_sub = (char *)MALLOC(20);
	MEMSET(pMe->sc_sub,0x00,20);

	sc_Item = (char *)MALLOC(128);
	MEMSET(sc_Item,0x00,128);

	MEMCPY(ss_tmp,me->pstrRecU,STRLEN(me->pstrRecU));


	sc_Start = STRSTR(ss_tmp,DEFAULT_SPLIT);
	if (!sc_Start) // ˵��ֻ��һ��
	{
		sc_Start1 = STRSTR(ss_tmp,COLON_SPLIT);
		if (sc_Start1)
		{
			// �ҵ��ֻ�����
			MEMCPY(pMe->sc_sub,ss_tmp,sc_Start1 - ss_tmp);
//			uf_RecLog(pMe,pMe->sc_sub);
//			si_Ret = Addr_Find_Number_01(me, pMe->sc_sub ,pMe);
			sc_tmp_f = STRSTR(sc_tmp_t,pMe->sc_sub);
			if (!sc_tmp_f) // ˵������绰���뻹û����� ��Ҫ���
			{
				STRCAT(sc_Content,me->pstrRecU);
				MEMSET(me->pstrRecU,0x00,STRLEN(me->pstrRecU));
				MEMCPY(me->pstrRecU,sc_Content,STRLEN(sc_Content));
//				uf_RecLog(pMe,me->pstrRecU);

			}
			else
			{
				MEMSET(me->pstrRecU,0x00,STRLEN(me->pstrRecU)); //˵���Ѿ�����ˣ��������
			}
		}

		FREEIF(ss_tmp);
		FREEIF(sc_Content);
		FREEIF(pMe->sc_sub);
		FREEIF(sc_Item);
		FREEIF(sc_tmp_t);
		return;
	}

	sc_tmpP = ss_tmp;
	while(sc_Start) 
	{
//		uf_RecLog(pMe,sc_tmpP);
		sc_Start1 = STRSTR(sc_tmpP,COLON_SPLIT);
		if (sc_Start1)
		{
			// �ҵ��ֻ�����
			MEMSET(pMe->sc_sub,0x00,20);
			MEMCPY(pMe->sc_sub,sc_tmpP,sc_Start1 - sc_tmpP);
//			uf_RecLog(pMe,pMe->sc_sub);
			sc_tmp_f = STRSTR(sc_tmp_t,pMe->sc_sub);
			if (!sc_tmp_f) // ˵������绰���뻹û����� ��Ҫ���
//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"si_Ret = %d",si_Ret);
//			if (!si_Ret) // ˵������绰���뻹û����� ��Ҫ���
			{
				MEMSET(sc_Item,0x00,128);
				MEMCPY(sc_Item,sc_tmpP,sc_Start - sc_tmpP);
				if (STRLEN(sc_Content) == 0)
				{
					STRCAT(sc_Content,sc_Item);
				}
				else
				{
					STRCAT(sc_Content,DEFAULT_SPLIT);
					STRCAT(sc_Content,sc_Item);					
				}

//				uf_RecLog(pMe,sc_Content);
			}
		}

		sc_tmpP = sc_Start + 1;
		sc_Start = STRSTR(sc_tmpP,DEFAULT_SPLIT);

	} 

	// ���������һ��
//	uf_RecLog(pMe,sc_Content);
	sc_Start1 = STRSTR(sc_tmpP,COLON_SPLIT);
	if (sc_Start1)
	{
		// �ҵ��ֻ�����
		MEMSET(pMe->sc_sub,0x00,20);
		MEMCPY(pMe->sc_sub,sc_tmpP,sc_Start1 - sc_tmpP);
//		uf_RecLog(pMe,pMe->sc_sub);
		sc_tmp_f = STRSTR(sc_tmp_t,pMe->sc_sub);
		if (!sc_tmp_f) // ˵������绰���뻹û����� ��Ҫ���
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"si_Ret 1 = %d",si_Ret);
//		if (!si_Ret) // ˵������绰���뻹û����� ��Ҫ���
		{
			MEMSET(sc_Item,0x00,128);
			STRCPY(sc_Item,sc_tmpP);
			if (STRLEN(sc_Content) == 0)
			{
				STRCAT(sc_Content,sc_Item);
			}
			else
			{
				STRCAT(sc_Content,DEFAULT_SPLIT);
				STRCAT(sc_Content,sc_Item);					
			}
		}
	}

	MEMSET(me->pstrRecU,0x00,STRLEN(me->pstrRecU));
	MEMCPY(me->pstrRecU,sc_Content,STRLEN(sc_Content));
//	uf_RecLog(pMe,me->pstrRecU);

	FREEIF(ss_tmp);
	FREEIF(sc_Content);
	FREEIF(pMe->sc_sub);
	FREEIF(sc_Item);
	FREEIF(sc_tmp_t);
	return;
	
}



static void do_PhoneNumber(newgimis *pMe)
{  // COLON_SPLIT  DEFAULT_SPLIT
	char *ss_tmp = NULL;
	char *sc_Start = NULL;
	char *sc_End = NULL;
	char *sc_Start1 = NULL;
	char *sc_End1 = NULL;
	char *sc_Content = NULL;  //������ս��
	char *sc_tmpP = NULL; //����������洢�����￪ʼ��ȡ��ָ��
	char *sc_Item = NULL;  // ������ŵ������ֵ
	crichAddr *me;
	int si_Ret = 0;
	me = pMe->pRichAddr;
	if (!me)
	{
		return;
	}
	ss_tmp = (char *)MALLOC(STRLEN(me->pstrRecU)+1);
	MEMSET(ss_tmp,0x00,STRLEN(me->pstrRecU)+1);

	sc_Content = (char *)MALLOC(STRLEN(me->pstrRecU)+1);
	MEMSET(sc_Content,0x00,STRLEN(me->pstrRecU)+1);

	pMe->sc_sub = (char *)MALLOC(20);
	MEMSET(pMe->sc_sub,0x00,20);

	sc_Item = (char *)MALLOC(128);
	MEMSET(sc_Item,0x00,128);

	MEMCPY(ss_tmp,me->pstrRecU,STRLEN(me->pstrRecU));


	sc_Start = STRSTR(ss_tmp,DEFAULT_SPLIT);
	if (!sc_Start) // ˵��ֻ��һ��
	{
		sc_Start1 = STRSTR(ss_tmp,COLON_SPLIT);
		if (sc_Start1)
		{
			// �ҵ��ֻ�����
			MEMCPY(pMe->sc_sub,ss_tmp,sc_Start1 - ss_tmp);
			uf_RecLog(pMe,pMe->sc_sub);
			si_Ret = Addr_Find_Number(me, pMe->sc_sub ,pMe);
			if (!si_Ret) // ˵������绰���뻹û����� ��Ҫ���
			{
				STRCAT(sc_Content,me->pstrRecU);
				MEMSET(me->pstrRecU,0x00,STRLEN(me->pstrRecU));
				MEMCPY(me->pstrRecU,sc_Content,STRLEN(sc_Content));
				uf_RecLog(pMe,me->pstrRecU);

			}
			else
			{
				MEMSET(me->pstrRecU,0x00,STRLEN(me->pstrRecU)); //˵���Ѿ�����ˣ��������
			}
		}

		FREEIF(ss_tmp);
		FREEIF(sc_Content);
		FREEIF(pMe->sc_sub);
		FREEIF(sc_Item);
		return;
	}

	sc_tmpP = ss_tmp;
	while(sc_Start) 
	{
		sc_Start1 = STRSTR(sc_tmpP,COLON_SPLIT);
		if (sc_Start1)
		{
			// �ҵ��ֻ�����
			MEMSET(pMe->sc_sub,0x00,20);
			MEMCPY(pMe->sc_sub,sc_tmpP,sc_Start1 - sc_tmpP);
			uf_RecLog(pMe,pMe->sc_sub);
			si_Ret = Addr_Find_Number(me, pMe->sc_sub ,pMe);
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"si_Ret = %d",si_Ret);
			if (!si_Ret) // ˵������绰���뻹û����� ��Ҫ���
			{
				MEMSET(sc_Item,0x00,128);
				MEMCPY(sc_Item,sc_tmpP,sc_Start - sc_tmpP);
				if (STRLEN(sc_Content) == 0)
				{
					STRCAT(sc_Content,sc_Item);
				}
				else
				{
					STRCAT(sc_Content,DEFAULT_SPLIT);
					STRCAT(sc_Content,sc_Item);					
				}

				uf_RecLog(pMe,sc_Content);
			}
		}

		sc_tmpP = sc_Start + 1;
		sc_Start = STRSTR(sc_tmpP,DEFAULT_SPLIT);

	} 

	// ���������һ��
	uf_RecLog(pMe,sc_Content);
	sc_Start1 = STRSTR(sc_tmpP,COLON_SPLIT);
	if (sc_Start1)
	{
		// �ҵ��ֻ�����
		MEMSET(pMe->sc_sub,0x00,20);
		MEMCPY(pMe->sc_sub,sc_tmpP,sc_Start1 - sc_tmpP);
		uf_RecLog(pMe,pMe->sc_sub);
		si_Ret = Addr_Find_Number(me, pMe->sc_sub,pMe );
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"si_Ret 1 = %d",si_Ret);
		if (!si_Ret) // ˵������绰���뻹û����� ��Ҫ���
		{
			MEMSET(sc_Item,0x00,128);
			STRCPY(sc_Item,sc_tmpP);
			if (STRLEN(sc_Content) == 0)
			{
				STRCAT(sc_Content,sc_Item);
			}
			else
			{
				STRCAT(sc_Content,DEFAULT_SPLIT);
				STRCAT(sc_Content,sc_Item);					
			}
		}
	}

	MEMSET(me->pstrRecU,0x00,STRLEN(me->pstrRecU));
	MEMCPY(me->pstrRecU,sc_Content,STRLEN(sc_Content));
	uf_RecLog(pMe,me->pstrRecU);



	FREEIF(ss_tmp);
	FREEIF(sc_Content);
	FREEIF(pMe->sc_sub);
	FREEIF(sc_Item);
	return;
	
}


static void do_PhoneNumber_01(newgimis *pMe)
{  // COLON_SPLIT  DEFAULT_SPLIT
	char *ss_tmp = NULL;
	char *sc_Start = NULL;
	char *sc_End = NULL;
	char *sc_Start1 = NULL;
	char *sc_End1 = NULL;
	char *sc_Content = NULL;  //������ս��
	char *sc_tmpP = NULL; //����������洢�����￪ʼ��ȡ��ָ��
	char *sc_Item = NULL;  // ������ŵ������ֵ
	crichAddr *me;
	int si_Ret = 0;
	char *sc_tmp_t = NULL;
	char *sc_tmp_f = NULL; //ָ�������ҵ��ַ�����ַ
	me = pMe->pRichAddr;
	if (!me)
	{
		return;
	}

	//////  �Ȼ�ȡͨѶ¼�����еļ�¼  //////////////////////
	sc_tmp_t = (char *) MALLOC(15000);
	MEMSET(sc_tmp_t,0x00,15000);

	Addr_Find_Number_haixin(me,pMe,sc_tmp_t);
//	uf_RecLog(pMe,"��ӡ���м�¼");
//	uf_RecLog(pMe,sc_tmp_t);
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"sc_tmp_t = %d",STRLEN(sc_tmp_t));
//	uf_RecLog(pMe,"��ӡ����");
	/////////////////////   END       //////////////////////
	ss_tmp = (char *)MALLOC(STRLEN(me->pstrRecU)+1);
	MEMSET(ss_tmp,0x00,STRLEN(me->pstrRecU)+1);

	sc_Content = (char *)MALLOC(STRLEN(me->pstrRecU)+1);
	MEMSET(sc_Content,0x00,STRLEN(me->pstrRecU)+1);

	pMe->sc_sub = (char *)MALLOC(20);
	MEMSET(pMe->sc_sub,0x00,20);

	sc_Item = (char *)MALLOC(128);
	MEMSET(sc_Item,0x00,128);

	MEMCPY(ss_tmp,me->pstrRecU,STRLEN(me->pstrRecU));


	sc_Start = STRSTR(ss_tmp,DEFAULT_SPLIT);
	if (!sc_Start) // ˵��ֻ��һ��
	{
		sc_Start1 = STRSTR(ss_tmp,COLON_SPLIT);
		if (sc_Start1)
		{
			// �ҵ��ֻ�����
			MEMCPY(pMe->sc_sub,ss_tmp,sc_Start1 - ss_tmp);
//			uf_RecLog(pMe,pMe->sc_sub);
//			si_Ret = Addr_Find_Number_01(me, pMe->sc_sub ,pMe);
			sc_tmp_f = STRSTR(sc_tmp_t,pMe->sc_sub);
			if (!sc_tmp_f) // ˵������绰���뻹û����� ��Ҫ���
			{
				STRCAT(sc_Content,me->pstrRecU);
				MEMSET(me->pstrRecU,0x00,STRLEN(me->pstrRecU));
				MEMCPY(me->pstrRecU,sc_Content,STRLEN(sc_Content));
//				uf_RecLog(pMe,me->pstrRecU);

			}
			else
			{
				MEMSET(me->pstrRecU,0x00,STRLEN(me->pstrRecU)); //˵���Ѿ�����ˣ��������
			}
		}

		FREEIF(ss_tmp);
		FREEIF(sc_Content);
		FREEIF(pMe->sc_sub);
		FREEIF(sc_Item);
		FREEIF(sc_tmp_t);
		return;
	}

	sc_tmpP = ss_tmp;
	while(sc_Start) 
	{
		//13348352536:13348352536:ep: ,13348552026:13348552026:����: ,13348552067:13348552067:������: ,13348552083:13348552083:����1:
//		uf_RecLog(pMe,sc_tmpP);
		sc_Start1 = STRSTR(sc_tmpP,COLON_SPLIT);
		if (sc_Start1)
		{
			// �ҵ��ֻ�����
			MEMSET(pMe->sc_sub,0x00,20);
			MEMCPY(pMe->sc_sub,sc_tmpP,sc_Start1 - sc_tmpP);
//			uf_RecLog(pMe,pMe->sc_sub);
			sc_tmp_f = STRSTR(sc_tmp_t,pMe->sc_sub);
			if (!sc_tmp_f) // ˵������绰���뻹û����� ��Ҫ���
//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"si_Ret = %d",si_Ret);
//			if (!si_Ret) // ˵������绰���뻹û����� ��Ҫ���
			{
				MEMSET(sc_Item,0x00,128);
				MEMCPY(sc_Item,sc_tmpP,sc_Start - sc_tmpP);
				if (STRLEN(sc_Content) == 0)
				{
					STRCAT(sc_Content,sc_Item);
				}
				else
				{
					STRCAT(sc_Content,DEFAULT_SPLIT);
					STRCAT(sc_Content,sc_Item);					
				}

//				uf_RecLog(pMe,sc_Content);
			}
		}

		sc_tmpP = sc_Start + 1;
		sc_Start = STRSTR(sc_tmpP,DEFAULT_SPLIT);

	} 

	// ���������һ��
//	uf_RecLog(pMe,sc_Content);
	sc_Start1 = STRSTR(sc_tmpP,COLON_SPLIT);
	if (sc_Start1)
	{
		// �ҵ��ֻ�����
		MEMSET(pMe->sc_sub,0x00,20);
		MEMCPY(pMe->sc_sub,sc_tmpP,sc_Start1 - sc_tmpP);
//		uf_RecLog(pMe,pMe->sc_sub);
		sc_tmp_f = STRSTR(sc_tmp_t,pMe->sc_sub);
		if (!sc_tmp_f) // ˵������绰���뻹û����� ��Ҫ���
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"si_Ret 1 = %d",si_Ret);
//		if (!si_Ret) // ˵������绰���뻹û����� ��Ҫ���
		{
			MEMSET(sc_Item,0x00,128);
			STRCPY(sc_Item,sc_tmpP);
			if (STRLEN(sc_Content) == 0)
			{
				STRCAT(sc_Content,sc_Item);
			}
			else
			{
				STRCAT(sc_Content,DEFAULT_SPLIT);
				STRCAT(sc_Content,sc_Item);					
			}
		}
	}

	MEMSET(me->pstrRecU,0x00,STRLEN(me->pstrRecU));
	MEMCPY(me->pstrRecU,sc_Content,STRLEN(sc_Content));
//	uf_RecLog(pMe,me->pstrRecU);

	FREEIF(ss_tmp);
	FREEIF(sc_Content);
	FREEIF(pMe->sc_sub);
	FREEIF(sc_Item);
	FREEIF(sc_tmp_t);
	return;
	
}