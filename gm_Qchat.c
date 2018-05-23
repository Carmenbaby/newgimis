/*===============================================================================
 �����й��ϴ��ļ�
=============================================================================== */

#include "newgimis.h"
#include "gm_Qchat.h"

#include "AEETelephone.h" 
#include "../../../brewsdk/inc/AEETelDef.h"				// H:\Qualcomm\BREW315\inc\AEETelDef.h
#include "qchatDataType.h"

#include "gmTools.h"

/* -----------------------------------------------------------------
micro define
-------------------------------------------------------------------*/
// ������������ĳһλ�õ�ֵΪ0
/*-------------------------------------------------------------------
Static function prototypes
-------------------------------------------------------------------*/
// ����ֻ��Ƿ�ΪQchat�ֻ�, �����QChat�ֻ�����ʵ�����ṹ
static boolean richptt_QchatInstance(newgimis *pMe);
// ֹͣQChatӦ��
static void richptt_StopApp(newgimis *pMe);
// ��ʼ��QDK�Ľӿ�
static boolean richptt_InitQDKInterface(crichptt *me);
// ע��QDK�ӿ�
static void richptt_UnRegisterIQDKInterface(crichptt *me);

static boolean richptt_InitAppData(crichptt* me);
static void richptt_FreeAppData(crichptt* me);
// ��ȡQchat�ֻ�����
static boolean richptt_GetPhoneNo(crichptt *me);  

// ����QDK�¼�
static boolean richptt_QDKNotifier(crichptt *me, AEEEvent eCode, uint16 wParam, uint32 dwParam);
// ����NMASK_QDK_CALL��ص��¼� 
static boolean richptt_EventQDKCall(crichptt *me, AEEEvent eCode, uint16 wParam, uint32 dwParam);
// ����NMASK_QDK_MEMBERSHIP��ص��¼� 
static boolean richptt_EventQDKMembership(crichptt *me, AEEEvent eCode, uint16 wParam, uint32 dwParam);

// ����һ������
static void richptt_PttCall(crichptt *me);
// �ֻ�������01��Ԥ�������
static void richptt_PttCall_01(crichptt *me); 
//  ͨ��Ԥ�������
static void richptt_PttCall_xx(crichptt *me);
// ����һ�����е��ֻ�����
static void richPtt_SetPttCallIInfo(crichptt *me);
// �ͷ�һ�����е���Ϣ
static void richptt_FreePttCallNo(crichptt *me);

// ͨ���ص������������
static void richptt_cbStartCall(crichptt *me);
// ����Ԥ�������
static void richptt_GroupCall(crichptt *me,int iLoc);
// �ֻ�������01��Ԥ�������
static void richptt_GroupCall_01(crichptt *me,int iLoc);
//  ͨ��Ԥ�������
static void richptt_GroupCall_xx(crichptt *me,int iLoc);
// �ͷŻ���Ȩ
static void richptt_ReleaseFloor(crichptt *me);
// ���뻰��Ȩ
static void richptt_RequestFloor(crichptt *me);
// ��������
static void richptt_EndCall(crichptt *me);
// ����Ԥ������ĺ���״̬,�����ǰ�Խ������ȼ���������ĶԽ������˳������ȼ�����������ȼ��ĶԽ�
static void richptt_SetReqGroupStatus(crichptt *me,char *name);
// ����confId����Ԥ������ĺ���״̬,�����ǰ�Խ������ȼ���������ĶԽ������˳������ȼ�����������ȼ��ĶԽ�
static boolean richptt_SetReqGroupStatusWithConfID(crichptt *me,IQDKConfIdType confId,IQDK_RejectReasonType rejectReason);

// �ͷ��Զ��������Ϣ
static void richptt_FreeGroup(crichptt *me);
// ���ɴ����е���Ϣ�б�
static IQDKAddressData * uf_CreateAddressData(newgimis *pMe,const char **urlList, int count,AECHAR *sDomain,IQDKAddressType iCallType);
// ��ȡQchat����
static void richptt_ReadQchatSetting(crichptt *me);
// д��Qchat����
static void richptt_WriteQchatSetting(crichptt *me);
// ��������е�Ԥ������������
static void richptt_WaitToCall(crichptt *me);

//   ����ע��İ���
static boolean richptt_NotifyShellKey(crichptt *me, AEEEvent eCode, uint16 wParam, uint32 dwParam);

// ��ȡQchat�ֻ��������Ϣ
static void richptt_GetGroupInfo(crichptt *me);

// �������ļ���ȡԤ��������Ϣ
static void richptt_ReadPrdGroupSetting(crichptt *me);

// Add by zjie 2013-03-15
// �ͷźڰ������ṹ�б�
static void richPtt_FreeCallRests(crichptt *me);
// �����ֻ��ڰ�����
static void richPtt_UpdCallRest(crichptt* me);
// Add End

/*-------------------------------------------------------------------
function prototypes
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
function implement
-------------------------------------------------------------------*/
// =======================================================================
// =====================================================================================
// ����ֻ��Ƿ�ΪQchat�ֻ�, �����QChat�ֻ�����ʵ�����ṹ
// ������
//	 TRUE :  �ɹ�
//   FALSE :  ʧ�� , ����Qchat�ֻ���Qchat������
// =====================================================================================
static boolean richptt_QchatInstance(newgimis *pMe)
{
    IQDK30Provision *pIQDKProvision = NULL;
    int nErr = EFAILED;
	int nResult;
	crichptt *me;
    
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"QChatManager_InitProvision in");
	
	if ( pMe->pRichptt != NULL)
		return TRUE;

	me = (crichptt *)MALLOC(sizeof(crichptt));
	MEMSET(me,0,sizeof(crichptt));
	
    nResult = ISHELL_CreateInstance(pMe->a.m_pIShell,  AEECLSID_IQDKPROVISION, (void **)&me->pIQDKProvision);
    if ((SUCCESS != nResult) || (NULL == me->pIQDKProvision))   {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"create IQDKProvision failed: %u", nResult);
		FREEIF(me);
        return FALSE;
    }
	
    nErr = IQDK30PROVISION_QueryInterface(me->pIQDKProvision, AEEIID_IQDK30PROVISION, (void**)&pIQDKProvision);
    if (SUCCESS == nErr)
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK30PROVISION_QueryInterface success");
    else  {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDKPROVISION_QueryInterface failed: %u", nErr);	
    }
	
    IQDK30PROVISION_Release(me->pIQDKProvision);
	if (SUCCESS != nErr) {
		FREEIF(me);
		return FALSE;
	}
	
    nErr = IQDK30PROVISION_Init(pIQDKProvision,  pMe->a.clsID);
    if (SUCCESS == nErr) {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK30PROVISION_Init success");
	}
    else {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK30PROVISION_Init failed: %u", nErr);
		IQDK30PROVISION_Release(pIQDKProvision);
		FREEIF(me);
        return FALSE;
    }
	
	me->m_pParent = pMe;
	pMe->pRichptt = (void *)me;
	richptt_InitAppData(me);
	me->pIQDKProvision = pIQDKProvision;
	return TRUE;
}



// =================================================================
// ��ȡQchat�ֻ�����, ����
static boolean richptt_GetPhoneNo(crichptt *me)  
{
	newgimis *pMe = me->m_pParent;

	IQDK30_ProvListEntry addrList = {0};
	IQDKAddressData *paddresses = NULL;
	AECHAR szAddrDft[QDK_CALL_NAME_LEN+1] = L"00000000000";
	int16 nListSize = 0;
	boolean bRet = FALSE;

	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_GetPhoneNo in");
	
	if (!me->pIQDKProvision) {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"No qdkprovision instance.");
		return FALSE;
	}

	IQDK30PROVISION_GetAddressListSize(me->pIQDKProvision,IQDK30_PROV_ADDRESSES_ALTERNATE_USER, &nListSize);

	// �ֻ��ź͵���������
	if (nListSize > 0)
	{	// nListSize
		paddresses = (IQDKAddressData *)MALLOC(sizeof(IQDKAddressData) * nListSize);
		MEMSET(paddresses,0,sizeof(IQDKAddressData) * nListSize);
		addrList.addressListEntry.addresses = paddresses;      //&addresses;

		IQDK30PROVISION_GetProvEntry_AddressList(me->pIQDKProvision, IQDK30_PROV_ADDRESSES_ALTERNATE_USER,&addrList);
		
		if ( (addrList.addressListEntry.count > 0) && (addrList.addressListEntry.addresses) )
		{
			int i;
			
			if (0 != WSTRCMP((const AECHAR*)szAddrDft, (const AECHAR *)paddresses[0].name))
			{
				char sTmp[DOMAIN_LEN*2+1];
				MEMSET(sTmp,0,sizeof(sTmp));
				WSTRTOSTR( paddresses[0].name,me->MyMobileNo,sizeof(me->MyMobileNo) );   
				STRCPY(pMe->MobileNoDJ,me->MyMobileNo);				// Add 2013-09-23
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "�Խ����룺MobileNoDJ=%s", me->MyMobileNo); // ��ȡ����
				bRet = TRUE;
			}
			
			for(i=0;i<addrList.addressListEntry.count;i++)
			{
				FREEIF((paddresses+i)->name);
				FREEIF(paddresses[i].domain);
			}
		}
		FREEIF(paddresses);
	}	// nListSize
	

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"QChatUtils_IsAuthened bRet=%d", bRet); 
	
	return bRet;
}

// ֹͣQChatӦ��
static void richptt_StopApp(newgimis *pMe)
{
	crichptt* me = pMe->pRichptt;

	richptt_FreeAppData(me);
	me->m_pParent = 0;
	FREEIF(pMe->pRichptt);

}

/*===========================================================================
��Ϣ�ӿ�
===========================================================================*/
boolean richptt_HandleEvent(newgimis *pMe,  AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  
	crichptt* me;

	if (pMe->pRichptt != NULL)
		me = (crichptt *)pMe->pRichptt;
	 else if (eCode != EVT_APP_START)
		return FALSE;
	
    switch (eCode) 
	{
        // App is told it is starting up
        case EVT_APP_START:                        
			if (pMe->pRichptt == NULL) {
				if (richptt_QchatInstance(pMe) == FALSE)
					return FALSE;
			}

			me = (crichptt *)pMe->pRichptt;
		
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_QchatInstance out"); 

			if (richptt_GetPhoneNo(me)) {
				// ע����QDK���¼�
				richptt_InitQDKInterface(me);
				richptt_ReadPrdGroupSetting(me);		//	���ڴ˺�������	richptt_GetGroupInfo(me);
				richPtt_SetPttCallIInfo(me);	// ����һ�����еĺ���
					
				return TRUE;
			}
			else {
				me->cStatus += 1;
				if (me->cStatus == 7) {	// ʼ�յò����ֻ����룬Qchat������
			        ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"get mobile no error");
					richptt_StopApp(pMe);
				}
				return FALSE;
			}

            break;
        case EVT_APP_STOP:
			DBGPRINTF("zjie-richptt_EVT_APP_STOP");
			richptt_StopApp(pMe);
      		break;

        case EVT_APP_SUSPEND:

      		break;


        case EVT_APP_RESUME:

      		return(TRUE);

		case EVT_APP_NO_SLEEP:
			break;
        case EVT_KEY:

			break;
		case EVT_KEY_PRESS:

			break;
		case EVT_KEY_RELEASE:
			break;
		case EVT_NOTIFY:
			{	
				AEENotify* pNotify = (AEENotify*) dwParam;

//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_HandleEvent in,EVT_NOTIFY");

				if (pNotify && (pNotify->cls == AEECLSID_IQDKNOTIFIER)) {	// _IQDKNOTIFIER
					if ( richptt_QDKNotifier(me, eCode, wParam, dwParam ) )
						return TRUE;
					
				} // _IQDKNOTIFIER

				if (pNotify && (pNotify->cls == AEECLSID_SHELL)) // event sender
				{	// -- shell
					// ����������
					if ((pNotify->dwMask & NMASK_SHELL_KEY ) == NMASK_SHELL_KEY )  
					{	// if -- KEY
						if ( richptt_NotifyShellKey( me, eCode, wParam, dwParam ) )
							return TRUE;
					}	// if -- KEY
				}   // -- shell
				
			}
			break;
		
 		case EVT_CALLAGAIN_EVENT:
			if (me->nMyCallCnt < 2) {		// 0��1��2 �ܹ�3��
				CALLBACK_Cancel( &me->cbGen );
				CALLBACK_Init( &me->cbGen, richptt_cbStartCall, me );
				ISHELL_SetTimerEx(me->m_pParent->a.m_pIShell, (RICH_CALL_INTERVAL-400), &me->cbGen );
				me->nMyCallCnt++;
			}
			else
				 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"call agein times=%d",me->nMyCallCnt);
 			break;
		case EVT_SETPTTCALL_EVENT:				// ����һ�����еĺ���
			richPtt_SetPttCallIInfo(me);
			break;
		case EVT_PTTCALL_EVENT:
			richptt_PttCall(me);
			break;
		case EVT_UPDREST_EVENT:
			 richPtt_UpdCallRest(me);
			 break;
 		case EVT_ENDCALL_EVENT:
 			richptt_EndCall(me);
 			break;
		case EVT_RELEASEFLOOR_EVENT:
			richptt_ReleaseFloor(me);
			break;
		case EVT_REQUESTFLOOR_EVENT:
			richptt_RequestFloor(me);
			break;
        // If nothing fits up to this point then we'll just break out
        default:
            break;
   }


   return FALSE;
}

// ====================================================================
// �ͷźڰ������ṹ�б�
static void richPtt_FreeCallRests(crichptt *me)
{
	newgimis *pMe;
	
	if (!me) return;
	pMe = (newgimis *)me->m_pParent;
	
	if (me->pCallRestList != NULL) {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richPtt_FreeCallRests in, Call Restriction count=%d",me->pCallRestList->count);
		if (me->pCallRestList->count >0)
		{
			FREEIF(me->pCallRestList->CallRestInfo);
			me->pCallRestList->count = 0;
		}
	}
	
}

// =================================================================
// ���������صĺڰ�������Ϣ���뵽�ڰ������ṹ
void richPtt_SetCallRestInfo(crichptt *me,char *CallRestInfo) 
{
	newgimis *pMe;
				//	IQDK_ErrorType nErr;
	int iLocNextRec, iLenRec;
	int iLocNextFld, iLenFld;
	int ndx;			// �ֶε�����
	
	if (!me) return;
	pMe = (newgimis *)me->m_pParent;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richPtt_SetCallRestInfo in,CallRestInfo=%s",CallRestInfo);
		
	richPtt_FreeCallRests(me);
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richPtt_SetCallRestInfo  1");
	iLenRec = 0;
	iLocNextRec = -1;
	while(TRUE)	
	{		// record

		char *pRecord = NULL;

		if (me->pCallRestList->count >= 80)		break;

		// ȡ����¼
		iLocNextRec += iLenRec + 1;	// ��','�ĳ���
		if (iLocNextRec >= (int)STRLEN(CallRestInfo) )
			break;
		iLenRec = ud_DistToSplit( CallRestInfo+iLocNextRec, DEFAULT_SPLIT );
		if (iLenRec <= 0) break;
			
		pRecord = CallRestInfo+iLocNextRec;		// ��¼�Ŀ�ʼλ��
		me->pCallRestList->count += 1;
			
		if (me->pCallRestList->count == 1)
			me->pCallRestList->CallRestInfo = (ICallRestriction *)MALLOC(sizeof(ICallRestriction));
		else
			me->pCallRestList->CallRestInfo = (ICallRestriction *)REALLOC( me->pCallRestList->CallRestInfo, me->pCallRestList->count*sizeof(ICallRestriction) );
		MEMSET(me->pCallRestList->CallRestInfo+(me->pCallRestList->count-1),0,sizeof(ICallRestriction));
			
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
			//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"in richPtt_SetCallRestInfo,ndx=%d,iLocNextFld=%d,iLenFld=%d",ndx,iLocNextFld,iLenFld);
			switch (ndx)
			{
				case 0:			// ���ƺ��е�ID
					MEMCPY(sTmp,pRecord+iLocNextFld, iLenFld);
					STRCPY(me->pCallRestList->CallRestInfo[me->pCallRestList->count-1].sName,sTmp);
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Rest name[%d]=%s",me->pCallRestList->count-1,me->pCallRestList->CallRestInfo[me->pCallRestList->count-1].sName);

					break;
				case 1:			// ��������
					MEMCPY(sTmp,pRecord+iLocNextFld, iLenFld);
					me->pCallRestList->CallRestInfo[me->pCallRestList->count-1].service = ATOI(sTmp);
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richPtt_SetCallRestInfo,Rest service[%d]=%d",me->pCallRestList->count-1,me->pCallRestList->CallRestInfo[me->pCallRestList->count-1].service);

					break;
				case 2:		  // ���Ʒ���	
					MEMCPY(sTmp,pRecord+iLocNextFld, iLenFld);
					me->pCallRestList->CallRestInfo[me->pCallRestList->count-1].direction = ATOI(sTmp);
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richPtt_SetCallRestInfo,Rest direction[%d]=%d",me->pCallRestList->count-1,me->pCallRestList->CallRestInfo[me->pCallRestList->count-1].direction);

					break;
				case 3:			// ��������
					MEMCPY(sTmp,pRecord+iLocNextFld, iLenFld);
					me->pCallRestList->CallRestInfo[me->pCallRestList->count-1].type = ATOI(sTmp);
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richPtt_SetCallRestInfo,Rest type[%d]=%d",me->pCallRestList->count-1,me->pCallRestList->CallRestInfo[me->pCallRestList->count-1].type);
					break;
				}
			}	// field
			
		}	// record
		
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richPtt_SetCallRestInfo before ISHELL_PostEvent EVT_UPDREST_EVENT");

   ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_UPDREST_EVENT,(uint16)0, 0);

		return;
}
// ======================================================
// �����ֻ��ڰ�����
static void richPtt_UpdCallRest(crichptt* me)
{
	newgimis *pMe;	
	IQDK_ErrorType	nErr;	
	int			idx = 0;
	uint32          nCount = 0;
	
	if (!me) return;
	pMe = (newgimis *)me->m_pParent;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richPtt_UpdCallRest in");

    nErr = IQDK30CALLRESTRICTION_GetCount(me->pIQDK30CallRestriction,&nCount);
	if (nErr != SUCCESS ) {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richPtt_UpdCallRest,IQDKCALLRESTRICTION_GetCount return=%d",nErr);
		return;
	}
	else
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richPtt_UpdCallRest,IQDKCALLRESTRICTION Count=%d",nCount);

// 				
// 				for(idx=0;idx<nCount;idx++)
// 				{
// 					IQDK30_CallRestrictionEntry entry;
// 
// 					MEMSET(&entry,0,sizeof(IQDK30_CallRestrictionEntry));
// 
// 					nErr = IQDK30CALLRESTRICTION_GetByIndex(me->pIQDK30CallRestriction,&entry,idx);
// 					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richPtt_UpdCallRest,IQDK30CALLRESTRICTION_GetByIndex return=%d",nErr);
// 
// 				//	EBADPARM
// 					if (nErr == SUCCESS) {
// 						char sTmp[100] = {0};
// 						char sDom[100] = {0};
// 						int nWStr= WSTRLEN(entry.token.name);
// 						WSTRTOUTF8(entry.token.name,nWStr,(byte *)sTmp,sizeof(sTmp));
// 						nWStr= WSTRLEN(entry.token.domain);
// 						WSTRTOUTF8(entry.token.domain,nWStr,(byte *)sDom,sizeof(sTmp));
// 						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"entry->level=%d,entry->type=%d,entry->direction=%d,services=%d,asteriskIsLiteral=%d,name=%s,domain=%s,addrType=%d",
// 							   entry.level,entry.type,entry.direction,entry.services,entry.asteriskIsLiteral,sTmp,sDom,entry.token.addrType);
// 			/* 
// 			���   services=31 �ֻ��˼��룬services=127 ͨ����ͨƽ̨����
// 			140[2013-03-25 14:27:38]:entry->level=1,entry->type=2,entry->direction=2,services=31,asteriskIsLiteral=0,name=*,domain=group.ptt.189.cn,addrType=2
// 			// ����
// 			142[2013-03-25 14:27:38]:entry->level=1,entry->type=2,entry->direction=2,services=31,asteriskIsLiteral=0,name=*,domain=ptt.189.cn,addrType=1
// 			*/
// 					}
// 
// 				}
	
//	return;
// ==================

//	for(idx=0; idx<nCount;idx++) {
		for(idx=(nCount-1); idx>=0; idx--) {
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richPtt_UpdCallRest, nCount=%d,idx=%d",nCount,idx);
			nErr = IQDK30CALLRESTRICTION_RemoveByIndex(me->pIQDK30CallRestriction,(uint32)idx);

			if (nErr != SUCCESS) {
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richPtt_UpdCallRest,IQDKCALLRESTRICTION_RemoveByIndex return=%d,nCount=%d,idx=%d",nErr,nCount,idx);
				return;
			}
		}


    for(idx=0;idx<me->pCallRestList->count;idx++)
	{
		IQDK30_CallRestrictionEntry *entry = NULL;
		int nsUsrLen = 0, nDomainLen = 0;		// ����name�ĳ��ȣ� domain �ĳ���	

		entry = (IQDK30_CallRestrictionEntry *)MALLOC(sizeof(IQDK30_CallRestrictionEntry));

		nsUsrLen = STRLEN(me->pCallRestList->CallRestInfo[idx].sName) + 1;
		entry->token.name = (AECHAR *)MALLOC(nsUsrLen*sizeof(AECHAR));
		STRTOWSTR(me->pCallRestList->CallRestInfo[idx].sName,entry->token.name,nsUsrLen*sizeof(AECHAR));
		{
			char sTmp[100] = {0};
			int nWStr= WSTRLEN(entry->token.name);
			WSTRTOUTF8(entry->token.name,nWStr,(byte *)sTmp,sizeof(sTmp));
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richPtt_UpdCallRest,name=%s,nsUsrLen=%d,nWStr=%d",sTmp,nsUsrLen,nWStr);
		}
		switch (me->pCallRestList->CallRestInfo[idx].service)
		{
		case IQDK_DIRECTCALLS:
			nDomainLen = WSTRLEN(me->sPttDomain) + 1;
			entry->token.domain = (AECHAR *)MALLOC(nDomainLen*sizeof(AECHAR));
			WSTRCPY(entry->token.domain,me->sPttDomain);
			entry->token.addrType = IQDK_ADDRTYPE_USER_ADDRESS;
			break;
		case IQDK_GROUPCALLS:
			nDomainLen = WSTRLEN(me->sGrpDomain) + 1;
			entry->token.domain = (AECHAR *)MALLOC(nDomainLen*sizeof(AECHAR));
			WSTRCPY(entry->token.domain,me->sGrpDomain);
			entry->token.addrType = IQDK_ADDRTYPE_PREDEFINED_GROUP_ADDRESS;
			break;

// 		case IQDK_ADHOCGROUPCALLS:			// ��ʱ���
// 			nDomainLen = WSTRLEN(me->sGrpDomain) + 1;
// 			entry->token.domain = (AECHAR *)MALLOC(nDomainLen*sizeof(AECHAR));
// 			WSTRCPY(entry->token.domain,me->sGrpDomain);
// 			entry->token.addrType = IQDK_ADDRTYPE_USER_ADDRESS;
// 			break;


		}
		
		{
			char sTmp[100] = {0};
			char sDom[100] = {0};
			int nWStr= WSTRLEN(entry->token.name);
			WSTRTOUTF8(entry->token.name,nWStr,(byte *)sTmp,sizeof(sTmp));
			nWStr= WSTRLEN(entry->token.domain);
			WSTRTOUTF8(entry->token.domain,nWStr,(byte *)sDom,sizeof(sTmp));
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"before set name=%s,domain=%s,addrType=%d",
				sTmp,sDom,entry->token.addrType);
		}

// 		if (me->pCallRestList->CallRestInfo[idx].type == IQDK_ALWAYS_ALLOW)
// 			entry->level = IQDK_CARRIER;		// IQDK_CallRestrictionLevel 
// 		else
		entry->level = IQDK_USER;
		entry->type =  me->pCallRestList->CallRestInfo[idx].type;
		entry->direction = me->pCallRestList->CallRestInfo[idx].direction;

// 					service:	represented by 4 hex digits
// 									These four hex digits represent 16 bits, one bit per service.
// 									Starting from the least significant bit, each bit represents the
// 									following services in order: 
// 								Alerts, Direct Calls, Group Calls, Ad Hoc calls, Chatroom Calls, 
// 									PresenceOnDemand
// 									The remaining bits shall be used for future services and are 
// 									currently don't cares.
// 									For example: 0001 represents Alerts only
// 													  0003 represents Alerts and Direct Calls)

		entry->services = 31;			//  me->pCallRestList->CallRestInfo[idx].service;
		entry->asteriskIsLiteral = FALSE;

		nErr = IQDK30CALLRESTRICTION_SetByIndex(me->pIQDK30CallRestriction,entry,(uint32)idx);

		FREEIF(entry->token.name);
		FREEIF(entry->token.domain);
		FREEIF(entry);

//		if (nErr != SUCCESS) {
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richPtt_UpdCallRest,IQDKCALLRESTRICTION_SetByIndex return=%d,idx=%d",nErr,idx);
//			return;
//		}
	}
}

// =================================================================
// ����һ�����е��ֻ�����
static void richPtt_SetPttCallIInfo(crichptt *me) 
{
	newgimis *pMe;
	int iLen = 0;
	int iLocNext;
	char sTmp[QDK_CALL_NAME_LEN+1];

	
	if (!me) return;
	pMe = me->m_pParent;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richPtt_SetPttCallIInfo in, PttCallNo addr=%d",pMe->sPttCallNo);

	if (pMe->sPttCallNo == NULL) return;
	ud_Trim(pMe->sPttCallNo);
	richptt_FreePttCallNo(me);

	iLen = 0;
	iLocNext = -1;
	while(TRUE)
	{	// while 
		iLocNext += iLen + 1;	// ��','�ĳ���
		if ( iLocNext >= (int)STRLEN(pMe->sPttCallNo) ) 
			break;
		iLen = ud_DistToSplit( pMe->sPttCallNo+iLocNext, DEFAULT_SPLIT );
		if (iLen <= 0) break;

		MEMSET(sTmp,0,sizeof(sTmp));
		MEMCPY(sTmp,pMe->sPttCallNo+iLocNext, iLen);
	
		if (me->pPttCallGroup->count == 0)
			me->pPttCallGroup->PttCallNoGroup = (ICallNoGroup *)MALLOC(sizeof(ICallNoGroup));
		else
			me->pPttCallGroup->PttCallNoGroup = (ICallNoGroup *)REALLOC(me->pPttCallGroup->PttCallNoGroup,(me->pPttCallGroup->count+1)*sizeof(ICallNoGroup));
		
		STRCPY(me->pPttCallGroup->PttCallNoGroup[me->pPttCallGroup->count].CallNo,sTmp);

		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richPtt_SetPttCallIInfo in,id=%d,CallNo=%s",me->pPttCallGroup->count,me->pPttCallGroup->PttCallNoGroup[me->pPttCallGroup->count].CallNo);

		me->pPttCallGroup->PttCallNoGroup[me->pPttCallGroup->count].cCallSts = 0;
		me->pPttCallGroup->count++;

	}	// while 

	me->pPttCallGroup->nCallId = me->pPttCallGroup->count;	// ����û�к��� 
	return;
}

// =================================================================
// ��ȡQchat�ֻ��������Ϣ
static void richptt_GetGroupInfo(crichptt *me) 
{
	newgimis *pMe;
	IQDKAddressData *paddresses;
	IQDK30_ProvListEntry addrList = {0};
	AECHAR szAddrDft[QDK_CALL_NAME_LEN] = {L"0"};
	int16 nListSize = 0;

	if (!me) return;
	pMe = me->m_pParent;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richptt_GetGroupInfo in,pIQDKProvision=%d",me->pIQDKProvision);
	if (me->pIQDKProvision == NULL)
		return;

	IQDK30PROVISION_GetAddressListSize(me->pIQDKProvision,IQDK30_PROV_ADDRESSES_PREDEFINED_GROUPS, &nListSize);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richptt_GetGroupInfo in:group count=%d", nListSize);
	
	if (nListSize > 0)
	{	// nListSize
		int i;
		//	addrList.addressListEntry.addresses = (IQDKAddressData *)&addresses;
		paddresses = (IQDKAddressData *)MALLOC(sizeof(IQDKAddressData) * nListSize);
		MEMSET(paddresses,0,sizeof(IQDKAddressData) * nListSize);
		addrList.addressListEntry.addresses = paddresses;
		
		IQDK30PROVISION_GetProvEntry_AddressList(me->pIQDKProvision,IQDK30_PROV_ADDRESSES_PREDEFINED_GROUPS,&addrList);
		if ( (addrList.addressListEntry.count > 0) && (addrList.addressListEntry.addresses) )
		{	// if -- 2
			
			richptt_FreeGroup(me);
			me->pGroupList->GroupInfo = (IUsrPredGroup *)MALLOC(addrList.addressListEntry.count*sizeof(IUsrPredGroup));
			MEMSET(me->pGroupList->GroupInfo,0,addrList.addressListEntry.count*sizeof(IUsrPredGroup));
			me->pGroupList->count = addrList.addressListEntry.count;

			for (i = 0; i < addrList.addressListEntry.count; i++)
			{	// for
				char sGroupName[QDK_CALL_NAME_LEN+1];							// ����
				
				if (0 != WSTRCMP((const AECHAR*)szAddrDft, (const AECHAR *)addrList.addressListEntry.addresses))
				{
					WSTRTOSTR( paddresses[i].name,sGroupName,sizeof(sGroupName) );
					STRCPY(me->pGroupList->GroupInfo[i].sName,sGroupName );
				
					{	// ConfId�� domain��ʽ��: group.ptt.189.cn:8320000e5402:���ȼ�����3
						char pConfId[IQDK_CONF_ID_SIZE*2+1];
						int j = 0;
						int iDomainLen = WSTRLEN(me->sGrpDomain) + 1;	// ���ָ���
						MEMSET(pConfId,0,sizeof(pConfId));
						WSTRTOUTF8(paddresses[i].domain+iDomainLen,IQDK_CONF_ID_SIZE*2,(byte *)pConfId,sizeof(pConfId));
						for (j = 0; j< IQDK_CONF_ID_SIZE; j++ )
						{
							char sTmp[3];
							MEMSET(sTmp,0,sizeof(sTmp));
							MEMCPY(sTmp,pConfId+j*2,2);
							me->pGroupList->GroupInfo[i].ConfId[j] = (unsigned char)STRTOUL(sTmp,NULL,16);
						}

						{
							char sAscConfId[IQDK_CONF_ID_SIZE*3+1];
							ud_ToAscFmt((char *)me->pGroupList->GroupInfo[i].ConfId,IQDK_CONF_ID_SIZE,sAscConfId,sizeof(sAscConfId));
							ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"confId=%s,sName=%s,sizeof confid=%d",sAscConfId,me->pGroupList->GroupInfo[i].sName,sizeof(IQDKConfIdType));
						}


					}   // ConfId
					
				}
				FREEIF((paddresses+i)->name);
				FREEIF(paddresses[i].domain);
		
			}	// for
		}	// if -- 2	
		
		FREEIF(paddresses);
		
	}  // nListSize

	return;
}


/* ===========================================================================================
˵��			
   ����һ������
�������

��������
============================================================================================== */
static void richptt_PttCall(crichptt *me)
{
	newgimis *pMe;
	uint8 idx;
	uint8 nFindId;

 	if (!me) return;
 	pMe = me->m_pParent;

 	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richptt_PttCall in,[Qchat Status]=%d,[ptt Call status]=%d",richptt_HasStatus(me,QCHAT_NMASK_CALL),me->pPttCallGroup->cPttCallSts);

	if(richptt_HasStatus(me,QCHAT_NMASK_CALL)==QCHAT_NMASK_CALL)		{
		me->pPttCallGroup->cPttCallSts = 6;
		return;
	}

	if (  (me->pPttCallGroup->cPttCallSts != 2) && (me->pPttCallGroup->cPttCallSts != 5) )
		return;

	me->pPttCallGroup->cPttCallSts = 3;

	// ��ȡ�����
	idx = (uint8)(GETTIMEMS() % me->pPttCallGroup->count);			// ����һ�������
	nFindId = idx;
	// Ѱ�ҿɺ��е��ֻ�����
	while(TRUE)
	{	

		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richptt_PttCall in,PttCallNoGroup[%d].cCallSts=%d",nFindId,me->pPttCallGroup->PttCallNoGroup[nFindId].cCallSts);

		if (me->pPttCallGroup->PttCallNoGroup[nFindId].cCallSts == 0) {
			idx = nFindId;	
			break;
		}

		if (nFindId == (idx-1)) {  // û�к��ʵĶԽ�����
			idx = me->pPttCallGroup->count;
			break;
		}

		nFindId++;
		if (nFindId == me->pPttCallGroup->count)  {
			if (idx == 0) {	// û�к��ʵĶԽ�����
				idx = me->pPttCallGroup->count;
				break;
			}
			else
				nFindId = 0;
		}
	}
	
	me->pPttCallGroup->nCallId = idx;
	if (idx == me->pPttCallGroup->count) {	// û�к��ʵĶԽ�����
		me->pPttCallGroup->cPttCallSts = 4;
		me->pPttCallGroup->iLastCallTime = GETTIMEMS();
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richptt_PttCall in,can not find idle ptt CallNo");
		return;
	}

	// �ҵ����ʵ�
	me->pPttCallGroup->iLastCallTime = GETTIMEMS();
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richptt_PttCall in,find pttCallNo=%s,mobile type=%s",me->pPttCallGroup->PttCallNoGroup[idx].CallNo,pMe->sMobileType);

	if (me->pPttCallGroup->cPttCallSts < 4)
		me->pPttCallGroup->cPttCallSts = 5;

    // ����
     if (STRCMP(pMe->sMobileType,"01") == 0 /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/)	
     {	// ����
 		richptt_PttCall_01(me);
     }
 	else {
		// default ֱ������qdk����
		richptt_PttCall_xx(me);
	}	


	return;
}


/* ===========================================================================================
˵��			
   ���Զ��رպ��ٴ�ǿ������(һ������)
�������

��������
============================================================================================== */
static void richptt_WaitForPttCall(crichptt *me)
{
	newgimis *pMe;
	
	if (!me) return;
	pMe = me->m_pParent;

	me->pPttCallGroup->cPttCallSts = 5;
    // ����
	if (STRCMP(pMe->sMobileType,"01") == 0 /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/)	
	{	// ����
		richptt_PttCall_01(me);
	}
	else {
		// default ֱ������qdk����
		richptt_PttCall_xx(me);
	}	
}


/* ========================================================================
ͨ�õ���
 ======================================================================== */
static void richptt_PttCall_xx(crichptt *me)
{
	char *sName = NULL;
	int idx;
	newgimis *pMe;
	IQDK_CommonCallPrefs commonPrefs;			
	IQDK_DirectCallPrefs  directCallPref;
	IQDK30_CallPreferences callPrefs;	

	IQDK_ErrorType nErr;

	if (!me) return;
	pMe = me->m_pParent;

	if ( (me->pPttCallGroup->nCallId == me->pPttCallGroup->count) ) return;
	
	if(richptt_HasStatus(me,QCHAT_NMASK_CALL)==QCHAT_NMASK_CALL) 
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_PttCall_xx in,Already in a call ...");
		return;
	}
	
	idx = me->pPttCallGroup->nCallId;
	sName = STRDUP(me->pPttCallGroup->PttCallNoGroup[idx].CallNo );
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_PttCall_xx, ready to call,name=%s,index=%d",sName,idx);
	
	if(me->addrList)
	{   
		FREEIF(me->addrList->addresses->name);
		FREEIF(me->addrList->addresses->domain);
		FREEIF(me->addrList->addresses);
		FREEIF(me->addrList);
	}
	if(me->addrData)
	{
		FREEIF(me->addrData->name);
		FREEIF(me->addrData->domain);
		FREEIF(me->addrData);
	}

	me->addrData = uf_CreateAddressData(pMe,(const char**)&sName,1,me->sPttDomain,	IQDK_ADDRTYPE_USER_ADDRESS );
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_PttCall_xx in,out uf_CreateAddressData ...");

	me->addrList= (IQDKAddressListType*)MALLOC(sizeof(IQDKAddressListType));
	me->addrList->addresses= (IQDKAddressData*)MALLOC(sizeof(IQDKAddressData));
	me->addrList->addresses->name = (AECHAR*)MALLOC(sizeof(AECHAR)*(WSTRLEN(me->addrData->name)+1));
	me->addrList->addresses->domain = (AECHAR*)MALLOC(sizeof(AECHAR)*(WSTRLEN(me->addrData->domain)+1));
	me->addrList->count=1;
	WSTRCPY(me->addrList->addresses->name,me->addrData->name);
	WSTRCPY(me->addrList->addresses->domain,me->addrData->domain);
	me->addrList->addresses->addrType= me->addrData->addrType; 
	
	commonPrefs.preferredAddrIndex = 1;  
	commonPrefs.waitForPTTPress    = FALSE;		//  FALSE;
	
	directCallPref.addMemberDisableOption = FALSE;
	directCallPref.callTerminationOption = IQDK_CALL_TERMINATE_IMMEDIATELY;
	directCallPref.common = commonPrefs;
	callPrefs.directCallPrefs = directCallPref;
	
	nErr = IQDK30CALL_StartCall(me->pIQDKCall, IQDK_DIRECT_CALL, me->addrList, NULL, &callPrefs);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_PttCall_xx in,IQDK30CALL_StartCall return=%d",nErr);
	
	if(me->addrData)
	{
		FREEIF(me->addrData->name);
		FREEIF(me->addrData->domain);
		FREEIF(me->addrData);
	}
	if(me->addrList)
	{
		FREEIF(me->addrList->addresses->domain);
		FREEIF(me->addrList->addresses->name);
		FREEIF(me->addrList->addresses);
		FREEIF(me->addrList);
	}
	
	FREEIF(sName);
	
	return;
}


// ================================================================================
//  �ֻ�������01�ĵ���
static void richptt_PttCall_01(crichptt *me)
{
	newgimis *pMe;
	int iLen;
	int idx;
	AECHAR szNumber[QCHAT_FLDLEN_PTT_LEN] = {0};
//   SpeedDialNum Number = {0};

	if (!me) return;
	pMe = me->m_pParent;

	idx = me->pPttCallGroup->nCallId;
	iLen = STRLEN(me->pPttCallGroup->PttCallNoGroup[idx].CallNo);

//	STRTOWSTR(me->pPttCallGroup->PttCallNoGroup[idx].CallNo,Number.pSpeedNum, sizeof(Number.pSpeedNum));
//	Number.speeddial_type = SPEEDDIAL_QCHAT_DIRECTCALL;
//	Number.AdHoc_id = 0;
	
//	ISHELL_SendEvent (pMe->a.m_pIShell,AEECLSID_QCHATUI,EVT_QCHAT_CALL,
//		QCHATCALL_EVENT_START_DIRECT_CALL_BY_NAME, (uint32)&Number);


 						UTF8TOWSTR( (byte *)me->pPttCallGroup->PttCallNoGroup[idx].CallNo,iLen,szNumber,sizeof(szNumber));
 						WSTRLCAT(szNumber,L"@",sizeof(szNumber) / sizeof(AECHAR));
 						WSTRLCAT(szNumber, me->sPttDomain, sizeof(szNumber) / sizeof(AECHAR));
 						{
 							char sTmp[126];
 							MEMSET(sTmp,0,sizeof(sTmp));
 							WSTRTOUTF8(szNumber,WSTRLEN(szNumber),(byte *)sTmp, sizeof(sTmp));  
 							ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richptt_PttCall_01 in, pttCallNo=%s",sTmp);
 						}
 						ISHELL_SendEvent (pMe->a.m_pIShell,AEECLSID_QCHATUI,EVT_QCHAT_CALL,
 							QCHATCALL_EVENT_START_DIRECT_CALL_BY_ADDRESS, (uint32)&szNumber[0]);
	
	return;
}


/* ===========================================================================================
˵��			
   ����ע��İ���
�������
   ͬ richptt_HandleEvent

��������
   TRUE ������ؼ��Ѿ���������Ϣ�� FALSE ����ؼ�û�д�����Ϣ
============================================================================================== */
static boolean richptt_NotifyShellKey(crichptt *me, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	newgimis *pMe;
	AEENotify* pNotify = (AEENotify*) dwParam;
	NotifyKeyEvent *pNKeyEvn = (NotifyKeyEvent*) pNotify->pData;
	uint32 nGetTimes = GETTIMEMS();

	if (!me) return FALSE;
	pMe = me->m_pParent;

// ��ʽ��Ҫ�ر�
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richptt_NotifyShellKey in, key value=%d",pNKeyEvn->wParam);

	// һ������,����gimis�нṹnewgimis���ֶ�iGetkey
	if ( pNKeyEvn->wParam == pMe->nPttkey)		// AVK_PTT   AVK_4
	{

		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richptt_NotifyShellKey in,AVK_PTT,cPttCallSts=%d,iGetkey=%d",me->pPttCallGroup->cPttCallSts,me->pPttCallGroup->iGetkey);

		if (me->pPttCallGroup->count < 1) {
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_NotifyShellKey in, Ptt CallNo Group count=0");
			return FALSE;
		}

		if(richptt_HasStatus(me,QCHAT_NMASK_CALL)==QCHAT_NMASK_CALL)		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_NotifyShellKey in,Already in a call ...");
			return FALSE;
		}
		else if (me->pPttCallGroup->cPttCallSts==5)		// 2013-10-01 �����и�©��
			me->pPttCallGroup->cPttCallSts=8;	

		if ( ((me->pPttCallGroup->cPttCallSts==4) || (me->pPttCallGroup->cPttCallSts==8))  && 
			 (pNKeyEvn->eCode != EVT_KEY_RELEASE) )	// �ϴκ���ʧ��
		{
			if ( nGetTimes < me->pPttCallGroup->iLastCallTime)
				 nGetTimes += 24*60*60*1000;
			if (  (nGetTimes -  me->pPttCallGroup->iLastCallTime) <= RICH_CALL_INTERVAL  )  {
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_NotifyShellKey in, (GETTIMEMS()-iLastCallTime)=%d",(GETTIMEMS() -  me->pPttCallGroup->iLastCallTime));
				return FALSE;
			}
			else {
				int idx;
				for(idx =0; idx < me->pPttCallGroup->count;idx++) {
					me->pPttCallGroup->PttCallNoGroup[idx].cCallSts = 0;
				}
				me->pPttCallGroup->cPttCallSts = 0;
				me->pPttCallGroup->iGetkey = 0;
				me->pPttCallGroup->iLastCallTime = 0;
				me->pPttCallGroup->nCallId = me->pPttCallGroup->count;	// ����û�к��� 
			}
		}
		
		if (me->pPttCallGroup->cPttCallSts == 1)
			if (nGetTimes < me->pPttCallGroup->iGetkey)
				 nGetTimes += 24*60*60*1000;

		if ( (me->pPttCallGroup->cPttCallSts == 0) && (me->pPttCallGroup->iGetkey == 0) && (pNKeyEvn->eCode != EVT_KEY_RELEASE) )   {
			me->pPttCallGroup->iGetkey = GETTIMEMS();
			me->pPttCallGroup->cPttCallSts = 1;
			return TRUE;
		}
		else if ( (me->pPttCallGroup->cPttCallSts == 1) && ((nGetTimes -  me->pPttCallGroup->iGetkey) >= 2000) ){	// 3��  if -- a
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "zjie-ptt after settimer");
			me->pPttCallGroup->cPttCallSts = 2;
			me->pPttCallGroup->iGetkey = 0;
			ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_PTTCALL_EVENT,(uint16)0, 0);
 			return TRUE;
		}		// 0.5��   if -- a
		else if ( (me->pPttCallGroup->cPttCallSts == 1) && (pNKeyEvn->eCode == EVT_KEY_RELEASE) ) { // if -- a0
			me->pPttCallGroup->iGetkey = 0;
			me->pPttCallGroup->cPttCallSts = 0;
			me->pPttCallGroup->iLastCallTime = 0;
		}	// if -- a0

	}

	return FALSE;

}
// ========================================================================
// ��������е�Ԥ������������
static void richptt_WaitToCall(crichptt *me)  
{
	newgimis *pMe;

	if (!me) return;
	pMe = me->m_pParent;
   
	if (me->nCallCnt < 3) { 
		int nWaitTime;
		uint32 nRand = GETTIMEMS() % 997 + (STRTOUL(me->MyMobileNo,NULL,10)) % 1067 ;			// ����һ�������
		if (me->nCallCnt == 0)
			nWaitTime = (nRand % 100)*10 + RICH_CALL_INTERVAL + 200;          
		else
		    nWaitTime = (nRand % 300)*10 + 2000; 
		me->nCallCnt += 1;
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_WaitToCall in nCallCnt=%d,nWaitTime=%d", me->nCallCnt,nWaitTime);	
		CALLBACK_Cancel( &me->cbGen );
		CALLBACK_Init( &me->cbGen, richptt_cbStartCall, me );
		ISHELL_SetTimerEx(me->m_pParent->a.m_pIShell, nWaitTime, &me->cbGen );
	}
	else {  // ����3�ζ���ʧ�ܣ���������
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_WaitToCall in nCallCnt=%d, call max time", me->nCallCnt);	
		me->nCallCnt = 0;
		me->idxCallReq = -1;
	}
	
	return;
}
/* ===========================================================================================
˵��			
   ����QDK�¼�

�������
   ͬ richppt_HandleEvent

��������
   TRUE ������ؼ��Ѿ���������Ϣ�� FALSE ����ؼ�û�д�����Ϣ
============================================================================================== */
static boolean richptt_QDKNotifier(crichptt *me, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{

   if ( richptt_EventQDKCall(me,eCode,wParam,dwParam) )
	   return TRUE;

   if ( richptt_EventQDKMembership(me,eCode,wParam,dwParam) )
	   return TRUE;



   return FALSE;
}

// =================================================================================
// ����NMASK_QDK_CALL��ص��¼� 
static boolean richptt_EventQDKCall(crichptt *me, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	newgimis *pMe;
	QDKNotifierEventType *pQDKNotifierEventType;
	AEENotify* pNotify = (AEENotify*)dwParam;

	if (!me) return FALSE;
	pMe = me->m_pParent;

	DBGPRINTF("zjie-EventQDKCall in");	//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"EventQDKCall in");
	
	if ((pNotify->dwMask & NMASK_QDK_CALL ) == NMASK_QDK_CALL )
	{
		boolean bRtn;
		IQDK_ErrorType nErr;
		IQDK_CallEventPayloadType * pCallEvent = NULL;
		pQDKNotifierEventType = (QDKNotifierEventType*) pNotify->pData;
		
		bRtn = FALSE;
 		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), 
 			"NMASK_QDK_CALL in,GroupList count =%d, evtCode=%u,evtPayloadv= %u",
 			me->pGroupList->count,pQDKNotifierEventType->evtCode,pQDKNotifierEventType->evtPayload);

		if (pQDKNotifierEventType->evtCode == IQDK_EVT_CALL_CONNECTED)
		{
			pMe->m_F_DJ = 1; //�����ڶԽ�

// 			if (pMe->pISoundPlayer != NULL)
// 			{
// 				ISOUNDPLAYER_Pause(pMe->pISoundPlayer);
// 			}
		}
		else if (pQDKNotifierEventType->evtCode == IQDK_EVT_END_CALL)
		{
			pMe->m_F_DJ = 0; //�Խ�����

// 			if (pMe->pISoundPlayer != NULL)
// 			{
// 				ISOUNDPLAYER_Resume(pMe->pISoundPlayer);
// 			}
		}

		if (pQDKNotifierEventType->evtCode == IQDK_EVT_CALL_NOTICE)	// 28928
		{
			//IQDK_CallNoticeEventType
			bRtn = FALSE;
			goto ExitHere;  // return TRUE;
		}
		else if (pQDKNotifierEventType->evtCode == IQDK_EVT_CALL_CONNECTED)	// =28929
		{		//			IQDK_CallEventPayloadType * pCallEvent = NULL;
			IQDK_CallConnectedEventType *pCallConnected = NULL;
			nErr = IQDK30CALL_GetEventPayload(me->pIQDKCall,AEEIID_IQDKCALL,IQDK_EVT_CALL_CONNECTED, &pCallEvent);
			if (nErr != SUCCESS) {
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK_EVT_CALL_CONNECTED,IQDK30CALL_GetEventPayload,err=%d",nErr);
				return FALSE;
			}

			pCallConnected = (IQDK_CallConnectedEventType*) pCallEvent;
			if( pCallConnected->rejectReason == IQDK_SUCCESS) {
				richptt_StatusOn(me,(QCHAT_NMASK_CALL|QCHAT_NMASK_CONNECT));
			}	
			else {
				richptt_StatusOff(me,QCHAT_NMASK_CONNECT);
			}


			if (pCallConnected->callType == IQDK_DIRECT_CALL) {	// if -- ����
				if( pCallConnected->rejectReason == IQDK_SUCCESS) {
					if (me->pPttCallGroup->nCallId < me->pPttCallGroup->count) {
						me->pPttCallGroup->cPttCallSts = 6;
						me->pPttCallGroup->PttCallNoGroup[me->pPttCallGroup->nCallId].cCallSts = 1; 

						// �����ϻ�ϵ���ԭ����ʱ������������������ֱ�ӻ�û���Ȩ
  						ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_REQUESTFLOOR_EVENT,(uint16)0, 0);
					}
					if (pCallConnected->originatorAddr.name != NULL) {
						char sTmp[100];
						MEMSET(sTmp,0,sizeof(sTmp));
						WSTRTOSTR(pCallConnected->originatorAddr.name, sTmp, sizeof(sTmp));
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),
							"Event received: Direct Call Connected Success,originatorAddr name=%s,results=%d",sTmp,pCallConnected->results);
					}
					else
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),
								"Event received: Direct Call Connected Success,originatorAddr name=null results=%d",pCallConnected->results);
				}
				else {
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),
						"Event received:Direct Call Connected Failure,rejectReason=%d,results=%d,cPttCallSts=%d",
						pCallConnected->rejectReason,pCallConnected->results,me->pPttCallGroup->cPttCallSts);	

					if ( me->pPttCallGroup->cPttCallSts == 5)
					{	// һ�����жԷ�δ����ʧ�ܲ��ڴ˴����Է�ͨ�����ڴ˴�
						if (me->pPttCallGroup->nCallId < me->pPttCallGroup->count) {
							me->pPttCallGroup->PttCallNoGroup[me->pPttCallGroup->nCallId].cCallSts = 2;
							ISHELL_SetTimer(pMe->a.m_pIShell, 3000, (PFNNOTIFY)richptt_PttCall, (uint32*)me);
						}
					}
				}

			}	// if -- ����
			else if (pCallConnected->callType == IQDK_PREDEFINED_GROUP_CALL) {	// if -- ���
				if (pCallConnected->confId != NULL) {
					boolean bHasName = FALSE;
					if (pCallConnected->originatorAddr.name != NULL) {
					   WSTRTOSTR(pCallConnected->originatorAddr.name, me->originatorName, sizeof(me->originatorName));
					   bHasName = TRUE;
					}
					else
						STRCPY(me->originatorName,me->MyMobileNo);
					if( pCallConnected->rejectReason == IQDK_SUCCESS) {
					    ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),
							"Event received: Group Call Connected Success,bHasName=%d,originator=%s,results=%d",
							bHasName,me->originatorName,pCallConnected->results);
					}
				    else {
						// IQDK_REJECT_ALL_TARGETS_REJECT = 15, IQDK_REJECT_TARGET_BUSY = 8, IQDK_REJECT_NO_RESOURCES_AVAILABLE = 7
					    ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),
							"Event received:Group Call Connected Failure,rejectReason=%d,bHasName=%d,originator=%s,results=%d",
							pCallConnected->rejectReason,bHasName,me->originatorName,pCallConnected->results);	
					}

					richptt_SetReqGroupStatusWithConfID(me,pCallConnected->confId,pCallConnected->rejectReason);
// 					if (!richptt_SetReqGroupStatusWithConfID(me,pCallConnected->confId,pCallConnected->rejectReason)) {
// 						MEMCPY(me->ConnConfID,pCallConnected->confId,sizeof(IQDKConfIdType));
// 					   IQDK30MEMBERSHIP_GetGroupName(me->pIQDK30Membership,pCallConnected->confId);
// 					}
				}
			}	// if -- ���
			else 
			{
				char sName[QDK_CALL_NAME_LEN*2+1];
				MEMSET(sName,0,sizeof(sName));
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Call Connected,type=%d,reason=%d,idxCalling=%d,idxCallWait=%d,idxCallReq=%d - cPttCallSts=%d",
					pCallConnected->callType,pCallConnected->rejectReason,me->idxCalling,me->idxCallWait,me->idxCallReq,me->pPttCallGroup->cPttCallSts);

				// Add by zjie 2013-01-01 һ������ʧ�����ڴ˴�
				if ( (pCallConnected->callType == 0) && (pCallConnected->rejectReason != IQDK_SUCCESS) )
				{
					if ( me->pPttCallGroup->cPttCallSts == 5)
					{	
						if (me->pPttCallGroup->nCallId < me->pPttCallGroup->count) {
							me->pPttCallGroup->PttCallNoGroup[me->pPttCallGroup->nCallId].cCallSts = 2;
							//                   2900 ����                 3000,  3500 ����
							ISHELL_SetTimer(pMe->a.m_pIShell, 3000, (PFNNOTIFY)richptt_PttCall, (uint32*)me);
						}
					}
					// ���ʱ����δע����쳣��ʵ���ֻ��ǿ��ŵģ�
					if ( pCallConnected->rejectReason == IQDK_REJECT_TARGETS_NOT_REACHABLE ) 	
						if ( (me->idxCallWait >= 0) || (me->idxCallReq >= 0) ) 
						{
							if (me->idxCallWait >= 0) {
								me->idxCallReq = me->idxCallWait;
								me->idxCallWait = -1;
							}
							richptt_WaitToCall(me);
						}

				}
				// Add End

 				if ( pCallConnected->originatorAddr.name != NULL) {
 					WSTRTOUTF8(pCallConnected->originatorAddr.name,WSTRLEN(pCallConnected->originatorAddr.name),(byte *)sName,sizeof(sName));
 					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"name=%s",sName);
 				}
 				else
 					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"name=null");
			}

			bRtn = TRUE;
			goto ExitHere;  // return TRUE;
			
		} 
		else if (pQDKNotifierEventType->evtCode == IQDK_EVT_END_CALL)  // = 28930
		{
			IQDK_EndCallEventType *pEndCall = NULL;
			nErr = IQDK30CALL_GetEventPayload(me->pIQDKCall,AEEIID_IQDKCALL,IQDK_EVT_END_CALL, &pCallEvent);
			if (nErr != SUCCESS) {
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK_EVT_END_CALL,IQDK30CALL_GetEventPayload,err=%d",nErr);
			}
			else
 			   pEndCall = (IQDK_EndCallEventType*) pCallEvent;

			if (pEndCall != NULL)
			{
				char sAscConfId[IQDK_CONF_ID_SIZE*3+1];
				ud_ToAscFmt((char *)pEndCall->confId,IQDK_CONF_ID_SIZE,sAscConfId,sizeof(sAscConfId));

				richptt_StatusOff(me,QCHAT_NMASK_CALL);
				richptt_StatusOff(me,QCHAT_NMASK_CONNECT);

				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),
					"Event received: Call Ended,calling=%d,req call=%d,callType=%d,confId=%s,cGrpCallSts=%d,iGrpCallTime=%d,DirectCall[nCallId=%d,count=%d,CallSts=%d]", 
					me->idxCalling,me->idxCallReq,pEndCall->callType,sAscConfId,me->pGroupList->cGrpCallSts,me->pGroupList->iGrpCallTime,me->pPttCallGroup->nCallId,me->pPttCallGroup->count,me->pPttCallGroup->cPttCallSts);	

				if ( (pEndCall->callType == IQDK_PREDEFINED_GROUP_CALL) && (me->idxCalling >= 0 ) )
				{ // if -- 1
					
				  	if ( MEMCMP(pEndCall->confId,me->pGroupList->GroupInfo[me->idxCalling].ConfId,sizeof(IQDKConfIdType)) == 0  )
					{
						if (me->pGroupList->cGrpCallSts != 0) {		// 2013-03-20
							uint32 nTimes = GETTIMEMS();			// ��ҹ�����ĺ�����
							if (nTimes < me->pGroupList->iGrpCallTime)  nTimes += (24*60*60*1000);	 // ������ҹ
//																						if ( (me->pGroupList->cGrpCallSts <=2) && ((nTimes - me->pGroupList->iGrpCallTime) < (5*1000)) )
							if ( ((nTimes - me->pGroupList->iGrpCallTime) < 3500) )	
							{
								me->idxCallReq =  me->idxCalling;
								me->idxCallWait = -1;
								me->idxCalling = -1;
								me->pGroupList->cGrpCallSts = 1;
								me->pGroupList->iGrpCallTime = 0;
								ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_CALLAGAIN_EVENT,(uint16)0, 0);
							}
							else {
								me->pGroupList->cGrpCallSts = 0;
								me->pGroupList->iGrpCallTime = 0;
							}
						}	// 2013-03-20

						if ( (me->idxCalling < me->pGroupList->count) && (me->idxCalling >=0) ) {
							me->pGroupList->GroupInfo[me->idxCalling].cStatus = 0;  
							MEMSET(me->pGroupList->GroupInfo[me->idxCalling].originator,0,sizeof(me->pGroupList->GroupInfo[me->idxCalling].originator));
						}
						me->idxCalling = -1;
						
						// ����������е�Ԥ�����飬�������
						if (me->pGroupList->cGrpCallSts == 0)	// ��Ӵ������Ա��뱾����Ϊ���зֿ� 2013-03-20
							if (me->idxCallReq >= 0) {	// -- Rec
								// �����µĺ���
								richptt_WaitToCall(me);
								bRtn = TRUE;
								goto ExitHere;  // return TRUE;
							}	// -- Rec
					}
				}  // if -- 1
				else if (pEndCall->callType == IQDK_DIRECT_CALL)
				{ // if ����
					int idx;
//					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK_EVT_END_CALL,IQDK_DIRECT_CALL");
					if (me->pPttCallGroup->nCallId != me->pPttCallGroup->count) { // һ�����з����
						if ( (me->pPttCallGroup->cPttCallSts == 6) || (me->pPttCallGroup->cPttCallSts == 5) )  {	// ��ͨ�����϶ϵ�
							me->pPttCallGroup->cPttCallSts = 8;
							me->pPttCallGroup->iLastCallTime = GETTIMEMS();
							ISHELL_SetTimer(pMe->a.m_pIShell, 3000, (PFNNOTIFY)richptt_WaitForPttCall, (uint32*)me);
						}	
						else {
							for(idx =0; idx < me->pPttCallGroup->count;idx++) {
								me->pPttCallGroup->PttCallNoGroup[idx].cCallSts = 0;
							}
							me->pPttCallGroup->iGetkey = 0;
							me->pPttCallGroup->cPttCallSts = 0;
							me->pPttCallGroup->iLastCallTime = 0;
							me->pPttCallGroup->nCallId = me->pPttCallGroup->count;	// ����û�к��� 
						}
					}
				} // if ����	
				else if (pEndCall->callType == IQDK_UNDEFINED_TYPE) 
				{	// ��֪Ϊʲô�����������	Invalid call type
					if (me->pPttCallGroup->nCallId != me->pPttCallGroup->count) { // һ�����з����
						if ( (me->pPttCallGroup->cPttCallSts == 6) || (me->pPttCallGroup->cPttCallSts == 5) )  {	// ��ͨ�����϶ϵ�
							me->pPttCallGroup->cPttCallSts = 8;
							me->pPttCallGroup->iLastCallTime = GETTIMEMS();
							ISHELL_SetTimer(pMe->a.m_pIShell, 3000, (PFNNOTIFY)richptt_WaitForPttCall, (uint32*)me);
						}	
					}	// һ�����з����
					else
					{	// Ԥ�������ȼ�������
						if (me->idxCalling != -1)	me->idxCalling = -1;

						if ( (me->idxCallWait >= 0) || (me->idxCallReq >= 0) ) 	{
							if (me->idxCallWait >= 0) {
								me->idxCallReq = me->idxCallWait;
								me->idxCallWait = -1;
							}
							richptt_WaitToCall(me);
						}

					}	// Ԥ�������ȼ�������

				}  // ��֪Ϊʲô�����������	Invalid call type

			}
			else
			   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Event received: Call Ended,pEndCall is null,req call=%d", me->idxCallReq);	

			bRtn = FALSE;
			goto ExitHere;  // return TRUE;

		}
		else if (pQDKNotifierEventType->evtCode  == IQDK_EVT_CALL_STATUS)	// 28931
		{	
			char sUserName[QDK_CALL_NAME_LEN+1] = {0};
			IQDK_CallStatusEventType *pCallStatus = NULL;


			nErr = IQDK30CALL_GetEventPayload(me->pIQDKCall,AEEIID_IQDKCALL,IQDK_EVT_CALL_STATUS, &pCallEvent);
			pCallStatus = (IQDK_CallStatusEventType*) pCallEvent;

			// Add by zjie 2013-01-01
			if (me->pPttCallGroup->cPttCallSts == 6) {
				if ( (pCallStatus->myAction == TRUE) && (pCallStatus->floorAction == 0) )
					me->pPttCallGroup->cPttCallSts = 7;
			}
			
			if ( pCallStatus->talker.name != NULL)
			    WSTRTOSTR(pCallStatus->talker.name, sUserName, sizeof(sUserName));
			else
				SPRINTF(sUserName,"null");		// ���������з�
// typedef enum
// {
// 	    IQDK_FLOOR_GRANT,
// 		IQDK_FLOOR_RELEASE,
// 		IQDK_FLOOR_DENIED_IN_USE,
// 		IQDK_FLOOR_DENIED_NOTAPARTICIPANT,
// 		IQDK_FLOOR_DENIED_LONELY_USER,
// 		IQDK_FLOOR_DENIED_NO_PRIVILEGE,
// 		IQDK_FLOOR_REVOKED_TIMEOUT,
// 		IQDK_FLOOR_REVOKED_INTERRUPT,
// 		IQDK_FLOOR_NO_CHANGE
// } IQDK_FloorActionType;

			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),
				  "IQDK_EVT_CALL_STATUS in,floorAction=%d,myAction=%d,paused=%d,talker=%s,memberCount=%d,participantCount=%d",
				  pCallStatus->floorAction,pCallStatus->myAction,pCallStatus->paused,sUserName,pCallStatus->memberCount,pCallStatus->participantCount);

			// Add by zjie 2013-03-20
			if ( me->pGroupList->cGrpCallSts>=1 ) {
				if ( (me->pGroupList->cGrpCallSts == 1) && (pCallStatus->floorAction == 1) ) {  
					me->pGroupList->iGrpCallTime = GETTIMEMS();    // ����Ҫ�ͷŻ���Ȩ���ͷųɹ���ʼ��ʱ��
					me->pGroupList->cGrpCallSts = 2;	
				}
			}
			// Add End

			bRtn = TRUE;
			goto ExitHere;  // return TRUE;
		}
		else if (pQDKNotifierEventType->evtCode  == IQDK_EVT_MEMBERS_INVITED)	// 28932
		{
			IQDK_MembersInvitedEventType *pMembersInvited = NULL;
			nErr = IQDK30CALL_GetEventPayload(me->pIQDKCall,AEEIID_IQDKCALL,IQDK_EVT_MEMBERS_INVITED, &pCallEvent);
			pMembersInvited = (IQDK_MembersInvitedEventType*) pCallEvent;
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK_EVT_MEMBERS_INVITED in,rejectReason=%d",pMembersInvited->rejectReason);
			bRtn = TRUE;
			goto ExitHere;  // return TRUE;
		}
		else if (pQDKNotifierEventType->evtCode  == IQDK_EVT_INVITE_RECEIVED)  // 28933
		{
			IQDK_InviteReceivedEventType *pInviteReceived = NULL;
			nErr = IQDK30CALL_GetEventPayload(me->pIQDKCall,AEEIID_IQDKCALL,IQDK_EVT_INVITE_RECEIVED, &pCallEvent);
			pInviteReceived = (IQDK_InviteReceivedEventType*) pCallEvent;
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK_EVT_INVITE_RECEIVED in,rejectReason=%d",pInviteReceived->rejectReason);
			bRtn = TRUE;
			goto ExitHere;  // return TRUE;
		}


ExitHere:
		FREEIF(pCallEvent);
		return bRtn;	//  FALSE;
	}
	
	return FALSE;
}

// ==================================================================
// ����NMASK_QDK_MEMBERSHIP��ص��¼� 
static boolean richptt_EventQDKMembership(crichptt *me, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	newgimis *pMe;
	QDKNotifierEventType *pQDKNotifierEventType;
	AEENotify* pNotify = (AEENotify*)dwParam;
	pQDKNotifierEventType = (QDKNotifierEventType*) pNotify->pData;

	if (!me) return FALSE;
	pMe = me->m_pParent;

	if ((pNotify->dwMask & NMASK_QDK_MEMBERSHIP ) == NMASK_QDK_MEMBERSHIP )
	{
		char sName[QDK_CALL_NAME_LEN+1];
		IQDK30_MembershipEventType *pMembership = NULL;
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"EventQDKMembership in,GroupList count=%d,evtCode=%u",
			 me->pGroupList->count,pQDKNotifierEventType->evtCode);

 		if (pQDKNotifierEventType->evtCode == IQDK_EVT_MEMBER_LIST)					// 29441
		{
			ud_RecLog(pMe->a.m_pIShell,'0',&(pMe->nInd),"MEMBER_LIST, in");

		}
		else if (pQDKNotifierEventType->evtCode == IQDK_EVT_DETAIL_GRP_CALL_INFO)		// 29442
		{
// ������л��ס
// 			IQDK_GroupCallMemberDetailsEventType *pGroupCallMemberDetails = NULL;
// 			IQDK30MEMBERSHIP_GetEventPayload(me->pIQDK30Membership,AEEIID_IQDK30MEMBERSHIP,IQDK_EVT_DETAIL_GRP_CALL_INFO,&pMembership);
// 			if (pMembership != NULL) {	// 29442
// 				char sInfo[MAX_MESSAGE_LEN+1] = {0};
// 				char sUserName[QDK_CALL_NAME_LEN+1] = {0};
// 				IQDK_UserParticipationStatus iUserStatus;
// 				int i;
// 
// 				pGroupCallMemberDetails = (IQDK_GroupCallMemberDetailsEventType *)pMembership;
// 
// 				SPRINTF(sInfo,"num=%d",pGroupCallMemberDetails->numMemberInfo);
// 				if (pGroupCallMemberDetails != NULL)
// 				{
// 					for (i = 0; i< (int)pGroupCallMemberDetails->numMemberInfo;i++)
// 					{
// 						WSTRTOSTR(pGroupCallMemberDetails->memberInfoPtr[i].userAddr.name , sUserName, sizeof(sUserName));
// 						iUserStatus = pGroupCallMemberDetails->memberInfoPtr[i].participationStatus;
// 						if ( (STRLEN(sInfo)+ STRLEN(sUserName) + 2 + 2) <= MAX_MESSAGE_LEN )
// 							SPRINTF(sInfo,"%s,%s=%d",sInfo,sUserName,iUserStatus);
// 						else
// 							break;
// 					}
// 					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), sInfo);
// 				}
// 				else
// 				   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pGroupCallMemberDetails is NULL");
// 
// 			}	// 29442
// 			else
// 			   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK_EVT_DETAIL_GRP_CALL_INFO in,pMembership is NULL");

		}
		else if (pQDKNotifierEventType->evtCode ==  IQDK_EVT_GROUP_NAME_AVAILABLE)		// 29443
		{
			IQDK30_GroupNameEventType *pGroupName = NULL;

			IQDK30MEMBERSHIP_GetEventPayload(me->pIQDK30Membership,AEEIID_IQDK30MEMBERSHIP,IQDK_EVT_GROUP_NAME_AVAILABLE,&pMembership);
			if (pMembership != NULL) {
				pGroupName = (IQDK30_GroupNameEventType *)pMembership;
				if (pGroupName != NULL) {
					if (pGroupName->failReason == IQDK_SUCCESS) {
						if (pGroupName->groupAddr.addrType == IQDK_ADDRTYPE_PREDEFINED_GROUP_ADDRESS) {
							//   û�� IQDK30MEMBERSHIP_GetGroupName�� �¼� IQDK_ADDRTYPE_PREDEFINED_GROUP_ADDRESS �о͵ò���groupAddr.name
							// ����ֻ��IQDK30MEMBERSHIP_GetGroupName����һ�Σ��õ�һ��groupAddr.name, ���Ҫ��groupAddr.name != 0 �ж�
							if (pGroupName->groupAddr.name != 0) {		
								int iLen = 0;
								iLen = WSTRLEN(pGroupName->groupAddr.name);
								if (iLen >0) {
									WSTRTOSTR(pGroupName->groupAddr.name,sName,sizeof(sName));
									richptt_SetReqGroupStatus(me,sName);
								}
							}
						}
					}
					FREEIF(pGroupName);
				}
				else
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK_EVT_GROUP_NAME_AVAILABLE pGroupName == NULL");
			}
		}
		FREEIF(pMembership);
		return TRUE;
	}

	return FALSE;

}

// ======================================================================
// ����Ԥ������ĺ���״̬,�����ǰ�Խ������ȼ���������ĶԽ������˳������ȼ�����������ȼ��ĶԽ�
// �������
//  name : ����ĶԽ���Ԥ��������
static void richptt_SetReqGroupStatus(crichptt *me,char *name)
{
	int idx = 0;
	boolean bFind = FALSE;
	newgimis *pMe = me->m_pParent;
	
	if (!name) return;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),
		            "_SetGroupStatus in,name=%s,GroupList count=%u,idxCalling=%d,idxCallWait=%d,idxCallReq=%d",
	                name,me->pGroupList->count,me->idxCalling,me->idxCallWait,me->idxCallReq);
	// ���ݻ�û��ͬ������

	for(idx =0; idx < me->pGroupList->count; idx++)
	{
		if (STRCMP(name,me->pGroupList->GroupInfo[idx].sName) == 0) {
			bFind = TRUE;
			MEMCPY(me->pGroupList->GroupInfo[idx].ConfId,me->ConnConfID,sizeof(IQDKConfIdType));

			if (richptt_HasStatus(me,QCHAT_NMASK_CONNECT)==QCHAT_NMASK_CONNECT) {	// -- ���гɹ�
				// �����ӳɹ�ʱ�������ͷŻ���Ȩ���������������Ȩ��������
				ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_RELEASEFLOOR_EVENT,(uint16)0, 0);

				me->pGroupList->GroupInfo[idx].cStatus = 1;
				me->idxCalling = idx;
				// ����Ѿ��еȴ����е����
				if (me->idxCallWait >=0) 
				{	// idxCallWait
					if (me->idxCallWait == idx) {
						me->idxCallWait = -1;
						me->nCallCnt = 0;
						return;
					}
					if (me->pGroupList->GroupInfo[me->idxCallWait].npriority > me->pGroupList->GroupInfo[idx].npriority)
					{
						if (me->idxCallReq >= 0) {	// if -- 12
							if (me->pGroupList->GroupInfo[me->idxCallWait].npriority > me->pGroupList->GroupInfo[me->idxCallReq].npriority)	{
							    me->idxCallReq = me->idxCallWait;
							}
							me->idxCallWait = -1;
						}	// if -- 12
						else {
							me->idxCallReq = me->idxCallWait;
							me->idxCallWait = -1;
						}
						me->nCallCnt = 0;
						ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_ENDCALL_EVENT,(uint16)0, 0);
					}
					else
						me->idxCallWait = -1;
					return;
				}	// idxCallWait
				if (me->idxCallReq >=0)  {	// idxCallReq	
					if (me->pGroupList->GroupInfo[me->idxCallReq].npriority > me->pGroupList->GroupInfo[idx].npriority)
					{
						me->nCallCnt = 0;
						ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_ENDCALL_EVENT,(uint16)0, 0);
						return;
					}
					else
						me->idxCallReq = -1;
					return;
				} // idxCallReq	
				// ����Ѿ��еȴ����е����

				// �����ڵȴ������
				return;
			}	// -- ���гɹ�
			else {	// -- 2
				me->pGroupList->GroupInfo[idx].cStatus = 2;
				if (me->idxCallWait == idx) {	// -- 21
					me->idxCallWait = -1;
					me->idxCallReq = idx;
					richptt_WaitToCall(me);
					return;
				}	// -- 21

			}	// -- 2

			break;
		}
	}
	
	if (!bFind) return;  // ���鲻���Զ����б���

// ========== �������������ʧ�ܵ���� ===============
	// �Ѿ���һ�����������(���ȼ��϶��������ڶԽ���)
	if (me->idxCallReq >= 0)  {
		if (me->pGroupList->GroupInfo[me->idxCallReq].npriority < me->pGroupList->GroupInfo[idx].npriority) 
			me->idxCallReq = idx;
		return;
	}

	// me->idxCalling �� me->idxCallWait
	if (me->idxCalling >= 0) 
		if (me->pGroupList->GroupInfo[me->idxCalling].npriority < me->pGroupList->GroupInfo[idx].npriority) 
		{
			STRCPY(me->pGroupList->GroupInfo[idx].originator,me->originatorName);
			me->idxCallReq = idx;
			// �˳�ԭ���ĺ��У������µĺ���
			me->nCallCnt = 0;
			ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_ENDCALL_EVENT,(uint16)0, 0);
			return;
		}
    
    if (me->idxCallWait > 0)
		if (me->pGroupList->GroupInfo[me->idxCallWait].npriority < me->pGroupList->GroupInfo[idx].npriority) 
		{
			STRCPY(me->pGroupList->GroupInfo[idx].originator,me->originatorName);
			me->idxCallReq = idx;
			me->idxCallWait = -1;
			// �˳�ԭ���ĺ��У������µĺ���
			me->nCallCnt = 0;
			ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_ENDCALL_EVENT,(uint16)0, 0);
			return;
		}
	
	return;
}

// ======================================================================
// ����confId����Ԥ������ĺ���״̬,�����ǰ�Խ������ȼ���������ĶԽ������˳������ȼ�����������ȼ��ĶԽ�
// �������
//  confId : 
static boolean richptt_SetReqGroupStatusWithConfID(crichptt *me,IQDKConfIdType  confId,IQDK_RejectReasonType rejectReason)
{
	int iExitLoc = 0;
    boolean bEndCall = FALSE;
	boolean bGetName = FALSE;
	int idx = 0;
	boolean bFind = FALSE;		// �ҵ����е�Ԥ������
	newgimis *pMe = me->m_pParent;
	
	if (!confId) return FALSE;
	
	{
		char sAscConfId[IQDK_CONF_ID_SIZE*3+1];
		ud_ToAscFmt((char *)confId,IQDK_CONF_ID_SIZE,sAscConfId,sizeof(sAscConfId));
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),
			"_SetGroupStatusWithConfID in,confID=%s,GroupList count=%u,idxCalling=%d,idxCallWait=%d,idxCallReq=%d,call status=%d",
			sAscConfId,me->pGroupList->count,me->idxCalling,me->idxCallWait,me->idxCallReq,richptt_HasStatus(me,QCHAT_NMASK_CONNECT));
	}

	for(idx =0; idx < me->pGroupList->count; idx++)
	{
		if (MEMCMP(confId,me->pGroupList->GroupInfo[idx].ConfId,sizeof(IQDKConfIdType)) == 0) {
			bFind = TRUE;

			if (richptt_HasStatus(me,QCHAT_NMASK_CONNECT)==QCHAT_NMASK_CONNECT) {	// -- ���ӳɹ�
				// �����ӳɹ�ʱ�������ͷŻ���Ȩ���������������Ȩ��������
				ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_RELEASEFLOOR_EVENT,(uint16)0, 0);

				if ( STRCMP(me->pGroupList->GroupInfo[idx].originator, me->MyMobileNo) == 0 ) 
					me->nMyCallCnt = 0;

				me->pGroupList->GroupInfo[idx].cStatus = 1;
				me->idxCalling = idx;
				// ����Ѿ��еȴ����е����
				if (me->idxCallWait >=0) 
				{	// idxCallWait
					if (me->idxCallWait == idx) {
						iExitLoc = 1;
 						// �����ͬʱ������У������˳ɹ���
 						if (STRCMP(me->originatorName,me->MyMobileNo) != 0 ) 
 							iExitLoc = 11;
						me->idxCallWait = -1;
						me->nCallCnt = 0;
						goto ExitHere;  //return bFind;
					}
					if (me->pGroupList->GroupInfo[me->idxCallWait].npriority > me->pGroupList->GroupInfo[idx].npriority)
					{
						if (me->idxCallReq >= 0) {	// if -- 12
							if (me->pGroupList->GroupInfo[me->idxCallWait].npriority > me->pGroupList->GroupInfo[me->idxCallReq].npriority)	{
							    me->idxCallReq = me->idxCallWait;
							}
							me->idxCallWait = -1;
						}	// if -- 12
						else {
							me->idxCallReq = me->idxCallWait;
							me->idxCallWait = -1;
						}
						me->nCallCnt = 0;
						ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_ENDCALL_EVENT,(uint16)0, 0);
						bEndCall = TRUE;
					}
					else
						me->idxCallWait = -1;

					iExitLoc = 2;
					goto ExitHere;  // return bFind;
				}	// idxCallWait
				if (me->idxCallReq >=0)  {	// idxCallReq	
					if (me->pGroupList->GroupInfo[me->idxCallReq].npriority > me->pGroupList->GroupInfo[idx].npriority)
					{
						me->nCallCnt = 0;
						ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_ENDCALL_EVENT,(uint16)0, 0);
						bEndCall = TRUE;
					}
					else {
						if (me->idxCallReq == idx) {	// Add 2013-03-20 �൱�ڱ����з��Ѿ��˳������з��ְ���һ��PTT�����ú�ͨ
							if ( STRCMP(me->originatorName,me->MyMobileNo) == 0 )
								if (me->pGroupList->cGrpCallSts < 1)
									 me->pGroupList->cGrpCallSts = 1;
						}	// Add End
						me->idxCallReq = -1;
					}
					iExitLoc = 3;
					goto ExitHere;  // return bFind;
				} // idxCallReq	
				// ����Ѿ��еȴ����е����

				// �����ڵȴ������
				iExitLoc = 4;

				goto ExitHere;  // return bFind;
			}	// -- ���гɹ�
			else {	// -- 2
				me->pGroupList->GroupInfo[idx].cStatus = 2;
				if (me->idxCallWait == idx) {	// -- 21
					if (STRCMP(me->originatorName,me->MyMobileNo) == 0 ) {
						me->idxCallWait = -1;
						me->idxCallReq = idx;
						ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_CALLAGAIN_EVENT,(uint16)0, 0);
						iExitLoc = 5;
					}
					else
					   iExitLoc = 15;
					goto ExitHere;  // return bFind;
				}	// -- 21

			}	// -- 2
			break;
		}
	}
	
	if (!bFind) {
		iExitLoc = 6;
		goto ExitHere;  //	return FALSE;  // ���鲻���Զ����б���
	}

// ========== �������������ʧ�ܵ���� ===============
	// �Ѿ���һ�����������(���ȼ��϶��������ڶԽ���)
	if (me->idxCallReq >= 0)  {  // 7
		if (me->idxCallReq == idx) {
			if (STRCMP(me->originatorName,me->MyMobileNo) == 0 ) {
				me->idxCallWait = -1;
				me->idxCallReq = idx;
				ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_CALLAGAIN_EVENT,(uint16)0, 0);
				iExitLoc = 17;
				goto ExitHere;
			}
		}
		else if (me->pGroupList->GroupInfo[me->idxCallReq].npriority < me->pGroupList->GroupInfo[idx].npriority) 
			me->idxCallReq = idx;
		
		iExitLoc = 7;
		goto ExitHere;  
	}  // 7

	// me->idxCalling �� me->idxCallWait
	if (me->idxCalling >= 0) {
		if (me->pGroupList->GroupInfo[me->idxCalling].npriority < me->pGroupList->GroupInfo[idx].npriority) 
		{
			bGetName = TRUE;
			STRCPY(me->pGroupList->GroupInfo[idx].originator,me->originatorName);

			me->idxCallReq = idx;
			// �˳�ԭ���ĺ��У������µĺ���
			me->nCallCnt = 0;
			ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_ENDCALL_EVENT,(uint16)0, 0);
			bEndCall = TRUE;
			iExitLoc = 8;
			goto ExitHere;  // return bFind;
		}
    }

    if (me->idxCallWait > 0) {
		if (me->pGroupList->GroupInfo[me->idxCallWait].npriority < me->pGroupList->GroupInfo[idx].npriority) 
		{
			bGetName = TRUE;
			STRCPY(me->pGroupList->GroupInfo[idx].originator,me->originatorName);

			me->idxCallReq = idx;
			me->idxCallWait = -1;
			// �˳�ԭ���ĺ��У������µĺ���
			me->nCallCnt = 0;
			ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_ENDCALL_EVENT,(uint16)0, 0);
			bEndCall = TRUE;
			iExitLoc = 9;
			goto ExitHere;  // return bFind;
		}
	}

	if ( (rejectReason == IQDK_REJECT_ALL_TARGETS_REJECT) || (rejectReason == IQDK_REJECT_TARGETS_NOT_REACHABLE) )
	{
		if ( STRCMP(me->originatorName,me->MyMobileNo) == 0 )
		{
			me->idxCallReq = idx;
			me->idxCallWait = -1;
			bGetName = TRUE;
			STRCPY(me->pGroupList->GroupInfo[idx].originator,me->originatorName);
			me->nCallCnt = 0;
			// Add 2013-03-20
			if (me->pGroupList->cGrpCallSts < 1)
				me->pGroupList->cGrpCallSts = 1;

			ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_CALLAGAIN_EVENT,(uint16)0, 0);
			iExitLoc = 12;
			goto ExitHere;
		}
	}

	iExitLoc = 10;


ExitHere:

	{
		char sTmp[QDK_CALL_NAME_LEN+1] = "null";
		if (bFind)
			if (me->pGroupList->GroupInfo[idx].originator != NULL)
				STRCPY(sTmp,me->pGroupList->GroupInfo[idx].originator);

		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),
			"_SetGroupStatusWithConfID out,bFind=%d,iExitLoc=%d,bEndCall=%d,bGetName=%d,orginator=%s,idxCalling=%d",
			bFind,iExitLoc,bEndCall,bGetName,sTmp,me->idxCalling);

	}

		return bFind;
}


// ================================================================================
// ͨ���ص������������
static void richptt_cbStartCall(crichptt *me)
{
	newgimis *pMe = me->m_pParent;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_cbStartCall in,request group=%d",me->idxCallReq);	
	
	if (me->idxCallReq >= 0) {
		richptt_GroupCall(me,me->idxCallReq);
		me->idxCallWait = me->idxCallReq;				
		me->idxCallReq = -1;
	}
	
	return;
}

// ================================================================================
//  �ֻ�������01��Ԥ�������
//  int iLoc : �Զ����Ԥ�������б�����
static void richptt_GroupCall(crichptt *me,int iLoc)
{
	newgimis *pMe;

	if (!me) return;
	pMe = me->m_pParent;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_GroupCall in,request mobile type=%s",pMe->sMobileType);	

    if (STRCMP(pMe->sMobileType,"01") == 0 /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/)	
    {	// ����
     	richptt_GroupCall_01(me,iLoc);
     	return;	
    }

	// default ֱ������qdk����
	richptt_GroupCall_xx(me,iLoc);
	return;
}

// ================================================================================
//  �ֻ�������01��Ԥ�������
//  int iLoc : �Զ����Ԥ�������б�����
static void richptt_GroupCall_01(crichptt *me,int iLoc)
{
	newgimis *pMe;
    SpeedDialNum Number = {0};
	
	if (!me) return;
	pMe = me->m_pParent;
	if ( (iLoc<0) || (iLoc>=me->pGroupList->count) ) return;

	STRTOWSTR(me->pGroupList->GroupInfo[iLoc].sName,Number.pSpeedNum, sizeof(Number.pSpeedNum));
	Number.speeddial_type = SPEEDDIAL_QCHAT_PREDEFINEDCALL;
	Number.AdHoc_id = 0;
	
	ISHELL_SendEvent (pMe->a.m_pIShell,AEECLSID_QCHATUI,EVT_QCHAT_CALL,
	                   QCHATCALL_EVENT_START_GROUP_CALL_BY_NAME, (uint32)&Number);


	return;
}

// ================================================================================
//  ͨ��Ԥ�������
//  int iLoc : �Զ����Ԥ�������б�����
static void richptt_GroupCall_xx(crichptt *me,int iLoc)
{
	newgimis *pMe;
	IQDK_CommonCallPrefs commonPrefs;
	IQDK_PredefinedGroupCallPrefs  predGroupCallPref;
	IQDK30_CallPreferences callPrefs;
	IQDKConfIdList confIdList;
	
	if (!me) return;
	pMe = me->m_pParent;
	
	if ( (iLoc<0) || (iLoc>=me->pGroupList->count) ) return;
	
	if(richptt_HasStatus(me,QCHAT_NMASK_CALL)==QCHAT_NMASK_CALL)  // if already in a call print out a  msg and exit
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_StartCall_xx in,Already in a call ...");
		return;
	}
	
	confIdList.count = 1;
	confIdList.confId = &(me->pGroupList->GroupInfo[iLoc].ConfId);
	{
		char sAscConfId[IQDK_CONF_ID_SIZE*3+1];
		ud_ToAscFmt((char *)confIdList.confId,IQDK_CONF_ID_SIZE,sAscConfId,sizeof(sAscConfId));
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_StartCall_xx in, ready to call,confid=%s,index=%d",sAscConfId,iLoc);
	}
	
	// User-defined preferences
	commonPrefs.preferredAddrIndex = 1;  
	commonPrefs.waitForPTTPress    = FALSE;
	
	predGroupCallPref.addMemberDisableOption =  0;
	predGroupCallPref.callTerminationOption = IQDK_CALL_TERMINATE_AFTER_PARTICIPANTS_LEAVE;   // IQDK_CALL_TERMINATE_IMMEDIATELY;
	predGroupCallPref.startGroupIfNotRunning = TRUE;
	
	predGroupCallPref.common = commonPrefs;
	callPrefs.predefinedGroupCallPrefs = predGroupCallPref;
	
	IQDK30CALL_StartCall(me->pIQDKCall, IQDK_PREDEFINED_GROUP_CALL, NULL,&confIdList, &callPrefs);
	
	
	return;
}


// static void richptt_GroupCall_xx(crichptt *me,int iLoc)
// {
// 	char *sName = NULL;
// 	newgimis *pMe;
// 	IQDK_CommonCallPrefs commonPrefs;
// 	IQDK_PredefinedGroupCallPrefs  predGroupCallPref;
// 	IQDK30_CallPreferences callPrefs;
// 
// 	if (!me) return;
// 	pMe = me->m_pParent;
// 
// 	if ( (iLoc<0) || (iLoc>=me->pGroupList->count) ) return;
// 	
// 	if(richptt_HasStatus(me,QCHAT_NMASK_CALL)==QCHAT_NMASK_CALL)  // if already in a call print out a  msg and exit
// 	{
// 		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_StartCall_xx in,Already in a call ...");
// 		return;
// 	}
// 	
// 	sName = STRDUP(me->pGroupList->GroupInfo[iLoc].sName);
// 	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_StartCall_xx in, ready to call,name=%s,index=%d",sName,iLoc);
// 	
// 	if(me->addrList)
// 	{   
// 		FREEIF(me->addrList->addresses->name);
// 		FREEIF(me->addrList->addresses->domain);
// 		FREEIF(me->addrList->addresses);
// 		FREEIF(me->addrList);
// 	}
// 	if(me->addrData)
// 	{
// 		FREEIF(me->addrData->name);
// 		FREEIF(me->addrData->domain);
// 		FREEIF(me->addrData);
// 	}
// 
// 	me->addrData = uf_CreateAddressData(pMe,(const char**)&sName,1,me->sGrpDomain,	IQDK_ADDRTYPE_PREDEFINED_GROUP_ADDRESS );
// 
// 	me->addrList= (IQDKAddressListType*)MALLOC(sizeof(IQDKAddressListType));
// 	me->addrList->addresses= (IQDKAddressData*)MALLOC(sizeof(IQDKAddressData));
// 	me->addrList->addresses->name = (AECHAR*)MALLOC(sizeof(AECHAR)*(WSTRLEN(me->addrData->name)+1));
// 	me->addrList->addresses->domain = (AECHAR*)MALLOC(sizeof(AECHAR)*(WSTRLEN(me->addrData->domain)+1));
// 	me->addrList->count=1;
// 	WSTRCPY(me->addrList->addresses->name,me->addrData->name);
// 	WSTRCPY(me->addrList->addresses->domain,me->addrData->domain);
// 	me->addrList->addresses->addrType= me->addrData->addrType; 
// 	
// 	// User-defined preferences
// 	commonPrefs.preferredAddrIndex = 1;  
// 	commonPrefs.waitForPTTPress    = FALSE;
// 	
// 	predGroupCallPref.addMemberDisableOption =  0;
// 	predGroupCallPref.callTerminationOption =  IQDK_CALL_TERMINATE_IMMEDIATELY;
// 	predGroupCallPref.startGroupIfNotRunning = TRUE;
// 	
// 	predGroupCallPref.common = commonPrefs;
// 	callPrefs.predefinedGroupCallPrefs = predGroupCallPref;
// 	
// 	IQDK30CALL_StartCall(me->pIQDKCall, IQDK_PREDEFINED_GROUP_CALL, me->addrList, NULL, &callPrefs);
// //	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_StartCall_xx in, after call");
// 
// 	//  IQDK30CALL_StartCall(pMe->pIQDKCall, IQDK_PREDEFINED_GROUP_CALL, NULL,&confIdList, &callPrefs);
// 	//  FREEIF(confIdList.confId);
// 	
// 	if(me->addrData)
// 	{
// 		FREEIF(me->addrData->name);
// 		FREEIF(me->addrData->domain);
// 		FREEIF(me->addrData);
// 	}
// 	if(me->addrList)
// 	{
// 		FREEIF(me->addrList->addresses->domain);
// 		FREEIF(me->addrList->addresses->name);
// 		FREEIF(me->addrList->addresses);
// 		FREEIF(me->addrList);
// 	}
// 	
// 	FREEIF(sName);
// 	
// 	return;
// }

// ===================================================================
// �ͷŻ���Ȩ
static void richptt_ReleaseFloor(crichptt *me)
{
	IQDK30CALL_ReleaseFloor(me->pIQDKCall);
}

// ===================================================================
// ���뻰��Ȩ
static void richptt_RequestFloor(crichptt *me)
{
	newgimis *pMe = me->m_pParent;
	IQDK30CALL_RequestFloor(me->pIQDKCall);
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_RequestFloor in");

}

// ===================================================================
// �������� 
static void richptt_EndCall(crichptt *me)
{
	newgimis *pMe = me->m_pParent;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_EndCall in, cStatus=%x,GroupList count=%u,idxCalling=%d,idxCallWait=%d,idxCallReq=%d",
		       richptt_HasStatus(me,QCHAT_NMASK_CALL),me->pGroupList->count,me->idxCalling,me->idxCallWait,me->idxCallReq);
	
    if(richptt_HasStatus(me,QCHAT_NMASK_CALL) == QCHAT_NMASK_CALL)  // if already in  a call
	{
		if (me->pGroupList->cGrpCallSts != 0) {		// 2013-03-20
			me->pGroupList->cGrpCallSts = 0;
			me->pGroupList->iGrpCallTime = 0;
		}  // Add End
		IQDK30CALL_EndCall(me->pIQDKCall);
	}
    else   // if not in a call
		DBGPRINTF("Not in call");
	
    if(me->addrList)
    {   
		FREEIF(me->addrList->addresses->name);
		FREEIF(me->addrList->addresses->domain);
		FREEIF(me->addrList->addresses);
		FREEIF(me->addrList);
    }
    if(me->addrData)
    {
		FREEIF(me->addrData->name);
		FREEIF(me->addrData->domain);
		FREEIF(me->addrData);
    }
	
	
}
//����Membership�ӿ�
static boolean QChatManager_InitMembership(crichptt *me)
{
	newgimis *pMe = me->m_pParent;
    IQDK30Membership  *pMembership = NULL;		//  IQDK30Provision *pIQDKProvision = NULL;
	//   Qchat_Prov_Entry mkey={0};
    int nErr = EFAILED;
	int nResult;
    
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"QChatManager_InitMembership in");	
	
	if (me->pIQDK30Membership != NULL)
		return TRUE;
	
	nResult = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IQDKMEMBERSHIP,  (void **)&me->pIQDK30Membership);
    if ((SUCCESS != nResult) || (NULL == me->pIQDKProvision))   {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"create IQDK30Membership failed: %u", nResult);
        return FALSE;
    }
	
	nErr = IQDK30MEMBERSHIP_QueryInterface(me->pIQDK30Membership,AEEIID_IQDK30MEMBERSHIP,(void**)&pMembership);
    if (SUCCESS == nErr)
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK30MEMBERSHIP_QueryInterface success");
    else  {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK30MEMBERSHIP_QueryInterface failed: %u", nErr);
        return FALSE;
    }
	
    IQDK30MEMBERSHIP_Release(me->pIQDK30Membership);
	
	nErr=IQDK30MEMBERSHIP_Init(pMembership,pMe->a.clsID);
    if (SUCCESS == nErr) {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK30MEMBERSHIP_Init success");	
		me->pIQDK30Membership = pMembership;
	}
    else {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDK30MEMBERSHIP_Init failed: %u", nErr);
		IQDK30MEMBERSHIP_Release(pMembership); 
        return FALSE;
    }
	
	return TRUE;
}

// ==========================================================
// ��ʼ��QDK�Ľӿ�
static boolean richptt_InitQDKInterface(crichptt *me)
{
	newgimis *pMe = me->m_pParent;
	boolean bRtn = FALSE;
    int   nErr = SUCCESS;
	
	// create instance of IQDKManager interface
	if (!me->pIQDKManager){
		nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IQDKMANAGER,(void **)&me->pIQDKManager);
		if (nErr != SUCCESS) {
			DBGPRINTF("zjie-nErr=%d,IQDKManager instance error", nErr);
			goto ExitHere;
		}
	}
	
	if(!me->pIQDKCall) {
		// create instance of IQDKCall interface
		nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IQDKCALL,  (void **)&me->pIQDKCall);
		if (nErr != SUCCESS) {
			DBGPRINTF("zjie-nErr=%d,create IQDKCall instance error", nErr);
			goto ExitHere;
		}
	}
	
	QChatManager_InitMembership(me);
	
	IQDK30CALL_Init(me->pIQDKCall, pMe->a.clsID);	
	IQDK30MANAGER_Init(me->pIQDKManager, pMe->a.clsID);
	ISHELL_RegisterNotify(pMe->a.m_pIShell, pMe->a.clsID, AEECLSID_IQDKNOTIFIER,NMASK_QDK_MANAGER | NMASK_QDK_CALL| NMASK_QDK_MEMBERSHIP );
	
	me->commonPrefs.preferredAddrIndex = 0;
	me->commonPrefs.waitForPTTPress = FALSE;
	me->directPref.addMemberDisableOption = 0;
	me->directPref.callTerminationOption = IQDK_CALL_TERMINATE_IMMEDIATELY;
	bRtn = TRUE;
	
	// �йغ�������
	if (!me->pIQDK30CallRestriction){                       
		nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_IQDKCALLRESTRICTION,(void **)&me->pIQDK30CallRestriction);
		if (nErr != SUCCESS) {
			DBGPRINTF("zjie-nErr=%d,IQDKCallRestriction instance error", nErr);
			goto ExitHere;
		}
	}
	nErr=IQDK30CALLRESTRICTION_Init(me->pIQDK30CallRestriction,pMe->a.clsID);
    if (SUCCESS == nErr) {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDKCALLRESTRICTION_Init success");	
	}
    else {
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"IQDKCALLRESTRICTION_Init failed: %u", nErr);
		IQDK30CALLRESTRICTION_Release(me->pIQDK30CallRestriction); 
        return FALSE;
    }


ExitHere:
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_InitQDKInterface out, return=%d",bRtn);
	return (bRtn);
	
}

// =============================================================
// ע��QDK�ӿ�
static void richptt_UnRegisterIQDKInterface(crichptt *me)
{
	newgimis *pMe = me->m_pParent;
	int nErr;
	uint32 dwMask = 0;	
	
	nErr = ISHELL_RegisterNotify(pMe->a.m_pIShell, pMe->a.clsID, AEECLSID_IQDKNOTIFIER, dwMask);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_UnRegisterIQDKEvent in,return=%d",nErr);
	
	if(me->pIQDKCall)
		IQDK30CALL_EndCall(me->pIQDKCall);
	
	if(me->pIQDKManager) {
		IQDK30MANAGER_Release(me->pIQDKManager);
		me->pIQDKManager = NULL;
	}

	if (me->pIQDK30Membership) {
		IQDK30MEMBERSHIP_Release(me->pIQDK30Membership);
		me->pIQDK30Membership = NULL;
	}

	if(me->pIQDKCall) {
		IQDK30CALL_Release(me->pIQDKCall);
		me->pIQDKCall = NULL;
	}
	
	if (me->pIQDK30CallRestriction) {
		IQDK30CALLRESTRICTION_Release(me->pIQDK30CallRestriction);
		me->pIQDK30CallRestriction = NULL;
	}

	return;

}
// ====================================================================
// �ͷ��Զ��������Ϣ
static void richptt_FreeGroup(crichptt *me)
{
	newgimis *pMe = me->m_pParent;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_FreeGroup in, group count=%d",me->pGroupList->count);

	if (me->pGroupList->count >0)
	{
		FREEIF(me->pGroupList->GroupInfo);
		me->pGroupList->count = 0;
		me->pGroupList->cGrpCallSts = 0;
	}
	
}

// �ͷ�һ�����е���Ϣ
static void richptt_FreePttCallNo(crichptt *me)
{
	newgimis *pMe = me->m_pParent;
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_FreePttCallNo in, PttCallNo count=%d",me->pPttCallGroup->count);
	
	if (me->pPttCallGroup->count >0)
	{
		FREEIF(me->pPttCallGroup->PttCallNoGroup);
		me->pPttCallGroup->count = 0;
	}
	
}

// =================================================================================
// ��������
// char *sGroupInfo : ����, ��ʽ�� ����(1λ)+���� 
//        ������      2ptt.189.cn  = ����������ptt.189.cn
//					  3group.ptt.189.cn = Ԥ������� ���� group.ptt.189.cn
void richptt_SetDomain(crichptt *me, char *sGroupInfo)
{
	byte sTmp[DOMAIN_LEN*2+1];
	newgimis *pMe;
	int iLen = 0;

	if (!me) return;

	pMe = me->m_pParent;

	iLen = STRLEN(sGroupInfo)-1;
	MEMSET(sTmp,0,sizeof(sTmp));
	MEMCPY(sTmp,sGroupInfo+1,iLen);

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"QB[%c]=%s,iLen=%d,iLen1=%d",sGroupInfo[0],sGroupInfo,iLen);

	switch (sGroupInfo[0])
	{
	case '2':
		MEMSET(me->sPttDomain,0,sizeof(me->sPttDomain));
		UTF8TOWSTR(sTmp,iLen,me->sPttDomain,sizeof(me->sPttDomain));
		break;
	case '3':
		MEMSET(me->sGrpDomain,0,sizeof(me->sGrpDomain));
		UTF8TOWSTR(sTmp,iLen,me->sGrpDomain,sizeof(me->sGrpDomain));
		break;
	}
	richptt_WriteQchatSetting(me);

	return;
}

// ==================================================================
// ����Ԥ���������ȼ�
// char *sGroupInfo : Ԥ���������Ϣ
//					��ʽ�����ȼ�(2λ)��ID,��ʽ�����ȼ�(2λ)��ID�����ֻ��һ���飬��Ҫ�ӡ�,���š�
//                        ���ȼ��Ӹߵ�������
void richptt_SetUsrGroup(crichptt *me,char *sGroupInfo)
{
	char sTmp[QDK_CALL_NAME_LEN+QDK_GROUP_PRIORITY+1];
	newgimis *pMe;
    int idx = 0;  
	int iLen = 0;
	int iLocNext;
	boolean bFind;

	if (!me) return;

	pMe = me->m_pParent;

	ud_Trim(sGroupInfo);
	iLen = STRLEN(sGroupInfo);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_SetUsrGroup in,Group len=%d",iLen);
	if (iLen == 0) {
		richptt_FreeGroup(me);
		return;
	}

	richptt_GetGroupInfo(me);

	iLen = 0;
	iLocNext = -1;
	while(TRUE)
	{	// while 
	   char sPri[3];
	   iLocNext += iLen + 1;	// ��','�ĳ���
	   if ( iLocNext >= (int)STRLEN(sGroupInfo)) 
		   break;
	   iLen = ud_DistToSplit( sGroupInfo+iLocNext, DEFAULT_SPLIT );
	   if (iLen <= 0) break;

	   MEMSET(sTmp,0,sizeof(sTmp));
	   MEMCPY(sTmp,sGroupInfo+iLocNext, iLen);
	   bFind = FALSE;	
	   for(idx = 0; idx < me->pGroupList->count;idx++)	
	   {	// for
		   if ( STRCMP(me->pGroupList->GroupInfo[idx].sName,sTmp+2) == 0 )	{
				me->pGroupList->GroupInfo[idx].cStatus = 0;
				MEMSET(sPri,0,sizeof(sPri));
				MEMCPY(sPri,sTmp,2);
				me->pGroupList->GroupInfo[idx].npriority = (unsigned char)ATOI(sPri);
				bFind = TRUE;
				break;
		   }
	   }	// for
	   
	   if (!bFind)
	   {
			me->pGroupList->GroupInfo = (IUsrPredGroup *)REALLOC(me->pGroupList->GroupInfo, (me->pGroupList->count+1)*sizeof(IUsrPredGroup));
			MEMSET(me->pGroupList->GroupInfo+me->pGroupList->count,0,sizeof(IUsrPredGroup));
			me->pGroupList->GroupInfo[idx].cStatus = 0;
			MEMSET(sPri,0,sizeof(sPri));
			MEMCPY(sPri,sTmp,2);
			me->pGroupList->GroupInfo[idx].npriority = (unsigned char)ATOI(sPri);
			STRCPY(me->pGroupList->GroupInfo[idx].sName,sTmp+2);
			me->pGroupList->count++; 			
	   }
	}	// while

 	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"count=%d",me->pGroupList->count); 
  	for (idx = 0; idx < me->pGroupList->count; idx++)
  	{
		char sAscConfId[IQDK_CONF_ID_SIZE*3+1];
		ud_ToAscFmt((char *)me->pGroupList->GroupInfo[idx].ConfId,IQDK_CONF_ID_SIZE,sAscConfId,sizeof(sAscConfId));
  		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"confId=%s,sName=%s,pri=%d",
  			         sAscConfId,me->pGroupList->GroupInfo[idx].sName, me->pGroupList->GroupInfo[idx].npriority );
  	}

	return;

}

//   ������ԭ��������				void richptt_SetUsrGroup(crichptt *me,char *sGroupInfo)
// 				{
// 					char sTmp[QDK_CALL_NAME_LEN+QDK_GROUP_PRIORITY+1];
// 					newgimis *pMe;
// 					int idx = 0;  
// 					int iLen = 0;
// 					int iLocNext;
// 					
// 					if (!me) return;
// 					
// 					pMe = me->m_pParent;
// 					
// 					ud_Trim(sGroupInfo);
// 					iLen = STRLEN(sGroupInfo);
// 					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_SetUsrGroup in,Group len=%d",iLen);
// 					
// 					if (iLen == 0) return;
// 					
// 					richptt_FreeGroup(me);
// 					
// 					idx = 0;  
// 					iLen = 0;
// 					iLocNext = -1;
// 					while(TRUE)
// 					{	// while 
// 						char sPri[3];
// 						iLocNext += iLen + 1;	// ��','�ĳ���
// 						if ( iLocNext >= (int)STRLEN(sGroupInfo)) 
// 							break;
// 						iLen = ud_DistToSplit( sGroupInfo+iLocNext, DEFAULT_SPLIT );
// 						if (iLen <= 0) break;
// 						
// 						if (iLen > 0) {	
// 							MEMSET(sTmp,0,sizeof(sTmp));
// 							MEMCPY(sTmp,sGroupInfo+iLocNext, iLen);
// 							
// 							if (idx == 0)
// 								me->pGroupList->GroupInfo = (IUsrPredGroup *)MALLOC(sizeof(IUsrPredGroup));
// 							else
// 								me->pGroupList->GroupInfo = (IUsrPredGroup *)REALLOC(me->pGroupList->GroupInfo, (idx+1)*sizeof(IUsrPredGroup));
// 							MEMSET(me->pGroupList->GroupInfo+idx,0,sizeof(IUsrPredGroup));
// 							
// 							me->pGroupList->GroupInfo[idx].cStatus = 0;
// 							MEMSET(sPri,0,sizeof(sPri));
// 							MEMCPY(sPri,sTmp,2);
// 							me->pGroupList->GroupInfo[idx].npriority = (unsigned char)ATOI(sPri);
// 							STRCPY(me->pGroupList->GroupInfo[idx].sName,	sTmp+2);
// 							
// 							idx++;
// 						}
// 						
// 					}	// while
// 					
// 					me->pGroupList->count = idx;		// ע�⣬�˳�ѭ��ʱ�Ѿ���1
// 					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"count=%d",me->pGroupList->count); 
// 					for (idx = 0; idx < me->pGroupList->count; idx++)
// 					{
// 						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"sName=%s,pri=%d",
// 							me->pGroupList->GroupInfo[idx].sName, me->pGroupList->GroupInfo[idx].npriority );
// 					}
// 					
// 					return;
// 					
// 				}

// =========================================================================================
// ���ɴ����е���Ϣ�б�
// const char **urlList : ������name���б�
// int count : ������name���б�ĳ���
// AECHAR *sDomain : ������name��domain
// IQDKAddressType iCallType : �������� 
static IQDKAddressData * uf_CreateAddressData(newgimis *pMe,const char **urlList, int count,AECHAR *sDomain,IQDKAddressType iCallType)
{
	IQDKAddressData *retAddresses;
	int nsUsrLen = 0, nDomainLen = 0;		// ����name�ĳ��ȣ� domain �ĳ���
	int i;
	
	if (!urlList || !count || !sDomain || !iCallType)
		return NULL;
	
	nsUsrLen = STRLEN(urlList[0]) + 1;
	nDomainLen = WSTRLEN(sDomain) + 1;
	retAddresses = MALLOC(sizeof(IQDKAddressData) * count);
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),
					   "uf_CreateAddressData in,name=%s,len=%d,nDomainLen=%d,IQDKAddressType=%d",
						urlList[0],nsUsrLen,nDomainLen,iCallType);

	if (retAddresses) {
		for(i=0; i<count; i++)
		{
			retAddresses[i].addrType = iCallType;
			
			retAddresses[i].name = (AECHAR *)MALLOC(nsUsrLen*sizeof(AECHAR));
			STRTOWSTR(urlList[i],retAddresses[i].name,nsUsrLen*sizeof(AECHAR));
 			{
 				char sTmp[100];
 				int nWStr= WSTRLEN(retAddresses[i].name);
 				WSTRTOUTF8(retAddresses[i].name,nWStr,(byte *)sTmp,sizeof(sTmp));
 				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"name=%s",sTmp);
 			}
			
			retAddresses[i].domain = (AECHAR *)MALLOC(nDomainLen*sizeof(AECHAR));
			WSTRCPY(retAddresses[i].domain,sDomain);

			{
				char sTmp[100];
				int nWStr= WSTRLEN(retAddresses[i].domain);
				WSTRTOUTF8(retAddresses[i].domain,nWStr,(byte *)sTmp,sizeof(sTmp));
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"domain=%s",sTmp);
			}

		}
	}
	else
		return NULL;

	return retAddresses;
}

// ==================================================
// �������ļ���ȡԤ��������Ϣ
static void richptt_ReadPrdGroupSetting(crichptt *me)
{
	IFileMgr *pFileMgr = NULL;
	IFile	 *pIConfigFile = NULL;
	FileInfo fiInfo;		// �ļ���Ϣ
	char    *pszBuf = NULL;
	char    *pszTok = NULL;
	int     nResult = 0;
	newgimis *pMe = me->m_pParent;	
	
	// �����ļ��ӿ�
	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS)
		goto ExitHere;
	
	if (IFILEMGR_Test(pFileMgr, SPD_CONFIG_FILE) != SUCCESS)
		goto ExitHere;
	
	pIConfigFile = IFILEMGR_OpenFile(pFileMgr, SPD_CONFIG_FILE, _OFM_READ);
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_ReadPrdGroupSetting in");
	
	if ( SUCCESS != IFILE_GetInfo( pIConfigFile, &fiInfo ) ) 
		goto ExitHere;
	
	if ( fiInfo.dwSize == 0 )
		goto ExitHere;
	
	pszBuf = MALLOC( fiInfo.dwSize );
	nResult = IFILE_Read( pIConfigFile, pszBuf, fiInfo.dwSize );
	if ( (uint32)nResult < fiInfo.dwSize ) 
		goto ExitHere;

	// Ԥ���������ȼ�
	pszTok = STRSTR( pszBuf, SPD_CONFIG_USRGROUP_STRING );
	if ( pszTok ) {
		char *pszSvr = NULL;
		pszTok = pszTok + STRLEN( SPD_CONFIG_USRGROUP_STRING );
		nResult = ud_DistToChar(pszTok,';');
		pszSvr = MALLOC( nResult+1 );
		MEMSET(pszSvr,0,nResult+1);
		MEMCPY(pszSvr,pszTok, nResult);
		richptt_SetUsrGroup(pMe->pRichptt,pszSvr);
		FREEIF(pszSvr);
	}

	
ExitHere:
	
	FREEIF( pszBuf );
	
	if (pIConfigFile != NULL)
		IFILE_Release( pIConfigFile );
	
	if (pFileMgr != NULL)
		IFILEMGR_Release( pFileMgr );
	
	return;
}

// ==================================================
// ��ȡQchat����
static void richptt_ReadQchatSetting(crichptt *me)
{
	IFileMgr *pFileMgr = NULL;
	IFile	 *pIConfigFile = NULL;
	FileInfo fiInfo;		// �ļ���Ϣ
	char    *pszBuf = NULL;
	char    *pszTok = NULL;
	int     nResult = 0;
	char    sDomain[DOMAIN_LEN*2+1];
	newgimis *pMe = me->m_pParent;	

	// �����ļ��ӿ�
	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS)
		goto ExitHere;

	 if (IFILEMGR_Test(pFileMgr, QCHAT_CONFIG_FILE) != SUCCESS)
		 goto ExitHere;

	 pIConfigFile = IFILEMGR_OpenFile(pFileMgr, QCHAT_CONFIG_FILE, _OFM_READWRITE);

	 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_ReadQchatSetting in");
		
	 if ( SUCCESS != IFILE_GetInfo( pIConfigFile, &fiInfo ) ) 
			goto ExitHere;
		
	 if ( fiInfo.dwSize == 0 )
		goto ExitHere;
		
	  pszBuf = MALLOC( fiInfo.dwSize );
	  nResult = IFILE_Read( pIConfigFile, pszBuf, fiInfo.dwSize );
	  if ( (uint32)nResult < fiInfo.dwSize ) 
		  goto ExitHere;

	  pszTok = STRSTR( pszBuf, CONFIG_GROUP_DOMAIN );
	  if ( pszTok ) {
			pszTok = pszTok + STRLEN( CONFIG_GROUP_DOMAIN );
			nResult = ud_DistToChar(pszTok,';'); 
			if (nResult != 0) {
				MEMSET(sDomain,0,sizeof(sDomain));
				MEMCPY(sDomain,pszTok, nResult);
				UTF8TOWSTR((byte *)sDomain,nResult,me->sGrpDomain,sizeof(me->sGrpDomain));
			}
	  }

	  pszTok = STRSTR( pszBuf, CONFIG_DIRECTCALL_DOMAIN );
	  if ( pszTok ) {
		  pszTok = pszTok + STRLEN( CONFIG_DIRECTCALL_DOMAIN );
		  nResult = ud_DistToChar(pszTok,';'); 
		  if (nResult != 0) {
			  MEMSET(sDomain,0,sizeof(sDomain));
			  MEMCPY(sDomain,pszTok, nResult);
			  UTF8TOWSTR((byte *)sDomain,nResult,me->sPttDomain,sizeof(me->sPttDomain));
		  }
	  }

ExitHere:

	FREEIF( pszBuf );

	if (pIConfigFile != NULL)
		IFILE_Release( pIConfigFile );

	if (pFileMgr != NULL)
		IFILEMGR_Release( pFileMgr );

		return;
}
// ================================================
// д��Qchat����
static void richptt_WriteQchatSetting(crichptt *me)
{
	IFileMgr *pFileMgr = NULL;
	IFile	 *pIConfigFile = NULL;
	char    pszBuf[501];
//	 	char    *pszBuf = NULL;
	int     nResult = 0;
	byte    sDomain[DOMAIN_LEN*2+1];
	newgimis *pMe = me->m_pParent;	

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"richptt_WriteQchatSetting in");

	// �����ļ��ӿ�
	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS)
		goto ExitHere;

	nResult = IFILEMGR_Test( pFileMgr, QCHAT_CONFIG_FILE );
	if ( SUCCESS == nResult ) 
		pIConfigFile = IFILEMGR_OpenFile(pFileMgr, QCHAT_CONFIG_FILE, _OFM_READWRITE);
	else
		pIConfigFile = IFILEMGR_OpenFile(pFileMgr, QCHAT_CONFIG_FILE, _OFM_CREATE);

    IFILE_Truncate( pIConfigFile, 0 );

	// Ԥ����������
	MEMSET(pszBuf,0,sizeof(pszBuf));
	MEMSET(sDomain,0,sizeof(sDomain));
	WSTRTOUTF8(me->sGrpDomain,WSTRLEN(me->sGrpDomain),sDomain,sizeof(sDomain));
	SPRINTF( pszBuf, CONFIG_GROUP_DOMAIN"%s;\r\n", sDomain );
//	nResult = IFILE_Write( pIConfigFile, pszBuf, STRLEN( pszBuf ) );
	if ( 0 == nResult ) 
		goto ExitHere;

	// ��������
	MEMSET(pszBuf,0,sizeof(pszBuf));
	MEMSET(sDomain,0,sizeof(sDomain));
	WSTRTOUTF8(me->sPttDomain,WSTRLEN(me->sPttDomain),sDomain,sizeof(sDomain));
	SPRINTF( pszBuf, CONFIG_DIRECTCALL_DOMAIN"%s;\r\n", sDomain );
	nResult = IFILE_Write( pIConfigFile, pszBuf, STRLEN( pszBuf ) );
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"02,nResult=%d,pszBuf=%s",nResult,pszBuf);
	if ( 0 == nResult ) 
		goto ExitHere;

ExitHere:

	if (pIConfigFile != NULL)
		IFILE_Release( pIConfigFile );

	if (pFileMgr != NULL)
		IFILEMGR_Release( pFileMgr );

	return;

}

//===========================================================================
// this function is called when your application is starting up
static boolean richptt_InitAppData(crichptt* me)
{
	newgimis *pMe = me->m_pParent;	
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richptt_InitAppData in");

	// ��ʼ���ṹ crichptt �е�ֵ
	me->pIQDKProvision = NULL;			
	me->pIQDK30Membership = NULL;
	me->addrList = NULL;
	me->pIQDK30CallRestriction = NULL;

	me->cStatus = 0;
	me->nCallCnt = 0;
	me->nMyCallCnt = 0;
	me->idxCalling = me->idxCallWait = me->idxCallReq = -1;
	me->pGroupList = (IUsrPredGroupList *)MALLOC(sizeof(IUsrPredGroupList));
	me->pGroupList->cGrpCallSts = 0;
	me->pGroupList->count = 0;
	me->pGroupList->GroupInfo = NULL;
	// �й�һ������
//	me->cPttCallSts = 0;
	me->pPttCallGroup = (IcallGroupList *)MALLOC(sizeof(IcallGroupList));
	me->pPttCallGroup->count = 0;
	me->pPttCallGroup->nCallId = me->pPttCallGroup->count;
	me->pPttCallGroup->PttCallNoGroup = NULL;
	// �йغڰ�����
	me->pCallRestList = (ICallRestrictionList *)MALLOC(sizeof(ICallRestrictionList));
	me->pCallRestList->count = 0;
	me->pCallRestList->CallRestInfo = NULL;

	MEMSET(me->ConnConfID,0,sizeof(IQDKConfIdType));
	MEMSET(me->MyMobileNo,0,sizeof(me->MyMobileNo));
	MEMSET(me->originatorName,0,sizeof(me->originatorName));

	MEMSET(me->sGrpDomain,0,sizeof(me->sGrpDomain));
	STRTOWSTR(RICH_GROUP_DOMAIN,me->sGrpDomain,sizeof(me->sGrpDomain));
	MEMSET(me->sPttDomain,0,sizeof(me->sPttDomain));
	STRTOWSTR(RICH_PTT_DOMAIN,me->sPttDomain,sizeof(me->sPttDomain));
	richptt_ReadQchatSetting(me);
	{
		char sPtt[DOMAIN_LEN*2+1]; 		char sGroup[DOMAIN_LEN*2+1];
		MEMSET(sPtt,0,sizeof(sPtt));	MEMSET(sGroup,0,sizeof(sGroup));
		WSTRTOUTF8( me->sGrpDomain,WSTRLEN(me->sGrpDomain),(byte *)sGroup,sizeof(sGroup) );   
		WSTRTOUTF8( me->sPttDomain,WSTRLEN(me->sPttDomain),(byte *)sPtt,sizeof(sPtt) );   
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "sPttDomain=%s,sGrpDomain=%s", sPtt,sGroup);
	}

	CALLBACK_Init( &me->cbGen, richptt_cbStartCall, me );

	
    return TRUE;
}

//===========================================================================
// this function is called when your application is exiting
static void richptt_FreeAppData(crichptt* me)
{
	newgimis *pMe = me->m_pParent;	
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "richptt_FreeAppData in");
	
	
	richptt_UnRegisterIQDKInterface(me);

 	CALLBACK_Cancel( &me->cbGen );
	
	richptt_FreeGroup(me);	
	FREEIF(me->pGroupList);

	richptt_FreePttCallNo(me);
	FREEIF(me->pPttCallGroup);
	
}


void call_richptt_PttCall(newgimis *pMe)
{
	crichptt* me;

	if (pMe->pRichptt != NULL)
	{
		me = (crichptt *)pMe->pRichptt;
		richptt_PttCall(me);
	}
}


 