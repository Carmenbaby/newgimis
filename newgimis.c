/*===========================================================================

FILE: newgimis.c
===========================================================================*/

#include "newgimis.h"
#include "gmTools.h"
#include "gpsOneXTRA.h"		// ������������
#include "gm_FileUpload.h"  // �ļ��ϴ�
#include "gm_Addr.h"

#include "gm_Qchat.h"
#include "gm_task.h"
// Winbase.h��Windows.h

#include "My_Function.h"

#include "gm_sms.h"//  [2/4/2016 Administrator] ##���ڶ���postevent����handler

//	#define TEST_URL  "http://zc.gota.richmap.cn/brewGPS/BrewServlet?act=autdec&ver=2.6.8&pp=FU/ctvH"  //	&tel=18606079685
//	#define TEST_URL  	"http://zc.gota.richmap.cn/brewGPS/BrewServlet?act=autdec&imsi=12345&ver=3.0.1ty&pp=FU/ctvH" // ������
	#define TEST_URL  	"http://gps.richtalk.cn/brewGPS/BrewServlet?act=autdec&imsi=12345&ver=3.1.2au&pp=FU/ctvH" // ������
//	#define TEST_URL  "http://192.168.1.218/brewGPS/BrewServlet"

#ifndef PHONE_AUTO_START
#define PHONE_AUTO_START
#endif

#ifndef PHONE_QCHAT_DUIJIANG
#define PHONE_QCHAT_DUIJIANG
#endif

/*===========================================================================
// 2013-05-09
// ������Ƭ�ĸ�·��
 =========================================================================== */
#define photo_SetRootDir(pMe,strRoot) {	\
	if ( (STRCMP(pMe->sMobileType,"16") == 0) ) {	\
		char sDate[11] = {0};						\
		JulianType dDate;							\
		int nLen;									\
		GETJULIANDATE(0,&dDate);					\
			SPRINTF(sDate,"%04d-%02d-%02d",dDate.wYear,dDate.wMonth,dDate.wDay);	\
		nLen = STRLEN(strRoot);														\
		if (strRoot[nLen-1] == '/')													\
			SPRINTF(strRoot, "%s%s/", strRoot,sDate);								\
		else																		\
			SPRINTF(strRoot, "%s/%s/", strRoot,sDate);								\
	}																				\
}																					





typedef struct _GotoScreenParam {
	ScreenId       id;
	AEEHANDLER     pfn;
	uint32         dwParam;
} GotoScreenParam;

/*-------------------------------------------------------------------
Function Prototypes
-------------------------------------------------------------------*/
static  boolean newgimis_HandleEvent(newgimis* pMe, AEEEvent eCode, 
                                             uint16 wParam, uint32 dwParam);
boolean newgimis_InitAppData(newgimis* pMe);
void    newgimis_FreeAppData(newgimis* pMe);


static boolean testgps_ScreenHandler( newgimis *pMe, AEEEvent eCode,
                                          uint16 wParam, uint32 dwParam );
static boolean testgps_InitApplet( newgimis *pMe );
static boolean testgps_StartApplet( newgimis *pMe );
static boolean testgps_StartApplet_fu( newgimis *pMe );  //�����testgps_StartApplet�ĸ���  ��Ҫ�ǳ�ʼ��Щֵ����Ȼ��ֱ̨��������ʱ��������

static boolean testgps_StartApplet_KJZQ( newgimis *pMe );

static boolean testgps_StartSendXT( newgimis *pMe );  // ������������

// testgps_StartBacktoforeApplet
static boolean testgps_StartBacktoforeApplet( newgimis *pMe ); //�Ӻ�̨�ص�ǰ̨ �ö�ʱ��

static boolean testgps_CloseApplet_SQYC( newgimis *pMe ); //��Ȩ�쳣 �رճ���

static boolean testgps_CloseApplet_WSQ( newgimis *pMe ); //δ��Ȩ �رճ���


////////// ������HTTP�йص� ��ʼ        /////////////////////////////////
static void myiweb_Start(newgimis *pMe, char *pszUrl,int iPage);
static void    myiweb_GotResp(void *p);
static void myiweb_ReadTaskBody(newgimis *pwa);
static void myiweb_DownloadFile(void *p);
static void	 myiweb_DisplayInfo(newgimis* pMe,AECHAR* pInfo);
static void myiweb_Status(void *p, WebStatus ws, void *pVal);
static void myiweb_Header(void *p, const char *cpszName, GetLine *pglVal);
static void myiwebhtml_NotifyCB( void* pvUser, HViewNotify* pNotify );
static void    myiweb_Stop(newgimis *pwa);

static void myiweb_HtmlError(newgimis *pwa);

// ��GPS ��HTTP��ص�
static void myiweb_Start_s(newgimis *pMe, char *pszUrl,int iPage);
static void    myiweb_GotResp_c(void *p);
static void    myiweb_Stop_s(newgimis *pwa);
/////////////////////       ����        /////////////////////////////////


//////////////////    ������ȨҲ���ǳ�������ʱ��û�п���û�����絼��û��������Ȩ�����  START  ////////////
static void    myiweb_Stop_qqsq(newgimis *pwa);
static void myiweb_Start_qqsq(newgimis *pMe, char *pszUrl,int iPage);
static void    myiweb_GotResp_qqsq(void *p);
static void myiweb_Status_qqsq(void *p, WebStatus ws, void *pVal);
static void myiweb_Header_qqsq(void *p, const char *cpszName, GetLine *pglVal);
////////////////////////////////////////               END              /////////////////////////////////////


//////////////   �������йص�  ///////////////
static void    myiweb_Stop_xt(newgimis *pwa);
static void myiweb_Start_xt(newgimis *pMe, char *pszUrl,int iPage);
static void    myiweb_GotResp_xt(void *p);
static void myiweb_Status_xt(void *p, WebStatus ws, void *pVal);
static void myiweb_Header_xt(void *p, const char *cpszName, GetLine *pglVal);
static void myiweb_xt_ReadTaskBody(newgimis *pwa);

/////////////////////////////////////////////

//////////////   ���ϱ����� �йص�  ///////////////
static void    myiweb_Stop_lx(newgimis *pwa);
static void myiweb_Start_lx(newgimis *pMe, char *pszUrl,int iPage);
static void    myiweb_GotResp_lx(void *p);
static void myiweb_lx_ReadTaskBody(newgimis *pwa);
static void myiweb_Status_lx(void *p, WebStatus ws, void *pVal);
static void myiweb_Header_lx(void *p, const char *cpszName, GetLine *pglVal);

//////////////   ���������� �йص�  ///////////////
static void    myiweb_Stop_slx(newgimis *pwa);
static void myiweb_Start_slx(newgimis *pMe, char *pszUrl,int iPage);
static void    myiweb_GotResp_slx(void *p);
static void myiweb_slx_ReadTaskBody(newgimis *pwa);
static void myiweb_Status_slx(void *p, WebStatus ws, void *pVal);
static void myiweb_Header_slx(void *p, const char *cpszName, GetLine *pglVal);

// //////////////  �ϱ�����GPS �йص�  ///////////////
static void    myiweb_Stop_gps(newgimis *pwa);
static void myiweb_Start_gps(newgimis *pMe, char *pszUrl,int iPage);
static void    myiweb_GotResp_gps(void *p);
static void myiweb_gps_ReadTaskBody(newgimis *pwa);
static void myiweb_Status_gps(void *p, WebStatus ws, void *pVal);
static void myiweb_Header_gps(void *p, const char *cpszName, GetLine *pglVal);

//////////////   ���ϱ����� GPS�����йغ� ��λ�쳣�ϱ��йص�  ///////////////
static void    myiweb_Stop_E(newgimis *pwa);
static void myiweb_Start_E(newgimis *pMe, char *pszUrl,int iPage);
static void    myiweb_GotResp_E(void *p);
static void myiweb_E_ReadTaskBody(newgimis *pwa);
static void myiweb_Status_E(void *p, WebStatus ws, void *pVal);
static void myiweb_Header_E(void *p, const char *cpszName, GetLine *pglVal);


// �����ϴ�ʧ��
void Send_GPS_error(newgimis *me);

// �����ϴ�ʧ��
void Send_GPS_error_no_save(newgimis *me);

// ���ڳɹ�
void Send_GPS_success(newgimis *me);
// ������ɺ�Ķ���
void GPS_Afterpost(newgimis *pMe);


// ������ɺ�Ķ���  ���ظ�����Ǹ�����
void GPS_Afterpost_Two(newgimis *pMe);




/////////////////////////////////////////////


////////////////  ��������Ƭ�йصĺ���  ///
// �ص�������ʾͼ��
void uf_ImageNotify(newgimis * pMe,IImage *pIImage, AEEImageInfo * pi, int nErr);


// �ص�������ʾͼ��  ����
void uf_ImageNotify_copy(newgimis * pMe,IImage *pIImage, AEEImageInfo * pi, int nErr);

// �ص�������ʾͼ��  ����
void uf_ImageNotify_copy_15(newgimis * pMe,IImage *pIImage, AEEImageInfo * pi, int nErr);

/*===============================================================================
FUNCTION DEFINITIONS
=============================================================================== */


//  ����յ��Ķ�������
char gimis_CheckMsg(newgimis *pMe, char* rChr_Msg);




////////////////////////////////////       ¼�����ź��� start   /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////
static void SoundPlay_Start(newgimis* pMe);
static void CBSoundPlay(void *p,AEESoundPlayerCmd eCBType,AEESoundPlayerStatus eSPStatus,uint32 dwParam);
/////////////////////////////////////////////////////////////////////////

static void ReleaseObj(void ** ppObj)
{
	if (*ppObj) {
		(void)IBASE_Release(((IBase *)*ppObj));
		*ppObj = NULL;
	}
}


static void SoundPlay_Start(newgimis* pMe)
{
	AEESoundPlayerInfo info;
	info.eInput = SDT_FILE;
	info.pData ="cari.mp3";/*"Voice_001.mp3"*/

	//����SOUNDPLAYʵ��
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOUNDPLAYER,(void **)&pMe->pISoundPlayer) != SUCCESS )
	{
		IAPPLET_Release((IApplet*)pMe);
		return ;
	}
	//ע��ص�
	ISOUNDPLAYER_RegisterNotify(pMe->pISoundPlayer,CBSoundPlay,(void *)pMe);
	//��������Ϊ���
//	ISOUNDPLAYER_SetVolume(pMe->pISoundPlayer, AEE_MAX_VOLUME);
	//����
	if (ISOUNDPLAYER_SetInfo(pMe->pISoundPlayer,&info)  == SUCCESS)
	{
		if (pMe->pISoundPlayer != NULL)
		{
			ISOUNDPLAYER_Play(pMe->pISoundPlayer);
		}
	}
}

static void CBSoundPlay(void *p,AEESoundPlayerCmd eCBType,AEESoundPlayerStatus eSPStatus,uint32 dwParam)
{
    newgimis* pMe = (newgimis*)p;
	AEESoundPlayerCmdData * pData = (AEESoundPlayerCmdData *) dwParam;
	switch(eCBType)
	{
	case AEE_SOUNDPLAYER_PLAY_CB:
		switch(eSPStatus)
		{
		case AEE_SOUNDPLAYER_SUCCESS://��������
			break;
		case AEE_SOUNDPLAYER_DONE://������ִ�����
			ISOUNDPLAYER_RegisterNotify(pMe->pISoundPlayer, NULL, NULL); //�رջص�
			ReleaseObj((void **)&pMe->pISoundPlayer);//�ͷ�
			break;
		case AEE_SOUNDPLAYER_FAILURE://�������
			ReleaseObj((void **)&pMe->pISoundPlayer);//�ͷ�
			break;
		default:
			break;
		}
		break;

	case AEE_SOUNDPLAYER_STATUS_CB:
		switch(eSPStatus)
		{
		case AEE_SOUNDPLAYER_SUCCESS://��������
			break;
		case AEE_SOUNDPLAYER_FAILURE://�������
			break;
		default:
			break;
		}
		break;

	default:
		break;
	}

}




////////////////////////////////////       ¼�����ź���  END    /////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////







//=====================================================================================
// this function is called when your application is starting up
//=====================================================================================



/*===========================================================================
FUNCTION: AEEClsCreateInstance

DESCRIPTION
	This function is invoked while the app is being loaded. All Modules must provide this 
	function. Ensure to retain the same name and parameters for this function.
	In here, the module must verify the ClassID and then invoke the AEEApplet_New() function
	that has been provided in AEEAppGen.c. 

   After invoking AEEApplet_New(), this function can do app specific initialization. In this
   example, a generic structure is provided so that app developers need not change app specific
   initialization section every time except for a call to IDisplay_InitAppData(). 
   This is done as follows: InitAppData() is called to initialize AppletData 
   instance. It is app developers responsibility to fill-in app data initialization 
   code of InitAppData(). App developer is also responsible to release memory 
   allocated for data contained in AppletData -- this can be done in 
   IDisplay_FreeAppData().

PROTOTYPE:
   int AEEClsCreateInstance(AEECLSID ClsId,IShell * pIShell,IModule * po,void ** ppObj)

PARAMETERS:
	clsID: [in]: Specifies the ClassID of the applet which is being loaded

	pIShell: [in]: Contains pointer to the IShell object. 

	pIModule: pin]: Contains pointer to the IModule object to the current module to which
	this app belongs

	ppObj: [out]: On return, *ppObj must point to a valid IApplet structure. Allocation
	of memory for this structure and initializing the base data members is done by AEEApplet_New().

DEPENDENCIES
  none

RETURN VALUE
  AEE_SUCCESS: If the app needs to be loaded and if AEEApplet_New() invocation was
     successful
  EFAILED: If the app does not need to be loaded or if errors occurred in 
     AEEApplet_New(). If this function returns FALSE, the app will not be loaded.

SIDE EFFECTS
  none
===========================================================================*/


//======================= ������������ ====================================================
static void gimis_XTRA(newgimis *pMe)
{
	if ( TRUE == XtraDc_HandleEvent(pMe,EVT_APP_START,0,0) )
		XtraDc_HandleEvent(pMe,XTRA_EVT_USER_INIT_DWNLD,0,0);   // XtraDc_HandleEvent(pMe,XTRA_EVT_USER_DATA_VALIDITY,wParam,dwParam);

	return;
}





int AEEClsCreateInstance(AEECLSID ClsId, IShell *pIShell, IModule *po, void **ppObj)
{
	int si_Ret = 0;
	*ppObj = NULL;

	if( ClsId == AEECLSID_NEWGIMIS )
	{
		// Create the applet and make room for the applet structure
		if( AEEApplet_New(sizeof(newgimis),
                          ClsId,
                          pIShell,
                          po,
                          (IApplet**)ppObj,
                          (AEEHANDLER)newgimis_HandleEvent,
                          (PFNFREEAPPDATA)newgimis_FreeAppData) ) // the FreeAppData function is called after sending EVT_APP_STOP to the HandleEvent function
                          
		{
			//Initialize applet data, this is called before sending EVT_APP_START
            // to the HandleEvent function

			uf_RecLog((newgimis*)*ppObj,"�ʼ����");
			si_Ret = gimis_TEST_MOD_FILE( (newgimis*)*ppObj ); //���MOD�ļ���û�д���
			if (si_Ret == 0)
			{
				// û��MOD�ļ�
				IAPPLET_Release((IApplet*)*ppObj);
				return EFAILED;
			}
			if(newgimis_InitAppData((newgimis*)*ppObj)) // ������һЩ����GPSֵ�Ĳ���
			{
				//Data initialized successfully
				return(AEE_SUCCESS);
			}
			else
			{
				//Release the applet. This will free the memory allocated for the applet when
				// AEEApplet_New was called.
				IAPPLET_Release((IApplet*)*ppObj);
				return EFAILED;
			}

        } // end AEEApplet_New

    }

	return(EFAILED);
}


/*===========================================================================
FUNCTION SampleAppWizard_HandleEvent

DESCRIPTION
	This is the EventHandler for this app. All events to this app are handled in this
	function. All APPs must supply an Event Handler.

PROTOTYPE:
	boolean SampleAppWizard_HandleEvent(IApplet * pi, AEEEvent eCode, uint16 wParam, uint32 dwParam)

PARAMETERS:
	pi: Pointer to the AEEApplet structure. This structure contains information specific
	to this applet. It was initialized during the AEEClsCreateInstance() function.

	ecode: Specifies the Event sent to this applet

   wParam, dwParam: Event specific data.

DEPENDENCIES
  none

RETURN VALUE
  TRUE: If the app has processed the event
  FALSE: If the app did not process the event

SIDE EFFECTS
  none


===========================================================================*/  

#define MAX_RES_STRING_BUF_SIZE      256                  // Max buffer size for string buffers



//�������и��ֻ������뷨��Ļ����
boolean TASK_InputMethod(newgimis *pMe,  uint16 wParam)
{
	cTaskApp *me = NULL;
	AEERect rRect_cln;

	if (pMe->pTaskApp)
		me = pMe->pTaskApp;
	else
		return FALSE;
	
//uf_RecLog(pMe,"TASK_InputMethod in");

	//SETAEERECT(&rRect_cln, 0, 10 , pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - 50);
	SETAEERECT(&rRect_cln, 0, 10 , pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - 10 - pMe->m_MenuH);



	if (STRCMP(pMe->sMobileType,"01") == 0  /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/)//����Q3
	{

		if (pMe->m_ConT_FH == 1)
		{
			if (wParam == AVK_5 || wParam == AVK_4 || wParam == AVK_3 || wParam == AVK_2 || wParam == AVK_1)
			{
			//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##########wParam == AVK_5 || wParam == AVK_4 || wParam == AVK_3 || wParam == AVK_2 || wParam == AVK_1 ##########");	
				IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rRect_cln);
				Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
				// ��������
				//Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
				// ����ǰ״̬		
				Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);
				Task_RedrawScreen(pMe);
				pMe->m_ConT_FH = 0;
			}
		}
		else
		{
			if (wParam == AVK_5 || wParam == AVK_4 || wParam == AVK_3 || wParam == AVK_2 || wParam == AVK_1||
		   		   wParam == AVK_6 || wParam == AVK_7 || wParam == AVK_8 || wParam == AVK_9 || wParam == AVK_0)
			{
				IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rRect_cln);
					
				Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
				// ��������
				//Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
				// ����ǰ״̬
				Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);
				Task_RedrawScreen(pMe);		   			   
				pMe->m_ConT_FH = 0;
			}
		}

		if (wParam == AVK_SELECT)
		{
			IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rRect_cln);

			//STRCAT(strTmp, me->pTaskRecB->pTaskName);
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), " ##########strlen = %d, strTmp = %s ########## " , STRLEN(me->pTaskRecB->pTaskName),strTmp );
			// �������
			Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
			// ��������
			//Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
			// ����ǰ״̬
			Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);
			Task_RedrawScreen(pMe);			  
			pMe->m_ConT_FH = 0;	   
		}
		else if (wParam == AVK_CLR)
		{
			IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rRect_cln);	   
			// �������
			Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
			// ��������
			//Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
			// ����ǰ״̬
			Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);
			Task_RedrawScreen(pMe);
			pMe->m_ConT_FH = 0;
		}
		else if (wParam == AVK_STAR) //  �Ǽ�
		{
		   pMe->m_ConT_FH = 1;
		}

		if (wParam == AVK_SOFT1 || wParam == AVK_SOFT2 || wParam == AVK_END)
		{

		   if (pMe->m_ConT_FH == 1)
		   {
			   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rRect_cln);
			   pMe->m_ConT_FH = 0;
		   }
		}

		if (wParam == AVK_SOFT1 || wParam == AVK_SOFT2 )
		{
			ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog, &(pMe->nInd), "wParam == AVK_SOFT1 || wParam == AVK_SOFT2");
		}

	//	if (wParam == AVK_LEFT || wParam == AVK_RIGHT)
	//	{
	//		if (pMe->m_ConT_FH != 1)
	//		{
	//			IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rRect_cln);
	//		}			   
	//	}  

	//	if (wParam == AVK_POUND)
	//	{
	//	   // ˵���л����ֻ����ַ���������
	//	   if (pMe->m_ConT_FH != 1)		   
	//	}

		ITEXTCTL_Redraw( pMe->textCtl_T );
		IDISPLAY_Update(pMe->a.m_pIDisplay); 

//uf_RecLog(pMe,"TASK_InputMethod out");
		#ifdef PHONE_QCHAT_DUIJIANG

		   if (pMe->m_F_DJ == 1)
		   {
			   // ˵�����ڶԽ�״̬
			   if (wParam == AVK_END)
			   {
				   return FALSE;
			   }
		   }
		#else   
		   return TRUE;
		#endif

		   return TRUE;
	}

/*
	if (STRCMP(pMe->sMobileType,"01") == 0)//����Q3
	{

		if (pMe->m_ConT_FH == 1)
		{
			if (wParam == AVK_5 || wParam == AVK_4 || wParam == AVK_3 || wParam == AVK_2 || wParam == AVK_1)
			{
			//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"##########wParam == AVK_5 || wParam == AVK_4 || wParam == AVK_3 || wParam == AVK_2 || wParam == AVK_1 ##########");	
				IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rRect_cln);
				Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
				// ��������
				Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
				// ����ǰ״̬		
				Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);
				Task_RedrawScreen(pMe);
				pMe->m_ConT_FH = 0;
			}
		}else{
			if (wParam == AVK_5 || wParam == AVK_4 || wParam == AVK_3 || wParam == AVK_2 || wParam == AVK_1||
		   		   wParam == AVK_6 || wParam == AVK_7 || wParam == AVK_8 || wParam == AVK_9 || wParam == AVK_0)
			{
				IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rRect_cln);
					
				Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
				// ��������
				Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
				// ����ǰ״̬
				Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);
				Task_RedrawScreen(pMe);		   			   
				pMe->m_ConT_FH = 0;
			}
		}

		if (wParam == AVK_SELECT)
		{
			IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rRect_cln);

			//STRCAT(strTmp, me->pTaskRecB->pTaskName);
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), " ##########strlen = %d, strTmp = %s ########## " , STRLEN(me->pTaskRecB->pTaskName),strTmp );
			// �������
			Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
			// ��������
			Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
			// ����ǰ״̬
			Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);
			Task_RedrawScreen(pMe);			  
			pMe->m_ConT_FH = 0;	   
		}
		else if (wParam == AVK_CLR)
		{
			IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rRect_cln);	   
			// �������
			Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
			// ��������
			Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
			// ����ǰ״̬
			Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);
			Task_RedrawScreen(pMe);
			pMe->m_ConT_FH = 0;
		}
		else if (wParam == AVK_STAR) //  �Ǽ�
		{
		   pMe->m_ConT_FH = 1;
		}

		if (wParam == AVK_SOFT1 || wParam == AVK_SOFT2 || wParam == AVK_END)
		{

		   if (pMe->m_ConT_FH == 1)
		   {
			   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rRect_cln);
			   pMe->m_ConT_FH = 0;
		   }
		}

		if (wParam == AVK_SOFT1 || wParam == AVK_SOFT2 )
		{
			ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog, &(pMe->nInd), "wParam == AVK_SOFT1 || wParam == AVK_SOFT2");
		}

	//	if (wParam == AVK_LEFT || wParam == AVK_RIGHT)
	//	{
	//		if (pMe->m_ConT_FH != 1)
	//		{
	//			IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &rRect_cln);
	//		}			   
	//	}  

	//	if (wParam == AVK_POUND)
	//	{
	//	   // ˵���л����ֻ����ַ���������
	//	   if (pMe->m_ConT_FH != 1)		   
	//	}

		ITEXTCTL_Redraw( pMe->textCtl_T );
		IDISPLAY_Update(pMe->a.m_pIDisplay); 

//uf_RecLog(pMe,"TASK_InputMethod out");
		#ifdef PHONE_QCHAT_DUIJIANG
	   if (pMe->m_F_DJ == 1)
		   {
			   // ˵�����ڶԽ�״̬
			   if (wParam == AVK_END)
			   {
				   return FALSE;
			   }
		   }
		#else   
		   return TRUE;
		#endif
	}


*/



	if (STRCMP(pMe->sMobileType,"57") == 0) //M6
	{
		//ud_RecLog(pMe->a.m_pIShell, pMe->cWrtLog, &(pMe->nInd), "sMobileType = %s",pMe->sMobileType);
		//��ⲻ��end, ֻ�������������
		if (wParam == AVK_END)
		{

		}

#ifdef PHONE_QCHAT_DUIJIANG

   if (pMe->m_F_DJ == 1)
   {
	   // ˵�����ڶԽ�״̬
	   if (wParam == AVK_END)
	   {
		   return FALSE;
	   }
   }
#else   
   return TRUE;
#endif

		   return TRUE;
	}


	return TRUE;
} 


static boolean newgimis_HandleEvent(newgimis* pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{  

	cTaskApp *me_t;
//	AECHAR szText[] = {'H','e','l','l','o',' ','W','o', 'r', 'l', 'd', '\0'};
	AECHAR psTextBuf[ MAX_RES_STRING_BUF_SIZE ];
//	AECHAR str[] = {0x6C49,0x5B57��'0'};
	AECHAR strerr[] = {0x5f53,0x524d,0x7f51,0x7edc,0xff1a,0x5f02,0x5e38,0x0000};//  �����쳣
	AECHAR strnal[] = {0x5f53,0x524d,0x7f51,0x7edc,0xff1a,0x6b63,0x5e38,0x0000};//  ��������
	AECHAR strqqsqsuss[] = {0x8bf7,0x6c42,0x6388,0x6743,0xff1a,0x6210,0x529f,0x0000} ;//������Ȩ�ɹ�
	AECHAR strqqsqfail[] = {0x8bf7,0x6c42,0x6388,0x6743,0xff1a,0x5931,0x8d25,0x0000} ;//������Ȩʧ��
	AECHAR strqqsqyc[] = {0x8bf7,0x6c42,0x6388,0x6743,0xff1a,0x7f51,0x7edc,0x5f02,0x5e38,0x0000} ;//ht������Ȩʧ��
	AECHAR strsqz[] = {0x8bf7,0x6c42,0x6388,0x6743,0x4e2d,0x0000};//  ������Ȩ��

	int si_Height = 0;

	////////////   10-13  ���������뷨������  START   ////////////////////////////////

	char s_buf[256];//  ������

    AEERect sr_Rect;
    AEERect sr_Rect1;

	AEERect sr_Rect2;

	AEERect sr_Rect3;

	AEERect	rRect_cln;
	cTaskApp *me = NULL; //�޸�

	if (pMe->pTaskApp)
	{
		me = pMe->pTaskApp;
	}

// 	if (eCode == EVT_KEY_PRESS)
// 	{
// 		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"����wParam=%d dwParam=%d eCode=%d", wParam,dwParam,eCode);
// 	}


	//   SETAEERECT(&sr_Rect, 0, (2 * pMe->m_SYSFBH) + pMe->m_PhotoH - 40, pMe->DeviceInfo.cxScreen, 40 );  // m_nFontHeight+10 
	// pMe->m_SYSFBH  m_MenuH
	SETAEERECT(&sr_Rect, 0, (2 * pMe->m_SYSFBH) + pMe->m_PhotoH - pMe->m_SYSFBH * 2, pMe->DeviceInfo.cxScreen, pMe->m_MenuH );

//	SETAEERECT(&sr_Rect1, 0, (2 * pMe->m_SYSFBH) + pMe->m_PhotoH - pMe->m_SYSFBH * 2, pMe->DeviceInfo.cxScreen, pMe->m_MenuH * 2 );
	SETAEERECT(&sr_Rect1, 0,  pMe->m_PhotoH , pMe->DeviceInfo.cxScreen, pMe->m_MenuH * 2 );
	//��ɫ�ǿ�

	if (STRCMP(pMe->sMobileType,"66") == 0)
	{	
		SETAEERECT(&sr_Rect2, 0, pMe->m_SYSFBH + pMe->m_PhotoH + pMe->m_SYSFBH /2 -8, pMe->DeviceInfo.cxScreen, pMe->m_SYSFBH /2 + 10 +8  /* pMe->m_MenuH */  );  // pMe->m_MenuH
	}
	else
	{
		SETAEERECT(&sr_Rect2, 0, pMe->m_SYSFBH + pMe->m_PhotoH + pMe->m_SYSFBH /2 , pMe->DeviceInfo.cxScreen, pMe->m_SYSFBH /2 + 10  /* pMe->m_MenuH */  );  // pMe->m_MenuH
	}
//	SETAEERECT(&sr_Rect2, 0, pMe->m_SYSFBH + pMe->m_PhotoH + pMe->m_SYSFBH /2, pMe->DeviceInfo.cxScreen, pMe->m_SYSFBH /2 + 10  /* pMe->m_MenuH */  );  // pMe->m_MenuH

	SETAEERECT(&sr_Rect3, 0, (2 * pMe->m_SYSFBH) + pMe->m_PhotoH , pMe->DeviceInfo.cxScreen, 10);  // m_nFontHeight+10



/////////////////////////////////�����ֻ���Ҫ���ΰ���///////////////////////////////////////////
	//57��M6�ֻ�
	if ( ( 0 == STRCMP(pMe->sMobileType ,"63") || 0 == STRCMP(pMe->sMobileType ,"57") || 0 == STRCMP(pMe->sMobileType ,"55") ) \
		&& AVK_END == wParam && (pMe->textCtl_T!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl_T)) ) //�ػ���
	{
		//ֻ�����ڴ����������뷨ʱ��� END��
		//uf_RecLog(pMe, "sMobileType = 57 or 63 press AVK_END #shield# .");
		return TRUE;
	}
	if (  ( 0 == STRCMP(pMe->sMobileType ,"63") || 0 == STRCMP(pMe->sMobileType ,"57") || 0 == STRCMP(pMe->sMobileType ,"55") ) \
		&& AVK_END == wParam && (pMe->textCtl!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl)))
	{
		//��Ƭ���뷨
		//uf_RecLog(pMe, "sMobileType = 57 or 63  press AVK_END #shield# .");
		return TRUE;
	}

////////////////////////////////��������������¼�����/////////////////////////////////////////
	if (  AVK_DOWN == wParam && 0 == pMe->m_Task_fh && (pMe->textCtl_T!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl_T)) ) //�ػ���
	{
		//ֻ�����ڴ����������뷨ʱ����¼�
		return TRUE;
	}


//////////////////////////////�������뷨 ����////////////////////////////////////0/
	SETAEERECT(&rRect_cln, 0, 10 , pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - 50);
	
	// X3
	if (0 == STRCMP(pMe->sMobileType,"33")) 
	{
		if(InputMode33(pMe,  pMe->textCtl_T, eCode, wParam, dwParam ))
		{
			return TRUE;
		}
	}

	//���� "01";  m6 "57";
	if((pMe->textCtl_T!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl_T))&& ITEXTCTL_HandleEvent( pMe->textCtl_T, eCode, wParam, dwParam ))
	{	
		return	TASK_InputMethod(pMe, wParam);
	}
//////////////////////////////////////////////////////////////////////////




	if (STRCMP(pMe->sMobileType,"57") == 0) //�������뷨 ��Ƭ
	{
		if((pMe->textCtl!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl))&& ITEXTCTL_HandleEvent( pMe->textCtl, eCode, wParam, dwParam ))
		{
			ITEXTCTL_GetText(pMe->textCtl,pMe->pText,256);
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), " : wParam =  %d, eCode = %d ",wParam, eCode);
			if (wParam == AVK_END)//��������END
			{
				//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"press AVK_END");
			}
			
#ifdef PHONE_QCHAT_DUIJIANG
			
			if (pMe->m_F_DJ == 1)
			{
				// ˵�����ڶԽ�״̬
				if (wParam == AVK_END)
				{
					return FALSE;
				}
			}
#else   
			return TRUE;
#endif
			
			return TRUE;
		}
	}
	


	if (STRCMP(pMe->sMobileType,"33") == 0) //X3��Ƭ���뷨
	{
		if (1 == pMe->m_ConT_FH)
		{
			if (  wParam == AVK_END || wParam == AVK_LEFT || wParam == AVK_RIGHT) // AVK_SOFT1 :57398   AVK_SEND 57391 ͨ����
			{
				return TRUE;
			}
			//����* ��ʧȥ����
//			if (wParam == AVK_END || wParam == AVK_CLR ||wParam == AVK_UP || wParam == AVK_DOWN || wParam == AVK_LEFT
//					|| wParam == AVK_RIGHT || (wParam >= AVK_1 && wParam <= AVK_9 && (eCode != EVT_KEY_HOOK_RELEASE && eCode != EVT_KEY_RELEASE) ))
			if ((wParam == AVK_END || wParam == AVK_CLR ||wParam == AVK_UP || wParam == AVK_DOWN || wParam == AVK_LEFT
					|| wParam == AVK_RIGHT || (wParam >= AVK_1 && wParam <= AVK_9))		 && (eCode != EVT_KEY_HOOK_RELEASE && eCode != EVT_KEY_RELEASE))
			{
			//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "DEBUGE: m_ConT_FH : wParam =  %d, eCode = %d ",wParam, eCode);

				if (ITEXTCTL_IsActive( pMe->textCtl))
				{
					ITEXTCTL_SetCursorPos(pMe->textCtl, TC_CURSOREND);//����������ı���ĩβ(���»�ȡ���� ȫѡ�ֱ�ɾ����)
				}
				else
				{
				//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "DEBUGE: m_ConT_FH==1  set active.. *******");
					ITEXTCTL_SetActive( pMe->textCtl, TRUE);
					ITEXTCTL_SetCursorPos(pMe->textCtl, TC_CURSOREND);
				}
			}	
		}


		if((pMe->textCtl!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl)))
		{	
			ITEXTCTL_GetText(pMe->textCtl,pMe->pText,256);
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "DEBUGE: wParam =  %d, eCode = %d ",wParam, eCode);
			if (wParam == AVK_POUND)  //#
			{
				if(EVT_KEY_PRESS == eCode || EVT_KEY_HOOK_PRESS == eCode)
				{
					AEERect re;
					//�����Ǽ����� EVT_KEY_HOOK_PRESS ����������������������������������������������������������������
					//AEETextInputMode   ImeNext;
					AEETextInputModeInfo CurIme;										
					
					ITEXTCTL_GetInputMode(pMe->textCtl,&CurIme);
					//ImeNext = ITEXTCTL_GetInputMode(pMe->textCtl,&CurIme);									
					//uf_ShowInputMode(pMe,CurIme.tmMode);
					
					if (AEE_TM_PINYIN == CurIme.tmMode)
					{
						ITEXTCTL_SetInputMode(pMe->textCtl, AEE_TM_NUMBERS);
					}
					else if(AEE_TM_NUMBERS == CurIme.tmMode)
					{
						ITEXTCTL_SetInputMode(pMe->textCtl, AEE_TM_LETTERS); //AEE_TM_LETTERS
					}
					else if(AEE_TM_LETTERS == CurIme.tmMode)
					{
						ITEXTCTL_SetInputMode(pMe->textCtl, AEE_TM_STROKE); //AEE_TM_LETTERS AEE_TM_STROKE
					}
					else
					{
						ITEXTCTL_SetInputMode(pMe->textCtl, AEE_TM_PINYIN);
					}
					
					//����ɵ�����
					//SETAEERECT( &re, nCol * di.cxScreen / MAXCOLUMNS, nLine * LINEHEIGHT, pMe->DeviceInfo.cxScreen, LINEHEIGHT+2);
					SETAEERECT( &re, 1 * pMe->DeviceInfo.cxScreen / MAXCOLUMNS, 13 * LINEHEIGHT, pMe->DeviceInfo.cxScreen, LINEHEIGHT+4);
					IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &re);
					//x3��Ƭ���뷨		
					{
						//AEETextInputModeInfo CurIme;											
						ITEXTCTL_GetInputMode( pMe->textCtl,&CurIme);									
						yb_ShowInputMode(pMe,CurIme.tmMode, 1);
					}
					
				}
				return TRUE; //��Ҫ��handle����
			}  
			//	if ( wParam == AVK_STAR)  //*
			/*			if ( wParam == AVK_STAR&& (eCode != EVT_KEY_HOOK_RELEASE && eCode != EVT_KEY_RELEASE))
			{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "DEBUGE: AVK_STAR   eCode = %d  SetInputMode",eCode);
			if (eCode != 273)
			{	uf_RecLog(pMe,"111");
			pMe->m_ConT_FH = 1;
			uf_RecLog(pMe,"222");
			ITEXTCTL_SetInputMode(pMe->textCtl, AEE_TM_SYMBOLS);
			uf_RecLog(pMe,"333");
			}
			
			  
				return TRUE;
				}
			*/
			if ( wParam == AVK_STAR)
			{
				if (eCode == EVT_KEY_HOOK_RELEASE && eCode == EVT_KEY_RELEASE)
				{
					//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "DEBUGE: AVK_STAR   eCode = EVT_KEY_HOOK_RELEASE");
					return TRUE;
				}
				if (eCode == 272)
				{	
					//	uf_RecLog(pMe,"272");
					return TRUE;
				}
				if (eCode == 256)
				{	
					//	uf_RecLog(pMe,"256");
					return TRUE;
				}
				
				//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "DEBUGE: AVK_STAR   eCode = %d  SetInputMode",eCode);
				pMe->m_ConT_FH = 1;
				ITEXTCTL_SetInputMode(pMe->textCtl, AEE_TM_SYMBOLS);
				
				return TRUE;
			}
			
			
			if ( wParam == AVK_LEFT || wParam == AVK_RIGHT || wParam == AVK_SELECT)  //*
			{
				if (1 == pMe->m_ConT_FH)
				{		
					//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "DEBUGE: AVK_SELECT 1");
					return TRUE;
				}
			}  
			
			if ( wParam == AVK_SELECT) 
			{			
				if (1 == pMe->m_ConT_FH)
				{
					//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "DEBUGE: AVK_SELECT 2");
					return TRUE;
				}
			} 
			
			if ( wParam == AVK_SEND || wParam == AVK_SOFT1 || wParam == AVK_END) // AVK_SOFT1 :57398   AVK_SEND 57391 ͨ����
			{
				
				return TRUE;
			}
			
			if (ITEXTCTL_HandleEvent( pMe->textCtl, eCode, wParam, dwParam ))
			{		
				if ( wParam == AVK_CLR)  //*
				{
					IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );				
					if (1 == pMe->m_ConT_FH)
					{	
						//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "DEBUGE:1 == pMe->m_ConT_FH    AVK_CLR ");
						pMe->m_ConT_FH = 0;
						
						if (pMe->pi_copy) 		
						{		
							IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
						}
						if (pMe->m_PhotoTextCtl)
						{						
							ISTATIC_Redraw(pMe->m_PhotoTextCtl);
						}
						if (pMenuCtl)
						{
							IMENUCTL_Redraw( pMenuCtl );
						}
						//x3��Ƭ���뷨		
						{
							AEETextInputModeInfo CurIme;																		
							ITEXTCTL_GetInputMode( pMe->textCtl,&CurIme);									
							yb_ShowInputMode(pMe,CurIme.tmMode,1);
						}
						return TRUE;
					}
					
					//	return TRUE;					
					
				} 
				
				
				if (wParam >= AVK_1 && wParam <= AVK_9)
				{
					IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );
					if (1 == pMe->m_ConT_FH)
					{		
						//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "DEBUGE:1 == pMe->m_ConT_FH    AVK_1-9 ");
						pMe->m_ConT_FH = 0;
						
						if (pMe->pi_copy)//newgimis_forphoto ���������������		
						{				
							IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
						}
						if (pMe->m_PhotoTextCtl)
						{
							ISTATIC_Redraw(pMe->m_PhotoTextCtl);
						}
						if (pMenuCtl)
						{					
							IMENUCTL_Redraw( pMenuCtl );
						}
						//x3��Ƭ���뷨		
						{
							AEETextInputModeInfo CurIme;											
							ITEXTCTL_GetInputMode( pMe->textCtl,&CurIme);									
							yb_ShowInputMode(pMe,CurIme.tmMode,1);
						}
						return TRUE;
					}
				}
				
				if (wParam == AVK_POUND)  //#
				{
					//ƴ��ģʽ  # *�Ǽ���������
					return TRUE;
				}
				if ( wParam == AVK_STAR)  //�Ǻ�
				{
					//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "DEBUGE: AVK_STAR   eCode = %d ",eCode);
					//	pMe->m_ConT_FH = 1;
					return TRUE;
				} 	
				
				if (wParam == AVK_END)
				{
					return TRUE;
				}
				
				//if (wParam == AVK_SELECT && 0 == pMe->m_ConT_FH )
				if ((wParam == AVK_CLR || wParam == AVK_UP ||wParam == AVK_DOWN ||wParam == AVK_SELECT ||wParam == AVK_LEFT || wParam == AVK_RIGHT)&& 0 == pMe->m_ConT_FH )
				{
					IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );
					
					//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "DEBUGE:AVK_SELECT   wParam =  %d, eCode = %d ",wParam, eCode);
					
					if (pMe->pi_copy)//newgimis_forphoto ���������������		
					{				
						IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
					}
					if (pMe->m_PhotoTextCtl)
					{
						ISTATIC_Redraw(pMe->m_PhotoTextCtl);
					}
					if (pMenuCtl)
					{					
						IMENUCTL_Redraw( pMenuCtl );
					}
					//x3��Ƭ���뷨		
					{
						AEETextInputModeInfo CurIme;											
						ITEXTCTL_GetInputMode( pMe->textCtl,&CurIme);									
						yb_ShowInputMode(pMe,CurIme.tmMode,1);
					}
					return TRUE;
				}
				
#ifdef PHONE_QCHAT_DUIJIANG
				
				if (pMe->m_F_DJ == 1)
				{
					// ˵�����ڶԽ�״̬
					if (wParam == AVK_END)
					{
						return FALSE;
					}
				}
#else   
				return TRUE;
#endif
				
				ITEXTCTL_GetText(pMe->textCtl,pMe->pText,256);
				WSTRTOSTR(pMe->pText,s_buf,256);
				//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), " Last save pMe->pText : s_buf=%s ",s_buf);
				return TRUE;
			}
		}		
	}





	if (STRCMP(pMe->sMobileType,"01") == 0  /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/)
	{
		 //  ���ڻ���һ���� * �������ַ��Ĵ���  ��Ϊ����������ܴ�
		if((pMe->textCtl!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl))&& ITEXTCTL_HandleEvent( pMe->textCtl, eCode, wParam, dwParam ))
		{
		   ITEXTCTL_GetText(pMe->textCtl,pMe->pText,256);
		   WSTRTOSTR(pMe->pText,s_buf,256);

		   
		   if (wParam == AVK_LEFT || wParam == AVK_RIGHT)
		   {
			   if (pMe->m_ConT_FH != 1)
			   {
	//			   if (pMe->pi_copy) 		
	//				{
	//					IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
	//				}
				   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect2);
			   }
			   
			   
		   }

		   

		   if (wParam == AVK_POUND)
		   {
			   // ˵���л����ֻ����ַ���������
//			   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);
			   if (pMe->m_ConT_FH != 1)
			   {
				   if (pMe->pi_copy) 		
				   {
						IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
				   }
				   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect2);
			   }

			   
			   
		   }

		   uf_RecLog(pMe,s_buf );

		   if (pMe->m_ConT_FH == 1)
		   {
			   if (wParam == AVK_5 || wParam == AVK_4 || wParam == AVK_3 || wParam == AVK_2 || wParam == AVK_1)
			   {
				   if (pMe->pi_copy) 		
				   {
						IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
				   }
				   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect2);
				   pMe->m_ConT_FH = 0;
			   }
///////////////////////////////////ֱ������Ǻ��ֻ���һ���������/////////////////////////////////////////////////
		   }
		   else
		   {
			   if (wParam == AVK_5 || wParam == AVK_4 || wParam == AVK_3 || wParam == AVK_2 || wParam == AVK_1||
				   wParam == AVK_6 || wParam == AVK_7 || wParam == AVK_8 || wParam == AVK_9 || wParam == AVK_0)
			   {
				   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);//2015-12-15�޸�/sr_Rect2���� 1��
				   pMe->m_ConT_FH = 0;
			   }
		   }



		   if (wParam == AVK_SELECT)
		   {
		//	   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect);
			   if (pMe->pi_copy) 		
				{
					IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
				}
			   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);
			   pMe->m_ConT_FH = 0;
			   
		   }
		   else if (wParam == AVK_CLR)
		   {
		//	   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);
//			   if (pMe->pi_copy) 		
//				{
//					IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
//				}
			   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);


///////////////////////////////////////////////////////////////////////////////////////////////////////////
			   if (pMe->pi_copy) 		
			   {
				   IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
			   }
////////////////////////////////////2015-12-15�޸����////////////////////////////////////////////////////
			   pMe->m_ConT_FH = 0;
		   }
		   else if (wParam == AVK_STAR) //*��
		   {
			   pMe->m_ConT_FH = 1;
			   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect2);
		   }



//		   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"����ֵ ���� %d", wParam);

		   if (STRCMP(s_buf,pMe->m_ConText) != 0)  // m_ConText �����洢��Ƭ����༭���еı༭����
		   {
			   MEMSET(pMe->m_ConText,0x00,256);
			   STRCPY(pMe->m_ConText,s_buf);   

			   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect3);
		   }



		   if (wParam == AVK_SOFT1 || wParam == AVK_SOFT2 || wParam == AVK_END)
		   {
			   if (pMe->m_ConT_FH == 1)
			   {
				   if (pMe->pi_copy) 		
					{
						IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
					}
				   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect2);
				   pMe->m_ConT_FH = 0;
			   }
		   }

		   ITEXTCTL_Redraw( pMe->textCtl );
		   IDISPLAY_Update(pMe->a.m_pIDisplay); 

#ifdef PHONE_QCHAT_DUIJIANG

		   if (pMe->m_F_DJ == 1)
		   {
			   // ˵�����ڶԽ�״̬
			   if (wParam == AVK_END)
			   {
				   return FALSE;
			   }
		   }
#else   
		   return TRUE;
#endif
		   return TRUE;
		}

//		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"����ֵ ���� %d", wParam);
	}




	////////////////////////////      ����һ���ֻ��ͺ� START     ///////////////////////
	////////////////////////////////////////////////////////////////////////////////////
	////////////////////////////////////////////////////////////////////////////////////

	if (STRCMP(pMe->sMobileType,"66") == 0)
	{
	//	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "*****newgimis_HandleEvent  sMobileType = (%s)", pMe->sMobileType);

		 //  ���ڻ���һ���� * �������ַ��Ĵ���  ��Ϊ����������ܴ�
		if((pMe->textCtl!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl))&& ITEXTCTL_HandleEvent( pMe->textCtl, eCode, wParam, dwParam ))
		{
		   ITEXTCTL_GetText(pMe->textCtl,pMe->pText,256);//��ȡ�������������� ITextCtl �ӿڶ���������ı� ���ı���С�� nMaxChars
		   WSTRTOSTR(pMe->pText,s_buf,256);//�˺������ڽ����ַ���ת��Ϊ���ֽ��ַ���

		   
			
			//���뷨С�飬�����ƶ�ʱ����Ҫ���ˢ��
		   if (wParam == AVK_LEFT || wParam == AVK_RIGHT)
		   {
			   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "*****newgimis_HandleEvent  wParam == AVK_RIGHT ||AVK_LEFT");
			   if (pMe->m_ConT_FH != 1)//�����ڷ��ŵ�״̬��
			   {
	//			   if (pMe->pi_copy) 		
	//				{
	//					IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
	//				}
				   //???????????????????????????????????????????????????????????
				   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect2); //ʹ��Ĭ�ϵı�����ɫ���� CLR_USER_BACKGROUND ��������ɫ�����������յ�����
																	////�� ��Ҫִ�л��Ʋ�������Ļ���򣬼������Ļ��
			   }
			   
			   
		   }



		   

	
		   if (wParam == AVK_POUND)//#��
		   {
			   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "*****newgimis_HandleEvent  wParam == AVK_POUND");
			   // ˵���л����ֻ����ַ���������
			   //IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);
			   if (pMe->m_ConT_FH != 1) //�����ڷ��ŵ�״̬��
			   {
				   if (pMe->pi_copy) 		
				   {
						//???????????????????????????????????????????????????????????
						IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
				   }
				   //IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect2);
				   //2015-12-15�޸� 66��01Ӧ�ö�����
				   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);
			   }
		   }

	   
		   uf_RecLog(pMe,s_buf );
   
		   if (pMe->m_ConT_FH == 1)//���ڷ��ŵ�״̬��
		   {
		   	   if (wParam == AVK_5 || wParam == AVK_4 || wParam == AVK_3 || wParam == AVK_2 || wParam == AVK_1)
		   	   {
		   		   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "*****newgimis_HandleEvent  wParam == AVK_5-4-3-2-1");
		   		   if (pMe->pi_copy) 		
		   		   {
		   				IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
		   		   }
		   		//   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect2);
				   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);//2015-12-15�޸�(����������С����)
		   		   pMe->m_ConT_FH = 0;
		   	   }
//////////////////////ֱ������Ǻ��ֻ���һ���������/////////////////////////////////////////////////
		   }else{
				if (wParam == AVK_5 || wParam == AVK_4 || wParam == AVK_3 || wParam == AVK_2 || wParam == AVK_1||
					wParam == AVK_6 || wParam == AVK_7 || wParam == AVK_8 || wParam == AVK_9 || wParam == AVK_0)
				{
					IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);//2015-12-15�޸�/sr_Rect2���� 1��
		   			pMe->m_ConT_FH = 0;
				}
		   }
///////////////////////////2015-12-15�޸����/////////////////////////////////////////////////////

//////////////////////////////////66�ֻ�////////////////////////////
		   if ( wParam == AVK_SOFT2&& STRCMP(pMe->sMobileType,"66") == 0 )// 66 �ֻ���CLR��
		   {
			   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "*****newgimis_HandleEvent   wParam == AVK_SOFT2");
			   if (pMe->m_ConT_FH == 1)
			   {
				   if (pMe->pi_copy) 		
				   {
					   IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
				   }
				   
				   pMe->m_ConT_FH = 0;
			   }
			   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);
		   }
   
///////////////////////////2015-12-15�޸����/////////////////////////////////////////////////////



		   if (wParam == AVK_SELECT)
		   {
		//	   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect);
		   	   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "*****newgimis_HandleEvent  wParam == AVK_SELECT");
IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);//�� ��Ҫִ�л��Ʋ�������Ļ���򣬼������Ļ��
		   	   if (pMe->pi_copy) 	
		   		{
		   			IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
		   		}
		   	  // IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);//�� ��Ҫִ�л��Ʋ�������Ļ���򣬼������Ļ��
		   	   pMe->m_ConT_FH = 0;
		   	   
		   }
	   

		   else if (wParam == AVK_CLR)
		   {

			   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "*****newgimis_HandleEvent  wParam == AVK_CLR");

/////////////////////�Ǻŵ�����ֱ��CLR�����������/////////////////////////////////////////////////

			//	if (STRCMP(pMe->sMobileType,"66"))
			//	{
			//		IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect2);//?????????sr_Rect1
			//	} 
			//	else
			//	{
					IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);
			//	}

			   
			   if (pMe->pi_copy) 		
				{
					IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
				}
////////////////////////////////////2015-12-15�޸����////////////////////////////////////////////////////
			  // IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1); //2015-12-15�޸�
			

			   pMe->m_ConT_FH = 0;
		   }
		   else if (wParam == AVK_STAR) // *��
		   {
			    ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "*****newgimis_HandleEvent  wParam == AVK_STAR *");
			   pMe->m_ConT_FH = 1;
			   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect2);
		   }



//		   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"����ֵ ���� %d", wParam);

		   if (STRCMP(s_buf,pMe->m_ConText) != 0)  // m_ConText �����洢��Ƭ����༭���еı༭����
		   {
			   MEMSET(pMe->m_ConText,0x00,256);
			   STRCPY(pMe->m_ConText,s_buf);   

			   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect3);

			   

		   }

			
			//?????????????????????????????????????????????????????????????
		   if (wParam == AVK_SOFT1 || wParam == AVK_SOFT2 || wParam == AVK_END)
		   {
			   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "*****newgimis_HandleEvent  wParam == AVK_SOFT1 || wParam == AVK_SOFT2");
			   if (pMe->m_ConT_FH == 1)
			   {
				   if (pMe->pi_copy) 		
					{
						IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy,pMe);  //ע��ص�
					}
				   IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect2);
				   pMe->m_ConT_FH = 0;
			   }
		   }

		   ITEXTCTL_Redraw( pMe->textCtl );//ITEXTCTL_Redraw() ����֮ǰ��ʹ���ı��ؼ���������ݽ��������ɸ���Ҳ�������仯
		   IDISPLAY_Update(pMe->a.m_pIDisplay); //���ô˺���ʱ Emulator ������������Ļ BREW �豸�ϵĸ��¿���Ҫ�Ŷ�

#ifdef PHONE_QCHAT_DUIJIANG

		   if (pMe->m_F_DJ == 1)
		   {
			   // ˵�����ڶԽ�״̬
			   if (wParam == AVK_END)
			   {
				   return FALSE;
			   }
		   }
#else   
		   return TRUE;
#endif


		   return TRUE;
		}

//		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"����ֵ ���� %d", wParam);
	}








	////////////////////////////      ����һ���ֻ��ͺ� START     ///////////////////////
		////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////
		if (STRCMP(pMe->sMobileType,"58") == 0)
		{
			if((pMe->textCtl!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl)) && wParam == AVK_END)
			{

#ifdef PHONE_QCHAT_DUIJIANG

			   if (pMe->m_F_DJ == 1)
			   {
				   // ˵�����ڶԽ�״̬
				   if (wParam == AVK_END)
				   {
					   return FALSE;
				   }
			   }
#else   
				return TRUE;
#endif
				return TRUE;
			}
		 //  ���ڻ���һ���� * �������ַ��Ĵ���  ��Ϊ����������ܴ�
			if((pMe->textCtl!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl))&& ITEXTCTL_HandleEvent( pMe->textCtl, eCode, wParam, dwParam ))
			{
		   

//				ITEXTCTL_Redraw( pMe->textCtl );
//				IDISPLAY_Update(pMe->a.m_pIDisplay); 

				if (AVK_SELECT == wParam)
				{
					// 
					//	AEE_TM_PINYIN

					ITEXTCTL_GetText(pMe->textCtl,pMe->pText,100);
					WSTRTOSTR(pMe->pText,s_buf,100);

				}
		   
				return TRUE;
			}

			
/*
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"����ֵ ���� %d", wParam);
			if ((pMe->textCtl!=NULL) && ITEXTCTL_IsActive( pMe->textCtl))
			{
				if (eCode == EVT_KEY && (wParam == AVK_LEFT || wParam == AVK_RIGHT))
				{
					return TRUE;
				}

				if (eCode == EVT_KEY_RELEASE && (wParam == AVK_LEFT || wParam == AVK_RIGHT))
				{
					return TRUE;
				}
			}
*/
		}
		////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////////
		////////////////////////////        END                      ///////////////////////



		if (STRCMP(pMe->sMobileType,"55") == 0  || (STRCMP(pMe->sMobileType,"03") == 0)  || (STRCMP(pMe->sMobileType,"63") == 0) || STRCMP(pMe->sMobileType,"57")==0 )
		{
		 //  ���ڻ���һ���� * �������ַ��Ĵ���  ��Ϊ����������ܴ�
			if((pMe->textCtl!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl)) && wParam == AVK_END)
			{

#ifdef PHONE_QCHAT_DUIJIANG

			   if (pMe->m_F_DJ == 1)
			   {
				   // ˵�����ڶԽ�״̬
				   if (wParam == AVK_END)
				   {
					   return FALSE;
				   }
			   }
#else   
				return TRUE;
#endif	


				
				return TRUE;
			}
			if((pMe->textCtl!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl))&& ITEXTCTL_HandleEvent( pMe->textCtl, eCode, wParam, dwParam ))
			{
		   

//				ITEXTCTL_Redraw( pMe->textCtl );
//				IDISPLAY_Update(pMe->a.m_pIDisplay); 

				if (AVK_SELECT == wParam)
				{
					// 
					//	AEE_TM_PINYIN

					ITEXTCTL_GetText(pMe->textCtl,pMe->pText,100);
					WSTRTOSTR(pMe->pText,s_buf,100);

				}
		   
				return TRUE;
			}
		}


		//if (STRCMP(pMe->sMobileType,"15") == 0)
		if ((STRCMP(pMe->sMobileType,"15") == 0) || (STRCMP(pMe->sMobileType,"16" )== 0) )  //update: 2016-1-27
		{
			// �������һ��������Ǳ༭���ֵ�ʱ�򷵻ز��������ػ�����
			// EVT_CTL_TEXT_MODECHANGED -- ��ʾ 			// EVT_CTL_TEXT_MODECHANGED -- ��ʾ 
		 //  ���ڻ���һ���� * �������ַ��Ĵ���  ��Ϊ����������ܴ�
			//  m_ys_code ; //���������˼����� ecode ����ֵ  // ��������ֵ��519 ��˵�����ڱ�д����  ������� 272 (EVT_KEY_HOOK) ���� 
			if((pMe->textCtl!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl)) && wParam == AVK_END)
			{

#ifdef PHONE_QCHAT_DUIJIANG

			   if (pMe->m_F_DJ == 1)
			   {
				   // ˵�����ڶԽ�״̬
				   if (wParam == AVK_END)
				   {
					   return FALSE;
				   }
			   }
#else   
				return TRUE;
#endif

				return TRUE;
			}
			if((pMe->textCtl!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl))&& ITEXTCTL_HandleEvent( pMe->textCtl, eCode, wParam, dwParam ))
			{
//				ITEXTCTL_Redraw( pMe->textCtl );
//				IDISPLAY_Update(pMe->a.m_pIDisplay); 


				if (pMe->m_ys_code == EVT_KEY_HOOK && AVK_SELECT == wParam) // ˵�����˳��༭����
				{
					//
					IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );
					struct _AEETextInputModeInfo *pst_tmp;
					pst_tmp = (struct _AEETextInputModeInfo *) MALLOC(sizeof(struct _AEETextInputModeInfo));
					ITEXTCTL_GetInputMode(pMe->textCtl,pst_tmp);
//					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"struct _AEETextInputModeInfo *pst_tmp 1 = %d",pst_tmp->tmMode);

						
					
					
						if (pMe->pi_copy)
						{
							IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy_15,pMe);  //ע��ص�
							//	IIMAGE_Release(pMe->pi_copy); 
						}
						else
						{
							pMe->pi_copy = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);
							IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy_15,pMe);  //ע��ص�
						}
						
						
						
						IMENUCTL_Redraw( pMenuCtl );
		//				ITEXTCTL_Redraw( pMe->textCtl );
						IDISPLAY_Update(pMe->a.m_pIDisplay); 
						pMenuCtl = NULL;
//					}
					FREE(pst_tmp);
					pst_tmp = NULL;
					
				}

				if (AVK_SELECT == wParam)
				{
					// 
					//	AEE_TM_PINYIN
					
					struct _AEETextInputModeInfo *pst_tmp;
					pst_tmp = (struct _AEETextInputModeInfo *) MALLOC(sizeof(struct _AEETextInputModeInfo));
					ITEXTCTL_GetInputMode(pMe->textCtl,pst_tmp);
//					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"struct _AEETextInputModeInfo *pst_tmp = %d",pst_tmp->tmMode);

					if (pst_tmp->tmMode == AEE_TM_NUMBERS) //����
					{

						IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );
						if (pMe->pi_copy)
						{
							IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy_15,pMe);  //ע��ص�
							//	IIMAGE_Release(pMe->pi_copy); 
						}
						else
						{
							pMe->pi_copy = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);
							IImage_Notify(pMe->pi_copy, (PFNIMAGEINFO)uf_ImageNotify_copy_15,pMe);  //ע��ص�
						}
						IMENUCTL_Redraw( pMenuCtl );
		//				ITEXTCTL_Redraw( pMe->textCtl );
						IDISPLAY_Update(pMe->a.m_pIDisplay); 
						pMenuCtl = NULL;
					}

					ITEXTCTL_GetText(pMe->textCtl,pMe->pText,100);
					WSTRTOSTR(pMe->pText,s_buf,100);

					FREE(pst_tmp);
					pst_tmp = NULL;
	//				uf_RecLog(pMe,"��Ƭת�ɵ�����" );
	//				uf_RecLog(pMe,s_buf );
				}
				return TRUE;
			}


			if((pMe->textCtl!=NULL) &&  (ITEXTCTL_IsActive( pMe->textCtl)))
			{
		   
				pMe->m_ys_code = eCode;

//				return TRUE;
			}
		}













		
	////////////     END      ////////////////////////////////////////////////////////

	if (pMe->m_pHTMLViewer && IHTMLVIEWER_HandleEvent(pMe->m_pHTMLViewer, eCode, wParam, dwParam)&&(pMe->m_iAppStatus==1))
		return TRUE;


	/*
30672  <=  30679
#define EVT_STARTCALL_EVENT		EVT_USER+2000			// �������   
#define EVT_ENDCALL_EVENT		EVT_USER+2001			// ��������
#define EVT_RELEASEFLOOR_EVENT	EVT_USER+2002			// �ͷŻ���Ȩ
#define EVT_REQUESTFLOOR_EVENT	EVT_USER+2003			// ���뻰��Ȩ
#define EVT_CALLAGAIN_EVENT		EVT_USER+2004			// (���)ͬʱ�������ʱ����ͻ���ٷ���һ��, ����Ĵ�����nMyCallCnt����
#define EVT_SETPTTCALL_EVENT	EVT_USER+2005			// ����һ��������Ϣ
#define EVT_PTTCALL_EVENT		EVT_USER+2006			// ����һ������
#define EVT_UPDREST_EVENT		EVT_USER+2007			// ���º������ƣ��ڰ�������


	*/

	if(eCode == 30673 || eCode == 30674 || eCode == 30678)
//	if(eCode >= 30672 && eCode <= 30679)  // ��������⣬����ϵͳ���Լ������� ���뻰��Ȩ ����Ϣ
	{
		if (pMe->pRichptt != NULL)
	   {
			crichptt *me = pMe->pRichptt;
		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "ppt EVENT eCode = %d",eCode);


		   if ( richptt_HandleEvent(pMe,eCode,wParam,dwParam) )
			   return TRUE;
		   
	   }	

	}
	
	////////////////////////////////////////  ���end������eCode���������  //////////////////////////////////
	//pMe->m_FOBFlag == 0 Ӱ���̨��END  [3/22/2016 Administrator]
	if ((pMe->m_FOBFlag == 0 && pMe->m_WitchPag == 0 && AVK_END == wParam) || (pMe->m_WitchPag == 1 && AVK_END == wParam ) \
		|| (pMe->m_WitchPag == 25 && AVK_END == wParam) || (pMe->m_WitchPag == 25 && AVK_CLR == wParam))
	{
		//ֻ���� END��
		//uf_RecLog(pMe, "pMe->m_WitchPag == 0  AND  pMe->m_WitchPag == 1 END RETURN.");
		return TRUE;
	}

	if (1 == pMe->m_bMiss && AVK_END == wParam )
	{
		//ֻ���� END��
		//uf_RecLog(pMe, "1 == pMe->m_bMiss END RETURN.");
		return TRUE;
	}
	////////////////////////////////////////  end //////////////////////////////////


	////////////////////////////////////////  ������������  //////////////////////////////////
	// �����Ǵ��� �������տ�������û�н��뵽�����棬Ȼ�������˳�����ֻ�������� �����ﴦ��
	if ((pMe->m_WitchPag == 0 && eCode == EVT_KEY ) || (pMe->m_WitchPag == 1 && eCode == EVT_KEY) \
		|| (pMe->m_WitchPag == 25 && eCode == EVT_KEY) )
	{
		return TRUE;
	}
	
	if (1 == pMe->m_bMiss && eCode == EVT_KEY )
	{
		return TRUE;
	}
	////////////////////////////////////////  end //////////////////////////////////
	

    switch (eCode) 
	{
        case EVT_APP_START:      
			//#��̨ģʽ��������#	���̿�ʼ����
			if (1 == pMe->m_bDoTask)
			{
				uf_RecLog(pMe,"EVT_APP_START: GotoScreen SCREENID_NOTIFY ");
				pMe->m_bDoTask = 0;
				pMe->m_WitchPag = 21; 
				pMe->m_Cur_Page = 1 ;									// ��ʾÿ�ν�ȥ���ǵ�һҳ

				pMe->m_FOBFlag = 0;//  [3/10/2016 Administrator]
				
				IDISPLAY_Backlight(pMe->a.m_pIDisplay, TRUE); //������Ļ8S
				//ISHELL_SetTimer(pMe->a.m_pIShell, 8000, (PFNNOTIFY) Timer_Close_BackLight, (uint32*) pMe);
				ISHELL_Beep(pMe->a.m_pIShell, BEEP_MSG, TRUE);		
				ISHELL_Beep(pMe->a.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);	
	            ISHELL_SetTimer(pMe->a.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*)pMe );	

				testgps_GotoScreen( pMe, SCREENID_NOTIFY, 0 );
				return TRUE;
			}

// 			if (1 == pMe->m_bDoLongSMS)
// 			{
// 				uf_RecLog(pMe,"EVT_APP_START: GotoScreen SCREENID_LONGSMS_Notify �¶��� ��̨��ʾ");
// 
// 				pMe->m_bDoLongSMS = 0;
// 				pMe->m_WitchPag = 13;
// 				pMe->m_FOBFlag = 0;
// 
// 				IDISPLAY_Backlight(pMe->a.m_pIDisplay, TRUE); //������Ļ8S
// 
// 				testgps_GotoScreen( pMe, SCREENID_LONGSMS_Notify, 0 );
// 				return TRUE;
// 			}

		case EVT_APP_START_BACKGROUND:


		    // Add your code here...
	/*		IDISPLAY_DrawText(pMe->a.m_pIDisplay,    // Display instance
				AEE_FONT_BOLD,       // Use BOLD font
				szText,              // Text - Normally comes from resource
				-1,                  // -1 = Use full string length
				0,                   // Ignored - IDF_ALIGN_CENTER
				0,                   // Ignored - IDF_ALIGN_MIDDLE
				NULL,                // No clipping
				IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
         IDISPLAY_Update (pMe->a.m_pIDisplay);
		 */

		//	testgps_InitApplet( pMe );  
			//  m_FOBFlag ;//ǰ��̨�ı�־λ  0-��ǰ̨����  1-�ں�̨����
			pMe->m_F_CD = 0; //����Ƿ�Ϊ��� == 1 ��  == 0 ����

			pMe->m_bSendPF =FALSE ;//��������Ƿ����ڷ���ͼƬ�ı�־λ TRUE -- ��ʾ���ڷ���״̬  FALSE -- ��ʾ���� 
			pMe->m_bGetGPSF = FALSE;//������Ǵ�ʱ��ȡGPS��״̬  TRUE -- ��ʾ��ȡ�쳣 FLASE -- ��ȡ����


			pMe->clsKey[0] = 0;		// ��֤�����յ���ϼ�
			pMe->clsKey[1] = 0;		// ��֤�����յ���ϼ�

			STRCPY(pMe->sVer,DEFAULT_VERSION);					//  �汾��
			STRCPY(pMe->sVer_1,D_VERSION);					//  �汾��



			////   ��ȡ��Ļ��һЩ����  ////
			si_Height = IDISPLAY_GetFontMetrics( pMe->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL );//ϵͳ��׼������߶�
			if (si_Height > pMe->m_SYSFNH)
			{
				pMe->m_SYSCX = pMe->DeviceInfo.cxScreen;//ϵͳ��Ļ���
				pMe->m_SYSCY = pMe->DeviceInfo.cyScreen;//ϵͳ��Ļ�߶�
				pMe->m_SYSFNH = IDISPLAY_GetFontMetrics( pMe->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL );//ϵͳ��׼������߶�
				pMe->m_SYSFBH = IDISPLAY_GetFontMetrics( pMe->a.m_pIDisplay, AEE_FONT_BOLD, NULL, NULL );//ϵͳ�������߶�
//				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "start ��׼����߶� = %d",pMe->m_SYSFNH);
//				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "start �Ӵ�����߶� = %d",pMe->m_SYSFBH);
				pMe->m_T_L = pMe->m_SYSCY / pMe->m_SYSFBH; // �ܵ�����
			}


			/////////////////////////////////////////////
			///////////      START  ������δ����Ǵ����������ܵ�    //////////////////////////////////
			if ((eCode == EVT_APP_START) && (pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 5) )//˵���ǽ��뷢����Ƭ����
			{
				pMe->m_WitchPag = 2;
				pMe->m_FOBFlag = 0;//20180411
				newgimis_forphoto(pMe,TRUE);
				
				return TRUE;
			}

			if ((eCode == EVT_APP_START) && (pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 8) )//˵���ǽ��뷢����Ƶ���� 
			{
				pMe->m_WitchPag = 3;
				pMe->m_FOBFlag = 0;//20180411
				newgimis_forAudio(pMe,TRUE);
				
				return TRUE;
			}

			if ((eCode == EVT_APP_START) && (pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 3) )//˵���ǽ��뷢���س��Ľ���
			{
				pMe->m_WitchPag = 8;
				pMe->m_FOBFlag = 0;//20180411
				newgimis_SendFull(pMe, TRUE );
				
				return TRUE;
			}

			if ((eCode == EVT_APP_START) && (pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 4) )//˵���ǽ��뷢�Ϳճ��Ľ���
			{
				pMe->m_WitchPag = 9;
				pMe->m_FOBFlag = 0;//20180411
				newgimis_SendEmpty(pMe, TRUE );
				
				return TRUE;
			}

			if ((eCode == EVT_APP_START) && (pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 9) )//˵���ǽ��뷢��״̬ 1 �Ľ���
			{
				pMe->m_WitchPag = 8;
				pMe->m_FOBFlag = 0;//20180411
			//	newgimis_forAudio(pMe,TRUE);
				newgimis_Send_ZT_1(pMe,TRUE);
				
				return TRUE;
			}

			if ((eCode == EVT_APP_START) && (pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 10) )//˵���ǽ��뷢��״̬ 0 �Ľ���
			{
				pMe->m_WitchPag = 9;
				pMe->m_FOBFlag = 0;//20180411
			//	newgimis_forAudio(pMe,TRUE);
				newgimis_Send_ZT_0(pMe,TRUE);
				
				return TRUE;
			}

			if ((eCode == EVT_APP_START) && (pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 2) )//˵���ǽ��뷢�Ϳ��ڵĽ���
			{
				pMe->m_WitchPag = 1;
				pMe->m_FOBFlag = 0;//20180411
				newgimis_forYJKQ(pMe,TRUE);
				
				return TRUE;
			}

			if ((eCode == EVT_APP_START) && (pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 11) )//˵���ǽ���һ���򿨽���
			{
				pMe->m_WitchPag = 25;
				pMe->m_FOBFlag = 0;//20180411
				newgimis_forYJDK(pMe,TRUE);
				
				return TRUE;
			}

// 			if ((eCode == EVT_APP_START) && (pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 12) )//˵���ǽ����ռ������
// 			{
// 				pMe->m_WitchPag = 11;
// 
// 				pMe->m_Flag_Key = 0;
// 				pMe->m_Witch_Key = -1;
// 
// 				pMe->m_FOBFlag = 0;//20180411
// 
// 				testgps_GotoScreen( pMe, SCREENID_LONGSMS, wParam );
// 				
// 				return TRUE;
// 			}

			///////////      END    �����������ܽ���                 //////////////////////////////////
			
			///////////////////////////////
			// ��ʼ�� �ϱ���GPS����  ��ʾGPS��û�п���  ���Խ���˻غ�̨��������������������
			if (!pMe->GPSFlag)
			{
				MEMSET(pMe->lx_lon,0x00,MAXTEXTLEN);	// LO ����
				MEMSET(pMe->lx_lat,0x00,MAXTEXTLEN);
				MEMSET(pMe->lx_vv,0x00,MAXTEXTLEN);
				MEMSET(pMe->lx_tt,0x00,15);				// TT ʱ��
				pMe->lx_ep = '0';							// EP ��Դ
				pMe->lx_pm = '0';	
			}
			// PM ��λģʽ		
			////////  ����  //////
			
			if (!pMe->m_FOBFlag  &&  pMe->m_temp_FOBFlag == 0 ) 
			{//
				// ��ǰ̨���� 
				int nErr1 = 0;
				int nErr = 0;
				uint32 dwMask = 0;

				pMe->m_temp_FOBFlag = 1;
				
				MEMSET(pMe->sUrl_yjkq,0x00,200);// һ�����ڱ������� ��ʼ��Ϊ 0
				pMe->m_RelyExit = FALSE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨

				pMe->m_gpsFlag = 0;		// �������GPS��ģʽѡ��  0-����  1- local 2-network
				
				pMe->m_GPSGroup = 0;// ��ʾÿ�η������� �����Ƿ�������
				
				gimis_GetGPSPara(pMe); //��ȡ�����ļ�

				pMe->gpsSettings.optim  = AEEGPS_OPT_DEFAULT;
				pMe->gpsSettings.qos    = SPD_QOS_DEFAULT;
				pMe->gpsSettings.server.svrType = AEEGPS_SERVER_DEFAULT;

				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"gimis_GetGPSPara pMe->MyMobileNo = %s",pMe->MyMobileNo);

				uf_GetNetSts_imsi(pMe);// ��ȡimisi [3/14/2016 Administrator]

				//����gps����������������һ�����������õ�����ֵ����������pMe->m_RofflineData_Num  [3/9/2016 Administrator]
				pMe->m_RofflineData_Num = pMe->m_WofflineData_Num;
				//gimis_judgedata(pMe); // �ж���û����������
		
				pMe->m_Mutex_LX = 0; 

				//  ע��  ���� 
				nErr = ISHELL_RegisterNotify(pMe->a.m_pIShell, pMe->a.clsID,	AEECLSID_PHONENOTIFIER,	AEET_NMASK_VOICE_CALL | AEET_NMASK_SS);
				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"_RegisterRssiEvent in,register AEET_NMASK_SS return=%d",nErr);
				nErr = ISHELL_RegisterNotify(pMe->a.m_pIShell, pMe->a.clsID,	AEECLSID_TAPI,	NMASK_TAPI_STATUS);
				
				 dwMask = (((uint32)NOTIFIER_VAL_ANY)<<16) | NMASK_SHELL_KEY;  	
				 
				 nErr1 = ISHELL_RegisterNotify(pMe->a.m_pIShell, pMe->a.clsID, AEECLSID_SHELL, dwMask);

				//==================================

				 //  �����Ķ�ʱ����  //////////////////  ��ʵ�������ֻ�����һ��  ��ʼ������ ��ʱΪ90��
				 ISHELL_SetTimer(pMe->a.m_pIShell, 60000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);
				
				 //�����ļ��ϴ��Լ������ļ�ɾ��
				 ISHELL_SetTimer(pMe->a.m_pIShell, 180000, (PFNNOTIFY) init_check_gpsData, (uint32*) pMe);

				 //�������û��δ����Ϣ
				// load_LongSMS_Num(pMe);//pMe->m_LongSMS_uReadNum


				 //////////////////////////////////////
				if (eCode == EVT_APP_START_BACKGROUND)
				{
					pMe->m_FOBFlag = 1;
					pMe->m_Flag_Qqsq = 1;
					pMe->m_F_R_O = 0;   // ��������ǲ��������Ŀ���
					
					testgps_StartApplet_fu(pMe); // ��ʼ����Щ���溯����ָ��

					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"into EVT_APP_START_BACKGROUND");

					ISHELL_SetTimer(pMe->a.m_pIShell, 30000, (PFNNOTIFY) testgps_StartApplet_KJZQ, (uint32*) pMe);
					return TRUE;
				}
				else
				{
					pMe->m_WitchPag = 0;
					gimis_XTRA(pMe);		// ���²�����
				
					IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
					testgps_Printf( pMe, 0, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "ver = 3.1.2au" );
					testgps_Printf( pMe, 2, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "start ..." );
					
					pMe->m_PowerFlag = '1';
					gimis_SetPram_Open( pMe); 




//					uf_GetNetSts_imsi(pMe);
//					uf_GetNetSts(pMe);	
//					if ( !uf_CheckNetSts(pMe) )		// ����״̬�����粻���ã��������Ƕ�����λ
					if ( 0 )		// ����״̬�����粻���ã��������Ƕ�����λ
					{
						pMe->m_PosMode = TRUE;// ��������Ƿ�ֻ�д�����ģʽ  TRUE == ��ʾֻ�д�����ģʽ  FALSE == ���б��ģʽ
						/*
						MEMSET(psTextBuf,0x00,MAX_RES_STRING_BUF_SIZE);
						ISHELL_LoadResString( pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_NETSTATUSERR, psTextBuf, MAX_RES_STRING_BUF_SIZE );
						testgps_Printf_hanzi( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strerr );
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"��ǰ����״̬���쳣");
						*/
						pMe->m_F_NONET = TRUE; //��������ڿ�����ʱ���û������  TRUE -- û������  FALSE -- ������
						pMe->m_has_QQSQ = FALSE; //��������Ƿ�������Ȩ��  TRUE == ��ʾ������Ȩ�� FALSE == ��ʾ��û��������Ȩ
						pMe->m_Rely_XT = FALSE; //��������Ƿ�������Ҫ��������  TRUE = Ҫ��������  FALSE = ��ʾ�������Ҫ���ͣ�ֻ�ǽ�����ʱ
						ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) testgps_StartApplet, (uint32*) pMe);
						return(TRUE);
					}
					else
					{
						pMe->m_F_NONET = FALSE; //��������ڿ�����ʱ���û������  TRUE -- ������ʱ��û������  FALSE -- ������
						/*
						//		testgps_Printf( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "��ǰ����״̬������" );
						MEMSET(psTextBuf,0x00,MAX_RES_STRING_BUF_SIZE);
						ISHELL_LoadResString( pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_NETSTATUSNAL, psTextBuf, MAX_RES_STRING_BUF_SIZE );
						testgps_Printf_hanzi( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strnal );
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"��ǰ����״̬������ 1");
						*/
						
					}

					pMe->m_PosMode = FALSE;// ��������Ƿ�ֻ�д�����ģʽ  TRUE == ��ʾֻ�д�����ģʽ  FALSE == ���б��ģʽ
					pMe->i_HttpMod = 1; //HTTP���շ�������ģʽ ����	1 - ������ջ�������������Ȩ�� 2 - ���ջ�������GPS�����ϴ�����������
					pMe->m_Flag_Qqsq = 1 ; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����

					//////////////////   add   2015-11-4  start   ////////////////////////
					//  �������һ����ʱ�� ��������QCHAT  ��һЩ��ʼ������
					pMe->m_Acc = 3;
					ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY) newgimis_WaitForQChat, (uint32*) pMe);
					 
					//////////////////           end              ////////////////////////
//					MEMSET(pMe->m_squrl,0x00,250);
//					SPRINTF(pMe->m_squrl,"http://gps.richtalk.cn/brewGPS/BrewServlet?act=autdec&tel=%s&imsi=12345&ver=3.0.1ty&pp=FU/ctvH",pMe->MobileNoDJ);
//					myiweb_Start(pMe,pMe->m_squrl,0);

			//		myiweb_Start(pMe,TEST_URL,0);
					
				}
			
				
				
				
			//	ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) testgps_StartApplet, (uint32*) pMe);
			}
			else
			{
				// �ں�̨����  תǰ̨
			 //	ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, 	EVT_SCREEN,SCREEN_PARAM_GOTO, (uint32)pParam );
			//	ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, 	EVT_SCREEN,SCREEN_PARAM_GOTO, (uint32)dwParam );

				pMe->m_PowerFlag = '1';  // �����������ֵ��Ŀ���Ǳ������ǿ��������� �����ֻ������Ķ�ʱʱ�仹û�е��͵��ȥ��˵�����ʱ���Ҫ������

				// ��ǰ̨���� 
				pMe->m_FOBFlag = 0;
				pMe->m_WitchPag = 0;
//				pMe->m_gpsFlag = 0;		// �������GPS��ģʽѡ��  0-����  1- local 2-network
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"from background turn to front:%d",pMe->m_WitchPag);
				
//				pMe->m_GPSGroup = 0;// ��ʾÿ�η������� �����Ƿ�������
				
				pMe->gpsSettings.optim  = AEEGPS_OPT_DEFAULT;
				pMe->gpsSettings.qos    = SPD_QOS_DEFAULT;
				pMe->gpsSettings.server.svrType = AEEGPS_SERVER_DEFAULT;
				
				

				
				IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
				testgps_Printf( pMe, 0, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "ver = 3.1.2au" );
				testgps_Printf( pMe, 2, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "start ..." );

				if (pMe->m_Flag_Qqsq == 1) //��������Ƿ�����������Ȩ  1 -- ��  0-- ����)
				{
					testgps_Printf_hanzi( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strsqz );
					return(TRUE);
				}
	
				//  ���������Ȩ�У�������Ͳ���ʾ
//				uf_GetNetSts(pMe);				
//				if ( !uf_CheckNetSts(pMe) )		// ����״̬�����粻���ã��������Ƕ�����λ
				if ( 0 )		// ����״̬�����粻���ã��������Ƕ�����λ
				{
					//		testgps_Printf( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "��ǰ����״̬���쳣" );
					MEMSET(psTextBuf,0x00,MAX_RES_STRING_BUF_SIZE);
					ISHELL_LoadResString( pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_NETSTATUSERR, psTextBuf, MAX_RES_STRING_BUF_SIZE );
					testgps_Printf_hanzi( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strerr );
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"net status: exception");

					pMe->m_F_NONET = TRUE; //��������ڿ�����ʱ���û������  TRUE -- û������  FALSE -- ������
					pMe->m_Rely_XT = FALSE; //��������Ƿ�������Ҫ��������  TRUE = Ҫ��������  FALSE = ��ʾ�������Ҫ���ͣ�ֻ�ǽ�����ʱ

					//		SETAEERECT( &rRect, 0, 30 , 150, 22 );
					//		IDISPLAY_DrawRect( pMe->a.m_pIDisplay, &rRect, RGB_BLACK, RGB_WHITE, IDF_RECT_FRAME | IDF_RECT_FILL );
					//		IDISPLAY_DrawText( pMe->a.m_pIDisplay, AEE_FONT_BOLD, psTextBuf, -1, 0, 0, &rRect, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE );
					//		IDISPLAY_Update( pMe->a.m_pIDisplay );
					ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) testgps_StartApplet, (uint32*) pMe);
					return(TRUE);
				}
				else
				{
					pMe->m_F_NONET = FALSE; //��������ڿ�����ʱ���û������  TRUE -- û������  FALSE -- ������
					pMe->m_Rely_XT = TRUE; //��������Ƿ�������Ҫ��������  TRUE = Ҫ��������  FALSE = ��ʾ�������Ҫ���ͣ�ֻ�ǽ�����ʱ


					//		testgps_Printf( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "��ǰ����״̬������" );
					MEMSET(psTextBuf,0x00,MAX_RES_STRING_BUF_SIZE);
					ISHELL_LoadResString( pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_NETSTATUSNAL, psTextBuf, MAX_RES_STRING_BUF_SIZE );
					testgps_Printf_hanzi( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strnal );
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"net status:normal 2");

					ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY) testgps_StartApplet, (uint32*) pMe);
					return(TRUE);
					
				}


		//		ISHELL_SetTimer(pMe->a.m_pIShell, 3000, (PFNNOTIFY) testgps_StartBacktoforeApplet, (uint32*) pMe);
			}


            return(TRUE);


        // App is told it is exiting
        case EVT_APP_STOP:
            // Add your code here...
//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�ֻ��б�Ĵ��� ֹͣ����");
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"end of program 0");
			if(pMe->m_RelyExit == FALSE)//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
			{
				*((boolean *)dwParam) = FALSE;	
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�˵���̨");
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"end of program 3");
			}
			else
			{
				uint32 dwMask = 0;
				*((boolean *)dwParam) = TRUE;

				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"end of program 2");
				// �ͷ� http ����Դ      ��ʼ   ////////
				myiweb_Stop(pMe);
				myiweb_Stop_xt(pMe);
				myiweb_Stop_slx(pMe);
				myiweb_Stop_lx(pMe);
				myiweb_Stop_s(pMe);
				myiweb_Stop_gps(pMe);

				Socket_Clean_UDP(pMe);
				//////////////////////   ����   ////////

#ifdef PHONE_QCHAT_DUIJIANG

				if (pMe->pRichptt != NULL)     richptt_HandleEvent(pMe,EVT_APP_STOP,wParam,dwParam); //�ͷŶԽ���Դ
#endif

				////////////////  �ͷ���Դ ////////////////////
				if ( (STRCMP(pMe->sMobileType,"01") == 0) /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/)
				{
					if (pMe->pi_copy)
					{
						IIMAGE_Release(pMe->pi_copy); 
					}	
				}
				
				///////////////////////////////////////////////

				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"end of program");
				//  ע��  ����
			//	dwMask = (((uint32)NOTIFIER_VAL_ANY)<<16) | NMASK_SHELL_KEY;  	
				dwMask = (((uint32)NOTIFIER_VAL_ANY)<<16) &  0xFFBF;  	
				ISHELL_RegisterNotify(pMe->a.m_pIShell, pMe->a.clsID, AEECLSID_TAPI, 0);
				ISHELL_RegisterNotify(pMe->a.m_pIShell, pMe->a.clsID, AEECLSID_SHELL, 0);
				ISHELL_RegisterNotify(pMe->a.m_pIShell, pMe->a.clsID,	AEECLSID_PHONENOTIFIER,0);

//				IAPPLET_Release((IApplet*)pMe);
//				ISHELL_RegisterNotify(pMe->a.m_pIShell, pMe->a.clsID, AEECLSID_SHELL, dwMask);

				
				
			}
      		return(TRUE);


        // App is being suspended 
        case EVT_APP_SUSPEND:
		    // Add your code here...
//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�ֻ��б�Ĵ��� �������");

			switch(pMe->m_WitchPag)
				{
				case 10: //��ҳ��
	//				Show_Main_Fun( pMe );
					break;
				case 5: // �鿴GPSҳ��
	//				Show_GPS_Fun( pMe );
					break;
				case 2:	//������Ƭҳ��	
	//				Show_Photo_Fun( pMe );
					break;
				case 3:	//������Ƶҳ��
					Show_Audio_Fun_Sup( pMe );
					break;
				}

      		return(TRUE);

		case EVENT_GETBD_DATA:
			{
				// BD_DATA_FAILED_BDOPENED(int 0):��ȡ����ʧ�ܣ���λ�Ѿ���������û�ж���λ
				// BD_DATA_SUCCESSED(int 1): ��ȡ���ݳɹ�
				// BD_DATA_FAILED_BDCLOSED(int 2):��ȡ����ʧ�ܣ���λ��û�п���
				int mi_ret = 0;
				BdBdPositioningdate_n bd_value;
				MEMSET(&bd_value,0x00,sizeof(BdBdPositioningdate_n));
				uf_RecLog(pMe,"start get ITTSEXT_GetPositioning");
				
				pMe->m_gpsFlag = 10;

				mi_ret = ITTSEXT_GetPositioning(pMe->pIgpsext,&bd_value);
				if (mi_ret == BD_DATA_FAILED_BDOPENED) //��ȡ����ʧ�ܣ���λ�Ѿ���������û�ж���λ
				{
					uf_RecLog(pMe,"ITTSEXT_GetPositioning get pos fail");
					//��������
					ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_ERR_BD,0, 0);	 //��Ա�����ӵ�һ���¼�
				}
				else if (mi_ret == BD_DATA_SUCCESSED) //��ȡ���ݳɹ�
				{
					uf_RecLog(pMe,"ITTSEXT_GetPositioning get pos success");
					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Bd_STATUS = %s",bd_value.Bd_STATUS);
					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"lat = %s",bd_value.lat);
					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"lon = %s",bd_value.lon);
					
					if (STRCMP( (char *)bd_value.Bd_STATUS, "A") == 0)
					{
						uf_RecLog(pMe,"bd_value.Bd_STATUS is A get success data");
						//ͳ�ƶ�λ����
						pMe->m_Fix_C ++;
						//��γ��
						STRCPY(pMe->latStr_BD, (char *)bd_value.lat );
						STRCPY(pMe->lonStr_BD, (char *)bd_value.lon );
						pMe->dVelHor_BD = bd_value.SPD;
						//��λģʽ
						pMe->m_PM_BD = 13;
						//��������
						ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_NAL_BD,0, 0);	 //��Ա�����ӵ�һ���¼�
					}
					else
					{
						/*SPRINTF(pMe->show_LatLon,"0x100001   0x100001");*/
						//��������
				    	ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_ERR_BD,0, 0);	 //��Ա�����ӵ�һ���¼�
					}	
				}
				else if (mi_ret == BD_DATA_FAILED_BDCLOSED) //��ȡ����ʧ�ܣ���λ��û�п���
				{
					uf_RecLog(pMe,"ITTSEXT_GetPositioning get pos fail , no open GPS pos");
					//��������
					ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_ERR_BD,0, 0);	 //��Ա�����ӵ�һ���¼�
					newgimis_CloseGPS_UKBD(pMe);
					newgimis_Init_UKBD( pMe );
					newgimis_OpenGPS_UKBD(pMe);
				}	
			}
			return TRUE;
		case XTRA_EVT_USER_NAL_BD:
			{
				SendBDData(pMe);
			}
			return TRUE;
		case XTRA_EVT_USER_ERR_BD:
			{
				SendErrorBDData(pMe);
			}
			return TRUE;

			//GPS=====S958
		case EVENT_GETGPS_DATA:
			{
				//GPS_DATA_SUCCESSED,
				//GPS_DATA_FAILED_GPSOPENED,
				//GPS_DATA_FAILED_GPSCLOSED,
				int mi_ret = 0;
				GPSPositioningdate_n bd_value;
				MEMSET(&bd_value,0x00,sizeof(GPSPositioningdate_n));
				uf_RecLog(pMe,"start get ITTSEXTGPS_GetPositioning");
				
				pMe->m_gpsFlag = 0;	//������ʾ
				
				mi_ret = ITTSEXTGPS_GetPositioning(pMe->pS958gps,&bd_value);
				if (mi_ret == GPS_DATA_FAILED_GPSOPENED) //��ȡ����ʧ�ܣ���λ�Ѿ���������û�ж���λ
				{
					uf_RecLog(pMe,"ITTSEXT_GetPositioning get pos fail");
					//��������
					ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_ERR_BD,0, 0);	 //��Ա�����ӵ�һ���¼�
				}
				else if (mi_ret == GPS_DATA_SUCCESSED) //��ȡ���ݳɹ�
				{
					uf_RecLog(pMe,"ITTSEXT_GetPositioning get pos success");
					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"Bd_STATUS = %s",bd_value.Bd_STATUS);
					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"lat = %s",bd_value.lat);
					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"lon = %s",bd_value.lon);
					
					if (STRCMP( (char *)bd_value.Bd_STATUS, "A") == 0)
					{
						uf_RecLog(pMe,"bd_value.Bd_STATUS is A get success data");
						//ͳ�ƶ�λ����
						pMe->m_Fix_C ++;
						//��γ��
						STRCPY(pMe->latStr_BD, (char *)bd_value.lat );
						STRCPY(pMe->lonStr_BD, (char *)bd_value.lon );
						pMe->dVelHor_BD = bd_value.SPD;
						//��λģʽ
						pMe->m_PM_BD = 14;
						//��������
						ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_NAL_BD,0, 0);	 //��Ա�����ӵ�һ���¼�
					}
					else
					{
						/*SPRINTF(pMe->show_LatLon,"0x100001   0x100001");*/
						//��������
						ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_ERR_BD,0, 0);	 //��Ա�����ӵ�һ���¼�
					}	
				}
				else if (mi_ret == GPS_DATA_FAILED_GPSCLOSED) //��ȡ����ʧ�ܣ���λ��û�п���
				{
					uf_RecLog(pMe,"ITTSEXT_GetPositioning get pos fail , no open GPS pos");
					//��������
					ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_ERR_BD,0, 0);	 //��Ա�����ӵ�һ���¼�
					newgimis_CloseGPS_S958GPS(pMe);
					if (newgimis_Init_S958GPS( pMe ))
					{
						newgimis_OpenGPS_S958GPS(pMe);
					}			
				}	
			}
			return TRUE;
        // App is being resumed
        case EVT_APP_RESUME:
		    // Add your code here...
			if (!pMe->m_FOBFlag) //��ʾ��ǰ̨����
			{
				switch(pMe->m_WitchPag)
				{
				case 10: //��ҳ��
					Show_Main_Fun( pMe );
					ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);	
					break;
				case 5: // �鿴GPSҳ��
					Show_GPS_Fun( pMe );
					break;
				case 2:	//������Ƭҳ��	
					Show_Photo_Fun( pMe );
					break;
				case 3:	//������Ƶҳ��
					Show_Audio_Fun( pMe );
					break;
				case 4:	//��ʾ���� ���Ǹ�ҳ��
					Show_MorePag_Fun( pMe );
					break;
				case 11://��ʾ�ռ������
					Show_LongSMS(pMe);
					break;
				case 12:
					Show_LongSMS_Content(pMe);
					break;
				case 13:
					Show_LongSMS_Notify(pMe);
				case 16://��ʾ�����б�ҳ��
					Show_Task_List( pMe );
					break;
				case 17://��ʾ������������
					Show_Task_Detail1( pMe );
					break;
				case 18://��ʾ�������������
					Show_Task_Detail2( pMe );
					break;
				case 19://��ʾ�������ݽ���
					Show_Task_Content( pMe );
					break;
				case 21://��ʾ#��̨ģʽ��������# ����
					show_Notify( pMe );
					break;
				}
			}
//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�ֻ��б�Ĵ��� ���Ѳ���");
      		return(TRUE);

        // An SMS message has arrived for this app. Message is in the dwParam above as (char *)
        // sender simply uses this format "//BREW:ClassId:Message", example //BREW:0x00000001:Hello World
        case EVT_APP_MESSAGE:
		{
			// Add your code here...	//BREW:0111ab91:A;  ����Ƕ��Ÿ�ʽ
			char lChr_Ind;
			int si_Count = 0;
			char *sc_buf = NULL;
			char *sc_buf_tmp = NULL;
			JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
			
//			char* lstr_Payload = (char *)dwParam;   // �������   //BREW:01012EF:F3,1,180;
			sc_buf = (char *)MALLOC(512);
			MEMSET(sc_buf,0x00,512);
			STRCPY(sc_buf,(char *)dwParam);
			uf_RecLog(pMe,sc_buf);
			uf_RecLog(pMe,"recevice message");

			si_Count = STRLEN(sc_buf);

			lChr_Ind = sc_buf[0];
			if (lChr_Ind >= '0' && lChr_Ind <= '9') //˵���� ���Ͷ�λ����ָ��  ���ǲɼ�����ָ��  ��������ĳ��ʱ�������ֻ�
			{
				char *sc_buf1 = NULL;
				int si_Number = 0;
				sc_buf1 = (char *) MALLOC(512);
				MEMSET(sc_buf1,0x00,512);
				STRNCPY(sc_buf1,sc_buf,si_Count - 1);

				si_Number = ATOI(sc_buf1);
				pMe->iInterval = si_Number;
				gimis_SetGPSPara(pMe); 
				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "pMe->iInterval = %d",pMe->iInterval);

				if (pMe->cPosDetMode == 13)
				{
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�����ɼ����ڵ���1");
					newgimis_CloseGPS_UKBD( pMe );
					newgimis_Init_UKBD( pMe );
					newgimis_OpenGPS_UKBD(pMe);
				}

				if (pMe->cPosDetMode == 14)
				{
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"S958GPS�ɼ����ڵ���1");
					newgimis_CloseGPS_S958GPS(pMe);
					if (newgimis_Init_S958GPS( pMe ))
					{
						newgimis_OpenGPS_S958GPS(pMe);
					}
				}

				//ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY)uf_Pos_Track_stopstart_error, pMe);  // �л�ģʽ	�������Ǹı䶨λ����							
				FREE(sc_buf1);
				sc_buf1 = NULL;

				if (si_Count > 10)  // ��ĳ��ʱ�������ֻ�
				{
					if (STRCMP(sc_buf+2,"giermisset;") == 0)  
					{
						char *tmp = NULL;
						sc_buf_tmp = (char *)MALLOC(3);
						MEMSET(sc_buf_tmp,0x00,3);
						MEMCPY(sc_buf_tmp,sc_buf,2);
						si_Count = ATOI(sc_buf_tmp);
						GETJULIANDATE(0,&dDate);		//	ȡ������
						FREE(sc_buf_tmp);
						sc_buf_tmp = NULL;
						// dDate.wHour,dDate.wMinute,dDate.wSecond
						if (dDate.wMinute == si_Count) // ��������
						{
							STRCPY(tmp,"123");
						}
						else if (dDate.wMinute > si_Count )
						{
							if (dDate.wMinute == (si_Count + 1))
							{
								STRCPY(tmp,"123");
							}
						}
						else
						{
							if (dDate.wMinute == (si_Count - 1))
							{
								ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"dDate.wSecond = %d",dDate.wSecond);
								ISHELL_SetTimer(pMe->a.m_pIShell, (61 - dDate.wSecond ) * 1000, (PFNNOTIFY) uf_RestartPhone, (uint32*) pMe);
							}
						}
					}
					return TRUE;
				}

				return TRUE;
			}
			if (si_Count == 2)
			{
				lChr_Ind = sc_buf[0];
				switch(lChr_Ind)
				{
				case 'A': // ����
//					ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
					pMe->m_PowerFlag = '1';
					pMe->m_enable_evdo = 1;
					ISHELL_StartBackgroundApplet(pMe->a.m_pIShell, pMe->a.clsID,NULL);
					break;
				case 'B': // �ر�
					{
							pMe->m_PowerFlag = '0';
							gimis_SetPram_Open( pMe);
							
							ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY)newgimis_ufStopApp, pMe);
					}
					break;
				case 'G': // ����
					break;
				}
			}
			else if (si_Count == 3)  // H0 H1 H2 H3
			{
				char *sc_buf1 = NULL;
				char *sc_buf2 = NULL;
				int si_Number = 0;
				sc_buf1 = (char *)MALLOC(512);
				MEMSET(sc_buf1,0x00,512);
				STRNCPY(sc_buf1,sc_buf, 1);
				
				sc_buf2 = (char *)MALLOC(512);
				MEMSET(sc_buf2,0x00,512);
				STRNCPY(sc_buf2,sc_buf+1, 1); // 
				si_Number = ATOI(sc_buf2);

				if (STRCMP(sc_buf1, "H") == 0)		//H0\H1\H2\H3;
				{
					switch(si_Number)
					{
					case 0:							 //�ر�д��־
						{
							pMe->cWrtLog = '0';
							gimis_SetGPSPara( pMe);
							break;	
						}
					case 1:							//����д��־
						{
							pMe->cWrtLog = '1';
							gimis_SetGPSPara( pMe);
							break;
						}
					case 2:							//���������ļ�
						{
							if ( FleUpd_HandleEvent(pMe,FLDUPD_EVT_START,0,0) )	 
							{
								ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"upload configure file");
								ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,FLDUPD_EVT_UPLOAD,50,0);
							}
							break;	
						}
					case 3:							//������־�ļ�
						{
							int si_Ret = 0;
							si_Ret = juage_logfile(pMe);
							if (!si_Ret)
							{
								//						uf_AEPrint_Sending(pMe,FALSE);  // ������ ..
								pMe->mb_MsgLog = TRUE;
								if ( FleUpd_HandleEvent(pMe,FLDUPD_EVT_START,0,0) )	 
								{
									ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"upload Audio file");
									ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,FLDUPD_EVT_UPLOAD,51,0); // 51�����ֵ���Ǵ��� ��3�� ��־�ϴ�
								}
							}
							break;
						}
					}//END SWITCH
				}//END IF

				if (STRCMP(sc_buf1, "D") == 0)		//Dx ��λ��ʽ;
				{
					uf_RecLog(pMe,sc_buf1);
					uf_RecLog(pMe,sc_buf2);

					//ZEROAT(pMe->m_cPosDetMode);
					MEMSET(pMe->m_cPosDetMode, 0, sizeof(pMe->m_cPosDetMode));
					STRNCPY((pMe->m_cPosDetMode),sc_buf2,1);
					pMe->cPosDetMode = ATOI(pMe->m_cPosDetMode);

					//����ģʽ�л�
					if (pMe->cPosDetMode == 13)
					{
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�л�������1");
						newgimis_CloseGPS( pMe );
						newgimis_Init_UKBD( pMe );
						newgimis_OpenGPS_UKBD(pMe);
					}
					else if (pMe->cPosDetMode == 14)
					{
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�л���S958GPS1");
						newgimis_CloseGPS_S958GPS(pMe);
						if (newgimis_Init_S958GPS( pMe ))
						{
							newgimis_OpenGPS_S958GPS(pMe);
						}
					}
					else
					{
						newgimis_CloseGPS_S958GPS(pMe);
						newgimis_CloseGPS_UKBD(pMe);
						newgimis_OpenGPS(pMe);
						pMe->m_NOMAINPOS = 0;
					}

				}
				
				if (STRCMP(sc_buf1, "E") == 0)		//Ex ��λ����;
				{
					uf_RecLog(pMe,sc_buf1);
					uf_RecLog(pMe,sc_buf2);
					
					STRNCPY(&(pMe->cAccType),sc_buf2,1);
					pMe->m_Acc = pMe->cAccType - '0'; //�ȼ�����   ��λ���� '1'��'2'��'3'��'4'��'5'��'6',ȱʡΪ'3'
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," positionaI accuracy m_Acc = %d",pMe->m_Acc); 
				}		


				FREE(sc_buf1);
				sc_buf1 = NULL;
				FREE(sc_buf2);
				sc_buf2 = NULL;
			}
			else if (si_Count == 4)	
			{
				char *sc_buf1 = NULL;
				char *sc_buf2 = NULL;
				int si_Number = 0;
				sc_buf1 = (char *)MALLOC(512);
				MEMSET(sc_buf1,0x00,512);
				STRNCPY(sc_buf1,sc_buf, 2);

				sc_buf2 = (char *)MALLOC(512);
				MEMSET(sc_buf2,0x00,512);
				STRNCPY(sc_buf2,sc_buf+2, 1); // 
				si_Number = ATOI(sc_buf2);

				if (STRCMP(sc_buf1, "QD") == 0)		//QDE;
				{
// 					uf_RecLog(pMe,sc_buf1);
// 					switch(si_Number)
// 					{
// 					case 'A':
// 						uf_RecLog(pMe,sc_buf2);
// 						addbook_init(pMe);
// 						Del_all_addbook(pMe);
// 						addbook_free(pMe);
// 						break;
// 					}
					if (STRCMP(sc_buf2, "E") == 0)
					{
						addbook_init(pMe);
						Del_all_addbook(pMe);
						addbook_free(pMe);
					}
				}
				
				if(STRCMP(sc_buf1, "CE") == 0)	//CEx X=��Ŷ��ٴ�1289�󲥷�����
				{
					uf_RecLog(pMe,sc_buf1);
					uf_RecLog(pMe,sc_buf2);
					
					pMe->m_E_SC = si_Number;
					gimis_SetGPSPara( pMe);
					ud_RecLog(pMe->a.m_pIShell, 1 ,&(pMe->nInd)," continue error number m_E_SC = %d",pMe->m_E_SC); 
				}


				FREE(sc_buf1);
				sc_buf1 = NULL;
				FREE(sc_buf2);
				sc_buf2 = NULL;

			}

			else if (si_Count > 5) // ˵���� XXXXX;
			{
				char *sc_buf1 = NULL;
				char *sc_buf2 = NULL;
				int si_Number = 0;
				sc_buf1 = (char *)MALLOC(512);
				MEMSET(sc_buf1,0x00,512);
				STRNCPY(sc_buf1,sc_buf,4);

				sc_buf2 = (char *)MALLOC(512);
				MEMSET(sc_buf2,0x00,512);
				STRNCPY(sc_buf2,sc_buf+4,si_Count - 5); // si_Count - 4 -1
				si_Number = ATOI(sc_buf2);
//				uf_RecLog(pMe,"�������ݳ��ȴ���5");
				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"message content  %s", sc_buf2);

				
				if (STRCMP(sc_buf1,"HTEL") == 0) //�ֻ�����д�������ļ�
				{
					gimis_GetGPSPara(pMe); //��ȡ�����ļ�

					MEMSET(pMe->MyMobileNo, 0, sizeof(pMe->MyMobileNo));
					STRCPY((pMe->MyMobileNo),sc_buf2);

					gimis_SetGPSPara( pMe);

					pMe->m_PowerFlag = '1';
					pMe->m_enable_evdo = 1;

					ISHELL_StartBackgroundApplet(pMe->a.m_pIShell, pMe->a.clsID,NULL);
				}
				else if (STRCMP(sc_buf1,"HHAD") == 0) //�Ƿ��� 1#�˳� 1-�� 0-��
				{
					STRCPY(&(pMe->cCanExit),sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHHT") == 0)  // HT��ֵΪ30����������
				{
					pMe->ipulsation = si_Number;
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHHG") == 0)  //  HG��ֵΪ30���ɼ�����
				{
					pMe->iInterval = si_Number;
					gimis_SetGPSPara( pMe);

					if (pMe->cPosDetMode == 13)
					{
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�����ɼ����ڵ���2");
						newgimis_CloseGPS_UKBD( pMe );
						newgimis_Init_UKBD( pMe );
						newgimis_OpenGPS_UKBD(pMe);
					}
					if (pMe->cPosDetMode == 14)
					{
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"S958GPS�ɼ����ڵ���2");
						newgimis_CloseGPS_S958GPS(pMe);
						if (newgimis_Init_S958GPS( pMe ))
						{
							newgimis_OpenGPS_S958GPS(pMe);
						}
					}
					//ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY)uf_Pos_Track_stopstart_error, pMe);  // �л�ģʽ	�������Ǹı䶨λ����
				}
				else if (STRCMP(sc_buf1,"HHGE") == 0)  //  GE��ֵΪ1�����ȵȼ��� "ACCURACY_TYPE = "
				{
					STRCPY(&(pMe->cAccType),sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHGD") == 0)  //  GD��ֵΪ0����λ��ʽ   "POSDET_MODE = "	
				{
					//ZEROAT(pMe->m_cPosDetMode);
					MEMSET(pMe->m_cPosDetMode, 0, sizeof(pMe->m_cPosDetMode));
					STRCPY((pMe->m_cPosDetMode),sc_buf2);
					pMe->cPosDetMode = ATOI(pMe->m_cPosDetMode);
					//����ģʽ�л�
					if (pMe->cPosDetMode == 13)
					{
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�л�������2");
						newgimis_CloseGPS( pMe );
						newgimis_Init_UKBD( pMe );
						newgimis_OpenGPS_UKBD(pMe);
					}
					else if (pMe->cPosDetMode == 14)
					{
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�л���S958GPS2");
						newgimis_CloseGPS_S958GPS(pMe);
						if (newgimis_Init_S958GPS( pMe ))
						{
							newgimis_OpenGPS_S958GPS(pMe);
						}
					}
					else
					{
						newgimis_CloseGPS_S958GPS(pMe);
						newgimis_CloseGPS_UKBD(pMe);
						newgimis_OpenGPS(pMe);
						pMe->m_NOMAINPOS = 0;
					}
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHAY") == 0)  // AY��ֵΪ1����������		"ARRAY_SIZE = "
				{
					pMe->nArySize = si_Number;
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHCP") == 0)  //  ��ַָ�"CENTER_IP"
				{
					STRCPY(pMe->strCenterIp,sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHVR") == 0)  //  �汾��ָ��"VERSION"
				{
					////////////////////////////////
				}
				else if (STRCMP(sc_buf1,"HHGW") == 0)  // ���Ź�ָ��"WATCHDOG"��
				{
					STRCPY(&(pMe->cWatchDog),sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHGS") == 0)  //  ��������ָ��"START_MODE"
				{
					STRCPY(&(pMe->cStartMode),sc_buf2);
					gimis_SetPram_Open( pMe);
				}
				else if (STRCMP(sc_buf1,"HHAU") == 0)  // �Ƿ���Ȩָ��"IF_AUTHORIZE"
				{
					STRCPY(&(pMe->cAuthorize),sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHGC") == 0)  //  ����GPSָ��"GPS_MODE"
				{
					STRCPY(&(pMe->cGpsMode),sc_buf2);
					if ((pMe->cGpsMode == '1')&&(pMe->GPSFlag == FALSE)) // ˵������GPS
					{
						newgimis_OpenGPS(pMe);  // ֻ����GPS 
					}
					if ((pMe->cGpsMode == '0') && (pMe->GPSFlag == TRUE)) // ˵���ر�GPS
					{
						newgimis_CloseGPS(pMe);
					}
//					if (pMe->m_WitchPag == 10)
					if ((pMe->m_WitchPag) == 10 && (pMe->m_F_DJ != 1))
					{		
						ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
					}
				}
				else if (STRCMP(sc_buf1,"HHPC") == 0)  //  ���������͵�ͼƬ״̬"PIC_MODE"
				{
					STRCPY(&(pMe->cPicMode),sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHUL") == 0)  // �Ƿ�ʹ�ó�����"USE_LOAD"
				{
					STRCPY(&(pMe->cUseLoad),sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHPP") == 0)  //  ͼƬ��״̬"IF_PHOTO"
				{
					STRCPY(&(pMe->cUsePhoto),sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHPW") == 0)  //  ͼƬ���"PHOTO_WIDTH"��
				{
					pMe->iPhotoWidth = si_Number;
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHPH") == 0)  //  ͼƬ�߶�"PHOTO_HEIGHT"��
				{
					pMe->iPhotoHeight = si_Number;
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHST") == 0)  //  PHOTO_TYPE"�ֻ��ͺ�
				{
					STRCPY(pMe->sMobileType,sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHAL") == 0)  //  "ADD_HOUR"��Ƭ��Ƶʱ��
				{
					pMe->iAddHour = si_Number;
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHLG") == 0)  //  ������־��"WRITE_LOG"��
				{
					STRCPY(&(pMe->cWrtLog),sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHTA") == 0)  //  ���ŷ��͵�������-TA��Ϣָ����Ϻ󼸷�����������
				{
					pMe->iErrConTa = si_Number;
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHTB") == 0)  // ���ŷ��͵�������-TB��Ϣָ�ÿ������ʱ�Σ���
				{
					STRCPY(pMe->sRestTb,sc_buf2);
					//pMe->iRestTb = ATOI(pMe->sRestTb);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHLX") == 0)  //  gps��������������� ר������д�������ļ�
				{
					pMe->m_WofflineData_Num = si_Number;
					//gimis_SetGPSPara( pMe);�Ȳ�д��
				}
				else if (STRCMP(sc_buf1,"HHPL") == 0)  //  ������Ƭ·��  "PHOTO_PATH = "
				{
					STRCPY(pMe->sPhotoPath,sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHVL") == 0)  //  ������Ƶ·��  "VIDEO_PATH = "
				{
					STRCPY(pMe->sVideoPath,sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHAL") == 0)  //  ����¼��·��  "AUDREC_PATH = "
				{
					STRCPY(pMe->sAudRecPath,sc_buf2);
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHAG") == 0)  //  ���ø����ɼ�����  "ASSIST_GPS_INTERVAL ="
				{
					pMe->iAssInterval = si_Number;
					gimis_SetGPSPara( pMe);
				}
				else if (STRCMP(sc_buf1,"HHDC") == 0)
				{
					char *tmp = NULL;
					//ɾ�������ļ�
					Delete_file(pMe,SPD_CONFIG_FILE);
					
					STRCPY(tmp,"123");
				}
				

				FREE(sc_buf1);
				sc_buf1 = NULL;
				FREE(sc_buf2);
				sc_buf2 = NULL;
			}

			

			FREE(sc_buf);
			sc_buf = NULL;
			
		}
		break;
		    


      		return(TRUE);
		case XTRA_EVT_USER_QQSQ_CG:
			//ud_RecLog(pMe->a.m_pIShell ,1,&(pMe->nInd),"sq === %d  cp === %s", pMe->m_SQ_C,pMe->strCenterIp);
				if( STRSTR(pMe->strCenterIp, "autdec") != NULL && (pMe->m_SQ_C == 1 || pMe->m_SQ_C == 2) )
				{
					uf_RecLog(pMe,"arise autdec restart QQSQ..." );
					if (STRCMP(pMe->MobileNoDJ,pMe->MyMobileNo) == 0) // ˵�� ������ͬ
					{
						MEMSET(pMe->m_SQ1,0x00,250);
						SPRINTF(pMe->m_SQ1,"http://%s&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->strCenterIp,pMe->MobileNoDJ,pMe->szMobileID);
					}
					else
					{
						if (STRLEN(pMe->MyMobileNo) < 1)
						{
							MEMSET(pMe->m_SQ1,0x00,250);
							SPRINTF(pMe->m_SQ1,"http://%s&tel=%s&ver=3.1.2au&pp=FU/ctvH",pMe->strCenterIp,pMe->MobileNoDJ);
						}
						else
						{
							MEMSET(pMe->m_SQ1,0x00,250);
							SPRINTF(pMe->m_SQ1,"http://%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->strCenterIp,pMe->szMobileID);
						}

					}
					pMe->m_SQ_C = 1; //��ʹ������ַ
					myiweb_Stop(pMe);
					myiweb_Start(pMe,pMe->m_SQ1,0);
					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),pMe->m_SQ1);
					uf_RecLog(pMe, pMe->m_SQ1 );
					return TRUE;
				}
				
				pMe->m_enable_evdo = 1;//��Ȩ�ɹ���Ҫ����λ��1���������ܿ������������������汾������ֱ�����У�Ҫ���źű������ƣ���û������ʱ��
				//û���źű���������Ҫ����Ȩʱ����1��

				pMe->m_Flag_Qqsq = 0; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����
				pMe->m_gpsFlag = Get_GPS_Mode(pMe);
				pMe->m_PosMode = FALSE;// ��������Ƿ���ֻ���ô����Ƕ�λ TRUE -- ��ʾֻ���ô�����  FLASH -- ���� �����Ա�ķ�ʽ
				if (pMe->cGpsMode == '1') // ˵������GPS
				{
					ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) newgimis_OpenGPS, (uint32*) pMe);
				}
				
				pMe->m_Rely_XT = TRUE; //��������Ƿ�������Ҫ��������  TRUE = Ҫ��������  FALSE = ��ʾ�������Ҫ���ͣ�ֻ�ǽ�����ʱ
				uf_GetNetSts_imsi(pMe);
				if (!pMe->m_FOBFlag)
				{	
//					uf_GetNetSts(pMe);					
	//				if ( !uf_CheckNetSts(pMe) )		// ����״̬�����粻���ã��������Ƕ�����λ
					if ( 0 )		// ����״̬�����粻���ã��������Ƕ�����λ
					{
						MEMSET(psTextBuf,0x00,MAX_RES_STRING_BUF_SIZE);
						ISHELL_LoadResString( pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_NETSTATUSERR, psTextBuf, MAX_RES_STRING_BUF_SIZE );
						testgps_Printf_hanzi( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strerr );
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"net status:exception 3");
					}
					else
					{
						//		testgps_Printf( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "��ǰ����״̬������" );
						MEMSET(psTextBuf,0x00,MAX_RES_STRING_BUF_SIZE);
						ISHELL_LoadResString( pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_NETSTATUSNAL, psTextBuf, MAX_RES_STRING_BUF_SIZE );

						testgps_Printf_hanzi( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strnal );
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"net status: normal 3");
						
					}

					if (pMe->m_T_L > 6)
					{
						testgps_Printf_hanzi( pMe, 6, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strqqsqsuss );
					}
					else
					{
						testgps_Printf_hanzi( pMe, pMe->m_T_L - 1, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strqqsqsuss );	
					}

					pMe->m_PowerFlag = '1';
					gimis_SetPram_Open_QQSQ_CG(pMe);  // ����Ƿ�ֹû�д����ļ���
					gimis_SetPram_Open( pMe);
					
					ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) testgps_StartApplet, (uint32*) pMe);
				}
				else
				{
					pMe->m_PowerFlag = '1';
					gimis_SetPram_Open_QQSQ_CG(pMe);  // ����Ƿ�ֹû�д����ļ���
					gimis_SetPram_Open( pMe);
				}
				
				//  ����ʱ�� ������
			//	ISHELL_SetTimer(pMe->a.m_pIShell, pMe->ipulsation*1000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);

				if (pMe->m_OffData)
				{
					pMe->m_Mutex_LX = 1;
					newgimis_PostOffData( pMe );
				}
				

				// ������������ݾͿ��������ϴ�GPS����
				//	gimis_judgedata(pMe);
//				if (pMe->m_OffData)
//				{
//					ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY) testgps_StartSend_LXSJ, (uint32*) pMe);
//				}
				
					
				//#������ѯ�����б�# 2016-1-21		
				uf_RecLog(pMe,"##open boot find task list##");				
				pMe->m_TaskC = TASK_COUNT_EVERYPAGE;			//Ĭ��һҳ��ʾ5��			
				pMe->m_Task_F++ ;								// task_first ��������ǲ��ǳ����������һ�ν�����������ǵĻ���������ǰ������û��������ǰ��
				cTaskApp_HandleEvent(pMe,EVT_APP_START,20,0);
				me_t = pMe->pTaskApp;							//cTaskApp *me_t;
				cTaskApp_TaskInteract(me_t);
				
				//--��Ƶ�·�HTTP����--//
				if (pMe->iUDPPORT_YYXF != 0  && pMe->sUDPIP_YYXF !=NULL && pMe->iTCPPORT_YYXF != 0  && pMe->sTCPIP_YYXF !=NULL)
				{
					//���������·����ܹرն�λ��http�·�
//					pMe->cGpsMode = '0';
//					ISHELL_SetTimer(pMe->a.m_pIShell, 10000, (PFNNOTIFY) newgimis_CloseGPS, (uint32*) pMe);

					ISHELL_SetTimer(pMe->a.m_pIShell, 500, (PFNNOTIFY) initModule_yyxf, (uint32*) pMe);
				}

				
/*				//��Ȩ�ɹ���һ���ϴ���־��		//ƽ��ҽԺ�汾�п�����ͨ�汾�ر�
				if ( ! juage_logfile(pMe) )
				{
					pMe->mb_MsgLog = TRUE;
					if ( FleUpd_HandleEvent(pMe,FLDUPD_EVT_START,0,0) )	 
					{
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"XTRA_EVT_USER_QQSQ_CG success upload log file");
						ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,FLDUPD_EVT_UPLOAD,51,0); // 51�����ֵ���Ǵ��� ��3�� ��־�ϴ�
					}
				}
*/
				////////////////////////
				//#��̨ģʽ��������#	��̨����ʱ���и������񣬵�����ʾ�Ƿ�ʼ������ 2016-1-21
				//���� ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY)Timer_Task_Notify,pMe);
//				ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) SoundPlay_Start, (uint32*) pMe);

				return TRUE;
		case XTRA_EVT_USER_QQSQ_SB:
				pMe->m_Flag_Qqsq = 0; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����

				// ��Ȩʧ�ܺ󣬿���������
				pMe->m_PowerFlag = '1';
				gimis_SetPram_Open( pMe); 
				// �رճ���
				pMe->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨

				if (!pMe->m_FOBFlag)
				{
					if (pMe->m_T_L > 6)
					{
						testgps_Printf_hanzi( pMe, 6, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strqqsqfail );
					}
					else
					{
						testgps_Printf_hanzi( pMe, pMe->m_T_L - 1, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strqqsqfail );
					}

// 					// ��Ȩʧ�ܺ󣬿���������
// 					pMe->m_PowerFlag = '0';
// 					gimis_SetPram_Open( pMe); 
					// 					
					// 					// �رճ���
					// 					pMe->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
					
					ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_SQYC, (uint32*) pMe); //�˳�����
				}
				else
				{
					//�����õ�AU0�����testgps_CloseApplet_WSQ��ʱ���Ͽ��ܻ�ͬ������Ϊ1�룬������
					//ISHELL_SetTimer(pMe->a.m_pIShell, 8*1000, (PFNNOTIFY)newgimis_ufStopApp, pMe);
					ISHELL_SetTimer(pMe->a.m_pIShell, 1*1000, (PFNNOTIFY)newgimis_ufStopApp, pMe);
				}
				
				return TRUE;
		case XTRA_EVT_USER_NAL_GPS:
			{
				SendGPSData(pMe);
			}
			return TRUE;
		case XTRA_EVT_USER_ERR_GPS:
			{
				SendErrorGPSData(pMe);
				//wifi ����λ�� 
			}
			return TRUE;

		case XTRA_EVT_USER_XT_CG:
			uf_RecLog(pMe,"XTRA_EVT_USER_XT_CG");
// 				if ((pMe->m_OffData == 1) && (pMe->m_Mutex_LX == 0))
// 				{
// 					//uf_RecLog(pMe,"XTRA_EVT_USER_XT_CG IN newgimis_PostOffData");
// 					newgimis_PostOffData( pMe );
// 				}
				
//                ///////////////////////GPS�������ݰ�/////////////////////////
// 			   if (0 != pMe->nKeepDays) //���������������Ϊ0����ô�ر��ϴ���
// 			   {
// 				   if (pMe->mux_7Day == 0)
// 				   {
// 					   send_GPSData_7Day(pMe);
// 					   if (pMe->nState_7Day == 1)
// 					   {
// 						   pMe->nState_7Day = 0;
// 						   if (Upload_Init_7Day(pMe) == 0){
// 							   Upload_Start_7Day(pMe);
// 						   }
// 						   else{
// 							   Upload_Stop_7Day(pMe);
// 						   }
// 					   }
// 				   }
// 			   }
				///////////////////////////////¼������//////////////////////
				if (pMe->m_F_DJ != 1 && pMe->m_enable_Audio == 1)
				{
					if (pMe->m_flag_Audio == 0)	
					{
						//uf_RecLog(pMe,"XTRA_EVT_USER_XT_CG newgimis_GetRecord into");
						newgimis_GetRecord(pMe);
					}
				}
				/////////////////////////
			break;
		case XTRA_EVT_USER_QQSQ_SB_WLYC:  
			pMe->m_Flag_Qqsq = 0; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����

			if (!pMe->m_FOBFlag)
			{
				if (pMe->m_T_L >9)
				{
					testgps_Printf_hanzi( pMe, 9, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strqqsqyc );
				}
				else
				{
					testgps_Printf_hanzi( pMe, pMe->m_T_L - 1, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strqqsqyc );
				}

				ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) testgps_StartApplet, (uint32*) pMe);
			}
			if (pMe->m_Error == 554)
			{
				pMe->m_PosMode = TRUE;// ��������Ƿ���ֻ���ô����Ƕ�λ TRUE -- ��ʾֻ���ô�����  FLASH -- ���� �����Ա�ķ�ʽ
				if (pMe->cGpsMode == '1') // ˵������GPS
				{
					newgimis_OpenGPS(pMe);  //ֻ����GPS �����Ƕ�λ�ķ�ʽ
				}
//				uf_RecLog(pMe,"�������� 123");
				pMe->m_Rely_XT = FALSE; //��������Ƿ�������Ҫ��������  TRUE = Ҫ��������  FALSE = ��ʾ�������Ҫ���ͣ�ֻ�ǽ�����ʱ
			}
			else
			{
				//  ����ʱ�� ������
				pMe->m_PosMode = FALSE;// ��������Ƿ���ֻ���ô����Ƕ�λ TRUE -- ��ʾֻ���ô�����  FLASH -- ���� �����Ա�ķ�ʽ
				if (pMe->cGpsMode == '1') // ˵������GPS
				{
					newgimis_OpenGPS(pMe);  // ֻ����GPS �����Ƕ�λ�ķ�ʽ
				}
//				ISHELL_SetTimer(pMe->a.m_pIShell, pMe->ipulsation*1000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);
//				uf_RecLog(pMe,"�������� 456");
				pMe->m_Rely_XT = TRUE; //��������Ƿ�������Ҫ��������  TRUE = Ҫ��������  FALSE = ��ʾ�������Ҫ���ͣ�ֻ�ǽ�����ʱ
			}
			
			

			
			

			// �رճ���
	//		pMe->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
	//		ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_SQYC, (uint32*) pMe);  //�˳�����
			return TRUE;

        // A key was pressed. Look at the wParam above to see which key was pressed. The key
        // codes are in AEEVCodes.h. Example "AVK_1" means that the "1" key was pressed.
   //     case EVT_KEY:
		    // Add your code here...

   //   		return(TRUE);
		case EVT_CHECKIN_EVENT:				// �ָ�����ʱ�����¼�
			{
				DBGPRINTF("zjie-CHECKIN_EVENT");
				if (pMe->nSavInd==0)
		//			uf_ReadCheckOn(pMe,'0');
				return	TRUE;
			}
		break;
		case EVT_SCREEN :
			 return testgps_ScreenHandler( pMe, eCode, wParam, dwParam );


		case EVT_NOTIFY:
			{	
				AEENotify* pNotify = (AEENotify*) dwParam;
				
				if (pNotify && (pNotify->cls == AEECLSID_PHONENOTIFIER)) 
				{ // _PHONENOTIFIER
			//		if ( gimis_RSSINotifier(pMe, eCode, wParam, dwParam ) )
			//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEECLSID_PHONENOTIFIER=%d   wParam",wParam);
			//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEECLSID_PHONENOTIFIER=%d   dwParam",dwParam);
			//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEECLSID_PHONENOTIFIER pNotify = %d",pNotify->dwMask);

					pMe->m_F_R_O = 1; //���յ�����ź�˵���������Ŀ���
//					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"come EVT_NOTIFY m_F_R_O = %d",pMe->m_F_R_O); 
//					uf_RecLog(pMe,"get AEECLSID_PHONENOTIFIER notify");
					if ((pNotify->dwMask & AEET_NMASK_SS ) == AEET_NMASK_SS )
					{
						AEETNotifyInfo *pNotifyInfo = (AEETNotifyInfo*) pNotify->pData;
						
//						ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"come EVT_NOTIFY AEET_NMASK_SS"); 

						if (pNotifyInfo->event  == AEET_EVENT_SS_HDR_RSSI) {	// evdo �ź�
//							ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "AEET_EVENT_SS_HDR_RSSI in");
							if (!gimis_HasStatus(pMe,USR_NMASK_SS_HDR_RSSI)) 
							{
								
								gimis_SetStatus(pMe,USR_NMASK_SS_HDR_RSSI);
								//gimis_1xStart(pMe);
								ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "evdo gimis_SetStatus ");
								if ( gimis_HasStatus(pMe,USR_NMASK_SS_HDR_RSSI) == USR_NMASK_SS_HDR_RSSI)
								{
									pMe->m_enable_evdo = 1;
									ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "edvo init success !!!");
								}
							}
						}
			//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEECLSID_PHONENOTIFIER AEETNotifyInfo = %d",pNotifyInfo->event);
					}

					return TRUE;
				}		// _PHONENOTIFIER
				
				if (pNotify && (pNotify->cls == AEECLSID_TAPI)) // event sender
				{
				//	if (gimis_TAPINotifier(pMe,eCode,wParam,dwParam))
						
						
			//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEECLSID_TAPI=%d",pMe->mInt_StartRun);

			//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEECLSID_TAPI pNotify = %d",pNotify->dwMask);
						return TRUE;
				}		
				
			
				if (pNotify && (pNotify->cls == AEECLSID_SHELL)) // event sender ������ʱ��ᴥ������¼�
				{	// -- shell
					// �����׶������Զ�����  temp zjie 
#ifdef PHONE_AUTO_START	
					if ((pNotify->dwMask & NMASK_SHELL_INIT) == NMASK_SHELL_INIT)   
					{   // �Զ�����
						// Add by zjie 2013-01-01

						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"NMASK_SHELL_INIT,mInt_StartRun=%d",pMe->mInt_StartRun);
						if (pMe->mInt_StartRun > 6) 
						{
							return FALSE;
						}
						// Add End
						gimis_InitGPSSettings( pMe );
						
						//�����������gps���ݵ�Ŀ¼
						init_gpsDir(pMe);
						init_smsDir(pMe);

						//	if ( (pMe->cStartMode == '1') || (pMe->cStartMode == 'A') )	
						if ( pMe->cStartMode == '1' )	
						{	// if -- a1
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"NMASK_SHELL_INIT,StartApplet start,cStartMode=%c",pMe->cStartMode);
				//		if (pMe->cStartMode == 'A') {	// Add 2013-04-12
				//			pMe->cStartMode = '0';
				//			gimis_SetGPSPara(pMe);
				//		}	// Add End
						pMe->mInt_StartRun = 1;

						// ���Ե�ʱ����
				//		pMe->cStartMode = '0';
				//		gimis_SetGPSPara(pMe);
							

						ISHELL_StartBackgroundApplet(pMe->a.m_pIShell, pMe->a.clsID,NULL);

//						uf_DelOff_File(pwa);


						}  // if -- a1
						else 
						{
							ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"NMASK_SHELL_INIT,StartApplet false");
						}

					}	// �Զ�����

#endif

					// ����������
					if ((pNotify->dwMask & NMASK_SHELL_KEY ) == NMASK_SHELL_KEY )  
					{	// if -- KEY
						if ( gimis_NotifyShellKey( pMe, eCode, wParam, dwParam ) )
							return TRUE;
			//				uf_RecLog(pMe,"�а�������");
					}	// if -- KEY	

				}

		
		}

			break;

		case SMS_EVT_DOWNLOAD:
			{
				//uf_RecLog(pMe,"IN case SMS_EVT_DOWNLOAD");
				Sms_HandleEvent(pMe,SMS_EVT_DOWNLOAD,wParam,dwParam);
				return TRUE;
			}
			break;
			
		case EVT_SAVE_SMS:
			{
				//uf_RecLog(pMe,"IN case EVT_SAVE_SMS");
				Sms_HandleEvent(pMe,EVT_SAVE_SMS,wParam,dwParam);
				return TRUE;
			}
			break;

        // If nothing fits up to this point then we'll just break out
        default:
			if (eCode != FLDUPD_EVT_UPLOAD)
			{
				if( pMe->currentHandler ) 
				{
//					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pMe->currentHandler=%d",eCode);
					return pMe->currentHandler( pMe, eCode, wParam, dwParam );
				}
				else if(eCode == 30673)
				{
					if (pMe->pRichptt != NULL)
				   {
					   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "ppt eCode 12321432 = %d",eCode);
					   if ( richptt_HandleEvent(pMe,eCode,wParam,dwParam) )
						   return TRUE;
					   
				   }	

				}
			}
			
            break;
   }
	
   if (pMe->pRichptt != NULL)
   {
//	   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "ppt eCode = %d",eCode);
	   if (eCode == 30675)
	   {
		   return TRUE;
	   }
	   if ( richptt_HandleEvent(pMe,eCode,wParam,dwParam) )
		   return TRUE;
	   
   }	
	

   if (pMe->FleUpdApp != NULL)
	   if ( FleUpd_HandleEvent(pMe,eCode,wParam,dwParam) )
			   return TRUE;

   return FALSE;
}


// this function is called when your application is starting up
boolean newgimis_InitAppData(newgimis* pMe)
{
	int m_nFontHeight = 0;
	IMenuCtl *pMenuCtl_t = NULL;
	AEERect iR_tmp;

    // Get the device information for this handset.
    // Reference all the data by looking at the pMe->DeviceInfo structure
    // Check the API reference guide for all the handy device info you can get
    pMe->DeviceInfo.wStructSize = sizeof(pMe->DeviceInfo);
    ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&pMe->DeviceInfo);

    // The display and shell interfaces are always created by
    // default, so we'll asign them so that you can access
    // them via the standard "pMe->" without the "a."
    pMe->pIDisplay = pMe->a.m_pIDisplay;
    pMe->pIShell   = pMe->a.m_pIShell;



//////////////////////////////��ʼ��/////////////////////////////////
	//���뷨��ʼ��
	pMe->m_ConT_FH = 0;
	
	
	//GPS�������ݰ�
//	pMe->m_offlineData_gps = 1; 
	pMe->m_Mutex_offgps = 0;
//	pMe->m_bFlg_errG = 0;

	//###����###2015-12-28//
	pMe->m_Task_fs = 0;
	pMe->m_bDoTask = 0;
	
	//WaitForQChatʧ��3�κ�Ͳ���WaitForQChat
	pMe->m_WaitForQChat_Num = 0;

	//��������
	pMe->iTBCT = 100;

	//��ʼ������ ��Ϊ��ƽ̨û��fk������ܼ����� ����Ϊ0��
	pMe->m_min_Accuracy = 1;

	//��¼���������ȴ��źŷ��͵Ĵ���
	pMe->m_evdo_Num = 0;

	MEMSET(pMe->m_readMsg_tcp,0,5120);

	pMe->m_Listen_Heart = 0;

	pMe->m_bDoLongSMS = 0;
	//�ӿڶ���flag
//	MEMSET(pMe->copy_sUrl_E, 0, sizeof(pMe->copy_sUrl_E));
//	MEMSET(pMe->copy_sUrl_GPS, 0, sizeof(pMe->copy_sUrl_GPS));
	//������һ��APP

//	if (SUCCESS == ISHELL_StartApplet(pMe->a.m_pIShell, 874831789))
//	{												
//		uf_RecLog(pMe, " ###########################ISHELL_StartApplet ###########################");
//	}
  

	
//	ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY)uf_GetNetSts1, pMe);
////////////////////////////2015-12-18�޸�///////////////////////////////////



    // Insert your code here for initializing or allocating resources...
	//�����ɹ��󣬽��ƶ���ppobj����
	if(//(ISHELL_CreateInstance(pIShell, AEECLSID_FILEMGR, (void**)(&pMe->m_pFileMgr)) != SUCCESS) ||
		(ISHELL_CreateInstance(pMe->pIShell, AEECLSID_HTML, (void**)(&pMe->m_pHTMLViewer)) != SUCCESS) ||
		//  (ISHELL_CreateInstance(pIShell, AEECLSID_NET, (void**)(&pMe->m_pINetMgr)) != SUCCESS) ||
		(ISHELL_CreateInstance(pMe->pIShell, AEECLSID_WEB, (void **)(&pMe->m_pIWeb)) != SUCCESS) ||
		(ISHELL_CreateInstance(pMe->pIShell, AEECLSID_WEB, (void **)(&pMe->m_pIWeb_s)) != SUCCESS) ||
		(ISHELL_CreateInstance(pMe->pIShell, AEECLSID_WEB, (void **)(&pMe->m_pIWeb_lx)) != SUCCESS) ||
      (ISHELL_CreateInstance(pMe->pIShell, AEECLSID_WEB, (void **)(&pMe->m_pIWeb_xt)) != SUCCESS))
	{
		// �˺������ڼ��� IApplet �ӿڶ�������ü��� ���ü���Ϊ 0  �� ʱ �����ڴ����ͷŸ� �����Ҳ�����Ч 
		
		IAPPLET_Release((IApplet*)pMe);
		return FALSE;
	}
	IHTMLVIEWER_SetNotifyFn(pMe->m_pHTMLViewer, (PFNHVIEWNOTIFY)myiwebhtml_NotifyCB, pMe);  // �˺�������֪ͨ�ص�
	IHTMLVIEWER_SetProperties(pMe->m_pHTMLViewer, HVP_SCROLLBAR);
	
	// Get device screen rect
	SETAEERECT(&pMe->m_rc, 0, 0, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen);  // �˺������ڳ�ʼ��������� ��Ⱥ͸߶ȵ�ָ������ 





	
		/////// �����ǻ�ȡ����ĸ߶� //////////////	

		pMe->m_SYSCX = pMe->DeviceInfo.cxScreen;//ϵͳ��Ļ���
		pMe->m_SYSCY = pMe->DeviceInfo.cyScreen;//ϵͳ��Ļ�߶�
		m_nFontHeight = IDISPLAY_GetFontMetrics( pMe->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL );
		pMe->m_SYSFNH = m_nFontHeight;//ϵͳ��׼������߶�	
		m_nFontHeight = IDISPLAY_GetFontMetrics( pMe->a.m_pIDisplay, AEE_FONT_BOLD, NULL, NULL );
		pMe->m_SYSFBH = m_nFontHeight;//ϵͳ�������߶�
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "normal font width = %d",pMe->m_SYSFNH);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "bold font height = %d",pMe->m_SYSFBH);

		pMe->m_T_L = pMe->m_SYSCY / pMe->m_SYSFBH; // �ܵ�����
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "total line  = %d",pMe->m_T_L);

		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "3.1.2au By:2018.05.23");

//		int m_MenuH;//�˵��ؼ��ĸ߶�  
//		int m_SYSFNH;//ϵͳ��׼������߶�
//		int m_SYSFBH;//ϵͳ�������߶�

		if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_SOFTKEYCTL, 
       (void **)&pMenuCtl_t ) == SUCCESS ) {
			IMENUCTL_AddItem( pMenuCtl_t, NEWGIMIS_RES_FILE, IDS_OPENGPS,
			MAINMENU_ITEM_YJKQ, NULL, (uint32)pMenuCtl_t );

			IMENUCTL_GetRect(pMenuCtl_t,&iR_tmp);
			pMe->m_MenuH = iR_tmp.dy;//�˵��ؼ��ĸ߶�  
			IMENUCTL_Release(pMenuCtl_t);
			pMenuCtl_t = NULL;

			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "menu height = %d",pMe->m_MenuH);
		}
		else
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "get menu height fail");
		}


//		if (pMe->m_T_L <= 7)
//		{
//			pMe->m_PhotoH = pMe->m_SYSCY - pMe->m_SYSFBH * 2 - pMe->m_MenuH - 5; // ��ʾ��Ƭ�߶�
//		}

//		else
//		{
			pMe->m_PhotoH = pMe->m_SYSCY - pMe->m_SYSFBH * 5 - pMe->m_MenuH; // ��ʾ��Ƭ�߶�
//		}
		
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "picture height = %d",pMe->m_PhotoH);
		
	  
		///////////////////////////////////////////


	
	
    // Initialize the IWeb with a few options
	{


		///////////////////////////////		���������ֻ��ϵ����ò��� ��ʼ
		
		int    i = 0;
		WebOpt awo[10]; 
		WebOpt awo_s[10];  //���ڷ���GPSֵ�Ĳ���
		WebOpt awo_xt[10];  //���ڷ����������Ĳ���
		WebOpt awo_lx[10];  //�����������ݵĲ���
		
		// set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)
		awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		
		// Marks the end of the array of WebOpts
		awo[i].nId  = WEBOPT_END;
		
		
		
		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb,awo);
		
		///////////////////////////////////////////// ����




		//////////////////////////////////////// 2015-7-27////////////////
			// set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)
		
		i = 0;
		awo_s[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo_s[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo_s[i].nId  = WEBOPT_PROXYSPEC;
		awo_s[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		
		// Marks the end of the array of WebOpts
		awo_s[i].nId  = WEBOPT_END;
		
		
		
		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb_s,awo_s);
		//////////////////////////////////////////////////////////////////

		/*
		///////////////////////  ģ�����ϵ����ò��� ��ʼ  ///////////////////////////
		
//		int    j = 0;
		
		
		i = 0;
		// set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)
		awo_s[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo_s[i].pVal = (void *)10000;
		i++;
		
		// test user-agent, uncomment this section to ship your own user-agent 
		// string. if unset, IWeb will send a default.  If set to NULL, no 
		// user agent header will be sent */
		
		// Set TEST_USER_AGENT in the NetDiagnostics project settings to all
		// shipping of your own user agent.
		
		/*
#ifdef TEST_USER_AGENT
		awo_s[i].nId  = WEBOPT_USERAGENT;
		awo_s[i].pVal = (void *)WEBBER_USERAGENT;
		i++;
#endif 
		
		// test nowaitconn, this only comes into effect if you build webber 
		// with multiple myiwebs (see the definition of struct Webber)
		awo_s[i].nId  = WEBOPT_FLAGS;
		awo_s[i].pVal = (void *)WEBREQUEST_NOWAITCONN;
		i++;
		
		
		// test forcenew, uncomment this section to try multiple simultaneous
		// "forced" new connections. Forced new connections are not kept alive
		// unless they are the first forced new connection to a host
#ifdef TEST_FORCENEWCONN
		awo_s[i].nId  = WEBOPT_FLAGS;
		awo_s[i].pVal = (void *)WEBREQUEST_FORCENEWCONN;
		i++;
#endif 
		
#ifdef TEST_USER_AGENT
		// turn off HTTP over HTTP proxying
		awo_s[i].nId  = WEBOPT_PROXYSPEC;
		awo_s[i].pVal = (void *)"http:///";
		i++;
		
		// turn on ALL proxying.  Proxyspecs are examined in WebOpt
		// order, so in this list, with the above and below PROXYSPECs,
		// everything except HTTP will be proxied through
		// http://webproxy.yourdomain.com:8080, (which you'll have to
		// set up to test, sorry
		awo_s[i].nId  = WEBOPT_PROXYSPEC;
		awo_s[i].pVal = (void *)"*:///http://webproxy.yourdomain.com:8080";
		i++;
#endif
		
		// Marks the end of the array of WebOpts
		awo_s[i].nId  = WEBOPT_END;
		
		// Add Options
      IWEB_AddOpt(pMe->m_pIWeb_s,awo_s);
	
	  
	  /////////////////////  ģ���������õĲ��� ����  //////////////////






	  ///////////////////   ����һ��  ////////////////////////////////////
	  	///////////////////////  ģ�����ϵ����ò��� ��ʼ  ///////////////////////////
		/*
		int    i = 0;
		WebOpt awo[10]; 
		
		// set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)
		awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo[i].pVal = (void *)10000;
		i++;
		
		// test user-agent, uncomment this section to ship your own user-agent 
		// string. if unset, IWeb will send a default.  If set to NULL, no 
		// user agent header will be sent */
		
		// Set TEST_USER_AGENT in the NetDiagnostics project settings to all
		// shipping of your own user agent.
		/*
		
#ifdef TEST_USER_AGENT
		awo[i].nId  = WEBOPT_USERAGENT;
		awo[i].pVal = (void *)WEBBER_USERAGENT;
		i++;
#endif 
		
		// test nowaitconn, this only comes into effect if you build webber 
		// with multiple myiwebs (see the definition of struct Webber)
		awo[i].nId  = WEBOPT_FLAGS;
		awo[i].pVal = (void *)WEBREQUEST_NOWAITCONN;
		i++;
		
		
		// test forcenew, uncomment this section to try multiple simultaneous
		// "forced" new connections. Forced new connections are not kept alive
		// unless they are the first forced new connection to a host
#ifdef TEST_FORCENEWCONN
		awo[i].nId  = WEBOPT_FLAGS;
		awo[i].pVal = (void *)WEBREQUEST_FORCENEWCONN;
		i++;
#endif 
		
#ifdef TEST_USER_AGENT
		// turn off HTTP over HTTP proxying
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"http:///";
		i++;
		
		// turn on ALL proxying.  Proxyspecs are examined in WebOpt
		// order, so in this list, with the above and below PROXYSPECs,
		// everything except HTTP will be proxied through
		// http://webproxy.yourdomain.com:8080, (which you'll have to
		// set up to test, sorry
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"*:///http://webproxy.yourdomain.com:8080";
		i++;
#endif
		
		// Marks the end of the array of WebOpts
		awo[i].nId  = WEBOPT_END;
		
		// Add Options
      IWEB_AddOpt(pMe->m_pIWeb,awo);

	  */
	  /////////////////////  ģ���������õĲ��� ����  //////////////////

	  //////////////////////////////////////////////////////////////////
	  ///////////////////////////////////   ����  //////////////////////


		i = 0;
		awo_xt[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo_xt[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo_xt[i].nId  = WEBOPT_PROXYSPEC;
		awo_xt[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		
		// Marks the end of the array of WebOpts
		awo_xt[i].nId  = WEBOPT_END;
		
		
		
		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb_xt,awo_xt);


		i = 0;
		awo_lx[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo_lx[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo_lx[i].nId  = WEBOPT_PROXYSPEC;
		awo_lx[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		
		// Marks the end of the array of WebOpts
		awo_lx[i].nId  = WEBOPT_END;
		
		
		
		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb_lx,awo_lx);


	}
	
   	pMe->m_BodyBuffer = NULL;
	pMe->m_BodySize = 0;
	pMe->m_BodyAllocSize = 0;
    // if there have been no failures up to this point then return success

	pMe->m_BodyBuffer_xt = NULL;		// Buffer to hold HTML data
	pMe->m_BodySize_xt = 0;			// Size of HTML data in buffer
	pMe->m_BodyAllocSize_xt = 0;	// Allocated size of HTML data buffer

	pMe->m_BodyBuffer_lx = NULL;		// Buffer to hold HTML data
	pMe->m_BodySize_lx = 0;			// Size of HTML data in buffer
	pMe->m_BodyAllocSize_lx = 0;	// Allocated size of HTML data buffer

    return TRUE;
}

// this function is called when your application is exiting
void newgimis_FreeAppData(newgimis* pMe)
{
    // insert your code here for freeing any resources you have allocated...

    // example to use for releasing each interface:
    // if ( pMe->pIMenuCtl != NULL )         // check for NULL first
    // {
    //    IMENUCTL_Release(pMe->pIMenuCtl)   // release the interface
    //    pMe->pIMenuCtl = NULL;             // set to NULL so no problems trying to free later
    // }
    //

}










/*======================================================================= 
Function: gimis_InitGPSSettings()

Description: 
   Initializes the GPS configuration to the default values.

Prototype:

   uint32 gimis_InitGPSSettings( CSamplePosDet *pMe );

�������:
   pMe: [in]. cgimis instance.

Return Value:

   SUCCESS - If the settings were initialized successfully.
   Returns error code otherwise.
 
Comments:  
   None

Side Effects: 
   None

See Also:
   None
=======================================================================*/
uint32 gimis_InitGPSSettings(newgimis *pMe)
{
   IFileMgr   *pIFileMgr = NULL;
   IFile      *pIConfigFile = NULL;
   uint32      nResult = 0;

    ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"_InitGPSSettings in");
	// Setup the default GPS settings
	pMe->gpsSettings.optim  = AEEGPS_OPT_DEFAULT;
	pMe->gpsSettings.qos    = SPD_QOS_DEFAULT;
	pMe->gpsSettings.server.svrType = AEEGPS_SERVER_DEFAULT;
	
	pMe->ipulsation = DEFAULT_PULSATION;				// ��������
	pMe->iInterval = DEFAULT_INTERVAL;					// ȱʡ�ɼ�����
	
	pMe->iAssInterval = pMe->iInterval;					// Add by zjie 2012-11-27 ������λ�ɼ�����

	STRCPY(pMe->strCenterIp,SPD_DEFAULT_CENTER_IP);	// ���Ķ�IP��ַ
	STRCPY(pMe->sVer,DEFAULT_VERSION);					//  �汾��
	pMe->cPosDetMode = DEFAULT_PosDetMode;				// ��λ��ʽ
	pMe->cAccType = DEFAULT_AccType;					// ��λ���ȣ�ȱʡ3��
	pMe->cWatchDog = DEFAULT_WatchDog;					// ���Ź�  0 ��Ҫ���Ź�
	pMe->cStartMode = DEFAULT_START;					// �Ƿ񿪻�������
	pMe->cAuthorize = DEFAULT_AUTHORIZE;				// ��Ȩ
	pMe->cGpsMode = DEFAULT_GpsMode;					// Gps״̬
	pMe->cPicMode = DEFAULT_PIC;						// ������ʱʹ�õ�ͼƬ
	pMe->nArySize = DEFAULT_ARRAY_SIZE;				// ����֮ǰ���漸������

	pMe->cCTAuth = DEFAULT_CT_AUTH;					// ��λ��Ȩ
	MEMSET(pMe->sVideoPath,0,sizeof(pMe->sVideoPath));
	STRCPY(pMe->sVideoPath,DEFAULT_PATH);
	MEMSET(pMe->sPhotoPath,0,sizeof(pMe->sPhotoPath));
	STRCPY(pMe->sPhotoPath,DEFAULT_PATH);
	pMe->cUseLoad = DEFAULT_UseLoad;					// ����װ��ģʽ
	pMe->cUsePhoto = DEFAULT_PHOTO;					// �й���Ƭģ��
	pMe->iPhotoWidth = DEFAULT_PHOTO_WIDTH;			
	pMe->iPhotoHeight = DEFAULT_PHOTO_HEIGHT;
	MEMSET(pMe->sMobileType,0,sizeof(pMe->sMobileType));		// �ֻ�����
	STRCPY(pMe->sMobileType,DEFAULT_PHOTO_TYPE);
	pMe->iFontWidth = DEFAULT_FONT_WIDTH;				// �ַ����,�˴�Ϊʵ��ÿ�ж��ٸ�����
	pMe->iFontHeight = DEFAULT_FONT_HEIGHT;				// �ַ��߶�,�˴�Ϊʵ��ÿ�ж��ٸ�����
	pMe->iAddHour = DEFAULT_ADD_HOUR;					// ʱ������
	pMe->mInt_ReStartGps = DEFAULT_ReStartGps;
	pMe->mInt_sot = DEFAULT_sot;
	pMe->cPhotoSend = DEFAULT_PHOTO_SEND;				
	pMe->iPosDetAtime =	DEFAULT_POSDET_ATIMES;				
	pMe->cWrtLog = DEFAULT_LOG;								
	pMe->cCanExit = DEFAULT_EXIT;						// Add by zjie 2013-01-01
	pMe->nPttkey = DEFAULT_PTTKEY;						// Add by zjie 2013-01-01


	MEMSET(pMe->sCusID,0,sizeof(pMe->sCusID));
	STRCPY(pMe->sCusID,DEFAULT_CT_CUSID);
	MEMSET(pMe->sPassword,0,sizeof(pMe->sPassword));
	STRCPY(pMe->sPassword,DEFAULT_CT_CUPWD);

	pMe->sPttCallNo = NULL;								// Add by zjie 2013-01-01
	pMe->sUsrGroup = NULL;								// Add by zjie 2013-01-01
    pMe->nKeepDays = DEFAULT_KEEP_DAY;					// Add 2013-04-27
	// Add 2013-08-19
	pMe->iErrConTa = DEFAULT_TA;						
	MEMSET(pMe->sRestTb,0,sizeof(pMe->sRestTb));	
	STRCPY(pMe->sRestTb,DEFAULT_TB);
	pMe->iRestTb = 25;
	// Add End

	// ����gps������������ [3/9/2016 Administrator]
	pMe->m_WofflineData_Num = DEFAULT_OFFLINNEDATA;

	//������������1289���ٴκ󲥷Ŵ�������
	pMe->m_E_SC = DEFAULT_CONNECT_ERROR;

	//�ҹ��ܼ��Ƿ��������� 0 1������
    pMe->m_Right_Key = DEFAULT_Right_Key  ;      //'0'ȱʡ������

	//�Ƿ�����Զ�¼���ϴ�����
	pMe->m_enable_Audio = DEFAULT_Enable_Audio ;//0ȱʡ������

	//�Ƿ��ͼƬ����MD5У��
	pMe->m_enable_MD5 = DEFAULT_Enable_MD5; //0ȱʡ������

   // Create the instance of IFileMgr
   nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
   if ( SUCCESS != nResult ) {
      return nResult;
   }

   nResult = IFILEMGR_Test( pIFileMgr, SPD_CONFIG_FILE );
   if ( SUCCESS == nResult ) {
      pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READ );
      if ( !pIConfigFile ) {
         nResult = EFAILED;
      }
      else {
         nResult = gimis_ReadGPSSettings( pMe, pIConfigFile );
      }
   }
   else {
      pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_CREATE );
      if ( !pIConfigFile ) 
         nResult = EFAILED;
      else 
         nResult = gimis_WriteGPSSettings( pMe, pIConfigFile );
   }

   IFILE_Release( pIConfigFile );
   IFILEMGR_Release( pIFileMgr );

   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_InitGPSSettings out");

   return nResult;
}


uint32 gimis_TEST_MOD_FILE( newgimis *pMe ) //���MOD�ļ���û�д���
{
	IFileMgr   *pIFileMgr = NULL;
   uint32      nResult = 0;

    ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"open flag in");
	pMe->m_PowerFlag = '0';

   // Create the instance of IFileMgr
   nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
   if ( SUCCESS != nResult ) {
      return nResult;
   }

   nResult = IFILEMGR_Test( pIFileMgr, SPD_MOD_FILE );
   if ( SUCCESS == nResult ) {
	   nResult = 1;
	return 1;
   }
   else {
	   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"NO MOD FILE");
	   nResult = 0;
     return 0;
   }


   if (pIFileMgr)
   {
	   IFILEMGR_Release( pIFileMgr );
   }
   

   

   return nResult;
}


uint32 gimis_Init_Pram_Open( newgimis *pMe ) //��ȡ����txt�ı�־λ �������Ҫ��Ҫ�Զ�����
{
   IFileMgr   *pIFileMgr = NULL;
   IFile      *pIConfigFile = NULL;
   uint32      nResult = 0;

    ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"open and read flag in");
	pMe->m_PowerFlag = '0';

   // Create the instance of IFileMgr
   nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
   if ( SUCCESS != nResult ) {
      return nResult;
   }

   nResult = IFILEMGR_Test( pIFileMgr, SPD_BOOT_FILE );
   if ( SUCCESS == nResult ) {
      pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_BOOT_FILE, _OFM_READ );
      if ( !pIConfigFile ) 
	  {
         nResult = EFAILED;
		 uf_RecLog(pMe,"Use Read Method open  PowerBoot file fail");
      }
      else 
	  {
         nResult = gimis_ReadPowerBoot( pMe, pIConfigFile );
      }
   }
   else {
      pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_BOOT_FILE, _OFM_CREATE );
      if ( !pIConfigFile ) 
	  {
         nResult = EFAILED;
		 uf_RecLog(pMe,"Use Create Method open  PowerBoot file fail");
	  }
      else 
	  {
 //        nResult = gimis_WritePowerBoot( pMe, pIConfigFile );
		   uf_RecLog(pMe,"Ignore the write operation");
	  }
   }

   IFILE_Release( pIConfigFile );
   IFILEMGR_Release( pIFileMgr );

   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_InitGPSSettings out");

   return nResult;
}




/*======================================================================= 
Function: gimis_WriteGPSSettings()
Description: 
   Write the GPS configuration settings from the configuration file.
=======================================================================*/
uint32 gimis_WriteGPSSettings( newgimis *pMe, IFile * pIFile )
{
   char    *pszBuf;
   int32    nResult;
   boolean  bSetUsrGroup = FALSE;  // �Ƿ��pMe->sUsrGroup�������ڴ�
   uint32 nRtn = EFAILED;

    ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"gimis_WriteGPSSettings in,mInt_StartRun=%d",pMe->mInt_StartRun);

    // Add by zjie 2012-09-06 ��δ��ʼ����ԭ������δ����������д�룬��������
//	if (pMe->mInt_StartRun < 20 )
//		return EFAILED;

	if (pMe->sUsrGroup == NULL)
	{															// Add by zjie 2013-01-01
		FileInfo fiInfo;		// �ļ���Ϣ
		char    *pszTok = NULL;
		if ( SUCCESS == IFILE_GetInfo( pIFile, &fiInfo ) ) { // if -- 0
			if ( fiInfo.dwSize != 0 ) {		// if -- 1
				pszBuf = MALLOC( fiInfo.dwSize );
				nResult = IFILE_Read( pIFile, pszBuf, fiInfo.dwSize );
				if ( (uint32)nResult == fiInfo.dwSize ) { // if -- 2
					pszTok = STRSTR( pszBuf, SPD_CONFIG_USRGROUP_STRING );		// Ԥ���������ȼ�
					if ( pszTok ) {
						pszTok = pszTok + STRLEN( SPD_CONFIG_USRGROUP_STRING );
						nResult = ud_DistToChar(pszTok,';');
						pMe->sUsrGroup = MALLOC( nResult+1 );
						bSetUsrGroup = TRUE;
						MEMSET(pMe->sUsrGroup,0,nResult+1);
						MEMCPY(pMe->sUsrGroup,pszTok, nResult);
					}
				} // if -- 2
				else
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"in gimis_WriteGPSSettings, read config file error fiInfo.dwSize=%d,nResult=%d",fiInfo.dwSize,nResult);
				FREEIF(pszBuf);
			}	// if -- 1
		} // if -- 0
	}																// Add End

	if (pMe->sUsrGroup != NULL) 			// Add by zjie 2013-01-01
		pszBuf = MALLOC( 1224 + 1 + STRLEN(pMe->sUsrGroup) );
    else		// Add End
	   pszBuf = MALLOC( 1224 );

   // Truncate the file, in case it already contains data
   IFILE_Truncate( pIFile, 0 );

   // Write out the optimization setting:
   SPRINTF( pszBuf, SPD_CONFIG_OPT_STRING"%d;\r\n", pMe->gpsSettings.optim );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   
   // Write out the QoS setting:
   SPRINTF( pszBuf, SPD_CONFIG_QOS_STRING"%d;\r\n", pMe->gpsSettings.qos );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   
   // Write out the server type setting:
   SPRINTF( pszBuf, SPD_CONFIG_SVR_TYPE_STRING"%d;\r\n", pMe->gpsSettings.server.svrType );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   
   if ( AEEGPS_SERVER_IP == pMe->gpsSettings.server.svrType ) {
      // Write out the IP address setting:
      INET_NTOA( pMe->gpsSettings.server.svr.ipsvr.addr, pszBuf, 50 );
      nResult = IFILE_Write( pIFile, SPD_CONFIG_SVR_IP_STRING, STRLEN( SPD_CONFIG_SVR_IP_STRING ) );
      if ( 0 == nResult ) {
		  goto ExitHere;
      }
      nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
      if ( 0 == nResult ) {
		  goto ExitHere;
      }
      nResult = IFILE_Write( pIFile, ";\r\n", STRLEN( ";\r\n" ) );
      if ( 0 == nResult ) {
		  goto ExitHere;
      }

      // Write out the port setting:
      SPRINTF( pszBuf, SPD_CONFIG_SVR_PORT_STRING"%d;\r\n", AEE_ntohs(pMe->gpsSettings.server.svr.ipsvr.port) );
      nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
      if ( 0 == nResult ) {
		  goto ExitHere;
      }
   }

// =========================== ����Ϊ���ò��� ==================================================
   // ��������
   SPRINTF( pszBuf, SPD_CONFIG_PULSATION"%d;\r\n", pMe->ipulsation );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   //  ����GPS�Ľ���ʱ����(�ɼ�����)
   SPRINTF( pszBuf, SPD_CONFIG_INTERVAL_GET_STRING"%d;\r\n", pMe->iInterval );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // Add by zjie 2012-11-27 ������λ�Ĳɼ�����
   SPRINTF( pszBuf, SPD_CONFIG_ASSIST_GPS_INTERVAL"%d;\r\n", pMe->iAssInterval );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // ���Ķ�IP��ַ
   SPRINTF( pszBuf, SPD_CONFIG_CENTER_IP_STRING"%s;\r\n", pMe->strCenterIp );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // �汾��
   SPRINTF( pszBuf, SPD_CONFIG_VERSION_STRING"%s;\r\n", pMe->sVer );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // ��λ��ʽ

   
	SPRINTF( pszBuf, SPD_CONFIG_POSDET_MODE_CHAR"%d;\r\n", pMe->cPosDetMode );   

   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

	// ��λ����
   SPRINTF( pszBuf, SPD_CONFIG_ACCURACY_CHAR"%c;\r\n", pMe->cAccType );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // ���Ź�
   SPRINTF( pszBuf, SPD_CONFIG_WATCHDOG_CHAR"%c;\r\n", pMe->cWatchDog );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // �Ƿ��Զ�����
   SPRINTF( pszBuf, SPD_CONFIG_START_CHAR"%c;\r\n", pMe->cStartMode );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // �Ƿ���Ȩ
   SPRINTF( pszBuf, SPD_CONFIG_AUTHORIZE"%c;\r\n", pMe->cAuthorize );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   
	// ����ʱGps״̬
   SPRINTF( pszBuf, SPD_CONFIG_GPS_MODE"%c;\r\n", pMe->cGpsMode );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // ������ʹ�õ�ͼƬ
   SPRINTF( pszBuf, SPD_CONFIG_PIC_CHAR"%c;\r\n", pMe->cPicMode );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

	// �Ƿ�ʹ�ð���װ��ģ��
   SPRINTF( pszBuf, SPD_CONFIG_USE_LOAD"%c;\r\n", pMe->cUseLoad );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

	// �Ƿ�ʹ����Ƭģ��
   SPRINTF( pszBuf, SPD_CONFIG_PHOTO"%c;\r\n", pMe->cUsePhoto );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // �ֻ����մ�ŵ�·��
   SPRINTF( pszBuf, SPD_PHOTO_PATH"%s;\r\n", pMe->sPhotoPath );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   
   // Add by zjie 2012-09-21 �ֻ���Ƶ��ŵ�·��
   SPRINTF( pszBuf, SPD_VIDEO_PATH"%s;\r\n", pMe->sVideoPath );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   // Add End 	

   // Add by zjie 2014-04-15 �ֻ�¼����ŵ�·��
   SPRINTF( pszBuf, SPD_AUDREC_PATH"%s;\r\n", pMe->sAudRecPath );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   // Add End 	

   // ��Ƭ���
   SPRINTF( pszBuf, SPD_PHOTO_WIDTH"%d;\r\n", pMe->iPhotoWidth );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // ��Ƭ�߶�
   SPRINTF( pszBuf, SPD_PHOTO_HEIGHT"%d;\r\n", pMe->iPhotoHeight );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // �ֻ��ͺ�
   SPRINTF( pszBuf, SPD_PHOTO_TYPE"%s;\r\n", pMe->sMobileType );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   //  ���÷���GPS��Ϣ֮ǰ���漸������
   SPRINTF( pszBuf, SPD_CONFIG_ARRAY_SIZE"%d;\r\n", pMe->nArySize );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // ��λ����ʱ����GPS
   SPRINTF( pszBuf, SPD_CONFIG_RSG"%d;\r\n", pMe->mInt_ReStartGps );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
	// ����GPS ʱʱ����
   SPRINTF( pszBuf, SPD_CONFIG_SOT"%d;\r\n", pMe->mInt_sot );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   //  �ַ����
   SPRINTF( pszBuf, FONT_WIDTH"%d;\r\n", pMe->iFontWidth );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   //  �ַ��߶�
   SPRINTF( pszBuf, FONT_HEIGHT"%d;\r\n", pMe->iFontHeight );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   // ʱ������
   SPRINTF( pszBuf, ADD_HOUR"%d;\r\n", pMe->iAddHour );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // Add by zjie 2012-05-29  �Ƿ�����������Ƭ
   SPRINTF( pszBuf, SPD_PHOTO_SEND"%c;\r\n", pMe->cPhotoSend );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   // Add End

   // Add by zjie 2012-07-20  ������λ����
   SPRINTF( pszBuf, SPD_CONFIG_POSDET_ATIMES"%d;\r\n", pMe->iPosDetAtime );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   // �Ƿ���д��־
   SPRINTF( pszBuf, SPD_CONFIG_LOG"%c;\r\n", pMe->cWrtLog );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
	// Add End

   SPRINTF( pszBuf, SPD_CONFIG_CT_AUTH"%c;\r\n", pMe->cCTAuth );	// �Ƿ�λ��Ȩ
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
	// ��λ��Ȩ�˻�
   SPRINTF( pszBuf, SPD_CONFIG_CUSID"%s;\r\n", pMe->sCusID );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
	// ��λ��Ȩ�˻�����
   SPRINTF( pszBuf, SPD_CONFIG_CUPWD"%s;\r\n", pMe->sPassword );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // Ԥ������	Add by zjie 2013-01-01
   if (pMe->sUsrGroup != NULL) {
	   SPRINTF( pszBuf, SPD_CONFIG_USRGROUP_STRING"%s;\r\n", pMe->sUsrGroup );
	   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
	   if ( 0 == nResult ) {
		   goto ExitHere;
	   }
   }

   // һ�����е��ֻ�����	
   if (pMe->sPttCallNo != NULL) {
	   SPRINTF( pszBuf, SPD_CONFIG_CALLNOS_STRING"%s;\r\n", pMe->sPttCallNo );
	   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
	   if ( 0 == nResult ) {
		   goto ExitHere;
	   }
   }
   // һ�����еİ���ֵ
   SPRINTF( pszBuf, SPD_CONFIG_PTT_KEY"%d;\r\n", pMe->nPttkey );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   // �Ƿ�����1#�˳�
   SPRINTF( pszBuf, SPD_CONFIG_EXIT"%c;\r\n", pMe->cCanExit );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   // Add 2013-04-27	
   SPRINTF( pszBuf, SPD_CONFIG_CHECKON_KEEP"%d;\r\n", pMe->nKeepDays );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
	// Add End
	// Add 2013-08-19
   SPRINTF( pszBuf, SPD_CONF_TA"%d;\r\n", pMe->iErrConTa );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   SPRINTF( pszBuf, SPD_CONF_TB"%s;\r\n", pMe->sRestTb );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   // Add End

   // Add  2013-10-01  ��qchat�ֻ�����
   SPRINTF( pszBuf, SPD_MOBILE_NO"%s;\r\n", pMe->MyMobileNo );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   // Add End	

   // Add  2017-08-30  IMEI��
   SPRINTF( pszBuf, SPD_MOBILE_IMEI"%s;\r\n", pMe->szMobileID );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   // Add End

   // ����gps������������ [3/9/2016 Administrator]
   SPRINTF( pszBuf, SPD_CONFIG_OFFLINNEDATA_MAX_STRING"%d;\r\n", pMe->m_WofflineData_Num );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   //ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"config.txt pMe->m_WofflineData_Num = %d",pMe->m_WofflineData_Num);
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   //END

   // ������������1289���ٴκ󲥷�һ�δ������� [3/14/2016 Administrator]
   SPRINTF( pszBuf, SPD_CONFIG_PLAY_ERROR_MP3_STRING"%d;\r\n", pMe->m_E_SC );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }
   //END

   // �ҹ��ܼ��Ƿ��˳������־λ
   SPRINTF( pszBuf, SPD_CONFIG_RIGHT_KEY_FLAG"%c;\r\n", pMe->m_Right_Key );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   	//�Ƿ�����Զ�¼���ϴ�����
   SPRINTF( pszBuf, SPD_CONFIG_ENABLE_AUDIO"%d;\r\n", pMe->m_enable_Audio );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }

   // �Ƿ��ͼƬ����MD5У��
   SPRINTF( pszBuf, SPD_CONFIG_ENABLE_MD5"%d;\r\n", pMe->m_enable_MD5 );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 == nResult ) {
	   goto ExitHere;
   }


	nRtn = SUCCESS;

ExitHere:
   FREEIF( pszBuf );
   if (bSetUsrGroup) 
	   FREEIF(pMe->sUsrGroup);

   return nRtn;
}




/*======================================================================= 
Function: gimis_WritePowerBoot
Description: 
   Write the GPS configuration settings from the configuration file.
=======================================================================*/
uint32 gimis_WritePowerBoot( newgimis *pMe, IFile * pIFile )
{
   char    *pszBuf;
   int32    nResult;

   uint32 nRtn = EFAILED;

   char    *pszTok = NULL;

   pszBuf = MALLOC( 20 );

   // Truncate the file, in case it already contains data
   IFILE_Seek(pIFile,_SEEK_START,0);
   IFILE_Truncate( pIFile, 0 );

	// ���������Ƿ������ı�־λ
   SPRINTF( pszBuf, SPD_BOOT_OPT_STRING"%c;\r\n", pMe->m_PowerFlag );
   nResult = IFILE_Write( pIFile, pszBuf, STRLEN( pszBuf ) );
   if ( 0 != nResult ) {
		  nRtn = SUCCESS;
   }
   FREEIF( pszBuf );
   return nRtn;
}



/*======================================================================= 
Function: gimis_ReadGPSSettings()

Description: 
   Reads the gimis configuration settings from the configuration file.

Parameters:
   pMe: [in]. gimis instance.
   pIFile: [in].  Pointer to the IFile instance representing the config
                 file.
Return Value:
  SUCCESS - If the settings were read successfully.
  Error code otherwise.
=======================================================================*/
uint32 gimis_ReadGPSSettings(newgimis *pMe, IFile * pIFile)
{
   char    *pszBuf = NULL;
   char    *pszTok = NULL;
   char    *pszSvr = NULL;
   char    *pszDelimiter = ";";
   int32   nResult = 0;
   FileInfo fiInfo;

   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_ReadGPSSettings in,mInt_StartRun=%d",pMe->mInt_StartRun);

   if ( SUCCESS != IFILE_GetInfo( pIFile, &fiInfo ) ) {
      return EFAILED;
   }

   if ( fiInfo.dwSize == 0 ) {
      ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_ReadGPSSettings in,fiInfo.dwSize=0");
	  ISHELL_SetTimer(pMe->a.m_pIShell, 200, (PFNNOTIFY)gimis_SetGPSPara, pMe);		// Add 2013-10-01
      return EFAILED;
   }

   // Allocate enough memory to read the full text into memory
   pszBuf = MALLOC( fiInfo.dwSize );

   nResult = IFILE_Read( pIFile, pszBuf, fiInfo.dwSize );
   if ( (uint32)nResult < fiInfo.dwSize ) {
      FREE( pszBuf );
      return EFAILED;
   }

   // Check for an optimization mode setting in the file:
   pszTok = STRSTR( pszBuf, SPD_CONFIG_OPT_STRING );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_OPT_STRING );
      pMe->gpsSettings.optim = (AEEGPSOpt)STRTOUL( pszTok, &pszDelimiter, 10 );
   }

   // Check for a QoS setting in the file:
   pszTok = STRSTR( pszBuf, SPD_CONFIG_QOS_STRING );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_QOS_STRING );
      pMe->gpsSettings.qos = (AEEGPSQos)STRTOUL( pszTok, &pszDelimiter, 10 );
   }

   // Check for a server type setting in the file:
   pszTok = STRSTR( pszBuf, SPD_CONFIG_SVR_TYPE_STRING );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_SVR_TYPE_STRING );
      pMe->gpsSettings.server.svrType = STRTOUL( pszTok, &pszDelimiter, 10 );

      // If the server type is IP, we need to find the ip address and the port number
      if ( AEEGPS_SERVER_IP == pMe->gpsSettings.server.svrType ) {
         pszTok = STRSTR( pszBuf, SPD_CONFIG_SVR_IP_STRING );
         if ( pszTok ) {
            pszTok = pszTok + STRLEN( SPD_CONFIG_SVR_IP_STRING );
            nResult = ud_DistToChar(pszTok,';');  //uf_DistToSemi( pszTok );
            pszSvr = MALLOC( nResult+1 );
            STRNCPY( pszSvr, pszTok, nResult );
            *(pszSvr+nResult) = 0;  // Need to manually NULL-terminate the string
            if ( !INET_ATON( pszSvr, &pMe->gpsSettings.server.svr.ipsvr.addr ) ) {
               FREE( pszBuf );
               FREE( pszSvr );
               return EFAILED;
            }
            FREE( pszSvr );
         }
         pszTok = STRSTR( pszBuf, SPD_CONFIG_SVR_PORT_STRING );
         if ( pszTok ) {
            pszTok = pszTok + STRLEN( SPD_CONFIG_SVR_PORT_STRING );
            pMe->gpsSettings.server.svr.ipsvr.port = AEE_htons((INPort)STRTOUL( pszTok, &pszDelimiter, 10 ));
         }
      }
   }

// =========================== ����Ϊ���ò��� ==================================================
   // ��������
   pszTok = STRSTR( pszBuf, SPD_CONFIG_PULSATION );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_PULSATION );
      pMe->ipulsation = STRTOUL( pszTok, &pszDelimiter, 10 );
   }
   else
	  pMe->ipulsation = DEFAULT_PULSATION;	


   // ��ȡ����GPS��ʱ����(�ɼ�����)
   pszTok = STRSTR( pszBuf, SPD_CONFIG_INTERVAL_GET_STRING );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_INTERVAL_GET_STRING );
      pMe->iInterval = STRTOUL( pszTok, &pszDelimiter, 10 );
   }
   else
	  pMe->iInterval = DEFAULT_INTERVAL;	

	// Add by zjie 2012-11-27 ������λ�ɼ�����
   pszTok = STRSTR( pszBuf, SPD_CONFIG_ASSIST_GPS_INTERVAL );
   if ( pszTok ) {
	   pszTok = pszTok + STRLEN( SPD_CONFIG_ASSIST_GPS_INTERVAL );
	   pMe->iAssInterval = STRTOUL( pszTok, &pszDelimiter, 10 );
	   if (pMe->iAssInterval == 0)
			pMe->iAssInterval = pMe->iInterval;
   }
   else
	   pMe->iAssInterval = pMe->iInterval;

	// ���ĵ�IP��ַ
   pszTok = STRSTR( pszBuf, SPD_CONFIG_CENTER_IP_STRING );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_CENTER_IP_STRING );
      nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
	  MEMSET(pMe->strCenterIp,0,sizeof(pMe->strCenterIp));
	  if (nResult < sizeof(pMe->strCenterIp))				// 2013-05-20
		 MEMCPY(pMe->strCenterIp,pszTok, nResult);
	  else
		 STRCPY(pMe->strCenterIp,SPD_DEFAULT_CENTER_IP);	
   }
   else
	  STRCPY(pMe->strCenterIp,SPD_DEFAULT_CENTER_IP);	

	// ��λ��ʽ
   pszTok = STRSTR( pszBuf, SPD_CONFIG_POSDET_MODE_CHAR );
   if ( pszTok ) {
	   pszTok = pszTok + STRLEN( SPD_CONFIG_POSDET_MODE_CHAR );
	   pMe->cPosDetMode = STRTOUL( pszTok, &pszDelimiter, 10 );
   }
   else
	  pMe->cPosDetMode = DEFAULT_PosDetMode;			//  ȱʡ OneShot ��ʽ

   // ��λ����
   pszTok = STRSTR( pszBuf, SPD_CONFIG_ACCURACY_CHAR );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_ACCURACY_CHAR );
      nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
      pszSvr = MALLOC( nResult+1 );
	  MEMSET(pszSvr,0,nResult+1);
      MEMCPY(pszSvr,pszTok, nResult);
	  ud_Trim(pszSvr);
	  pMe->cAccType = *pszSvr;
	  FREE(pszSvr);	
   }
   else
	  pMe->cAccType = DEFAULT_AccType;			//  ȱʡ 

	// ���Ź�
   pszTok = STRSTR( pszBuf, SPD_CONFIG_WATCHDOG_CHAR );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_WATCHDOG_CHAR );
      nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
      pszSvr = MALLOC( nResult+1 );
	  MEMSET(pszSvr,0,nResult+1);
      MEMCPY(pszSvr,pszTok, nResult);
	  ud_Trim(pszSvr);
	  pMe->cWatchDog = *pszSvr;
	  FREE(pszSvr);	
   }
   else
	  pMe->cWatchDog = DEFAULT_WatchDog;			//  ȱʡ ��Ҫ���Ź� ��ʽ

    // �Ƿ񿪻�������
   pszTok = STRSTR( pszBuf, SPD_CONFIG_START_CHAR );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_START_CHAR );
      nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
      pszSvr = MALLOC( nResult+1 );
	  MEMSET(pszSvr,0,nResult+1);
      MEMCPY(pszSvr,pszTok, nResult);
	  ud_Trim(pszSvr);
	  pMe->cStartMode = *pszSvr;
	  FREE(pszSvr);	
   }
   else
	  pMe->cStartMode = DEFAULT_START;			//  ȱʡ ��1�β�Ҫ������ ��ʽ

   // ��Ȩ
   pszTok = STRSTR( pszBuf, SPD_CONFIG_AUTHORIZE );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_AUTHORIZE );
      nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
      pszSvr = MALLOC( nResult+1 );
	  MEMSET(pszSvr,0,nResult+1);
      MEMCPY(pszSvr,pszTok, nResult);
	  ud_Trim(pszSvr);
	  pMe->cAuthorize = *pszSvr;
	  FREE(pszSvr);	
   }
   else
	  pMe->cAuthorize = DEFAULT_AUTHORIZE;			//  ȱʡ 

   // ����ʱGps״̬
   pszTok = STRSTR( pszBuf, SPD_CONFIG_GPS_MODE );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_GPS_MODE );
      nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
      pszSvr = MALLOC( nResult+1 );
	  MEMSET(pszSvr,0,nResult+1);
      MEMCPY(pszSvr,pszTok, nResult);
	  ud_Trim(pszSvr);
	  pMe->cGpsMode = *pszSvr;
	  FREE(pszSvr);	
   }
   else
	  pMe->cGpsMode = DEFAULT_GpsMode;			//  ȱʡ 

   // ������ʱʹ�õ�ͼƬ
   pszTok = STRSTR( pszBuf, SPD_CONFIG_PIC_CHAR );
   if ( pszTok ) {
	   pszTok = pszTok + STRLEN( SPD_CONFIG_PIC_CHAR );
	   nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
	   pszSvr = MALLOC( nResult+1 );
	   MEMSET(pszSvr,0,nResult+1);
	   MEMCPY(pszSvr,pszTok, nResult);
	   ud_Trim(pszSvr);
	   pMe->cPicMode = *pszSvr;
	   FREE(pszSvr);	
   }
   else
	   pMe->cPicMode = DEFAULT_PIC;			//  

   // �Ƿ�ʹ�ð���װ��ģʽ
   pszTok = STRSTR( pszBuf, SPD_CONFIG_USE_LOAD );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_USE_LOAD );
      nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
      pszSvr = MALLOC( nResult+1 );
	  MEMSET(pszSvr,0,nResult+1);
      MEMCPY(pszSvr,pszTok, nResult);
	  ud_Trim(pszSvr);
	  pMe->cUseLoad = *pszSvr;
	  FREE(pszSvr);	
   }
   else
	  pMe->cUseLoad = DEFAULT_UseLoad;			//  ȱʡ 

   // �Ƿ�ʹ�÷�����Ƭ����
   pszTok = STRSTR( pszBuf, SPD_CONFIG_PHOTO );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_PHOTO );
      nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
      pszSvr = MALLOC( nResult+1 );
	  MEMSET(pszSvr,0,nResult+1);
      MEMCPY(pszSvr,pszTok, nResult);
	  ud_Trim(pszSvr);
	  pMe->cUsePhoto = *pszSvr;
	  FREE(pszSvr);	
   }
   else
	  pMe->cUsePhoto = DEFAULT_PHOTO;			//  ȱʡ 

   // �ֻ����մ�ŵ�·��
   pszTok = STRSTR( pszBuf, SPD_PHOTO_PATH );
   if ( pszTok ) {
	   pszTok = pszTok + STRLEN( SPD_PHOTO_PATH );
	   nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
	   MEMSET(pMe->sPhotoPath,0,sizeof(pMe->sPhotoPath));
	   if ( nResult < sizeof(pMe->sPhotoPath) )					// 2013-05-20
			MEMCPY(pMe->sPhotoPath,pszTok, nResult);
	   else
			STRCPY(pMe->sPhotoPath,DEFAULT_PATH);
   }
   else
	   STRCPY(pMe->sPhotoPath,DEFAULT_PATH);	

   //  Add by zjie 2012-09-21 �ֻ���Ƶ��·��
   pszTok = STRSTR( pszBuf, SPD_VIDEO_PATH );
   if ( pszTok ) {
	   pszTok = pszTok + STRLEN( SPD_VIDEO_PATH );
	   nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
	   MEMSET(pMe->sVideoPath,0,sizeof(pMe->sVideoPath));
	   if (nResult < sizeof(pMe->sVideoPath) )			// 2013-05-20
		  MEMCPY(pMe->sVideoPath,pszTok, nResult);
	   else
		  STRCPY(pMe->sVideoPath,DEFAULT_PATH);	
   }
   else
	   STRCPY(pMe->sVideoPath,DEFAULT_PATH);	
	// Add End

   //  Add by zjie 2014-04-15 �ֻ�¼����·��
   pszTok = STRSTR( pszBuf, SPD_AUDREC_PATH );
   if ( pszTok ) {
	   pszTok = pszTok + STRLEN( SPD_AUDREC_PATH );
	   nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
	   MEMSET(pMe->sAudRecPath,0,sizeof(pMe->sAudRecPath));
	   if (nResult < sizeof(pMe->sAudRecPath) )			// 2013-05-20
		   MEMCPY(pMe->sAudRecPath,pszTok, nResult);
	   else
		   STRCPY(pMe->sAudRecPath,DEFAULT_PATH);	
   }
   else
	   STRCPY(pMe->sAudRecPath,DEFAULT_PATH);	
   // Add End

   // ��Ƭ���
   pszTok = STRSTR( pszBuf, SPD_PHOTO_WIDTH );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_PHOTO_WIDTH );
      pMe->iPhotoWidth = STRTOUL( pszTok, &pszDelimiter, 10 );
   }
   else
	  pMe->iPhotoWidth = DEFAULT_PHOTO_WIDTH;	

   // ��Ƭ�߶�
   pszTok = STRSTR( pszBuf, SPD_PHOTO_HEIGHT );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_PHOTO_HEIGHT );
      pMe->iPhotoHeight = STRTOUL( pszTok, &pszDelimiter, 10 );
   }
   else
	  pMe->iPhotoHeight = DEFAULT_PHOTO_HEIGHT;	

   // �ֻ�����
   pszTok = STRSTR( pszBuf, SPD_PHOTO_TYPE );
   if ( pszTok ) {
	   pszTok = pszTok + STRLEN( SPD_PHOTO_TYPE );
	   nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
	   MEMSET(pMe->sMobileType,0,sizeof(pMe->sMobileType));
	   if (nResult < sizeof(pMe->sMobileType) )			// 2013-05-20
		   MEMCPY(pMe->sMobileType,pszTok, nResult);
	   else
		   STRCPY(pMe->sMobileType,DEFAULT_PHOTO_TYPE);	
   }
   else
	   STRCPY(pMe->sMobileType,DEFAULT_PHOTO_TYPE);	

   // ��ȡ���ͼ���ڱ��漸������
   pszTok = STRSTR( pszBuf, SPD_CONFIG_ARRAY_SIZE );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_CONFIG_ARRAY_SIZE );
      pMe->nArySize = STRTOUL( pszTok, &pszDelimiter, 10 );
	  if (pMe->nArySize >= DEFAULT_MAX_ARRAY_SIZE)
		  pMe->nArySize = DEFAULT_MAX_ARRAY_SIZE;
   }
   else
	  pMe->nArySize = DEFAULT_ARRAY_SIZE;	
	
   // ��λ����ʱ����GPS
   pszTok = STRSTR( pszBuf, SPD_CONFIG_RSG );
   if ( pszTok ) {
	   pszTok = pszTok + STRLEN( SPD_CONFIG_RSG );
	   pMe->mInt_ReStartGps = STRTOUL( pszTok, &pszDelimiter, 10 );
   }
   else
		pMe->mInt_ReStartGps = DEFAULT_ReStartGps;
   // ����GPS ʱʱ����
   pszTok = STRSTR( pszBuf, SPD_CONFIG_SOT );
   if ( pszTok ) {
	   pszTok = pszTok + STRLEN( SPD_CONFIG_SOT );
	   pMe->mInt_sot = STRTOUL( pszTok, &pszDelimiter, 10 );
	   if (pMe->mInt_sot<DEFAULT_sot)  pMe->mInt_sot = DEFAULT_sot;
   }
	else
		pMe->mInt_sot = DEFAULT_sot;

	pszTok = STRSTR( pszBuf, FONT_WIDTH );				// �ַ����
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( FONT_WIDTH );
		pMe->iFontWidth = STRTOUL( pszTok, &pszDelimiter, 10 );
	}
	else
		pMe->iFontWidth = DEFAULT_FONT_WIDTH;	
	
	pszTok = STRSTR( pszBuf, FONT_HEIGHT );				// �ַ��߶�
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( FONT_HEIGHT );
		pMe->iFontHeight = STRTOUL( pszTok, &pszDelimiter, 10 );
	}
	else
		pMe->iFontHeight = DEFAULT_FONT_HEIGHT;	
	
	pszTok = STRSTR( pszBuf, ADD_HOUR );				// ʱ������
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( ADD_HOUR );
		pMe->iAddHour = STRTOUL( pszTok, &pszDelimiter, 10 );
	}
	else
		pMe->iAddHour = DEFAULT_ADD_HOUR;	

	// �Ƿ�����������Ƭ
	pszTok = STRSTR( pszBuf, SPD_PHOTO_SEND );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_PHOTO_SEND );
		nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
		pszSvr = MALLOC( nResult+1 );
		MEMSET(pszSvr,0,nResult+1);
		MEMCPY(pszSvr,pszTok, nResult);
		ud_Trim(pszSvr);
		DBGPRINTF("zjie-ReadcPhotoSend %s",pszSvr);
		pMe->cPhotoSend = *pszSvr;
		FREE(pszSvr);	
	}
	else
		pMe->cPhotoSend = DEFAULT_PHOTO_SEND;			//  ȱʡ 

	// 2012-10-16 ������ʷԭ�򣬶��˴˲�����2.5.9�����Ӵ��жϣ���Ӧ����cPhotoSend��cUsePhoto�����Ժ�
	if (pMe->cPhotoSend == '0') {
		pMe->cPhotoSend =  uf_HasFun(pMe,USR_NMASK_TRANSMITPHOTO) ? '1' : '0';
	}
	// Add End

	// Add by zjie 2012-07-20	// ������λ����
	pszTok = STRSTR( pszBuf, SPD_CONFIG_POSDET_ATIMES );				
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_POSDET_ATIMES );
		pMe->iPosDetAtime = STRTOUL( pszTok, &pszDelimiter, 10 );
	}
	else
		pMe->iPosDetAtime =	DEFAULT_POSDET_ATIMES;	

	// �Ƿ���д��־
	pszTok = STRSTR( pszBuf, SPD_CONFIG_LOG );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_LOG );
		nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
		pszSvr = MALLOC( nResult+1 );
		MEMSET(pszSvr,0,nResult+1);
		MEMCPY(pszSvr,pszTok, nResult);
		ud_Trim(pszSvr);
		pMe->cWrtLog = *pszSvr;
		FREE(pszSvr);	
	}
	// Add End

	pszTok = STRSTR( pszBuf, SPD_CONFIG_CT_AUTH );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_CT_AUTH );
		nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
		pszSvr = MALLOC( nResult+1 );
		MEMSET(pszSvr,0,nResult+1);
		MEMCPY(pszSvr,pszTok, nResult);
		ud_Trim(pszSvr);
		pMe->cCTAuth = *pszSvr;
		FREE(pszSvr);	
	}
	else
		pMe->cCTAuth = DEFAULT_CT_AUTH;			//  ȱʡ 

	pszTok = STRSTR( pszBuf, SPD_CONFIG_CUSID );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_CUSID );
		nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
		MEMSET(pMe->sCusID,0,sizeof(pMe->sCusID));
		if (nResult < sizeof(pMe->sCusID))			// 2013-05-20
		   MEMCPY(pMe->sCusID,pszTok, nResult);
		else
			STRCPY(pMe->sCusID,DEFAULT_CT_CUSID);
	}
	else
		STRCPY(pMe->sCusID,DEFAULT_CT_CUSID);	

	pszTok = STRSTR( pszBuf, SPD_CONFIG_CUPWD );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_CUPWD );
		nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
		MEMSET(pMe->sPassword,0,sizeof(pMe->sPassword));
		if ( nResult < sizeof(pMe->sPassword) )				// 2013-05-20
		   MEMCPY(pMe->sPassword,pszTok, nResult);
		else
		   STRCPY(pMe->sPassword,DEFAULT_CT_CUPWD);
	}
	else
		STRCPY(pMe->sPassword,DEFAULT_CT_CUPWD);	

	// һ�����еĶԽ������� Add by zjie 2013-01-01
	pszTok = STRSTR( pszBuf, SPD_CONFIG_CALLNOS_STRING );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_CALLNOS_STRING );
		nResult = ud_DistToChar(pszTok,';');
		FREEIF(pMe->sPttCallNo);
		pMe->sPttCallNo = (char *)MALLOC((nResult+1)*sizeof(char));
		MEMSET(pMe->sPttCallNo,0,(nResult+1)*sizeof(char));
		MEMCPY(pMe->sPttCallNo,pszTok, nResult);
	}
	else
		FREEIF(pMe->sPttCallNo);

	// һ�����еİ���ֵ
	pszTok = STRSTR( pszBuf, SPD_CONFIG_PTT_KEY );				// �ַ��߶�
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_PTT_KEY );
		pMe->nPttkey = STRTOUL( pszTok, &pszDelimiter, 10 );
	}
	else
		pMe->nPttkey = DEFAULT_PTTKEY;	

	// �Ƿ�����1#�˳�
	pszTok = STRSTR( pszBuf, SPD_CONFIG_EXIT );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_EXIT );
		nResult = ud_DistToSplit( pszTok, DEFAULT_FIELD_SPLIT );
		pszSvr = MALLOC( nResult+1 );
		MEMCPY(pszSvr,pszTok, nResult);
		ud_Trim(pszSvr);
		pMe->cCanExit = *pszSvr;
		FREE(pszSvr);	
	}
	else
		pMe->cCanExit = DEFAULT_EXIT;			//  ȱʡ 
	// Add End;

	// Add 2013-04-27
	pszTok = STRSTR( pszBuf, SPD_CONFIG_CHECKON_KEEP );				
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_CHECKON_KEEP );
		pMe->nKeepDays = STRTOUL( pszTok, &pszDelimiter, 10 );
	}
	else
		pMe->nKeepDays = DEFAULT_KEEP_DAY;			//  ȱʡ 
	// Add End

	//  ���ò���TA   2013-08-19
	pszTok = STRSTR( pszBuf, SPD_CONF_TA );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONF_TA );
		pMe->iErrConTa = STRTOUL( pszTok, &pszDelimiter, 10 );
		if (pMe->iErrConTa < DEFAULT_TA)
			pMe->iErrConTa = DEFAULT_TA;
	}
	else
		pMe->iErrConTa = DEFAULT_TA;	

	pszTok = STRSTR( pszBuf, SPD_CONF_TB );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONF_TB );
		nResult = ud_DistToChar(pszTok,';'); 
		MEMSET(pMe->sRestTb,0,sizeof(pMe->sRestTb));
		if ( nResult < sizeof(pMe->sRestTb) )
		{
			MEMCPY(pMe->sRestTb,pszTok, nResult);
		}
		else
		{
			STRCPY(pMe->sRestTb,DEFAULT_TB);
		}
		pMe->iRestTb = ATOI(pMe->sRestTb);
	}
	else
	{
		STRCPY(pMe->sRestTb,DEFAULT_TB);
		pMe->iRestTb = 25;
	}
	//  Add End 2013-08-19

	// Add 2013-10-01  ��qchat���ֻ�����
	if (STRLEN(pMe->MyMobileNo) == 0) {
		pszTok = STRSTR( pszBuf, SPD_MOBILE_NO );
		if ( pszTok ) {
			pszTok = pszTok + STRLEN( SPD_MOBILE_NO );
			nResult = ud_DistToChar(pszTok,';');
			if (nResult < sizeof(pMe->MyMobileNo))	
			   MEMCPY(pMe->MyMobileNo,pszTok, nResult);
		}
	} 

	// Add End

	// Add 2017-08-30  imei�� ���������ļ�
	pszTok = STRSTR( pszBuf, SPD_MOBILE_IMEI );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_MOBILE_IMEI );
		nResult = ud_DistToChar(pszTok,';');
		MEMSET(pMe->szMobileID_Configer,0,sizeof(pMe->szMobileID_Configer));
		MEMCPY(pMe->szMobileID_Configer,pszTok, nResult);
	}
	// Add End

	// ����gps������������ [3/9/2016 Administrator]
	pszTok = STRSTR( pszBuf, SPD_CONFIG_OFFLINNEDATA_MAX_STRING );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_OFFLINNEDATA_MAX_STRING );
		pMe->m_WofflineData_Num = STRTOUL( pszTok, &pszDelimiter, 10 );
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"gimis_ReadGPSSettings 1 pMe->m_WofflineData_Num = %d",pMe->m_WofflineData_Num);
	}
	else{
		pMe->m_WofflineData_Num = DEFAULT_OFFLINNEDATA;
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"gimis_ReadGPSSettings 0 pMe->m_WofflineData_Num = %d",pMe->m_WofflineData_Num);
	}
	//end

	//������������1289���ٴκ󲥷�һ������ Add 2016-3-14
	pszTok = STRSTR( pszBuf, SPD_CONFIG_PLAY_ERROR_MP3_STRING );				
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_PLAY_ERROR_MP3_STRING );
		pMe->m_E_SC = STRTOUL( pszTok, &pszDelimiter, 10 );
	}
	else
		pMe->m_E_SC = DEFAULT_CONNECT_ERROR;			//  ȱʡ 
	// Add End

	   // �ҹ��ܼ��Ƿ���������
	pszTok = STRSTR( pszBuf, SPD_CONFIG_RIGHT_KEY_FLAG );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_RIGHT_KEY_FLAG );
		nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
		pszSvr = MALLOC( nResult+1 );
		MEMSET(pszSvr,0,nResult+1);
		MEMCPY(pszSvr,pszTok, nResult);
		ud_Trim(pszSvr);
		pMe->m_Right_Key = *pszSvr;
		FREE(pszSvr);	
	}
	else
		pMe->m_Right_Key = DEFAULT_Right_Key;			//  ȱʡ 
	//add End

	//�Ƿ�����Զ�¼���ϴ�����
	pszTok = STRSTR( pszBuf, SPD_CONFIG_ENABLE_AUDIO );				
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_ENABLE_AUDIO );
		pMe->m_enable_Audio = STRTOUL( pszTok, &pszDelimiter, 10 );
	}
	else
		pMe->m_enable_Audio = DEFAULT_Enable_Audio;			//  ȱʡ 
	// Add End

	//�Ƿ��ͼƬ����MD5У��
	   // ��������
	pszTok = STRSTR( pszBuf, SPD_CONFIG_ENABLE_MD5 );
	if ( pszTok ) {
		pszTok = pszTok + STRLEN( SPD_CONFIG_ENABLE_MD5 );
		pMe->m_enable_MD5 = STRTOUL( pszTok, &pszDelimiter, 10 );
	}
	else
	  pMe->m_enable_MD5 = DEFAULT_PULSATION;	
	//add End

   FREE( pszBuf );
   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_ReadGPSSettings out");

   return SUCCESS;
}



uint32 gimis_ReadPowerBoot( newgimis *pMe, IFile * pIFile )
{
   char    *pszBuf = NULL;
   char    *pszTok = NULL;
   char    *pszSvr = NULL;
   char    *pszDelimiter = ";";
   int32   nResult = 0;
   FileInfo fiInfo;

   if ( SUCCESS != IFILE_GetInfo( pIFile, &fiInfo ) ) {
      return EFAILED;
   }

   if ( fiInfo.dwSize == 0 ) {
      ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_Read Boot in,fiInfo.dwSize=0");
//	  ISHELL_SetTimer(pMe->a.m_pIShell, 200, (PFNNOTIFY)gimis_SetGPSPara, pMe);		// Add 2013-10-01
      return EFAILED;
   }

   // Allocate enough memory to read the full text into memory
   pszBuf = MALLOC( fiInfo.dwSize );

   nResult = IFILE_Read( pIFile, pszBuf, fiInfo.dwSize );
   if ( (uint32)nResult < fiInfo.dwSize ) {
      FREE( pszBuf );
      return EFAILED;
   }

   // �����Ƿ������� ��־Ϊ
   pszTok = STRSTR( pszBuf, SPD_BOOT_OPT_STRING );
   if ( pszTok ) {
      pszTok = pszTok + STRLEN( SPD_BOOT_OPT_STRING );
      nResult = ud_DistToChar(pszTok,';'); // uf_DistToSemi( pszTok );
      pszSvr = MALLOC( nResult+1 );
	  MEMSET(pszSvr,0,nResult+1);
      MEMCPY(pszSvr,pszTok, nResult);
	  ud_Trim(pszSvr);
	  pMe->m_PowerFlag = *pszSvr;
	  
	  FREE(pszSvr);	
	  pszSvr = NULL;
   }
   else
	  pMe->m_PowerFlag = '0';			//  ȱʡ 


   FREE( pszBuf );
   pszBuf = NULL;
   
   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_ReadBoot out");

   return SUCCESS;
}

//������newgimis��ʱ��д�����ļ����µ�����Ŀ
// uint32 gimis_WriteHTEL(newgimis *pMe)
// {
// 	IFileMgr   *pIFileMgr = NULL;
// 	IFile      *pIConfigFile = NULL;
// 	FileInfo* pFileInfo = NULL;	
// 	uint32      nResult = 0;
// 	char    *pszBuf;
// 	char    *pszStr;
// 	char    *pszFileBuf;
// 	char *szStart;
// 	char *szEnd;
// 	char lChr_Ind;
// 
// 	
// 	// Create the instance of IFileMgr
// 	nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
// 	if ( SUCCESS != nResult ) {
// 		return nResult;
// 	}
// 
// 	pFileInfo = (FileInfo*) MALLOC( sizeof( FileInfo ) );
// 	MEMSET(pFileInfo, 0x00, sizeof(FileInfo) );
// 	
// 	nResult = IFILEMGR_Test( pIFileMgr, SPD_CONFIG_FILE );
// 	if ( SUCCESS == nResult ) {
// 		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE  );
// 		if ( !pIConfigFile ) {
// 			nResult = EFAILED;
// 		}
// 		else {
// 			if ( IFILEMGR_GetInfo(pIFileMgr, SPD_CONFIG_FILE, pFileInfo) == SUCCESS )
// 			{
// 				pszStr = MALLOC(10240);
// 				MEMSET(pszStr,0,10240);
// 
// 				pszFileBuf = MALLOC(10240);
// 				MEMSET(pszFileBuf,0,10240);
// 
// 				IFILE_Seek(pIConfigFile,_SEEK_START,0);
// 				nResult = IFILE_Read(pIConfigFile,pszStr,pFileInfo->dwSize); //pszStrȫ���ı�
// 				if (nResult == pFileInfo->dwSize)
// 				{
// 					IFILE_Truncate(pIConfigFile, 0);
// 					//MOBILE_NO = 
// 					szStart = STRSTR(pszStr,SPD_MOBILE_NO);//szStart�ֶο�ʼ
// 					szEnd = STRSTR(szStart,";\r\n");
// 					lChr_Ind = szStart[12];
// 					if (lChr_Ind == ';')  
// 					{
// 						MEMCPY(pszFileBuf,pszStr,szStart-pszStr);
// 						//SPRINTF( pszBuf, SPD_MOBILE_NO"%s;\r\n", pMe->MyMobileNo );
// 						SPRINTF( pszBuf, SPD_MOBILE_NO"110;\r\n" );
// 						MEMCPY(pszFileBuf + STRLEN(szStart), pszBuf, STRLEN( pszBuf ) );
// 						MEMCPY(pszFileBuf + STRLEN(szStart) + STRLEN(pszBuf), szEnd + 3,STRLEN(szEnd+3) );
// 
// 						IFILE_Write( pIConfigFile , pszFileBuf, STRLEN( pszFileBuf ) );
// 					}
// 					
// 				}
// 
// 				FREE(pFileInfo);
// 				FREE(pszStr);
// 				FREE(pszFileBuf);
// 			}
// 		}
// 	}
// 	
// 	// Free the IFileMgr and IFile instances
// 	IFILE_Release( pIConfigFile );
// 	IFILEMGR_Release( pIFileMgr );
// 	
// 	return nResult;
// }


// =======================================================================
// ��������gimis�Ĳ���
// =======================================================================
uint32 gimis_SetGPSPara(newgimis *pMe)
{
	IFileMgr   *pIFileMgr = NULL;
	IFile      *pIConfigFile = NULL;
	uint32      nResult = 0;
	
	// Create the instance of IFileMgr
	nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
	if ( SUCCESS != nResult ) {
		return nResult;
	}
	
	// If the config file exists, open it and read the settings.  Otherwise, we need to
	// create a new config file.
	nResult = IFILEMGR_Test( pIFileMgr, SPD_CONFIG_FILE );
	if ( SUCCESS == nResult ) {
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE  );
		if ( !pIConfigFile ) {
			nResult = EFAILED;
		}
		else {
			nResult = gimis_WriteGPSSettings( pMe, pIConfigFile );
		}
	}
	
	// Free the IFileMgr and IFile instances
	IFILE_Release( pIConfigFile );
	IFILEMGR_Release( pIFileMgr );
	
	return nResult;
}



// ���� ����������Ȩ�ɹ� �����Ƿ�������
uint32 gimis_SetPram_Open_QQSQ_CG(newgimis *pMe)
{
   IFileMgr   *pIFileMgr = NULL;
   IFile      *pIConfigFile = NULL;
   uint32      nResult = 0;

   // Create the instance of IFileMgr
   nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
   if ( SUCCESS != nResult ) {
      return nResult;
   }

   nResult = IFILEMGR_Test( pIFileMgr, SPD_BOOT_FILE );
   if ( SUCCESS == nResult ) 
   {

   }
   else 
   {
      pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_BOOT_FILE, _OFM_CREATE );
      if ( !pIConfigFile ) 
         nResult = EFAILED;
      else 
         nResult = gimis_WritePowerBoot( pMe, pIConfigFile );
   }

   if (pIConfigFile)
   {
	   IFILE_Release( pIConfigFile );
   }
   if (pIFileMgr)
   {
	   IFILEMGR_Release( pIFileMgr );
   }
   return nResult;
}

//  ���ÿ����Ƿ�������������ļ�
uint32 gimis_SetPram_Open(newgimis *pMe)
{
	IFileMgr   *pIFileMgr = NULL;
	IFile      *pIConfigFile = NULL;
	uint32      nResult = 0;
	
	// Create the instance of IFileMgr
	nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
	if ( SUCCESS != nResult ) {
		return nResult;
	}
	
	// If the config file exists, open it and read the settings.  Otherwise, we need to
	// create a new config file.
	nResult = IFILEMGR_Test( pIFileMgr, SPD_BOOT_FILE );
	if ( SUCCESS == nResult ) {
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_BOOT_FILE, _OFM_READWRITE  );
		if ( !pIConfigFile ) {
			nResult = EFAILED;
		}
		else {
			nResult = gimis_WritePowerBoot( pMe, pIConfigFile );
		}
	}
	
	// Free the IFileMgr and IFile instances
	if (pIConfigFile)
	{
		IFILE_Release( pIConfigFile );
	}
	if (pIFileMgr)
	{
		IFILEMGR_Release( pIFileMgr );
	}
	
	
	return nResult;
}

// =======================================================================
// ��ȡ����gimis�Ĳ���
// =======================================================================
uint32 gimis_GetGPSPara(newgimis *pMe)
{
	IFileMgr   *pIFileMgr = NULL;
	IFile      *pIConfigFile = NULL;
	uint32      nResult = 0;
	
	// Create the instance of IFileMgr
	nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
	if ( SUCCESS != nResult ) {
		return nResult;
	}
	
	// If the config file exists, open it and read the settings.  Otherwise, we need to
	// create a new config file.
	nResult = IFILEMGR_Test( pIFileMgr, SPD_CONFIG_FILE );
	if ( SUCCESS == nResult ) {
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE  );
		if ( !pIConfigFile ) {
			nResult = EFAILED;
		}
		else {
			nResult = gimis_ReadGPSSettings( pMe, pIConfigFile );
		}
	}
	
	// Free the IFileMgr and IFile instances
	IFILE_Release( pIConfigFile );
	IFILEMGR_Release( pIFileMgr );
	
	return nResult;
}



// =======================================================================
// �ж���û����������
// =======================================================================
uint32 gimis_judgedata(newgimis *pMe)
{
	IFileMgr   *pIFileMgr = NULL;
	IFile      *pIConfigFile = NULL;
	uint32      nResult = 0;
    FileInfo fiInfo;
	
	// Create the instance of IFileMgr
	nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
	if ( SUCCESS != nResult ) {
//		uf_RecLog(pMe,"�ж����߿������� �����ļ����ʧ��" );
		uf_RecLog(pMe,"judge offline data create file handle fail" );
		return nResult;
	}
	
	// If the config file exists, open it and read the settings.  Otherwise, we need to
	// create a new config file.
	nResult = IFILEMGR_Test( pIFileMgr, SPD_OFFLINE_1 ); //����ָ���ļ���Ŀ¼�Ƿ����
	
	if ( SUCCESS == nResult ) 
	{
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE  );
		if ( pIConfigFile ) 
		{
			if ( SUCCESS == IFILE_GetInfo( pIConfigFile, &fiInfo ) ) // ��ȡ IFile �ӿڶ�����ָ�ļ��Ĵ������ڴ�С���ƺ�����
			{
				if ( fiInfo.dwSize > 0 ) 
				{
					pMe->m_OffData = 1;
					IFILE_Release( pIConfigFile );
					IFILEMGR_Release( pIFileMgr );
					return 1;
				}
			}

		}
	}
	

	nResult = IFILEMGR_Test( pIFileMgr, SPD_OFFLINE_2 );
	if ( SUCCESS == nResult ) 
	{
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE  );
		if ( pIConfigFile ) 
		{
			if ( SUCCESS == IFILE_GetInfo( pIConfigFile, &fiInfo ) ) 
			{
				if ( fiInfo.dwSize > 0 ) 
				{
					pMe->m_OffData = 1;
					IFILE_Release( pIConfigFile );
					IFILEMGR_Release( pIFileMgr );
					return 2;
				}
			}

		}
	}



	nResult = IFILEMGR_Test( pIFileMgr, SPD_OFFLINE_3 );
	if ( SUCCESS == nResult ) 
	{
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE  );
		if ( pIConfigFile ) 
		{
			if ( SUCCESS == IFILE_GetInfo( pIConfigFile, &fiInfo ) ) 
			{
				if ( fiInfo.dwSize > 0 ) 
				{
					pMe->m_OffData = 1;
					IFILE_Release( pIConfigFile );
					IFILEMGR_Release( pIFileMgr );
					return 3;
				}
			}

		}
	}



	nResult = IFILEMGR_Test( pIFileMgr, SPD_OFFLINE_4 );
	if ( SUCCESS == nResult ) 
	{
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE  );
		if ( pIConfigFile ) 
		{
			if ( SUCCESS == IFILE_GetInfo( pIConfigFile, &fiInfo ) ) 
			{
				if ( fiInfo.dwSize > 0 ) 
				{
					pMe->m_OffData = 1;
					IFILE_Release( pIConfigFile );
					IFILEMGR_Release( pIFileMgr );
					return 4;
				}
			}

		}
	}



	nResult = IFILEMGR_Test( pIFileMgr, SPD_OFFLINE_5 );
	if ( SUCCESS == nResult ) 
	{
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE  );
		if ( pIConfigFile ) 
		{
			if ( SUCCESS == IFILE_GetInfo( pIConfigFile, &fiInfo ) ) 
			{
				if ( fiInfo.dwSize > 0 ) 
				{
					pMe->m_OffData = 1;
					IFILE_Release( pIConfigFile );
					IFILEMGR_Release( pIFileMgr );
					return 5;
				}
			}

		}
	}



	nResult = IFILEMGR_Test( pIFileMgr, SPD_OFFLINE_6 );
	if ( SUCCESS == nResult ) 
	{
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE  );
		if ( pIConfigFile ) 
		{
			if ( SUCCESS == IFILE_GetInfo( pIConfigFile, &fiInfo ) ) 
			{
				if ( fiInfo.dwSize > 0 ) 
				{
					pMe->m_OffData = 1;
					IFILE_Release( pIConfigFile );
					IFILEMGR_Release( pIFileMgr );
					return 6;
				}
			}

		}
	}



	nResult = IFILEMGR_Test( pIFileMgr, SPD_OFFLINE_7 );
	if ( SUCCESS == nResult ) 
	{
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE  );
		if ( pIConfigFile ) 
		{
			if ( SUCCESS == IFILE_GetInfo( pIConfigFile, &fiInfo ) ) 
			{
				if ( fiInfo.dwSize > 0 ) 
				{
					pMe->m_OffData = 1;
					IFILE_Release( pIConfigFile );
					IFILEMGR_Release( pIFileMgr );
					return 7;
				}
			}

		}
	}



	nResult = IFILEMGR_Test( pIFileMgr, SPD_OFFLINE_8 );
	if ( SUCCESS == nResult ) 
	{
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE  );
		if ( pIConfigFile ) 
		{
			if ( SUCCESS == IFILE_GetInfo( pIConfigFile, &fiInfo ) ) 
			{
				if ( fiInfo.dwSize > 0 ) 
				{
					pMe->m_OffData = 1;
					IFILE_Release( pIConfigFile );
					IFILEMGR_Release( pIFileMgr );
					return 8;
				}
			}
		}
	}




	nResult = IFILEMGR_Test( pIFileMgr, SPD_OFFLINE_9 );
	if ( SUCCESS == nResult ) 
	{
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READWRITE  );
		if ( pIConfigFile ) 
		{
			if ( SUCCESS == IFILE_GetInfo( pIConfigFile, &fiInfo ) ) 
			{
				if ( fiInfo.dwSize > 0 ) 
				{
					pMe->m_OffData = 1;
					IFILE_Release( pIConfigFile );
					IFILEMGR_Release( pIFileMgr );
					return 9;
				}
			}

		}
	}
	pMe->m_OffData = 0; // ˵��û�����߿�������
	// Free the IFileMgr and IFile instances

	IFILEMGR_Release( pIFileMgr );
	
	return 0; // EFAILED
}


/*===========================================================================
�ж��Ƿ�������Ƭ(�����ļ�)�ȹ���
===========================================================================*/
boolean uf_HasFun(newgimis *pMe, int nMask)
{
	boolean bRtn = FALSE;
	char sLogBuf[128];
	
	// ���÷����ļ�
	if ( (pMe->cUsePhoto & nMask) == nMask ) {
		SPRINTF(sLogBuf,"uf_HasFun in nMask=%d,ok",nMask);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),sLogBuf);
		bRtn = TRUE;
	}
	else {
		SPRINTF(sLogBuf,"uf_HasFun in nMask=%d,failure",nMask);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),sLogBuf);
	}
	
	return bRtn;
	
}



// ============================================================
// ��������Ƿ����
// ����������true ���ã� false ������  02 2012-02-11 ����
//===============================================================
boolean uf_CheckNetSts(newgimis * pMe)
{
	boolean bNetSts = TRUE;
	
    if ( ( gimis_HasStatus(pMe,USR_NMASK_SS_TAPI) == 0 ) || 
		( gimis_HasStatus(pMe,USR_NMASK_WAIT_IDLE) == USR_NMASK_WAIT_IDLE ) )
		bNetSts = FALSE;
	
	if (!bNetSts)
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "uf_CheckNetSts,cPhoneSts=%d,1=%d,32=%d",
		pMe->cPhoneSts,gimis_HasStatus(pMe,USR_NMASK_SS_TAPI),gimis_HasStatus(pMe,USR_NMASK_WAIT_IDLE));
	
	return bNetSts;
}


void uf_GetNetSts_imsi(newgimis * pMe)				// ��ȡimsi��
{
	int nErr = SUCCESS;
	ITAPI * pITAPI = NULL;
	TAPIStatus PhoneStatus;
	
		
	
	// ����״̬
	if ( (nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_TAPI, (void **) (&(pITAPI)))) != SUCCESS)
    {
        DBGPRINTF("NetSts 0: ERR%d!", nErr);
		return;
    }
	
	if ( (nErr=ITAPI_GetStatus(pITAPI, &PhoneStatus)) != SUCCESS )
	{
        DBGPRINTF("-NetSts 1: ERR%d!", nErr);
		return;
	}
	// 
	if (STRLEN(pMe->szMobileID)==0) 
		STRCPY(pMe->szMobileID,PhoneStatus.szMobileID);

	return;
}

// ============================================================
// ��ȡ����״̬   02 2012-02-11 ����
// ����������true ���ã� false ������
//===============================================================
void uf_GetNetSts(newgimis * pMe)
{
	int nErr = SUCCESS;
	ITAPI * pITAPI = NULL;
	TAPIStatus PhoneStatus;
	
		
	
	// ����״̬
	if ( (nErr = ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_TAPI, (void **) (&(pITAPI)))) != SUCCESS)
    {
        DBGPRINTF("NetSts 0: ERR%d!", nErr);
		goto ExitHere;
    }
	
	if ( (nErr=ITAPI_GetStatus(pITAPI, &PhoneStatus)) != SUCCESS )
	{
        DBGPRINTF("-NetSts 1: ERR%d!", nErr);
		goto ExitHere;
	}
	// 
	if (STRLEN(pMe->szMobileID)==0) 
		STRCPY(pMe->szMobileID,PhoneStatus.szMobileID);
//	uf_RecLog(pMe,"��ȡ����");
//	uf_RecLog(pMe,"get phone number");
//	uf_RecLog(pMe,PhoneStatus.szMobileID);
	// Add End
	
	if (  (gimis_HasStatus(pMe,USR_NMASK_SS_TAPI) == 0) && (PhoneStatus.state != 0) && (pMe->nSavInd > 0) ) {		// pMe->cPhoneSts == 0
		DBGPRINTF( "zjie-NetSts 2");
		gimis_SetStatus(pMe,USR_NMASK_WAIT_IDLE);				   //pMe->cPhoneSts = '0';
		ISHELL_SetTimer(pMe->a.m_pIShell, 13*1000, (PFNNOTIFY)uf_SetNetSts, (uint32*)pMe);
		ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_CHECKIN_EVENT,0, 0);
	}
	else if(gimis_HasStatus(pMe,USR_NMASK_WAIT_IDLE) != USR_NMASK_WAIT_IDLE)								// pMe->cPhoneSts != '0'
		gimis_SetStatus(pMe,PhoneStatus.state);	// pMe->cPhoneSts = PhoneStatus.state;
	
ExitHere:
	//   ���Σ�һ��ʱ���������
	//   	if (pITAPI != NULL) {			
	//  		DBGPRINTF( "zjie-NetSts 3");
	//  		ITAPI_Release(pITAPI);
	//  	}
	
	return;
	
}

/* ===========================================================================================
˵��
// �����������״̬תΪ����״̬ʱ����������״̬
============================================================================================== */
void uf_SetNetSts(newgimis *pMe)
{
	
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "uf_SetNetSts in, cPhoneSts=%d",pMe->cPhoneSts);
	if ( gimis_HasStatus(pMe,USR_NMASK_WAIT_IDLE) == USR_NMASK_WAIT_IDLE ) {		//	if (pMe->cPhoneSts == '0')		pMe->cPhoneSts = 1;
		char cPhoneSts = pMe->cPhoneSts;
		pMe->cPhoneSts = cPhoneSts ^ USR_NMASK_WAIT_IDLE;
		gimis_SetStatus(pMe,1);
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "uf_SetNetSts out,cPhoneSts=%d",pMe->cPhoneSts);
	}
	
}


/********************************************************************************************
// �ڱ��ؼ�¼����
�������
cgimis * pMe
char *vpStr_msg �� ���¼���ַ���
********************************************************************************************/
void uf_RecLog(newgimis * pMe,char *vpStr_msg)
{
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),vpStr_msg);		
}



// ���� ��GPS�йص�  ////////////
/*===========================================================================
===========================================================================*/
void *testgps_GetScreenData( newgimis *pMe )
{
	return pMe->pScreenData;
}





void *testgps_GetScreenData_gps( newgimis *pMe )
{
	return pMe->pScreenData_gps;
}

void *testgps_GetScreenData_Audio( newgimis *pMe )
{
	return pMe->pScreenData_Audio;
}

void *testgps_GetScreenData_LongSMS( newgimis *pMe )
{
	return pMe->pScreenData_LongSMS;
}

void *testgps_GetScreenData_LongSMS_Notity( newgimis *pMe )
{
	return pMe->pScreenData_LongSMS_Notity;
}







/*===========================================================================
===========================================================================*/
void testgps_DrawScreen( newgimis *pMe, uint32 dwParam )
{
	newgimis_HandleEvent( pMe, EVT_SCREEN, SCREEN_PARAM_PAINT, dwParam );
}


/*===========================================================================
HELPER ROUTINES FOR DRAWING.
===========================================================================*/
#define LINEHEIGHT   16   // hack
#define TOPLINE      0
#define ARRAYSIZE(a)   (sizeof(a) / sizeof((a)[0]))
#define min(a,b)  ((a) < (b) ? (a) : (b))


/*===========================================================================
===========================================================================*/
static void xDrawTextA(IDisplay * pd,AEEFont fnt, const char * pszText, int nChars,
                       int x,int y,const AEERect * prcBackground,uint32 dwFlags)
{
	AECHAR wcBuf[80];
	
	if (nChars < 0)
		nChars = STRLEN(pszText);
	nChars = min(nChars, ARRAYSIZE(wcBuf));
	
	STR_TO_WSTR((char*)pszText, wcBuf, sizeof(wcBuf));
	
	IDISPLAY_DrawText(pd, fnt, wcBuf, nChars, x, y, prcBackground, dwFlags);
}

/*===========================================================================
===========================================================================*/
static void xDrawTextA_hanzi(IDisplay * pd,AEEFont fnt, AECHAR * pszText, int nChars,
                       int x,int y,const AEERect * prcBackground,uint32 dwFlags)
{
	IDISPLAY_DrawText(pd, fnt, pszText, -1, x, y, prcBackground, dwFlags);
}


/*===========================================================================
===========================================================================*/
static void xDisplay( AEEApplet *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, const char *psz)
{
	AEEDeviceInfo di;
	AEERect rc;
	int nMaxLines;
	newgimis *ppMe =(newgimis *) pMe;
	
	ISHELL_GetDeviceInfo(pMe->m_pIShell,&di);
	nMaxLines = (di.cyScreen / ppMe->m_SYSFBH) - 2;
	if (nMaxLines < 1)
		nMaxLines = 1;
	
	rc.x = nCol;
	rc.dx = di.cxScreen - nCol;
	
	rc.y = nLine * ppMe->m_SYSFBH;
	if( dwFlags & IDF_ALIGNVERT_MASK ) {
		rc.dy = di.cyScreen - rc.y;
	}
	else {
		rc.dy = ppMe->m_SYSFBH;
	}
	
	xDrawTextA(pMe->m_pIDisplay,
		fnt,
		psz, -1, rc.x, rc.y, &rc, dwFlags);
	
	IDISPLAY_Update(pMe->m_pIDisplay);
	ppMe = NULL;
}


/*===========================================================================
===========================================================================*/
static void xDisplay_hanzi( AEEApplet *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *psz)
{
	AEEDeviceInfo di;
	AEERect rc;
	int nMaxLines;

	newgimis *ppMe =(newgimis *) pMe;
	
	ISHELL_GetDeviceInfo(pMe->m_pIShell,&di);
	nMaxLines = (di.cyScreen /  ppMe->m_SYSFBH) - 2;
	if (nMaxLines < 1)
		nMaxLines = 1;
	
	rc.x = nCol;
	rc.dx = di.cxScreen - nCol;
	
	rc.y = nLine *  ppMe->m_SYSFBH;
	if( dwFlags & IDF_ALIGNVERT_MASK ) {
		rc.dy = di.cyScreen - rc.y;
	}
	else {
		rc.dy =  ppMe->m_SYSFBH;
	}
	
	xDrawTextA_hanzi(pMe->m_pIDisplay,
		fnt,
		psz, -1, rc.x, rc.y, &rc, dwFlags);
	
	IDISPLAY_Update(pMe->m_pIDisplay);
	ppMe = NULL;
}


/*===========================================================================
===========================================================================*/
void *testgps_SetScreenData( newgimis *pMe, void *screenData )
{
	void *oldScreenData = pMe->pScreenData;
	pMe->pScreenData    = screenData;
	return oldScreenData;
}


/*===========================================================================
===========================================================================*/


void *testgps_SetScreenData_gps( newgimis *pMe, void *screenData )
{
	void *oldScreenData = pMe->pScreenData_gps;
	pMe->pScreenData_gps    = screenData;
	return oldScreenData;
}


void *testgps_SetScreenData_Audio( newgimis *pMe, void *screenData )
{
	void *oldScreenData = pMe->pScreenData_Audio;
	pMe->pScreenData_Audio    = screenData;
	return oldScreenData;
}

void *testgps_SetScreenData_LongSMS( newgimis *pMe, void *screenData )
{
	void *oldScreenData = pMe->pScreenData_LongSMS;
	pMe->pScreenData_LongSMS    = screenData;
	return oldScreenData;
}

void *testgps_SetScreenData_LongSMS_Notity( newgimis *pMe, void *screenData )
{
	void *oldScreenData = pMe->pScreenData_LongSMS_Notity;
	pMe->pScreenData_LongSMS_Notity    = screenData;
	return oldScreenData;
}

/*===========================================================================
===========================================================================*/
boolean testgps_GotoScreen( newgimis *pMe, ScreenId id, uint32 dwParam )
{
	GotoScreenParam *pParam = (GotoScreenParam *)MALLOC( sizeof(GotoScreenParam) );
	if( pParam ) {
		pParam->id = id;
		pParam->pfn = pMe->appScreens[id].theHandler;
		pParam->dwParam = dwParam;
		ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN,
			SCREEN_PARAM_GOTO, (uint32)pParam );
		return TRUE;
	}
	return FALSE;
}


/*===========================================================================
===========================================================================*/
boolean testgps_GotoScreentogps( newgimis *pMe, ScreenId id, uint32 dwParam )
{
	GotoScreenParam *pParam = (GotoScreenParam *)MALLOC( sizeof(GotoScreenParam) );
	if( pParam ) {
		pParam->id = id;
		pParam->pfn = pMe->appScreens[id].theHandler;
		pParam->dwParam = dwParam;
		ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN,
			SCREEN_PARAM_PAINT, (uint32)pParam );
		return TRUE;
	}
	return FALSE;
}



/*===========================================================================
===========================================================================*/
void testgps_Printf( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, 
					const char *szFormat, ... )
{
	char              szBuf[64];
	va_list args;
	va_start( args, szFormat );
	
	(void)VSNPRINTF( szBuf, 64, szFormat, args );
	
	va_end( args );
	xDisplay( (AEEApplet *)pMe, nLine, nCol, fnt, dwFlags, szBuf );
}

/*===========================================================================
===========================================================================*/
void testgps_Printf_hanzi( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, 
					AECHAR *szFormat, ... )
{
	xDisplay_hanzi( (AEEApplet *)pMe, nLine, nCol, fnt, dwFlags, szFormat );
}

static boolean testgps_ScreenHandler( newgimis *pMe, AEEEvent eCode,
									 uint16 wParam, uint32 dwParam )
{
	if( wParam == SCREEN_PARAM_GOTO ) {
		if( pMe->currentHandler ) {
			pMe->currentHandler( pMe, EVT_SCREEN, SCREEN_PARAM_CLOSE, 0 );
		}
		if( dwParam ) {
			
			GotoScreenParam *pParam = (GotoScreenParam *)dwParam;
			pMe->currentHandler = (AEEHANDLER)pParam->pfn;
			if( pMe->currentHandler( pMe, EVT_SCREEN, SCREEN_PARAM_INIT, pParam->dwParam ) == FALSE ) {
				pMe->currentHandler = 0;
			}
			
			FREE( pParam );
			return TRUE;
		}
	}
	
	else {
		if( pMe->currentHandler ) {
			return pMe->currentHandler( pMe, eCode, wParam, dwParam );
		}
	}
	
	return FALSE;
}

/*===========================================================================
===========================================================================*/
static boolean testgps_InitApplet( newgimis *pMe )
{
	boolean bRet = TRUE;
	pMe->currentHandler = 0;
	
	pMe->GPSFlag = FALSE;

	// Load the GPS settings from the config file, if possible
	testgps_InitGPSSettings( pMe );
	
	return bRet;
}




/*======================================================================= 
Function: testgps_InitGPSSettings()

  Description: 
  Initializes the GPS configuration to the default values.
  
	Prototype:
	
	  uint32 testgps_InitGPSSettings( testgps *pMe );
	  
		Parameters:
		pMe: [in]. CSamplePosDet instance.
		
		  Return Value:
		  
			SUCCESS - If the settings were initialized successfully.
			Returns error code otherwise.
			
			  Comments:  
			  None
			  
				Side Effects: 
				None
				
				  See Also:
				  None
=======================================================================*/
uint32 testgps_InitGPSSettings(newgimis *pMe)
{
	IFileMgr   *pIFileMgr = NULL;
	IFile      *pIConfigFile = NULL;
	uint32      nResult = 0;
	
	// Create the instance of IFileMgr
	nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
	if ( SUCCESS != nResult ) {
		return nResult;
	}
	
	// If the config file exists, open it and read the settings.  Otherwise, we need to
	// create a new config file.
	nResult = IFILEMGR_Test( pIFileMgr, SPD_CONFIG_FILE );
	if ( SUCCESS == nResult ) {
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_READ );
		if ( !pIConfigFile ) {
			nResult = EFAILED;
		}
		else {
			nResult = testgps_ReadGPSSettings( pMe, pIConfigFile );
		}
	}
	else {
		pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, SPD_CONFIG_FILE, _OFM_CREATE );
		if ( !pIConfigFile ) {
			nResult = EFAILED;
		}
		else {
			// Setup the default GPS settings
			pMe->gpsSettings.optim  = AEEGPS_OPT_DEFAULT;
			pMe->gpsSettings.qos    = SPD_QOS_DEFAULT;
			pMe->gpsSettings.server.svrType = AEEGPS_SERVER_DEFAULT;

			nResult = testgps_WriteGPSSettings( pMe, pIConfigFile );  // ��ʱ����  
		}
	}
	
	// Free the IFileMgr and IFile instances
	IFILE_Release( pIConfigFile );
	IFILEMGR_Release( pIFileMgr );
	
	return nResult;
}


/*===========================================================================
===========================================================================*/
static boolean testgps_StartApplet( newgimis *pMe )
{
	pMe->appScreens[SCREENID_MAINMENU].theHandler     = (AEEHANDLER)testgps_MainMenu_HandleEvent;
	pMe->appScreens[SCREENID_SHOWGETGPSINFO].theHandler   = (AEEHANDLER)testgps_ShowGetGPSInfo_HandleEvent;
	//  pMe->appScreens[SCREENID_CONFIG].theHandler       = (AEEHANDLER)SamplePosDet_ConfigMenu_HandleEvent;
	//  pMe->appScreens[SCREENID_PDE_SETTINGS].theHandler = (AEEHANDLER)SamplePosDet_PDESettings_HandleEvent;
	//   pMe->appScreens[SCREENID_PORT_SETTINGS].theHandler = (AEEHANDLER)SamplePosDet_PortSettings_HandleEvent;
	pMe->appScreens[SCREENID_SENDPHOTO].theHandler = (AEEHANDLER)SamplePosDet_SendPhoto_HandleEvent;
	pMe->appScreens[SCREENID_SENDAUDIO].theHandler = (AEEHANDLER)SamplePosDet_SendAudio_HandleEvent;

	pMe->appScreens[SCREENID_MORE].theHandler = (AEEHANDLER)SamplePosDet_More_HandleEvent;

	// ���һ�������б�Ľ���
	pMe->appScreens[SCREENID_TASKLIST].theHandler = (AEEHANDLER)SamplePosDet_Tasklist_HandleEvent;

	pMe->appScreens[SCREENID_DETAIL1].theHandler = (AEEHANDLER)SamplePosDet_Detail1_HandleEvent; // ����
	pMe->appScreens[SCREENID_DETAIL2].theHandler = (AEEHANDLER)SamplePosDet_Detail2_HandleEvent; // ������

	//������������������
	pMe->appScreens[SCREENID_CONTENT].theHandler = (AEEHANDLER)SamplePosDet_Content_HandleEvent;

	//#��̨ģʽ��������# ��������
	pMe->appScreens[SCREENID_NOTIFY].theHandler = (AEEHANDLER)SamplePosDet_TaskNotify_HandleEvent;

	//�ռ������
	pMe->appScreens[SCREENID_LONGSMS].theHandler = (AEEHANDLER)SamplePosDet_LongSMS_HandleEvent;

	//��Ϣ����
	pMe->appScreens[SCREENID_LONGSMS_CONTENT].theHandler = (AEEHANDLER)SamplePosDet_LongSMS_Content_HandleEvent;

	//��̨��ʾ
	pMe->appScreens[SCREENID_LONGSMS_Notify].theHandler = (AEEHANDLER)SamplePosDet_LongSMSNotify_HandleEvent;


//	pMe->appScreens[SCREENID_GETGPSINFO].theHandler = (AEEHANDLER)testgps_GetGPSInfo_HandleEvent;
	/* Open the main menu and send a paint. */
	//	testgps_GotoScreen_gps( pMe, SCREENID_GETGPSINFO, 0 );

	if (pMe->cGpsMode == '1') // ˵������GPS
	{
		newgimis_OpenGPS(pMe);  // ֻ����GPS 
	}
	pMe->m_WitchPag = 10;
	testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );


	
	return TRUE;
}


static boolean testgps_StartApplet_fu( newgimis *pMe )  //�����testgps_StartApplet�ĸ���  ��Ҫ�ǳ�ʼ��Щֵ����Ȼ��ֱ̨��������ʱ��������
{
	pMe->appScreens[SCREENID_MAINMENU].theHandler     = (AEEHANDLER)testgps_MainMenu_HandleEvent;
	pMe->appScreens[SCREENID_SHOWGETGPSINFO].theHandler   = (AEEHANDLER)testgps_ShowGetGPSInfo_HandleEvent;
	//  pMe->appScreens[SCREENID_CONFIG].theHandler       = (AEEHANDLER)SamplePosDet_ConfigMenu_HandleEvent;
	//  pMe->appScreens[SCREENID_PDE_SETTINGS].theHandler = (AEEHANDLER)SamplePosDet_PDESettings_HandleEvent;
	//   pMe->appScreens[SCREENID_PORT_SETTINGS].theHandler = (AEEHANDLER)SamplePosDet_PortSettings_HandleEvent;
	pMe->appScreens[SCREENID_SENDPHOTO].theHandler = (AEEHANDLER)SamplePosDet_SendPhoto_HandleEvent;
	pMe->appScreens[SCREENID_SENDAUDIO].theHandler = (AEEHANDLER)SamplePosDet_SendAudio_HandleEvent;

	pMe->appScreens[SCREENID_MORE].theHandler = (AEEHANDLER)SamplePosDet_More_HandleEvent;

	pMe->appScreens[SCREENID_TASKLIST].theHandler = (AEEHANDLER)SamplePosDet_Tasklist_HandleEvent;

	pMe->appScreens[SCREENID_DETAIL1].theHandler = (AEEHANDLER)SamplePosDet_Detail1_HandleEvent; // ����
	pMe->appScreens[SCREENID_DETAIL2].theHandler = (AEEHANDLER)SamplePosDet_Detail2_HandleEvent; // ������

	pMe->appScreens[SCREENID_CONTENT].theHandler = (AEEHANDLER)SamplePosDet_Content_HandleEvent;

	//pMe->appScreens[SCREENID_SENDAUDIO].theHandler = (AEEHANDLER)SamplePosDet_SendAudio_HandleEvent;	//�ռ������
	//pMe->appScreens[SCREENID_CONTENT].theHandler = (AEEHANDLER)SamplePosDet_Content_HandleEvent;      //������Ž���

	//#��̨ģʽ��������# ��������
	pMe->appScreens[SCREENID_NOTIFY].theHandler = (AEEHANDLER)SamplePosDet_TaskNotify_HandleEvent;

	//�ռ������
	pMe->appScreens[SCREENID_LONGSMS].theHandler = (AEEHANDLER)SamplePosDet_LongSMS_HandleEvent;
	
	//��Ϣ����
	pMe->appScreens[SCREENID_LONGSMS_CONTENT].theHandler = (AEEHANDLER)SamplePosDet_LongSMS_Content_HandleEvent;

	//��̨��ʾ
	pMe->appScreens[SCREENID_LONGSMS_Notify].theHandler = (AEEHANDLER)SamplePosDet_LongSMSNotify_HandleEvent;
	
	return TRUE;
}


static boolean testgps_CloseApplet_WSQ( newgimis *pMe )
{

	// \u672a\u6388\u6743\uff0c
	AECHAR szText[] = {0x672a,0x6388,0x6743,0xff0c,0x7a0b,0x5e8f,0x6b63,0x5728,0x9000,0x51fa,0x0000}; // L"δ��Ȩ�����������˳�";

	if (pMe->m_FOBFlag == 1)
	{
		IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
		IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
			3, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
		IDISPLAY_Update(pMe->a.m_pIDisplay);
	}

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)newgimis_ufStopApp, pMe);	
	return TRUE;
}

static boolean testgps_CloseApplet_SQYC( newgimis *pMe )
{


	AECHAR szText[] = {0x7a0b,0x5e8f,0x6b63,0x5728,0x9000,0x51fa,0x0000}; // L"���������˳�";
   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
						3, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)newgimis_ufStopApp, pMe);	
	return TRUE;
}

static boolean testgps_StartApplet_KJZQ( newgimis *pMe )  // �����Զ�����
{
	int iRet = -1;
//	ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY) testgps_StartApplet_KJZQ, (uint32*) pMe);

	pMe->m_PowerFlag = '1';//���������power�ļ���ʧ���޷���������

	if (pMe->m_PowerFlag != '1')
	{
		uf_RecLog(pMe,"Get PowerBoot flag in");
		gimis_Init_Pram_Open( pMe ); //��ȡ����txt�ı�־λ �������Ҫ��Ҫ�Զ�����
		uf_RecLog(pMe,"Get PowerBoot flag out");
	}

//	gimis_Init_Pram_Open( pMe ); //��ȡ����txt�ı�־λ �������Ҫ��Ҫ�Զ�����
	if (pMe->m_PowerFlag == '0')
	{
		// �رճ���
		pMe->m_Flag_Qqsq = 0; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����
		pMe->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨				
		iRet = ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"���ӵ��� �ر� 1");
		if (iRet == -1)
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"operation failure  1");
		}
		else
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"operation successful  1");
		}
		return TRUE;
	}
	if (pMe->m_F_R_O == 0) // ˵�����ǿ���������  ֻ�ǳ����������ӵ��� �����ǵ�һ�ο���
	{
		// �رճ���
		/*
		pMe->m_Flag_Qqsq = 0; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����
		pMe->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨				
		iRet = ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"���ӵ��� �ر� 2");

		if (iRet == -1)
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"operation failure  2");
		}
		else
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"operation successful  2");
		}
		*/
		pMe->m_KJZQ_C++ ;// ͳ�ƿ�����������Ĵ���
//		if (pMe->m_KJZQ_C++ < 3)
//		{
		pMe->m_F_CD = 1; //����Ƿ�Ϊ��� == 1 ��  == 0 ����
		uf_RecLog(pMe,"testgps_StartApplet_KJZQ again");
		ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) testgps_StartApplet_KJZQ, (uint32*) pMe);
/*		}
		else
		{
			pMe->m_Flag_Qqsq = 0; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����
			pMe->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨				
			iRet = ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
	//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"���ӵ��� �ر� 2");

			if (iRet == -1)
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"operation failure  2");
			}
			else
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"operation successful  2");
			}
		}
*/		
		return TRUE;
	}

	if (pMe->m_KJZQ_C > 0)
	{
//		ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
		uf_RecLog(pMe,"really start");
		pMe->m_FOBFlag = 0;
		pMe->m_WitchPag = 0;
		pMe->m_F_QF = FALSE;
		ISHELL_StartBackgroundApplet(pMe->a.m_pIShell, pMe->a.clsID,NULL);
		pMe->m_KJZQ_C = 0;
		pMe->m_temp_FOBFlag = 0;
		return TRUE;
	}
	
//	uf_GetNetSts(pMe);					
//	if ( !uf_CheckNetSts(pMe) )		// ����״̬�����粻���ã��������Ƕ�����λ
	if ( 0 )		// ����״̬�����粻���ã��������Ƕ�����λ
	{
		pMe->m_PosMode = TRUE;// ��������Ƿ�ֻ�д�����ģʽ  TRUE == ��ʾֻ�д�����ģʽ  FALSE == ���б��ģʽ
		pMe->m_F_NONET = TRUE; //��������ڿ�����ʱ���û������  TRUE -- ������ʱ��û������  FALSE -- ������
		pMe->m_has_QQSQ = FALSE;
		pMe->m_Rely_XT = FALSE; //��������Ƿ�������Ҫ��������  TRUE = Ҫ��������  FALSE = ��ʾ�������Ҫ���ͣ�ֻ�ǽ�����ʱ
		ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) testgps_StartApplet, (uint32*) pMe);
		return(TRUE);
	}
	else
	{
		pMe->m_PosMode = FALSE;// ��������Ƿ�ֻ�д�����ģʽ  TRUE == ��ʾֻ�д�����ģʽ  FALSE == ���б��ģʽ
		pMe->m_F_NONET = FALSE; //��������ڿ�����ʱ���û������  TRUE -- ������ʱ��û������  FALSE -- ������
		/*
		//		testgps_Printf( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "��ǰ����״̬������" );
		MEMSET(psTextBuf,0x00,MAX_RES_STRING_BUF_SIZE);
		ISHELL_LoadResString( pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_NETSTATUSNAL, psTextBuf, MAX_RES_STRING_BUF_SIZE );
		testgps_Printf_hanzi( pMe, 4, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strnal );
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"��ǰ����״̬������ 1");
		*/
		
	}

	//�����evdo�źź������
	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"testgps_StartApplet_KJZQ �ֻ��ź� = %d",pMe->m_enable_evdo);
	
	if (pMe->m_enable_evdo != 1)
	{
		pMe->m_evdo_Num++;
		if (pMe->m_evdo_Num >= 2 )
		{
			pMe->m_enable_evdo = 1; //�ȴ�����15s�󣬾Ͳ��ȴ��źű�����ǿ����1.
		}

		ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) testgps_StartApplet_KJZQ, (uint32*) pMe);
		return(TRUE);
	}

	gimis_XTRA(pMe);		// ���²�����

	//	testgps_Printf_hanzi( pMe, 6, 5, AEE_FONT_BOLD, IDF_ALIGN_LEFT, strnal );
	pMe->i_HttpMod = 1; //HTTP���շ�������ģʽ ����	1 - ������ջ�������������Ȩ�� 2 - ���ջ�������GPS�����ϴ�����������
	pMe->m_Flag_Qqsq = 1; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����
	pMe->m_Acc = 3;
	ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY) newgimis_WaitForQChat, (uint32*) pMe);

	
//	myiweb_Start(pMe,TEST_URL,0);

	
	return TRUE;
}



static boolean testgps_StartBacktoforeApplet( newgimis *pMe )
{
	
	switch (pMe->m_WitchPag)
	{
	case 0:
		testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
		break;
	case 5:
		testgps_GotoScreen( pMe, SCREENID_SHOWGETGPSINFO, 0 );	
//		testgps_GotoScreen( pMe, SCREENID_SHOWGETGPSINFO, wParam );
		break;
	}
	pMe->m_FOBFlag = FALSE;
	ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY) testgps_StartBacktoforeApplet, (uint32*) pMe);
	return TRUE;
}







////// ������GPS�йصĽ���  /////


///////////////  ������HTTP �йصĿ�ʼ /////////////////////
static void myiweb_Start(newgimis *pMe, char *pszUrl,int iPage)
{
	ISourceUtil *pisu;
	int    i = 0;
    WebOpt awo[10];
	pMe->m_iAppStatus = 1;
	// look to see if there's POST data, this is totally non-standard, but 
	// easy to put into tests
	pMe->pszPostData = STRCHR(pszUrl, 1);  // �˺��������ַ����в����ַ� ���������뵥�ֽ��ַ� 
	
	// if there's post data, construct a stream for IWeb to consume

	pMe->m_has_QQSQ = TRUE;

	if(!pMe->m_pIWeb)
   {
	   ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)(&pMe->m_pIWeb));
	   // set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)

	   
		awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		
		// Marks the end of the array of WebOpts
		awo[i].nId  = WEBOPT_END;
		


  
		
		
		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb,awo);
   }



	if ((char *)0 != pMe->pszPostData) 
	{
		*pMe->pszPostData = 0;
		if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pisu)) 
		{
			
			ISOURCEUTIL_PeekFromMemory(pisu, pMe->pszPostData + 1, 
				STRLEN(pMe->pszPostData + 1), 0, 0,
				&pMe->pipPostData);
			ISOURCEUTIL_Release(pisu);
		}
	}
	
	// initialize the callback, where I'll be called when the request
	// completes
	if(iPage == 0)
	{
		CALLBACK_Cancel(&pMe->cb);
		CALLBACK_Init(&pMe->cb, myiweb_GotResp, pMe);
	}else if(iPage ==1)
	{
		CALLBACK_Cancel(&pMe->cb);
		CALLBACK_Init(&pMe->cb, myiweb_DownloadFile, pMe);
	}

	pMe->iPostPhTm = 0;
	pMe->uStart = GETUPTIMEMS();
	// start transaction, pass callbacks for web status, web headers
	// the extra WEBOPT_HEADER is used to help test what's sent 
	// (snoop.sh above shows all the headers)
	if(!pMe->m_pIWeb)
		ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)(&pMe->m_pIWeb));
	if ((IPeek *)0 != pMe->pipPostData) {
		IWEB_GetResponse(pMe->m_pIWeb,
			(pMe->m_pIWeb, &pMe->piWResp, &pMe->cb, pszUrl, 
			WEBOPT_HANDLERDATA, pMe,
			WEBOPT_HEADER, "X-Method: POST\r\n", /* for kicks */
			WEBOPT_HEADERHANDLER, myiweb_Header, 
			WEBOPT_STATUSHANDLER, myiweb_Status, 
			WEBOPT_METHOD, "POST",
			WEBOPT_BODY, pMe->pipPostData,
			WEBOPT_CONTENTLENGTH, STRLEN(pMe->pszPostData + 1),
			WEBOPT_END));
	} else {
		IWEB_GetResponse(pMe->m_pIWeb,
			(pMe->m_pIWeb, &pMe->piWResp, &pMe->cb, pszUrl, 
			WEBOPT_HANDLERDATA, pMe, 
			WEBOPT_HEADER, "X-Method: GET \r\n",
			WEBOPT_METHOD, "GET",
			WEBOPT_HEADERHANDLER, myiweb_Header, 
			WEBOPT_STATUSHANDLER, myiweb_Status, 
			WEBOPT_END));
	}
}




///////////////  ������HTTP �йصĿ�ʼ /////////////////////
static void myiweb_Start_s(newgimis *pMe, char *pszUrl,int iPage)
{
	ISourceUtil *pisu;
	int    i = 0;
	WebOpt awo[10]; 

	pMe->m_iAppStatus = 1;
	// look to see if there's POST data, this is totally non-standard, but 
	// easy to put into tests
	pMe->pszPostData = STRCHR(pszUrl, 1);  // �˺��������ַ����в����ַ� ���������뵥�ֽ��ַ� 
	
	// if there's post data, construct a stream for IWeb to consume
	if ((char *)0 != pMe->pszPostData) 
	{
		*pMe->pszPostData = 0;
		if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pisu)) 
		{
			
			ISOURCEUTIL_PeekFromMemory(pisu, pMe->pszPostData + 1, 
				STRLEN(pMe->pszPostData + 1), 0, 0,
				&pMe->pipPostData);
			ISOURCEUTIL_Release(pisu);
		}
	}
	
	// initialize the callback, where I'll be called when the request
	// completes
	if(iPage == 0)
	{
		CALLBACK_Cancel(&pMe->cb);
		CALLBACK_Init(&pMe->cb, myiweb_GotResp_c, pMe);
	}else if(iPage ==1)
	{
		CALLBACK_Cancel(&pMe->cb);
		CALLBACK_Init(&pMe->cb, myiweb_DownloadFile, pMe);
	}
	pMe->uStart = GETUPTIMEMS();
	// start transaction, pass callbacks for web status, web headers
	// the extra WEBOPT_HEADER is used to help test what's sent 
	// (snoop.sh above shows all the headers)
	if(!pMe->m_pIWeb_s)
	{
		ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)(&pMe->m_pIWeb_s));

		
		
		// set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)
		awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		
		// Marks the end of the array of WebOpts
		awo[i].nId  = WEBOPT_END;
		
		
		
		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb_s,awo);
		

	}
	if ((IPeek *)0 != pMe->pipPostData) {
		IWEB_GetResponse(pMe->m_pIWeb_s,
			(pMe->m_pIWeb_s, &pMe->piWResp, &pMe->cb, pszUrl, 
			WEBOPT_HANDLERDATA, pMe,
			WEBOPT_HEADER, "X-Method: POST\r\n", /* for kicks */
			WEBOPT_HEADERHANDLER, myiweb_Header, 
			WEBOPT_STATUSHANDLER, myiweb_Status, 
			WEBOPT_METHOD, "POST",
			WEBOPT_BODY, pMe->pipPostData,
			WEBOPT_CONTENTLENGTH, STRLEN(pMe->pszPostData + 1),
			WEBOPT_END));
	} else {
		IWEB_GetResponse(pMe->m_pIWeb_s,
			(pMe->m_pIWeb_s, &pMe->piWResp, &pMe->cb, pszUrl, 
			WEBOPT_HANDLERDATA, pMe, 
			WEBOPT_HEADER, "X-Method: GET \r\n",
			WEBOPT_HEADERHANDLER, myiweb_Header, 
			WEBOPT_STATUSHANDLER, myiweb_Status, 
			WEBOPT_END));
	}
}





//8888 

static void myiweb_GotResp(void *p)
{
	newgimis *pwa = (newgimis *)p;
// 	char *sBuf = NULL;
// 	char *sBuf_d = NULL;
// 	char *sBuf_T = NULL;
// 	char *sBuf1 = NULL;
// 	
// 	int nRead = 0;
// 	int si_Ret = 0;  //����ֵ
// 	char sc_Count[2];
	
	char lStr_msg[100];
	
	
	WebRespInfo *pwri;

	ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"coming myiweb_Start callback"); 

	pwri = IWEBRESP_GetInfo(pwa->piWResp);
	
	//   ND_Print(pMe, "** got response...\n** info code: %d\n", pwri->nCode);
	pwa->m_Flag_Qqsq = 0;

	pwa->iPostPhTm = 0;
	ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"myiweb_Start callback pwri->nCode = %d",pwri->nCode);

	if (!WEB_ERROR_SUCCEEDED(pwri->nCode)) 
	{
		int nErr = 0;

 		if (pwa->m_SQ_C == 1)		 //20180301//���� 554 1289 ���ߵڶ���������󻹻�ص����  �����Ȩ����ΪHTLMҲ���ߵڶ�����������ǻ����
 		{
 			//ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"���Բ���111111111111111111111111111111");
 			myiweb_Stop(pwa);
 			myiweb_Start(pwa,pwa->m_SQ2,0);
 			uf_RecLog(pwa, pwa->m_SQ2 );
 			pwa->m_SQ_C = 2;
 			return;
 		}

 		if (pwa->m_SQ_C == 2)			//20180301		�ڶ���ַ��ֱ�ӱ����������
 		{
			ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"��Ȩ����-----������Ȩ");
			myiweb_Stop(pwa);
		 	myiweb_Start(pwa,pwa->m_SQ1,0);		//���������Ҫ������ʱ������ȻƵ�ʻ�̫�죬δ��
		 	uf_RecLog(pwa, pwa->m_SQ1 );
		 	pwa->m_SQ_C = 1;
		 	return;
 		}

//----------------------start		//ƽ��ҽԺ�汾����
		//�쳣�� ����tcp���ӡ�		//���������������ҳ�����Ҳ�ᵽ��������������ҳ��ȥ����ڶ���Ȩ��ַ����ô�����302����
//		ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"��Ȩ�쳣--WEB_ERROR_SUCCEEDED����--���̶�ip����TCP");
//		STRCPY(pwa->sTCPIP_YYXF,"221.236.9.207");
//		pwa->iTCPPORT_YYXF = 8999;
//		STRCPY(pwa->sUDPIP_YYXF,"221.236.9.207");
//		pwa->iUDPPORT_YYXF = 12777;
//		ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY) initModule_yyxf, (uint32*) pwa);
//----------------------end

		nErr = pwri->nCode;
		SPRINTF(lStr_msg, "request authorization no response, code: %d",nErr);
		ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),lStr_msg); 

		// ����û�з��ͳɹ�
		if (nErr <0) 	nErr = (-1 * nErr);

		pwa->m_Error = nErr;
//		if (pwa->m_WitchPag == 10)
		if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))
		{		
	//		ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_PRINT,0, 0);
			ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
		}

		if (nErr == WEB_ERROR_CONNECT) {	// 1289
			char *sc_buf = NULL;
			//  ��������  gimis�˳�һ���Ӻ����� һ��ͨѶ��ָ�����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT");

//			IPOSDET_Release_fun(pwa);

			ud_RecLog_c(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT");
			if (pwa->cWatchDog != '0')
			{
				if (pwa->m_W_E++ > 3) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
				{

					uf_RecLog(pwa,"WatchDog Reboot");
					IPOSDET_Release_fun(pwa);
					STRCPY(sc_buf,"123"); //�������
				}
			}
			else
			{
				if (pwa->m_E_SC > 0)
				{
					if (pwa->m_W_E_P++ > pwa->m_E_SC) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
					{

						uf_RecLog(pwa,"Play error Sound");
						SoundPlay_Start( pwa );
						pwa->m_W_E_P = 0;
					}
				}
				
			}
			
			

		}	// 1289
		else {
			// �������ֻ�����ԭ�� ����Ƿ�ѵ����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"no response, nErr=%d",nErr);
			if (nErr == 554)
			{
				pwa->m_F_QF = TRUE;  // ˵���ֻ�Ƿ��
				pwa->m_PosMode = TRUE; //ֻ����������
			}
			
		}
		// Add End
		ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_QQSQ_SB_WLYC,0, 0);	// //������Ȩʧ�ܵķ����¼�	 //����Ҳ��ʾ������Ȩʧ��
		myiweb_Stop(pwa);
		return ;
	}	

	pwa->m_Error = pwri->nCode;
	if (pwa->m_Error < 0)
	{
		pwa->m_Error = (-1 * pwa->m_Error);
	}
//	if (pwa->m_WitchPag == 10)
	if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
	{		
		ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
	}

	if ((ISource *)0 != pwri->pisMessage)
	{
		pwa->m_W_E = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�
		pwa->m_W_E_P = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ�򲥷�¼��
		
		ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"coming myiweb_Start callback has get message"); 
		pwa->pSource = pwri->pisMessage;
		ISOURCE_AddRef(pwa->pSource);
		
		IWEBRESP_Release(pwa->piWResp);
		pwa->piWResp = 0;
		
		CALLBACK_Cancel(&pwa->cb);
		CALLBACK_Init(&pwa->cb, myiweb_ReadTaskBody, pwa);
		myiweb_ReadTaskBody(pwa);
	}
}


static void myiweb_ReadTaskBody(newgimis *pwa)
{
	char *sBuf = NULL;
	char *sBuf_d = NULL;
	char *sBuf_T = NULL;
	char *sBuf1 = NULL;
	
	int nRead = 0;
	int si_Ret = 0;  //����ֵ
	char sc_Count[2];
	
	//char lStr_msg[100];
	ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"coming myiweb_ReadTaskBody");
	
	sBuf = (char *)MALLOC(1024);
	MEMSET(sBuf,0x00,sizeof(sBuf));
	
	sBuf_d = (char *)MALLOC(1024);
	MEMSET(sBuf_d,0x00,sizeof(sBuf_d));
	
	nRead = ISOURCE_Read(pwa->pSource, sBuf, 1023);
	if(ISOURCE_WAIT == nRead) 
	{
		ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"QQSQ ,ISOURCE_WAIT"); 
		//		ISOURCE_Readable(me->pSource, &me->webcb);
		//		ISOURCEUTIL_Release(pwa->pSource);

		ISOURCE_Readable(pwa->pSource, &pwa->cb);	//##2016-2-4## ��Ժ�����һ�ν���ʱWAIT����������ڶ�
		
		FREE(sBuf);
		sBuf = NULL;
		FREE(sBuf_d);
		sBuf_d = NULL;
		
		// 			pwa->m_F_R_SQ = 0; 
		
		// 			FREE(pwa->m_H_SQ);
		// 			pwa->m_H_SQ = NULL;
		
		//			myiweb_Stop(pwa);
		return;
	}
	
	if (nRead > 0)		
	{
		DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   successful");
		DBGPRINTF(sBuf);
		//		IHTMLVIEWER_LoadSource(pwa->m_pHTMLViewer, pwri->pisMessage);
		SPRINTF(sc_Count,"%d",nRead);
		DBGPRINTF(sc_Count);
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf); 
		// ����������Ȩ�ķ��ص�ֵ��д�������ļ�
		if (pwa->i_HttpMod == 1)	//HTTP���շ�������ģʽ ����	1 - ������ջ�������������Ȩ�� 2 - ���ջ�������GPS�����ϴ�����������)
		{
			
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," nRead  QQSQ = %d",nRead); 
			
			if (nRead > 100)
			{
				sBuf1 = (char *)MALLOC(1024);
				sBuf_T = (char *)MALLOC(102400);
				
				while (!sBuf_T)
				{
					ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," sBuf_T QQSQ Failure to apply for space 1"); 
					sBuf_T = (char *)MALLOC(102400);
				}
				MEMSET(sBuf_T,0x00,102400);
				STRCPY(sBuf_T,sBuf);
			}
			else
			{
				sBuf1 = (char *)MALLOC(1024);
				sBuf_T = (char *)MALLOC(1024);
				while(!sBuf_T)
				{
					ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," sBuf_T QQSQ Failure to apply for space 2"); 
					sBuf_T = (char *)MALLOC(1024);
				}
				MEMSET(sBuf_T,0x00,1024);
				STRCPY(sBuf_T,sBuf);
			}
			
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," nRead  QQSQ 2 = %d",nRead);
			while (nRead > 100)
			{
				
				MEMSET(sBuf1,0x00,1024);
				nRead = ISOURCE_Read(pwa->pSource, sBuf1, 1023);
				STRCAT(sBuf_T,sBuf1);
				ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"while nRead QQSQ = %d",nRead); 
				ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf1); 
			}
			
			if(ISOURCE_WAIT == nRead) 
			{
				uf_RecLog(pwa," ISOURCE_Readable QQSQ");
				
				if (pwa->m_F_R_SQ == 1) // ˵��һ�������������ε� ISOURCE_WAIT �ⲻ�ǵ�һ�γ��� 
				{
//					STRCAT(pwa->m_H_GPS,sBuf_T);
				}
				else // ��һ�γ���
				{
					pwa->m_H_SQ = (char *) MALLOC(102400);
					MEMSET(pwa->m_H_SQ,0x00,102400);
					STRCPY(pwa->m_H_SQ,sBuf_T);
				}
				
				pwa->m_F_R_SQ = 1; //

				ISOURCE_Readable(pwa->pSource, &pwa->cb);
				
				FREEIF(sBuf_T);
				sBuf_T = NULL ;
				
				FREEIF(sBuf1);
				sBuf1 = NULL ;
				
				FREE(sBuf);
				sBuf = NULL;
				FREE(sBuf_d);
				sBuf_d = NULL;
				
				return;
			}
			
			if (pwa->m_F_R_SQ == 1 && pwa->m_H_SQ)
			{
				STRCAT(pwa->m_H_SQ,sBuf_T);
				uf_RecLog(pwa,"wait time STRCAT HTTP CONTENT QQSQ");
				uf_RecLog(pwa,"wait time STRCAT HTTP CONTENT QQSQ");
				MEMSET(sBuf_T,0x00,102400);
				STRCPY(sBuf_T,pwa->m_H_SQ);
				uf_RecLog(pwa,"wait time 1 STRCAT HTTP CONTENT QQSQ");
				uf_RecLog(pwa,"wait time 1 STRCAT HTTP CONTENT QQSQ");
				pwa->m_F_R_SQ = 0; //
				FREE(pwa->m_H_SQ);
				pwa->m_H_SQ = NULL;
			}
			
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," sBuf_T QQSQ = %d ",STRLEN(sBuf_T)); 
			uf_RecLog(pwa,sBuf_T);
			
			uf_fromESC(sBuf_T,sBuf_d);

			si_Ret = ParseStringandWriteToConfigure(pwa,sBuf_d);

			FREEIF(sBuf_T);
			sBuf_T = NULL ;
			
			FREEIF(sBuf1);
		    sBuf1 = NULL ;



			if (si_Ret)  //˵���ɹ�
			{
				//gimis_WriteGPSSettings(pwa,);
				pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
				uf_RecLog(pwa,"gimis_SetGPSPara coming");
				gimis_SetGPSPara(pwa); 
				uf_RecLog(pwa,"gimis_SetGPSPara out");
				if (pwa->m_SQ_again == 0)
				{
					
					if (pwa->cAuthorize == '1')
					{
						ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_QQSQ_CG,0, 0);	 //������Ȩ�ɹ��ķ����¼�	
					}
					else
					{
						ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_QQSQ_SB,0, 0);	// //������Ȩʧ�ܵķ����¼�		
					}
				}
				
				
				//		ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) testgps_StartApplet, (uint32*) pMe);
			}
			else
			{
				if (pwa->m_SQ_C == 1)
				{
					myiweb_Stop(pwa);
					FREE(sBuf);
					sBuf = NULL;
					FREE(sBuf_d);
					sBuf_d = NULL;
					myiweb_Start(pwa,pwa->m_SQ2,0);
					uf_RecLog(pwa, pwa->m_SQ2 );
					pwa->m_SQ_C = 2;
					return ;
					
				}
				ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_QQSQ_SB,0, 0);	// //������Ȩʧ�ܵķ����¼�	
			}
		}
		else if (pwa->i_HttpMod == 2)
		{
			
		}
		
		FREE(sBuf);
		sBuf = NULL;
		FREE(sBuf_d);
		sBuf_d = NULL;
	}
	myiweb_Stop(pwa);
	if (pwa->m_SQ_again == 1)
	{
		pwa->m_SQ_again = 0;
		pwa->i_HttpMod = 1;
		uf_RecLog(pwa, pwa->strCenterIp );
		
		
		if (STRCMP(pwa->MobileNoDJ,pwa->MyMobileNo) == 0) // ˵�� ������ͬ
		{
			MEMSET(pwa->m_SQ1,0x00,250);
			SPRINTF(pwa->m_SQ1,"http://%s&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pwa->strCenterIp,pwa->MobileNoDJ,pwa->szMobileID);
			
			//		MEMSET(pMe->m_SQ2,0x00,250);
			//		SPRINTF(pMe->m_SQ2,"http://%s&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->strCenterIp,pMe->MobileNoDJ,pMe->szMobileID);
			
		}
		else
		{
			if (STRLEN(pwa->MyMobileNo) < 1)
			{
				MEMSET(pwa->m_SQ1,0x00,250);
				SPRINTF(pwa->m_SQ1,"http://%s&tel=%s&ver=3.1.2au&pp=FU/ctvH",pwa->strCenterIp,pwa->MobileNoDJ);
				
				//			MEMSET(pMe->m_SQ2,0x00,250);
				//			SPRINTF(pMe->m_SQ2,"http://%s&tel=%s&ver=3.1.2au&pp=FU/ctvH",pMe->strCenterIp,pMe->MobileNoDJ);
				
			}
			else
			{
				
				MEMSET(pwa->m_SQ1,0x00,250);
				SPRINTF(pwa->m_SQ1,"http://%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pwa->strCenterIp,pwa->szMobileID);
				
				//		MEMSET(pMe->m_SQ2,0x00,250);
				//		SPRINTF(pMe->m_SQ2,"http://gps.richtalk.cn/brewGPS/BrewServlet?act=autdec&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->szMobileID);
				
			}
			
		}
		pwa->m_SQ_C = 1; //��ʹ������ַ
		
		uf_RecLog(pwa, pwa->m_SQ1 );
		
		myiweb_Start(pwa,pwa->m_SQ1,0);
		
	}
}


static void myiweb_HtmlError(newgimis *pwa)
{
	myiweb_Stop(pwa);

	pwa->i_HttpMod = 1;
	uf_RecLog(pwa, pwa->strCenterIp );
	
	if (STRCMP(pwa->MobileNoDJ,pwa->MyMobileNo) == 0) // ˵�� ������ͬ
	{
		MEMSET(pwa->m_SQ1,0x00,250);
		SPRINTF(pwa->m_SQ1,"http://%s&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pwa->strCenterIp,pwa->MobileNoDJ,pwa->szMobileID);		
	}
	else
	{
		if (STRLEN(pwa->MyMobileNo) < 1)
		{
			MEMSET(pwa->m_SQ1,0x00,250);
			SPRINTF(pwa->m_SQ1,"http://%s&tel=%s&ver=3.1.2au&pp=FU/ctvH",pwa->strCenterIp,pwa->MobileNoDJ);
		}
		else
		{
			MEMSET(pwa->m_SQ1,0x00,250);
			SPRINTF(pwa->m_SQ1,"http://%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pwa->strCenterIp,pwa->szMobileID);	
		}
		
	}
	pwa->m_SQ_C = 1; //��ʹ������ַ
	uf_RecLog(pwa, pwa->m_SQ1 );
	myiweb_Start(pwa,pwa->m_SQ1,0);
}



static void myiweb_GotResp_c(void *p)
{
	newgimis *pwa = (newgimis *)p;
	char *sBuf = NULL;
	char *sBuf_d = NULL;
	int nRead = 0;
	int si_Ret = 0;  //����ֵ
	char sc_Count[2];
	
	char lStr_msg[100];
	
	
	WebRespInfo *pwri;
	
	// get information about how the web transaction went
	// pwa->piWResp is ***NEVER NULL***, even though the transaction may fail
	// for wont of memory
	pwri = IWEBRESP_GetInfo(pwa->piWResp);
	
	//   ND_Print(pMe, "** got response...\n** info code: %d\n", pwri->nCode);


	if (!WEB_ERROR_SUCCEEDED(pwri->nCode)) 
	{
		int nErr = 0;
		nErr = pwri->nCode;
		SPRINTF(lStr_msg, "three GPS info no response, code: %d",nErr);
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),lStr_msg); 

		// ����û�з��ͳɹ�
		if (nErr <0) 	nErr = (-1 * nErr);

		pwa->m_Error = nErr;
//		if (pwa->m_WitchPag == 10)
		if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))
		{		
	//		ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_PRINT,0, 0);
			ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
		}
		
		

//		 myiweb_Stop(pwa,1); // stop cleans up

		

		if (nErr == WEB_ERROR_CONNECT) {	// 1289
			char *sc_buf = NULL;
			//  ��������  gimis�˳�һ���Ӻ����� һ��ͨѶ��ָ�����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT");

//			IPOSDET_Release_fun(pwa);
			ud_RecLog_c(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT");
			if (pwa->cWatchDog != '0')
			{
				if (pwa->m_W_E++ > 3) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
				{

					uf_RecLog(pwa,"WatchDog Reboot");
					IPOSDET_Release_fun(pwa);
					STRCPY(sc_buf,"123"); //�������
				}
			}
			else
			{
				if (pwa->m_E_SC > 0)
				{
					if (pwa->m_W_E_P++ > pwa->m_E_SC) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
					{

						uf_RecLog(pwa,"Play error Sound");
						SoundPlay_Start( pwa );
						pwa->m_W_E_P = 0;
					}
				}
			}

		}	// 1289
		else {
			// �������ֻ�����ԭ�� ����Ƿ�ѵ����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"no response, nErr=%d",nErr);
			if (nErr == 554)
			{
				pwa->m_F_QF = TRUE;  // ˵���ֻ�Ƿ��
				pwa->m_PosMode = TRUE; //ֻ����������
			}
		}
		// Add End
		myiweb_Stop_s(pwa);
		return ;
	}	

	pwa->m_Error = pwri->nCode;
	if (pwa->m_Error < 0)
	{
		pwa->m_Error = (-1 * pwa->m_Error);
	}
//	if (pwa->m_WitchPag == 10)
	if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
	{		
		ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
	}

	
	// body may be NULL
	if ((ISource *)0 != pwri->pisMessage) 
	{
		// �˺����� ISource �����������ĵ� �������ݰ���HTML ��ʽ���� ɾ��������ǰ������ 
//		IHTMLVIEWER_LoadSource(pwa->m_pHTMLViewer, pwri->pisMessage);
		DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   1");
		pwa->m_W_E = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
		pwa->m_W_E_P = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ�򲥷�¼��
		
//		MEMSET(sc_Count,0x00,sizeof(sBuf));
		
		pwa->pSource = pwri->pisMessage;
		ISOURCE_AddRef(pwa->pSource);	
		sBuf = (char *)MALLOC(1024);
		MEMSET(sBuf,0x00,sizeof(sBuf));
		
		sBuf_d = (char *)MALLOC(1024);
		MEMSET(sBuf_d,0x00,sizeof(sBuf_d));

		nRead = ISOURCE_Read(pwa->pSource, sBuf, 1024);
		if(ISOURCE_WAIT == nRead) 
		{
			//		ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cAudioApp_ReadWebBody in,ISOURCE_WAIT");
			//		ISOURCE_Readable(me->pSource, &me->webcb);
			//		ISOURCEUTIL_Release(pwa->pSource);
			//		return;


//			ISOURCE_Readable(pwa->pSource, &pwa->cb);
			FREE(sBuf);
			sBuf = NULL;
			FREE(sBuf_d);
			sBuf_d = NULL;

			return;
		}
		
		if (nRead > 0)		
		{
			DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   successful");
			DBGPRINTF(sBuf);
			//		IHTMLVIEWER_LoadSource(pwa->m_pHTMLViewer, pwri->pisMessage);
			SPRINTF(sc_Count,"%d",nRead);
			DBGPRINTF(sc_Count);
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf); 
			// ����������Ȩ�ķ��ص�ֵ��д�������ļ�
			if (pwa->i_HttpMod == 2)	//HTTP���շ�������ģʽ ����	1 - ������ջ�������������Ȩ�� 2 - ���ջ�������GPS�����ϴ�����������)
			{							// 5 -- ����Ȩ��δ��Ȩ ����ر�
				uf_fromESC(sBuf,sBuf_d);
				si_Ret = ParseStringandWriteToConfigure_xintiao_GPS(pwa,sBuf_d);
				switch (si_Ret)  //˵���ɹ�
				{
				case 0:
					{
						// ˵������ʧ��
					}
					break;	
				case 1:
					{
						//gimis_WriteGPSSettings(pwa,);
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 					
					}
					break;	 
				case 5:
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 	
						// �رճ���
						pwa->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
					
						ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_WSQ, (uint32*) pwa); //�˳�����
						break;
					}
				}
				
			}
			else if (pwa->i_HttpMod == 1)
			{
				
			}
			
			FREE(sBuf);
			sBuf = NULL;

			FREE(sBuf_d);
			sBuf_d = NULL;
		}

		myiweb_Stop_s(pwa);
	}
	
}



static void myiweb_GotResp_gps(void *p)
{
	newgimis *pwa = (newgimis *)p;
	char lStr_msg[100];
	WebRespInfo *pwri;

	uf_RecLog( pwa , "into myiweb_GotResp_gps");
	
	//MEMSET(pwa->copy_sUrl_GPS, 0x00, sizeof(pwa->copy_sUrl_GPS));

	// pwa->piWResp is ***NEVER NULL***, even though the transaction may fail
	// for wont of memory
	pwri = IWEBRESP_GetInfo(pwa->piWResp_gps);
	
	//   ND_Print(pMe, "** got response...\n** info code: %d\n", pwri->nCode);

	
	pwa->iPostPhTm_gps = 0;
	ISHELL_SetTimer(pwa->a.m_pIShell, 30 * 1000, (PFNNOTIFY)uf_MonitorWait_GPS, pwa);


	if (!WEB_ERROR_SUCCEEDED(pwri->nCode)) 
	{
		int nErr = 0;
		nErr = pwri->nCode;
		SPRINTF(lStr_msg, "three gps info no response 2, code: %d",nErr);
		ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),lStr_msg); 

		// ����û�з��ͳɹ�
		if (nErr <0) 	nErr = (-1 * nErr);
		//////////////////////////GPS�������ݰ�////////////////
		if (pwa->mux_7Day == 0)
	   {
		   write_GPSData_7Day(pwa, pwa->sUrl);
	   }
		
		//--ԭ��������
// 		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"write File_Write_OfflineData_Gps 2"); 
// 		File_Write_OfflineData_Gps(pwa, pwa->sUrl);
// 		pwa->m_offlineData_gps = 1;
		//-end
		
		pwa->m_Error = nErr;
//		if (pwa->m_WitchPag == 10)
		if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
		{		
	//		ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_PRINT,0, 0);
			ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
		}

		if (nErr == WEB_ERROR_CONNECT) {	// 1289
			char *sc_buf = NULL;
			//  ��������  gimis�˳�һ���Ӻ����� һ��ͨѶ��ָ�����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT");
//			IPOSDET_Release_fun(pwa);

			ud_RecLog_c(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT");
			if (pwa->cWatchDog != '0')
			{
				if (pwa->m_W_E++ > 3) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
				{

					uf_RecLog(pwa,"WatchDog Reboot");
					IPOSDET_Release_fun(pwa);
					STRCPY(sc_buf,"123"); //�������
				}
			}
			else
			{
				if (pwa->m_E_SC > 0)
				{
					if (pwa->m_W_E_P++ > pwa->m_E_SC) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
					{

						uf_RecLog(pwa,"Play error Sound");
						SoundPlay_Start( pwa );
						pwa->m_W_E_P = 0;
					}
				}
			}
		}	// 1289
		else {
			// �������ֻ�����ԭ�� ����Ƿ�ѵ����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"no response, nErr=%d",nErr);
			if (nErr == 554)
			{
				pwa->m_F_QF = TRUE;  // ˵���ֻ�Ƿ��
				pwa->m_PosMode = TRUE; //ֻ����������
			}
		}
		// Add End
		myiweb_Stop_gps(pwa);
		// ��HTTP���� 
		// // m_MUTEX; // 1 = ˵�����ܽ��з���HTTP �Ѿ���ռ����http����Դ  0= ˵������ʹ��http��Դ
		pwa->m_MUTEX = 0;
		return ;
	}	


	pwa->m_Error = pwri->nCode;
	if (pwa->m_Error < 0)
	{
		pwa->m_Error = (-1 * pwa->m_Error);
	}
//	if (pwa->m_WitchPag == 10)
	if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
	{		
		ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
	}

	if ((ISource *)0 != pwri->pisMessage) 
	{
		pwa->m_W_E = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
		pwa->m_W_E_P = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ�򲥷�¼��

		pwa->pSource_gps = pwri->pisMessage;
		ISOURCE_AddRef(pwa->pSource_gps);
		
		IWEBRESP_Release(pwa->piWResp_gps);
		pwa->piWResp_gps = 0;
		
		CALLBACK_Cancel(&pwa->cb_gps);
		CALLBACK_Init(&pwa->cb_gps, myiweb_gps_ReadTaskBody, pwa);
		myiweb_gps_ReadTaskBody(pwa);
	}
}

static void myiweb_gps_ReadTaskBody(newgimis *pwa)
{
	char *sBuf = NULL;
	char *sBuf_d = NULL;
	char *sBuf_T = NULL;
	char *sBuf1 = NULL;
	int nRead = 0;
	int si_Ret = 0;  //����ֵ
	char sc_Count[2];
	
	ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"coming myiweb_gps_ReadTaskBody");

	sBuf = (char *)MALLOC(1024);
	MEMSET(sBuf,0x00,1024);

	while (!sBuf)
	{
		ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"myiweb_gps_ReadTaskBody sBuf malloc fail ");
		sBuf = (char *)MALLOC(1024);
		MEMSET(sBuf,0x00,1024);
	}
	
	sBuf_d = (char *)MALLOC(102400);
	MEMSET(sBuf_d,0x00,102400);

	while (!sBuf_d)
	{
		ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"myiweb_gps_ReadTaskBody sBuf_d malloc fail ");
		sBuf_d = (char *)MALLOC(102400);
		MEMSET(sBuf_d,0x00,102400);
	}
	
	nRead = ISOURCE_Read(pwa->pSource_gps, sBuf, 1023);
	if(ISOURCE_WAIT == nRead) 
	{
		ISOURCE_Readable(pwa->pSource_gps, &pwa->cb_gps);
		FREE(sBuf);
		sBuf = NULL;
		FREE(sBuf_d);
		sBuf_d = NULL;
		
		if (pwa->m_F_R_GPS == 0)
		{
			FREE(pwa->m_H_GPS);
			pwa->m_H_GPS = NULL;
			pwa->m_F_R_GPS = 0;
		}
		return;
	}
	
	if (nRead > 0)		
	{
		DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   successful");
		DBGPRINTF(sBuf);
		//		IHTMLVIEWER_LoadSource(pwa->m_pHTMLViewer, pwri->pisMessage);
		SPRINTF(sc_Count,"%d",nRead);
		DBGPRINTF(sc_Count);
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf); 
		// ����������Ȩ�ķ��ص�ֵ��д�������ļ�
		if (pwa->i_HttpMod == 2)	//HTTP���շ�������ģʽ ����	1 - ������ջ�������������Ȩ�� 2 - ���ջ�������GPS�����ϴ�����������)
		{							// 5 -- ����Ȩ��δ��Ȩ ����ر�
			
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," nRead  GPS = %d",nRead); 
			
			if (nRead > 1)
			{
				sBuf1 = (char *)MALLOC(1024);
				sBuf_T = (char *)MALLOC(102400);
				
				while (!sBuf_T)
				{
					ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," sBuf_T GPS Failure to apply for space 1"); 
					sBuf_T = (char *)MALLOC(102400);
				}
				MEMSET(sBuf_T,0x00,102400);
				STRCPY(sBuf_T,sBuf);
			}
			else
			{
				sBuf_T = (char *)MALLOC(1024);
				MEMSET(sBuf_T,0x00,1024);
				STRCPY(sBuf_T,sBuf);
			}
			
			while (nRead > 1)
			{
				MEMSET(sBuf1,0x00,1024);
				nRead = ISOURCE_Read(pwa->pSource_gps, sBuf1, 1023);
				STRCAT(sBuf_T,sBuf1);
				ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"while nRead GPS = %d",nRead); 
				ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf1); 
			}
			
			if(ISOURCE_WAIT == nRead) 
			{
				uf_RecLog(pwa," ISOURCE_Readable GPS");
				
				if (pwa->m_F_R_GPS == 1) // ˵��һ�������������ε� ISOURCE_WAIT �ⲻ�ǵ�һ�γ��� 
				{
					STRCAT(pwa->m_H_GPS,sBuf_T);
				}
				else // ��һ�γ���
				{
					pwa->m_H_GPS = (char *) MALLOC(102400);
					while (!pwa->m_H_GPS)
					{
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," m_H_GPS malloc fail"); 
						pwa->m_H_GPS = (char *)MALLOC(102400);
					}
					MEMSET(pwa->m_H_GPS,0x00,102400);
					STRCPY(pwa->m_H_GPS,sBuf_T);
				}
	
				pwa->m_F_R_GPS = 1; //

				ISOURCE_Readable(pwa->pSource_gps, &pwa->cb_gps);
				
				FREEIF(sBuf_T);
				sBuf_T = NULL ;
				
				FREEIF(sBuf1);
				sBuf1 = NULL ;
				
				FREE(sBuf);
				sBuf = NULL;
				FREE(sBuf_d);
				sBuf_d = NULL;
				
				
				return;
			}
			
			if (pwa->m_F_R_GPS == 1 && pwa->m_H_GPS)
			{
				STRCAT(pwa->m_H_GPS,sBuf_T);
				uf_RecLog(pwa,"wait time STRCAT HTTP CONTENT GPS");
				uf_RecLog(pwa,"wait time STRCAT HTTP CONTENT GPS");
				MEMSET(sBuf_T,0x00,102400);
				STRCPY(sBuf_T,pwa->m_H_GPS);
				uf_RecLog(pwa,"wait time 1 STRCAT HTTP CONTENT GPS");
				uf_RecLog(pwa,"wait time 1 STRCAT HTTP CONTENT GPS");
				pwa->m_F_R_GPS = 0; //
				FREE(pwa->m_H_GPS);
				pwa->m_H_GPS = NULL;
			}
			
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," sBuf_T gps = %d ",STRLEN(sBuf_T)); 
			uf_RecLog(pwa,sBuf_T);
			
			uf_fromESC(sBuf_T,sBuf_d);

//			uf_RecLog(pwa,"11111111111111111111111111111");
			si_Ret = ParseStringandWriteToConfigure_xintiao_GPS(pwa,sBuf_d);

			FREEIF(sBuf_T);
			sBuf_T = NULL ;
			FREEIF(sBuf1);
			sBuf1 = NULL ;

//			uf_RecLog(pwa,"222222222222222222222222222222");
			switch (si_Ret)  //˵���ɹ�
			{
			case 0:
				{	
				}
				break;	
			case 1:
				{
					//gimis_WriteGPSSettings(pwa,);
					pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
					gimis_SetGPSPara(pwa); 		
				}
				break;
				
			case 5:
				{
					pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
					gimis_SetGPSPara(pwa); 	
					// �رճ���
					pwa->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
					
					ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_WSQ, (uint32*) pwa); //�˳�����
					break;
				}
			}
			
			}
			else if (pwa->i_HttpMod == 1)
			{
			}
			
			FREE(sBuf);
			sBuf = NULL;
			
			FREE(sBuf_d);
			sBuf_d = NULL;
		}
		
		myiweb_Stop_gps(pwa);
		// ��HTTP���� 
		// // m_MUTEX; // 1 = ˵�����ܽ��з���HTTP �Ѿ���ռ����http����Դ  0= ˵������ʹ��http��Դ
		pwa->m_MUTEX = 0;
}



static void myiweb_DownloadFile(void * p)
{
	/*
	newgimis *pMe = (newgimis *)p;
	
	WebRespInfo *pwri;
	char targetFile[24];
	byte buf[1024];
	int ByteCount;
	IFile * pFile;
	   DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa  5");
	   // get information about how the web transaction went
	   // pwa->piWResp is ***NEVER NULL***, even though the transaction may fail
	   // for wont of memory

	   pwri = IWEBRESP_GetInfo(pMe->piWResp);
	   
	   //   ND_Print(pMe, "** got response...\n** info code: %d\n", pwri->nCode);
	   if(WEB_ERROR_SUCCEEDED(pwri->nCode))
		   DBGPRINTF("request successful!");
	   else{
		   WEB_ERROR_MAP(pwri->nCode);
		   DBGPRINTF("request successful %i",pwri->nCode);
	   }
	   // body may be NULL
	   if ((ISource *)0 != pwri->pisMessage) 
	   {
		   ISource * pISource = pwri->pisMessage;
//		   ByteCount =	ISOURCE_Read(pISource, (byte*)buf, sizeof(buf));

		   switch(ByteCount)
		   {
		   case ISOURCE_END:
			   if (pMe->m_BodySize < pMe->m_BodyAllocSize)
				   pMe->m_BodyBuffer[pMe->m_BodySize] = 0;
			   else
				   pMe->m_BodyBuffer[pMe->m_BodyAllocSize - 1] = 0;
			   
			   if(!pMe->m_pIFileMgr)
				   ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_FILEMGR, (void **)&pMe->m_pIFileMgr);
			   if(pMe->m_iMediaType == 1)
			   {
				   if (IFILEMGR_MkDir(pMe->m_pIFileMgr, "pic") != SUCCESS)
				   {	
					   IFILEMGR_Release(pMe->m_pIFileMgr);
					   pMe->m_pIFileMgr = NULL;
					   return;
				   }
				   STRCPY(targetFile,"pic\\");
			   }else if(pMe->m_iMediaType == 2)
			   {
				   if (IFILEMGR_MkDir(pMe->m_pIFileMgr, "music") != SUCCESS)
				   {	
					   IFILEMGR_Release(pMe->m_pIFileMgr);
					   pMe->m_pIFileMgr = NULL;
					   return;
				   }
				   STRCPY(targetFile,"music\\");
			   }
			   STRCAT(targetFile,pMe->m_pszFileName);
			   if(IFILEMGR_Test(pMe->m_pIFileMgr,targetFile)==SUCCESS)
				   IFILEMGR_Remove(pMe->m_pIFileMgr,targetFile);
			   pFile= IFILEMGR_OpenFile(pMe->m_pIFileMgr,targetFile,_OFM_CREATE);
			   IFILE_Write(pFile,(void*)pMe->m_BodyBuffer,pMe->m_BodyAllocSize);
			   IFILE_Release(pFile);
			   IFILEMGR_Release(pMe->m_pIFileMgr);
			   pMe->m_pIFileMgr = NULL;
			   
			   {
				   AECHAR szFile[64];
				   AECHAR szText[64];
				   STRTOWSTR(pMe->m_pszFileName,szFile,64);
				   ISHELL_LoadResString(pMe->a.m_pIShell, MYHTTP_RES_FILE, IDS_DOWNLOAD_OK, szText, sizeof(szText));
				   WSTRCAT(szFile,szText);
				   myiweb_DisplayInfo(pMe,szFile);
			   }
			   FREEIF(pMe->m_pszFileName);
			   break;
		   case ISOURCE_ERROR:
			   break;
		   case ISOURCE_WAIT:
			   ISOURCE_Readable(pISource, &pMe->cb);
			   break;
		   default:
			   
			   if (ByteCount)
			   {
				   if (pMe->m_BodySize + ByteCount > pMe->m_BodyAllocSize)
				   {
					   const int NewSize = pMe->m_BodyAllocSize + 1024;
					   byte* NewBuf = (byte*)REALLOC(pMe->m_BodyBuffer, NewSize);
					   
					   if (NewBuf)
					   {
						   pMe->m_BodyBuffer = NewBuf;
						   pMe->m_BodyAllocSize = NewSize;
					   }
				   }
				   if(pMe->m_BodySize + ByteCount <= pMe->m_BodyAllocSize)
				   {
					   MEMCPY(pMe->m_BodyBuffer + pMe->m_BodySize, buf, ByteCount);
					   pMe->m_BodySize += ByteCount;
				   }
			   }
			   
			   ISOURCE_Readable(pISource, &pMe->cb);
			   break;
		   } 
	   }	   
   */
}




/*===========================================================================

FUNCTION: myiweb_Header

DESCRIPTION:
   Received header callback for a web transaction. cpszName is NULL in the case 
   of continuation header line parts.

PARAMETERS:
   p: a myiweb (the subscriber)
   cpszName: the name of the web header (like "Content-Type")
   pglVal: the value of the header, like "text/html"

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void myiweb_Header(void *p, const char *cpszName, GetLine *pglVal)
{
   newgimis *pwa = (newgimis *)p;
  
  
 //  if (pMe->m_bRS) { // If response is to be displayed
      if ((char *)0 != cpszName) {
        // ND_Print(pMe, "%s:", cpszName);
      }
      //ND_Print(pMe, "%s\n", pglVal->psz);
  // }
}

/*===========================================================================

FUNCTION: myiweb_Status

DESCRIPTION:
   Web status callback for a Web transaction 

PARAMETERS:
   p: a myiweb (the subscriber)
   ws: type of status
   pVal: unused as of yet

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void myiweb_Status(void *p, WebStatus ws, void *pVal)
{
   AECHAR     szText[32];
   char tmp[32];
   newgimis * pMe = (newgimis *)p;
   
   (void)pVal;

   switch (ws) {
   case WEBS_CANCELLED:
	  ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CANCELLED, szText, sizeof(szText));
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"������Ȩ �Ѿ�ȡ��");
	  ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"QQSQ cancel");
      break;
   case WEBS_GETHOSTBYNAME:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GETHOSTBYNAME,szText, sizeof(szText));
//	  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"������Ȩ ��ȡ����");
	  ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"QQSQ get host");
      break;          
   case WEBS_CONNECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CONNECT, szText, sizeof(szText));
//	  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"������Ȩ ��������");
	  ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"QQSQ connect");
	  if (pMe->iPostPhTm == 0) {			//  [3/14/2016 Administrator]
		  ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"QQSQ connect 2 settimer");
		  pMe->iPostPhTm = GETTIMEMS();
		  ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost, pMe);
		}		// Add End
      break;
   case WEBS_SENDREQUEST:
		ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_SENDREQUEST, szText, sizeof(szText));
//	  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"������Ȩ ��������");
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"QQSQ request to send");
		pMe->iPostPhTm = GETTIMEMS();
		ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost, pMe);
      break;
   case WEBS_READRESPONSE:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_READRESPONSE, szText, sizeof(szText));
//	  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"������Ȩ ��������");
	  ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"QQSQ request to receive");
      break;
   case WEBS_GOTREDIRECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GOTREDIRECT, szText, sizeof(szText));
//	  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"������Ȩ �ض���");
	  ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"QQSQ redirect");
      break;
   case WEBS_CACHEHIT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CACHEHIT, szText, sizeof(szText));
//	  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"������Ȩ ������");
	  ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"QQSQ cache");
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   4");
      break;

	default:
		if (pMe->iPostPhTm == 0) {			//  Add 2013-09-23
		   pMe->iPostPhTm = GETTIMEMS();
		   ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost, pMe);
		}		// Add End
		break;
   }

   // show that status!
   if ((AECHAR *)0 != szText) 
   {
  //    myiweb_DisplayInfo(pMe,szText);
	   SPRINTF(tmp,"ws=%d",ws);

//	   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),tmp);
	  DBGPRINTF("bbbbbbbbbbbbbbbbbbbbbbbbbbb");
   }
}


// static void	 myiweb_DisplayInfo(newgimis* pMe,AECHAR* pInfo)
// {
// 		AEERect rct;
// 	
// 	if(!pMe->m_pIStatic)
// 	ISHELL_CreateInstance(pMe->pIShell, AEECLSID_STATIC, (void **)&pMe->m_pIStatic);
// 	SETAEERECT(&rct, 16, 16, pMe->DeviceInfo.cxScreen-32, pMe->DeviceInfo.cyScreen-32);
// 	ISTATIC_SetRect(pMe->m_pIStatic, &rct);
// 	ISTATIC_SetText(pMe->m_pIStatic, NULL,pInfo, AEE_FONT_BOLD, AEE_FONT_NORMAL);
// 	ISTATIC_Redraw(pMe->m_pIStatic);
// }



/*===========================================================================

  FUNCTION: myiweb_Stop
  
	DESCRIPTION:
	Halts a web transaction, wrapped/represented by a myiweb
	
	  PARAMETERS:
	  pwa: the myiweb
	  
		DEPENDENCIES:
		None
		
		  RETURN VALUE:
		  None
		  
			SIDE EFFECTS:
			None
===========================================================================*/
static void myiweb_Stop(newgimis *pwa)
{
	// this cancels any pending web transaction, or readable on the 
	// response body.  if nothing is pending, this has no effect

	   CALLBACK_Cancel(&pwa->cb);
   
   // then clean up, if necessary
   if ((char *)0 != pwa->pszPostData) {

	   FREE(pwa->pszPostData);
		pwa->pszPostData = NULL;
//		pwa->iFileRead = 0;

//      *pwa->pszPostData_xt = 1; // write delimiter back in, if any
 //     pwa->pszPostData_xt = 0;
   }
 // ����  
   if (pwa->pipPostData != NULL)
	{
		IPEEK_Release(pwa->pipPostData);
		// Add by zjie 2011-01-30
		pwa->pipPostData = NULL;

	}
 // ����  
	if (pwa->pSource != NULL) {
		uint32 nErr = ISOURCE_Release(pwa->pSource);	// 2013-04-12 �� nErr
		pwa->pSource = NULL;
	}

   
	if (pwa->piWResp)
	{
		IWEBRESP_Release(pwa->piWResp);
		pwa->piWResp= NULL;
	}

	if (pwa->m_pIWeb) 
	{
//		IWEB_Release(pwa->m_pIWeb_xt);
//		pwa->m_pIWeb_xt = NULL;



		int i,n;			// 2.6.4D2
		n = 0;
		i=IWEB_Release(pwa->m_pIWeb);
		while (i>0) {
			n++;
			i=IWEB_Release(pwa->m_pIWeb);
		}

		pwa->m_pIWeb = NULL;
	}

	if (pwa->m_BodyBuffer)
	{
		FREE(pwa->m_BodyBuffer);
		pwa->m_BodyBuffer = NULL;
	}
	pwa->m_BodySize = 0;
	pwa->m_BodyAllocSize = 0;


	/*
	CALLBACK_Cancel(&pwa->cb);
	
	// then clean up, if necessary
	if ((char *)0 != pwa->pszPostData) {
		*pwa->pszPostData = 1; // write delimiter back in, if any
		pwa->pszPostData = 0;
	}
	if (pwa->piWResp)
	{
		IWEBRESP_Release(pwa->piWResp);
		pwa->piWResp= NULL;
	}
	
	if (pwa->m_pIWeb) 
	{
		IWEB_Release(pwa->m_pIWeb);
		pwa->m_pIWeb = NULL;
	}
	
	if (pwa->m_BodyBuffer)
	{
		FREE(pwa->m_BodyBuffer);
		pwa->m_BodyBuffer = NULL;
	}
	pwa->m_BodySize = 0;
	pwa->m_BodyAllocSize = 0;
	*/
	
}


/*===========================================================================

  FUNCTION: myiweb_Stop
  
	DESCRIPTION:
	Halts a web transaction, wrapped/represented by a myiweb
	
	  PARAMETERS:
	  pwa: the myiweb
	  
		DEPENDENCIES:
		None
		
		  RETURN VALUE:
		  None
		  
			SIDE EFFECTS:
			None
===========================================================================*/
static void myiweb_Stop_s(newgimis *pwa)
{
	// this cancels any pending web transaction, or readable on the 
	// response body.  if nothing is pending, this has no effect
	   CALLBACK_Cancel(&pwa->cb);
   
   // then clean up, if necessary
   if ((char *)0 != pwa->pszPostData) {

	   FREE(pwa->pszPostData);
		pwa->pszPostData = NULL;
//		pwa->iFileRead = 0;

//      *pwa->pszPostData_xt = 1; // write delimiter back in, if any
 //     pwa->pszPostData_xt = 0;
   }
 // ����  
   if (pwa->pipPostData != NULL)
	{
		IPEEK_Release(pwa->pipPostData);
		// Add by zjie 2011-01-30
		pwa->pipPostData = NULL;

	}
 // ����  
	if (pwa->pSource != NULL) {
		uint32 nErr = ISOURCE_Release(pwa->pSource);	// 2013-04-12 �� nErr
		pwa->pSource = NULL;
	}

   
	if (pwa->piWResp)
	{
		IWEBRESP_Release(pwa->piWResp);
		pwa->piWResp= NULL;
	}

	if (pwa->m_pIWeb) 
	{
//		IWEB_Release(pwa->m_pIWeb_xt);
//		pwa->m_pIWeb_xt = NULL;



		int i,n;			// 2.6.4D2
		n = 0;
		i=IWEB_Release(pwa->m_pIWeb);
		while (i>0) {
			n++;
			i=IWEB_Release(pwa->m_pIWeb);
		}

		pwa->m_pIWeb = NULL;
	}

	if (pwa->m_BodyBuffer)
	{
		FREE(pwa->m_BodyBuffer);
		pwa->m_BodyBuffer = NULL;
	}
	pwa->m_BodySize = 0;
	pwa->m_BodyAllocSize = 0;

	/*
	CALLBACK_Cancel(&pwa->cb);
	
	// then clean up, if necessary
	if ((char *)0 != pwa->pszPostData) {
		*pwa->pszPostData = 1; // write delimiter back in, if any
		pwa->pszPostData = 0;
	}
	if (pwa->piWResp)
	{
		IWEBRESP_Release(pwa->piWResp);
		pwa->piWResp= NULL;
	}
	
	if (pwa->m_pIWeb_s) 
	{
		IWEB_Release(pwa->m_pIWeb_s);
		pwa->m_pIWeb_s = NULL;
	}
	
	if (pwa->m_BodyBuffer)
	{
		FREE(pwa->m_BodyBuffer);
		pwa->m_BodyBuffer = NULL;
	}
	pwa->m_BodySize = 0;
	pwa->m_BodyAllocSize = 0;
	*/
	
}



static void myiwebhtml_NotifyCB( void* pvUser, HViewNotify* pNotify )
{
	newgimis* pMe = (newgimis*) pvUser;   
	
	switch( pNotify->code )
	{
		
	case HVN_REDRAW_SCREEN:
		IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
		IHTMLVIEWER_Redraw(pMe->m_pHTMLViewer);  // �˺�������ˢ��ȫ�� HTML ������ؼ�
		break;
		
	case HVN_JUMP:
	case HVN_SUBMIT:
		STRCPY(pMe->targetUrl,TEST_HOST);
		pMe->m_pszFileName = (char*)MALLOC(16);
		FIND_STRING(pNotify->u.jump.pszURL, "PIC")
		{
			STRCAT(pMe->targetUrl,pNotify->u.jump.pszURL+4);
			STRCPY(pMe->m_pszFileName,pNotify->u.jump.pszURL+4);
			pMe->m_iMediaType = 1;
		}
		FIND_STRING(pNotify->u.jump.pszURL, "MUSIC")
		{
			STRCAT(pMe->targetUrl,pNotify->u.jump.pszURL+6);
			STRCPY(pMe->m_pszFileName,pNotify->u.jump.pszURL+6);
			pMe->m_iMediaType = 2;
		}
		//  IHTMLVIEWER_SetNotifyFn(pMe->m_pHTMLViewer, NULL, pMe);
		myiweb_Stop(pMe);
		myiweb_Start(pMe,pMe->targetUrl,1);
		
		break;
		
	case HVN_DONE:
		IHTMLVIEWER_SetRect(pMe->m_pHTMLViewer, &pMe->m_rc);  // �˺������ÿؼ��������Ļ���� �������ܻᵼ�¿ؼ�ˢ��������
		IHTMLVIEWER_Redraw( pMe->m_pHTMLViewer );
		break;
		
	}
}


///////////////  ������HTTP�йص� ���� /////////////////////






/*===========================================================================
                   M A I N   M E N U
===========================================================================*/
boolean testgps_MainMenu_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam )
{

	// \u53ef\u89c6\u5316\u8c03\u5ea6\uff1a\u56\u33\u2e\u30\u2e\u30\u2e\u31
   AECHAR szTitle[] = {0x53ef,0x89c6,0x5316,0x8c03,0x5ea6,0xff1a,0x0000}; // ���ӻ����ȣ�V3.0.0.1
   AECHAR szGPS[] = {0x47,0x50,0x53,0x3a,0x0000}; // GPS:  \u47\u50\u53\u3a
   AECHAR szOpen[] = {0x5f00,0x0000}; // ��  \u5f00\u5173
   AECHAR szClose[] = {0x5173,0x0000}; // ��
   AECHAR szPost[] = {0x5b9a, 0x4f4d  , 0x6a21 ,0x5f0f ,0x0000}; // ��λģʽ  \u5b9a\u4f4d\u6a21\u5f0f

   AECHAR strBD[] = {0x5317,0x6597,0x5b9a,0x4f4d,0x0000};  // ������λ \u5317\u6597\u5b9a\u4f4d
   AECHAR strWX[] = {0x7eaf,0x536b,0x661f,0x0000};  // ������
   AECHAR strJZ[] = {0x7eaf,0x57fa,0x7ad9,0x0000};  // ����վ
   AECHAR strWL[] = {0x7eaf,0x7f51,0x7edc,0x0000};  // ������
   AECHAR strZX[] = {0x624b,0x673a,0x81ea,0x9009,0x0000};  // �ֻ���ѡ  \u624b\u673a\u81ea\u9009
   AECHAR strSX[] = {0x9996,0x5148,0x0000};  // ����  \u9996\u5148
   AECHAR strJZFZ[] = {0x57fa,0x7ad9,0x8f85,0x52a9, 0x0000};  // ��վ����  \u8f85\u52a9
   AECHAR strPZCW[] = {0x914d,0x7f6e,0x9519,0x8bef,0xff08,0x975e,0x5317,0x6597,0xff09,0x0000};//���ô��󣨷Ǳ�����\u914d\u7f6e\u9519\u8bef\uff08\u975e\u5317\u6597\uff09

   AECHAR szTXZC[] = {0x901a,0x8baf,0x6b63,0x5e38,0x0000};  // ͨѶ���� \u901a\u8baf\u6b63\u5e38
   AECHAR szTXYC[] = {0x901a,0x8baf,0x5f02,0x5e38,0x0000};  // ͨѶ�쳣 \u901a\u8baf\u5f02\u5e38
   AECHAR szZDQF[] = {0x7ec8,0x7aef,0x6b20,0x8d39, 0x0000};  // �ն�Ƿ��  \u7ec8\u7aef\u6b20\u8d39
   AECHAR szDWYC[] = {0x5b9a,0x4f4d,0x5f02,0x5e38, 0x0000};  // ��λ�쳣 \u5b9a\u4f4d\u5f02\u5e38

   AECHAR szDWMS[] = {0x6a21,0x5f0f,0xff1a, 0x0000};  // ��λģʽ  \u5b9a\u4f4d\u6a21\u5f0f\uff1a
   AECHAR szJWD[] = {0x7ecf,0x7eac,0x5ea6,0xff1a, 0x0000};  // ��γ�� \u7ecf\u7eac\u5ea6\uff1a

   AECHAR szREPH[] = {0x8bf7,0x91cd,0x542f,0x624b,0x673a,0x0000} ;// �������ֻ�  \u8bf7\u91cd\u542f\u624b\u673a

   AECHAR szLJZC[] = {0x54,0x43,0x50,0x3a,0x6b63,0x5e38,0x0000};	//TCP:���� \u54\u43\u50\u3a\u6b63\u5e38
   AECHAR szLJYC[] = {0x54,0x43,0x50,0x3a,0x5f02,0x5e38,0x0000};	//TCP:�쳣 \u54\u43\u50\u3a\u5f02\u5e38


   AEERect rRect;
   boolean bHandled = FALSE;
   IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );

   switch( eCode ) {
   case EVT_SCREEN:
      if( wParam == SCREEN_PARAM_INIT ) {
         if( pMenuCtl ) {  /* This is not expected to happen */
            testgps_DrawScreen( pMe, 0 );
            bHandled = TRUE;
         }
         else {
            if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_MENUCTL, 
               (void **)&pMenuCtl ) == SUCCESS ) {

			   /////////  7-31 ��ʱ��ӵ�  ///////
 				if (pMe->cPicMode == '4')
 				{
					//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"###########����");
					IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_FORWARD_DATA,
                  MAINMENU_ITEM_YJDK, NULL, (uint32)pMenuCtl );// ����ת�� --һ����
				}
 			   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_YJKQ,
                   MAINMENU_ITEM_YJKQ, NULL, (uint32)pMenuCtl );
			   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_SEND_PHOTO,
                  MAINMENU_ITEM_PHOTO, NULL, (uint32)pMenuCtl );
			   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_SEND_AUDIO,
                  MAINMENU_ITEM_AUDIO, NULL, (uint32)pMenuCtl );
				
			   //�����ռ�����Ŀ
//			   if (pMe->m_LongSMS_uReadNum > 0){
//					MP_AddMenuItem(pMenuCtl,	IDS_LONGSMS, NULL, IDB_HAVE_TASK, MAINMENU_ITEM_LONGSMS, (uint32)pMenuCtl);
//			   }else{
//					IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_LONGSMS,MAINMENU_ITEM_LONGSMS, NULL, (uint32)pMenuCtl );
//			   }


			   //IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_MORE, MAINMENU_ITEM_MORE, NULL, (uint32)pMenuCtl );
//////////////////////////////////////////////////////////////////////////
			   //#��������ͼ��#		����δ������񣬲˵�������ʾͼ��  2016-1-21
			   //IDS_MORE_HAVE_TASK "  ����"
			   //IDS_MORE			"����"				Ϊ����ʾͼ���ʱ���ܶ��� .bar������2��"����"
			   if (pMe->pTaskApp)
			   {
				   cTaskApp *me = pMe->pTaskApp;
				   if(me->p_head) 
				   {					 
					   MP_AddMenuItem(pMenuCtl,	IDS_MORE_HAVE_TASK, NULL, IDB_HAVE_TASK, MAINMENU_ITEM_MORE, (uint32)pMenuCtl);
				   }else{
					   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_MORE, MAINMENU_ITEM_MORE, NULL, (uint32)pMenuCtl );//"����"��ť
				   }
			   }
			   else
			   {
				   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_MORE, MAINMENU_ITEM_MORE, NULL, (uint32)pMenuCtl );//"����"��ť	
			   }
//////////////////////end//////////////////////////////////////////////


//			   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_OPENGPS,
//                  MAINMENU_ITEM_GPS, NULL, (uint32)pMenuCtl );
			   /////////   end ////
		    	SETAEERECT(&rRect, 0, pMe->m_SYSFBH * 5 + 5, pMe->DeviceInfo.cxScreen, pMe->DeviceInfo.cyScreen - (pMe->m_SYSFBH * 5) - 5);
          //     IMENUCTL_SetTitle( pMenuCtl, NEWGIMIS_RES_FILE, IDS_GIMISTITLE, NULL );

			   IMENUCTL_SetRect(pMenuCtl,&rRect);
               testgps_SetScreenData( pMe, (void *)pMenuCtl );
               testgps_DrawScreen( pMe, 0 );
               bHandled = TRUE;
            }
         }
      }
      else if( wParam == SCREEN_PARAM_CLOSE ) {
         IMENUCTL_Release( pMenuCtl );
         testgps_SetScreenData( pMe, 0 );
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
      else if( wParam == SCREEN_PARAM_PAINT ) {
		  int si_width = 0;
		  int si_width1 = 0;
		  IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
		  testgps_Printf_hanzi( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL,szTitle );
		 si_width = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szTitle,-1,-1,NULL);
		 testgps_Printf( pMe, 0, si_width + 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, pMe->sVer );
//		 testgps_Printf( pMe, 1, 0, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "code:" );

		 testgps_Printf_hanzi( pMe, 2, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, szGPS );
		 si_width = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD, szGPS ,-1,-1,NULL);
		 
		 if (pMe->GPSFlag)
		 {
			testgps_Printf_hanzi( pMe, 2, si_width + 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, szOpen );
			// ��Ҫ��ʾ����ϱ���ʱ��

			if (STRLEN(pMe->show_Time) > 0 && pMe->m_Error == 200) 
			 {
				si_width1 = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD, szOpen ,-1,-1,NULL);
				 testgps_Printf( pMe, 2, si_width+si_width1+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, pMe->show_Time );
			 
				 testgps_Printf( pMe, 4, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, pMe->show_LatLon );
				 
			 }

			// ��ʾģʽ
			testgps_Printf_hanzi( pMe, 3, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL,szDWMS );
			si_width = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD, szDWMS ,-1,-1,NULL);


			 if( pMe->m_gpsFlag  == 0 ) {  // ˵��������
			 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL,strWL );
			 }
			 else if(  pMe->m_gpsFlag == 1 ) {
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strZX );
			 }
			 else if( pMe->m_gpsFlag == 2 ) {
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strJZFZ );
			 }
			 else if(  pMe->m_gpsFlag == 3 ) {
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strJZ );
			 }
			 else if( pMe->m_gpsFlag == 4 ) {
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strSX );
			 }
			 else if( pMe->m_gpsFlag == 5 ) {
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strWX );
			 }
			 else if( pMe->m_gpsFlag == 10 ) {
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strBD );
			 }
			 else if (pMe->m_gpsFlag == 11 )
			 {
				 //��ʾ���Ǳ����ֻ�
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strPZCW );
			 }
		 }
		 else
		 {
			 testgps_Printf_hanzi( pMe, 2, si_width + 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, szClose );
			 testgps_Printf( pMe, 3, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT | IDF_RECT_FILL, "" ); // ģʽ
			 testgps_Printf( pMe, 4, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT | IDF_RECT_FILL, "" ); // ��γ��
		 }

		 //2018 0105		//ƽ��ҽԺtcp������ʾ
		 if(pMe->iUDPPORT_YYXF != 0  && pMe->sUDPIP_YYXF !=NULL && pMe->iTCPPORT_YYXF != 0  && pMe->sTCPIP_YYXF !=NULL)
		 {
			 //TCP:״̬
			if (pMe->TCP_Status_yyxf)
			{
				//��������
				testgps_Printf_hanzi( pMe, 4, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, szLJZC ); //
			}
			else
			{
				//�����쳣
				testgps_Printf_hanzi( pMe, 4, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, szLJYC ); // 
			}
		 }

		 

         if( IMENUCTL_IsActive( pMenuCtl ) ) {
            IMENUCTL_Redraw( pMenuCtl );
			
         }
		 else
		 {
            IMENUCTL_SetActive( pMenuCtl, TRUE );  /* Also causes a menu draw */			 
		 }
      }
	 else if (wParam == XTRA_EVT_USER_PRINT)
	 {
		 int si_width = 0;
		 int si_width1 = 0;
		 char sc_tmp[30];
		 MEMSET(sc_tmp,0x00,30);
		 if (pMe->m_Error ==  1048580 || pMe->m_Error == 1048577)
		 {
			 SPRINTF(sc_tmp," code: %x",pMe->m_Error);
		 }
		 else
		 {
			SPRINTF(sc_tmp," code: %d",pMe->m_Error);	 
		 }
		 
		 testgps_Printf_hanzi( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL,szTitle );
		 si_width = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szTitle,-1,-1,NULL);
		 testgps_Printf( pMe, 0, si_width + 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, pMe->sVer );


		 testgps_Printf_hanzi( pMe, 2, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, szGPS );
		 si_width = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD, szGPS ,-1,-1,NULL);
		 if (pMe->GPSFlag)
		 {
			testgps_Printf_hanzi( pMe, 2, si_width + 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, szOpen );
			// ��Ҫ��ʾ����ϱ���ʱ��

			if (STRLEN(pMe->show_Time) > 0 && pMe->m_Error == 200) 
			 {
				si_width1 = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD, szOpen ,-1,-1,NULL);
				 testgps_Printf( pMe, 2, si_width+si_width1+3+5, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, pMe->show_Time );
			 
				 testgps_Printf( pMe, 4, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, pMe->show_LatLon );
				 
			 }

			// ��ʾģʽ
			testgps_Printf_hanzi( pMe, 3, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL,szDWMS );
			si_width = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD, szDWMS ,-1,-1,NULL);

			 if( pMe->m_gpsFlag  == 0 ) {  // ˵��������
			 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL,strWL );
			 }
			 else if(  pMe->m_gpsFlag == 1 ) {
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strZX );
			 }
			 else if( pMe->m_gpsFlag == 2 ) {
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strJZFZ );
			 }
			 else if(  pMe->m_gpsFlag == 3 ) {
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strJZ );
			 }
			 else if( pMe->m_gpsFlag == 4 ) {
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strSX );
			 }
			 else if( pMe->m_gpsFlag == 5 ) {
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strWX );
			 }
			 else if( pMe->m_gpsFlag == 10 ) {
				 testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strBD );
			 }
			 else if (pMe->m_gpsFlag == 11 )
			 {
				 //��ʾ���Ǳ����ֻ�
				testgps_Printf_hanzi( pMe, 3, si_width+3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, strPZCW );
			 }

		 }
		 else
		 {
			 testgps_Printf_hanzi( pMe, 2, si_width + 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, szClose );
			 testgps_Printf( pMe, 3, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT | IDF_RECT_FILL, "" ); // ģʽ
			 testgps_Printf( pMe, 4, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT | IDF_RECT_FILL, "" ); // ��γ��
		 }

		 //2018 0105		//ƽ��ҽԺtcp������ʾ
		 if(pMe->iUDPPORT_YYXF != 0  && pMe->sUDPIP_YYXF !=NULL && pMe->iTCPPORT_YYXF != 0  && pMe->sTCPIP_YYXF !=NULL)
		 {
			 //TCP:״̬
			 if (pMe->TCP_Status_yyxf)
			 {
				 //��������
				 testgps_Printf_hanzi( pMe, 4, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, szLJZC ); //
			 }
			 else
			 {
				 //�����쳣
				 testgps_Printf_hanzi( pMe, 4, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, szLJYC ); // 
			 }
		 }




		 if (pMe->m_Error == 554) //
		 {
			 testgps_Printf_hanzi(pMe,1,3,AEE_FONT_BOLD,IDF_ALIGN_LEFT | IDF_RECT_FILL,szZDQF);
			 si_width = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szZDQF,-1,-1,NULL);
			 testgps_Printf( pMe, 1, si_width + 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, sc_tmp );
		 }
		 else if (pMe->m_Error == 200) //
		 {
			 testgps_Printf_hanzi(pMe,1,3,AEE_FONT_BOLD,IDF_ALIGN_LEFT | IDF_RECT_FILL,szTXZC);
			 si_width = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szTXZC,-1,-1,NULL);
			 testgps_Printf( pMe, 1, si_width + 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, sc_tmp );
		 }
		 else if (pMe->m_Error == 0x100001) //
		 {
			 testgps_Printf_hanzi(pMe,1,3,AEE_FONT_BOLD,IDF_ALIGN_LEFT | IDF_RECT_FILL,szDWYC);
			 si_width = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szDWYC,-1,-1,NULL);
			 testgps_Printf( pMe, 1, si_width + 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, sc_tmp );
		 }
		 else if (pMe->m_Error == 0x100004) // 1048580
		 {
			 testgps_Printf_hanzi(pMe,1,3,AEE_FONT_BOLD,IDF_ALIGN_LEFT | IDF_RECT_FILL,szDWYC);
			 si_width = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szDWYC,-1,-1,NULL);
			 testgps_Printf( pMe, 1, si_width + 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, sc_tmp );
		 }
		 /////////////////////////////////////////////////
		 else if (pMe->m_Error == WEB_ERROR_CONNECT) // 1289
		 {
			 if (STRCMP(pMe->sMobileType,"03") != 0)
			 {
				 testgps_Printf_hanzi(pMe,4,3,AEE_FONT_BOLD,IDF_ALIGN_LEFT | IDF_RECT_FILL,szREPH); // �������ֻ�
			 }
			 
		 }
		 /////////////////////////////////////////////////
		 else
		 {
			testgps_Printf_hanzi(pMe,1,3,AEE_FONT_BOLD,IDF_ALIGN_LEFT | IDF_RECT_FILL,szTXYC);
			 si_width = IDISPLAY_MeasureTextEx(pMe->a.m_pIDisplay,AEE_FONT_BOLD,szTXYC,-1,-1,NULL);
			 testgps_Printf( pMe, 1, si_width + 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT | IDF_RECT_FILL, sc_tmp );
			 
		 }
		 
		 





	 }
      break;

   case EVT_KEY:
	   bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "���� EVT_KEY_RELEASE 1");
	   // AVK_POWER   AVK_END
	   if( wParam == AVK_END  || wParam == AVK_CLR ) {
		   if (pMenuCtl)
		   {
			   IMENUCTL_Release( pMenuCtl );
			   pMenuCtl = NULL;
		   }
		   
           pMe->currentHandler = 0;
		   testgps_SetScreenData( pMe, 0 );

		   //	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
		   pMe->m_RelyExit = FALSE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
		   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
		   pMe->m_FOBFlag = TRUE; //�ں�̨����
		   pMe->m_WitchPag = 0;
//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"��̨���� 6:%d",pMe->m_WitchPag);
		   bHandled = TRUE;

			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_------------------------CESHI1");
	   }


	   	if ( (wParam == AVK_1) || (wParam == AVK_POUND) )
		{
			if (pMe->clsKey[0] == 0)
				pMe->clsKey[0] = wParam;
			else if (pMe->clsKey[1] == 0)
				pMe->clsKey[1] = wParam;
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�ɼ���ť����");
			if (newgimis_ClosseApplet(pMe))	// �ӳ�1�룬��ֻ֤�յ�2����
				return TRUE;
		}
		else 
		{
		   pMe->clsKey[0] = 0;		// ��֤�����յ���ϼ�
		   pMe->clsKey[1] = 0;		// ��֤�����յ���ϼ�
		   bHandled = TRUE;
		}  
	   break;
	   /*
   case EVT_KEY_PRESS:
   case EVT_KEY_RELEASE:
		  bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );
		  if( wParam == AVK_END  || wParam == AVK_CLR ) {
			  ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
			  pMe->m_FOBFlag = TRUE; //�ں�̨����
			  pMe->m_WitchPag = 0;
			  bHandled = TRUE;
	   }
      break;
	  */

   case EVT_COMMAND:
      if( dwParam == (uint32)pMenuCtl )
	  {
         switch( wParam ) 
		 {

 /* 
			case MAINMENU_ITEM_GPS:
			{
				if (pMe->cGpsMode == '0')
				{
					return TRUE;
				}
				 pMe->m_WitchPag = 5;
//				 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"����GPS��ʾ����:%d",pMe->m_WitchPag);
				 
				testgps_GotoScreen( pMe, SCREENID_SHOWGETGPSINFO, wParam );	

				
			 }
            break;

*/

/*
		case MAINMENU_ITEM_EMPUT:
				{
					newgimis_SendEmpty(pMe, TRUE );
				}
			break;
		case MAINMENU_ITEM_FULL:
				{
					newgimis_SendFull(pMe, TRUE );
				}
			break;
*/
		 case MAINMENU_ITEM_YJDK:
			 {
				 if (pMe->cPicMode == '4') // һ����-����ת��
				 {
					 pMe->m_WitchPag = 25;	//����
					 newgimis_forYJDK(pMe, TRUE );
				 }
			 }
			 return TRUE;

		 case MAINMENU_ITEM_YJKQ:  // ���Ϳ�����Ϣ
			 {
 				 pMe->m_WitchPag = 1;
 				 newgimis_forYJKQ(pMe,TRUE);

				 //Scoket_init_TCP(pMe);
				 //initModule_yyxf(pMe);
				 //SoundPlay_YYXF(pMe);
			 }
			
			return TRUE;
		case MAINMENU_ITEM_PHOTO:  // ������Ƭ
				{
					pMe->m_WitchPag = 2;
					newgimis_forphoto(pMe,TRUE);
			//		uf_forphoto(pMe,TRUE);
				}
				return TRUE;
         case MAINMENU_ITEM_AUDIO: //  ������Ƶ
			 {
				 pMe->m_WitchPag = 3;
				newgimis_forAudio(pMe,TRUE);
			 }
			 break;
		 case MAINMENU_ITEM_LONGSMS:
			 {
				 pMe->m_WitchPag = 11;
				 testgps_GotoScreen( pMe, SCREENID_LONGSMS, wParam );	
			 }
			 break;
		 case MAINMENU_ITEM_MORE: //  ����
			 {  // SCREENID_MORE
				 pMe->m_WitchPag = 4;
				 
				testgps_GotoScreen( pMe, SCREENID_MORE, wParam );	
			//	newgimis_forAudio(pMe,TRUE);
			 }
			 break;
         default:
            break;
         }
      }
      break;
   }
   return bHandled;
}


// ����������Ȩ�ķ��ص�ֵ��д�������ļ�
// ����ֵ˵�� ������� 1 - ��ɹ�����������Ȩ - ����Ҫ����д����  0 - �򷵻ص��Ǵ�����Ϣ - ����ʧ��  2 - ��û��Ҫ�ĵģ�����Ҫ����д����  
// 3 --�����ڿ����ϵ� ������ʧ�� ��Ӧ�����ַ� ��2��      4 -- �������ϴ����ɹ�  5 -- ����Ȩ��δ��Ȩ ����ر�
int ParseStringandWriteToConfigure_xintiao_GPS( newgimis *pMe,char *pString)
{
	int si_ret = 0;
	int si_Start = 0;
	int si_End = 0;
	int si_Count = 0;
	char *sc_Start = NULL;
	char *sc_End = NULL;
	char *sc_tmp = NULL;
	char *sc_tmp1 = NULL;
	char *first_tmp = NULL; 

	char sc_one;

	char *sc_tmpz = NULL; //��ʱ�洢��һ���ֶε�ֵ
	char scc_tmp1;
	char scc_tmp2;
	char scc_tmp3;  //����QA��
	char scc_tmp4;	// ����QA��

	char pszDelimiter[DEFAULT_SPLIT_LEN]  = DEFAULT_FIELD_SPLIT;

	MEMCPY(&sc_one,pString,1);


	if (sc_one == '1')
	{
		//��˵�����ճɹ���ȷ��������Ϣ

		sc_Start = STRSTR(pString,"1decAU0GS0");
		if(sc_Start != NULL)
		{
			si_ret = 5;
			pMe->cStartMode = '0';
			return si_ret;
		}

		sc_Start = STRSTR(pString,"1decAU0");
		if(sc_Start != NULL)
		{
			si_ret = 5;
			pMe->cStartMode = '0';
			return si_ret;
		}
		
		if (!STRCMP(pString,"1")) //��ͬ���� 0
		{
			si_ret = 2;
			return si_ret;
		}
		si_ret = 1;

		// ���ҵ�һ��
		sc_Start = STRSTR(pString,"1dec");
//		uf_RecLog(pMe,"������ 222A");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start,";");
//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," sc_End = %p",sc_End); 
//			uf_RecLog(pMe,"������ 221A");
			si_Count = sc_End - sc_Start - 4;
//			uf_RecLog(pMe,"������ 221B");
			FREEIF(sc_tmp);
			sc_tmp = (char *)MALLOC(si_Count + 5);
//			uf_RecLog(pMe,"������ 221C");
//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," sc_tmp = %p",sc_tmp); 
//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," si_Count + 5 = %d",si_Count + 5); 
			MEMSET(sc_tmp,0x00,si_Count + 5);
//			uf_RecLog(pMe,"������ 221D");
			STRNCPY(sc_tmp,sc_Start+4,si_Count);
//			uf_RecLog(pMe,"������ 221F");
			FREE(sc_tmp);
			sc_tmp = NULL;

			sc_tmpz = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmpz,0x00,si_Count + 5);

			STRNCPY(sc_tmpz,sc_Start+4,si_Count);
			uf_RecLog(pMe, sc_tmpz );
			MEMCPY(&scc_tmp1,sc_tmpz,1);
			MEMCPY(&scc_tmp2,sc_tmpz+1,1);
//			uf_RecLog(pMe,"������ 221Q");

			

			// A2-----����߶�����------iFontHeight				A3----����������-------iFontWidth
			// A4-----��Ƶ�ļ�ʱ������------iAddHour			AU------��Ȩ------cAuthorize
			// AA-----ʹ�ü�Ȩ��λ-----cCTAuth					AD-----�Ƿ�����ʹ�� ��1#�� ���˳�-----cCanExit
			// AY-----����GPS���� ----

			// CP---�ϱ�GPS���������ĵ�ַ----strCenterIp

			// GE ----- ��λ����------cAccType					GW------���Ź�------cWatchDog
			// GS-----�Ƿ񿪻�������-----cStartMode				GT-----������λ����-----iPosDetAtime	
			// GD------��λ��ʽ------cPosDetMode				GM-  --------ʹ�ø�����λʱ�Ķ�λ����-------iAssInterval
			// GC------����GPS����  0Ϊ�ر�gps ���˳�����¼�����ļ�����1Ϊ����gps����¼������  2-Ϊ�˳�����

			// HT---������������ ---- ipulsation                HG --- �ɼ����ڣ����ϱ�GPS��ʱ���� ---- iInterval
					
			// 	LG------�Ƿ���д��־-----cWrtLog
		
			// PP------����(������Ƭ)����-----cUsePhoto			PW------�ֻ�����Ƭ����ʾ�ߴ�(���)------iPhotoWidth
			// PD----������Ϣ��������------nKeepDays			PH-----�ֻ�����Ƭ����ʾ�ߴ�(�߶ȶ�)------iPhotoHeight 
			// PC-----������ʱʹ�õ�ͼƬ����ţ� 0��    �á��ճ��������س���  cPicMode         
			// PL-----sPhotoPath ��Ƭ·��  sVideoPath  ��Ƶ��ŵ�·��	
			// QC ---- ��ӦͨѶ¼ QCA --- ���ͨѶ¼  QCD -- ɾ��ĳ��ͨѶ¼  QCDA -- ɾ��ȫ��ͨѶ¼
			
			// ST----�����ֻ��ͺ�------sMobileType	
			first_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(first_tmp,0x00,si_Count + 5);
			STRCPY(first_tmp,sc_tmpz+2);

//			uf_RecLog(pMe,"������ 221W");
			switch(scc_tmp1)  
			{
				case 'A':
					{
						switch(scc_tmp2)
						{
						case '1': // A1
							break;
						case '2': // A2
							break;
						case '3': // A3
							break;
						case '4': // A4
							break;
						case 'A': // AA
							break;
						case 'D': // AD
							STRCPY(&(pMe->cCanExit),first_tmp);
							break;
						case 'U': // AU
							break;
						case 'Y': // AY ����GPS����
							{ 

								pMe->nArySize = ATOI(first_tmp);
							}
							break;
						}
					}
					break;
				case 'C':
					{		
						switch(scc_tmp2)
						{
						case 'P': //CP
							STRCPY(pMe->strCenterIp,first_tmp);
							break;
						}
					}
				break;

				case 'G':
					{		
						switch(scc_tmp2)
						{
						case 'E': // GE
							STRCPY(&(pMe->cAccType),first_tmp);
							pMe->m_Acc = pMe->cAccType - '0'; //�ȼ�����   ��λ���� '1'��'2'��'3'��'4'��'5'��'6',ȱʡΪ'3'
							break;
						case 'W': // GW
							STRCPY(&(pMe->cWatchDog),first_tmp);
							break;
						case 'S':// GS
							STRCPY(&(pMe->cStartMode),first_tmp);
							break;
						case 'T':// GT
							pMe->iPosDetAtime = ATOI(first_tmp);
							break;
						case 'D':// GD

							//ZEROAT(pMe->m_cPosDetMode);
							MEMSET(pMe->m_cPosDetMode, 0, sizeof(pMe->m_cPosDetMode));
							STRCPY((pMe->m_cPosDetMode),first_tmp);
							pMe->cPosDetMode = ATOI(pMe->m_cPosDetMode);
							//����ģʽ�л�
							if (pMe->cPosDetMode == 13)
							{
								ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�л�������3");
								newgimis_CloseGPS( pMe );
								newgimis_Init_UKBD( pMe );
								newgimis_OpenGPS_UKBD(pMe);
							}
							else if (pMe->cPosDetMode == 14)
							{
								ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�л���S958GPS3");
								newgimis_CloseGPS_S958GPS(pMe);
								if (newgimis_Init_S958GPS( pMe ))
								{
									newgimis_OpenGPS_S958GPS(pMe);
								}
							}
							else
							{
								newgimis_CloseGPS_S958GPS(pMe);
								newgimis_CloseGPS_UKBD(pMe);
								newgimis_OpenGPS(pMe);
								pMe->m_NOMAINPOS = 0;
							}

							break;
						case 'M':// GM
							MEMSET(first_tmp,0x00,300);
							STRCPY(first_tmp,sc_tmpz+3);
							pMe->iAssInterval = ATOI(first_tmp);
							break;
						case 'C':// GC
							{
								STRCPY(&(pMe->cGpsMode),first_tmp);
								if ((pMe->cGpsMode == '1')&&(pMe->GPSFlag == FALSE)) // ˵������GPS
								{
									newgimis_OpenGPS(pMe);  // ֻ����GPS 
								}
								if ((pMe->cGpsMode == '0') && (pMe->GPSFlag == TRUE)) // ˵���ر�GPS
								{
									newgimis_CloseGPS(pMe);
								}
							}
							break;	
							
						}
					}
				break;

				case 'H':
					{		
						switch(scc_tmp2)
						{
						case 'T': // HT
							pMe->ipulsation = ATOI(first_tmp);
							break;
						case 'G': // HG
							{
								pMe->iInterval = ATOI(first_tmp);
//								uf_RecLog(pMe,"�ı䶨λ����");
								uf_RecLog(pMe,"change position cycle");

								if (pMe->cPosDetMode == 13)
								{
									ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�����ɼ����ڵ���3");
									newgimis_CloseGPS_UKBD( pMe );
									newgimis_Init_UKBD( pMe );
									newgimis_OpenGPS_UKBD(pMe);
								}
								else if (pMe->cPosDetMode == 14)
								{
									ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"S958GPS�ɼ����ڵ���3");
									newgimis_CloseGPS_S958GPS(pMe);
									if (newgimis_Init_S958GPS( pMe ))
									{
										newgimis_OpenGPS_S958GPS(pMe);
									}
								}
								else
								{
									ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY)uf_Pos_Track_stopstart_error, pMe);  // �л�ģʽ	�������Ǹı䶨λ����		
								}
													
							}
							break;
					
						}
					}
				break;

				case 'L':
					{		
						switch(scc_tmp2)
						{
						case 'G': // LG
							STRCPY(&(pMe->cWrtLog),first_tmp);
							break;
					
						}
					}
				break;

				case 'P':
					{		
						switch(scc_tmp2)
						{
						case 'P': // PP
							STRCPY(&(pMe->cUsePhoto),first_tmp);
							break;
						case 'W': // PW
							break;
						case 'D':// PD
							pMe->nKeepDays = ATOI(first_tmp);
							break;
						case 'H':// PH
							break;
						case 'C':// PC
							STRCPY(&(pMe->cPicMode),first_tmp);
							break;
						case 'L':// PL
							break;
							
						}
					}
				break;

				case 'Q': // ͨѶ¼�ô�
					{
//						uf_RecLog(pMe," ������ 222B ");
						if (scc_tmp2 == 'C')
						{
							crichAddr *me = NULL;
							MEMCPY(&scc_tmp3,sc_tmpz+2,1);
							MEMCPY(&scc_tmp4,sc_tmpz+3,1);
							// sc_tmpz
//							uf_RecLog(pMe," ������ 222C ");
							switch(scc_tmp3)
							{
								case 'A':// QCA // ���ͨѶ¼
									{
									//	if (pszTok) 
									//	{
									//		nResult = ud_DistToSplit( pszTok, DEFAULT_FIELD_SPLIT );
									//		if (nResult > 0)	
									//		{
//												uf_RecLog(pMe,"������ 3");
												addbook_init(pMe);

												me = pMe->pRichAddr;
												me->pstrRecU = (char *)MALLOC(si_Count+5);	
												MEMSET(me->pstrRecU,0x00,si_Count+5);
												MEMCPY(me->pstrRecU,sc_tmpz+3,si_Count+5-3);

												uf_RecLog(pMe,"add addbook content");
												uf_RecLog(pMe,me->pstrRecU);

												if (me->pstrRecD != NULL) 
												{
													int iLen = si_Count+5 + STRLEN(me->pstrRecD) + 1 ;
													me->pstrRecD = (char *)REALLOC(me->pstrRecD,(iLen+1));
													STRCAT(me->pstrRecD,DEFAULT_SPLIT);
													STRCAT(me->pstrRecD,me->pstrRecU);
												}
												

									//		}
									//	}
										Add_addbook(pMe);
										addbook_free(pMe);
									}
							
								break;	
								case 'D':// QCD // ɾ��ĳ��ͨѶ¼  ���� ɾ��ȫ��ͨѶ¼
									{
										if (scc_tmp4 == 'A') // ˵����ɾ��ȫ��ͨѶ¼
										{
											addbook_init(pMe);
											Del_all_addbook(pMe);
											addbook_free(pMe);
										}
										else  // ɾ��ĳ��ͨѶ¼
										{

											// ɾ��ĳ��ͨѶ¼
											addbook_init(pMe);

											me = pMe->pRichAddr;
											me->pstrRecD = (char *)MALLOC(si_Count+5);	
											MEMSET(me->pstrRecD,0x00,si_Count+5);
											MEMCPY(me->pstrRecD,sc_tmpz+3,si_Count+5-3);

											uf_RecLog(pMe,me->pstrRecD);										
											
											Del_addbook(pMe);
											addbook_free(pMe);																		
										}
									}
							
								break;
							}	
						}
						else if(scc_tmp2 == 'E')
						{
		
							FREEIF(pMe->sPttCallNo);
							pMe->sPttCallNo = (char *)MALLOC((si_Count + 5)*sizeof(char));
							MEMSET(pMe->sPttCallNo,0,(si_Count + 5)*sizeof(char));
							STRCPY(pMe->sPttCallNo,first_tmp);
							ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_SETPTTCALL_EVENT,(uint16)0, 0);

						}
						else if (scc_tmp2 == 'F')
						{
							pMe->nPttkey = STRTOUL( first_tmp, (char **)&pszDelimiter, 10 );
						}
						else if (scc_tmp2 == 'A')
						{
							// һ�����еĶԽ�����

							richptt_SetUsrGroup(pMe->pRichptt,first_tmp);

						   FREEIF(pMe->sUsrGroup);
						   pMe->sUsrGroup = (char *)MALLOC((si_Count + 5)*sizeof(char));
						   MEMSET(pMe->sUsrGroup,0,(si_Count + 5)*sizeof(char));
						   STRCPY(pMe->sUsrGroup,first_tmp);
											
							
						}
						else if (scc_tmp2 == 'B')
						{
							richptt_SetDomain(pMe->pRichptt,first_tmp);
						}
						else if (scc_tmp2 == 'G')
						{
							richPtt_SetCallRestInfo(pMe->pRichptt,first_tmp);
						}
						
					}
					break;
				case 'S':
					{		
						switch(scc_tmp2)
						{
						case 'T': // ST
							MEMSET(pMe->sMobileType,0x00,sizeof(pMe->sMobileType));
							STRCPY(pMe->sMobileType,first_tmp);

							break;
						case 'N': // SN
							{
								// ���������
								cTaskApp *me_t;
								pMe->m_Task_F++ ;// task_first ��������ǲ��ǳ����������һ�ν�����������ǵĻ���������ǰ������û��������ǰ��

								pMe->m_TaskC = ATOI(first_tmp);

								cTaskApp_HandleEvent(pMe,EVT_APP_START,20,0);	//���ռ�pTaskApp����ʼ������web�ӿ� 
								me_t = pMe->pTaskApp;	
								cTaskApp_TaskInteract(me_t);
								
						    	//#��̨ģʽ��������#	��̨����ʱ���и������񣬵�����ʾ�Ƿ�ʼ������ 2016-1-21
								uf_RecLog(pMe,"#��̨ģʽ �յ������� 2#");
			
								//ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "=== %d",me_t->cTimeout);
								
								

 								ISHELL_SetTimer(pMe->a.m_pIShell, TASK_SET_TIMER*1000, (PFNNOTIFY)Timer_Task_Notify,pMe);

								
								
							}
							break;
						case 'M'://SM
							{
								//uf_RecLog(pMe,"ParseStringandWriteToConfigure_xintiao_GPS get SM message");
								//  [2/4/2016 Administrator]
								// Add �Ƿ�Ҫ���ն���    	
								if (*first_tmp == '1')
								{  // ���ն���
									if ( Sms_HandleEvent(pMe,SMS_EVT_START,0,0) )
									{
										ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,SMS_EVT_DOWNLOAD,0,0);
									}
								}	
								// Add End
							}
							break;
						}
					}
				break;
				case 'T':
					{
						switch(scc_tmp2)
						{
						case 'B': // TB  ����ʱ��
							MEMSET(pMe->sMobileType,0x00,sizeof(pMe->sMobileType));
							pMe->iRestTb = ATOI(first_tmp);  // == 25 ˵������ʱ���ǹرյ�
							MEMSET(pMe->sRestTb,0x00,11);
							STRCPY(pMe->sRestTb,first_tmp);
	//						STRCPY(pMe->sMobileType,first_tmp);
							break;
					
						}
					}
					break;
				case 'U':
					{
						switch(scc_tmp2)
						{
						case 'L':
							STRNCPY(&(pMe->cUseLoad),first_tmp,1);
							break;
						}
					}
					break;
			}

			FREE(first_tmp);
			first_tmp = NULL;

			FREE(sc_tmpz);
			sc_tmpz = NULL;

			//  ������������ǲ���������ֵ��
			// ����д����ʡ��

			
		}
		
		// ������������   ipulsation  int ��
		sc_Start = STRSTR(pString,";HT");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->ipulsation = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// �ɼ����ڣ����ϱ�GPS��ʱ����   iInterval  int ��
		sc_Start = STRSTR(pString,";HG");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iInterval = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;

			if (pMe->cPosDetMode == 13)
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�����ɼ����ڵ���4");
				newgimis_CloseGPS_UKBD( pMe );
				newgimis_Init_UKBD( pMe );
				newgimis_OpenGPS_UKBD(pMe);
			}
			else if (pMe->cPosDetMode == 14)
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"S958GPS�ɼ����ڵ���4");
				newgimis_CloseGPS_S958GPS(pMe);
				if (newgimis_Init_S958GPS( pMe ))
				{
					newgimis_OpenGPS_S958GPS(pMe);
				}
			}
		}

		/////////////  ��Ӳ���������Ϣ   m_E_SC int
		

		sc_Start = STRSTR(pString,";CE");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->m_E_SC = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
			ud_RecLog(pMe->a.m_pIShell, 1 ,&(pMe->nInd)," continue error number m_E_SC = %d",pMe->m_E_SC); 
		}
		////////////////////////////////////////////////


		// �ϱ�GPS���������ĵ�ַ  strCenterIp   
		sc_Start = STRSTR(pString,";CP");
		if(sc_Start != NULL)
		{
			sc_Start = STRSTR(pString,";CP");
			if(sc_Start != NULL)
			{
				char * sc_tmp = NULL;
				sc_tmp = STRSTR(sc_Start,"http://");
				if(sc_tmp == NULL)
				{
					sc_tmp = STRSTR(sc_Start,"HTTP://");
					if (sc_tmp == NULL)
					{
						sc_End = STRSTR(sc_Start+2,";");
						si_Count = sc_End - sc_Start - 3;
						MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
						STRNCPY(pMe->strCenterIp,sc_Start+3,si_Count);
					}
					else
					{
						sc_End = STRSTR(sc_Start+2,";");
						si_Count = sc_End - sc_Start - 3 - 7;
						MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
						STRNCPY(pMe->strCenterIp,sc_Start+3 + 7,si_Count);
					}
				}
				else
				{
					sc_End = STRSTR(sc_Start+2,";");
					si_Count = sc_End - sc_Start - 3 - 7;
					MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
					STRNCPY(pMe->strCenterIp,sc_Start+3 + 7,si_Count);
				}
				sc_tmp = NULL;
			}
			//			sc_End = STRSTR(sc_Start+2,";");
			//			si_Count = sc_End - sc_Start - 3;
			//			MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
			//			STRNCPY(pMe->strCenterIp,sc_Start+3,si_Count);
		}

		// ��λ��ʽ  cPosDetMode  char    
		sc_Start = STRSTR(pString,";GD");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;

			//ZEROAT(pMe->m_cPosDetMode);
			MEMSET(pMe->m_cPosDetMode, 0, sizeof(pMe->m_cPosDetMode));
			STRNCPY((pMe->m_cPosDetMode),sc_Start+3,si_Count);

			pMe->cPosDetMode = ATOI(pMe->m_cPosDetMode);

			//����ģʽ�л�
			if (pMe->cPosDetMode == 13)
			{
				newgimis_CloseGPS( pMe );
				newgimis_Init_UKBD( pMe );
				newgimis_OpenGPS_UKBD(pMe);
			}
			else if (pMe->cPosDetMode == 14)
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�л���S958GPSģʽ");
				newgimis_CloseGPS_S958GPS(pMe);
				if (newgimis_Init_S958GPS( pMe ))
				{
					newgimis_OpenGPS_S958GPS(pMe);
				}
			}
			else
			{
				newgimis_CloseGPS_S958GPS(pMe);
				newgimis_CloseGPS_UKBD(pMe);
				newgimis_OpenGPS(pMe);
				pMe->m_NOMAINPOS = 0;
			}
		}

		// ��λ����  cAccType  char   
		sc_Start = STRSTR(pString,";GE");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cAccType),sc_Start+3,si_Count);
			pMe->m_Acc = pMe->cAccType - '0'; //�ȼ�����   ��λ���� '1'��'2'��'3'��'4'��'5'��'6',ȱʡΪ'3'
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," positionaI accuracy m_Acc = %d",pMe->m_Acc); 
		}

		// ���Ź�  cWatchDog  char     
		sc_Start = STRSTR(pString,";GW");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cWatchDog),sc_Start+3,si_Count);
		}

		// ���϶೤ʱ��(����)�������ֻ�  cWatchDog  char 

		////////////////////////////////////////////////////
		///////////////////////////
		//////////////////////// ���»�û�������� ����   
		sc_Start = STRSTR(pString,";TA");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
		//	STRNCPY(&(pMe->cWatchDog),sc_Start+3,si_Count);
		}

		
		// �Ƿ񿪻�������  cStartMode  char   
		sc_Start = STRSTR(pString,";GS");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cStartMode),sc_Start+3,si_Count);
		}

		// ��Ȩ  cAuthorize  char     
		sc_Start = STRSTR(pString,";AU");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cAuthorize),sc_Start+3,si_Count);

			if (pMe->cAuthorize == '0')	//ϵͳ�������ڿ϶���1dectel��ͷ���������AU0Ӧ��Ҫ�����Ƴ���si_ret=5���غ��˳���
			{
				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"xintiao_gps  AU = %c so exit ...",pMe->cAuthorize); 
				si_ret = 5;
				pMe->cStartMode = '0';
				return si_ret;
			}
	
		}

		// 0Ϊ�ر�gps���˳�����¼�����ļ�����1Ϊ����gps����¼�����ļ�����2�˳����򣨲���¼�����ļ���  cAuthorize  char 


		sc_Start = STRSTR(pString,";GC");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cGpsMode),sc_Start+3,si_Count);

			//20170317 ����
			if ((pMe->cGpsMode == '1')&&(pMe->GPSFlag == FALSE)) // ˵������GPS
			{
				newgimis_OpenGPS(pMe);  // ֻ����GPS 
			}
			if ((pMe->cGpsMode == '0') && (pMe->GPSFlag == TRUE)) // ˵���ر�GPS
			{
				newgimis_CloseGPS(pMe);
			}
		}



		////////////////////////////////////////////
		//////////////////////////////////
		//////////////////////   δ���

		// ÿ�η��ͼ������ݣ����8��  cAuthorize  char 
		sc_Start = STRSTR(pString,";AY");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;

			sc_tmp = (char *)MALLOC(10);
			MEMSET(sc_tmp,0x00,10);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->nArySize = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
			
	//		STRNCPY(&(pMe->cAuthorize),sc_Start+3,si_Count);
		}

		sc_Start = STRSTR(pString,";TB");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;

			sc_tmp = (char *)MALLOC(10);
			MEMSET(sc_tmp,0x00,10);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iRestTb = ATOI(sc_tmp);
			MEMSET(pMe->sRestTb,0x00,11);
			STRCPY(pMe->sRestTb,sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
			
		}

		
		// ����(����������״̬)����   0 : ������ , 1 ����  cUseLoad  char 
		sc_Start = STRSTR(pString,";UL");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cUseLoad),sc_Start+3,si_Count);
		}

		// ������λ������ȱʡΪ10  iPosDetAtime  int    
		sc_Start = STRSTR(pString,";GT");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iPosDetAtime = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}


		// �Ƿ���д��־�Ĺ��ܣ�  '0' ��д��־, '1' д��־   cWrtLog  char    
		sc_Start = STRSTR(pString,";LG");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cWrtLog),sc_Start+3,si_Count);
		}
		
		// ����߶����أ�ȱʡֵΪ16. ���磺 16   iFontHeight  int    
		sc_Start = STRSTR(pString,";A2");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iFontHeight = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// ���������أ�ȱʡֵΪ22�����磺 22   iFontWidth  int     
		sc_Start = STRSTR(pString,";A3");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iFontWidth = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}


		// ��Ƶ�ļ�ʱ������   iAddHour  int     
		sc_Start = STRSTR(pString,";A4");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iAddHour = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// �����ֻ��ͺţ� sMobileType     
		sc_Start = STRSTR(pString,";ST");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			MEMSET(pMe->sMobileType,0x00,sizeof(pMe->sMobileType));
			STRNCPY(pMe->sMobileType,sc_Start+3,si_Count);


		}

		// 
		//����(������Ƭ)����       0 : ������ , 1 ����  cUsePhoto    ������Ƭ����Ƶ���� 
		sc_Start = STRSTR(pString,";PP");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			STRNCPY(&(pMe->cUsePhoto),sc_Start+3,si_Count);
		}

		// 
		// �ֻ�����Ƭ����ʾ�ߴ�(���)  iPhotoWidth  int      
		sc_Start = STRSTR(pString,";PW");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iPhotoWidth = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}


		// �ֻ�����Ƭ����ʾ�ߴ�(�߶ȶ�)  iPhotoHeight  int   
		sc_Start = STRSTR(pString,";PH");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iPhotoHeight = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// 


		///////////////////////////////////////////////////////
		////////////////////////////////////////////
		////////////////////////   δ���
		// ��Ƭ�ļ�ʱ����������λ��Сʱ��   iAddHour  int 
		sc_Start = STRSTR(pString,";A1");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iAddHour = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}


		//'1'  ʹ�ü�Ȩ��λ����ʹ�õ��ŵĽӿڣ�, '0' : �����ü�Ȩ��λ (����ʹ�õ��ŵĽӿ�)   cCTAuth  char  
		sc_Start = STRSTR(pString,";AA");  
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			STRNCPY(&(pMe->cCTAuth),sc_Start+3,si_Count);
		}
		
		// 
		// ������Ϣ��������   nKeepDays  int    
		sc_Start = STRSTR(pString,";PD");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->nKeepDays = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// 
		// ʹ�ø�����λʱ�Ķ�λ����,��λ��    iAssInterval  int  
		sc_Start = STRSTR(pString,";GM-");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 4;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+4,si_Count);
			pMe->iAssInterval = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// ���������
		sc_Start = STRSTR(pString,";SN");
		if(sc_Start != NULL)
		{

			// ˵��������  m_TaskC
			cTaskApp *me_t;


			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(10);
			MEMSET(sc_tmp,0x00,10);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->m_TaskC = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;

			pMe->m_Task_F++ ;// task_first ��������ǲ��ǳ����������һ�ν�����������ǵĻ���������ǰ������û��������ǰ��

			cTaskApp_HandleEvent(pMe,EVT_APP_START,20,0);
			me_t = pMe->pTaskApp;	
			cTaskApp_TaskInteract(me_t);
			
			//#��̨ģʽ��������#	��̨����ʱ���и������񣬵�����ʾ�Ƿ�ʼ������ 2016-1-21
 			uf_RecLog(pMe,"#��̨ģʽ �յ������� 1 #");

			ISHELL_SetTimer(pMe->a.m_pIShell, TASK_SET_TIMER*1000, (PFNNOTIFY)Timer_Task_Notify,pMe);

		}

		//  [3/4/2016 Administrator]
		// һ�����еĶԽ�����
		sc_Start = STRSTR(pString,";IP");
		if(sc_Start != NULL)
		{
			
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
			MEMSET(pMe->m_IP,0x00,256);
			STRCPY(pMe->m_IP,sc_tmp);
			
			
			FREE(sc_tmp);
			sc_tmp = NULL;
			
			uf_RecLog(pMe,"IP ��ַ");
			uf_RecLog(pMe,pMe->m_IP);
		}

		// һ�����еĶԽ�����
		sc_Start = STRSTR(pString,";QA");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
	
			richptt_SetUsrGroup(pMe->pRichptt,sc_tmp);

		   FREEIF(pMe->sUsrGroup);
		   pMe->sUsrGroup = (char *)MALLOC((si_Count + 5)*sizeof(char));
		   MEMSET(pMe->sUsrGroup,0,(si_Count + 5)*sizeof(char));
		   STRCPY(pMe->sUsrGroup,sc_tmp);


			FREE(sc_tmp);
			sc_tmp = NULL;

			
			
		}

		// һ�����еĶԽ�����
		sc_Start = STRSTR(pString,";QE");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
	
			FREEIF(pMe->sPttCallNo);
		    pMe->sPttCallNo = (char *)MALLOC((si_Count + 5)*sizeof(char));
		    MEMSET(pMe->sPttCallNo,0,(si_Count + 5)*sizeof(char));
		    STRCPY(pMe->sPttCallNo,sc_tmp);
		    ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_SETPTTCALL_EVENT,(uint16)0, 0);


			FREE(sc_tmp);
			sc_tmp = NULL;

			
			
		}

		// �ڰ����� 
		sc_Start = STRSTR(pString,";QG");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
			richPtt_SetCallRestInfo(pMe->pRichptt,sc_tmp);

			FREE(sc_tmp);
			sc_tmp = NULL;

			
			
		}
		

		// Qchat���� 
		sc_Start = STRSTR(pString,";QB");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
			richptt_SetDomain(pMe->pRichptt,sc_tmp);

			FREE(sc_tmp);
			sc_tmp = NULL;

			
			
		}

		// pMe->nPttkey = STRTOUL( first_tmp, (char **)&pszDelimiter, 10 )

		// һ�����еİ���ֵ
		sc_Start = STRSTR(pString,";QF");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
			pMe->nPttkey = STRTOUL( sc_tmp, (char **)&pszDelimiter, 10 );

			FREE(sc_tmp);
			sc_tmp = NULL;		
		}


		// ͨѶ¼ͬ�� ɾ��
		sc_Start = STRSTR(pString,";QCA");
		if(sc_Start != NULL)
		{
			crichAddr *me;
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 4;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+4,si_Count);
			
			addbook_init(pMe);

			me = pMe->pRichAddr;
			me->pstrRecU = (char *)MALLOC(si_Count+5);	
			MEMSET(me->pstrRecU,0x00,si_Count+5);
			STRCPY(me->pstrRecU,sc_tmp);

			uf_RecLog(pMe,me->pstrRecU);

			if (me->pstrRecD != NULL) 
			{
				int iLen = si_Count+5 + STRLEN(me->pstrRecD) + 1 ;
				me->pstrRecD = (char *)REALLOC(me->pstrRecD,(iLen+1));
				STRCAT(me->pstrRecD,DEFAULT_SPLIT);
				STRCAT(me->pstrRecD,me->pstrRecU);
			}

			Add_addbook(pMe);
			addbook_free(pMe);

			FREE(sc_tmp1);
			FREE(sc_tmp);
			sc_tmp = NULL;
			sc_tmp1 = NULL;
			
			
		}

		// ͨѶ¼ͬ�� ɾ��
		sc_Start = STRSTR(pString,";QCD");
		if(sc_Start != NULL)
		{
			crichAddr *me;
			char *sc_StartE = NULL;
			sc_StartE = STRSTR(pString,";QCDA");
			if (sc_StartE != NULL)
			{
				// ˵��ɾ������ͨѶ¼ȫ��

				addbook_init(pMe);
				Del_all_addbook(pMe);
				addbook_free(pMe);
				sc_StartE = NULL;
			}
			else
			{
				// ɾ��ĳ��ͨѶ¼
			


				sc_End = STRSTR(sc_Start+2,";");
				si_Count = sc_End - sc_Start - 4;
				sc_tmp = (char *)MALLOC(si_Count + 5);
				MEMSET(sc_tmp,0x00,si_Count + 5);
				STRNCPY(sc_tmp,sc_Start+4,si_Count);
				
				addbook_init(pMe);
				me = pMe->pRichAddr;
				me->pstrRecD = (char *)MALLOC(si_Count+5);	
				MEMSET(me->pstrRecD,0x00,si_Count+5);
				STRCPY(me->pstrRecD,sc_tmp);

				uf_RecLog(pMe,me->pstrRecD);

				Del_addbook(pMe);
				addbook_free(pMe);
			}

			FREE(sc_tmp1);
			FREE(sc_tmp);
			sc_tmp = NULL;
			sc_tmp1 = NULL;
			
			
		}


		// Del_addbook(newgimis *pMe)

		//  
		//������ʱʹ�õ�ͼƬ����ţ� 0��    �á��ճ��������س���  cPicMode     
		sc_Start = STRSTR(pString,";PC");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			STRNCPY(&(pMe->cPicMode),sc_Start+3,si_Count);
		}


		//	QEZA=;
		//	QFZA=; ����������û�н���
		//	
		//�Ƿ�����ʹ�� ��1#�� ���˳�  0 �������� , 1 ����    cCanExit char  
		sc_Start = STRSTR(pString,";AD");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			STRNCPY(&(pMe->cCanExit),sc_Start+3,si_Count);
		}


		//  2015-10-29  start //
		// sPhotoPath ��Ƭ·��  sVideoPath  ��Ƶ��ŵ�·��   
		// �ֻ�����Ƭ�Ĵ洢Ŀ¼����Ƶ�ļ���Ŀ¼����ƬĿ¼����ƵĿ¼֮����","�ָ�
		sc_Start = STRSTR(pString,";PL");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			sc_Start = STRSTR(sc_tmp,",");
			if(sc_Start != NULL)
			{
				MEMSET(pMe->sPhotoPath,0x00,sizeof(pMe->sPhotoPath));
				STRNCPY(pMe->sPhotoPath,sc_tmp,sc_Start-sc_tmp);

				sc_tmp1 = (char *)MALLOC(100);
				MEMSET(sc_tmp1,0x00,100);
				STRNCPY(sc_tmp1,sc_Start+1,si_Count - (sc_Start -sc_tmp + 1));
				
				sc_Start = STRSTR(sc_tmp1,",");
				if (sc_Start != NULL)
				{
					MEMSET(pMe->sVideoPath,0x00,sizeof(pMe->sVideoPath));
					STRNCPY(pMe->sVideoPath,sc_tmp1,sc_Start-sc_tmp1);

					MEMSET(pMe->sAudRecPath,0x00,sizeof(pMe->sAudRecPath));
					STRCPY(pMe->sAudRecPath,sc_Start+1); //��֪�������᲻��������
				}
				else
				{
					MEMSET(pMe->sVideoPath,0x00,sizeof(pMe->sVideoPath));
					STRCPY(pMe->sVideoPath,sc_tmp1);
				}

				
			
			}
			else //Ĭ��ֻ����Ƭ·��  ��ʵ��������ǲ����ڵ�
			{
				MEMSET(pMe->sPhotoPath,0x00,sizeof(pMe->sPhotoPath));
				STRCPY(pMe->sPhotoPath,sc_tmp);
			}

			FREE(sc_tmp1);
			FREE(sc_tmp);
			sc_tmp = NULL;
			sc_tmp1 = NULL;
			
			
		}
		//     end   ///////////

		sc_End = NULL;
		sc_Start =NULL;

	}
	else if (sc_one == '2')
	{
		si_ret = 3; // ������ʧ��
		return si_ret;
	}
	else if (sc_one == '0') 
	{
		si_ret = 4; // �������ϴ����ɹ�
		return si_ret;
	}

	
	return si_ret;

}





////  ����˵��  ������������� ʧ�ܺ�Ż��ȥ�ģ�������ֵ��2��ͷ��

// ����������Ȩ�ķ��ص�ֵ��д�������ļ�
// ����ֵ˵�� ������� 1 - ��ɹ�����������Ȩ - ����Ҫ����д����  0 - �򷵻ص��Ǵ�����Ϣ - ����ʧ��  2 - ��û��Ҫ�ĵģ�����Ҫ����д����  
// 3 --�����ڿ����ϵ� ������ʧ�� ��Ӧ�����ַ� ��2��      4 -- �������ϴ����ɹ�  5 -- ����Ȩ��δ��Ȩ ����ر�
int ParseStringandWriteToConfigure_xintiao_GPS_FAIL( newgimis *pMe,char *pString)
{
	int si_ret = 0;
	int si_Start = 0;
	int si_End = 0;
	int si_Count = 0;
	char *sc_Start = NULL;
	char *sc_End = NULL;
	char *sc_tmp = NULL;
	char *sc_tmp1 = NULL;
	char *first_tmp = NULL; 

	char sc_one;

	char *sc_tmpz = NULL; //��ʱ�洢��һ���ֶε�ֵ
	char scc_tmp1;
	char scc_tmp2;
	char scc_tmp3;  //����QA��
	char scc_tmp4;	// ����QA��

	char pszDelimiter[DEFAULT_SPLIT_LEN]  = DEFAULT_FIELD_SPLIT;

	MEMCPY(&sc_one,pString,1);


	if (sc_one == '2')
	{
		//��˵�����ճɹ���ȷ��������Ϣ


		// ���ҵ�һ��
		sc_Start = STRSTR(pString,"2dec");
//		uf_RecLog(pMe,"������ 222A FAIL");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start,";");
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," sc_End = %p",sc_End); 

			si_Count = sc_End - sc_Start - 4;

			FREEIF(sc_tmp);
			sc_tmp = (char *)MALLOC(si_Count + 5);

//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," sc_tmp = %p",sc_tmp); 
//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," si_Count + 5 = %d",si_Count + 5); 
			MEMSET(sc_tmp,0x00,si_Count + 5);
//			uf_RecLog(pMe,"������ 221D FAIL ");
			STRNCPY(sc_tmp,sc_Start+4,si_Count);
//			uf_RecLog(pMe,"������ 221F FAIL ");
			FREE(sc_tmp);
			sc_tmp = NULL;

			sc_tmpz = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmpz,0x00,si_Count + 5);

			STRNCPY(sc_tmpz,sc_Start+4,si_Count);
			uf_RecLog(pMe, sc_tmpz );
			MEMCPY(&scc_tmp1,sc_tmpz,1);
			MEMCPY(&scc_tmp2,sc_tmpz+1,1);
//			uf_RecLog(pMe,"������ 221Q FAIL ");

			

			// A2-----����߶�����------iFontHeight				A3----����������-------iFontWidth
			// A4-----��Ƶ�ļ�ʱ������------iAddHour			AU------��Ȩ------cAuthorize
			// AA-----ʹ�ü�Ȩ��λ-----cCTAuth					AD-----�Ƿ�����ʹ�� ��1#�� ���˳�-----cCanExit
			// AY-----����GPS���� ----

			// CP---�ϱ�GPS���������ĵ�ַ----strCenterIp

			// GE ----- ��λ����------cAccType					GW------���Ź�------cWatchDog
			// GS-----�Ƿ񿪻�������-----cStartMode				GT-----������λ����-----iPosDetAtime	
			// GD------��λ��ʽ------cPosDetMode				GM-  --------ʹ�ø�����λʱ�Ķ�λ����-------iAssInterval
			// GC------����GPS����  0Ϊ�ر�gps ���˳�����¼�����ļ�����1Ϊ����gps����¼������  2-Ϊ�˳�����

			// HT---������������ ---- ipulsation                HG --- �ɼ����ڣ����ϱ�GPS��ʱ���� ---- iInterval
					
			// 	LG------�Ƿ���д��־-----cWrtLog
		
			// PP------����(������Ƭ)����-----cUsePhoto			PW------�ֻ�����Ƭ����ʾ�ߴ�(���)------iPhotoWidth
			// PD----������Ϣ��������------nKeepDays			PH-----�ֻ�����Ƭ����ʾ�ߴ�(�߶ȶ�)------iPhotoHeight 
			// PC-----������ʱʹ�õ�ͼƬ����ţ� 0��    �á��ճ��������س���  cPicMode         
			// PL-----sPhotoPath ��Ƭ·��  sVideoPath  ��Ƶ��ŵ�·��	
			// QC ---- ��ӦͨѶ¼ QCA --- ���ͨѶ¼  QCD -- ɾ��ĳ��ͨѶ¼  QCDA -- ɾ��ȫ��ͨѶ¼
			
			// ST----�����ֻ��ͺ�------sMobileType	
			first_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(first_tmp,0x00,si_Count + 5);
			STRCPY(first_tmp,sc_tmpz+2);

//			uf_RecLog(pMe,"������ 221W FAIL");
			switch(scc_tmp1)  
			{
				case 'A':
					{
						switch(scc_tmp2)
						{
						case '1': // A1
							break;
						case '2': // A2
							break;
						case '3': // A3
							break;
						case '4': // A4
							break;
						case 'A': // AA
							break;
						case 'D': // AD
							STRCPY(&(pMe->cCanExit),first_tmp);
							break;
						case 'U': // AU
							break;
						case 'Y': // AY ����GPS����
							{ 

								pMe->nArySize = ATOI(first_tmp);
							}
							break;
						}
					}
					break;
				case 'C':
					{		
						switch(scc_tmp2)
						{
						case 'P': //CP
							STRCPY(pMe->strCenterIp,first_tmp);
							break;
						}
					}
				break;

				case 'G':
					{		
						switch(scc_tmp2)
						{
						case 'E': // GE
							STRCPY(&(pMe->cAccType),first_tmp);
							pMe->m_Acc = pMe->cAccType - '0'; //�ȼ�����   ��λ���� '1'��'2'��'3'��'4'��'5'��'6',ȱʡΪ'3'
							break;
						case 'W': // GW
							STRCPY(&(pMe->cWatchDog),first_tmp);
							break;
						case 'S':// GS
							STRCPY(&(pMe->cStartMode),first_tmp);
							break;
						case 'T':// GT
							pMe->iPosDetAtime = ATOI(first_tmp);
							break;
						case 'D':// GD

							//ZEROAT(pMe->m_cPosDetMode);
							MEMSET(pMe->m_cPosDetMode, 0, sizeof(pMe->m_cPosDetMode));
							STRCPY((pMe->m_cPosDetMode),first_tmp);
							pMe->cPosDetMode = ATOI(pMe->m_cPosDetMode);
							//����ģʽ�л�
							if (pMe->cPosDetMode == 13)
							{
								ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�л�������4");
								newgimis_CloseGPS( pMe );
								newgimis_Init_UKBD( pMe );
								newgimis_OpenGPS_UKBD(pMe);
							}
							else if (pMe->cPosDetMode == 14)
							{
								ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�л���S958GPS4");
								newgimis_CloseGPS_S958GPS(pMe);
								if (newgimis_Init_S958GPS( pMe ))
								{
									newgimis_OpenGPS_S958GPS(pMe);
								}
							}
							else
							{
								newgimis_CloseGPS_S958GPS(pMe);
								newgimis_CloseGPS_UKBD(pMe);
								newgimis_OpenGPS(pMe);
								pMe->m_NOMAINPOS = 0;
							}
							break;
						case 'M':// GM
							MEMSET(first_tmp,0x00,300);
							STRCPY(first_tmp,sc_tmpz+3);
							pMe->iAssInterval = ATOI(first_tmp);
							break;
						case 'C':// GC
							{
								STRCPY(&(pMe->cGpsMode),first_tmp);
								if ((pMe->cGpsMode == '1')&&(pMe->GPSFlag == FALSE)) // ˵������GPS
								{
									newgimis_OpenGPS(pMe);  // ֻ����GPS 
								}
								if ((pMe->cGpsMode == '0') && (pMe->GPSFlag == TRUE)) // ˵���ر�GPS
								{
									newgimis_CloseGPS(pMe);
								}
							}
							break;	
							
						}
					}
				break;

				case 'H':
					{		
						switch(scc_tmp2)
						{
						case 'T': // HT
							pMe->ipulsation = ATOI(first_tmp);
							break;
						case 'G': // HG
							{
								pMe->iInterval = ATOI(first_tmp);
//								uf_RecLog(pMe,"�ı䶨λ����");
								uf_RecLog(pMe,"change position cycle");

								if (pMe->cPosDetMode == 13)
								{
									ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�����ɼ����ڵ���5");
									newgimis_CloseGPS_UKBD( pMe );
									newgimis_Init_UKBD( pMe );
									newgimis_OpenGPS_UKBD(pMe);
								}
								else if (pMe->cPosDetMode == 14)
								{
									ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"S958GPS�ɼ����ڵ���5");
									newgimis_CloseGPS_S958GPS(pMe);
									if (newgimis_Init_S958GPS( pMe ))
									{
										newgimis_OpenGPS_S958GPS(pMe);
									}
								}
								else
								{
									ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY)uf_Pos_Track_stopstart_error, pMe);  // �л�ģʽ	�������Ǹı䶨λ����		
								}
								
													
							}
							break;
					
						}
					}
				break;

				case 'L':
					{		
						switch(scc_tmp2)
						{
						case 'G': // LG
							STRCPY(&(pMe->cWrtLog),first_tmp);
							break;
					
						}
					}
				break;

				case 'P':
					{		
						switch(scc_tmp2)
						{
						case 'P': // PP
							STRCPY(&(pMe->cUsePhoto),first_tmp);
							break;
						case 'W': // PW
							break;
						case 'D':// PD
							pMe->nKeepDays = ATOI(first_tmp);
							break;
						case 'H':// PH
							break;
						case 'C':// PC
							STRCPY(&(pMe->cPicMode),first_tmp);
							break;
						case 'L':// PL
							break;
							
						}
					}
				break;

				case 'Q': // ͨѶ¼�ô�
					{
//						uf_RecLog(pMe," ������ 222B FAIL");
						if (scc_tmp2 == 'C')
						{
							crichAddr *me = NULL;
							MEMCPY(&scc_tmp3,sc_tmpz+2,1);
							MEMCPY(&scc_tmp4,sc_tmpz+3,1);
							// sc_tmpz
//							uf_RecLog(pMe," ������ 222C FAIL");
							switch(scc_tmp3)
							{
								case 'A':// QCA // ���ͨѶ¼
									{
									//	if (pszTok) 
									//	{
									//		nResult = ud_DistToSplit( pszTok, DEFAULT_FIELD_SPLIT );
									//		if (nResult > 0)	
									//		{
//												uf_RecLog(pMe,"������ 3 FAIL");
												addbook_init(pMe);

												me = pMe->pRichAddr;
												me->pstrRecU = (char *)MALLOC(si_Count+5);	
												MEMSET(me->pstrRecU,0x00,si_Count+5);
												MEMCPY(me->pstrRecU,sc_tmpz+3,si_Count+5-3);

												uf_RecLog(pMe,"add addbook content 2");
												uf_RecLog(pMe,me->pstrRecU);

												if (me->pstrRecD != NULL) 
												{
													int iLen = si_Count+5 + STRLEN(me->pstrRecD) + 1 ;
													me->pstrRecD = (char *)REALLOC(me->pstrRecD,(iLen+1));
													STRCAT(me->pstrRecD,DEFAULT_SPLIT);
													STRCAT(me->pstrRecD,me->pstrRecU);
												}
												

									//		}
									//	}
										Add_addbook(pMe);
										addbook_free(pMe);
									}
							
								break;	
								case 'D':// QCD // ɾ��ĳ��ͨѶ¼  ���� ɾ��ȫ��ͨѶ¼
									{
										if (scc_tmp4 == 'A') // ˵����ɾ��ȫ��ͨѶ¼
										{
											addbook_init(pMe);
											Del_all_addbook(pMe);
											addbook_free(pMe);
										}
										else  // ɾ��ĳ��ͨѶ¼
										{

											// ɾ��ĳ��ͨѶ¼
											addbook_init(pMe);

											me = pMe->pRichAddr;
											me->pstrRecD = (char *)MALLOC(si_Count+5);	
											MEMSET(me->pstrRecD,0x00,si_Count+5);
											MEMCPY(me->pstrRecD,sc_tmpz+3,si_Count+5-3);

											uf_RecLog(pMe,me->pstrRecD);										
											
											Del_addbook(pMe);
											addbook_free(pMe);																		
										}
									}
							
								break;
							}	
						}
						
					}
					break;
				case 'S':
					{		
						switch(scc_tmp2)
						{
						case 'T': // ST
							MEMSET(pMe->sMobileType,0x00,sizeof(pMe->sMobileType));
							STRCPY(pMe->sMobileType,first_tmp);
							break;
					
						}
					}
				break;
				case 'T':
					{
						switch(scc_tmp2)
						{
						case 'B': // TB  ����ʱ��
							MEMSET(pMe->sMobileType,0x00,sizeof(pMe->sMobileType));
							pMe->iRestTb = ATOI(first_tmp);  // == 25 ˵������ʱ���ǹرյ�
							MEMSET(pMe->sRestTb,0x00,11);
							STRCPY(pMe->sRestTb,first_tmp);
	//						STRCPY(pMe->sMobileType,first_tmp);
							break;
					
						}
					}
					break;
			}

			FREE(first_tmp);
			first_tmp = NULL;

			FREE(sc_tmpz);
			sc_tmpz = NULL;

			//  ������������ǲ���������ֵ��
			// ����д����ʡ��

			
		}
		
		// ������������   ipulsation  int ��
		sc_Start = STRSTR(pString,";HT");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->ipulsation = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// �ɼ����ڣ����ϱ�GPS��ʱ����   iInterval  int ��
		sc_Start = STRSTR(pString,";HG");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iInterval = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;

			if (pMe->cPosDetMode == 13)
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�����ɼ����ڵ���6");
				newgimis_CloseGPS_UKBD( pMe );
				newgimis_Init_UKBD( pMe );
				newgimis_OpenGPS_UKBD(pMe);
			}
			else if (pMe->cPosDetMode == 14)
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"S958GPS�ɼ����ڵ���6");
				newgimis_CloseGPS_S958GPS(pMe);
				if (newgimis_Init_S958GPS( pMe ))
				{
					newgimis_OpenGPS_S958GPS(pMe);
				}
			}
		}

		/////////////  ��Ӳ���������Ϣ   m_E_SC
		
		// �ɼ����ڣ����ϱ�GPS��ʱ����   iInterval  int ��
		sc_Start = STRSTR(pString,";CE");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->m_E_SC = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
			ud_RecLog(pMe->a.m_pIShell, 1 ,&(pMe->nInd)," continue error number m_E_SC = %d",pMe->m_E_SC); 
		}
		////////////////////////////////////////////////


		// �ϱ�GPS���������ĵ�ַ  strCenterIp   
		sc_Start = STRSTR(pString,";CP");
		if(sc_Start != NULL)
		{
			sc_Start = STRSTR(pString,";CP");
			if(sc_Start != NULL)
			{
				char * sc_tmp = NULL;
				sc_tmp = STRSTR(sc_Start,"http://");
				if(sc_tmp == NULL)
				{
					sc_tmp = STRSTR(sc_Start,"HTTP://");
					if (sc_tmp == NULL)
					{
						sc_End = STRSTR(sc_Start+2,";");
						si_Count = sc_End - sc_Start - 3;
						MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
						STRNCPY(pMe->strCenterIp,sc_Start+3,si_Count);
					}
					else
					{
						sc_End = STRSTR(sc_Start+2,";");
						si_Count = sc_End - sc_Start - 3 - 7;
						MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
						STRNCPY(pMe->strCenterIp,sc_Start+3 + 7,si_Count);
					}
				}
				else
				{
					sc_End = STRSTR(sc_Start+2,";");
					si_Count = sc_End - sc_Start - 3 - 7;
					MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
					STRNCPY(pMe->strCenterIp,sc_Start+3 + 7,si_Count);
				}
				sc_tmp = NULL;
			}
			//			sc_End = STRSTR(sc_Start+2,";");
			//			si_Count = sc_End - sc_Start - 3;
			//			MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
			//			STRNCPY(pMe->strCenterIp,sc_Start+3,si_Count);
		}

		// ��λ��ʽ  cPosDetMode  char    
		sc_Start = STRSTR(pString,";GD");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;

			//ZEROAT(pMe->m_cPosDetMode);
			MEMSET(pMe->m_cPosDetMode, 0, sizeof(pMe->m_cPosDetMode));
			STRNCPY((pMe->m_cPosDetMode),sc_Start+3,si_Count);

			pMe->cPosDetMode = ATOI(pMe->m_cPosDetMode);

			//����ģʽ�л�
			if (pMe->cPosDetMode == 13)
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�л�������5");
				newgimis_CloseGPS( pMe );
				newgimis_Init_UKBD( pMe );
				newgimis_OpenGPS_UKBD(pMe);
			}
			else if (pMe->cPosDetMode == 14)
			{
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�л���S958GPS5");
				newgimis_CloseGPS_S958GPS(pMe);
				if (newgimis_Init_S958GPS( pMe ))
				{
					newgimis_OpenGPS_S958GPS(pMe);
				}
			}
			else
			{
				newgimis_CloseGPS_S958GPS(pMe);
				newgimis_CloseGPS_UKBD(pMe);
				newgimis_OpenGPS(pMe);
				pMe->m_NOMAINPOS = 0;
			}
		}

		// ��λ����  cAccType  char   
		sc_Start = STRSTR(pString,";GE");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cAccType),sc_Start+3,si_Count);
			pMe->m_Acc = pMe->cAccType - '0'; //�ȼ�����   ��λ���� '1'��'2'��'3'��'4'��'5'��'6',ȱʡΪ'3'
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," positionaI accuracy m_Acc = %d",pMe->m_Acc); 
		}

		// ���Ź�  cWatchDog  char     
		sc_Start = STRSTR(pString,";GW");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cWatchDog),sc_Start+3,si_Count);
		}

		// ���϶೤ʱ��(����)�������ֻ�  cWatchDog  char 

		////////////////////////////////////////////////////
		///////////////////////////
		//////////////////////// ���»�û�������� ����   
		sc_Start = STRSTR(pString,";TA");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
		//	STRNCPY(&(pMe->cWatchDog),sc_Start+3,si_Count);
		}

		
		// �Ƿ񿪻�������  cStartMode  char   
		sc_Start = STRSTR(pString,";GS");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cStartMode),sc_Start+3,si_Count);
		}

		// ��Ȩ  cAuthorize  char     	//���ڷ���ʱ�� �ж���Ȩ��־λ���˳�
		sc_Start = STRSTR(pString,";AU");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cAuthorize),sc_Start+3,si_Count);
		}

		// 0Ϊ�ر�gps���˳�����¼�����ļ�����1Ϊ����gps����¼�����ļ�����2�˳����򣨲���¼�����ļ���  cAuthorize  char 


		sc_Start = STRSTR(pString,";GC");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cGpsMode),sc_Start+3,si_Count);

			//20170317 ����
			if ((pMe->cGpsMode == '1')&&(pMe->GPSFlag == FALSE)) // ˵������GPS
			{
				newgimis_OpenGPS(pMe);  // ֻ����GPS 
			}
			if ((pMe->cGpsMode == '0') && (pMe->GPSFlag == TRUE)) // ˵���ر�GPS
			{
				newgimis_CloseGPS(pMe);
			}
		}



		////////////////////////////////////////////
		//////////////////////////////////
		//////////////////////   δ���

		// ÿ�η��ͼ������ݣ����8��  cAuthorize  char 
		sc_Start = STRSTR(pString,";AY");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;

			sc_tmp = (char *)MALLOC(10);
			MEMSET(sc_tmp,0x00,10);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->nArySize = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
			
	//		STRNCPY(&(pMe->cAuthorize),sc_Start+3,si_Count);
		}

		sc_Start = STRSTR(pString,";TB");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;

			sc_tmp = (char *)MALLOC(10);
			MEMSET(sc_tmp,0x00,10);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iRestTb = ATOI(sc_tmp);
			MEMSET(pMe->sRestTb,0x00,11);
			STRCPY(pMe->sRestTb,sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
			
		}

		
		// ����(����������״̬)����   0 : ������ , 1 ����  cUseLoad  char 
		sc_Start = STRSTR(pString,";UL");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cUseLoad),sc_Start+3,si_Count);
		}
		
		// ������λ������ȱʡΪ10  iPosDetAtime  int    
		sc_Start = STRSTR(pString,";GT");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iPosDetAtime = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}


		// �Ƿ���д��־�Ĺ��ܣ�  '0' ��д��־, '1' д��־   cWrtLog  char    
		sc_Start = STRSTR(pString,";LG");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cWrtLog),sc_Start+3,si_Count);
		}
		
		// ����߶����أ�ȱʡֵΪ16. ���磺 16   iFontHeight  int    
		sc_Start = STRSTR(pString,";A2");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iFontHeight = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// ���������أ�ȱʡֵΪ22�����磺 22   iFontWidth  int     
		sc_Start = STRSTR(pString,";A3");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iFontWidth = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}


		// ��Ƶ�ļ�ʱ������   iAddHour  int     
		sc_Start = STRSTR(pString,";A4");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iAddHour = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// �����ֻ��ͺţ� sMobileType     
		sc_Start = STRSTR(pString,";ST");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			MEMSET(pMe->sMobileType,0x00,sizeof(pMe->sMobileType));
			STRNCPY(pMe->sMobileType,sc_Start+3,si_Count);
		}

		// 
		//����(������Ƭ)����       0 : ������ , 1 ����  cUsePhoto    ������Ƭ����Ƶ���� 
		sc_Start = STRSTR(pString,";PP");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			STRNCPY(&(pMe->cUsePhoto),sc_Start+3,si_Count);
		}

		// 
		// �ֻ�����Ƭ����ʾ�ߴ�(���)  iPhotoWidth  int      
		sc_Start = STRSTR(pString,";PW");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iPhotoWidth = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}


		// �ֻ�����Ƭ����ʾ�ߴ�(�߶ȶ�)  iPhotoHeight  int   
		sc_Start = STRSTR(pString,";PH");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iPhotoHeight = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// 


		///////////////////////////////////////////////////////
		////////////////////////////////////////////
		////////////////////////   δ���
		// ��Ƭ�ļ�ʱ����������λ��Сʱ��   iAddHour  int 
		sc_Start = STRSTR(pString,";A1");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iAddHour = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}


		//'1'  ʹ�ü�Ȩ��λ����ʹ�õ��ŵĽӿڣ�, '0' : �����ü�Ȩ��λ (����ʹ�õ��ŵĽӿ�)   cCTAuth  char  
		sc_Start = STRSTR(pString,";AA");  
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			STRNCPY(&(pMe->cCTAuth),sc_Start+3,si_Count);
		}
		
		// 
		// ������Ϣ��������   nKeepDays  int    
		sc_Start = STRSTR(pString,";PD");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->nKeepDays = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// 
		// ʹ�ø�����λʱ�Ķ�λ����,��λ��    iAssInterval  int  
		sc_Start = STRSTR(pString,";GM-");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 4;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+4,si_Count);
			pMe->iAssInterval = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// ���������
		sc_Start = STRSTR(pString,";SN");
		if(sc_Start != NULL)
		{

			// ˵��������	

			cTaskApp *me_t;

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(10);
			MEMSET(sc_tmp,0x00,10);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->m_TaskC = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;

			pMe->m_Task_F++ ;// task_first ��������ǲ��ǳ����������һ�ν�����������ǵĻ���������ǰ������û��������ǰ��
			cTaskApp_HandleEvent(pMe,EVT_APP_START,20,0);
			me_t = pMe->pTaskApp;	
			cTaskApp_TaskInteract(me_t);

			
// 			//#��̨ģʽ��������#	��̨����ʱ���и������񣬵�����ʾ�Ƿ�ʼ������ 2016-1-21
 			uf_RecLog(pMe,"#��̨ģʽ �յ�������3 #");

			ISHELL_SetTimer(pMe->a.m_pIShell, TASK_SET_TIMER*1000, (PFNNOTIFY)Timer_Task_Notify,pMe);

		}

		//  [3/4/2016 Administrator]
		// һ�����еĶԽ�����
		sc_Start = STRSTR(pString,";IP");
		if(sc_Start != NULL)
		{
			
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
			MEMSET(pMe->m_IP,0x00,256);
			STRCPY(pMe->m_IP,sc_tmp);
			
			
			FREE(sc_tmp);
			sc_tmp = NULL;
			
			uf_RecLog(pMe,"IP ��ַ");
			uf_RecLog(pMe,pMe->m_IP);
		}

		// һ�����еĶԽ�����
		sc_Start = STRSTR(pString,";QA");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
	
			richptt_SetUsrGroup(pMe->pRichptt,sc_tmp);

		   FREEIF(pMe->sUsrGroup);
		   pMe->sUsrGroup = (char *)MALLOC((si_Count + 5)*sizeof(char));
		   MEMSET(pMe->sUsrGroup,0,(si_Count + 5)*sizeof(char));
		   STRCPY(pMe->sUsrGroup,sc_tmp);


			FREE(sc_tmp);
			sc_tmp = NULL;

			
			
		}


		// һ�����еĶԽ�����
		sc_Start = STRSTR(pString,";QE");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
	
			FREEIF(pMe->sPttCallNo);
		    pMe->sPttCallNo = (char *)MALLOC((si_Count + 5)*sizeof(char));
		    MEMSET(pMe->sPttCallNo,0,(si_Count + 5)*sizeof(char));
		    STRCPY(pMe->sPttCallNo,sc_tmp);
		    ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_SETPTTCALL_EVENT,(uint16)0, 0);


			FREE(sc_tmp);
			sc_tmp = NULL;

			
			
		}


		// �ڰ����� 
		sc_Start = STRSTR(pString,";QG");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
			richPtt_SetCallRestInfo(pMe->pRichptt,sc_tmp);

			FREE(sc_tmp);
			sc_tmp = NULL;

			
			
		}

		// Qchat���� 
		sc_Start = STRSTR(pString,";QB");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
			richptt_SetDomain(pMe->pRichptt,sc_tmp);

			FREE(sc_tmp);
			sc_tmp = NULL;

			
			
		}

		// һ�����еİ���ֵ
		sc_Start = STRSTR(pString,";QF");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
			pMe->nPttkey = STRTOUL( sc_tmp, (char **)&pszDelimiter, 10 );

			FREE(sc_tmp);
			sc_tmp = NULL;		
		}
		

		// ͨѶ¼ͬ�� ɾ��
		sc_Start = STRSTR(pString,";QCA");
		if(sc_Start != NULL)
		{
			crichAddr *me;
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 4;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+4,si_Count);
			
			addbook_init(pMe);
			
			me = pMe->pRichAddr;
			me->pstrRecU = (char *)MALLOC(si_Count+5);	
			MEMSET(me->pstrRecU,0x00,si_Count+5);
			STRCPY(me->pstrRecU,sc_tmp);

			uf_RecLog(pMe,me->pstrRecU);

			if (me->pstrRecD != NULL) 
			{
				int iLen = si_Count+5 + STRLEN(me->pstrRecD) + 1 ;
				me->pstrRecD = (char *)REALLOC(me->pstrRecD,(iLen+1));
				STRCAT(me->pstrRecD,DEFAULT_SPLIT);
				STRCAT(me->pstrRecD,me->pstrRecU);
			}

			Add_addbook(pMe);
			addbook_free(pMe);

			FREE(sc_tmp1);
			FREE(sc_tmp);
			sc_tmp = NULL;
			sc_tmp1 = NULL;
			
			
		}

		// ͨѶ¼ͬ�� ɾ��
		sc_Start = STRSTR(pString,";QCD");
		if(sc_Start != NULL)
		{
			crichAddr *me;
			char *sc_StartE = NULL;
			sc_StartE = STRSTR(pString,";QCDA");
			if (sc_StartE != NULL)
			{
				// ˵��ɾ������ͨѶ¼ȫ��

				addbook_init(pMe);
				Del_all_addbook(pMe);
				addbook_free(pMe);
				sc_StartE = NULL;
			}
			else
			{
				// ɾ��ĳ��ͨѶ¼
			


				sc_End = STRSTR(sc_Start+2,";");
				si_Count = sc_End - sc_Start - 4;
				sc_tmp = (char *)MALLOC(si_Count + 5);
				MEMSET(sc_tmp,0x00,si_Count + 5);
				STRNCPY(sc_tmp,sc_Start+4,si_Count);
				
				addbook_init(pMe);
				me = pMe->pRichAddr;
				me->pstrRecD = (char *)MALLOC(si_Count+5);	
				MEMSET(me->pstrRecD,0x00,si_Count+5);
				STRCPY(me->pstrRecD,sc_tmp);

				uf_RecLog(pMe,me->pstrRecD);

				Del_addbook(pMe);
				addbook_free(pMe);
			}

			FREE(sc_tmp1);
			FREE(sc_tmp);
			sc_tmp = NULL;
			sc_tmp1 = NULL;
			
			
		}


		// Del_addbook(newgimis *pMe)

		//  
		//������ʱʹ�õ�ͼƬ����ţ� 0��    �á��ճ��������س���  cPicMode     
		sc_Start = STRSTR(pString,";PC");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			STRNCPY(&(pMe->cPicMode),sc_Start+3,si_Count);
		}


		//	QEZA=;
		//	QFZA=; ����������û�н���
		//	
		//�Ƿ�����ʹ�� ��1#�� ���˳�  0 �������� , 1 ����    cCanExit char  
		sc_Start = STRSTR(pString,";AD");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			STRNCPY(&(pMe->cCanExit),sc_Start+3,si_Count);
		}


		//  2015-10-29  start //
		// sPhotoPath ��Ƭ·��  sVideoPath  ��Ƶ��ŵ�·��   
		// �ֻ�����Ƭ�Ĵ洢Ŀ¼����Ƶ�ļ���Ŀ¼����ƬĿ¼����ƵĿ¼֮����","�ָ�
		sc_Start = STRSTR(pString,";PL");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			sc_Start = STRSTR(sc_tmp,",");
			if(sc_Start != NULL)
			{
				MEMSET(pMe->sPhotoPath,0x00,sizeof(pMe->sPhotoPath));
				STRNCPY(pMe->sPhotoPath,sc_tmp,sc_Start-sc_tmp);

				sc_tmp1 = (char *)MALLOC(100);
				MEMSET(sc_tmp1,0x00,100);
				STRNCPY(sc_tmp1,sc_Start+1,si_Count - (sc_Start -sc_tmp + 1));
				
				sc_Start = STRSTR(sc_tmp1,",");
				if (sc_Start != NULL)
				{
					MEMSET(pMe->sVideoPath,0x00,sizeof(pMe->sVideoPath));
					STRNCPY(pMe->sVideoPath,sc_tmp1,sc_Start-sc_tmp1);

					MEMSET(pMe->sAudRecPath,0x00,sizeof(pMe->sAudRecPath));
					STRCPY(pMe->sAudRecPath,sc_Start+1); //��֪�������᲻��������
				}
				else
				{
					MEMSET(pMe->sVideoPath,0x00,sizeof(pMe->sVideoPath));
					STRCPY(pMe->sVideoPath,sc_tmp1);
				}

				
			
			}
			else //Ĭ��ֻ����Ƭ·��  ��ʵ��������ǲ����ڵ�
			{
				MEMSET(pMe->sPhotoPath,0x00,sizeof(pMe->sPhotoPath));
				STRCPY(pMe->sPhotoPath,sc_tmp);
			}

			FREE(sc_tmp1);
			FREE(sc_tmp);
			sc_tmp = NULL;
			sc_tmp1 = NULL;
			
			
		}
		//     end   ///////////

		sc_End = NULL;
		sc_Start =NULL;

	}
	
	return si_ret;

}


//////////   end  ///////////////////////////////////










// ����������Ȩ�ķ��ص�ֵ��д�������ļ�
// ����ֵ˵�� ������� 1 - ��ɹ�����������Ȩ  0 - �򷵻ص��Ǵ�����Ϣ
int ParseStringandWriteToConfigure( newgimis *pMe,char *pString)
{
	int si_ret = 0;
	int si_Start = 0;
	int si_End = 0;
	int si_Count = 0;
	char *sc_Start = NULL;
	char *sc_End = NULL;
	char *sc_tmp = NULL;
	char *sc_tmp1 = NULL;

	char sc_one;
	char pszDelimiter[DEFAULT_SPLIT_LEN]  = DEFAULT_FIELD_SPLIT;
	
	MEMCPY(&sc_one,pString,1);
	if (sc_one == '1')
	{
		//��˵�����ճɹ���ȷ��������Ϣ
		si_ret = 1;

		// ���ҵ绰����
		sc_Start = STRSTR(pString,"1dectel");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start,";");
			si_Count = sc_End - sc_Start - 7;
// 			sc_tmp = (char *)MALLOC(100);
// 			MEMSET(sc_tmp,0x00,100);
// 			STRNCPY(sc_tmp,sc_Start+7,si_Count);
// 			FREE(sc_tmp);
// 			sc_tmp = NULL;
			MEMSET(pMe->MyMobileNo,0x00,sizeof(pMe->MyMobileNo));
			STRNCPY(pMe->MyMobileNo,sc_Start+7,si_Count);
		}
		else
		//////// ���� else �ǽ����Ȩ������
		{
			if ( STRSTR(pString,"1decCPnull") != NULL || STRSTR(pString,"1decCPNULL") != NULL )
			{
				pMe->m_SQ_C = 2;
				ud_RecLog(pMe->a.m_pIShell ,1,&(pMe->nInd),"ǰ��̨��־λ = %d",pMe->m_FOBFlag);
				return 0;
			}
			
			sc_Start = STRSTR(pString,"1decAU0");
			if (sc_Start != NULL)
			{
				sc_Start = STRSTR(pString,";CP");
				if (sc_Start != NULL)
				{
					sc_End = STRSTR(sc_Start+1,";");
					if (sc_End == NULL)
					{
						sc_tmp = (char *)MALLOC(200);
						MEMSET(sc_tmp,0x00,200);
						STRCPY(sc_tmp, sc_Start+3);
						
						MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
						STRCPY(pMe->strCenterIp,sc_tmp);
						pMe->m_SQ_again = 1;
						FREE(sc_tmp);
						sc_tmp = NULL;
					}
					else
					{
						si_Count = sc_End - sc_Start - 3;
						sc_tmp = (char *)MALLOC(200);
						MEMSET(sc_tmp,0x00,200);
						STRNCPY(sc_tmp,sc_Start+3,si_Count);
						
						MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
						STRCPY(pMe->strCenterIp,sc_tmp);
						pMe->m_SQ_again = 1;
						FREE(sc_tmp);
						sc_tmp = NULL;
					}
					return 1;
				}
				// ˵��û����Ȩ
				pMe->m_SQ_C = 2;
				return 0;
			}

			sc_Start = STRSTR(pString,"1decAU1");
			if (sc_Start != NULL)
			{
				sc_Start = STRSTR(pString,";CP");
				if (sc_Start != NULL)
				{
					sc_End = STRSTR(sc_Start+1,";");
					if (sc_End == NULL)
					{
						sc_tmp = (char *)MALLOC(200);
						MEMSET(sc_tmp,0x00,200);
						STRCPY(sc_tmp, sc_Start+3);
						
						MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
						STRCPY(pMe->strCenterIp,sc_tmp);
						pMe->m_SQ_again = 1;
						FREE(sc_tmp);
						sc_tmp = NULL;
					}
					else
					{
						si_Count = sc_End - sc_Start - 3;
						sc_tmp = (char *)MALLOC(200);
						MEMSET(sc_tmp,0x00,200);
						STRNCPY(sc_tmp,sc_Start+3,si_Count);
						
						MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
						STRCPY(pMe->strCenterIp,sc_tmp);
						pMe->m_SQ_again = 1;
						FREE(sc_tmp);
						sc_tmp = NULL;
					}
					return 1;
				}
				// ˵��û����Ȩ
				pMe->m_SQ_C = 2;
				return 0;
			}


			sc_Start = STRSTR(pString,"1decCP");
			if (sc_Start != NULL)
			{
				sc_End = STRSTR(sc_Start,";");
				if (sc_End == NULL)
				{
					sc_tmp = (char *)MALLOC(200);
					MEMSET(sc_tmp,0x00,200);
					STRCPY(sc_tmp, sc_Start+6);

					MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
					STRCPY(pMe->strCenterIp,sc_tmp);
					pMe->m_SQ_again = 1;
					FREE(sc_tmp);
					sc_tmp = NULL;

				}
				else
				{
					si_Count = sc_End - sc_Start - 6;
					sc_tmp = (char *)MALLOC(200);
					MEMSET(sc_tmp,0x00,200);
					STRNCPY(sc_tmp,sc_Start+6,si_Count);
					
					MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
					STRCPY(pMe->strCenterIp,sc_tmp);
					pMe->m_SQ_again = 1;
					FREE(sc_tmp);
					sc_tmp = NULL;
				}
			}
		}

		//---------------------����8λ����
		sc_Start = STRSTR(pString,";FK"); //;FK000000
		if(sc_Start != NULL)
		{
			int i = 1;
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;

			sc_tmp = (char *)MALLOC(100);
			while (i <= si_Count)
			{
				MEMSET(sc_tmp,0x00,100);
				STRNCPY(sc_tmp,sc_Start+ 2 + i ,1);
				switch(i)
				{
				case 1:
					pMe->m_enable_Audio = ATOI(sc_tmp);
					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"��Ȩ�Զ��ϴ�¼��ʹ�� = %d",pMe->m_enable_Audio);
					break;
				case 2:
					STRCPY(&(pMe->m_Right_Key), sc_tmp);
					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"��Ȩ�Ҽ��������� = %c",pMe->m_Right_Key);
					break;
				case 3:
					pMe->m_min_Accuracy = ATOI(sc_tmp);
					if (pMe->m_min_Accuracy == 0)
					{
						pMe->m_min_Accuracy = 1;
					}
					else if (pMe->m_min_Accuracy > 6)
					{
						pMe->m_min_Accuracy = 6;
					}
					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"��Ȩ��;��� = %d",pMe->m_min_Accuracy);
					break;
				case 4:
					pMe->m_E_SC = ATOI(sc_tmp);
					ud_RecLog(pMe->a.m_pIShell, 1 ,&(pMe->nInd),"��Ȩ������������ = %d",pMe->m_E_SC); 
					break;
				case 5:
					pMe->m_del_Audio = ATOI(sc_tmp);
					ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"��Ȩ¼���Ƿ�ɾ�� = %d",pMe->m_del_Audio);
					break;
				case 6:
					break;
				}
				i++;
			}
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		//�����·�HTTP��ַ   sUrl_YYXF  CHAR[] ��
		sc_Start = STRSTR(pString,";AP");
		if(sc_Start != NULL)
		{
//			char * sc_tmp = NULL;
//			sc_tmp = STRSTR(sc_Start,"http://");
// 			if(sc_tmp == NULL)
// 			{
// 				sc_tmp = STRSTR(sc_Start,"HTTP://");
// 				if (sc_tmp == NULL)
// 				{
// 					sc_End = STRSTR(sc_Start+2,";");
// 					si_Count = sc_End - sc_Start - 3;
// 					MEMSET(pMe->sHttp_YYXF,0x00,sizeof(pMe->sHttp_YYXF));
// 					STRNCPY(pMe->sHttp_YYXF,sc_Start+3,si_Count);
// 				}
// 				else
// 				{
// 					sc_End = STRSTR(sc_Start+2,";");
// 					si_Count = sc_End - sc_Start - 3 - 7;
// 					MEMSET(pMe->sHttp_YYXF,0x00,sizeof(pMe->sHttp_YYXF));
// 					STRNCPY(pMe->sHttp_YYXF,sc_Start+3 + 7,si_Count);
// 				}
// 			}
// 			else
// 			{
// 				sc_End = STRSTR(sc_Start+2,";");
// 				si_Count = sc_End - sc_Start - 3 - 7;
// 				MEMSET(pMe->sHttp_YYXF,0x00,sizeof(pMe->sHttp_YYXF));
// 				STRNCPY(pMe->sHttp_YYXF,sc_Start+3 + 7,si_Count);
// 			}
// 			sc_tmp = NULL;

			char * sc_tmp = NULL;
			sc_tmp = STRSTR(sc_Start,"act=autdec");
			if(sc_tmp == NULL)
			{
				//tcpģʽ
				sc_End = STRSTR(sc_Start+2,":");
				si_Count = sc_End - sc_Start - 3;
				
				MEMSET(pMe->sTCPIP_YYXF,0x00,sizeof(pMe->sUDPIP_YYXF));
				STRNCPY(pMe->sTCPIP_YYXF,sc_Start+3,si_Count);
				
				sc_Start = STRSTR(sc_Start,":");
				sc_End = STRSTR(sc_Start + 1,";");
				si_Count = sc_End - sc_Start - 1;
				
				sc_tmp = (char *)MALLOC(100);
				MEMSET(sc_tmp,0x00,100);
				STRNCPY(sc_tmp,sc_Start+1,si_Count);
				pMe->iTCPPORT_YYXF = ATOI(sc_tmp);
				FREE(sc_tmp);
				
				ud_RecLog(pMe->a.m_pIShell, 1 ,&(pMe->nInd),"�����·�TCP IP = %s PORT = %d",pMe->sTCPIP_YYXF,pMe->iTCPPORT_YYXF); 
			}
			else
			{
				//httpģʽ
				sc_End = STRSTR(sc_Start+2,";");
				si_Count = sc_End - sc_Start - 3 - 7;
				MEMSET(pMe->sHttp_YYXF,0x00,sizeof(pMe->sHttp_YYXF));
				STRNCPY(pMe->sHttp_YYXF,sc_Start+3 + 7,si_Count);

				ud_RecLog(pMe->a.m_pIShell, 1 ,&(pMe->nInd),"�����·�HTTP��ַ = %s",pMe->sHttp_YYXF); 
			}
			
			sc_tmp = NULL;
		}
		
		//�����·�udp��ַ   
		sc_Start = STRSTR(pString,";UP");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,":");
			si_Count = sc_End - sc_Start - 3;
			
			MEMSET(pMe->sUDPIP_YYXF,0x00,sizeof(pMe->sUDPIP_YYXF));
			STRNCPY(pMe->sUDPIP_YYXF,sc_Start+3,si_Count);
			
			sc_Start = STRSTR(sc_Start,":");
			sc_End = STRSTR(sc_Start + 1,";");
			si_Count = sc_End - sc_Start - 1;

			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+1,si_Count);
			pMe->iUDPPORT_YYXF = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;

			ud_RecLog(pMe->a.m_pIShell, 1 ,&(pMe->nInd),"�����·�UDP IP = %s PORT = %d",pMe->sUDPIP_YYXF,pMe->iUDPPORT_YYXF); 
		}


		//�Ƿ��ͼƬ����MD5У��   m_MD5_EC  uint8 ��
		sc_Start = STRSTR(pString,";EC");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->m_enable_MD5 = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// ������������   ipulsation  int ��
		sc_Start = STRSTR(pString,";HT");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->ipulsation = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// �ɼ����ڣ����ϱ�GPS��ʱ����   iInterval  int ��
		sc_Start = STRSTR(pString,";HG");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iInterval = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;

// 			if (pMe->cPosDetMode == 13)
// 			{
// 				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�����ɼ����ڵ���7");
// 				newgimis_CloseGPS_UKBD( pMe );
// 				newgimis_Init_UKBD( pMe );
// 				newgimis_OpenGPS_UKBD(pMe);
// 			}
		}

		/////////////  ��Ӳ���������Ϣ   m_E_SC
		
		// �ɼ����ڣ����ϱ�GPS��ʱ����   iInterval  int ��
		sc_Start = STRSTR(pString,";CE");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->m_E_SC = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
			ud_RecLog(pMe->a.m_pIShell, 1 ,&(pMe->nInd)," continue error number m_E_SC = %d",pMe->m_E_SC); 
		}
		////////////////////////////////////////////////


		// �ϱ�GPS���������ĵ�ַ  strCenterIp
		sc_Start = STRSTR(pString,";CP");
		if(sc_Start != NULL)
		{
			sc_Start = STRSTR(pString,";CP");
			if(sc_Start != NULL)
			{
				char * sc_tmp = NULL;
				sc_tmp = STRSTR(sc_Start,"http://");
				if(sc_tmp == NULL)
				{
					sc_tmp = STRSTR(sc_Start,"HTTP://");
					if (sc_tmp == NULL)
					{
						sc_End = STRSTR(sc_Start+2,";");
						si_Count = sc_End - sc_Start - 3;
						MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
						STRNCPY(pMe->strCenterIp,sc_Start+3,si_Count);
					}
					else
					{
						sc_End = STRSTR(sc_Start+2,";");
						si_Count = sc_End - sc_Start - 3 - 7;
						MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
						STRNCPY(pMe->strCenterIp,sc_Start+3 + 7,si_Count);
					}
				}
				else
				{
					sc_End = STRSTR(sc_Start+2,";");
					si_Count = sc_End - sc_Start - 3 - 7;
					MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
					STRNCPY(pMe->strCenterIp,sc_Start+3 + 7,si_Count);
				}
				sc_tmp = NULL;
			}
			//			sc_End = STRSTR(sc_Start+2,";");
			//			si_Count = sc_End - sc_Start - 3;
			//			MEMSET(pMe->strCenterIp,0x00,sizeof(pMe->strCenterIp));
			//			STRNCPY(pMe->strCenterIp,sc_Start+3,si_Count);
		}

		// ��λ��ʽ  cPosDetMode  char 
		sc_Start = STRSTR(pString,";GD");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;

			//ZEROAT(pMe->m_cPosDetMode);
			MEMSET(pMe->m_cPosDetMode, 0, sizeof(pMe->m_cPosDetMode));
			STRNCPY((pMe->m_cPosDetMode),sc_Start+3,si_Count);
			pMe->cPosDetMode = ATOI(pMe->m_cPosDetMode);

// 			//����ģʽ�л�
// 			if (pMe->cPosDetMode == 13)
// 			{
// 				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�л�������6");
// 				newgimis_CloseGPS( pMe );
// 				newgimis_Init_UKBD( pMe );
// 				newgimis_OpenGPS_UKBD(pMe);
// 			}
// 			else
// 			{
// 				newgimis_CloseGPS_UKBD(pMe);
// 				newgimis_OpenGPS(pMe);
// 				pMe->m_NOMAINPOS = 0;
// 			}
		}

		// ��λ����  cAccType  char 
		sc_Start = STRSTR(pString,";GE");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cAccType),sc_Start+3,si_Count);
			pMe->m_Acc = pMe->cAccType - '0'; //�ȼ�����   ��λ���� '1'��'2'��'3'��'4'��'5'��'6',ȱʡΪ'3'
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," positionaI accuracy m_Acc = %d",pMe->m_Acc); 
		}

		// ���Ź�  cWatchDog  char 
		sc_Start = STRSTR(pString,";GW");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cWatchDog),sc_Start+3,si_Count);
		}

		// ���϶೤ʱ��(����)�������ֻ�  cWatchDog  char 

		////////////////////////////////////////////////////
		///////////////////////////
		//////////////////////// ���»�û�������� ����
		sc_Start = STRSTR(pString,";TA");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
		//	STRNCPY(&(pMe->cWatchDog),sc_Start+3,si_Count);
		}

		
		// �Ƿ񿪻�������  cStartMode  char 
		sc_Start = STRSTR(pString,";GS");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cStartMode),sc_Start+3,si_Count);
		}

		// ��Ȩ  cAuthorize  char	//���ڷ���ʱ�� �ж���Ȩ��־λ���˳�
		sc_Start = STRSTR(pString,";AU");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cAuthorize),sc_Start+3,si_Count);
		}

		// 0Ϊ�ر�gps���˳�����¼�����ļ�����1Ϊ����gps����¼�����ļ�����2�˳����򣨲���¼�����ļ���  cAuthorize  char 

		sc_Start = STRSTR(pString,";GC");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cGpsMode),sc_Start+3,si_Count);

			//20170317 ����
			if ((pMe->cGpsMode == '1')&&(pMe->GPSFlag == FALSE) && (NULL == STRSTR(pMe->strCenterIp, "autdec")) ) // ˵������GPS //20180322�������Ӵautdec��Ӧ�ÿ���
			{
				newgimis_OpenGPS(pMe);  // ֻ����GPS 
			}
			if ((pMe->cGpsMode == '0') && (pMe->GPSFlag == TRUE)) // ˵���ر�GPS
			{
				newgimis_CloseGPS(pMe);
			}
		}



		////////////////////////////////////////////
		//////////////////////////////////


		// ÿ�η��ͼ������ݣ����8��  nArySize  int 
		sc_Start = STRSTR(pString,";AY");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(10);
			MEMSET(sc_tmp,0x00,10);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->nArySize = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;

	//		STRNCPY(&(pMe->cAuthorize),sc_Start+3,si_Count);
		}

		////////////////////////////////////////////
		//////////////////////////////////
		//////////////////////   δ���
		
		// ����(����������״̬)����   0 : ������ , 1 ����  cAuthorize  char 
		sc_Start = STRSTR(pString,";UL");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cUseLoad),sc_Start+3,si_Count);
		}

		// ������λ������ȱʡΪ10  iPosDetAtime  int 
		sc_Start = STRSTR(pString,";GT");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iPosDetAtime = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}


		// �Ƿ���д��־�Ĺ��ܣ�  '0' ��д��־, '1' д��־   cWrtLog  char 
		sc_Start = STRSTR(pString,";LG");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			
			STRNCPY(&(pMe->cWrtLog),sc_Start+3,si_Count);
		}
		
		// ����߶����أ�ȱʡֵΪ16. ���磺 16   iFontHeight  int 
		sc_Start = STRSTR(pString,";A2");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iFontHeight = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// ���������أ�ȱʡֵΪ22�����磺 22   iFontWidth  int 
		sc_Start = STRSTR(pString,";A3");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iFontWidth = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}


		// ��Ƶ�ļ�ʱ������   iAddHour  int 
		sc_Start = STRSTR(pString,";A4");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iAddHour = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// �����ֻ��ͺţ� sMobileType 
		sc_Start = STRSTR(pString,";ST");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			MEMSET(pMe->sMobileType,0x00,sizeof(pMe->sMobileType));
			STRNCPY(pMe->sMobileType,sc_Start+3,si_Count);
		}

		// 
		//����(������Ƭ)����       0 : ������ , 1 ����  cUsePhoto 
		sc_Start = STRSTR(pString,";PP");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			STRNCPY(&(pMe->cUsePhoto),sc_Start+3,si_Count);
		}

		// 
		// �ֻ�����Ƭ����ʾ�ߴ�(���)  iPhotoWidth  int 
		sc_Start = STRSTR(pString,";PW");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iPhotoWidth = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// �ֻ�����Ƭ����ʾ�ߴ�(�߶ȶ�)  iPhotoHeight  int 
		sc_Start = STRSTR(pString,";PH");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iPhotoHeight = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// 


		///////////////////////////////////////////////////////
		////////////////////////////////////////////
		////////////////////////   δ���
		// ��Ƭ�ļ�ʱ����������λ��Сʱ��   iAddHour  int 
		sc_Start = STRSTR(pString,";A1");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->iAddHour = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}


		//'1'  ʹ�ü�Ȩ��λ����ʹ�õ��ŵĽӿڣ�, '0' : �����ü�Ȩ��λ (����ʹ�õ��ŵĽӿ�)   cCTAuth  char  
		sc_Start = STRSTR(pString,";AA");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			STRNCPY(&(pMe->cCTAuth),sc_Start+3,si_Count);
		}
		
		// 
		// ������Ϣ��������   nKeepDays  int 
		sc_Start = STRSTR(pString,";PD");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->nKeepDays = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// 
		// ʹ�ø�����λʱ�Ķ�λ����,��λ��    iAssInterval  int 
		sc_Start = STRSTR(pString,";GM-");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 4;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+4,si_Count);
			pMe->iAssInterval = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
		}

		// sPhotoPath ��Ƭ·��  sVideoPath  ��Ƶ��ŵ�·��
		// �ֻ�����Ƭ�Ĵ洢Ŀ¼����Ƶ�ļ���Ŀ¼����ƬĿ¼����ƵĿ¼֮����","�ָ�
		sc_Start = STRSTR(pString,";PL");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(100);
			MEMSET(sc_tmp,0x00,100);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			sc_Start = STRSTR(sc_tmp,",");
			if(sc_Start != NULL)
			{
				MEMSET(pMe->sPhotoPath,0x00,sizeof(pMe->sPhotoPath));
				STRNCPY(pMe->sPhotoPath,sc_tmp,sc_Start-sc_tmp);

				sc_tmp1 = (char *)MALLOC(100);
				MEMSET(sc_tmp1,0x00,100);
				STRNCPY(sc_tmp1,sc_Start+1,si_Count - (sc_Start -sc_tmp + 1));
				
				sc_Start = STRSTR(sc_tmp1,",");
				if (sc_Start != NULL)
				{
					MEMSET(pMe->sVideoPath,0x00,sizeof(pMe->sVideoPath));
					STRNCPY(pMe->sVideoPath,sc_tmp1,sc_Start-sc_tmp1);

					MEMSET(pMe->sAudRecPath,0x00,sizeof(pMe->sAudRecPath));
					STRCPY(pMe->sAudRecPath,sc_Start+1); //��֪�������᲻��������
				}
				else
				{
					MEMSET(pMe->sVideoPath,0x00,sizeof(pMe->sVideoPath));
					STRCPY(pMe->sVideoPath,sc_tmp1);
				}

				
			
			}
			else //Ĭ��ֻ����Ƭ·��  ��ʵ��������ǲ����ڵ�
			{
				MEMSET(pMe->sPhotoPath,0x00,sizeof(pMe->sPhotoPath));
				STRCPY(pMe->sPhotoPath,sc_tmp);
			}

			FREE(sc_tmp1);
			FREE(sc_tmp);
			sc_tmp = NULL;
			sc_tmp1 = NULL;
			
			
		}

		// ���������
		sc_Start = STRSTR(pString,";SN");
		if(sc_Start != NULL)
		{

			// ˵��������	
			cTaskApp *me_t;

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(10);
			MEMSET(sc_tmp,0x00,10);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			pMe->m_TaskC = ATOI(sc_tmp);
			FREE(sc_tmp);
			sc_tmp = NULL;
			pMe->m_Task_F++ ;// task_first ��������ǲ��ǳ����������һ�ν�����������ǵĻ���������ǰ������û��������ǰ��

			cTaskApp_HandleEvent(pMe,EVT_APP_START,20,0);
			me_t = pMe->pTaskApp;	
			cTaskApp_TaskInteract(me_t);

			

// 			//#��̨ģʽ��������#	��̨����ʱ���и������񣬵�����ʾ�Ƿ�ʼ������ 2016-1-21
 			uf_RecLog(pMe,"#��̨ģʽ �յ������� 4 #");

 			ISHELL_SetTimer(pMe->a.m_pIShell, TASK_SET_TIMER*1000, (PFNNOTIFY)Timer_Task_Notify,pMe);

		}

		// һ�����еĶԽ�����
		sc_Start = STRSTR(pString,";IP");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);

		   MEMSET(pMe->m_IP,0x00,256);
		   STRCPY(pMe->m_IP,sc_tmp);


			FREE(sc_tmp);
			sc_tmp = NULL;

			uf_RecLog(pMe,"IP ��ַ");
			uf_RecLog(pMe,pMe->m_IP);
		}

		// һ�����еĶԽ�����
		sc_Start = STRSTR(pString,";QA");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
	
			richptt_SetUsrGroup(pMe->pRichptt,sc_tmp);

		   FREEIF(pMe->sUsrGroup);
		   pMe->sUsrGroup = (char *)MALLOC((si_Count + 5)*sizeof(char));
		   MEMSET(pMe->sUsrGroup,0,(si_Count + 5)*sizeof(char));
		   STRCPY(pMe->sUsrGroup,sc_tmp);


			FREE(sc_tmp);
			sc_tmp = NULL;

			
			
		}


		// һ�����еĶԽ�����
		sc_Start = STRSTR(pString,";QE");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);


			FREEIF(pMe->sPttCallNo);
		   pMe->sPttCallNo = (char *)MALLOC((si_Count+5)*sizeof(char));
		   MEMSET(pMe->sPttCallNo,0,(si_Count+5)*sizeof(char));
		   MEMCPY(pMe->sPttCallNo,sc_Start+3,si_Count);
		   ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, EVT_SETPTTCALL_EVENT,(uint16)0, 0);

			uf_RecLog(pMe,pMe->sPttCallNo);

			FREE(sc_tmp);
			sc_tmp = NULL;						
		}

		// �ڰ����� 
		sc_Start = STRSTR(pString,";QG");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
			richPtt_SetCallRestInfo(pMe->pRichptt,sc_tmp);

			FREE(sc_tmp);
			sc_tmp = NULL;

			
			
		}


		// Qchat���� 
		sc_Start = STRSTR(pString,";QB");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
			richptt_SetDomain(pMe->pRichptt,sc_tmp);

			FREE(sc_tmp);
			sc_tmp = NULL;

			
			
		}

		// һ�����еİ���ֵ
		sc_Start = STRSTR(pString,";QF");
		if(sc_Start != NULL)
		{

			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+3,si_Count);
			
			pMe->nPttkey = STRTOUL( sc_tmp, (char **)&pszDelimiter, 10 );

			FREE(sc_tmp);
			sc_tmp = NULL;		
		}


			// ͨѶ¼ͬ��
		sc_Start = STRSTR(pString,";QCA");
		if(sc_Start != NULL)
		{
			crichAddr *me;
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 4;
			sc_tmp = (char *)MALLOC(si_Count + 5);
			MEMSET(sc_tmp,0x00,si_Count + 5);
			STRNCPY(sc_tmp,sc_Start+4,si_Count);
			
			addbook_init(pMe);
			
			me = pMe->pRichAddr;
			me->pstrRecU = (char *)MALLOC(si_Count+5);	
			MEMSET(me->pstrRecU,0x00,si_Count+5);
			STRCPY(me->pstrRecU,sc_tmp);

			uf_RecLog(pMe,me->pstrRecU);

			if (me->pstrRecD != NULL) 
			{
				int iLen = si_Count+5 + STRLEN(me->pstrRecD) + 1 ;
				me->pstrRecD = (char *)REALLOC(me->pstrRecD,(iLen+1));
				STRCAT(me->pstrRecD,DEFAULT_SPLIT);
				STRCAT(me->pstrRecD,me->pstrRecU);
			}

			Add_addbook(pMe);
			addbook_free(pMe);

			FREE(sc_tmp1);
			FREE(sc_tmp);
			sc_tmp = NULL;
			sc_tmp1 = NULL;
			
			
		}

		//  
		//������ʱʹ�õ�ͼƬ����ţ� 0��    �á��ճ��������س���  cPicMode   
		sc_Start = STRSTR(pString,";PC");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			STRNCPY(&(pMe->cPicMode),sc_Start+3,si_Count);
		}


		//	QEZA=;
		//	QFZA=; ����������û�н���
		//	
		//�Ƿ�����ʹ�� ��1#�� ���˳�  0 �������� , 1 ����    cCanExit char
		sc_Start = STRSTR(pString,";AD");
		if(sc_Start != NULL)
		{
			sc_End = STRSTR(sc_Start+2,";");
			si_Count = sc_End - sc_Start - 3;
			STRNCPY(&(pMe->cCanExit),sc_Start+3,si_Count);
		}

		sc_End = NULL;
		sc_Start =NULL;

	}

	
	return si_ret;

	
}


boolean SendGPSData( newgimis *pMe )
{
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	char sc_buf1[10];
	char sc_buf2[10];
	

	AECHAR wcText[MAXTEXTLEN];
//	char		 s_sUrl[250];  //�������͵���ַ ��ʱ���ڷ��� GPS����
	
	
	GETJULIANDATE(0,&dDate);		//	ȡ������


	
	pMe->m_GPSGroup++;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," pMe->m_sendGPSGroup = %d",pMe->m_GPSGroup); 

	if (pMe->m_GPSGroup > pMe->nArySize) // �������Ŀ���Ƿ�ֹ�Ӹ������ĳɵ�������ʱ�������
	{
		pMe->m_GPSGroup = pMe->nArySize;
	}

//	MEMSET(sDateTime,0,sizeof(sDateTime));
	// yyyy-mm-dd hh:MM:ss    ��19���ֽ�
//   SPRINTF(sDateTime,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	pMe->i_HttpMod = 2; //HTTP���շ�������ģʽ ����	1 - ������ջ�������������Ȩ�� 2 - ���ջ�������GPS�����ϴ�����������
	
	MEMSET(pMe->sUrl,0x00,250);

	if(pMe->m_GPSGroup == 1)
	{

		SPRINTF(pMe->sDateTime1,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		FLOATTOWSTR( FABS(pMe->lat), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->latStr1, MAXTEXTLEN );
		ud_Trim(pMe->latStr1);	

		 // ����  			
		FLOATTOWSTR( FABS(pMe->lon), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->lonStr1, MAXTEXTLEN );
		ud_Trim(pMe->lonStr1);

		//ˮƽ�ٶ�
		pMe->dVelHor1 = pMe->dVelHor;

		STRCPY(pMe->lx_lon,pMe->lonStr1);
		STRCPY(pMe->lx_lat,pMe->latStr1);
		Send_PM(pMe,1);

		//////////  ����ǵ�һ�����������ͱ���ȥ START ///////////
		if (pMe->m_Fix_C == 1)
		{
//			char sc_buf1[10];
//			char sc_buf2[10];
			SPRINTF(pMe->show_Time,"%02d:%02d:%02d",dDate.wHour,dDate.wMinute,dDate.wSecond);
			if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
			{
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s;LA%s;VV%d;EP0;KS0;PM%c;TT%s",pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,pMe->lonStr1,pMe->latStr1,pMe->dVelHor1,pMe->m_PM1,pMe->sDateTime1);
			}
			else
			{
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&imsi=%s&pp=LO%s;LA%s;VV%d;EP0;KS0;PM%c;TT%s",pMe->strCenterIp,pMe->szMobileID,pMe->lonStr1,pMe->latStr1,pMe->dVelHor1,pMe->m_PM1,pMe->sDateTime1);
			}


		//	SPRINTF(pMe->sUrl,"http://%s&ver=2.6.8&pp=LO%s,%s,%s;LA%s,%s,%s;VV%d,0,0;EP0,0,0;KS0,0,0;PM0,0,0;TT%s,%s,%s&tel=%s",pMe->strCenterIp,pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->MyMobileNo);

		//	SPRINTF(pMe->sUrl,"http://cn.richtalk.cn:80/brewGPS/BrewServlet?act=httpdec&ver=2.6.8&pp=LO%s,%s,%s;LA%s,%s,%s;VV%d,0,0;EP0,0,0;KS0,0,0;PM0,0,0;TT%s,%s,%s&tel=%s",pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->MyMobileNo);

			MEMSET(sc_buf1,0x00,10);
			MEMSET(sc_buf2,0x00,10);
			STRNCPY(sc_buf1,pMe->lonStr1,8);
			STRNCPY(sc_buf2,pMe->latStr1,7);
			SPRINTF(pMe->show_LatLon,"%s   %s",sc_buf1,sc_buf2);


//			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"ֻ��һ��"); 
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),pMe->sUrl); 

/*
			if (pMe->m_MUTEX == 1)
			{
	//			uf_RecLog(pMe,"������ȥ 1");
				uf_RecLog(pMe,"HTTP not idle 1");
				return TRUE;
			}
*/
			pMe->m_MUTEX = 2;
		//	myiweb_Stop_s(pMe);
		//	myiweb_Start_s(pMe,pMe->sUrl,0);

			
			if ((pMe->m_F_QF == FALSE) && pMe->m_F_R_GPS != 1)
			{
				myiweb_Stop_gps(pMe);
				myiweb_Start_gps(pMe,pMe->sUrl,0);
				pMe->m_GPSGroup = 0;
			}
			else
			{
				if (pMe->mux_7Day == 0)
				{
					write_GPSData_7Day(pMe, pMe->sUrl);
				}
			}

			return TRUE;
		}
		//////////               END                   ///////////
		/*
		MEMSET(pMe->sUrl_yjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s&tel=%s",pMe->strCenterIp,pMe->lonStr1,pMe->latStr1,pMe->sDateTime1,pMe->MyMobileNo);
		}
		else
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s",pMe->strCenterIp,pMe->lonStr1,pMe->latStr1,pMe->sDateTime1);
		}
		*/
		if (pMe->nArySize  == 1)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}

		
	
	}
	else if(pMe->m_GPSGroup == 2)
	{
		SPRINTF(pMe->sDateTime2,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		FLOATTOWSTR( FABS(pMe->lat), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->latStr2, MAXTEXTLEN );
		ud_Trim(pMe->latStr2);	

		uf_RecLog(pMe,"���� m_GPSGroup =2 1");

		 // ����  			
		FLOATTOWSTR( FABS(pMe->lon), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->lonStr2, MAXTEXTLEN );
		ud_Trim(pMe->lonStr2);

		//ˮƽ�ٶ�
		pMe->dVelHor2 = pMe->dVelHor;

		uf_RecLog(pMe,"���� m_GPSGroup =2 2");
		STRCPY(pMe->lx_lon,pMe->lonStr2);
		STRCPY(pMe->lx_lat,pMe->latStr2);
		Send_PM(pMe,2);
		uf_RecLog(pMe,"���� m_GPSGroup =2 3");

/*		MEMSET(pMe->sUrl_yjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s&tel=%s",pMe->strCenterIp,pMe->lonStr2,pMe->latStr2,pMe->sDateTime2,pMe->MyMobileNo);
		}
		else
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s",pMe->strCenterIp,pMe->lonStr2,pMe->latStr2,pMe->sDateTime2);
		}
*/
		if (pMe->nArySize  == 2)
		{


			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	
	}
	else if (pMe->m_GPSGroup == 3)
	{
//		char sc_buf1[10];
//		char sc_buf2[10];
		SPRINTF(pMe->sDateTime3,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		FLOATTOWSTR( FABS(pMe->lat), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->latStr3, MAXTEXTLEN );
		ud_Trim(pMe->latStr3);	

		 // ����  			
		FLOATTOWSTR( FABS(pMe->lon), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->lonStr3, MAXTEXTLEN );
		ud_Trim(pMe->lonStr3);	

		//ˮƽ�ٶ�
		pMe->dVelHor3 = pMe->dVelHor;

		STRCPY(pMe->lx_lon,pMe->lonStr3);
		STRCPY(pMe->lx_lat,pMe->latStr3);

/*	
		MEMSET(sc_buf1,0x00,10);
		MEMSET(sc_buf2,0x00,10);
		STRNCPY(sc_buf1,pMe->lx_lon,8);
		STRNCPY(sc_buf2,pMe->lx_lat,7);
		SPRINTF(pMe->show_LatLon,"%s   %s",sc_buf1,sc_buf2);
*/
		Send_PM(pMe,3);
		/*
		MEMSET(pMe->sUrl_yjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s&tel=%s",pMe->strCenterIp,pMe->lonStr3,pMe->latStr3,pMe->sDateTime3,pMe->MyMobileNo);
		}
		else
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s",pMe->strCenterIp,pMe->lonStr3,pMe->latStr3,pMe->sDateTime3);
		}
		*/

		if (pMe->nArySize  == 3)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
//		pMe->m_GPSGroup = 0;
	}



	else if(pMe->m_GPSGroup == 4)
	{
		SPRINTF(pMe->sDateTime4,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		FLOATTOWSTR( FABS(pMe->lat), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->latStr4, MAXTEXTLEN );
		ud_Trim(pMe->latStr4);	

		 // ����  			
		FLOATTOWSTR( FABS(pMe->lon), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->lonStr4, MAXTEXTLEN );
		ud_Trim(pMe->lonStr4);

		//ˮƽ�ٶ�
		pMe->dVelHor4 = pMe->dVelHor;

		STRCPY(pMe->lx_lon,pMe->lonStr4);
		STRCPY(pMe->lx_lat,pMe->latStr4);
		Send_PM(pMe,9);

/*		MEMSET(pMe->sUrl_yjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s&tel=%s",pMe->strCenterIp,pMe->lonStr2,pMe->latStr2,pMe->sDateTime2,pMe->MyMobileNo);
		}
		else
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s",pMe->strCenterIp,pMe->lonStr2,pMe->latStr2,pMe->sDateTime2);
		}
*/
		if (pMe->nArySize  == 4)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	
	}
	else if(pMe->m_GPSGroup == 5)
	{
		SPRINTF(pMe->sDateTime5,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		FLOATTOWSTR( FABS(pMe->lat), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->latStr5, MAXTEXTLEN );
		ud_Trim(pMe->latStr5);	

		 // ����  			
		FLOATTOWSTR( FABS(pMe->lon), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->lonStr5, MAXTEXTLEN );
		ud_Trim(pMe->lonStr5);

		//ˮƽ�ٶ�
		pMe->dVelHor5 = pMe->dVelHor;

		STRCPY(pMe->lx_lon,pMe->lonStr5);
		STRCPY(pMe->lx_lat,pMe->latStr5);
		Send_PM(pMe,5);

/*		MEMSET(pMe->sUrl_yjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s&tel=%s",pMe->strCenterIp,pMe->lonStr2,pMe->latStr2,pMe->sDateTime2,pMe->MyMobileNo);
		}
		else
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s",pMe->strCenterIp,pMe->lonStr2,pMe->latStr2,pMe->sDateTime2);
		}
*/
		if (pMe->nArySize  == 5)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	
	}
	else if(pMe->m_GPSGroup == 6)
	{
		SPRINTF(pMe->sDateTime6,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		FLOATTOWSTR( FABS(pMe->lat), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->latStr6, MAXTEXTLEN );
		ud_Trim(pMe->latStr6);	

		 // ����  			
		FLOATTOWSTR( FABS(pMe->lon), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->lonStr6, MAXTEXTLEN );
		ud_Trim(pMe->lonStr6);

		//ˮƽ�ٶ�
		pMe->dVelHor6 = pMe->dVelHor;

		STRCPY(pMe->lx_lon,pMe->lonStr6);
		STRCPY(pMe->lx_lat,pMe->latStr6);
		Send_PM(pMe,6);

/*		MEMSET(pMe->sUrl_yjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s&tel=%s",pMe->strCenterIp,pMe->lonStr2,pMe->latStr2,pMe->sDateTime2,pMe->MyMobileNo);
		}
		else
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s",pMe->strCenterIp,pMe->lonStr2,pMe->latStr2,pMe->sDateTime2);
		}
*/
		if (pMe->nArySize  == 6)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	
	}
	else if(pMe->m_GPSGroup == 7)
	{
		SPRINTF(pMe->sDateTime7,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		FLOATTOWSTR( FABS(pMe->lat), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->latStr7, MAXTEXTLEN );
		ud_Trim(pMe->latStr7);	

		 // ����  			
		FLOATTOWSTR( FABS(pMe->lon), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->lonStr7, MAXTEXTLEN );
		ud_Trim(pMe->lonStr7);

		//ˮƽ�ٶ�
		pMe->dVelHor7 = pMe->dVelHor;

		STRCPY(pMe->lx_lon,pMe->lonStr7);
		STRCPY(pMe->lx_lat,pMe->latStr7);
		Send_PM(pMe,7);

/*		MEMSET(pMe->sUrl_yjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s&tel=%s",pMe->strCenterIp,pMe->lonStr2,pMe->latStr2,pMe->sDateTime2,pMe->MyMobileNo);
		}
		else
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s",pMe->strCenterIp,pMe->lonStr2,pMe->latStr2,pMe->sDateTime2);
		}
*/
		if (pMe->nArySize  == 7)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	
	}
	else if(pMe->m_GPSGroup == 8)
	{
		SPRINTF(pMe->sDateTime8,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		FLOATTOWSTR( FABS(pMe->lat), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->latStr8, MAXTEXTLEN );
		ud_Trim(pMe->latStr8);	

		 // ����  			
		FLOATTOWSTR( FABS(pMe->lon), wcText, MAXTEXTLEN * sizeof(AECHAR) );
		WSTR_TO_STR( wcText, pMe->lonStr8, MAXTEXTLEN );
		ud_Trim(pMe->lonStr8);

		//ˮƽ�ٶ�
		pMe->dVelHor8 = pMe->dVelHor;

		STRCPY(pMe->lx_lon,pMe->lonStr8);
		STRCPY(pMe->lx_lat,pMe->latStr8);
		Send_PM(pMe,8);

/*		MEMSET(pMe->sUrl_yjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s&tel=%s",pMe->strCenterIp,pMe->lonStr2,pMe->latStr2,pMe->sDateTime2,pMe->MyMobileNo);
		}
		else
		{
			SPRINTF(pMe->sUrl_yjkq,"http://%s&ver=2.6.8&pp=LO%s;LA%s;VV%d;EP0;KS2;PM0;TT%s",pMe->strCenterIp,pMe->lonStr2,pMe->latStr2,pMe->sDateTime2);
		}
*/
		if (pMe->nArySize  == 8)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	
	}

/////  ������ĵط�

	MEMSET(sc_buf1,0x00,10);
	MEMSET(sc_buf2,0x00,10);
	STRNCPY(sc_buf1,pMe->lx_lon,8);
	STRNCPY(sc_buf2,pMe->lx_lat,7);
	SPRINTF(pMe->show_LatLon,"%s   %s",sc_buf1,sc_buf2);

	SPRINTF(pMe->show_Time,"%02d:%02d:%02d",dDate.wHour,dDate.wMinute,dDate.wSecond);


	if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
	{
		switch(pMe->nArySize)
		{
		case 1:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s;LA%s;VV%d;EP0;KS0;PM%c;TT%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,pMe->lonStr1,pMe->latStr1,pMe->dVelHor1,pMe->m_PM1,pMe->sDateTime1);
			break;
		case 2:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s;LA%s,%s;VV%d,%d;EP0,0;KS0,0;PM%c,%c;TT%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,pMe->lonStr1,pMe->lonStr2,pMe->latStr1,pMe->latStr2,pMe->dVelHor1,pMe->dVelHor2,pMe->m_PM1,pMe->m_PM2,pMe->sDateTime1,pMe->sDateTime2);
			break;
		case 3:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s,%s;LA%s,%s,%s;VV%d,%d,%d;EP0,0,0;KS0,0,0;PM%c,%c,%c;TT%s,%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,\
				pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3);
			break;
		case 4:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s,%s,%s;LA%s,%s,%s,%s;VV%d,%d,%d,%d;EP0,0,0,0;KS0,0,0,0;PM%c,%c,%c,%c;TT%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,\
				pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,pMe->m_PM4,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4);
			break;
		case 5:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d;EP0,0,0,0,0;KS0,0,0,0,0;PM%c,%c,%c,%c,%c;TT%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,\
				pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,pMe->m_PM4,pMe->m_PM5,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5);
			break;
		case 6:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d,%d;EP0,0,0,0,0,0;KS0,0,0,0,0,0;PM%c,%c,%c,%c,%c,%c;TT%s,%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,pMe->lonStr6,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,pMe->latStr6,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,pMe->dVelHor6,\
				pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,pMe->m_PM4,pMe->m_PM5,pMe->m_PM6,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5,pMe->sDateTime6);
			break;
		case 7:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s,%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d,%d,%d;EP0,0,0,0,0,0,0;KS0,0,0,0,0,0,0;PM%c,%c,%c,%c,%c,%c,%c;TT%s,%s,%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,pMe->lonStr6,pMe->lonStr7,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,pMe->latStr6,pMe->latStr7,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,pMe->dVelHor6,pMe->dVelHor7,\
				pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,pMe->m_PM4,pMe->m_PM5,pMe->m_PM6,pMe->m_PM7,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5,pMe->sDateTime6,pMe->sDateTime7);
			break;
		case 8:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s,%s,%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d,%d,%d,%d;EP0,0,0,0,0,0,0,0;KS0,0,0,0,0,0,0,0;PM%c,%c,%c,%c,%c,%c,%c,%c;TT%s,%s,%s,%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,pMe->lonStr6,pMe->lonStr7,pMe->lonStr8,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,pMe->latStr6,pMe->latStr7,pMe->latStr8,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,pMe->dVelHor6,pMe->dVelHor7,pMe->dVelHor8,\
				pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,pMe->m_PM4,pMe->m_PM5,pMe->m_PM6,pMe->m_PM7,pMe->m_PM8,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5,pMe->sDateTime6,pMe->sDateTime7,pMe->sDateTime8);
			break;
		}
	}
	else
	{
		switch(pMe->nArySize)
		{
		case 1:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s;LA%s;VV%d;EP0;KS0;PM%c;TT%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->latStr1,pMe->dVelHor1,pMe->m_PM1,pMe->sDateTime1);
			break;
		case 2:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s;LA%s,%s;VV%d,%d;EP0,0;KS0,0;PM%c,%c;TT%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->latStr1,pMe->latStr2,pMe->dVelHor1,pMe->dVelHor2,pMe->m_PM1,pMe->m_PM2,pMe->sDateTime1,pMe->sDateTime2);
			break;
		case 3:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s,%s;LA%s,%s,%s;VV%d,%d,%d;EP0,0,0;KS0,0,0;PM%c,%c,%c;TT%s,%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,\
				pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3);
			break;
		case 4:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s,%s,%s;LA%s,%s,%s,%s;VV%d,%d,%d,%d;EP0,0,0,0;KS0,0,0,0;PM%c,%c,%c,%c;TT%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,\
				pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,pMe->m_PM4,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4);
			break;
		case 5:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d;EP0,0,0,0,0;KS0,0,0,0,0;PM%c,%c,%c,%c,%c;TT%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,\
				pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,pMe->m_PM4,pMe->m_PM5,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5);
			break;
		case 6:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d,%d;EP0,0,0,0,0,0;KS0,0,0,0,0,0;PM%c,%c,%c,%c,%c.%c;TT%s,%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,pMe->lonStr6,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,pMe->latStr6,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,pMe->dVelHor6,\
				pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,pMe->m_PM4,pMe->m_PM5,pMe->m_PM6,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5,pMe->sDateTime6);
			break;
		case 7:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s,%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d,%d,%d;EP0,0,0,0,0,0,0;KS0,0,0,0,0,0,0;PM%c,%c,%c,%c,%c,%c,%c;TT%s,%s,%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,pMe->lonStr6,pMe->lonStr7,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,pMe->latStr6,pMe->latStr7,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,pMe->dVelHor6,pMe->dVelHor7,\
				pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,pMe->m_PM4,pMe->m_PM5,pMe->m_PM6,pMe->m_PM7,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5,pMe->sDateTime6,pMe->sDateTime7);
			break;
		case 8:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s,%s,%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d,%d,%d,%d;EP0,0,0,0,0,0,0,0;KS0,0,0,0,0,0,0,0;PM%c,%c,%c,%c,%c,%c,%c,%c;TT%s,%s,%s,%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,pMe->lonStr6,pMe->lonStr7,pMe->lonStr8,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,pMe->latStr6,pMe->latStr7,pMe->latStr8,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,pMe->dVelHor6,pMe->dVelHor7,pMe->dVelHor8,\
				pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,pMe->m_PM4,pMe->m_PM5,pMe->m_PM6,pMe->m_PM7,pMe->m_PM8,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5,pMe->sDateTime6,pMe->sDateTime7,pMe->sDateTime8);
			break;
		}
//		SPRINTF(pMe->sUrl,"http://%s&ver=3.0.1w&pp=LO%s,%s,%s;LA%s,%s,%s;VV%d,0,0;EP0,0,0;KS0,0,0;PM%c,%c,%c;TT%s,%s,%s",pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3);
	}


//	SPRINTF(pMe->sUrl,"http://%s&ver=2.6.8&pp=LO%s,%s,%s;LA%s,%s,%s;VV%d,0,0;EP0,0,0;KS0,0,0;PM0,0,0;TT%s,%s,%s&tel=%s",pMe->strCenterIp,pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->MyMobileNo);

//	SPRINTF(pMe->sUrl,"http://cn.richtalk.cn:80/brewGPS/BrewServlet?act=httpdec&ver=2.6.8&pp=LO%s,%s,%s;LA%s,%s,%s;VV%d,0,0;EP0,0,0;KS0,0,0;PM0,0,0;TT%s,%s,%s&tel=%s",pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->MyMobileNo);


	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),pMe->sUrl); 

	/*
	if (pMe->m_MUTEX == 1)
	{
		uf_RecLog(pMe,"HTTP not idle 2");

		return TRUE;
	}
	*/
	pMe->m_MUTEX = 2;

//	myiweb_Stop_s(pMe);
//	myiweb_Start_s(pMe,pMe->sUrl,0);

	if ((pMe->m_F_QF == FALSE) && (pMe->m_F_R_GPS != 1))
	{
		myiweb_Stop_gps(pMe);
		myiweb_Start_gps(pMe,pMe->sUrl,0);
	}
	else
	{
		if (pMe->mux_7Day == 0)
		{
			write_GPSData_7Day(pMe, pMe->sUrl);
		}
	}

	return TRUE;
}




boolean SendErrorGPSData( newgimis *pMe )
{
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��

//	char		 s_sUrl[250];  //�������͵���ַ ��ʱ���ڷ��� GPS����

	////////////////// 2015-10-21 START ////////////////
	int si_ret = 0;
	si_ret = judgeSendorCancel( pMe ); // �ж��Ƿ��� myiweb_start_xt ����ȡ������   1 - ��ʾ����   0 - ��ʾ������
	if (si_ret == 0) // ��ʾ������
	{
		uf_RecLog(pMe,"judgeSendorCancel exit");
		return TRUE;
	}
	/////////////////////    END     ///////////////////////////////
	
	
	GETJULIANDATE(0,&dDate);		//	ȡ������
	


//	MEMSET(sDateTime,0,sizeof(sDateTime));
	// yyyy-mm-dd hh:MM:ss    ��19���ֽ�
//   SPRINTF(sDateTime,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	pMe->i_HttpMod = 2; //HTTP���շ�������ģʽ ����	1 - ������ջ�������������Ȩ�� 2 - ���ջ�������GPS�����ϴ�����������
	
	MEMSET(pMe->sUrl,0x00,1024);

	SPRINTF(pMe->sDateTime1,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);

	Send_PM(pMe,4);

	

	if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
	{
		SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO0x100004;LA0x100004;VV0;EP0;KS0;PM%c;TT%s",pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,pMe->m_ycPM,pMe->sDateTime1);
	}
	else
	{
		SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&imsi=%s&pp=LO0x100004;LA0x100004;VV0;EP0;KS0;PM%c;TT%s",pMe->strCenterIp,pMe->szMobileID,pMe->m_ycPM,pMe->sDateTime1);
	}

	/*
	if (pMe->m_MUTEX == 2)
	{
		uf_RecLog(pMe,"HTTP not idle 3");
		return TRUE;
	}
	*/
	pMe->m_MUTEX = 1;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),pMe->sUrl); 

//	myiweb_Stop_s(pMe);
//	myiweb_Start_s(pMe,pMe->sUrl,0);

	MEMSET(pMe->show_Time,0x00,10); //����ڽ�������ʾ��ʱ��

	SPRINTF(pMe->show_Time,"%02d:%02d:%02d",dDate.wHour,dDate.wMinute,dDate.wSecond);
	SPRINTF(pMe->show_LatLon,"0x100004   0x100004");

	if (pMe->m_F_QF == FALSE)
	{
	
//		myiweb_Stop_gps(pMe);
//		myiweb_Start_gps(pMe,pMe->sUrl,0);	 // ���������滻�������

		uf_RecLog(pMe,"Send error GPS data");
/*
		if (pMe->m_F_R_C == 0)
		{
			myiweb_Stop_xt(pMe);
			myiweb_Start_xt(pMe,pMe->sUrl,0);  //����ȥ��Ҫȫ�ֱ���
		}
*/
		if (pMe->m_F_R_E == 0)
		{
		//	myiweb_Stop_E(pMe);
		//	myiweb_Start_E(pMe,pMe->sUrl,0);  //����ȥ��Ҫȫ�ֱ���	
			
			////////////////////////GPS�������ݰ�//////////////////////////////////
			myiweb_start_E_cb(pMe);
			////////////////////////////////////�޸Ĳ���/////////////////////////////
		}
	}
	else
	{
		if (pMe->mux_7Day == 0)
		{
			write_GPSData_7Day(pMe, pMe->sUrl);
		}
	}


	return TRUE;
}


void myiweb_start_E_cb(void *p)//��ʱ���Ѳ���
{	
	newgimis *pMe = (newgimis *)p;

	

	if (0 == pMe->m_Mutex_offgps)
	{
	//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"err gps sent 100004  m_Mutex_offgps = %d ",  pMe->m_Mutex_offgps);
//		pMe->m_Mutex_offgps = 1; 
//		pMe->m_bFlg_errG = 0;

		myiweb_Stop_E(pMe);
		myiweb_Start_E(pMe,pMe->sUrl,0);  //����ȥ��Ҫȫ�ֱ���
		return;

	}else{ //���ڷ�������GPS

	//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"err gps sent 100004 waiting...ISHELL_SetTimer(1s).  m_Mutex_offgps = %d ",  pMe->m_Mutex_offgps);

//		pMe->m_bFlg_errG = 1; //��־ ֪ͨ����GPSֹͣռ��Http
		
		//��ʱx����ٴ�������
		ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY)myiweb_start_E_cb, pMe);
	}
	return;
}




boolean Send_Offline_GPSData(newgimis *pMe)

{
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��

	char *chTmp = NULL;
	char *url = NULL;
	int ret;
	////////////////// 2015-10-21 START ////////////////
	int si_ret = 0;


	/////////////////////    END     ///////////////////////////////
	
	
//	GETJULIANDATE(0,&dDate);		//	ȡ������
	


//	MEMSET(sDateTime,0,sizeof(sDateTime));
	// yyyy-mm-dd hh:MM:ss    ��19���ֽ�
//   SPRINTF(sDateTime,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	 
	

//	SPRINTF(pMe->sDateTime1,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);



	url = (char*)MALLOC(1024);
	MEMSET(url,0x00,1024);
	ret = File_Get_OfflineData_Gps(pMe, &url);
//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"File_Get_OfflineData_Gps: %s", url);
	if (0 == ret)
	{
		//get�������ݣ��������������
		pMe->m_offlineData_gps = 0;
		
		//����Ҫ�Ļر�־λ��...
		pMe->m_Mutex_offgps = 0;

// 		//��ǰû���������ݵ�ʱ�� �������ļ�����������Ŀ
// 		pMe->m_RofflineData_Num = pMe->m_WofflineData_Num;
// 		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Send_Offline_GPSData pMe->m_RofflineData_Num = %d",pMe->m_RofflineData_Num);

		return FALSE;
	}

	//1.����pMe->sUrl
	MEMSET(pMe->sUrl,0x00,1024);

	SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au",pMe->strCenterIp);
//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"pMe->strCenterIp: %s", pMe->strCenterIp);
//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"pMe->sUrl  <- pMe->strCenterIp: %s", pMe->sUrl);

	chTmp = STRSTR(url, "&pp");	
//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"&pp  : %s", chTmp);

	STRCAT(pMe->sUrl, chTmp);
//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"Send_Offline_GPSData: %s", pMe->sUrl);
//	uf_RecLog(pMe,"�Ҵ�ӡ��");
	uf_RecLog(pMe,pMe->sUrl);

	FREE(url);
	url = NULL;
	



	if (pMe->m_F_QF == FALSE)
	{
	
//		myiweb_Stop_gps(pMe);
//		myiweb_Start_gps(pMe,pMe->sUrl,0);	 // ���������滻�������

//		uf_RecLog(pMe,"Send Offline GPS data");
/*
		if (pMe->m_F_R_C == 0)
		{
			myiweb_Stop_xt(pMe);
			myiweb_Start_xt(pMe,pMe->sUrl,0);  //����ȥ��Ҫȫ�ֱ���
		}
*/
		if (pMe->m_F_R_E == 0)
		{
			pMe->m_Mutex_offgps = 1;

			myiweb_Stop_E(pMe);
			
			myiweb_Start_E(pMe,pMe->sUrl,0);  //����ȥ��Ҫȫ�ֱ���
		//	My_Log(pMe->a.m_pIShell,'1', &(pMe->nInd),"myiweb_Start_E()");
		}
		
	
	}
/*	if ((pMe->m_offlineData_gps == 1))
	{
		ISHELL_SetTimer(pMe->a.m_pIShell, 1000,(PFNNOTIFY)Send_Offline_GPSData,  pMe);
	}
*/

	return TRUE;
}

/*
void SetTimer_SentGPS_OfflineData(void *pts)
{
	newgimis *pMe = (newgimis *)pts;

	uint32 si_tmp_t = 0;
	//	uf_RecLog(pMe,"TimetoTrack_cbInterval 1");
	if (pMe->m_MUTEX == 0)
	{
		si_tmp_t = GETTIMESECONDS();
		//		pMe->m_T_H_G = si_tmp_t;
		Track_cbInterval(  s_pts );	
		return;
	}
	//	uf_RecLog(pMe,"TimetoTrack_cbInterval 2");
	ISHELL_SetTimer(pMe->a.m_pIShell, 2000, (PFNNOTIFY)TimetoTrack_cbInterval,s_pts);
	return;
}

*/

boolean SendErrorGPSData_0( newgimis *pMe )
{
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��

//	char		 s_sUrl[250];  //�������͵���ַ ��ʱ���ڷ��� GPS����
	
	////////////////// 2015-10-21 START ////////////////
	int si_ret = 0;
	si_ret = judgeSendorCancel( pMe ); // �ж��Ƿ��� myiweb_start_xt ����ȡ������   1 - ��ʾ����   0 - ��ʾ������
	if (si_ret == 0) // ��ʾ������
	{
		return TRUE;
	}
	/////////////////////    END     ///////////////////////////////
	
	GETJULIANDATE(0,&dDate);		//	ȡ������
	


//	MEMSET(sDateTime,0,sizeof(sDateTime));
	// yyyy-mm-dd hh:MM:ss    ��19���ֽ�
//   SPRINTF(sDateTime,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	pMe->i_HttpMod = 2; //HTTP���շ�������ģʽ ����	1 - ������ջ�������������Ȩ�� 2 - ���ջ�������GPS�����ϴ�����������
	
	MEMSET(pMe->sUrl,0x00,250);

	SPRINTF(pMe->sDateTime1,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);

	Send_PM(pMe,4);

	
	

	if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
	{
		SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO0x100004;LA0x100004;VV0;EP0;KS0;PM%c;TT%s",pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,pMe->m_ycPM,pMe->sDateTime1);
	}
	else
	{
		SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&imsi=%s&pp=LO0x100004;LA0x100004;VV0;EP0;KS0;PM%c;TT%s",pMe->strCenterIp,pMe->szMobileID,pMe->m_ycPM,pMe->sDateTime1);

	}

	/*
	if (pMe->m_MUTEX == 2)
	{
		uf_RecLog(pMe,"HTTP not idle 3");
		return TRUE;
	}
	*/
	pMe->m_MUTEX = 1;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),pMe->sUrl); 

//	myiweb_Stop_s(pMe);
//	myiweb_Start_s(pMe,pMe->sUrl,0);

	MEMSET(pMe->show_Time,0x00,10); //����ڽ�������ʾ��ʱ��

	SPRINTF(pMe->show_Time,"%02d:%02d:%02d",dDate.wHour,dDate.wMinute,dDate.wSecond);
	SPRINTF(pMe->show_LatLon,"0x100001   0x100001");
	if (pMe->m_F_QF == FALSE)
	{
	
//		myiweb_Stop_gps(pMe);
//		myiweb_Start_gps(pMe,pMe->sUrl,0);	 // ���������滻�������

		uf_RecLog(pMe,"Send error GPS data");
/*	
		if (pMe->m_F_R_C == 0)
		{
			myiweb_Stop_xt(pMe);
			myiweb_Start_xt(pMe,pMe->sUrl,0);  //����ȥ��Ҫȫ�ֱ���
		}
*/
		if (pMe->m_F_R_E == 0)
		{
		//	myiweb_Stop_E(pMe);
		//	myiweb_Start_E(pMe,pMe->sUrl,0);  //����ȥ��Ҫȫ�ֱ���

			//////////////////////////////////GPS�������ݰ�//////////////////////////////
			myiweb_start_E_cb(pMe);
			////////////////////////////////////�޸Ĳ���/////////////////////////////
		}
	
	}
	else
	{
		if (pMe->mux_7Day == 0)
		{
			write_GPSData_7Day(pMe, pMe->sUrl);
		}
	}


	return TRUE;
}


/////////////////  �� �����йص�  ////////////////////////

static boolean testgps_StartSendXT( newgimis *pMe )  // ������������
{
	int si_ret = 0;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"come xt"); 


	//ֻ�����ź�֮��ſ�ʼ������
	if (pMe->m_enable_evdo != 1)	
	{
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"testgps_StartSendXT evdo init continue....");
		ISHELL_SetTimer(pMe->a.m_pIShell,5000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);
		return;
	}


	if (pMe->iUDPPORT_YYXF != 0  && pMe->sUDPIP_YYXF !=NULL && pMe->iTCPPORT_YYXF != 0  && pMe->sTCPIP_YYXF !=NULL)
	{
		//ƽ��ҽԺ����������
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"ƽ��ҽԺ-�ر���������");
		return TRUE;
	}



/*	si_ret = judgeSendorCancel( pMe ); // �ж��Ƿ��� myiweb_start_xt ����ȡ������   1 - ��ʾ����   0 - ��ʾ������
	if (si_ret == 0) // ��ʾ������
	{
		if(pMe->ipulsation < 5)
		{
			ISHELL_SetTimer(pMe->a.m_pIShell,5000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);
		}
		else
		{
			ISHELL_SetTimer(pMe->a.m_pIShell, pMe->ipulsation*1000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);	
		}	
		return TRUE;
	}
*/
//	uf_GetNetSts(pMe);					
//	if ( !uf_CheckNetSts(pMe) )		//  false ������  true ����
//	{
//		pMe->m_PosMode = TRUE;// ��������Ƿ�ֻ�д�����ģʽ  TRUE == ��ʾֻ�д�����ģʽ  FALSE == ���б��ģʽ
//		pMe->m_F_NONET = TRUE; //��������ڿ�����ʱ���û������  TRUE -- ������ʱ��û������  FALSE -- ������
//		pMe->m_Rely_XT = FALSE; //��������Ƿ�������Ҫ��������  TRUE = Ҫ��������  FALSE = ��ʾ�������Ҫ���ͣ�ֻ�ǽ�����ʱ
//	}
//	else
//	{
//		pMe->m_PosMode = FALSE;// ��������Ƿ�ֻ�д�����ģʽ  TRUE == ��ʾֻ�д�����ģʽ  FALSE == ���б��ģʽ
		pMe->m_F_NONET = FALSE; //��������ڿ�����ʱ���û������  TRUE -- ������ʱ��û������  FALSE -- ������
		pMe->m_Rely_XT = TRUE; //��������Ƿ�������Ҫ��������  TRUE = Ҫ��������  FALSE = ��ʾ�������Ҫ���ͣ�ֻ�ǽ�����ʱ
//	}
//	if (pMe->m_has_QQSQ == FALSE) // ˵����û��������Ȩ
//	{
//		pMe->m_Flag_Qqsq = 1; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����
//		pMe->m_Acc = 3;
//		myiweb_Stop_qqsq(pMe);
//		myiweb_Start_qqsq(pMe,TEST_URL,0);
//		pMe->m_has_QQSQ = TRUE;

//		if(pMe->ipulsation < 5)
//		{
//			ISHELL_SetTimer(pMe->a.m_pIShell,5000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);
//		}
//		else
//		{
//			ISHELL_SetTimer(pMe->a.m_pIShell, pMe->ipulsation*1000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);	
//		}	

//		return TRUE;
//	}
	
	if (!pMe->m_Rely_XT)
	{
		if(pMe->ipulsation < 5)
		{
			ISHELL_SetTimer(pMe->a.m_pIShell,5000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);
		}
		else
		{
			ISHELL_SetTimer(pMe->a.m_pIShell, pMe->ipulsation*1000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);	
		}	
		return TRUE;
	}

/*	if (pMe->m_F_QF) // ��ʾ�ֻ�Ƿ�� ����Ҫ����������
	{
		if(pMe->ipulsation < 5)
		{
			ISHELL_SetTimer(pMe->a.m_pIShell,5000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);
		}
		else
		{
			ISHELL_SetTimer(pMe->a.m_pIShell, pMe->ipulsation*1000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);	
		}	

		return TRUE;
	}
*/	
	// m_MUTEX; // 1 = ˵�����ܽ��з���HTTP �Ѿ���ռ����http����Դ  0= ˵������ʹ��http��Դ
	/*
	if (pMe->m_MUTEX == 2)
	{
		if(pMe->ipulsation < 5)
		{
			ISHELL_SetTimer(pMe->a.m_pIShell,5000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);
		}
		else
		{
			ISHELL_SetTimer(pMe->a.m_pIShell, pMe->ipulsation*1000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);	
		}	
		uf_RecLog(pMe,"HTTP not idle 4");
		return TRUE;
	}
	*/
	pMe->m_MUTEX = 1;  // ��HTTP����


	//  [3/4/2016 Administrator]
	MEMSET(pMe->Url_st,0x00,250);
	STRCAT(pMe->Url_st,"http://");
	STRCAT(pMe->Url_st,pMe->strCenterIp);
	STRCAT(pMe->Url_st,"&ver=3.1.2au");

	if (STRLEN(pMe->MyMobileNo) > 0) 
	{
		STRCAT(pMe->Url_st,"&tel=");
		STRCAT(pMe->Url_st,pMe->MyMobileNo);
		STRCAT(pMe->Url_st,"&imsi=");
		STRCAT(pMe->Url_st,pMe->szMobileID);
	}
	//end

	
	if (pMe->m_F_R_C == 0)
	{

		if (pMe->m_F_CD != 1)  //����Ƿ�Ϊ��� == 1 ��  == 0 ����
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),pMe->Url_st); 
			myiweb_Stop_xt(pMe);
		//	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"http://cn.richtalk.cn:80/brewGPS/BrewServlet?act=httpdec&ver=2.6.8"); 
		//		myiweb_Start_xt(pMe,"http://cn.richtalk.cn:80/brewGPS/BrewServlet?act=httpdec&ver=2.6.8",0);  //������
			myiweb_Start_xt(pMe,pMe->Url_st,0);  //����ȥ��Ҫȫ�ֱ���	
		}

	}


	if(pMe->ipulsation < 5)
	{
		ISHELL_SetTimer(pMe->a.m_pIShell,5000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);
	}
	else
	{
		ISHELL_SetTimer(pMe->a.m_pIShell, pMe->ipulsation*1000, (PFNNOTIFY) testgps_StartSendXT, (uint32*) pMe);	
	}
	
	return TRUE;
}

/*  myiweb_Start_lx ��������*/
static void myiweb_Start_lx(newgimis *pMe, char *pszUrl,int iPage)
{
   ISourceUtil *pisu;
   int    i = 0;
   WebOpt awo[10];
	pMe->m_iAppStatus_lx = 1;
   // look to see if there's POST data, this is totally non-standard, but 
   // easy to put into tests
   pMe->pszPostData_lx = STRCHR(pszUrl, 1);  // �˺��������ַ����в����ַ� ���������뵥�ֽ��ַ� 
   
   // if there's post data, construct a stream for IWeb to consume
   if ((char *)0 != pMe->pszPostData_lx) 
   {
      *pMe->pszPostData_lx = 0;
      if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pisu)) 
	  {
         
         ISOURCEUTIL_PeekFromMemory(pisu, pMe->pszPostData_lx + 1, 
                                    STRLEN(pMe->pszPostData_lx + 1), 0, 0,
                                    &pMe->pipPostData_lx);
         ISOURCEUTIL_Release(pisu);
      }
   }

   // initialize the callback, where I'll be called when the request
   // completes
   if(iPage == 0)
   {
	   CALLBACK_Cancel(&pMe->cb_lx);
	   CALLBACK_Init(&pMe->cb_lx, myiweb_GotResp_lx, pMe);
   }
   pMe->uStart_lx = GETUPTIMEMS();
   // start transaction, pass callbacks for web status, web headers
   // the extra WEBOPT_HEADER is used to help test what's sent 
   // (snoop.sh above shows all the headers)
   if(!pMe->m_pIWeb_lx)
   {
	   ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)(&pMe->m_pIWeb_lx));
	   // set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)

	   
		awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		
		// Marks the end of the array of WebOpts
		awo[i].nId  = WEBOPT_END;
		


  
		
		
		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb_lx,awo);
   }
   if ((IPeek *)0 != pMe->pipPostData_lx) {
      IWEB_GetResponse(pMe->m_pIWeb_lx,
                       (pMe->m_pIWeb_lx, &pMe->piWResp_lx, &pMe->cb_lx, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe,
                        WEBOPT_HEADER, "X-Method: POST\r\n", /* for kicks */
                        WEBOPT_HEADERHANDLER, myiweb_Header_lx, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_lx, 
                        WEBOPT_METHOD, "POST",
                        WEBOPT_BODY, pMe->pipPostData_lx,
                        WEBOPT_CONTENTLENGTH, STRLEN(pMe->pszPostData_lx + 1),
                        WEBOPT_END));
   } else {
      IWEB_GetResponse(pMe->m_pIWeb_lx,
                       (pMe->m_pIWeb_lx, &pMe->piWResp_lx, &pMe->cb_lx, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe, 
                        WEBOPT_HEADER, "X-Method: GET \r\n",
						WEBOPT_METHOD, "GET",
                        WEBOPT_HEADERHANDLER, myiweb_Header_lx, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_lx, 
                        WEBOPT_END));
   }
}




static void myiweb_Start_gps(newgimis *pMe, char *pszUrl,int iPage)
{
   ISourceUtil *pisu;
   int    i = 0;
   WebOpt awo[10];

	pMe->m_iAppStatus_gps = 1;
   // look to see if there's POST data, this is totally non-standard, but 
   // easy to put into tests 
   pMe->pszPostData_gps = STRCHR(pszUrl, 1);  // �˺��������ַ����в����ַ� ���������뵥�ֽ��ַ� 
   
   // if there's post data, construct a stream for IWeb to consume
   if ((char *)0 != pMe->pszPostData_gps) 
   {
      *pMe->pszPostData_gps = 0;
      if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pisu)) 
	  {
         
         ISOURCEUTIL_PeekFromMemory(pisu, pMe->pszPostData_gps + 1, 
                                    STRLEN(pMe->pszPostData_gps + 1), 0, 0,
                                    &pMe->pipPostData_gps);
         ISOURCEUTIL_Release(pisu);
      }
   }

   // initialize the callback, where I'll be called when the request
   // completes
   if(iPage == 0)
   {
	   CALLBACK_Cancel(&pMe->cb_gps);
	   CALLBACK_Init(&pMe->cb_gps, myiweb_GotResp_gps, pMe);
   }
   pMe->uStart_gps = GETUPTIMEMS();
   // start transaction, pass callbacks for web status, web headers
   // the extra WEBOPT_HEADER is used to help test what's sent 
   // (snoop.sh above shows all the headers)
   if(!pMe->m_pIWeb_gps)
   {
	   ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)(&pMe->m_pIWeb_gps));
	   // set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)

	   
		awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		
		// Marks the end of the array of WebOpts
		awo[i].nId  = WEBOPT_END;
		


  
		
		
		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb_gps,awo);
   }
   if ((IPeek *)0 != pMe->pipPostData_gps) {
      IWEB_GetResponse(pMe->m_pIWeb_gps,
                       (pMe->m_pIWeb_gps, &pMe->piWResp_gps, &pMe->cb_gps, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe,
                        WEBOPT_HEADER, "X-Method: POST\r\n", /* for kicks */
                        WEBOPT_HEADERHANDLER, myiweb_Header_gps, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_gps, 
                        WEBOPT_METHOD, "POST",
                        WEBOPT_BODY, pMe->pipPostData_gps,
                        WEBOPT_CONTENTLENGTH, STRLEN(pMe->pszPostData_gps + 1),
                        WEBOPT_END));
   } else {
      IWEB_GetResponse(pMe->m_pIWeb_gps,
                       (pMe->m_pIWeb_gps, &pMe->piWResp_gps, &pMe->cb_gps, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe, 
                        WEBOPT_HEADER, "X-Method: GET \r\n",
						WEBOPT_METHOD, "GET",
                        WEBOPT_HEADERHANDLER, myiweb_Header_gps, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_gps, 
                        WEBOPT_END));
   }
}



/*  myiweb_Start_slx ��������*/
static void myiweb_Start_slx(newgimis *pMe, char *pszUrl,int iPage)
{
   ISourceUtil *pisu;
   int    i = 0;
   WebOpt awo[10];
	pMe->m_iAppStatus_slx = 1;
   // look to see if there's POST data, this is totally non-standard, but 
   // easy to put into tests
   
   // if there's post data, construct a stream for IWeb to consume


	//	pMe->pszPostData_slx�����ҪPost�����ݰ�..
   if ((char *)0 != pMe->pszPostData_slx) 
   {
	   uf_RecLog(pMe,"Offline attendance come a5");
	   uf_RecLog(pMe,pszUrl);
	   uf_RecLog(pMe,pMe->pszPostData_slx);
      if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pisu)) 
	  {
         
         ISOURCEUTIL_PeekFromMemory(pisu, pMe->pszPostData_slx, 
                                    STRLEN(pMe->pszPostData_slx + 1), 0, 0,
                                    &pMe->pipPostData_slx);
         ISOURCEUTIL_Release(pisu);
      }
   }

   // initialize the callback, where I'll be called when the request
   // completes
   if(iPage == 0)
   {
	   CALLBACK_Cancel(&pMe->cb_slx);
	   CALLBACK_Init(&pMe->cb_slx, myiweb_GotResp_slx, pMe);
   }
   pMe->uStart_slx = GETUPTIMEMS();
   // start transaction, pass callbacks for web status, web headers
   // the extra WEBOPT_HEADER is used to help test what's sent 
   // (snoop.sh above shows all the headers)
   if(!pMe->m_pIWeb_slx)
   {
	   ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)(&pMe->m_pIWeb_slx));
	   // set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)

	   
		awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		
		// Marks the end of the array of WebOpts
		awo[i].nId  = WEBOPT_END;
		


  
		
		
		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb_slx,awo);
   }
   if ((IPeek *)0 != pMe->pipPostData_slx) {
      IWEB_GetResponse(pMe->m_pIWeb_slx,
                       (pMe->m_pIWeb_slx, &pMe->piWResp_slx, &pMe->cb_slx, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe,
                        WEBOPT_HEADER, "X-Method: POST\r\n", /* for kicks */
                        WEBOPT_HEADERHANDLER, NULL, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_slx, 
                        WEBOPT_METHOD, "POST",
                        WEBOPT_BODY, pMe->pipPostData_slx,
                        WEBOPT_CONTENTLENGTH, STRLEN(pMe->pszPostData_slx + 1),
                        WEBOPT_END));
   } else {
      IWEB_GetResponse(pMe->m_pIWeb_slx,
                       (pMe->m_pIWeb_slx, &pMe->piWResp_slx, &pMe->cb_slx, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe, 
                        WEBOPT_HEADER, "X-Method: GET \r\n",
						WEBOPT_METHOD, "GET",
                        WEBOPT_HEADERHANDLER, myiweb_Header_slx, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_slx, 
                        WEBOPT_END));
   }
}



/// myiweb_start_qqsq  start ////////
static void myiweb_Start_qqsq(newgimis *pMe, char *pszUrl,int iPage)
{
   ISourceUtil *pisu;
   int    i = 0;
   WebOpt awo[10];
	pMe->m_iAppStatus_qqsq = 1;
   // look to see if there's POST data, this is totally non-standard, but 
   // easy to put into tests
   pMe->pszPostData_qqsq = STRCHR(pszUrl, 1);  // �˺��������ַ����в����ַ� ���������뵥�ֽ��ַ� 
   
   // if there's post data, construct a stream for IWeb to consume
   if ((char *)0 != pMe->pszPostData_qqsq) 
   {
      *pMe->pszPostData_qqsq = 0;
      if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pisu)) 
	  {
         
         ISOURCEUTIL_PeekFromMemory(pisu, pMe->pszPostData_qqsq + 1, 
                                    STRLEN(pMe->pszPostData_qqsq + 1), 0, 0,
                                    &pMe->pipPostData_qqsq);
         ISOURCEUTIL_Release(pisu);
      }
   }

   // initialize the callback, where I'll be called when the request
   // completes
   if(iPage == 0)
   {
	   CALLBACK_Cancel(&pMe->cb_qqsq);
	   CALLBACK_Init(&pMe->cb_qqsq, myiweb_GotResp_qqsq, pMe);
   }
   pMe->uStart_qqsq = GETUPTIMEMS();
   // start transaction, pass callbacks for web status, web headers
   // the extra WEBOPT_HEADER is used to help test what's sent 
   // (snoop.sh above shows all the headers)
   if(!pMe->m_pIWeb_qqsq)
   {
	   ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)(&pMe->m_pIWeb_qqsq));
	   // set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)

	   
		awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		
		// Marks the end of the array of WebOpts
		awo[i].nId  = WEBOPT_END;

  
		
		
		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb_qqsq,awo);
   }
   if ((IPeek *)0 != pMe->pipPostData_qqsq) {
      IWEB_GetResponse(pMe->m_pIWeb_qqsq,
                       (pMe->m_pIWeb_qqsq, &pMe->piWResp_qqsq, &pMe->cb_qqsq, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe,
                        WEBOPT_HEADER, "X-Method: POST\r\n", /* for kicks */
                        WEBOPT_HEADERHANDLER, myiweb_Header_qqsq, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_qqsq, 
                        WEBOPT_METHOD, "POST",
                        WEBOPT_BODY, pMe->pipPostData_qqsq,
                        WEBOPT_CONTENTLENGTH, STRLEN(pMe->pszPostData_qqsq + 1),
                        WEBOPT_END));
   } else {
      IWEB_GetResponse(pMe->m_pIWeb_qqsq,
                       (pMe->m_pIWeb_qqsq, &pMe->piWResp_qqsq, &pMe->cb_qqsq, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe, 
                        WEBOPT_HEADER, "X-Method: GET \r\n",
						WEBOPT_METHOD, "GET",
                        WEBOPT_HEADERHANDLER, myiweb_Header_qqsq, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_qqsq, 
                        WEBOPT_END));
   }
}
//  myiweb_start_qqsq  end  /////////



static void myiweb_Start_xt(newgimis *pMe, char *pszUrl,int iPage)
{
   ISourceUtil *pisu;
   int    i = 0;
   WebOpt awo[10];
	pMe->m_iAppStatus_xt = 1;
   // look to see if there's POST data, this is totally non-standard, but 
   // easy to put into tests
   pMe->pszPostData_xt = STRCHR(pszUrl, 1);  // �˺��������ַ����в����ַ� ���������뵥�ֽ��ַ� 
   
   // if there's post data, construct a stream for IWeb to consume
   if ((char *)0 != pMe->pszPostData_xt) 
   {
      *pMe->pszPostData_xt = 0;
      if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pisu)) 
	  {
         
         ISOURCEUTIL_PeekFromMemory(pisu, pMe->pszPostData_xt + 1, 
                                    STRLEN(pMe->pszPostData_xt + 1), 0, 0,
                                    &pMe->pipPostData_xt);
         ISOURCEUTIL_Release(pisu);
      }
   }

   // initialize the callback, where I'll be called when the request
   // completes
   if(iPage == 0)
   {
	   CALLBACK_Cancel(&pMe->cb_xt);
	   CALLBACK_Init(&pMe->cb_xt, myiweb_GotResp_xt, pMe);
   }
   pMe->uStart_xt = GETUPTIMEMS();
   // start transaction, pass callbacks for web status, web headers
   // the extra WEBOPT_HEADER is used to help test what's sent 
   // (snoop.sh above shows all the headers)
   if(!pMe->m_pIWeb_xt)
   {
	   ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)(&pMe->m_pIWeb_xt));
	   // set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)

	   
		awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		
		// Marks the end of the array of WebOpts
		awo[i].nId  = WEBOPT_END;
		





/*

		i = 0;
		// set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)
		awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo[i].pVal = (void *)10000;
		i++;
		
		// test user-agent, uncomment this section to ship your own user-agent 
		// string. if unset, IWeb will send a default.  If set to NULL, no 
		// user agent header will be sent */
		
		// Set TEST_USER_AGENT in the NetDiagnostics project settings to all
		// shipping of your own user agent.
/*		
		
#ifdef TEST_USER_AGENT
		awo[i].nId  = WEBOPT_USERAGENT;
		awo[i].pVal = (void *)WEBBER_USERAGENT;
		i++;
#endif 
		
		// test nowaitconn, this only comes into effect if you build webber 
		// with multiple myiwebs (see the definition of struct Webber)
		awo[i].nId  = WEBOPT_FLAGS;
		awo[i].pVal = (void *)WEBREQUEST_NOWAITCONN;
		i++;
		
		
		// test forcenew, uncomment this section to try multiple simultaneous
		// "forced" new connections. Forced new connections are not kept alive
		// unless they are the first forced new connection to a host
#ifdef TEST_FORCENEWCONN
		awo[i].nId  = WEBOPT_FLAGS;
		awo[i].pVal = (void *)WEBREQUEST_FORCENEWCONN;
		i++;
#endif 
		
#ifdef TEST_USER_AGENT
		// turn off HTTP over HTTP proxying
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"http:///";
		i++;
		
		// turn on ALL proxying.  Proxyspecs are examined in WebOpt
		// order, so in this list, with the above and below PROXYSPECs,
		// everything except HTTP will be proxied through
		// http://webproxy.yourdomain.com:8080, (which you'll have to
		// set up to test, sorry
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"*:///http://webproxy.yourdomain.com:8080";
		i++;
#endif
		
		// Marks the end of the array of WebOpts
		awo[i].nId  = WEBOPT_END;
		
		// Add Options
    //  IWEB_AddOpt(pMe->m_pIWeb_s,awo);


*/

  
		
		
		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb_xt,awo);
   }
   if ((IPeek *)0 != pMe->pipPostData_xt) {
      IWEB_GetResponse(pMe->m_pIWeb_xt,
                       (pMe->m_pIWeb_xt, &pMe->piWResp_xt, &pMe->cb_xt, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe,
                        WEBOPT_HEADER, "X-Method: POST\r\n", /* for kicks */
                        WEBOPT_HEADERHANDLER, myiweb_Header_xt, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_xt, 
                        WEBOPT_METHOD, "POST",
                        WEBOPT_BODY, pMe->pipPostData_xt,
                        WEBOPT_CONTENTLENGTH, STRLEN(pMe->pszPostData_xt + 1),
                        WEBOPT_END));
   } else {
      IWEB_GetResponse(pMe->m_pIWeb_xt,
                       (pMe->m_pIWeb_xt, &pMe->piWResp_xt, &pMe->cb_xt, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe, 
                        WEBOPT_HEADER, "X-Method: GET \r\n",
						WEBOPT_METHOD, "GET",
                        WEBOPT_HEADERHANDLER, myiweb_Header_xt, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_xt, 
                        WEBOPT_END));
   }
}

/* myiweb_GotResp_lx �����������й�*/

static void myiweb_GotResp_lx(void *p)
{	
	newgimis *pwa = (newgimis *)p;
// 	char *sBuf = NULL;
// 	char *sBuf_d = NULL;
// 
// 	char *sBuf_T = NULL;
// 	char *sBuf1 = NULL;
// 
// 	
// 	int nRead = 0;
// 	int si_Ret = 0;  //����ֵ
// 	char sc_Count[2];
	
	char lStr_msg[100];
	
	
	WebRespInfo *pwri;
	
	// get information about how the web transaction went
	// pwa->piWResp is ***NEVER NULL***, even though the transaction may fail
	// for wont of memory
	pwri = IWEBRESP_GetInfo(pwa->piWResp_lx);
	
	//   ND_Print(pMe, "** got response...\n** info code: %d\n", pwri->nCode);

	pwa->iPos_kq_zt = 0;

	if (!WEB_ERROR_SUCCEEDED(pwri->nCode)) 
	{
		int nErr = 0;
		nErr = pwri->nCode;
		SPRINTF(lStr_msg, "kao qin no response, code: %d",nErr);
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),lStr_msg); 

		// ����û�з��ͳɹ�
		if (nErr <0) 	nErr = (-1 * nErr);
		
		pwa->m_Error = nErr;
//		if (pwa->m_WitchPag == 10)
		if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
		{		
	//		ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_PRINT,0, 0);
			ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
		}
		

//		 myiweb_Stop(pwa,1); // stop cleans up

		

		if (nErr == WEB_ERROR_CONNECT) {	// 1289
			char *sc_buf = NULL;
			//  ��������  gimis�˳�һ���Ӻ����� һ��ͨѶ��ָ�����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT  xin tiao");
//			IPOSDET_Release_fun(pwa);

			ud_RecLog_c(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT");
			if (pwa->cWatchDog != '0')
			{
				if (pwa->m_W_E++ > 3) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
				{

					uf_RecLog(pwa,"WatchDog Reboot");
					IPOSDET_Release_fun(pwa);
					STRCPY(sc_buf,"123"); //�������
				}
			}
			else
			{
				if (pwa->m_E_SC > 0)
				{
					if (pwa->m_W_E_P++ > pwa->m_E_SC) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
					{

						uf_RecLog(pwa,"Play error Sound");
						SoundPlay_Start( pwa );
						pwa->m_W_E_P = 0;
					}
				}
			}
		}	// 1289
		else {
			// �������ֻ�����ԭ�� ����Ƿ�ѵ����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"no response, nErr=%d",nErr);
			if (nErr == 554)
			{
				pwa->m_F_QF = TRUE;  // ˵���ֻ�Ƿ��
				pwa->m_PosMode = TRUE; //ֻ����������
			}
		}

		// pMe->m_Flag_F = 2; // ������ǵ�ǰ������������ʲô����  1 -- һ������      2 -- ���Ϳճ�     3 --  �����س�
		switch(pwa->m_Flag_F)
		{
		case 1:  // һ������
			Send_GPS_error(pwa); // ��ʾʧ�ܵĽ���
			Save_GPSData_All( pwa );

			ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);
			break;
		case 2:  // ���Ϳճ�
			uf_AEPrint_EMPTY_FAIL_Back(pwa);
			ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);
			break;
		case 3:	 // �����س�
			uf_AEPrint_FULL_FAIL_Back(pwa);
			ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);
			break;
		case 4:  // ����״̬ 0
			uf_AEPrint_ZT_0_FAIL_Back(pwa);
			ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);
			break;
		case 5:	 // ����״̬ 1
			uf_AEPrint_ZT_1_FAIL_Back(pwa);
			ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);
			break;
		case 6:  //һ����
			uf_AEPrint_YJDK_FAIL_Back(pwa);	
			ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);
			break;
		}

		myiweb_Stop_lx(pwa);
		return ;
	}	

	pwa->m_Error = pwri->nCode;
	if (pwa->m_Error < 0)
	{
		pwa->m_Error = (-1 * pwa->m_Error);
	}
//	if (pwa->m_WitchPag == 10)
	if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
	{		
		ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
	}

	if ((ISource *)0 != pwri->pisMessage)
	{
		pwa->m_W_E = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
		pwa->m_W_E_P = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ�򲥷�¼��

		pwa->pSource_lx = pwri->pisMessage;
		ISOURCE_AddRef(pwa->pSource_lx);
		
		IWEBRESP_Release(pwa->piWResp_lx);
		pwa->piWResp_lx = 0;
		
		CALLBACK_Cancel(&pwa->cb_lx);
		CALLBACK_Init(&pwa->cb_lx, myiweb_lx_ReadTaskBody, pwa);
		myiweb_lx_ReadTaskBody(pwa);
	}
}


static void myiweb_lx_ReadTaskBody(newgimis *pwa)
{
	char *sBuf = NULL;
	char *sBuf_d = NULL;
	
	char *sBuf_T = NULL;
	char *sBuf1 = NULL;
	
	int nRead = 0;
	int si_Ret = 0;  //����ֵ
	char sc_Count[2];
	
	ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"coming myiweb_lx_ReadTaskBody");
	
	sBuf = (char *)MALLOC(1024);
	MEMSET(sBuf,0x00,1024);
	
	sBuf_d = (char *)MALLOC(102400);
	MEMSET(sBuf_d,0x00,102400);
	
	nRead = ISOURCE_Read(pwa->pSource_lx, sBuf, 1023);
	if(ISOURCE_WAIT == nRead) 
	{
		//		ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cAudioApp_ReadWebBody in,ISOURCE_WAIT");
		//		ISOURCE_Readable(me->pSource_xt, &me->webcb);
		//		ISOURCEUTIL_Release(pwa->pSource_xt);
		/*			myiweb_Stop_lx(pwa);
		FREE(sBuf_d);
		sBuf_d = NULL;
		FREE(sBuf);
		sBuf = NULL;
		*/
		
		ISOURCE_Readable(pwa->pSource_lx, &pwa->cb_lx);
		
		FREE(sBuf);
		sBuf = NULL;
		FREE(sBuf_d);
		sBuf_d = NULL;
		if (pwa->m_F_R_LX == 0)
		{
			FREE(pwa->m_H_LX);
			pwa->m_H_LX = NULL;
			pwa->m_F_R_LX = 0;
		}
		
		//			myiweb_Stop_lx(pwa);
		
		return;
	}
	
	if (nRead > 0)		
	{
		DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   successful");
		DBGPRINTF(sBuf);
		//		IHTMLVIEWER_LoadSource(pwa->m_pHTMLViewer, pwri->pisMessage);
		SPRINTF(sc_Count,"%d",nRead);
		DBGPRINTF(sc_Count);
		//			ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"xin tiao kai shi"); 
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf); 
		//			ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"xin tiao jie shu"); 
		// ����������Ȩ�ķ��ص�ֵ��д�������ļ�
		
		
		
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," nRead  KQ = %d",nRead); 
		
		if (nRead > 100)
		{
			sBuf1 = (char *)MALLOC(1024);
			sBuf_T = (char *)MALLOC(102400);
			
			while (!sBuf_T)
			{
				ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," sBuf_T KQ Failure to apply for space 1"); 
				sBuf_T = (char *)MALLOC(102400);
			}
			MEMSET(sBuf_T,0x00,102400);
			STRCPY(sBuf_T,sBuf);
		}
		else
		{
			sBuf1 = (char *)MALLOC(1024);
			sBuf_T = (char *)MALLOC(1024);
			while(!sBuf_T)
			{
				ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," sBuf_T KQ Failure to apply for space 2"); 
				sBuf_T = (char *)MALLOC(1024);
			}
			MEMSET(sBuf_T,0x00,1024);
			STRCPY(sBuf_T,sBuf);
		}
		
		while (nRead > 100)
		{
			MEMSET(sBuf1,0x00,1024);
			nRead = ISOURCE_Read(pwa->pSource_lx, sBuf1, 1023);
			STRCAT(sBuf_T,sBuf1);
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"while nRead KQ = %d",nRead); 
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf1); 
		}
		
		
		if(ISOURCE_WAIT == nRead) 
		{
			uf_RecLog(pwa," ISOURCE_Readable KQ");
			
			if (pwa->m_F_R_LX == 1) // ˵��һ�������������ε� ISOURCE_WAIT �ⲻ�ǵ�һ�γ��� 
			{
				STRCAT(pwa->m_H_LX,sBuf_T);	
			}
			else // ��һ�γ���
			{
				pwa->m_H_LX = (char *) MALLOC(102400);
				MEMSET(pwa->m_H_LX,0x00,102400);
				STRCPY(pwa->m_H_LX,sBuf_T);
			}
			
			
			pwa->m_F_R_LX = 1; //
			ISOURCE_Readable(pwa->pSource_lx, &pwa->cb_lx);
			
			FREEIF(sBuf_T);
			sBuf_T = NULL ;
			
			FREEIF(sBuf1);
			sBuf1 = NULL ;
			
			FREE(sBuf);
			sBuf = NULL;
			FREE(sBuf_d);
			sBuf_d = NULL;
			
			
			
			return;
		}
		
		
		
		
		if (pwa->m_F_R_LX == 1 && pwa->m_H_LX)
		{
			STRCAT(pwa->m_H_LX,sBuf_T);
			uf_RecLog(pwa,"wait time STRCAT HTTP CONTENT KQ");
			uf_RecLog(pwa,"wait time STRCAT HTTP CONTENT KQ");
			MEMSET(sBuf_T,0x00,102400);
			STRCPY(sBuf_T,pwa->m_H_LX);
			uf_RecLog(pwa,"wait time 1 STRCAT HTTP CONTENT KQ");
			uf_RecLog(pwa,"wait time 1 STRCAT HTTP CONTENT KQ");
			pwa->m_F_R_LX = 0; //
			FREE(pwa->m_H_LX);
			pwa->m_H_LX = NULL;
		}
		
		
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," sBuf_T KQ = %d ",STRLEN(sBuf_T)); 
		uf_RecLog(pwa,sBuf_T);
		
		uf_fromESC(sBuf_T,sBuf_d);
		
		FREEIF(sBuf_T);
		sBuf_T = NULL ;
		
		FREEIF(sBuf1);
		sBuf1 = NULL ;

		if (STRSTR(sBuf_d,"<html>") != NULL)
		{
			Display_NetworkErr(pwa);
			ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);

			FREE(sBuf);
			sBuf = NULL;
			
			FREE(sBuf_d);
			sBuf_d = NULL;

			myiweb_Stop_lx(pwa);
			return;
		}

		
		
		si_Ret = ParseStringandWriteToConfigure_xintiao_GPS(pwa,sBuf_d);
		//����ֵ  5 -- ����Ȩ��δ��Ȩ ����ر�
		
		if (si_Ret == 3)
		{
			ParseStringandWriteToConfigure_xintiao_GPS_FAIL( pwa,sBuf_d);
		}
		
		
		switch(pwa->m_Flag_F)
		{
		case 1:  // һ������
			{
				switch (si_Ret)  //˵���ɹ�
				{
				case 0:
					{
						// ˵������ʧ��  �ϴ����ɹ�
						Send_GPS_error_no_save(pwa); // ��ʾʧ�ܵĽ���
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"attendance fail 0");
					}
				case 1: // ���ڳɹ�
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 
						Send_GPS_success(pwa); // ��ʾʧ�ܵĽ���
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);	
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"attendance successful 2");
					}
				case 2: // ���ڳɹ�	
					{
						Send_GPS_success(pwa); // ��ʾʧ�ܵĽ���
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"attendance successful 1");
					}
					break;	
				case 4:  // �ϴ����ɹ�
					{
						// ˵������ʧ��  �ϴ����ɹ�
						Send_GPS_error_no_save(pwa); // ��ʾʧ�ܵĽ���
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"attendance fail 4");
					}
					break;	
				case 3: // ����ʧ��
					{
						gimis_SetGPSPara(pwa); 
						Send_GPS_error_no_save(pwa); // ��ʾʧ�ܵĽ���
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);	
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"attendance fail 3");
					}
					break;	 
					
				case 5:
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 	
						// �رճ���
						pwa->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
						
						ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_WSQ, (uint32*) pwa); //�˳�����
						break;
					}
					
				}
			}
			break;
		case 2:  // ���Ϳճ�
			{
				switch (si_Ret)  //˵���ɹ�
				{
				case 1: // �ɹ�
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 
						uf_AEPrint_EMPTY_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);	
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send empty success 2");
					}
				case 2: // ���ڳɹ�	
					{
						uf_AEPrint_EMPTY_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send empty success 1");
					}
					break;	
				case 4:  // �ϴ����ɹ�
					{
						// ˵������ʧ��  �ϴ����ɹ�
						uf_AEPrint_EMPTY_FAIL_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send empty fail 4");
					}
					break;	
				case 3: // ����ʧ��
					{
						uf_AEPrint_EMPTY_FAIL_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);	
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send empty fail 3");
					}
					break;
					
				case 5:
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 	
						// �رճ���
						pwa->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
						
						ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_WSQ, (uint32*) pwa); //�˳�����
						break;
					}
					
				}
			}
			break;
		case 3:	 // �����س�
			{
				switch (si_Ret)  //˵���ɹ�
				{
				case 1: // �ɹ�
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 
						uf_AEPrint_FULL_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);	
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send full 2");
					}
				case 2: // ���ڳɹ�	
					{
						uf_AEPrint_FULL_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send full 1");
					}
					break;	
				case 4:  // �ϴ����ɹ�
					{
						// ˵������ʧ��  �ϴ����ɹ�
						uf_AEPrint_FULL_FAIL_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send full fail 4");
					}
					break;	
				case 3: // ����ʧ��
					{
						uf_AEPrint_FULL_FAIL_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);	
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send full fail 3");
					}
					break;	
					
				case 5:
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 	
						// �رճ���
						pwa->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
						
						ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_WSQ, (uint32*) pwa); //�˳�����
						break;
					}
					
				}
			}
			break;
			
			//////////////////////////////// 
		case 4:  // ����״̬0
			{
				switch (si_Ret)  //˵���ɹ�
				{
				case 1: // �ɹ�
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 
						uf_AEPrint_ZT_0_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);	
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send empty success 2");
					}
				case 2: // ���ڳɹ�	
					{
						uf_AEPrint_ZT_0_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send empty success 1");
					}
					break;	
				case 4:  // �ϴ����ɹ�
					{
						// ˵������ʧ��  �ϴ����ɹ�
						uf_AEPrint_ZT_0_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send empty fail 4");
					}
					break;	
				case 3: // ����ʧ��
					{
						uf_AEPrint_ZT_0_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);	
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send empty fail 3");
					}
					break;	 
					
				case 5:
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 	
						// �رճ���
						pwa->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
						
						ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_WSQ, (uint32*) pwa); //�˳�����
						break;
					}
					
				}
			}
			break;
		case 5:	 // ����״̬1
			{
				switch (si_Ret)  //˵���ɹ�
				{
				case 1: // �ɹ�
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 
						uf_AEPrint_ZT_1_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);	
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send full 2");
					}
				case 2: // ���ڳɹ�	
					{
						uf_AEPrint_ZT_1_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send full 1");
					}
					break;	
				case 4:  // �ϴ����ɹ�
					{
						// ˵������ʧ��  �ϴ����ɹ�
						uf_AEPrint_ZT_1_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send full fail 4");
					}
					break;	
				case 3: // ����ʧ��
					{
						uf_AEPrint_ZT_1_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost_Two,pwa);	
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"send full fail 3");
					}
					break;	 
					
				case 5:
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 	
						// �رճ���
						pwa->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
						
						ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_WSQ, (uint32*) pwa); //�˳�����
						break;
					}
					
				}
			}
			break;

		case 6:	 // һ����
			{
				switch (si_Ret)  //˵���ɹ�
				{
				case 0:
					{
						// ˵������ʧ��  �ϴ����ɹ�
						uf_AEPrint_YJDK_FAIL_Back(pwa); // ��ʾʧ�ܵĽ���
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"uf_AEPrint_YJDK_FAIL_Back fail 0");
					}
				case 1: // �ɹ�
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 
						uf_AEPrint_YJDK_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);	
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"uf_AEPrint_YJDK_FAIL_Back success 1");
					}
				case 2: // ���ڳɹ�	
					{
						uf_AEPrint_YJDK_SUCC_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"uf_AEPrint_YJDK_FAIL_Back fail 2");
					}
					break;	
				case 4:  // �ϴ����ɹ�
					{
						// ˵������ʧ��  �ϴ����ɹ�
						uf_AEPrint_YJDK_FAIL_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"uf_AEPrint_YJDK_FAIL_Back fail 4");
					}
					break;	
				case 3: // ����ʧ��
					{
						uf_AEPrint_YJDK_FAIL_Back(pwa);
						ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);	
						ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"uf_AEPrint_YJDK_FAIL_Back fail 3");
					}
					break;	 
					
				case 5:
					{
						pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
						gimis_SetGPSPara(pwa); 	
						// �رճ���
						pwa->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
						
						ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_WSQ, (uint32*) pwa); //�˳�����
						break;
					}
					
				}
			}
			break;
		}
			
			
			FREE(sBuf);
			sBuf = NULL;
			
			FREE(sBuf_d);
			sBuf_d = NULL;
		}
		
		myiweb_Stop_lx(pwa);
}




static void myiweb_GotResp_slx(void *p)
{	
	newgimis *pwa = (newgimis *)p;
// 	char *sBuf = NULL;
// 	char *sBuf_d = NULL;
// 	int nRead = 0;
// 	int si_Ret = 0;  //����ֵ
// 	char sc_Count[2];
	
	char lStr_msg[100];
	
	
	WebRespInfo *pwri;
	
	// get information about how the web transaction went
	// pwa->piWResp is ***NEVER NULL***, even though the transaction may fail
	// for wont of memory
	pwri = IWEBRESP_GetInfo(pwa->piWResp_slx);
	
	//   ND_Print(pMe, "** got response...\n** info code: %d\n", pwri->nCode);

	pwa->m_Mutex_LX = 0;  //˵��������������������

	if (!WEB_ERROR_SUCCEEDED(pwri->nCode)) 
	{
		int nErr = 0;
		nErr = pwri->nCode;
		SPRINTF(lStr_msg, "offline attendance no response, code: %d",nErr);
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),lStr_msg); 

		// ����û�з��ͳɹ�
		if (nErr <0) 	nErr = (-1 * nErr);

		pwa->m_Error = nErr;
//		if (pwa->m_WitchPag == 10)
		if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
		{		
	//		ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_PRINT,0, 0);
			ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
		}
		

//		 myiweb_Stop(pwa,1); // stop cleans up

		

		if (nErr == WEB_ERROR_CONNECT) {	// 1289
			char *sc_buf = NULL;
			//  ��������  gimis�˳�һ���Ӻ����� һ��ͨѶ��ָ�����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT  xin tiao");
//			IPOSDET_Release_fun(pwa);

			ud_RecLog_c(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT");
			if (pwa->cWatchDog != '0')
			{
				if (pwa->m_W_E++ > 3) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
				{

					uf_RecLog(pwa,"WatchDog Reboot");
					IPOSDET_Release_fun(pwa);
					STRCPY(sc_buf,"123"); //�������
				}
			}
			else
			{
				if (pwa->m_E_SC > 0)
				{
					if (pwa->m_W_E_P++ > pwa->m_E_SC) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
					{

						uf_RecLog(pwa,"Play error Sound");
						SoundPlay_Start( pwa );
						pwa->m_W_E_P = 0;
					}
				}
			}
		}	// 1289
		else {
			// �������ֻ�����ԭ�� ����Ƿ�ѵ����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"no response, nErr=%d",nErr);
			if (nErr == 554)
			{
				pwa->m_F_QF = TRUE;  // ˵���ֻ�Ƿ��
				pwa->m_PosMode = TRUE; //ֻ����������
			}
		}

		// Add End
		FREEIF(pwa->pszPostData_slx);

		myiweb_Stop_slx(pwa);
		return ;
	}	

	pwa->m_Error = pwri->nCode;
	if (pwa->m_Error < 0)
	{
		pwa->m_Error = (-1 * pwa->m_Error);
	}
//	if (pwa->m_WitchPag == 10)
	if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
	{		
		ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
	}

	if ((ISource *)0 != pwri->pisMessage) 
	{
		pwa->m_W_E = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
		pwa->m_W_E_P = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ�򲥷�¼��

		pwa->pSource_slx = pwri->pisMessage;
		ISOURCE_AddRef(pwa->pSource_slx);
		
		IWEBRESP_Release(pwa->piWResp_slx);
		pwa->piWResp_slx = 0;
		
		CALLBACK_Cancel(&pwa->cb_slx);
		CALLBACK_Init(&pwa->cb_slx, myiweb_slx_ReadTaskBody, pwa);
		myiweb_slx_ReadTaskBody(pwa);
	}
}

static void myiweb_slx_ReadTaskBody(newgimis *pwa)
{	
	char *sBuf = NULL;
	char *sBuf_d = NULL;
	int nRead = 0;
	int si_Ret = 0;  //����ֵ
	char sc_Count[2];

	ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"coming myiweb_slx_ReadTaskBody");
	
	sBuf = (char *)MALLOC(1024);
	MEMSET(sBuf,0x00,sizeof(sBuf));
	
	sBuf_d = (char *)MALLOC(1024);
	MEMSET(sBuf_d,0x00,sizeof(sBuf_d));
	
	nRead = ISOURCE_Read(pwa->pSource_slx, sBuf, 1024);
	if(ISOURCE_WAIT == nRead) 
	{
		//		ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cAudioApp_ReadWebBody in,ISOURCE_WAIT");
		//		ISOURCE_Readable(me->pSource_xt, &me->webcb);
		//		ISOURCEUTIL_Release(pwa->pSource_xt);
		 ISOURCE_Readable(pwa->pSource_slx, &pwa->cb_slx);

		FREE(sBuf);
		sBuf = NULL;
		FREE(sBuf_d);
		sBuf_d = NULL;

	   
		
		return;
	}
	
	if (nRead > 0)		
	{
		DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   successful");
		DBGPRINTF(sBuf);
		//		IHTMLVIEWER_LoadSource(pwa->m_pHTMLViewer, pwri->pisMessage);
		SPRINTF(sc_Count,"%d",nRead);
		DBGPRINTF(sc_Count);
		//			ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"xin tiao kai shi"); 
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"Offline attendance return content"); 
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf); 
		//			ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"xin tiao jie shu"); 
		// ����������Ȩ�ķ��ص�ֵ��д�������ļ�
		
		// ɾ�������ļ� 
		uf_DelOff_File(pwa);
		
		uf_fromESC(sBuf,sBuf_d);
		si_Ret = ParseStringandWriteToConfigure_xintiao_GPS(pwa,sBuf_d);
		//����ֵ 5 -- ����Ȩ��δ��Ȩ ����ر�
		
		switch (si_Ret)  //˵���ɹ�
		{
		case 1: // ���߿��ڳɹ�
			{
				pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
				gimis_SetGPSPara(pwa); 
				
				// ɾ�������ļ� 
				//		uf_DelOff_File(pwa);
			}
		case 2: // ���ڳɹ�	
			{
				//			   Send_GPS_success(pwa); // ��ʾʧ�ܵĽ���
				//			   ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);
				//			   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"���߿��ڳɹ�1");
				
				// ɾ�������ļ� 
				//		uf_DelOff_File(pwa);
			}
			break;	
		case 4:  // ʧ�ܲ���ɾ���ļ�
			{
				
			}
			break;	
		case 3: // ʧ��  ����ɾ���ļ�
			{	
				// ɾ�������ļ� 
				//		uf_DelOff_File(pwa);
			}
			break;	
			
		case 5:
			{
				pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
				gimis_SetGPSPara(pwa); 	
				// �رճ���
				pwa->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
				
				ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_WSQ, (uint32*) pwa); //�˳�����
				break;
			}
			
		}
		
		FREE(sBuf);
		sBuf = NULL;
		
		FREE(sBuf_d);
		sBuf_d = NULL;
	}
	FREEIF(pwa->pszPostData_slx);
	
	myiweb_Stop_slx(pwa);
}

//  myiweb_GotResp_qqsq start   ///////////////
static void myiweb_GotResp_qqsq(void *p)
{
	newgimis *pwa = (newgimis *)p;
	char *sBuf = NULL;

	char *sBuf_d = NULL;
	int nRead = 0;
	int si_Ret = 0;  //����ֵ
	char sc_Count[2];
	
	char lStr_msg[100];
	
	
	WebRespInfo *pwri;
	
	// get information about how the web transaction went
	// pwa->piWResp is ***NEVER NULL***, even though the transaction may fail
	// for wont of memory
	pwri = IWEBRESP_GetInfo(pwa->piWResp_qqsq);
	
	//   ND_Print(pMe, "** got response...\n** info code: %d\n", pwri->nCode);

	pwa->m_Flag_Qqsq = 0; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����

	pwa->iPostPhTm = 0;

	if (!WEB_ERROR_SUCCEEDED(pwri->nCode)) 
	{
		int nErr = 0;
		nErr = pwri->nCode;
		SPRINTF(lStr_msg, "Authorization 1 no response, code: %d",nErr);
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),lStr_msg); 

		// ����û�з��ͳɹ�
		if (nErr <0) 	nErr = (-1 * nErr);

		pwa->m_Error = nErr;
//		if (pwa->m_WitchPag == 10)
		if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
		{		
	//		ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_PRINT,0, 0);
			ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
		}


		
		

		if (nErr == WEB_ERROR_CONNECT) {	// 1289
			char *sc_buf = NULL;
			//  ��������  gimis�˳�һ���Ӻ����� һ��ͨѶ��ָ�����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT 1");
//			IPOSDET_Release_fun(pwa);

			ud_RecLog_c(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT");
			if (pwa->cWatchDog != '0')
			{
				if (pwa->m_W_E++ > 3) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
				{

					uf_RecLog(pwa,"WatchDog Reboot");
					IPOSDET_Release_fun(pwa);
					STRCPY(sc_buf,"123"); //�������
				}
			}
			else
			{
				if (pwa->m_E_SC > 0)
				{
					if (pwa->m_W_E_P++ > pwa->m_E_SC) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
					{

						uf_RecLog(pwa,"Play error Sound");
						SoundPlay_Start( pwa );
						pwa->m_W_E_P = 0;
					}
				}
			}
		}	// 1289
		else {
			// �������ֻ�����ԭ�� ����Ƿ�ѵ����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"no response 1, nErr=%d",nErr);
			if (nErr == 554)
			{
				pwa->m_F_QF = TRUE;  // ˵���ֻ�Ƿ��
				pwa->m_PosMode = TRUE; //ֻ����������
			}
		}
		// Add End
		ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_QQSQ_SB_WLYC,0, 0);	// //������Ȩʧ�ܵķ����¼�	 //����Ҳ��ʾ������Ȩʧ��
		myiweb_Stop_qqsq(pwa);
		return ;
	}



	pwa->m_Error = pwri->nCode;
	if (pwa->m_Error < 0)
	{
		pwa->m_Error = (-1 * pwa->m_Error);
	}
//	if (pwa->m_WitchPag == 10)
	if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
	{		
		ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
	}


		// body may be NULL
	if ((ISource *)0 != pwri->pisMessage) 
	{
	//	ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"���᲻���������");
		// �˺����� ISource �����������ĵ� �������ݰ���HTML ��ʽ���� ɾ��������ǰ������ 
//		IHTMLVIEWER_LoadSource(pwa->m_pHTMLViewer, pwri->pisMessage);
		DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   1");
		pwa->m_W_E = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
		pwa->m_W_E_P = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ�򲥷�¼��
		
//		MEMSET(sc_Count,0x00,sizeof(sBuf));
		
		pwa->pSource_qqsq = pwri->pisMessage;
		ISOURCE_AddRef(pwa->pSource_qqsq);	
		sBuf = (char *)MALLOC(1024);
		MEMSET(sBuf,0x00,sizeof(sBuf));

		sBuf_d = (char *)MALLOC(1024);
		MEMSET(sBuf_d,0x00,sizeof(sBuf_d));

		nRead = ISOURCE_Read(pwa->pSource_qqsq, sBuf, 1024);
		if(ISOURCE_WAIT == nRead) 
		{
			//		ud_RecLog(pMe->theApp.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"cAudioApp_ReadWebBody in,ISOURCE_WAIT");
			//		ISOURCE_Readable(me->pSource, &me->webcb);
			//		ISOURCEUTIL_Release(pwa->pSource);
			return;
		}
		
		if (nRead > 0)		
		{
			DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   successful");
			DBGPRINTF(sBuf);
			//		IHTMLVIEWER_LoadSource(pwa->m_pHTMLViewer, pwri->pisMessage);
			SPRINTF(sc_Count,"%d",nRead);
			DBGPRINTF(sc_Count);
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf); 
			// ����������Ȩ�ķ��ص�ֵ��д�������ļ�
			if (pwa->i_HttpMod == 1)	//HTTP���շ�������ģʽ ����	1 - ������ջ�������������Ȩ�� 2 - ���ջ�������GPS�����ϴ�����������)
			{
				uf_fromESC(sBuf,sBuf_d);
				si_Ret = ParseStringandWriteToConfigure(pwa,sBuf_d);
				if (si_Ret)  //˵���ɹ�
				{
					//gimis_WriteGPSSettings(pwa,);
					pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
					gimis_SetGPSPara(pwa); 

					if (pwa->cAuthorize == '1')
					{
				//		ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_QQSQ_CG,0, 0);	 //������Ȩ�ɹ��ķ����¼�
						pwa->m_Flag_Qqsq = 0; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����
						pwa->m_PosMode = FALSE;// ��������Ƿ���ֻ���ô����Ƕ�λ TRUE -- ��ʾֻ���ô�����  FLASH -- ���� �����Ա�ķ�ʽ
						if (pwa->cGpsMode == '1') // ˵������GPS
						{
							newgimis_OpenGPS(pwa);  // ֻ����GPS 
						}
					}
					else
					{
						//	ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_QQSQ_SB,0, 0);	// //������Ȩʧ�ܵķ����¼�		
						// ��Ȩʧ�ܺ󣬿���������
						pwa->m_PowerFlag = '0';
						gimis_SetPram_Open( pwa); 
						
						// �رճ���
						pwa->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
						
						ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_SQYC, (uint32*) pwa); //�˳�����
					}
			//		ISHELL_SetTimer(pMe->a.m_pIShell, 5000, (PFNNOTIFY) testgps_StartApplet, (uint32*) pMe);
				}
				else
				{
				//	ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_QQSQ_SB,0, 0);	// //������Ȩʧ�ܵķ����¼�	
					pwa->m_Flag_Qqsq = 0; //��������Ƿ�����������Ȩ  1 -- ��  0-- ����
					pwa->m_PosMode = FALSE;// ��������Ƿ���ֻ���ô����Ƕ�λ TRUE -- ��ʾֻ���ô�����  FLASH -- ���� �����Ա�ķ�ʽ
					if (pwa->cGpsMode == '1') // ˵������GPS
					{
						newgimis_OpenGPS(pwa);  // ֻ����GPS 
					}
				}
			}
			else if (pwa->i_HttpMod == 2)
			{

			}
			
			FREE(sBuf);
			sBuf = NULL;
			FREE(sBuf_d);
			sBuf_d = NULL;
		}
	}
	myiweb_Stop_qqsq(pwa);	
}



//  myiweb_GotResp_qqsq  end    ///////////////


static void myiweb_GotResp_xt(void *p)
{
	newgimis *pwa = (newgimis *)p;
	char *sBuf = NULL;
	char *sBuf1 = NULL;
	char *sBuf_T = NULL;

	char *sBuf_d = NULL;
	int nRead = 0;
	int si_Ret = 0;  //����ֵ
	char sc_Count[2];
	
	char lStr_msg[100];
	
	
	WebRespInfo *pwri;
	
	// get information about how the web transaction went
	// pwa->piWResp is ***NEVER NULL***, even though the transaction may fail
	// for wont of memory
	pwri = IWEBRESP_GetInfo(pwa->piWResp_xt);
	
	//   ND_Print(pMe, "** got response...\n** info code: %d\n", pwri->nCode);

	pwa->iPostPhTm_xt = 0;

	ISHELL_SetTimer(pwa->a.m_pIShell, 30 * 1000, (PFNNOTIFY)uf_MonitorWait_HTTP, pwa);

	pwa->m_T_E_C = 0; // ����ͳ���������� ��ȡʧ�ܼ��� ���������������ͣ��ȡ����ͣʱ��Ϊ200�룬Ȼ�������ȡ ����������������ϴ����������

	// �ж��Ƿ���Ҫ����
	judge_reset_TB(pwa);

	if (!WEB_ERROR_SUCCEEDED(pwri->nCode)) 
	{
		int nErr = 0;
		nErr = pwri->nCode;
		SPRINTF(lStr_msg, " heartbeat no response, code: %d",nErr);
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),lStr_msg); 

		// ����û�з��ͳɹ�
		if (nErr <0) 	nErr = (-1 * nErr);
		
		pwa->m_Error = nErr;
//		if (pwa->m_WitchPag == 10)
		if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
		{		
	//		ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_PRINT,0, 0);
			ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
		}
		

//		 myiweb_Stop(pwa,1); // stop cleans up


		if (nErr == WEB_ERROR_CONNECT)
		{	// 1289
			char *sc_buf = NULL;
			//  ��������  gimis�˳�һ���Ӻ����� һ��ͨѶ��ָ�����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT  xin tiao");
//			IPOSDET_Release_fun(pwa);

			ud_RecLog_c(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT");
			if (pwa->cWatchDog != '0')
			{
				if (pwa->m_W_E++ > 3) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
				{

					uf_RecLog(pwa,"WatchDog Reboot");
					IPOSDET_Release_fun(pwa);
					STRCPY(sc_buf,"123"); //�������
				}
			}
			else
			{
				if (pwa->m_E_SC > 0)
				{
					if (pwa->m_W_E_P++ > pwa->m_E_SC) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
					{

						uf_RecLog(pwa,"Play error Sound");
						SoundPlay_Start( pwa );
						pwa->m_W_E_P = 0;
					}
				}
			}
		}	// 1289
		else {
			// �������ֻ�����ԭ�� ����Ƿ�ѵ����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"no response, nErr=%d",nErr);
			if (nErr == 554)
			{
				ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"no response, m_F_QF=%d",pwa->m_F_QF);
				pwa->m_F_QF = TRUE;  // ˵���ֻ�Ƿ��
				pwa->m_PosMode = TRUE; //ֻ����������
			}
		}
		// Add End

		myiweb_Stop_xt(pwa);
		// ��HTTP���� 
		// // m_MUTEX; // 1 = ˵�����ܽ��з���HTTP �Ѿ���ռ����http����Դ  0= ˵������ʹ��http��Դ
		pwa->m_MUTEX = 0;
		return ;
	}	

	pwa->m_Error = pwri->nCode;
	if (pwa->m_Error < 0)
	{
		pwa->m_Error = (-1 * pwa->m_Error);
	}
//	if (pwa->m_WitchPag == 10)
	if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
	{		
		ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
	}

	if ((ISource *)0 != pwri->pisMessage)
	{
		pwa->m_W_E = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
		pwa->m_W_E_P = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ�򲥷�¼��
		pwa->m_PosMode = FALSE; //�������Ϊ ������Q3+���ֻ��ϻ�ͻȻ��ɴ�����ģʽ
		//  add 2015-11-4  start  //
		pwa->m_F_QF = FALSE;  // ˵���ֻ�Ƿ��
		pwa->m_PosMode = FALSE; //ֻ����������

		pwa->pSource_xt = pwri->pisMessage;
		ISOURCE_AddRef(pwa->pSource_xt);
		
		IWEBRESP_Release(pwa->piWResp_xt);
		pwa->piWResp_xt = 0;
		
		CALLBACK_Cancel(&pwa->cb_xt);
		CALLBACK_Init(&pwa->cb_xt, myiweb_xt_ReadTaskBody, pwa);
		myiweb_xt_ReadTaskBody(pwa);
	}
}


static void myiweb_xt_ReadTaskBody(newgimis *pwa)
{
	char *sBuf = NULL;
	char *sBuf1 = NULL;
	char *sBuf_T = NULL;
	
	char *sBuf_d = NULL;
	int nRead = 0;
	int si_Ret = 0;  //����ֵ
	char sc_Count[2];
	
	char lStr_msg[100];
	
	sBuf = (char *)MALLOC(1024);
	MEMSET(sBuf,0x00,1024);

	while (!sBuf)
	{
		ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"myiweb_xt_ReadTaskBody sBuf malloc fail ");
		sBuf = (char *)MALLOC(1024);
		MEMSET(sBuf,0x00,1024);
	}
	
	sBuf_d = (char *)MALLOC(102400);
	MEMSET(sBuf_d,0x00,102400);

	while (!sBuf_d)
	{
		ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"myiweb_xt_ReadTaskBody sBuf_d malloc fail ");
		sBuf_d = (char *)MALLOC(102400);
    	MEMSET(sBuf_d,0x00,102400);
	}
	
	ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"coming myiweb_xt_ReadTaskBody");

	nRead = ISOURCE_Read(pwa->pSource_xt, sBuf, 1023);
	if(ISOURCE_WAIT == nRead) 
	{
		ISOURCE_Readable(pwa->pSource_xt, &pwa->cb_xt);

		FREE(sBuf);
		sBuf = NULL;
		FREE(sBuf_d);
		sBuf_d = NULL;
		
		if (pwa->m_F_R_C == 0)
		{
			FREE(pwa->m_HTTP_C);
			pwa->m_HTTP_C = NULL;
			pwa->m_F_R_C = 0;
		}
		//			myiweb_Stop_xt(pwa);
		//pwa->m_MUTEX = 0;
		
		return;
	}
	
	if (nRead > 0)		
	{
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," nRead  xt = %d",nRead); 
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf); 
		
		if (nRead > 1)
		{
			sBuf1 = (char *)MALLOC(1024);
			sBuf_T = (char *)MALLOC(102400);
			
			while (!sBuf_T)
			{
				ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," sBuf_T XT Failure to apply for space 1"); 
				sBuf_T = (char *)MALLOC(102400);
			}
			MEMSET(sBuf_T,0x00,102400);
			STRCPY(sBuf_T,sBuf);
		}
		else
		{
			sBuf_T = (char *)MALLOC(1024);
			MEMSET(sBuf_T,0x00,1024);
			STRCPY(sBuf_T,sBuf);
		}
		
		while (nRead > 1)
		{
			MEMSET(sBuf1,0x00,1024);
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"ISOURCE_Read"); 
			nRead = ISOURCE_Read(pwa->pSource_xt, sBuf1, 1023);
			STRCAT(sBuf_T,sBuf1);
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"while nRead  = %d",nRead); 
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf1); 
		}
		
		if(ISOURCE_WAIT == nRead) 
		{
			uf_RecLog(pwa," ISOURCE_Readable ");
			
			if (pwa->m_F_R_C == 1) // ˵��һ�������������ε� ISOURCE_WAIT �ⲻ�ǵ�һ�γ��� 
			{
				STRCAT(pwa->m_HTTP_C,sBuf_T);
			}
			else
			{
				pwa->m_HTTP_C = (char *) MALLOC(102400);
				while (!pwa->m_HTTP_C)
				{
					ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," m_HTTP_C malloc fail"); 
					pwa->m_HTTP_C = (char *)MALLOC(102400);
				}
				MEMSET(pwa->m_HTTP_C,0x00,102400);
				STRCPY(pwa->m_HTTP_C,sBuf_T);
			}
			
			pwa->m_F_R_C = 1; //
			ISOURCE_Readable(pwa->pSource_xt, &pwa->cb_xt);
			
			FREEIF(sBuf_T);
			sBuf_T = NULL ;
			
			FREEIF(sBuf1);
			sBuf1 = NULL ;
			
			FREE(sBuf);
			sBuf = NULL;
			FREE(sBuf_d);
			sBuf_d = NULL;

		
			return;
		}
		
		if (pwa->m_F_R_C == 1 && pwa->m_HTTP_C)
		{
			STRCAT(pwa->m_HTTP_C,sBuf_T);
			uf_RecLog(pwa,"wait time STRCAT HTTP CONTENT");
			uf_RecLog(pwa,"wait time STRCAT HTTP CONTENT");
			MEMSET(sBuf_T,0x00,102400);
			STRCPY(sBuf_T,pwa->m_HTTP_C);
			uf_RecLog(pwa,"wait time 1 STRCAT HTTP CONTENT");
			uf_RecLog(pwa,"wait time 1 STRCAT HTTP CONTENT");
			pwa->m_F_R_C = 0; //
			FREE(pwa->m_HTTP_C);
			pwa->m_HTTP_C = NULL;
		}
		
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," sBuf_T 1 = %d ",STRLEN(sBuf_T)); 
		uf_RecLog(pwa,sBuf_T);
		
		uf_fromESC(sBuf_T,sBuf_d);

//		uf_RecLog(pwa,"11111111111111111111111111111");
		
		si_Ret = ParseStringandWriteToConfigure_xintiao_GPS(pwa,sBuf_d);
		// ����ֵ˵�� ������� 1 - ��ɹ�����������Ȩ - ����Ҫ����д����  0 - �򷵻ص��Ǵ�����Ϣ - ����ʧ��  2 - ��û��Ҫ�ĵģ�����Ҫ����д����  
		// 3 --�����ڿ����ϵ� ������ʧ�� ��Ӧ�����ַ� ��2��      4 -- �������ϴ����ɹ�  5 -- ����Ȩ��δ��Ȩ ����ر�// ����ֵ˵�� ������� 1 - ��ɹ�����������Ȩ - ����Ҫ����д����  0 - �򷵻ص��Ǵ�����Ϣ - ����ʧ��  2 - ��û��Ҫ�ĵģ�����Ҫ����д����  
		// 3 --�����ڿ����ϵ� ������ʧ�� ��Ӧ�����ַ� ��2��      4 -- �������ϴ����ɹ�  5 -- ����Ȩ��δ��Ȩ ����ر�
		
//		uf_RecLog(pwa,"222222222222222222222222222222");

		FREEIF(sBuf_T);
		sBuf_T = NULL ;
		FREEIF(sBuf1);
		sBuf1 = NULL ;

		switch (si_Ret)  //˵���ɹ�
		{
		case 4:
			{
				// ˵������ʧ��
			}
			break;	
		case 1: // 
			{
				pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
				gimis_SetGPSPara(pwa); 

				//uf_RecLog(pwa,"333333333333333333333333333333");
				ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_XT_CG,0, 0);	 //������Ȩ�ɹ��ķ����¼�	
			}
			break;
		case 2: // 
			{
				ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_XT_CG,0, 0);	 //������Ȩ�ɹ��ķ����¼�	
			}
			break;	
			
		case 5:
			{
				pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
				gimis_SetGPSPara(pwa); 	
				// �رճ���
				pwa->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
				ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_WSQ, (uint32*) pwa); //�˳�����
				break;
			}
			/*
			  case 1: // ���ڳɹ�
			  {
			  pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
			  gimis_SetGPSPara(pwa); 
			  Send_GPS_success(pwa); // ��ʾʧ�ܵĽ���
			  ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);	
			  ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"���ڳɹ�2");
			  }
			  case 2: // ���ڳɹ�	
			  {
			  Send_GPS_success(pwa); // ��ʾʧ�ܵĽ���
			  ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);
			  ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"���ڳɹ�1");
			  }
			  break;	
			  case 4:  // �ϴ����ɹ�
			  {
			  // ˵������ʧ��  �ϴ����ɹ�
			  Send_GPS_error(pwa); // ��ʾʧ�ܵĽ���
			  ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);
			  ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"����ʧ��4");
			  }
			  break;	
			  case 3: // ����ʧ��
			  {
			  Send_GPS_error(pwa); // ��ʾʧ�ܵĽ���
			  ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)GPS_Afterpost,pwa);	
			  ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"����ʧ��3");
			  }
			  break;	 
			*/
		}
		
		FREE(sBuf);
		sBuf = NULL;
		
		FREE(sBuf_d);
		sBuf_d = NULL;
		}
		myiweb_Stop_xt(pwa);
		// ��HTTP���� 
		// // m_MUTEX; // 1 = ˵�����ܽ��з���HTTP �Ѿ���ռ����http����Դ  0= ˵������ʹ��http��Դ
		pwa->m_MUTEX = 0;
}



/* myiweb_Header_lx ��������*/
/*===========================================================================

FUNCTION: myiweb_Header

DESCRIPTION:
   Received header callback for a web transaction. cpszName is NULL in the case 
   of continuation header line parts.

PARAMETERS:
   p: a myiweb (the subscriber)
   cpszName: the name of the web header (like "Content-Type")
   pglVal: the value of the header, like "text/html"

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void myiweb_Header_lx(void *p, const char *cpszName, GetLine *pglVal)
{
   newgimis *pwa = (newgimis *)p;
  
  
 //  if (pMe->m_bRS) { // If response is to be displayed
      if ((char *)0 != cpszName) {
        // ND_Print(pMe, "%s:", cpszName);
      }
      //ND_Print(pMe, "%s\n", pglVal->psz);
  // }
}


static void myiweb_Header_gps(void *p, const char *cpszName, GetLine *pglVal)
{
   newgimis *pwa = (newgimis *)p;
  
  
 //  if (pMe->m_bRS) { // If response is to be displayed
      if ((char *)0 != cpszName) {
        // ND_Print(pMe, "%s:", cpszName);
      }
      //ND_Print(pMe, "%s\n", pglVal->psz);
  // }
}


static void myiweb_Header_slx(void *p, const char *cpszName, GetLine *pglVal)
{
   newgimis *pwa = (newgimis *)p;
  
  
 //  if (pMe->m_bRS) { // If response is to be displayed
      if ((char *)0 != cpszName) {
        // ND_Print(pMe, "%s:", cpszName);
      }
      //ND_Print(pMe, "%s\n", pglVal->psz);
  // }
}

/*===========================================================================

FUNCTION: myiweb_Header

DESCRIPTION:
   Received header callback for a web transaction. cpszName is NULL in the case 
   of continuation header line parts.

PARAMETERS:
   p: a myiweb (the subscriber)
   cpszName: the name of the web header (like "Content-Type")
   pglVal: the value of the header, like "text/html"

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void myiweb_Header_xt(void *p, const char *cpszName, GetLine *pglVal)
{
   newgimis *pwa = (newgimis *)p;
  
  
 //  if (pMe->m_bRS) { // If response is to be displayed
      if ((char *)0 != cpszName) {
        // ND_Print(pMe, "%s:", cpszName);
      }
      //ND_Print(pMe, "%s\n", pglVal->psz);
  // }
}

static void myiweb_Header_qqsq(void *p, const char *cpszName, GetLine *pglVal)
{
   newgimis *pwa = (newgimis *)p;
  
  
 //  if (pMe->m_bRS) { // If response is to be displayed
      if ((char *)0 != cpszName) {
        // ND_Print(pMe, "%s:", cpszName);
      }
      //ND_Print(pMe, "%s\n", pglVal->psz);
  // }
}


static void myiweb_Status_qqsq(void *p, WebStatus ws, void *pVal)
{
   AECHAR     szText[32];
   newgimis * pMe = (newgimis *)p;
   
   (void)pVal;

   switch (ws) {
   case WEBS_CANCELLED:
	  ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CANCELLED, szText, sizeof(szText));
      break;
   case WEBS_GETHOSTBYNAME:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GETHOSTBYNAME,szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   2");
      break;          
   case WEBS_CONNECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CONNECT, szText, sizeof(szText));
      break;
   case WEBS_SENDREQUEST:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_SENDREQUEST, szText, sizeof(szText));
      break;
   case WEBS_READRESPONSE:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_READRESPONSE, szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   3");
      break;
   case WEBS_GOTREDIRECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GOTREDIRECT, szText, sizeof(szText));
      break;
   case WEBS_CACHEHIT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CACHEHIT, szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   4");
      break;
   }

   // show that status!
   if ((AECHAR *)0 != szText) 
   {
  //    myiweb_DisplayInfo(pMe,szText);
	  DBGPRINTF("bbbbbbbbbbbbbbbbbbbbbbbbbbb");
   }
}



/* �������� myiweb_Status_lx*/
/*===========================================================================

FUNCTION: myiweb_Status

DESCRIPTION:
   Web status callback for a Web transaction 

PARAMETERS:
   p: a myiweb (the subscriber)
   ws: type of status
   pVal: unused as of yet

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void myiweb_Status_lx(void *p, WebStatus ws, void *pVal)
{
   AECHAR     szText[32];
   newgimis * pMe = (newgimis *)p;
   
   (void)pVal;

   switch (ws) {
   case WEBS_CANCELLED:
	  ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CANCELLED, szText, sizeof(szText));
      break;
   case WEBS_GETHOSTBYNAME:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GETHOSTBYNAME,szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   2");
      break;          
   case WEBS_CONNECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CONNECT, szText, sizeof(szText));
      break;
   case WEBS_SENDREQUEST:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_SENDREQUEST, szText, sizeof(szText));
	  pMe->iPos_kq_zt = GETTIMEMS();
		ISHELL_SetTimer(pMe->a.m_pIShell, 30 * 1000, (PFNNOTIFY)uf_MonitorPost_kq_zt, pMe);
      break;
   case WEBS_READRESPONSE:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_READRESPONSE, szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   3");
      break;
   case WEBS_GOTREDIRECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GOTREDIRECT, szText, sizeof(szText));
      break;
   case WEBS_CACHEHIT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CACHEHIT, szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   4");
      break;
   default:
		if (pMe->iPos_kq_zt == 0) {			//  Add 2013-09-23
		   pMe->iPos_kq_zt = GETTIMEMS();
		   ISHELL_SetTimer(pMe->a.m_pIShell, 30 * 1000, (PFNNOTIFY)uf_MonitorPost_kq_zt, pMe);
		}		// Add End
		break;  
   }

   // show that status!
   if ((AECHAR *)0 != szText) 

   {
  //    myiweb_DisplayInfo(pMe,szText);
	  DBGPRINTF("bbbbbbbbbbbbbbbbbbbbbbbbbbb");
   }
}




static void myiweb_Status_gps(void *p, WebStatus ws, void *pVal)
{
   AECHAR     szText[32];
   newgimis * pMe = (newgimis *)p;
   
   (void)pVal;

   switch (ws) {
   case WEBS_CANCELLED:
	  ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CANCELLED, szText, sizeof(szText));
      break;
   case WEBS_GETHOSTBYNAME:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GETHOSTBYNAME,szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   2");
      break;          
   case WEBS_CONNECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CONNECT, szText, sizeof(szText));
      break;
   case WEBS_SENDREQUEST:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_SENDREQUEST, szText, sizeof(szText));

	  pMe->iPostPhTm_gps = GETTIMEMS();
		ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost_gps, pMe);
      break;
   case WEBS_READRESPONSE:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_READRESPONSE, szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   3");
      break;
   case WEBS_GOTREDIRECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GOTREDIRECT, szText, sizeof(szText));
      break;
   case WEBS_CACHEHIT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CACHEHIT, szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   4");
      break;


	  	default:
		if (pMe->iPostPhTm_gps == 0) {			//  Add 2013-09-23
		   pMe->iPostPhTm_gps = GETTIMEMS();
		   ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost_gps, pMe);
		}		// Add End
		break;
   }

   // show that status!
   if ((AECHAR *)0 != szText) 
   {
  //    myiweb_DisplayInfo(pMe,szText);
	  DBGPRINTF("bbbbbbbbbbbbbbbbbbbbbbbbbbb");
   }
}


static void myiweb_Status_slx(void *p, WebStatus ws, void *pVal)
{
   AECHAR     szText[32];
   newgimis * pMe = (newgimis *)p;
   
   (void)pVal;

   switch (ws) {
   case WEBS_CANCELLED:
	  ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CANCELLED, szText, sizeof(szText));
      break;
   case WEBS_GETHOSTBYNAME:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GETHOSTBYNAME,szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   2");
      break;          
   case WEBS_CONNECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CONNECT, szText, sizeof(szText));
      break;
   case WEBS_SENDREQUEST:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_SENDREQUEST, szText, sizeof(szText));
      break;
   case WEBS_READRESPONSE:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_READRESPONSE, szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   3");
      break;
   case WEBS_GOTREDIRECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GOTREDIRECT, szText, sizeof(szText));
      break;
   case WEBS_CACHEHIT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CACHEHIT, szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   4");
      break;
   }

   // show that status!
   if ((AECHAR *)0 != szText) 
   {
  //    myiweb_DisplayInfo(pMe,szText);
	  DBGPRINTF("bbbbbbbbbbbbbbbbbbbbbbbbbbb");
   }
}



/*===========================================================================

FUNCTION: myiweb_Status

DESCRIPTION:
   Web status callback for a Web transaction 

PARAMETERS:
   p: a myiweb (the subscriber)
   ws: type of status
   pVal: unused as of yet

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void myiweb_Status_xt(void *p, WebStatus ws, void *pVal)
{
   AECHAR     szText[32];
   newgimis * pMe = (newgimis *)p;
   
   (void)pVal;

   switch (ws) {
   case WEBS_CANCELLED:
	  ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CANCELLED, szText, sizeof(szText));
      break;
   case WEBS_GETHOSTBYNAME:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GETHOSTBYNAME,szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   2");
      break;          
   case WEBS_CONNECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CONNECT, szText, sizeof(szText));
      break;
   case WEBS_SENDREQUEST:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_SENDREQUEST, szText, sizeof(szText));
	  pMe->iPostPhTm_xt = GETTIMEMS();
		ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost_xt, pMe);
      break;
   case WEBS_READRESPONSE:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_READRESPONSE, szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   3");
      break;
   case WEBS_GOTREDIRECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GOTREDIRECT, szText, sizeof(szText));
      break;
   case WEBS_CACHEHIT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CACHEHIT, szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   4");
      break;

	default:
		if (pMe->iPostPhTm_xt == 0) {			//  Add 2013-09-23
		   pMe->iPostPhTm_xt = GETTIMEMS();
		   ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost_xt, pMe);
		}		// Add End
		break;
   }

   // show that status!
   if ((AECHAR *)0 != szText) 
   {
  //    myiweb_DisplayInfo(pMe,szText);
	  DBGPRINTF("bbbbbbbbbbbbbbbbbbbbbbbbbbb");
   }

}

/* myiweb_Stop_lx  �������� */

/*===========================================================================

FUNCTION: myiweb_Stop

DESCRIPTION:
   Halts a web transaction, wrapped/represented by a myiweb

PARAMETERS:
   pwa: the myiweb

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void myiweb_Stop_lx(newgimis *pwa)
{
   // this cancels any pending web transaction, or readable on the 
   // response body.  if nothing is pending, this has no effect
   CALLBACK_Cancel(&pwa->cb_lx);
   
   // then clean up, if necessary
   if ((char *)0 != pwa->pszPostData_lx) {

	   FREE(pwa->pszPostData_lx);
		pwa->pszPostData_lx = NULL;
//		pwa->iFileRead = 0;

//      *pwa->pszPostData_xt = 1; // write delimiter back in, if any
 //     pwa->pszPostData_xt = 0;
   }
 // ����  
   if (pwa->pipPostData_lx != NULL)
	{
		IPEEK_Release(pwa->pipPostData_lx);
		// Add by zjie 2011-01-30
		pwa->pipPostData_lx = NULL;

	}
 // ����  
	if (pwa->pSource_lx != NULL) {
		uint32 nErr = ISOURCE_Release(pwa->pSource_lx);	// 2013-04-12 �� nErr
		pwa->pSource_lx = NULL;
	}

   
	if (pwa->piWResp_lx)
	{
		IWEBRESP_Release(pwa->piWResp_lx);
		pwa->piWResp_lx= NULL;
	}

	if (pwa->m_pIWeb_lx) 
	{
//		IWEB_Release(pwa->m_pIWeb_xt);
//		pwa->m_pIWeb_xt = NULL;



		int i,n;			// 2.6.4D2
		n = 0;
		i=IWEB_Release(pwa->m_pIWeb_lx);
		while (i>0) {
			n++;
			i=IWEB_Release(pwa->m_pIWeb_lx);
		}

		pwa->m_pIWeb_lx = NULL;
	}

	if (pwa->m_BodyBuffer_lx)
	{
		FREE(pwa->m_BodyBuffer_lx);
		pwa->m_BodyBuffer_lx = NULL;
	}
	pwa->m_BodySize_lx = 0;
	pwa->m_BodyAllocSize_lx = 0;

}




static void myiweb_Stop_gps(newgimis *pwa)
{
   // this cancels any pending web transaction, or readable on the 
   // response body.  if nothing is pending, this has no effect
   CALLBACK_Cancel(&pwa->cb_gps);
   
   // then clean up, if necessary
   if ((char *)0 != pwa->pszPostData_gps) {

	   FREE(pwa->pszPostData_gps);
		pwa->pszPostData_gps = NULL;
//		pwa->iFileRead = 0;

//      *pwa->pszPostData_xt = 1; // write delimiter back in, if any
 //     pwa->pszPostData_xt = 0;
   }
 // ����  
   if (pwa->pipPostData_gps != NULL)
	{
		IPEEK_Release(pwa->pipPostData_gps);
		// Add by zjie 2011-01-30
		pwa->pipPostData_gps = NULL;

	}
 // ����  
	if (pwa->pSource_gps != NULL) {
		uint32 nErr = ISOURCE_Release(pwa->pSource_gps);	// 2013-04-12 �� nErr
		//ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"myiweb_Stop_gps ISOURCE_Release nErr = %d",nErr);
		pwa->pSource_gps = NULL;
	}

   
	if (pwa->piWResp_gps)
	{
		IWEBRESP_Release(pwa->piWResp_gps);
		pwa->piWResp_gps= NULL;
	}

	if (pwa->m_pIWeb_gps) 
	{
//		IWEB_Release(pwa->m_pIWeb_xt);
//		pwa->m_pIWeb_xt = NULL;



		int i,n;			// 2.6.4D2
		n = 0;
		i=IWEB_Release(pwa->m_pIWeb_gps);
		while (i>0) {
			n++;
			i=IWEB_Release(pwa->m_pIWeb_gps);
		}

		pwa->m_pIWeb_gps = NULL;
	}

	if (pwa->m_BodyBuffer_gps)
	{
		FREE(pwa->m_BodyBuffer_gps);
		pwa->m_BodyBuffer_gps = NULL;
	}
	pwa->m_BodySize_gps = 0;
	pwa->m_BodyAllocSize_gps = 0;

}





/*===========================================================================

FUNCTION: myiweb_Stop

DESCRIPTION:
   Halts a web transaction, wrapped/represented by a myiweb

PARAMETERS:
   pwa: the myiweb

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void myiweb_Stop_slx(newgimis *pwa)
{
   // this cancels any pending web transaction, or readable on the 
   // response body.  if nothing is pending, this has no effect
   CALLBACK_Cancel(&pwa->cb_slx);
   
   // then clean up, if necessary
   if ((char *)0 != pwa->pszPostData_slx) {

	   FREE(pwa->pszPostData_slx);
		pwa->pszPostData_slx = NULL;
//		pwa->iFileRead = 0;

//      *pwa->pszPostData_xt = 1; // write delimiter back in, if any
 //     pwa->pszPostData_xt = 0;
   }
 // ����  
   if (pwa->pipPostData_slx != NULL)
	{
		IPEEK_Release(pwa->pipPostData_slx);
		// Add by zjie 2011-01-30
		pwa->pipPostData_slx = NULL;

	}
 // ����  
	if (pwa->pSource_slx != NULL) {
		uint32 nErr = ISOURCE_Release(pwa->pSource_slx);	// 2013-04-12 �� nErr
		pwa->pSource_slx = NULL;
	}

   
	if (pwa->piWResp_slx)
	{
		IWEBRESP_Release(pwa->piWResp_slx);
		pwa->piWResp_slx= NULL;
	}

	if (pwa->m_pIWeb_slx) 
	{
//		IWEB_Release(pwa->m_pIWeb_xt);
//		pwa->m_pIWeb_xt = NULL;



		int i,n;			// 2.6.4D2
		n = 0;
		i=IWEB_Release(pwa->m_pIWeb_slx);
		while (i>0) {
			n++;
			i=IWEB_Release(pwa->m_pIWeb_slx);
		}

		pwa->m_pIWeb_slx = NULL;
	}

	if (pwa->m_BodyBuffer_slx)
	{
		FREE(pwa->m_BodyBuffer_slx);
		pwa->m_BodyBuffer_slx = NULL;
	}
	pwa->m_BodySize_slx = 0;
	pwa->m_BodyAllocSize_slx = 0;

}



// myiweb_Stop_qqsq start
static void myiweb_Stop_qqsq(newgimis *pwa)
{
   // this cancels any pending web transaction, or readable on the 
   // response body.  if nothing is pending, this has no effect
   CALLBACK_Cancel(&pwa->cb_qqsq);
   
   // then clean up, if necessary
   if ((char *)0 != pwa->pszPostData_qqsq) {

	   FREE(pwa->pszPostData_qqsq);
		pwa->pszPostData_qqsq = NULL;
//		pwa->iFileRead = 0;

//      *pwa->pszPostData_qqsq = 1; // write delimiter back in, if any
 //     pwa->pszPostData_qqsq = 0;
   }
 // ����  
   if (pwa->pipPostData_qqsq != NULL)
	{
		IPEEK_Release(pwa->pipPostData_qqsq);
		// Add by zjie 2011-01-30
		pwa->pipPostData_qqsq = NULL;

	}
 // ����  
	if (pwa->pSource_qqsq != NULL) {
		uint32 nErr = ISOURCE_Release(pwa->pSource_qqsq);	// 2013-04-12 �� nErr
		pwa->pSource_qqsq = NULL;
	}

   
	if (pwa->piWResp_qqsq)
	{
		IWEBRESP_Release(pwa->piWResp_qqsq);
		pwa->piWResp_qqsq= NULL;
	}

	if (pwa->m_pIWeb_qqsq) 
	{
//		IWEB_Release(pwa->m_pIWeb_qqsq);
//		pwa->m_pIWeb_qqsq = NULL;



		int i,n;			// 2.6.4D2
		n = 0;
		i=IWEB_Release(pwa->m_pIWeb_qqsq);
		while (i>0) {
			n++;
			i=IWEB_Release(pwa->m_pIWeb_qqsq);
		}

		pwa->m_pIWeb_qqsq = NULL;
	}

	if (pwa->m_BodyBuffer_qqsq)
	{
		FREE(pwa->m_BodyBuffer_qqsq);
		pwa->m_BodyBuffer_qqsq = NULL;
	}
	pwa->m_BodySize_qqsq = 0;
	pwa->m_BodyAllocSize_qqsq = 0;

}
// myiweb_Stop_qqsq end 

//////////////////////////////////////////////////////////


/*===========================================================================

FUNCTION: myiweb_Stop

DESCRIPTION:
   Halts a web transaction, wrapped/represented by a myiweb

PARAMETERS:
   pwa: the myiweb

DEPENDENCIES:
   None

RETURN VALUE:
   None

SIDE EFFECTS:
   None
===========================================================================*/
static void myiweb_Stop_xt(newgimis *pwa)
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
 // ����  
   if (pwa->pipPostData_xt != NULL)
	{
		IPEEK_Release(pwa->pipPostData_xt);
		// Add by zjie 2011-01-30
		pwa->pipPostData_xt = NULL;

	}
 // ����  
	if (pwa->pSource_xt != NULL) {
		uint32 nErr = ISOURCE_Release(pwa->pSource_xt);	// 2013-04-12 �� nErr
		//ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"myiweb_Stop_xt ISOURCE_Release nErr = %d",nErr);
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

		//////////////  9-25  ��ʱ�Ȳ��������  start //////////////
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


//////////////////////////////////////////////////////////



// ��ʾ�ֻ�״̬
void uf_PhoneSts(newgimis *pMe)
{
	DBGPRINTF( "zjie-uf_PhoneSts : %d", pMe->cPhoneSts);
	
	switch (gimis_HasStatus(pMe,USR_NMASK_SS_TAPI))					// pMe->cPhoneSts
	{
	case 0:
		testgps_Printf( pMe, 4, 1, AEE_FONT_NORMAL, IDF_ALIGN_LEFT|IDF_RECT_FILL, "status:OFFLINE");  //��������״̬
		break;
	case 1:
		testgps_Printf( pMe, 4, 1, AEE_FONT_NORMAL, IDF_ALIGN_LEFT|IDF_RECT_FILL, "status:IDLE");  // ���ڿ���״̬
		break;
	case 2:
		testgps_Printf( pMe, 4, 1, AEE_FONT_NORMAL, IDF_ALIGN_LEFT|IDF_RECT_FILL, "status:INCOMING");  // ����������״̬  ����һ��ָ���豸�Ľ������
		break;
	case 3:
		testgps_Printf( pMe, 4, 1, AEE_FONT_NORMAL, IDF_ALIGN_LEFT|IDF_RECT_FILL, "status:ORIG");  // �������ڽ�����·״̬  �豸���ڼ���ͨ��
		break;
	case 4:
		testgps_Printf( pMe, 4, 1, AEE_FONT_NORMAL, IDF_ALIGN_LEFT|IDF_RECT_FILL, "status:CONVERSATION");  //  ���ڻỰ״̬����������ʹ�ã� �豸����ͨ����
		break;
	}
	
	return;
}

// һ���򿨣�����ת�����й� 
void newgimis_forYJDK(newgimis * pMe,boolean bGet)
{
	// Change   ���û�������򱣴���������

	char   strForwardIp[150];// ת����ַ
	int idx = 0;
	
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	MEMSET(pMe->lx_vv,0x00,MAXTEXTLEN);
	MEMSET(pMe->lx_tt,0x00,15);				// TT ʱ��
	
	SPRINTF(pMe->lx_vv,"0");
	GETJULIANDATE(0,&dDate);		//	ȡ������
	SPRINTF(pMe->lx_tt,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	//                      LO LA VV EP KS PM TT
	if (pMe->cPosDetMode != 13)
	{
		switch (pMe->m_gpsFlag)
		{
		case 0:
			pMe->m_PM = '2'; 
			break;
		case 1:
			pMe->m_PM = '5'; 
			break;
		case 2:
			pMe->m_PM = '1'; 
			break;
		case 3:
			pMe->m_PM = '0'; 
			break;
		case 4:
			pMe->m_PM = '9'; 
			break;
		case 5:
			pMe->m_PM = '6'; 
			break;
		}
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS2;PM%c;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor,pMe->lx_ep,pMe->m_PM ,pMe->lx_tt);
	}
	else
	{
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS2;PM%d;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor_BD,pMe->lx_ep,pMe->m_PM_BD ,pMe->lx_tt);
	}
	if ((pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 11) )//˵���ǽ���һ���򿨽���
	{
		pMe->m_Flag_Key = 0;
		pMe->m_Witch_Key = -1;
	}
	
	
	//	if ( STRLEN(pMe->sUrl_yjkq) == 0 ) 
	if ( STRLEN(pMe->lx_lat) == 0  && STRLEN(pMe->lx_lon) == 0) 
	{
		// ˵��û�п�������
		uf_AEPrint_YJDK_Back(pMe,FALSE);		
		return;
	}
	
	if (pMe->m_F_QF)  // ��ʾ�ֻ�Ƿ��
	{
		uf_AEPrint_NO_NET_Back(pMe,FALSE);
		//Save_GPSData_All( pMe );
		return;
	}
	
	MEMSET(strForwardIp,0x00,150);// һ�����ڱ�������
	idx = ud_DistToChar(pMe->strCenterIp,'/');  
	if ( idx >= 0 )
		MEMCPY(strForwardIp, pMe->strCenterIp, idx); 
	else
	{
		//STRCPY(strForwardIp, pMe->strCenterIp);
		// ˵��û�п�������
		uf_AEPrint_YJDK_Back(pMe,FALSE);		
		return;
	}

	MEMSET(pMe->sUrl_Cyjkq,0x00,200);// һ�����ڱ�������
	
	if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
	{
		SPRINTF(pMe->sUrl_Cyjkq,"http://%s/forwardAgent?&ver=%s&forwarddata=%s&tel=%s&imsi=%s",strForwardIp,pMe->sVer_1,pMe->sUrl_yjkq,pMe->MyMobileNo,pMe->szMobileID);
	}
	else
	{
		SPRINTF(pMe->sUrl_Cyjkq,"http://%s/forwardAgent?&ver=%s&forwarddata=%s&imsi=%s",strForwardIp,pMe->sVer_1,pMe->sUrl_yjkq,pMe->szMobileID);
	}
	
	uf_AEPrint_Sending(pMe,FALSE);
	
	uf_RecLog(pMe,pMe->sUrl_Cyjkq);
	pMe->m_Flag_F = 6; // ������ǵ�ǰ������������ʲô����  1 -- һ������      2 -- ���Ϳճ�     3 --  �����س�  6--һ����

	myiweb_Stop_lx(pMe);
	myiweb_Start_lx(pMe, pMe->sUrl_Cyjkq,0);
	//	}	// if -- 2
	//	else  // û�����磬������Ϣ
	//	{
	//		uf_AEPrint_NO_NET_Back(pMe,FALSE);
	//		Save_GPSData_All( pMe );	
	//	}
}

// �뷢�Ϳճ��й�
void newgimis_SendEmpty(newgimis * pMe,boolean bGet)
{
	// Change   ���û�������򱣴���������
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	MEMSET(pMe->lx_vv,0x00,MAXTEXTLEN);
	MEMSET(pMe->lx_tt,0x00,15);				// TT ʱ��

	SPRINTF(pMe->lx_vv,"0");
	GETJULIANDATE(0,&dDate);		//	ȡ������
	SPRINTF(pMe->lx_tt,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	//                      LO LA VV EP KS PM TT
	if (pMe->cPosDetMode != 13)
	{
		switch (pMe->m_gpsFlag)
		{
		case 0:
			pMe->m_PM = '2'; 
			break;
		case 1:
			pMe->m_PM = '5'; 
			break;
		case 2:
			pMe->m_PM = '1'; 
			break;
		case 3:
			pMe->m_PM = '0'; 
			break;
		case 4:
			pMe->m_PM = '9'; 
			break;
		case 5:
			pMe->m_PM = '6'; 
			break;
		}
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS0;PM%c;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor,pMe->lx_ep,pMe->m_PM ,pMe->lx_tt);
	}
	else
	{
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS0;PM%d;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor_BD,pMe->lx_ep,pMe->m_PM_BD ,pMe->lx_tt);
	}

	

	
//	if ( STRLEN(pMe->sUrl_yjkq) == 0 ) 
	if ( STRLEN(pMe->lx_lat) == 0  && STRLEN(pMe->lx_lon) == 0) 
	{
		// ˵��û�п�������
		uf_AEPrint_YJKQ_Back_Two(pMe,FALSE);		
		return;
	}


	if ((pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 4) )//˵���ǽ��뷢�Ϳճ�����
	{
		pMe->m_Flag_Key = 0;
		pMe->m_Witch_Key = -1;
	}


//	if (uf_CheckNetSts(pMe)) 
//	{	// if -- 1 ˵��������
		if (pMe->m_F_QF) // ��ʾ�ֻ�Ƿ��
		{
			uf_AEPrint_NO_NET_Back_Two(pMe,FALSE);	
			return;
		}
		// ���Ϳ�����Ϣ
		MEMSET(pMe->sUrl_Cyjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_Cyjkq,"http://%s&ver=%s&pp=%s&tel=%s&imsi=%s",pMe->strCenterIp,pMe->sVer_1,pMe->sUrl_yjkq,pMe->MyMobileNo,pMe->szMobileID);
		}
		else
		{
			SPRINTF(pMe->sUrl_Cyjkq,"http://%s&ver=%s&pp=%s&imsi=%s",pMe->strCenterIp,pMe->sVer_1,pMe->sUrl_yjkq,pMe->szMobileID);
		}
		uf_AEPrint_Sending(pMe,FALSE);

		uf_RecLog(pMe,pMe->sUrl_Cyjkq);

		pMe->m_Flag_F = 2; // ������ǵ�ǰ������������ʲô����  1 -- һ������      2 -- ���Ϳճ�     3 --  �����س�
		myiweb_Stop_lx(pMe);
		myiweb_Start_lx(pMe, pMe->sUrl_Cyjkq,0);
//	}	// if -- 2
//	else  // û�����磬������Ϣ
//	{
//		uf_AEPrint_NO_NET_Back_Two(pMe,FALSE);	
//	}
}

// newgimis_Send_ZT_0 START //
// �뷢�Ϳճ��й�
void newgimis_Send_ZT_0(newgimis * pMe,boolean bGet)
{
	// Change   ���û�������򱣴���������
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	MEMSET(pMe->lx_vv,0x00,MAXTEXTLEN);
	MEMSET(pMe->lx_tt,0x00,15);				// TT ʱ��

	SPRINTF(pMe->lx_vv,"0");
	GETJULIANDATE(0,&dDate);		//	ȡ������
	SPRINTF(pMe->lx_tt,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	//                      LO LA VV EP KS PM TT
	if (pMe->cPosDetMode != 13)
	{
		switch (pMe->m_gpsFlag)
		{
		case 0:
			pMe->m_PM = '2'; 
			break;
		case 1:
			pMe->m_PM = '5'; 
			break;
		case 2:
			pMe->m_PM = '1'; 
			break;
		case 3:
			pMe->m_PM = '0'; 
			break;
		case 4:
			pMe->m_PM = '9'; 
			break;
		case 5:
			pMe->m_PM = '6'; 
			break;
		}
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS0;PM%c;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor,pMe->lx_ep,pMe->m_PM ,pMe->lx_tt);
	}
	else
	{
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS0;PM%d;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor_BD,pMe->lx_ep,pMe->m_PM_BD ,pMe->lx_tt);
	}

	

	
//	if ( STRLEN(pMe->sUrl_yjkq) == 0 ) 
	if ( STRLEN(pMe->lx_lat) == 0  && STRLEN(pMe->lx_lon) == 0) 
	{
		// ˵��û�п�������
		uf_AEPrint_YJKQ_Back_Two(pMe,FALSE);		
		return;
	}


	if ((pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 10) )//˵���ǽ��뷢�Ϳճ�����
	{
		pMe->m_Flag_Key = 0;
		pMe->m_Witch_Key = -1;
	}


//	if (uf_CheckNetSts(pMe)) 
//	{	// if -- 1 ˵��������
		if (pMe->m_F_QF) // ��ʾ�ֻ�Ƿ��
		{
			uf_AEPrint_NO_NET_Back_Two(pMe,FALSE);	
			return;
		}
		// ���Ϳ�����Ϣ
		MEMSET(pMe->sUrl_Cyjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_Cyjkq,"http://%s&ver=%s&pp=%s&tel=%s&imsi=%s",pMe->strCenterIp,pMe->sVer_1,pMe->sUrl_yjkq,pMe->MyMobileNo,pMe->szMobileID);
		}
		else
		{
			SPRINTF(pMe->sUrl_Cyjkq,"http://%s&ver=%s&pp=%s&imsi=%s",pMe->strCenterIp,pMe->sVer_1,pMe->sUrl_yjkq,pMe->szMobileID);
		}
		uf_AEPrint_Sending(pMe,FALSE);

		uf_RecLog(pMe,pMe->sUrl_Cyjkq);

		pMe->m_Flag_F = 4; // ������ǵ�ǰ������������ʲô����  1 -- һ������      2 -- ���Ϳճ�     3 --  �����س�
		myiweb_Stop_lx(pMe);
		myiweb_Start_lx(pMe, pMe->sUrl_Cyjkq,0);
//	}	// if -- 2
//	else  // û�����磬������Ϣ
//	{
//		uf_AEPrint_NO_NET_Back_Two(pMe,FALSE);	
//	}
}


// newgimis_Send_ZT_0  END  //
// �뷢���س��й�
void newgimis_SendFull(newgimis * pMe,boolean bGet)
{
	// Change   ���û�������򱣴���������
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	MEMSET(pMe->lx_vv,0x00,MAXTEXTLEN);
	MEMSET(pMe->lx_tt,0x00,15);				// TT ʱ��

	SPRINTF(pMe->lx_vv,"0");
	GETJULIANDATE(0,&dDate);		//	ȡ������
	SPRINTF(pMe->lx_tt,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	//                      LO LA VV EP KS PM TT
	if (pMe->cPosDetMode != 13)
	{
		switch (pMe->m_gpsFlag)
		{
		case 0:
			pMe->m_PM = '2'; 
			break;
		case 1:
			pMe->m_PM = '5'; 
			break;
		case 2:
			pMe->m_PM = '1'; 
			break;
		case 3:
			pMe->m_PM = '0'; 
			break;
		case 4:
			pMe->m_PM = '9'; 
			break;
		case 5:
			pMe->m_PM = '6'; 
			break;
		}
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS0;PM%c;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor,pMe->lx_ep,pMe->m_PM ,pMe->lx_tt);
	}
	else
	{
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS0;PM%d;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor_BD,pMe->lx_ep,pMe->m_PM_BD ,pMe->lx_tt);
	}

	
	if ((pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 3) )//˵���ǽ��뷢���س�����
	{
		pMe->m_Flag_Key = 0;
		pMe->m_Witch_Key = -1;
	}
	

//	if ( STRLEN(pMe->sUrl_yjkq) == 0 ) 
	if ( STRLEN(pMe->lx_lat) == 0  && STRLEN(pMe->lx_lon) == 0) 
	{
		// ˵��û�п�������
		uf_AEPrint_YJKQ_Back_Two(pMe,FALSE);		
		return;
	}




//	if (uf_CheckNetSts(pMe)) 
//	{	// if -- 1 ˵��������
		// ���Ϳ�����Ϣ
		if (pMe->m_F_QF)  // ��ʾ�ֻ�Ƿ��
		{
			uf_AEPrint_NO_NET_Back_Two(pMe,FALSE);	
			return;
		}
		MEMSET(pMe->sUrl_Cyjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_Cyjkq,"http://%s&ver=%s&pp=%s&tel=%s&imsi=%s",pMe->strCenterIp,pMe->sVer_1,pMe->sUrl_yjkq,pMe->MyMobileNo,pMe->szMobileID);
		}
		else
		{
			SPRINTF(pMe->sUrl_Cyjkq,"http://%s&ver=%s&pp=%s&imsi=%s",pMe->strCenterIp,pMe->sVer_1,pMe->sUrl_yjkq,pMe->szMobileID);
		}

		uf_AEPrint_Sending(pMe,FALSE);

		uf_RecLog(pMe,pMe->sUrl_Cyjkq);

		pMe->m_Flag_F = 3; // ������ǵ�ǰ������������ʲô����  1 -- һ������      2 -- ���Ϳճ�     3 --  �����س�
		myiweb_Stop_lx(pMe);
		myiweb_Start_lx(pMe, pMe->sUrl_Cyjkq,0);
//	}	// if -- 2
//	else  // û�����磬������Ϣ
//	{
//		uf_AEPrint_NO_NET_Back_Two(pMe,FALSE);	
//	}
}

// newgimis_Send_ZT_1 START //
// ����״̬ 1
void newgimis_Send_ZT_1(newgimis * pMe,boolean bGet)
{
	// Change   ���û�������򱣴���������
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	MEMSET(pMe->lx_vv,0x00,MAXTEXTLEN);
	MEMSET(pMe->lx_tt,0x00,15);				// TT ʱ��

	SPRINTF(pMe->lx_vv,"0");
	GETJULIANDATE(0,&dDate);		//	ȡ������
	SPRINTF(pMe->lx_tt,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	//                      LO LA VV EP KS PM TT
	if (pMe->cPosDetMode != 13)
	{
		switch (pMe->m_gpsFlag)
		{
		case 0:
			pMe->m_PM = '2'; 
			break;
		case 1:
			pMe->m_PM = '5'; 
			break;
		case 2:
			pMe->m_PM = '1'; 
			break;
		case 3:
			pMe->m_PM = '0'; 
			break;
		case 4:
			pMe->m_PM = '9'; 
			break;
		case 5:
			pMe->m_PM = '6'; 
			break;
		}
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS0;PM%c;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor,pMe->lx_ep,pMe->m_PM ,pMe->lx_tt);
	}
	else
	{
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS0;PM%d;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor_BD,pMe->lx_ep,pMe->m_PM_BD ,pMe->lx_tt);
	}

	
	if ((pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 9) )//˵���ǽ��뷢���س�����
	{
		pMe->m_Flag_Key = 0;
		pMe->m_Witch_Key = -1;
	}
	

//	if ( STRLEN(pMe->sUrl_yjkq) == 0 ) 
	if ( STRLEN(pMe->lx_lat) == 0  && STRLEN(pMe->lx_lon) == 0) 
	{
		// ˵��û�п�������
		uf_AEPrint_YJKQ_Back_Two(pMe,FALSE);		
		return;
	}




//	if (uf_CheckNetSts(pMe)) 
//	{	// if -- 1 ˵��������
		// ���Ϳ�����Ϣ
		if (pMe->m_F_QF)  // ��ʾ�ֻ�Ƿ��
		{
			uf_AEPrint_NO_NET_Back_Two(pMe,FALSE);	
			return;
		}
		MEMSET(pMe->sUrl_Cyjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_Cyjkq,"http://%s&ver=%s&pp=%s&tel=%s&imsi=%s",pMe->strCenterIp,pMe->sVer_1,pMe->sUrl_yjkq,pMe->MyMobileNo,pMe->szMobileID);
		}
		else
		{
			SPRINTF(pMe->sUrl_Cyjkq,"http://%s&ver=%s&pp=%s&imsi=%s",pMe->strCenterIp,pMe->sVer_1,pMe->sUrl_yjkq,pMe->szMobileID);
		}

		uf_AEPrint_Sending(pMe,FALSE);

		uf_RecLog(pMe,pMe->sUrl_Cyjkq);

		pMe->m_Flag_F = 5; // ������ǵ�ǰ������������ʲô����  1 -- һ������      2 -- ���Ϳճ�     3 --  �����س� 
		myiweb_Stop_lx(pMe);
		myiweb_Start_lx(pMe, pMe->sUrl_Cyjkq,0);
//	}	// if -- 2
//	else  // û�����磬������Ϣ
//	{
//		uf_AEPrint_NO_NET_Back_Two(pMe,FALSE);	
//	}
}
// newgimis_Send_ZT_1 END //


// ��һ�������й�
void newgimis_forYJKQ(newgimis * pMe,boolean bGet)
{
	// Change   ���û�������򱣴���������

	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	MEMSET(pMe->lx_vv,0x00,MAXTEXTLEN);
	MEMSET(pMe->lx_tt,0x00,15);				// TT ʱ��

	SPRINTF(pMe->lx_vv,"0");
	GETJULIANDATE(0,&dDate);		//	ȡ������
	SPRINTF(pMe->lx_tt,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	//                      LO LA VV EP KS PM TT
	if (pMe->cPosDetMode != 13)
	{
		switch (pMe->m_gpsFlag)
		{
		case 0:
			pMe->m_PM = '2'; 
			break;
		case 1:
			pMe->m_PM = '5'; 
			break;
		case 2:
			pMe->m_PM = '1'; 
			break;
		case 3:
			pMe->m_PM = '0'; 
			break;
		case 4:
			pMe->m_PM = '9'; 
			break;
		case 5:
			pMe->m_PM = '6'; 
			break;
		}
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS2;PM%c;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor,pMe->lx_ep,pMe->m_PM ,pMe->lx_tt);
	}
	else
	{
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS2;PM%d;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor_BD,pMe->lx_ep,pMe->m_PM_BD ,pMe->lx_tt);
	}
	if ((pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 2) )//˵���ǽ��뷢����Ƭ����
	{
		pMe->m_Flag_Key = 0;
		pMe->m_Witch_Key = -1;
	}


//	if ( STRLEN(pMe->sUrl_yjkq) == 0 ) 
	if ( STRLEN(pMe->lx_lat) == 0  && STRLEN(pMe->lx_lon) == 0) 
	{
		// ˵��û�п�������
		uf_AEPrint_YJKQ_Back(pMe,FALSE);		
		return;
	}




//	if (uf_CheckNetSts(pMe)) 
//	{	// if -- 1 ˵��������
		// ���Ϳ�����Ϣ
		if (pMe->m_F_QF)  // ��ʾ�ֻ�Ƿ��
		{
			uf_AEPrint_NO_NET_Back(pMe,FALSE);
			Save_GPSData_All( pMe );
			return;
		}
		MEMSET(pMe->sUrl_Cyjkq,0x00,200);// һ�����ڱ�������
		if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
		{
			SPRINTF(pMe->sUrl_Cyjkq,"http://%s&ver=%s&pp=%s&tel=%s&imsi=%s",pMe->strCenterIp,pMe->sVer_1,pMe->sUrl_yjkq,pMe->MyMobileNo,pMe->szMobileID);
		}
		else
		{
			SPRINTF(pMe->sUrl_Cyjkq,"http://%s&ver=%s&pp=%s&imsi=%s",pMe->strCenterIp,pMe->sVer_1,pMe->sUrl_yjkq,pMe->szMobileID);
		}

		uf_AEPrint_Sending(pMe,FALSE);

		uf_RecLog(pMe,pMe->sUrl_Cyjkq);
		pMe->m_Flag_F = 1; // ������ǵ�ǰ������������ʲô����  1 -- һ������      2 -- ���Ϳճ�     3 --  �����س�
		myiweb_Stop_lx(pMe);
		myiweb_Start_lx(pMe, pMe->sUrl_Cyjkq,0);
//	}	// if -- 2
//	else  // û�����磬������Ϣ
//	{
//		uf_AEPrint_NO_NET_Back(pMe,FALSE);
//		Save_GPSData_All( pMe );	
//	}
}


// ������Ƭ���ͽ���
void newgimis_forphoto(newgimis * pMe,boolean bGet)
{
	// Change by zjie 2011-11-18  ���û����������Ҫ��ʾ��Ƭ
	
	// ��ȡ��Ƭ
	
	if ((pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 5) )//˵���ǽ��뷢����Ƭ����
	{
		pMe->m_Flag_Key = 0;
		pMe->m_Witch_Key = -1;
	}
	
	//	 pMe->cUsePhoto = '7'; // �ж���û�и�Ȩ�� 
	if (pMe->cUsePhoto == '1' || pMe->cUsePhoto == '2' || pMe->cUsePhoto == '5' || pMe->cUsePhoto == '7')
	{
		// ��������
	}
	else
	{
		//û�п����ù���
		pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
		uf_AEPrint_Photo_Back(pMe,FALSE);		// û�п����ù���
		return;
	}
	newgimis_GetPhote(pMe);
	if ( STRLEN(pMe->m_photo[0].photoname) == 0 ) 
	{
		// ˵��û����Ƭ
		pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
		uf_AEPrintPhoto_Back(pMe,FALSE);		// һ�Ŷ�û��û��
		return;
	}
	
	//	if (uf_CheckNetSts(pMe)) {	// if -- 1
	if (pMe->m_F_QF) // �ֻ�Ƿ��
	{
		pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
		uf_AEPrint_NO_NET_Back(pMe,FALSE);
		return;
	}
	
	
	if (bGet) 
	{
		pMe->m_WitchPag = 2; // ������Ƭ����
		testgps_GotoScreen( pMe, SCREENID_SENDPHOTO, 0 );
	}
	// ��ȡ��Ƭ
	//		newgimis_GetPhote(pMe);
	
	// ��ʾ��Ƭ 
	if ( STRLEN(pMe->m_photo[0].photoname) > 0 ) 
	{
		uf_RecLog(pMe,"Find Send photo" );
		STRCPY(pMe->sFileName,pMe->m_photo[0].photoname);
		pMe->m_i_photo = pMe->m_photo[0].i_dex;
		
		pMe->pi = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);
		//if (STRCMP(pMe->sMobileType,"01") == 0 || STRCMP(pMe->sMobileType,"15") == 0)
		if (STRCMP(pMe->sMobileType,"01") == 0 /*|| (STRCMP(pMe->sMobileType,"02") == 0)*/ || STRCMP(pMe->sMobileType,"15") == 0 || STRCMP(pMe->sMobileType,"33") == 0 )  //X3��Ƭ���뷨
		{
			ISHELL_SetTimer(pMe->a.m_pIShell, 1000, (PFNNOTIFY)Load_Image_fun, pMe);
			//			pMe->pi_copy = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);
		}
		if (pMe->pi) 		
		{
			IImage_Notify(pMe->pi, (PFNIMAGEINFO)uf_ImageNotify,pMe);  //ע��ص�
		}
		
	}
	else 
	{
		uf_RecLog(pMe,"can not Find Send photo" );
		pMe->m_i_photo = 0; //û��Ҫ��ʾ����Ƭ
		pMe->imgidx = 0;		   // �ֻ���û����Ƭ
		uf_AEPrint(pMe,FALSE);		// һ�Ŷ�û��û��
	}
	
	return;
	
	// ��ʾ��Ƭ
	pMe->iPressSts = 14;
	//  Mark by zjie  2012-11-27  ��Ϊ��uf_GetNewestPhotoForSend�л�ȡ������Ƭ	pMe->imgidx = 1;
	// Change by zjie 2012-05-29, ��Ϊ����������Ƭ
	//		pMe->iShowText = 1;
	if (pMe->cPhotoSend == '1')
	{
		pMe->iShowText = 2;
		pMe->iSendPhoto = 1;	 // ������Ƭ
	}
	else
		pMe->iShowText = 1;
	// Change End
	//		uf_SetPhote(pMe);
	//	}	// if -- 1
	//	else {		// if -- 2
	// ��ʾû������
	//		uf_AEPrint_NO_NET_Back(pMe,FALSE);
	//	}	// if -- 2
}


/*===========================================================================
   ����������Ƭ�Ĺ���
boolean bGet :  true  // Change by zjie 2012-11-27    "����Ƭ��������Ŀ¼��" ��Ϊ"�ҵ����µ���Ƭ"  
===========================================================================*/
void uf_forphoto(newgimis * pMe,boolean bGet)
{
	// Change by zjie 2011-11-18  ���û����������Ҫ��ʾ��Ƭ
	if (uf_CheckNetSts(pMe)) {	// if -- 1
		if (pMe->m_F_QF) // ��ʾ�ֻ�Ƿ��
		{
			return;
		}
		if (bGet) {

			pMe->iPressSts = 13;
		 // ����Ƭ��������Ŀ¼��
			DBGPRINTF( "zjie-photo 2" );
		 // Change by zjie  2012-11-27	
		 //	uf_GetPhote(pMe);
//			if (pMe->cPhotoSend != '1')	 {
//				uf_GetNewestPhotoForSend(pMe);
//				if (pMe->imgidx == 0)
//					return;
//			}
	     // Change End
		 
		}
	
		// ��ʾ��Ƭ
		pMe->iPressSts = 14;
		//  Mark by zjie  2012-11-27  ��Ϊ��uf_GetNewestPhotoForSend�л�ȡ������Ƭ	pMe->imgidx = 1;
		// Change by zjie 2012-05-29, ��Ϊ����������Ƭ
		//		pMe->iShowText = 1;
		if (pMe->cPhotoSend == '1') {
			pMe->iShowText = 2;
			pMe->iSendPhoto = 1;	 // ������Ƭ
		}
		else
			pMe->iShowText = 1;
		// Change End
		uf_SetPhote(pMe);
	}	// if -- 1
	else {		// if -- 2
		// ��ʾû������
		/*
		MEMSET(pMe->sFile_2,0,sizeof(pMe->sFile_2));
		SPRINTF(pMe->sFile_2,"%s", IMG_FILE_NONET);  // SPRINTF(pMe->sFile_2,AEEFS_ROOT_DIR"%s%s", IMG_PATH,IMG_FILE_NONET);
		
		if ( !uf_CheckFile(pMe,pMe->sFile_2) )
			return;
		
		DBGPRINTF( "zjie-forphoto 3");
		pMe->nActiveWin = 0;
		pMe->iPressSts = 2;
														//		pMe->m_bOldGoBg_2 = pMe->m_bOldGoBg;
		if ( (pMe->iErrCount/100) != 19 )	// Add ������ 2013-03-27
			CALLBACK_Init(&pMe->cbCallback,uf_ShowStatus,pMe);
		uf_ShowStatus(pMe);
		*/
	}	// if -- 2

}









// =============================================================
// �ص�������ʾͼ��
// =============================================================
void uf_ImageNotify(newgimis * pMe,IImage *pIImage, AEEImageInfo * pi, int nErr ) 
{ 

	AECHAR s_Tmp[100];
	char *sc_tmp;
	int si_tmp;
	int i_tmp = 0;
	
	if ( (pMe->pi == pIImage) && (!nErr) )
	{
/*  ��ʱע�͵�
		if (pMe->iShowText != 4 ) {	// if -- 4
			IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
			uf_ShowText(pMe);
		}	// if -- 4
*/
		//��ʾ��Ƭ����
		sc_tmp = (char *)MALLOC(100);
		MEMSET(sc_tmp,0x00,100);
		si_tmp = ud_rDistToChar(pMe->m_photo[pMe->m_i_photo-1].photoname,'/');
		if (si_tmp >= 0)
		{
			STRCPY(sc_tmp,pMe->m_photo[pMe->m_i_photo-1].photoname+si_tmp+1);
			STR_TO_WSTR(sc_tmp,s_Tmp,100);
		}
		else
		{
			STR_TO_WSTR(pMe->m_photo[pMe->m_i_photo-1].photoname,s_Tmp,100);
		}
		
		ISTATIC_SetText( pMe->m_PhotoTextCtl, NULL,s_Tmp , AEE_FONT_NORMAL, AEE_FONT_NORMAL);
//		IIMAGE_SetParm(pIImage,IPARM_SCALE,pMe->iPhotoWidth,pMe->iPhotoHeight);	
		IIMAGE_SetParm(pIImage,IPARM_SCALE,pMe->m_SYSCX,pMe->m_PhotoH);
//		IImage_Draw(pMe->pi,0,25);   //  ��Ļ��ָ���������ͼ��
		i_tmp = (int )(pMe->m_SYSFBH + pMe->m_SYSFBH /2);
		IImage_Draw(pMe->pi,0,i_tmp);   //  ��Ļ��ָ���������ͼ��
		ISTATIC_Redraw(pMe->m_PhotoTextCtl);
		IDISPLAY_Update(pMe->a.m_pIDisplay); 
		// ���ڴ˴��ͷ�
		IIMAGE_Release(pMe->pi); 
		pMe->pi = NULL;
		pMe->iPressSts = 14;
		DBGPRINTF( "zjie-photo a1" );

		FREE(sc_tmp);
		sc_tmp = NULL;

		/*
		if (pMe->iSendPhoto== 1) {	// ������Ƭ
			pMe->iSendPhoto = 2;
			DBGPRINTF( "zjie-send 0" );

			// �ͷ���ѡ����Ƭ�йصĿؼ���Դ
			uf_ReleaseCtl(pMe,FALSE);
			// ������Ƭ
//  Change by zjie  2012-10-16			uf_SendPhoto(pMe);
			if ( FleUpd_HandleEvent(pMe,FLDUPD_EVT_START,0,0) )	 {
				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"shang  chuan jin lai");
				ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,FLDUPD_EVT_UPLOAD,52,0);
			}
// Change End
		}
		*/
	}
} 



// =============================================================
// �ص�������ʾͼ��
// =============================================================
void uf_ImageNotify_copy_15(newgimis * pMe,IImage *pIImage, AEEImageInfo * pi, int nErr ) 
{ 

	AECHAR s_Tmp[100];
	char *sc_tmp;
	int si_tmp;
	int i_tmp = 0;
	
	if ( (pMe->pi_copy == pIImage) && (!nErr) )
	{

		//��ʾ��Ƭ����
		sc_tmp = (char *)MALLOC(100);
		MEMSET(sc_tmp,0x00,100);
		si_tmp = ud_rDistToChar(pMe->m_photo[pMe->m_i_photo-1].photoname,'/');
		if (si_tmp >= 0)
		{
			STRCPY(sc_tmp,pMe->m_photo[pMe->m_i_photo-1].photoname+si_tmp+1);
			STR_TO_WSTR(sc_tmp,s_Tmp,100);
		}
		else
		{
			STR_TO_WSTR(pMe->m_photo[pMe->m_i_photo-1].photoname,s_Tmp,100);
		}
		
		ISTATIC_SetText( pMe->m_PhotoTextCtl, NULL,s_Tmp , AEE_FONT_NORMAL, AEE_FONT_NORMAL);
//		IIMAGE_SetParm(pIImage,IPARM_SCALE,pMe->iPhotoWidth,pMe->iPhotoHeight);	
		IIMAGE_SetParm(pIImage,IPARM_SCALE,pMe->m_SYSCX,pMe->m_PhotoH);
//		IImage_Draw(pMe->pi,0,25);   //  ��Ļ��ָ���������ͼ��
		i_tmp = (int )(pMe->m_SYSFBH + pMe->m_SYSFBH /2);
		IImage_Draw(pMe->pi_copy,0,i_tmp);   //  ��Ļ��ָ���������ͼ��
		ISTATIC_Redraw(pMe->m_PhotoTextCtl);
		IDISPLAY_Update(pMe->a.m_pIDisplay); 
		FREE(sc_tmp);
		sc_tmp = NULL;
	}
} 



// =============================================================
// �ص�������ʾͼ��  ����
// =============================================================
void uf_ImageNotify_copy(newgimis * pMe,IImage *pIImage, AEEImageInfo * pi, int nErr ) 
{ 

	AECHAR s_Tmp[100];
	char *sc_tmp;
	int si_tmp;
	int i_tmp = 0;
	AEERect sr_Rect1;
	SETAEERECT(&sr_Rect1, 0, (2 * pMe->m_SYSFBH) + pMe->m_PhotoH - pMe->m_SYSFBH * 2, pMe->DeviceInfo.cxScreen, pMe->m_MenuH * 2 - pMe->m_SYSFBH /2 - 18 );

	
	if ( (pMe->pi_copy == pIImage) && (!nErr) )
	{
/*  ��ʱע�͵�
		if (pMe->iShowText != 4 ) {	// if -- 4
			IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
			uf_ShowText(pMe);
		}	// if -- 4
*/
		//��ʾ��Ƭ����
		sc_tmp = (char *)MALLOC(100);
		MEMSET(sc_tmp,0x00,100);
		si_tmp = ud_rDistToChar(pMe->m_photo[pMe->m_i_photo-1].photoname,'/');
		if (si_tmp >= 0)
		{
			STRCPY(sc_tmp,pMe->m_photo[pMe->m_i_photo-1].photoname+si_tmp+1);
			STR_TO_WSTR(sc_tmp,s_Tmp,100);
		}
		else
		{
			STR_TO_WSTR(pMe->m_photo[pMe->m_i_photo-1].photoname,s_Tmp,100);
		}
		
		ISTATIC_SetText( pMe->m_PhotoTextCtl, NULL,s_Tmp , AEE_FONT_NORMAL, AEE_FONT_NORMAL);
//		IIMAGE_SetParm(pIImage,IPARM_SCALE,pMe->iPhotoWidth,pMe->iPhotoHeight);	
		IIMAGE_SetParm(pIImage,IPARM_SCALE,pMe->m_SYSCX,pMe->m_PhotoH);
//		IImage_Draw(pMe->pi,0,25);   //  ��Ļ��ָ���������ͼ��
		i_tmp = (int )(pMe->m_SYSFBH + pMe->m_SYSFBH /2);
		IImage_Draw(pMe->pi_copy,0,i_tmp);   //  ��Ļ��ָ���������ͼ��
		ISTATIC_Redraw(pMe->m_PhotoTextCtl);

		// pMe->m_PhotoH - (pMe->m_MenuH * 2 - pMe->m_SYSFBH /2 - 10 )

		IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &sr_Rect1);
		IDISPLAY_Update(pMe->a.m_pIDisplay); 

		/*
		// ���ڴ˴��ͷ�
		IIMAGE_Release(pMe->pi); 
		pMe->pi = NULL;
		pMe->iPressSts = 14;
		DBGPRINTF( "zjie-photo a1" );
		*/

		FREE(sc_tmp);
		sc_tmp = NULL;
	}
} 




/* ===========================================================================================
˵��
   �û��޸����뷨
�������
   ͬ gimis_HandleEvent

��������
   TRUE ������ؼ��Ѿ���������Ϣ�� FALSE ����ؼ�û�д�����Ϣ
============================================================================================== */
boolean uf_TextModechange(newgimis *pMe)
{
    AEETextInputMode   ImeNext;
	AEETextInputModeInfo CurIme;

    ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"TextModechange in");

	 if (pMe->textCtl) {
		ImeNext = ITEXTCTL_GetInputMode(pMe->textCtl,&CurIme);
	
		DBGPRINTF( "zjie-SMode=%s", CurIme.modeString);
		DBGPRINTF( "zjie-enumMode=%d", CurIme.tmMode);

		uf_ShowInputMode(pMe,CurIme.tmMode);
		ITEXTCTL_Redraw(pMe->textCtl);
	 }
	return FALSE;
}





//=====================================================================================
// ֹͣ����
// Change by zjei 2011-10-26  static void uf_StopBeep(cgimis *pMe)
void uf_StopBeep(newgimis *pMe)
{
	 ISHELL_Beep(pMe->a.m_pIShell, BEEP_OFF, TRUE);
}


void uf_RestartPhone(newgimis *pMe)
{
	char *tmp = NULL;
	STRCPY(tmp, "123");
}


/* ===========================================================================================
˵��
	��̨תǰ̨�Ķ���
�������
��������
   true : ���˺�̨תǰ̨�Ķ����� false �Ѿ���ǰ̨
============================================================================================== */
boolean uf_ForgeGroundEvt(newgimis *pMe)
{
/*
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ForgeGroundEvt,m_bGoBg=%d",pMe->m_bGoBg );
	if (pMe->m_bGoBg)  {		// �����ǰ�ں�̨,ת��ǰ̨����ʾ
		pMe->m_bOldGoBg = pMe->m_bGoBg;				
		ISHELL_StartApplet(pMe->theApp.m_pIShell, pMe->theApp.clsID);
		return TRUE;
	}
	else
		return FALSE;	
*/
	return TRUE;
}



/*===========================================================================
// ����Ļ����ʾ˫�ֽ���Ϣ
===========================================================================*/
void gimis_WPrintf( newgimis *pMe, int nLine, int nCol, AEEFont fnt, uint32 dwFlags, AECHAR *szText)
{
	/*
	AEEDeviceInfo di;
	AEERect rc;
	int nMaxLines;

	int nAEChars;
	int nCPixel;		// ÿ���ַ�����ռ������

	ISHELL_GetDeviceInfo(pMe->a.m_pIShell,&di);
	nMaxLines = (di.cyScreen / LINEHEIGHT) - 2;
	if (nMaxLines < 1)		nMaxLines = 1;

	nCPixel = di.cxScreen / MAXCOLUMNS;
	rc.x = nCol * nCPixel;
	rc.dx = di.cxScreen - nCol * nCPixel;
	rc.y = nLine * LINEHEIGHT;


   if( dwFlags & IDF_ALIGNVERT_MASK ) {
	   rc.dy = di.cyScreen - rc.y;
   }
   else {
      rc.dy = LINEHEIGHT;
   }

   nAEChars = WSTRLEN(szText);
   IDISPLAY_DrawText(pMe->theApp.m_pIDisplay, fnt, szText, nAEChars, rc.x, rc.y, &rc, dwFlags);
   IDISPLAY_Update(pMe->theApp.m_pIDisplay);
   */
}



// ==========================================================
// ��ʾ �ϴ�GPS�ɹ�
// ==========================================================
void Send_GPS_success(newgimis *me)
{
	// ��ʾ���ͽ����Ϣ  ���ڳɹ�
	// \u8003\u52e4\u6210\u529f
	AECHAR szFailure[] = {0x8003,0x52e4,0x6210,0x529f,0x0000};	// ���ͳɹ�
//	AECHAR szNoFile[] = {0x5DF2,0x65E0,0x89C6,0x9891,0x6587,0x4EF6,0x0000}; // ������Ƶ�ļ� \uFEFF\u5DF2\u65E0\u89C6\u9891\u6587\u4EF6

	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1,    // -1 = �����ַ����Ķ���ʾ
	8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(me->a.m_pIDisplay);
	return;
}



// ==========================================================
// ��ʾ �ϴ�GPSʧ��
// ==========================================================
void Send_GPS_error(newgimis *me)
{

	// ��ʾ���ͽ����Ϣ  ����ʧ���ѱ���
	// \u8003\u52e4\u5931\u8d25\uff0c\u5df2\u4fdd\u5b58
	AECHAR szFailure[] = {0x8003,0x52e4,0x5931,0x8d25,0xff0c,0x5df2,0x4fdd,0x5b58,0x0000};	// ����ʧ��
//	AECHAR szNoFile[] = {0x5DF2,0x65E0,0x89C6,0x9891,0x6587,0x4EF6,0x0000}; // ������Ƶ�ļ� \uFEFF\u5DF2\u65E0\u89C6\u9891\u6587\u4EF6

	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1,    // -1 = �����ַ����Ķ���ʾ
	8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(me->a.m_pIDisplay);

	
	
	return;
}


// ==========================================================
// ��ʾ �ϴ�GPSʧ��
// ==========================================================
void Send_GPS_error_no_save(newgimis *me)
{

	// ��ʾ���ͽ����Ϣ  ����ʧ��
	// \u8003\u52e4\u5931\u8d25\uff0c\u5df2\u4fdd\u5b58
	AECHAR szFailure[] = {0x8003,0x52e4,0x5931,0x8d25,0x0000};	// ����ʧ��
//	AECHAR szNoFile[] = {0x5DF2,0x65E0,0x89C6,0x9891,0x6587,0x4EF6,0x0000}; // ������Ƶ�ļ� \uFEFF\u5DF2\u65E0\u89C6\u9891\u6587\u4EF6

	IDISPLAY_ClearScreen(me->a.m_pIDisplay);
	IDISPLAY_DrawText(me->a.m_pIDisplay, AEE_FONT_BOLD, szFailure, -1,    // -1 = �����ַ����Ķ���ʾ
	8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(me->a.m_pIDisplay);

	
	
	return;
}
/********************************************************************************************
// ������ɺ�Ķ���
// �������
  FileUpload * pMe	: 
********************************************************************************************/
void GPS_Afterpost(newgimis *pMe)
{

	pMe->m_WitchPag = 10;
	testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );  //���ص�������

	return;
}








////////// ��������Ƶ�й� ////////


// ������Ƶ����
void newgimis_forAudio(newgimis * pMe,boolean bGet)
{

	if ((pMe->m_Flag_Key == 1) && (pMe->m_Witch_Key == 8) )//˵���ǽ��뷢����Ƶ����
	{
		pMe->m_Flag_Key = 0;
		pMe->m_Witch_Key = -1;
	}

	

//	 pMe->cUsePhoto = '7'; // �ж���û�и�Ȩ�� 
	if ( pMe->cUsePhoto == '4' || pMe->cUsePhoto == '5' || pMe->cUsePhoto == '7')
	{
		// ��������
	}
	else
	{
		//û�п����ù���
		pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
		uf_AEPrint_Audio_Back(pMe,FALSE);		// һ����Ƶ��û��
		return;
	}
	newgimis_GetAudio(pMe);

	if ( STRLEN(pMe->m_Audio[0].photoname) == 0 ) 
	{
		// ˵��û����Ƶ
		pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
		uf_AEPrint_Back(pMe,FALSE);		// һ�Ŷ�û��û��
		return;
	}




//	if (uf_CheckNetSts(pMe)) {	// if -- 1
		if (pMe->m_F_QF) //��ʾ�ֻ�Ƿ��
		{
			pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
			uf_AEPrint_NO_NET_Back(pMe,FALSE);
			return;
		}
		if (bGet) {
			pMe->m_WitchPag = 3; // ������Ƶ����

			testgps_GotoScreen( pMe, SCREENID_SENDAUDIO, 0 );
	
	
		 
		}

	
	
		return;
	

//	}	// if -- 1
//	else  // û������
//	{

//		uf_AEPrint_NO_NET_Back(pMe,FALSE);
//	}	// if -- 2
}




// ==============================================
// ���ַ�����ԭ�ַ�ת��Ϊת���ַ�
// ==============================================
void uf_toESC(char *rStr_source,char *rStr_Esc)
{
    char *ptStr = NULL;
	uf_trfESC(rStr_source,&ptStr,ESC_SP,ESC_EQ);
	STRCPY(rStr_Esc,ptStr);
	uf_trfESC(rStr_Esc,&ptStr,ESC_ZA_,ESC_SUB);
	STRCPY(rStr_Esc,ptStr);
	// Add by zjie 2013-03-15
	uf_trfESC(rStr_Esc,&ptStr,ESC_ASTERISK,ESC_for_asterisk);
	STRCPY(rStr_Esc,ptStr);
	// Add End
	FREE(ptStr);
	
}

// ==============================================
// ���ַ�����ת���ַ�ת��Ϊԭ�ַ�
// ==============================================
void uf_fromESC(char *rStr_Esc,char *rStr_source)
{
    char *ptStr = NULL;
	uf_trfESC(rStr_Esc,&ptStr,ESC_EQ,ESC_SP);
	STRCPY(rStr_source,ptStr);
	uf_trfESC(rStr_source,&ptStr,ESC_SUB,ESC_ZA_);
	STRCPY(rStr_source,ptStr);
	
	// Add by zjie 2013-03-15
	uf_trfESC(rStr_source,&ptStr,ESC_for_asterisk,ESC_ASTERISK);
	STRCPY(rStr_source,ptStr);
	// Add End
	
	FREE(ptStr);
	
}



// ==============================================
// ���ַ������滻���ַ���
// �������
//    rStr_source : �����滻�����ַ������ַ���
//    rStr_Esc    : �����滻�����ַ������ַ���
//    vStr_sESC   : ���滻�����ַ���
//    vStr_tESC   : ���滻�����ַ���
// ==============================================
void uf_trfESC(char *rStr_source,char **rStr_Esc,const char *vStr_sESC, const char *vStr_tESC)
{
    char *psloc = NULL;			// ԭ�ַ�����λ��
	char *psTmp = NULL;
	char *psTarget = NULL;
	int lInt_sloc;
	int iLen = STRLEN(rStr_source);

	if (*rStr_Esc != NULL)
		FREE(*rStr_Esc);

	psTmp = MALLOC(iLen+1);
	STRCPY(psTmp,rStr_source);

    while(1)
	{
		psloc = STRSTR(psTmp,vStr_sESC);
		if (psloc != NULL) {
			iLen += STRLEN(vStr_tESC) - STRLEN(vStr_sESC);
			lInt_sloc = psloc-psTmp;
			psTarget = MALLOC(iLen+1);
			MEMSET(psTarget,0,iLen+1);
			MEMCPY(psTarget,psTmp,lInt_sloc);
			MEMCPY(psTarget+lInt_sloc,vStr_tESC,STRLEN(vStr_tESC));
			STRCPY(psTarget+lInt_sloc+STRLEN(vStr_tESC),psloc+STRLEN(vStr_sESC));
			FREE(psTmp);
			psTmp = psTarget;
		}
		else {
			psTarget = psTmp;
			break;
		}
	}

	*rStr_Esc = psTarget;
	return;
}

int Show_LongSMS_Notify(newgimis *pnewgimis)
{
	IMenuCtl *pMenuCtl = testgps_GetScreenData_LongSMS_Notity( pnewgimis );
    IMenuCtl *pMenuCtl_N = testgps_GetScreenData( pnewgimis );
	
	if (pMenuCtl != NULL && pMenuCtl_N != NULL)
	{
		IMENUCTL_SetActive( pMenuCtl, TRUE );    
		IMENUCTL_SetActive( pMenuCtl_N, FALSE );  
		IMENUCTL_Redraw( pMenuCtl_N );
        IMENUCTL_Redraw( pMenuCtl );
		
		pMenuCtl = NULL;
		pMenuCtl_N = NULL;
		return 1;
	}
	
	pMenuCtl = NULL;
	pMenuCtl_N = NULL;	  
	return 0;
}

//��ʾ��Ϣ����
int Show_LongSMS_Content(newgimis *pnewgimis)
{
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pnewgimis );
	
	if( (pMenuCtl != NULL) && (pnewgimis->m_pIStatic != NULL) ){
				
		ISTATIC_SetActive(pnewgimis->m_pIStatic,TRUE);
		IMENUCTL_SetActive( pMenuCtl, TRUE );

		ISTATIC_Redraw( pnewgimis->m_pIStatic );
		IMENUCTL_Redraw( pMenuCtl);
	}
	pMenuCtl = NULL;
	return 0;
}

// ��ʾ�ռ������
int Show_LongSMS(newgimis *pnewgimis)
{	
	IMenuCtl *pMenuCtl = testgps_GetScreenData_LongSMS( pnewgimis );
    IMenuCtl *pMenuCtl_N = testgps_GetScreenData( pnewgimis );
	
	if (pMenuCtl != NULL && pMenuCtl_N != NULL)
	{
		IMENUCTL_SetActive( pMenuCtl, TRUE );       
		IMENUCTL_SetActive( pMenuCtl_N, TRUE );       
		IMENUCTL_Redraw( pMenuCtl_N );
        IMENUCTL_Redraw( pMenuCtl );
		
		pMenuCtl = NULL;
		pMenuCtl_N = NULL;
		return 1;
	}
	
	pMenuCtl = NULL;
	pMenuCtl_N = NULL;	  
	return 0;
}


//��ʾ�������ݽ���

int Show_Task_Content(newgimis *pnewgimis)
{
	cTaskApp *me = NULL;
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pnewgimis );
	me = pnewgimis->pTaskApp;

	if (me == NULL){
		return 0;
	}
	if( (pMenuCtl != NULL) && (me->m_pIStatic != NULL) ){
		if ( ISTATIC_IsActive(me->m_pIStatic) )	{

			ISTATIC_SetActive(me->m_pIStatic,TRUE);
			IMENUCTL_SetActive( pMenuCtl, FALSE );
		}else{
			ISTATIC_SetActive(me->m_pIStatic,FALSE);
			IMENUCTL_SetActive( pMenuCtl, TRUE );
		}
		ISTATIC_Redraw( me->m_pIStatic );
		IMENUCTL_Redraw( pMenuCtl);
	}
	pMenuCtl = NULL;
	return 0;
}


////////////////////////////////////
//��ʾ�������������
int Show_Task_Detail2(newgimis *pnewgimis)
{
	cTaskApp *me = NULL;
	
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pnewgimis );
	me = pnewgimis->pTaskApp;

	if (me == NULL)
	{
		return 0;
	}
	
	if( (pMenuCtl != NULL) && (pnewgimis->textCtl_T != NULL) && (me->pMenuList != NULL) && (me->pMenuList_T) != NULL )
	{

		{
			//���������ɷ�ʱ��
			JulianType dDate;
			AECHAR  asText[50] = {0};
			char    szBuf[25] = {0};
			GETJULIANDATE(pnewgimis->m_Cur_nDateTime, &dDate);
			SPRINTF(szBuf,"time: %04d-%02d-%02d %02d:%02d%",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute);
			STRTOWSTR(szBuf, asText, sizeof(asText));
			Task_Print_unicode(pnewgimis,0,1,AEE_FONT_BOLD,IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL,asText);
		}

		
		Task_Print_asc( pnewgimis, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
		//Task_Print_asc( pnewgimis, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
		Task_Print_asc( pnewgimis, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);

		{// �б�ؼ���ǩ+�б�ؼ����� ��ʾ	�ı�����
			AECHAR* psTextBuf = NULL;
			uint32  bufSize = 0;
			uint32  nRtn;
			AEERect rRectList;
			//��ǩ��"�����ʾ����"
			ISHELL_GetResSize(pnewgimis->a.m_pIShell,NEWGIMIS_RES_FILE, IDS_TASK_CONTENT,RESTYPE_STRING,&bufSize);
			psTextBuf = (AECHAR*)MALLOC((bufSize+1)*sizeof(AECHAR));
			nRtn = ISHELL_LoadResString(pnewgimis->a.m_pIShell, NEWGIMIS_RES_FILE,IDS_TASK_CONTENT, psTextBuf,bufSize);
			
			if (nRtn >0 )
				rRectList = Task_Print_unicode( pnewgimis, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,psTextBuf);
			else
				ud_RecLog(pnewgimis->a.m_pIShell,pnewgimis->cWrtLog,&(pnewgimis->nInd),"cTaskApp_DisplayCtlList in LoadResString nRtn=%d",nRtn);
			FREE(psTextBuf);
			
			//�˵�list�ؼ���ʾ
			rRectList.x += rRectList.dx + 3;
			rRectList.dx = pnewgimis->DeviceInfo.cxScreen - rRectList.x - 5;
			
			rRectList.dy = pnewgimis->m_SYSFBH +MENULIST_HEIGHT; //update: 2016-2-2
			IMENUCTL_SetRect( me->pMenuList_T, &rRectList );
			//IMENUCTL_Redraw( me->pMenuList );
		}
		
		
		{// �б�ؼ���ǩ+�б�ؼ����� ��ʾ
			AECHAR* psTextBuf = NULL;
			uint32  bufSize = 0;
			uint32  nRtn;
			AEERect rRectList;
			//��ǩ��"������"
			ISHELL_GetResSize(pnewgimis->a.m_pIShell,NEWGIMIS_RES_FILE, IDS_TASK_02,RESTYPE_STRING,&bufSize);
			psTextBuf = (AECHAR*)MALLOC((bufSize+1)*sizeof(AECHAR));
			nRtn = ISHELL_LoadResString(pnewgimis->a.m_pIShell, NEWGIMIS_RES_FILE,IDS_TASK_02, psTextBuf,bufSize);
			
			if (nRtn >0 )
				rRectList = Task_Print_unicode( pnewgimis, 4, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,psTextBuf);
			else
				ud_RecLog(pnewgimis->a.m_pIShell,pnewgimis->cWrtLog,&(pnewgimis->nInd)," Show_Task_Detail2 in LoadResString nRtn=%d",nRtn);
			FREE(psTextBuf);
			
			//�˵�list�ؼ���ʾ
			rRectList.x += rRectList.dx + 3;
			rRectList.dx = pnewgimis->DeviceInfo.cxScreen - rRectList.x - 5;
			
			rRectList.dy = pnewgimis->m_SYSFBH +MENULIST_HEIGHT; //update: 2016-2-2
			IMENUCTL_SetRect( me->pMenuList, &rRectList );
		}
		
		
			{	//��ע����
				int nRtn;
				int nLine;
				int nExpLen;
				AECHAR szText[10];

				AECHAR temp[10] = {0x2e, 0x2e, 0x2e,0x0000};
				char   exp[51];
				ZEROAT(exp);

				//nLine = (me->pTaskRecB->bFlow) ? 4 : 5;
				nRtn = ISHELL_LoadResString(pnewgimis->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_TASK_BZ, szText,sizeof(szText));
				if (nRtn > 0)
				{
					Task_Print_unicode( pnewgimis, 5, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,szText);
				}
			}
			
			{
				AEETextInputModeInfo CurIme;											
				ITEXTCTL_GetInputMode(pnewgimis->textCtl_T,&CurIme);									
				yb_ShowInputMode(pnewgimis,CurIme.tmMode,2);
			}
			
			if (IMENUCTL_IsActive( pMenuCtl ))
			{
				IMENUCTL_SetActive(pMenuCtl,TRUE); 
				ITEXTCTL_SetActive(pnewgimis->textCtl_T,FALSE);
				IMENUCTL_SetActive(me->pMenuList,FALSE); 
				IMENUCTL_SetActive(me->pMenuList_T,FALSE);
			}
			else if (IMENUCTL_IsActive( me->pMenuList ))
			{
				IMENUCTL_SetActive(pMenuCtl,FALSE); 
				ITEXTCTL_SetActive(pnewgimis->textCtl_T,FALSE);
				IMENUCTL_SetActive(me->pMenuList,TRUE); 
				IMENUCTL_SetActive(me->pMenuList_T,FALSE);
			}
			else if (ITEXTCTL_IsActive(pnewgimis->textCtl_T) )
			{
				IMENUCTL_SetActive(pMenuCtl,FALSE); 
				ITEXTCTL_SetActive(pnewgimis->textCtl_T,TRUE);
				IMENUCTL_SetActive(me->pMenuList,FALSE);
				IMENUCTL_SetActive(me->pMenuList_T,FALSE);
				ITEXTCTL_SetCursorPos(pnewgimis->textCtl_T, TC_CURSOREND);
		}
		else
		{
			IMENUCTL_SetActive(pMenuCtl,FALSE); 
			ITEXTCTL_SetActive(pnewgimis->textCtl_T,FALSE);
			IMENUCTL_SetActive(me->pMenuList,FALSE);
			IMENUCTL_SetActive(me->pMenuList_T,TRUE);
		}

		IMENUCTL_Redraw(pMenuCtl);
		IMENUCTL_Redraw(me->pMenuList);
		IMENUCTL_Redraw(me->pMenuList_T);
		ITEXTCTL_Redraw(pnewgimis->textCtl_T);
	}
	
	pMenuCtl = NULL;
	return 0;
}


//��ʾ������������
int Show_Task_Detail1(newgimis *pnewgimis)
{
	cTaskApp *me = NULL;

	IMenuCtl *pMenuCtl = testgps_GetScreenData( pnewgimis );
	me = pnewgimis->pTaskApp;
	
	if (me == NULL)
	{
		return 0;
	}

	if( (pMenuCtl != NULL) && (pnewgimis->textCtl_T != NULL) && (me->pMenuList_T != NULL) )
	{
		//IDISPLAY_ClearScreen( pnewgimis->a.m_pIDisplay );
		//IDISPLAY_Update (pnewgimis->a.m_pIDisplay);
		{
			//���������ɷ�ʱ��
			JulianType dDate;
			AECHAR  asText[50] = {0};
			char    szBuf[25] = {0};
			GETJULIANDATE(pnewgimis->m_Cur_nDateTime, &dDate);
			SPRINTF(szBuf,"time: %04d-%02d-%02d %02d:%02d%",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute);
			STRTOWSTR(szBuf, asText, sizeof(asText));
			Task_Print_unicode(pnewgimis,0,1,AEE_FONT_BOLD,IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL,asText);
		}


		Task_Print_asc( pnewgimis, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
		//Task_Print_asc( pnewgimis, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
		Task_Print_asc( pnewgimis, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_LEFT|IDF_ALIGN_TOP|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);

		{// �б�ؼ���ǩ+�б�ؼ����� ��ʾ	�ı�����
			AECHAR* psTextBuf = NULL;
			uint32  bufSize = 0;
			uint32  nRtn;
			AEERect rRectList;
			//��ǩ��"�����ʾ����"
			ISHELL_GetResSize(pnewgimis->a.m_pIShell,NEWGIMIS_RES_FILE, IDS_TASK_CONTENT,RESTYPE_STRING,&bufSize);
			psTextBuf = (AECHAR*)MALLOC((bufSize+1)*sizeof(AECHAR));
			nRtn = ISHELL_LoadResString(pnewgimis->a.m_pIShell, NEWGIMIS_RES_FILE,IDS_TASK_CONTENT, psTextBuf,bufSize);
			
			if (nRtn >0 )
				rRectList = Task_Print_unicode( pnewgimis, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,psTextBuf);
			else
				ud_RecLog(pnewgimis->a.m_pIShell,pnewgimis->cWrtLog,&(pnewgimis->nInd),"cTaskApp_DisplayCtlList in LoadResString nRtn=%d",nRtn);
			FREE(psTextBuf);
			
			//�˵�list�ؼ���ʾ
			rRectList.x += rRectList.dx + 3;
			rRectList.dx = pnewgimis->DeviceInfo.cxScreen - rRectList.x - 5;
			
			rRectList.dy = pnewgimis->m_SYSFBH +MENULIST_HEIGHT; //update: 2016-2-2
			IMENUCTL_SetRect( me->pMenuList_T, &rRectList );
			//IMENUCTL_Redraw( me->pMenuList );
		}

		{	//��ע����
			int nRtn;
			int nLine;
			int nExpLen;
			AECHAR szText[10];
			
			AECHAR temp[10] = {0x2e, 0x2e, 0x2e,0x0000};
			char   exp[51];
			ZEROAT(exp);
			
			//nLine = (me->pTaskRecB->bFlow) ? 4 : 5;
			nRtn = ISHELL_LoadResString(pnewgimis->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_TASK_BZ, szText,sizeof(szText));
			if (nRtn > 0)
			{
				Task_Print_unicode( pnewgimis, 4, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL,szText);
			}
		}
		
		{
			AEETextInputModeInfo CurIme;											
			ITEXTCTL_GetInputMode( pnewgimis->textCtl_T,&CurIme);									
			yb_ShowInputMode(pnewgimis,CurIme.tmMode,2);
		}

		if (IMENUCTL_IsActive( pMenuCtl ))
		{
			IMENUCTL_SetActive(pMenuCtl,TRUE);
			IMENUCTL_SetActive(me->pMenuList_T,FALSE);
			ITEXTCTL_SetActive(pnewgimis->textCtl_T,FALSE);
		}
		else if (ITEXTCTL_IsActive(pnewgimis->textCtl_T))
		{
			IMENUCTL_SetActive(pMenuCtl,FALSE); 
			IMENUCTL_SetActive(me->pMenuList_T,FALSE);
			ITEXTCTL_SetActive(pnewgimis->textCtl_T,TRUE);
			ITEXTCTL_SetCursorPos(pnewgimis->textCtl_T, TC_CURSOREND);
		}
		else
		{
			IMENUCTL_SetActive(pMenuCtl,FALSE); 
			IMENUCTL_SetActive(me->pMenuList_T,TRUE);
			ITEXTCTL_SetActive(pnewgimis->textCtl_T,FALSE);
		}
		IMENUCTL_Redraw(pMenuCtl);
		IMENUCTL_Redraw(me->pMenuList_T);
		ITEXTCTL_Redraw(pnewgimis->textCtl_T);
	}

	pMenuCtl = NULL;
	return 0;
}


//��ʾ�����б����
int Show_Task_List(newgimis *pnewgimis)
{
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pnewgimis );
    IMenuCtl *pMenuCtl_N = testgps_GetScreenData_task( pnewgimis );
	
	if (pMenuCtl != NULL && pMenuCtl_N != NULL)
	{
		if (IMENUCTL_IsActive( pMenuCtl ))
		{
			IMENUCTL_SetActive( pMenuCtl, TRUE ); 
			IMENUCTL_SetActive( pMenuCtl_N, FALSE ); 
		}
		else
		{
			IMENUCTL_SetActive( pMenuCtl, FALSE ); 
			IMENUCTL_SetActive( pMenuCtl_N, TRUE ); 
		}
     
		IMENUCTL_Redraw( pMenuCtl_N );
        IMENUCTL_Redraw( pMenuCtl );
		
		pMenuCtl = NULL;
		pMenuCtl_N = NULL;
		return 1;
	}
	
	pMenuCtl = NULL;
	pMenuCtl_N = NULL;	  
	return 0;
}

//��ʾ��̨�������ѽ���
int show_Notify(newgimis *pnewgimis)
{
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pnewgimis );
	if(pMenuCtl)
	{
		IMENUCTL_SetActive(pMenuCtl,TRUE);
		IMENUCTL_Redraw( pMenuCtl );
		pMenuCtl = NULL;
		return 1;
	}
	pMenuCtl = NULL;
	return 0;
}


// ��ʾ������Ǹ�����
int Show_MorePag_Fun(newgimis *pnewgimis)
{
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pnewgimis );
	if(pMenuCtl)
	{
		IMENUCTL_SetActive(pMenuCtl,TRUE);
		IMENUCTL_Redraw( pMenuCtl );
		pMenuCtl = NULL;
		return 1;
	}
	pMenuCtl = NULL;
	return 0;
}




// �������ڻ���ʱ���õĽ���   ���µķ���ֵ�� 1 -- ����ɹ�����  0 -- ��û���ҵ��ؼ�ָ�� û�л���
// ��ʾ������
int Show_Main_Fun(newgimis *pnewgimis)
{
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pnewgimis );
	if(pMenuCtl)
	{
		IMENUCTL_SetActive(pMenuCtl,TRUE);
		IMENUCTL_Redraw( pMenuCtl );
		pMenuCtl = NULL;
		return 1;
	}
	pMenuCtl = NULL;
	return 0;


}





// ��ʾ��Ƭ����
int Show_Photo_Fun(newgimis *pnewgimis)
{
	IMenuCtl *pMenuCtl = testgps_GetScreenData( pnewgimis );


	if(pMenuCtl)
	{
		pnewgimis->pi = ISHELL_LoadImage(pnewgimis->a.m_pIShell, pnewgimis->sFileName);
		if (pnewgimis->pi) 		
		{
			IImage_Notify(pnewgimis->pi, (PFNIMAGEINFO)uf_ImageNotify,pnewgimis);  //ע��ص�
		}


		 if (pnewgimis->cUsePhoto == '1' || pnewgimis->cUsePhoto == '5')
		 {
			 if (pnewgimis->textCtl)
			 {
				ITEXTCTL_Redraw(pnewgimis->textCtl);
			 }		 
		 }
		IMENUCTL_Redraw( pMenuCtl );
		pMenuCtl = NULL;
		return 1;
	}
	pMenuCtl = NULL;
	return 0;
}


// ��ʾ��Ƶ����
int Show_Audio_Fun(newgimis *pnewgimis)
{	
	IMenuCtl *pMenuCtl = testgps_GetScreenData_Audio( pnewgimis );
    IMenuCtl *pMenuCtl_N = testgps_GetScreenData( pnewgimis );

	if (pMenuCtl != NULL && pMenuCtl_N != NULL)
	{
		

		IMENUCTL_SetActive( pMenuCtl_N, TRUE );       
		IMENUCTL_Redraw( pMenuCtl_N );
        IMENUCTL_Redraw( pMenuCtl );

		pMenuCtl = NULL;
		pMenuCtl_N = NULL;
		return 1;
	}

	pMenuCtl = NULL;
	pMenuCtl_N = NULL;	  
	return 0;
}

int Show_Audio_Fun_Sup(newgimis *pnewgimis)
{
	IMenuCtl *pMenuCtl_N = testgps_GetScreenData( pnewgimis );
	IMENUCTL_SetActive( pMenuCtl_N, FALSE );
	pMenuCtl_N = NULL;
	return 0;
}


/*===========================================================================
˵��
  �˳�����,(ͨ����ϼ�)
�������
    cgimis *pMe : ȫ�ֽṹ
===========================================================================*/
boolean newgimis_ClosseApplet( newgimis *pMe)
{

	AECHAR szText[] = {0x7a0b,0x5e8f,0x6b63,0x5728,0x9000,0x51fa,0x0000}; // L"���������˳�";
	

//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�ɼ���ť���� 3 %d",pMe->cCanExit);
   if (pMe->cCanExit != '1')	// Add by zjie 2013-01-01
		return FALSE;
	
   if ( ( (pMe->clsKey[0] == AVK_1) && (pMe->clsKey[1] == AVK_POUND) ) 
	    || ( (pMe->clsKey[1] == AVK_1) && (pMe->clsKey[0] == AVK_POUND) )
	  )
   {
	   IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
		IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
							3, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
		IDISPLAY_Update(pMe->a.m_pIDisplay);

		pMe->m_PowerFlag = '0';
		gimis_SetPram_Open( pMe);
		
		ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)newgimis_ufStopApp, pMe);
	    ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"trigger user 1# exit 2");
	   
	   return TRUE;
   }
   else
	   return FALSE;
}

void newgimis_ufStopApp( newgimis *pMe )
{
	int i=0;

	IMenuCtl *pMenuctl = testgps_GetScreenData( pMe );
	// \u7a0b\u5e8f\u6b63\u5728\u9000\u51fa
	
	if (pMe->cPosDetMode == 13)
	{
		newgimis_CloseGPS_UKBD(pMe);
	}
	else if (pMe->cPosDetMode == 14)
	{
		newgimis_CloseGPS_S958GPS(pMe);
	}
	else
	{
		newgimis_CloseGPS( pMe );
	}

// 	if (pMe->pIgpsext)
// 	{
// 		i=ITTSEXT_Release(pMe->pIgpsext);
// 		while (i>0) {
// 			i=ITTSEXT_Release(pMe->pIgpsext);	
// 		}
// 	}

	if (pMenuctl)
	{
		//uf_RecLog(pMe,"recevice message 2016-3-7 pMenuctl release");
		IMENUCTL_Release(pMenuctl);
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�ɼ���ť���� 4");
	}
	
	pMe->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�ɼ���ť���� 5");
	if (!pMe->m_FOBFlag)
	{
		ISHELL_CloseApplet(pMe->a.m_pIShell,TRUE); //ǰ̨��close
	}
	else
	{
		ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
	}
	
	//uf_RecLog(pMe,"recevice message 2016-3-7 ISHELL_CloseApplet");
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�ɼ���ť���� 6");
	return ;
}




void Save_GPSData_All(newgimis *me)
{
	int i_Ret = 0;
	uf_RecLog(me,"judge offline data 1");

	// ������ʵ�ֱ�����������
	i_Ret = gimis_judgedata(me);
	switch(i_Ret)
	{
	case 0:  //  ˵����û�������ļ�����
//		ud_RecLog(me->a.m_pIShell,me->cWrtLog,&(me->nInd),"����GPS����");
//		uf_RecLog(me,"�ж���û���������� 2");
		Save_GPSData(me);
		break;
	default: //˵�����ļ����� 
		Save_GPSData_had( me );
		break;
	}
	me->m_OffData = 1;
	
//	uf_RecLog(me,"�ж���û���������� 3");
	return ;
}





/********************************************************************************************
 �ڱ��ؼ�¼��־
�������
  cgimis * pMe
  IShell *pShell  :  
  cWrtLog         :   1  д����־�� ��1 ����qxdm����
  uint16 *pIndex  :   ����������ʾ��־�г���ʼִ�е�λ�ã��ɵ��õĳ��򱣴��ֵ
  const char *szFormat, ...  �� ���¼���ַ���
********************************************************************************************/
void Save_GPSData(newgimis *pMe)
{

	int lInt_Len;	
	IFileMgr		*pFileMgr;
	IFile			*pTempFile;
    FileInfo fiInfo;		// �ļ���Ϣ
	
	char c_tmp[200];

	/*
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	MEMSET(pMe->lx_vv,0x00,MAXTEXTLEN);
	MEMSET(pMe->lx_tt,0x00,15);				// TT ʱ��

	SPRINTF(pMe->lx_vv,"0");
	GETJULIANDATE(0,&dDate);		//	ȡ������
	SPRINTF(pMe->lx_tt,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	//                      LO LA VV EP KS PM TT
	SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%s;EP%c;KS2;PM%c;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->lx_vv,pMe->lx_ep,pMe->lx_pm ,pMe->lx_tt);
	*/


	ZEROAT(c_tmp);
	STRCPY(c_tmp,pMe->sUrl_yjkq);


//	STRCAT(c_tmp,":");

	// �����ļ��ӿ�
	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
	{
		if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_1) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_1, _OFM_READWRITE);
//			uf_RecLog(pMe,"�ж���û���������� 5");
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_1, _OFM_CREATE);
//			uf_RecLog(pMe,"�ж���û���������� 4");
		}
	}
	else
	{
//		uf_RecLog(pMe,"���������ļ�ʧ��");
		uf_RecLog(pMe,"create offline file fail");
		return;
	}

	if (pTempFile != NULL)
	{
		if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) 
		{
			IFILE_Seek(pTempFile,_SEEK_END,0);
		

//			uf_RecLog(pMe,"�ж���û���������� 6");
	   
		   lInt_Len = STRLEN(c_tmp);
		   IFILE_Write(pTempFile, c_tmp, lInt_Len);
		}
	}

//	uf_RecLog(pMe,"�ж���û���������� 7");

   // Free the IFileMgr and IFile instances
	if (pTempFile != NULL)
	   IFILE_Release( pTempFile );

	if (pFileMgr != NULL)
	   IFILEMGR_Release( pFileMgr );
}



void Save_GPSData_had(newgimis *pMe)  //���Ѿ����������ݵ�����¼���������������
{
	int lInt_Len;	
	IFileMgr		*pFileMgr;
	IFile			*pTempFile;
    FileInfo fiInfo;		// �ļ���Ϣ
	
	char c_tmp[200];

	
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	MEMSET(pMe->lx_vv,0x00,MAXTEXTLEN);
	MEMSET(pMe->lx_tt,0x00,15);				// TT ʱ��

	SPRINTF(pMe->lx_vv,"0");
	GETJULIANDATE(0,&dDate);		//	ȡ������
	SPRINTF(pMe->lx_tt,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	//                      LO LA VV EP KS PM TT

	MEMSET(pMe->sUrl_yjkq,0x00,200);
	if (pMe->cPosDetMode != 13)
	{
		switch (pMe->m_gpsFlag)
		{
		case 0:
			pMe->m_PM = '2'; 
			break;
		case 1:
			pMe->m_PM = '5'; 
			break;
		case 2:
			pMe->m_PM = '1'; 
			break;
		case 3:
			pMe->m_PM = '0'; 
			break;
		case 4:
			pMe->m_PM = '9'; 
			break;
		case 5:
			pMe->m_PM = '6'; 
			break;
		}
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS2;PM%c;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor,pMe->lx_ep,pMe->m_PM ,pMe->lx_tt);
	}
	else
	{
		SPRINTF(pMe->sUrl_yjkq,"LO%s;LA%s;VV%d;EP%c;KS2;PM%d;TT%s",pMe->lx_lon,pMe->lx_lat,pMe->dVelHor_BD,pMe->lx_ep,pMe->m_PM_BD ,pMe->lx_tt);
	}

	

	ZEROAT(c_tmp);
	STRCPY(c_tmp,pMe->sUrl_yjkq);


//	STRCAT(c_tmp,"\r\n");

	// �����ļ��ӿ�
	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
	{
		if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_1) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_1, _OFM_READWRITE);
//			uf_RecLog(pMe,"�ж���û���������� 5");
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_1, _OFM_CREATE);
//			uf_RecLog(pMe,"�ж���û���������� 4");
		}

		if (pTempFile != NULL)
		{
			if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) 
			{
				if (fiInfo.dwSize < FILE_OFFDATA - ITEM_OFFDATA)  // ˵�������Լ�����������
				{
					IFILE_Seek(pTempFile,_SEEK_END,0);
//					uf_RecLog(pMe,"�ж���û���������� 6");
					lInt_Len = STRLEN(c_tmp);
					IFILE_Write(pTempFile, c_tmp, lInt_Len);

					if (pTempFile != NULL)
					   IFILE_Release( pTempFile );

					if (pFileMgr != NULL)
					   IFILEMGR_Release( pFileMgr );

					pFileMgr = NULL;
					pTempFile = NULL;
					return ;
				}
				else
				{
					IFILE_Release( pTempFile );
					pTempFile = NULL;
				}
			}
		}
		else
		{
			uf_RecLog(pMe,"crate offline file 11");
			if (pFileMgr != NULL)
				  IFILEMGR_Release( pFileMgr );
			return;
		}


		/////////  �ڶ����ļ�

		if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_2) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_2, _OFM_READWRITE);
//			uf_RecLog(pMe,"�ж���û���������� 52");
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_2, _OFM_CREATE);
//			uf_RecLog(pMe,"�ж���û���������� 42");
		}

		if (pTempFile != NULL)
		{
			if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) 
			{
				if (fiInfo.dwSize < FILE_OFFDATA - ITEM_OFFDATA)  // ˵�������Լ�����������
				{
					IFILE_Seek(pTempFile,_SEEK_END,0);
//					uf_RecLog(pMe,"�ж���û���������� 6");
					lInt_Len = STRLEN(c_tmp);
					IFILE_Write(pTempFile, c_tmp, lInt_Len);

					if (pTempFile != NULL)
					   IFILE_Release( pTempFile );

					if (pFileMgr != NULL)
					   IFILEMGR_Release( pFileMgr );

					pFileMgr = NULL;
					pTempFile = NULL;
					return ;
				}
				else
				{
					IFILE_Release( pTempFile );
					pTempFile = NULL;
				}
			}
		}
		else
		{
			uf_RecLog(pMe,"create offline file fail 12");
			if (pFileMgr != NULL)
				  IFILEMGR_Release( pFileMgr );
			return;
		}


		/////////  �������ļ�

		if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_3) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_3, _OFM_READWRITE);
//			uf_RecLog(pMe,"�ж���û���������� 53");
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_3, _OFM_CREATE);
//			uf_RecLog(pMe,"�ж���û���������� 43");
		}

		if (pTempFile != NULL)
		{
			if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) 
			{
				if (fiInfo.dwSize < FILE_OFFDATA - ITEM_OFFDATA)  // ˵�������Լ�����������
				{
					IFILE_Seek(pTempFile,_SEEK_END,0);
//					uf_RecLog(pMe,"�ж���û���������� 63");
					lInt_Len = STRLEN(c_tmp);
					IFILE_Write(pTempFile, c_tmp, lInt_Len);

					if (pTempFile != NULL)
					   IFILE_Release( pTempFile );

					if (pFileMgr != NULL)
					   IFILEMGR_Release( pFileMgr );

					pFileMgr = NULL;
					pTempFile = NULL;
					return ;
				}
				else
				{
					IFILE_Release( pTempFile );
					pTempFile = NULL;
				}
			}
		}
		else
		{
			uf_RecLog(pMe,"create offline file fail 13");
			if (pFileMgr != NULL)
				  IFILEMGR_Release( pFileMgr );
			return;
		}



		/////////  ���ĸ��ļ�

		if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_4) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_4, _OFM_READWRITE);
//			uf_RecLog(pMe,"�ж���û���������� 54");
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_4, _OFM_CREATE);
//			uf_RecLog(pMe,"�ж���û���������� 44");
		}

		if (pTempFile != NULL)
		{
			if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) 
			{
				if (fiInfo.dwSize < FILE_OFFDATA - ITEM_OFFDATA)  // ˵�������Լ�����������
				{
					IFILE_Seek(pTempFile,_SEEK_END,0);
//					uf_RecLog(pMe,"�ж���û���������� 64");
					lInt_Len = STRLEN(c_tmp);
					IFILE_Write(pTempFile, c_tmp, lInt_Len);

					if (pTempFile != NULL)
					   IFILE_Release( pTempFile );

					if (pFileMgr != NULL)
					   IFILEMGR_Release( pFileMgr );

					pFileMgr = NULL;
					pTempFile = NULL;
					return ;
				}
				else
				{
					IFILE_Release( pTempFile );
					pTempFile = NULL;
				}
			}
		}
		else
		{
			uf_RecLog(pMe,"create offline file fail 14");
			if (pFileMgr != NULL)
				  IFILEMGR_Release( pFileMgr );
			return;
		}




		/////////  ������ļ�

		if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_5) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_5, _OFM_READWRITE);
//			uf_RecLog(pMe,"�ж���û���������� 55");
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_5, _OFM_CREATE);
//			uf_RecLog(pMe,"�ж���û���������� 45");
		}

		if (pTempFile != NULL)
		{
			if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) 
			{
				if (fiInfo.dwSize < FILE_OFFDATA - ITEM_OFFDATA)  // ˵�������Լ�����������
				{
					IFILE_Seek(pTempFile,_SEEK_END,0);
//					uf_RecLog(pMe,"�ж���û���������� 65");
					lInt_Len = STRLEN(c_tmp);
					IFILE_Write(pTempFile, c_tmp, lInt_Len);

					if (pTempFile != NULL)
					   IFILE_Release( pTempFile );

					if (pFileMgr != NULL)
					   IFILEMGR_Release( pFileMgr );

					pFileMgr = NULL;
					pTempFile = NULL;
					return ;
				}
				else
				{
					IFILE_Release( pTempFile );
					pTempFile = NULL;
				}
			}
		}
		else
		{
			uf_RecLog(pMe,"create offline file fail 15");
			if (pFileMgr != NULL)
				  IFILEMGR_Release( pFileMgr );
			return;
		}



		/////////  �������ļ�

		if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_6) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_6, _OFM_READWRITE);
//			uf_RecLog(pMe,"�ж���û���������� 56");
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_6, _OFM_CREATE);
//			uf_RecLog(pMe,"�ж���û���������� 46");
		}

		if (pTempFile != NULL)
		{
			if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) 
			{
				if (fiInfo.dwSize < FILE_OFFDATA - ITEM_OFFDATA)  // ˵�������Լ�����������
				{
					IFILE_Seek(pTempFile,_SEEK_END,0);
//					uf_RecLog(pMe,"�ж���û���������� 66");
					lInt_Len = STRLEN(c_tmp);
					IFILE_Write(pTempFile, c_tmp, lInt_Len);

					if (pTempFile != NULL)
					   IFILE_Release( pTempFile );

					if (pFileMgr != NULL)
					   IFILEMGR_Release( pFileMgr );

					pFileMgr = NULL;
					pTempFile = NULL;
					return ;
				}
				else
				{
					IFILE_Release( pTempFile );
					pTempFile = NULL;
				}
			}
		}
		else
		{
			uf_RecLog(pMe,"create offline file fail 16");
			if (pFileMgr != NULL)
				  IFILEMGR_Release( pFileMgr );
			return;
		}



		/////////  ���߸��ļ�

		if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_7) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_7, _OFM_READWRITE);
//			uf_RecLog(pMe,"�ж���û���������� 57");
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_7, _OFM_CREATE);
//			uf_RecLog(pMe,"�ж���û���������� 47");
		}

		if (pTempFile != NULL)
		{
			if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) 
			{
				if (fiInfo.dwSize < FILE_OFFDATA - ITEM_OFFDATA)  // ˵�������Լ�����������
				{
					IFILE_Seek(pTempFile,_SEEK_END,0);
//					uf_RecLog(pMe,"�ж���û���������� 67");
					lInt_Len = STRLEN(c_tmp);
					IFILE_Write(pTempFile, c_tmp, lInt_Len);

					if (pTempFile != NULL)
					   IFILE_Release( pTempFile );

					if (pFileMgr != NULL)
					   IFILEMGR_Release( pFileMgr );

					pFileMgr = NULL;
					pTempFile = NULL;
					return ;
				}
				else
				{
					IFILE_Release( pTempFile );
					pTempFile = NULL;
				}
			}
		}
		else
		{
			uf_RecLog(pMe,"create offline file fail 17");
			if (pFileMgr != NULL)
				  IFILEMGR_Release( pFileMgr );
			return;
		}




		/////////  �ڰ˸��ļ�

		if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_8) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_8, _OFM_READWRITE);
//			uf_RecLog(pMe,"�ж���û���������� 58");
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_8, _OFM_CREATE);
//			uf_RecLog(pMe,"�ж���û���������� 48");
		}

		if (pTempFile != NULL)
		{
			if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) 
			{
				if (fiInfo.dwSize < FILE_OFFDATA - ITEM_OFFDATA)  // ˵�������Լ�����������
				{
					IFILE_Seek(pTempFile,_SEEK_END,0);
//					uf_RecLog(pMe,"�ж���û���������� 68");
					lInt_Len = STRLEN(c_tmp);
					IFILE_Write(pTempFile, c_tmp, lInt_Len);

					if (pTempFile != NULL)
					   IFILE_Release( pTempFile );

					if (pFileMgr != NULL)
					   IFILEMGR_Release( pFileMgr );

					pFileMgr = NULL;
					pTempFile = NULL;
					return ;
				}
				else
				{
					IFILE_Release( pTempFile );
					pTempFile = NULL;
				}
			}
		}
		else
		{
			uf_RecLog(pMe,"create offline file fail 18");
			if (pFileMgr != NULL)
				  IFILEMGR_Release( pFileMgr );
			return;
		}



		/////////  �ھŸ��ļ�

		if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_9) == SUCCESS)
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_9, _OFM_READWRITE);
//			uf_RecLog(pMe,"�ж���û���������� 59");
		}
		else
		{
			pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_9, _OFM_CREATE);
//			uf_RecLog(pMe,"�ж���û���������� 49");
		}

		if (pTempFile != NULL)
		{
			if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) 
			{
				if (fiInfo.dwSize < FILE_OFFDATA - ITEM_OFFDATA)  // ˵�������Լ�����������
				{
					IFILE_Seek(pTempFile,_SEEK_END,0);
//					uf_RecLog(pMe,"�ж���û���������� 69");
					lInt_Len = STRLEN(c_tmp);
					IFILE_Write(pTempFile, c_tmp, lInt_Len);

					if (pTempFile != NULL)
					   IFILE_Release( pTempFile );

					if (pFileMgr != NULL)
					   IFILEMGR_Release( pFileMgr );

					pFileMgr = NULL;
					pTempFile = NULL;
					return ;
				}
				else
				{
					IFILE_Release( pTempFile );
					pTempFile = NULL;
				}
			}
		}
		else
		{
			uf_RecLog(pMe,"create offline file fail 19");
			if (pFileMgr != NULL)
				  IFILEMGR_Release( pFileMgr );
			return;
		}






	}
	else
	{
		uf_RecLog(pMe,"create offline file fail 001");
		return;
	}

	if (pTempFile != NULL)
	{
		if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) 
		{
			IFILE_Seek(pTempFile,_SEEK_END,0);
		

//			uf_RecLog(pMe,"�ж���û���������� 6");
	   
		   lInt_Len = STRLEN(c_tmp);
		   IFILE_Write(pTempFile, c_tmp, lInt_Len);
		}
	}

//	uf_RecLog(pMe,"�ж���û���������� 7");

   // Free the IFileMgr and IFile instances
	if (pTempFile != NULL)
	   IFILE_Release( pTempFile );
		

	if (pFileMgr != NULL)
	   IFILEMGR_Release( pFileMgr );

	pFileMgr = NULL;
	pTempFile = NULL;
}




/* ====================================================================
    ��post��ʽ������������
======================================================================*/
void newgimis_PostOffData(newgimis * pMe)
{
	char lStr_Fmt[251] = {0};
	int iLoc;
	int si_Ret = 0; 
	
	MEMSET(pMe->sUrl_lx, 0x00,251); //�������ߵ���ַ
	//1. ���÷��͵�http��ַ: pMe->sUrl_lx
	// pMe->strCenterIp = zc.gota.richmap.cn/brewGPS/BrewServlet?act=httpdec  תΪ zc.gota.richmap.cn/LocationServlet

	MEMSET(lStr_Fmt,0,sizeof(lStr_Fmt));
    iLoc = ud_DistToChar(pMe->strCenterIp,'/');   // ��ȡ�ϱ���IP��ַ
	if ( iLoc >= 0 )
		MEMCPY(lStr_Fmt, pMe->strCenterIp, iLoc); 
	else
		STRCPY(lStr_Fmt, pMe->strCenterIp);

	if (STRLEN(pMe->MyMobileNo) > 0)
		SPRINTF(pMe->sUrl_lx,"http://%s/LocationServlet?tel=%s&imsi=%s",lStr_Fmt,pMe->MyMobileNo,pMe->szMobileID);
	else
		SPRINTF(pMe->sUrl_lx,"http://%s/LocationServlet",lStr_Fmt);

	uf_RecLog(pMe, pMe->sUrl_lx );



	// ��ȡ�ϱ���ַ����
	myiweb_Stop_slx(pMe); //�����ŵ���������Ϊ�����и� body��ָ�뱻�ͷ���

	//2. ��ȡ�ļ��������������  pMe->pszPostData_slx�����ػ�ȡ���ǵڼ�����������
	si_Ret = Get_LX_GPSData(pMe);
	pMe->m_WitchLX = si_Ret;
	if (si_Ret) //˵����ȡ���ݳɹ�
	{
		//3. ���͡�
		myiweb_Start_slx(pMe, pMe->sUrl_lx,0);
	}
	else
	{
		uf_RecLog(pMe,"get offline data file");
	}

	
	
	
	// ��ȡ�������� 
//	if ( uf_ReadCheckOn(pMe,'1') != 1)
//		return;
	
	// ��ʼWeb����
//	uf_PostDataStart(pParam);		

}




int Get_LX_GPSData(newgimis *pMe)  //��ȡ�������� �ɹ� ���� > 0  ����ڼ����ļ�  ʧ�ܷ��� 0
{
	IFileMgr		*pFileMgr = NULL;
	IFile			*pTempFile = NULL;
    FileInfo fiInfo;		// �ļ���Ϣ
	int lInt_Len = 0;
	int i_Ret = 0;
	int i_Witch = 0;


	int iLen = 0;
	int iLen1 = 0;
	char lStr_Fmt[200];
	int iFileRead = 0;
	char *sc_buf = NULL;
	
	sc_buf = (char *) MALLOC (LX_FILE_OFFDATA);//
	MEMSET( sc_buf ,0x00,LX_FILE_OFFDATA);
	i_Witch = gimis_judgedata(pMe);

	// �����ļ��ӿ�
	if (ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) == SUCCESS)
	{
		switch(i_Witch)
		{
		case 1:
			{
				if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_1) == SUCCESS)
				{
				    pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_1, _OFM_READWRITE);
//					uf_RecLog(pMe,"����  �ж���û���������� 5");
				}
				else
				{
					i_Ret = 0;
//					uf_RecLog(pMe," ����  ��ȡ�����ļ�ʧ�� 11");
				}
			}
			break;
		case 2:
			{
				if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_2) == SUCCESS)
				{
				    pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_2, _OFM_READWRITE);
//					uf_RecLog(pMe,"����  �ж���û���������� 5");
				}
				else
				{
					i_Ret = 0;
//					uf_RecLog(pMe," ����  ��ȡ�����ļ�ʧ�� 11");
				}
			}
			break;
		case 3:
			{
				if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_3) == SUCCESS)
				{
				    pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_3, _OFM_READWRITE);
//					uf_RecLog(pMe,"����  �ж���û���������� 5");
				}
				else
				{
					i_Ret = 0;
//					uf_RecLog(pMe," ����  ��ȡ�����ļ�ʧ�� 11");
				}
			}
			break;
		case 4:
			{
				if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_4) == SUCCESS)
				{
				    pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_4, _OFM_READWRITE);
//					uf_RecLog(pMe,"����  �ж���û���������� 5");
				}
				else
				{
					i_Ret = 0;
//					uf_RecLog(pMe," ����  ��ȡ�����ļ�ʧ�� 11");
				}
			}
			break;
		case 5:
			{
				if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_5) == SUCCESS)
				{
				    pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_5, _OFM_READWRITE);
//					uf_RecLog(pMe,"����  �ж���û���������� 5");
				}
				else
				{
					i_Ret = 0;
//					uf_RecLog(pMe," ����  ��ȡ�����ļ�ʧ�� 11");
				}
			}
			break;
		case 6:
			{
				if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_6) == SUCCESS)
				{
				    pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_6, _OFM_READWRITE);
//					uf_RecLog(pMe,"����  �ж���û���������� 5");
				}
				else
				{
					i_Ret = 0;
//					uf_RecLog(pMe," ����  ��ȡ�����ļ�ʧ�� 11");
				}	
			}
			break;
		case 7:
			{
				if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_7) == SUCCESS)
				{
				    pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_7, _OFM_READWRITE);
//					uf_RecLog(pMe,"����  �ж���û���������� 5");
				}
				else
				{
					i_Ret = 0;
//					uf_RecLog(pMe," ����  ��ȡ�����ļ�ʧ�� 11");
				}	
			}
			break;
		case 8:
			{
				if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_8) == SUCCESS)
				{
				    pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_8, _OFM_READWRITE);
//					uf_RecLog(pMe,"����  �ж���û���������� 5");
				}
				else
				{
					i_Ret = 0;
//					uf_RecLog(pMe," ����  ��ȡ�����ļ�ʧ�� 11");
				}
			}
			break;
		case 9:
			{
				if (IFILEMGR_Test(pFileMgr, SPD_OFFLINE_9) == SUCCESS)
				{
				    pTempFile = IFILEMGR_OpenFile(pFileMgr, SPD_OFFLINE_9, _OFM_READWRITE);
//					uf_RecLog(pMe,"����  �ж���û���������� 5");
				}
				else
				{
					i_Ret = 0;
//					uf_RecLog(pMe," ����  ��ȡ�����ļ�ʧ�� 11");
				}
			}
			break;
		}
		

		if (pTempFile != NULL)
		{

			if ( SUCCESS == IFILE_GetInfo( pTempFile, &fiInfo ) ) 
			{
				if (fiInfo.dwSize > 0 )  // ˵�������Լ�����������
				{
					IFILE_Seek(pTempFile,_SEEK_START,0);  //ʹ�� IFILE_Seek() ���õ�ǰ�ļ�ָ��λ��ʹ��
//					uf_RecLog(pMe,"����  �ж���û���������� 6");


					iLen = ud_DistToChar(pMe->strCenterIp,'?');	
					iLen1 = STRLEN(pMe->strCenterIp);
					MEMSET(lStr_Fmt,0x00,200);
					MEMCPY(lStr_Fmt,pMe->strCenterIp+iLen+1,(iLen1-iLen-1));
					SPRINTF(lStr_Fmt,"%s&ver=%s&pp=",lStr_Fmt,pMe->sVer_1);



					iLen1 = STRLEN(lStr_Fmt);

					iFileRead = fiInfo.dwSize + iLen1;
					pMe->pszPostData_slx = MALLOC( iFileRead  + 1 );
					STRCPY(pMe->pszPostData_slx,lStr_Fmt);

					
					

					lInt_Len = IFILE_Read( pTempFile, sc_buf, fiInfo.dwSize );
					if ( (uint32)lInt_Len == fiInfo.dwSize ) 
					{ // ��ȡ�ɹ�
						
						STRCAT(pMe->pszPostData_slx,sc_buf);
//						uf_RecLog(pMe,"�����ļ������������");
						uf_RecLog(pMe, pMe->pszPostData_slx );
						i_Ret = i_Witch;
					} 
					else
					{
						i_Ret = 0;
					}
				}
			}
		}
		
	}


	if (pTempFile != NULL)
		IFILE_Release( pTempFile );

	if (pFileMgr != NULL)
		IFILEMGR_Release( pFileMgr );

	pFileMgr = NULL;
	pTempFile = NULL;
	return i_Ret;
}

// ������ɾ�������ļ�

int uf_Del_ConfigFile(newgimis *pMe)
{
	IFileMgr* pFileMgr = NULL;
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
		return 0;
	if ( IFILEMGR_Remove(pFileMgr, SPD_OFFLINE_1) == SUCCESS ) 
	{
		return 1;
	}
	else
	{
		return 0;
	}

	if (pFileMgr)
	{
		IFILEMGR_Release(pFileMgr);
		pFileMgr = NULL;
	}
}
// =============================================================
// ɾ�������ļ�
// =============================================================
int uf_DelOff_File(newgimis *pMe)
{
	IFileMgr* pFileMgr = NULL;
	if ( ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)(&pFileMgr)) != SUCCESS )
		return 0;
	switch(pMe->m_WitchLX)
	{
	case 1:
		{
			if ( IFILEMGR_Remove(pFileMgr, SPD_OFFLINE_1) == SUCCESS ) 
			{
				return 1;
			}
			else
			{
				return 0;
			}
		
		}
	break;

	case 2:
		{
			if ( IFILEMGR_Remove(pFileMgr, SPD_OFFLINE_2) == SUCCESS ) 
			{
				return 1;
			}
			else
			{
				return 0;
			}
		
		}
	break;

	case 3:
		{
			if ( IFILEMGR_Remove(pFileMgr, SPD_OFFLINE_3) == SUCCESS ) 
			{
				return 1;
			}
			else
			{
				return 0;
			}
		
		}
	break;

	case 4:
		{
			if ( IFILEMGR_Remove(pFileMgr, SPD_OFFLINE_4) == SUCCESS ) 
			{
				return 1;
			}
			else
			{
				return 0;
			}
		
		}
	break;

	case 5:
		{
			if ( IFILEMGR_Remove(pFileMgr, SPD_OFFLINE_5) == SUCCESS ) 
			{
				return 1;
			}
			else
			{
				return 0;
			}
		
		}
	break;

	case 6:
		{
			if ( IFILEMGR_Remove(pFileMgr, SPD_OFFLINE_6) == SUCCESS ) 
			{
				return 1;
			}
			else
			{
				return 0;
			}
		
		}
	break;

	case 7:
		{
			if ( IFILEMGR_Remove(pFileMgr, SPD_OFFLINE_7) == SUCCESS ) 
			{
				return 1;
			}
			else
			{
				return 0;
			}
		
		}
	break;

	case 8:
		{
			if ( IFILEMGR_Remove(pFileMgr, SPD_OFFLINE_8) == SUCCESS ) 
			{
				return 1;
			}
			else
			{
				return 0;
			}
		
		}
	break;

	case 9:
		{
			if ( IFILEMGR_Remove(pFileMgr, SPD_OFFLINE_9) == SUCCESS ) 
			{
				return 1;
			}
			else
			{
				return 0;
			}
		
		}
	break;

	}
	

	if (pFileMgr)
	{
		IFILEMGR_Release(pFileMgr);
		pFileMgr = NULL;
	}
	return 0;

}



// ============================================
// ��[�����ڲ�ѯ�İ���
static boolean newgimis_NotifyFuncKey(newgimis *pMe, AEEEvent eCode,uint16 wParam)
{
	boolean bRtn = TRUE;
	char cKey = 0;
	int si = 0;

	if (eCode != EVT_KEY_PRESS)
		return FALSE;

	//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"������ϼ�,wParam=%d,m_PressSts=%d,iGetkey=%d", pMe->wParam,pMe->m_PressSts,pMe->iGetkey);

	switch (wParam)
	{
	case AVK_9:
		cKey = '9';
		break;
	case AVK_STAR:
		cKey = '*';
		break;
	case AVK_POUND:
		cKey = '#';
		break;
	case AVK_CLR:
	case AVK_END:
		MEMSET(pMe->m_zuhe,0x00,sizeof(pMe->m_zuhe));
		break;
	}
	if (cKey != 0) 
	{	// 1
		for (si = 0; si < 5;si++)
		{
			if (pMe->m_zuhe[si] == 0)
			{
				pMe->m_zuhe[si] = cKey;
				break;
			}
			else
			{
				continue ;
			}
			
		}
		if ( STRCMP(pMe->m_zuhe,"*#999" ) == 0 ) 
		{   // --1			// *#999
			ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
		}
			
	}
	


	return bRtn;
}




/* ===========================================================================================
˵��		
   ����ע��İ���
�������
   ͬ gimis_HandleEvent

��������
   TRUE ������ؼ��Ѿ���������Ϣ�� FALSE ����ؼ�û�д�����Ϣ
============================================================================================== */
static boolean gimis_NotifyShellKey(newgimis *pMe, AEEEvent eCode, uint16 wParam, uint32 dwParam)
{
	AEENotify* pNotify = (AEENotify*) dwParam;
	NotifyKeyEvent *pNKeyEvn = (NotifyKeyEvent*) pNotify->pData;

	
	
    // 2013-10-01 ���� AVK_7

//	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"����ֵ %d", pNKeyEvn->wParam);
//	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"����ֵ ���� %d", pNKeyEvn->eCode);
//	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"����ֵ ����  %d", pMe->m_PressSts);

	if(pMe->m_FOBFlag == 0)
	{
		MEMSET(pMe->m_zuhe,0x00,sizeof(pMe->m_zuhe));
		pMe->zuhe_loong = 0 ;// �ص�������״̬
		pMe->m_Key_Count = 0;// ������ǵ�ǰ�ж��ټ�������
		return TRUE;
	}

	if (pNKeyEvn->eCode == EVT_KEY_PRESS)
	{
		pMe->m_Key_Count++;
	}
	if ((pNKeyEvn->eCode == EVT_KEY_RELEASE) && (pMe->m_Key_Count > 0))
	{
		pMe->m_Key_Count--;
	}

	if (pMe->m_Key_Count > 1) //˵������������ͬʱ���� ���ʱ�����еĶ���Ч
	{
		// ������������ͬʱ�����µ�ʱ�������ϼ��Ѿ������İ���ֻ
		MEMSET(pMe->m_zuhe,0x00,sizeof(pMe->m_zuhe));
		pMe->zuhe_loong = 0 ;// �ص�������״̬

		// ������������ͬʱ�����µ�ʱ������������йص�����
		pMe->m_PressSts = 0;
		pMe->iGetkey = 0;
		pMe->wParam  = 0;

		pMe->m_Key_Count = 0;

		
		return TRUE;
	}

	if (pMe->zuhe_loong == 0) //��������ʱ�� ���� * ���л�����ϼ�״̬
	{
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"������ϼ�57387,pNKeyEvn->wParam=%d", pNKeyEvn->wParam);
		if  (pNKeyEvn->wParam == AVK_STAR) // '*'
		{
			MEMSET(pMe->m_zuhe,0x00,sizeof(pMe->m_zuhe));
			pMe->zuhe_loong = 1 ;//����뵽��ϼ�
		}
	}
	if (pMe->zuhe_loong == 1)
	{
		if ( (pNKeyEvn->wParam == AVK_STAR) || (pNKeyEvn->wParam == AVK_POUND) || (pNKeyEvn->wParam == AVK_9))
		{
			if ( newgimis_NotifyFuncKey(pMe,pNKeyEvn->eCode,pNKeyEvn->wParam) )// ����������鿴���� ���� *#999
			{
				return TRUE;
			}
			
		}
		else
		{
				MEMSET(pMe->m_zuhe,0x00,sizeof(pMe->m_zuhe));
				pMe->zuhe_loong = 0 ;// �ص�������״̬
			//	pMe->m_Key_Count = 0;
			//	return TRUE;
		}
		
	}
	
	if (pMe->cUseLoad == '1')  // ˵����������������
	{
		if (pMe->zuhe_loong == 1)
		{
			return TRUE;
		}
	
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"NotifyShellKey 1,wParam=%d,m_PressSts=%d,iGetkey=%d,eCode=%d", pMe->wParam,pMe->m_PressSts,pMe->iGetkey,pNKeyEvn->eCode);
//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"NotifyShellKey 1,pNKeyEvn->wParam=%d", pNKeyEvn->wParam);


		if ( (pNKeyEvn->wParam == AVK_2) || (pNKeyEvn->wParam == AVK_3) || (pNKeyEvn->wParam == AVK_5) || 
			 (pNKeyEvn->wParam == AVK_6) || (pNKeyEvn->wParam == AVK_8) || (pNKeyEvn->wParam == AVK_9) ||
			 (pNKeyEvn->wParam == AVK_0) )	
		{	// AVK_02

	//		if (gimis_IsTextCtl(pMe))	// ����Ƿ���������Ϣ��״̬
	//			return FALSE;

//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"NotifyShellKey,wParam=%d,m_PressSts=%d,iGetkey=%d,eCode=%d", pMe->wParam,pMe->m_PressSts,pMe->iGetkey,pNKeyEvn->eCode);
//			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"NotifyShellKey,pNKeyEvn->wParam=%d", pNKeyEvn->wParam);

			// �����û�����������
			/*
			if ( (pMe->m_PressSts == 1) && (pMe->iGetkey != 0) && (pMe->wParam != 0) && (pMe->wParam != pNKeyEvn->wParam)  ) {
				pMe->m_PressSts = 0;
				pMe->iGetkey = 0;
				pMe->wParam  = 0;
			}
			*/
			
			if ( (pMe->m_PressSts == 0) && (pMe->iGetkey == 0) && (pNKeyEvn->eCode != EVT_KEY_RELEASE) )   
			{
				pMe->iGetkey = GETTIMEMS();
				pMe->m_PressSts = 1;
// 				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"==============");
// 				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"NotifyShellKey,pNKeyEvn->wParam=%d", pNKeyEvn->wParam);
				if ( STRCMP(pMe->sMobileType,"33") == 0 )
				{
					pMe->wParam = pNKeyEvn->wParam;			// �����ֻ�����
					ISHELL_SetTimer(pMe->a.m_pIShell, DEFAULT_KEYTIME*1000, (PFNNOTIFY)newgimis_NotifyShellKey_33, (uint32*)pMe);

					//��ע ����ҳ���1�� �ſ� 3���Ҳ��ȥ
				}
				
			}

			else if ( (pMe->m_PressSts == 1) && ((GETTIMEMS() -  pMe->iGetkey) >= DEFAULT_KEYTIME *1000) && (pMe->iGetkey > 0) && (pNKeyEvn->eCode == EVT_KEY))
			{	// if -- a
				pMe->iGetkey = 0;
				pMe->wParam = pNKeyEvn->wParam;			// �����ֻ�����
	//			uf_NotifyKey(pMe);
				pMe->m_PressSts = 0;// ˵���Ѿ�������

				//20180328
				if (pNKeyEvn->wParam == AVK_0)  // �鿴�����ռ���
				{
					if (pMe->m_FOBFlag == 1)
					{
						pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
						pMe->m_Witch_Key = 12; // ��������Ǵ���ʲô���ܵ�  
						ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
					}
				}


				if (pNKeyEvn->wParam == AVK_3)  // һ���򿨣�����ת����
				{
					if (pMe->cPicMode == '4')
					{
						if (pMe->m_FOBFlag == 1)
						{
							pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
							pMe->m_Witch_Key = 11; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
							ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
						}
					}
				}

				if (pNKeyEvn->wParam == AVK_5)  // ������Ƭ
				{
					if (pMe->m_FOBFlag == 1)
					{
						pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
						pMe->m_Witch_Key = 5; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
						ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
					}
				}

				if (pNKeyEvn->wParam == AVK_8) // ������Ƶ
				{
					if (pMe->m_FOBFlag == 1)
					{
						pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
						pMe->m_Witch_Key = 8; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
						ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
					}
				}

				if (pNKeyEvn->wParam == AVK_2) // ���Ϳ���
				{
					if (pMe->m_FOBFlag == 1)
					{
						pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
						pMe->m_Witch_Key = 2; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ   
						ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
					}
				}

				if (pNKeyEvn->wParam == AVK_6) // ���Ϳճ� �� ����״̬ 0 
				{
					if (pMe->m_FOBFlag == 1)
					{
						switch(pMe->cPicMode)
						{
						case '0':  //�ճ�  
							{
								pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
								pMe->m_Witch_Key = 4; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
								ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
							}
							break;
						case '1': // ����״̬ 0 
							{
								pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
								pMe->m_Witch_Key = 10; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
								ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
							}
							break;
						}
					}
				}

				if (pNKeyEvn->wParam == AVK_9) // �����س� �� ����״̬ 1 
				{
					if (pMe->m_FOBFlag == 1)
					{
						switch(pMe->cPicMode)
						{
						case '0':  //�س�  
							{
								pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
								pMe->m_Witch_Key = 3; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
								ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
							}
							break;
						case '1': // ����״̬  1
							{
								pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
								pMe->m_Witch_Key = 9; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
								ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
							}
							break;
						}
					}
				}

				/*/////////   add һ�����й���
				if (pNKeyEvn->wParam == pMe->nPttkey)
				{
					crichptt *me = (crichptt *)pMe->pRichptt;
					uint32 nGetTimes = GETTIMEMS();



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


*/
				
			}		// if -- a

			else if (pNKeyEvn->eCode == EVT_KEY_RELEASE)  
			{ // if -- a0
				if ( STRCMP(pMe->sMobileType,"33") == 0  && (pMe->wParam == pNKeyEvn->wParam)) 
				{
					if (pMe->iGetkey > 0)
					{
						ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY) newgimis_NotifyShellKey_33, (uint32*) pMe);
					}
					pMe->iGetkey = 0;
					pMe->m_PressSts = 0;
					pMe->wParam = 0;
					
				}
				else
				{
					pMe->iGetkey = 0;
					pMe->m_PressSts = 0;
					pMe->wParam = 0;				
				}

//				uf_RecLog(pMe,"���� ��û�н�������");
			}	// if -- a0

			return TRUE;
		}	// AVK_02
		else
			return FALSE;
	}
	return TRUE;
}




/********************************************************************************************
	���http�Ƿ�ʱ
**********************************************************************************************/
void uf_MonitorPost(newgimis *pMe)                        // (WebAction * pwa)		// b61 2012-02-09		
{

//	AECHAR szText[6];
    AECHAR szFailure[] = {0x4E2D,0x5FC3,0x65E0,0x5E94,0x7B54,0x0000};	// ������Ӧ��

	if ( pMe->iPostPhTm == 0)
		return;


	// �������60��û�յ�������֣������ϵ�
	/*
		 WSTRCPY(szText,szFailure);

		 IDISPLAY_ClearScreen(pMe->theApp.m_pIDisplay);
		 IDISPLAY_DrawText(pMe->theApp.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
			 8, 2,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
		 IDISPLAY_Update(pMe->theApp.m_pIDisplay);

		 // ����ʧ�ܣ�������ʾ
		 ISHELL_Beep(pMe->theApp.m_pIShell, BEEP_MSG, TRUE);
		 ISHELL_Beep(pMe->theApp.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
		 ISHELL_SetTimer(pMe->theApp.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*) pMe);

	*/
 	 
		pMe->m_F_QQSQ_CS = 1; //��������ǲ���������Ȩ��ʱ  1 -- ��ʱ  0 -- û�г�ʱ 

//		if (pMe->m_WitchPag == 10)
		if ((pMe->m_WitchPag) == 10 && (pMe->m_F_DJ != 1))	
		{		
	//		ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_PRINT,0, 0);
			ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
		}


	 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"uf_MonitorPost in, http time out");
//	 uf_AfterSend(pMe,"0");				// 2013-10-01
	 myiweb_Stop(pMe);
	 
	 if (pMe->m_SQ_C == 1)
	 {
		myiweb_Stop(pMe);
		myiweb_Start(pMe,pMe->m_SQ2,0);
		uf_RecLog(pMe, pMe->m_SQ2 );
		pMe->m_SQ_C = 2;
		return;
	 }
	 ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_QQSQ_SB_WLYC,0, 0);	// //������Ȩʧ�ܵķ����¼�	 //����Ҳ��ʾ������Ȩʧ��
     return;


}



/********************************************************************************************
	���http�Ƿ�ʱ
**********************************************************************************************/
void uf_MonitorPost_xt(newgimis *pMe)                        // (WebAction * pwa)		// b61 2012-02-09		
{

//	AECHAR szText[6];
    AECHAR szFailure[] = {0x4E2D,0x5FC3,0x65E0,0x5E94,0x7B54,0x0000};	// ������Ӧ��

	if ( pMe->iPostPhTm_xt == 0)
		return;


	// �������60��û�յ�������֣������ϵ�
	/*
		 WSTRCPY(szText,szFailure);

		 IDISPLAY_ClearScreen(pMe->theApp.m_pIDisplay);
		 IDISPLAY_DrawText(pMe->theApp.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
			 8, 2,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
		 IDISPLAY_Update(pMe->theApp.m_pIDisplay);

		 // ����ʧ�ܣ�������ʾ
		 ISHELL_Beep(pMe->theApp.m_pIShell, BEEP_MSG, TRUE);
		 ISHELL_Beep(pMe->theApp.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
		 ISHELL_SetTimer(pMe->theApp.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*) pMe);

	*/
 	 
	pMe->m_MUTEX = 0;

	 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"uf_MonitorPost in xin tiao, http time out");
//	 uf_AfterSend(pMe,"0");				// 2013-10-01
	 myiweb_Stop_xt(pMe);

}




// ���HTTP�Ƿ�ʱ һ������ �� ״̬
void uf_MonitorPost_kq_zt(newgimis *pMe)	 // ������Ȩ��ʱ������������ ˵�� ���ܺ�ƽ̨���� ���е����ݶ������ߵ� ��λҲֻ�������Ƕ�λ
{
	//	AECHAR szText[6];
    AECHAR szFailure[] = {0x4E2D,0x5FC3,0x65E0,0x5E94,0x7B54,0x0000};	// ������Ӧ��

	if ( pMe->iPos_kq_zt == 0)
		return;


	// �������60��û�յ�������֣������ϵ�
	/*
		 WSTRCPY(szText,szFailure);

		 IDISPLAY_ClearScreen(pMe->theApp.m_pIDisplay);
		 IDISPLAY_DrawText(pMe->theApp.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
			 8, 2,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
		 IDISPLAY_Update(pMe->theApp.m_pIDisplay);

		 // ����ʧ�ܣ�������ʾ
		 ISHELL_Beep(pMe->theApp.m_pIShell, BEEP_MSG, TRUE);
		 ISHELL_Beep(pMe->theApp.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
		 ISHELL_SetTimer(pMe->theApp.m_pIShell, 3 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*) pMe);
	*/

	 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"uf_MonitorPost_kq_zt in kao qing, http time out");


//	 uf_AfterSend(pMe,"0");				// 2013-10-01
	 myiweb_Stop_lx(pMe);
	
	 //һ������  [3/4/2016 Administrator]
	 pMe->m_WitchPag = 10;
     testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
}


/********************************************************************************************
	���http�Ƿ�ʱ
**********************************************************************************************/
void uf_MonitorPost_gps(newgimis *pMe)                        // (WebAction * pwa)		// b61 2012-02-09		
{

//	AECHAR szText[6];
    AECHAR szFailure[] = {0x4E2D,0x5FC3,0x65E0,0x5E94,0x7B54,0x0000};	// ������Ӧ��

	if ( pMe->iPostPhTm_gps == 0)
		return;


	// �������60��û�յ�������֣������ϵ�
	/*
		 WSTRCPY(szText,szFailure);

		 IDISPLAY_ClearScreen(pMe->theApp.m_pIDisplay);
		 IDISPLAY_DrawText(pMe->theApp.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
			 8, 2,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
		 IDISPLAY_Update(pMe->theApp.m_pIDisplay);

		 // ����ʧ�ܣ�������ʾ
		 ISHELL_Beep(pMe->theApp.m_pIShell, BEEP_MSG, TRUE);
		 ISHELL_Beep(pMe->theApp.m_pIShell, BEEP_VIBRATE_ALERT, TRUE);
		 ISHELL_SetTimer(pMe->theApp.m_pIShell, 5 * 1000, (PFNNOTIFY) uf_StopBeep, (uint32*) pMe);

	*/
 	 

	pMe->m_MUTEX = 0;

	 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"uf_MonitorPost in gps, http time out");


	 if (pMe->mux_7Day == 0)
	 {
		 write_GPSData_7Day(pMe, pMe->sUrl);
	 }
//	 uf_AfterSend(pMe,"0");				// 2013-10-01
	 myiweb_Stop_gps(pMe);

}


//  ��ȡ���͵�ģʽ
void Send_PM(newgimis *pMe, int pi)
{
	switch(pi)
	{
	case 1:
		{
			switch (pMe->m_gpsFlag)
			{
			case 0:
				pMe->m_PM1 = '2'; 
				break;
			case 1:
				pMe->m_PM1 = '5'; 
				break;
			case 2:
				pMe->m_PM1 = '1'; 
				break;
			case 3:
				pMe->m_PM1 = '0'; 
				break;
			case 4:
				pMe->m_PM1 = '9'; 
				break;
			case 5:
				pMe->m_PM1 = '6'; 
				break;
			}
		}
		break;
	case 2:
		{
			switch (pMe->m_gpsFlag)
			{
			case 0:
				pMe->m_PM2 = '2'; 
				break;
			case 1:
				pMe->m_PM2 = '5'; 
				break;
			case 2:
				pMe->m_PM2 = '1'; 
				break;
			case 3:
				pMe->m_PM2 = '0'; 
				break;
			case 4:
				pMe->m_PM2 = '9'; 
				break;
			case 5:
				pMe->m_PM2 = '6'; 
				break;
			}
		}
		break;	

	case 3:
		{
			switch (pMe->m_gpsFlag)
			{
			case 0:
				pMe->m_PM3 = '2'; 
				break;
			case 1:
				pMe->m_PM3 = '5'; 
				break;
			case 2:
				pMe->m_PM3 = '1'; 
				break;
			case 3:
				pMe->m_PM3 = '0'; 
				break;
			case 4:
				pMe->m_PM3 = '9'; 
				break;
			case 5:
				pMe->m_PM3 = '6'; 
				break;
			}
		}
		break;	

	case 4:
		{
			switch (pMe->m_gpsFlag)
			{
			case 0:
				pMe->m_ycPM = '2'; 
				break;
			case 1:
				pMe->m_ycPM = '5'; 
				break;
			case 2:
				pMe->m_ycPM = '1'; 
				break;
			case 3:
				pMe->m_ycPM = '0'; 
				break;
			case 4:
				pMe->m_ycPM = '9'; 
				break;
			case 5:
				pMe->m_ycPM = '6'; 
				break;
			}
		}
		break;	


		case 5:
		{
			switch (pMe->m_gpsFlag)
			{
			case 0:
				pMe->m_PM5 = '2'; 
				break;
			case 1:
				pMe->m_PM5 = '5'; 
				break;
			case 2:
				pMe->m_PM5 = '1'; 
				break;
			case 3:
				pMe->m_PM5 = '0'; 
				break;
			case 4:
				pMe->m_PM5 = '9'; 
				break;
			case 5:
				pMe->m_PM5 = '6'; 
				break;
			}
		}
		break;	

		case 6:
		{
			switch (pMe->m_gpsFlag)
			{
			case 0:
				pMe->m_PM6 = '2'; 
				break;
			case 1:
				pMe->m_PM6 = '5'; 
				break;
			case 2:
				pMe->m_PM6 = '1'; 
				break;
			case 3:
				pMe->m_PM6 = '0'; 
				break;
			case 4:
				pMe->m_PM6 = '9'; 
				break;
			case 5:
				pMe->m_PM6 = '6'; 
				break;
			}
		}
		break;	


		case 7:
		{
			switch (pMe->m_gpsFlag)
			{
			case 0:
				pMe->m_PM7 = '2'; 
				break;
			case 1:
				pMe->m_PM7 = '5'; 
				break;
			case 2:
				pMe->m_PM7 = '1'; 
				break;
			case 3:
				pMe->m_PM7 = '0'; 
				break;
			case 4:
				pMe->m_PM7 = '9'; 
				break;
			case 5:
				pMe->m_PM7 = '6'; 
				break;
			}
		}
		break;	


		case 8:
		{
			switch (pMe->m_gpsFlag)
			{
			case 0:
				pMe->m_PM8 = '2'; 
				break;
			case 1:
				pMe->m_PM8 = '5'; 
				break;
			case 2:
				pMe->m_PM8 = '1'; 
				break;
			case 3:
				pMe->m_PM8 = '0'; 
				break;
			case 4:
				pMe->m_PM8 = '9'; 
				break;
			case 5:
				pMe->m_PM8 = '6'; 
				break;
			}
		}
		break;	

		case 9:
		{
			switch (pMe->m_gpsFlag)
			{
			case 0:
				pMe->m_PM4 = '2'; 
				break;
			case 1:
				pMe->m_PM4 = '5'; 
				break;
			case 2:
				pMe->m_PM4 = '1'; 
				break;
			case 3:
				pMe->m_PM4 = '0'; 
				break;
			case 4:
				pMe->m_PM4 = '9'; 
				break;
			case 5:
				pMe->m_PM4 = '6'; 
				break;
			}
		}
		break;	
		
	}
}





// =======================================================================
//  ����յ��Ķ�������
// �������
//   char* rStr_Msg		�����еĲ���	
// ��������
//    return     '0' �� ������Ϣ��ָ������л�δ���� '1'���Բ����Ѿ����˴���,�������ֻ�
//				 '2' : �� �Բ����Ѿ����˴���
// =======================================================================
char gimis_CheckMsg(newgimis *pMe, char* rChr_Msg)
{
	char lChr_Rtn = '1';		
	int32   nResult = 0;
	
	char stopstring[2]  = ";";
	char stopstring_1[2]  = ",";

	char lChr_Ind;					// ��Ϣ�ĵ�һ���ַ������ݵĵ�һ���ж�ָ�������
	int lInt_Tmp;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"_CheckMsg in,%s",rChr_Msg);

	lChr_Ind = *rChr_Msg;
	if (lChr_Ind >= 65 ) {		// if -- 65
		lChr_Rtn = lChr_Ind;
		switch (lChr_Rtn)
		{	// switch lChr_Rtn
			case 'C':			// �������Ķ�IP��ַ
			   gimis_InitGPSSettings( pMe );	// ��ȡ��ԭ��������
			   nResult = ud_DistToChar(rChr_Msg,';') - 1; // uf_DistToSemi( rChr_Msg ) - 1;
			   MEMSET(pMe->strCenterIp,0,sizeof(pMe->strCenterIp));
			   MEMCPY(pMe->strCenterIp,rChr_Msg+1, nResult);
			   gimis_SetGPSPara(pMe);	// д���µ�����	
				goto ExitHere;
			case 'D':			// ��λ��ʽ,�´ο�����Ч
			   gimis_InitGPSSettings( pMe );
			   if (*(rChr_Msg+2) == ';' ) {
				   if ( (*(rChr_Msg+1) >= '0') && (*(rChr_Msg+1) <= '9') ) {	// Add by zjie 2012-09-21 �������ж�����
					   pMe->cPosDetMode = *(rChr_Msg+1);
					   gimis_SetGPSPara(pMe);	
				   }
			   }
			   	lChr_Rtn = '2';
				goto ExitHere;
			case 'E':			// ���ȵȼ�,�´ο�����Ч
			   gimis_InitGPSSettings( pMe );
				if ( (*(rChr_Msg+1) >= '1') && (*(rChr_Msg+1) <= '6') ) {	// Add by zjie 2012-09-21 �������ж�����
					pMe->cAccType = *(rChr_Msg+1);
					gimis_SetGPSPara(pMe);	
				}
				lChr_Rtn = '2';
				goto ExitHere;
			case 'F':		// ָ�� F3,1,60;	(�ϱ���λ�Ĵ���,ÿ�μ���,�ϱ�������) 
				{
					int iPosCnt,iArySize,iInterval;
					int iLoc;
					DBGPRINTF("msg0=%s",rChr_Msg);
					iPosCnt = STRTOUL( rChr_Msg+1, (char **)&stopstring_1, 10 );	// ��λ�Ĵ���
					DBGPRINTF("msm a1,Cnt=%d",iPosCnt);
					if (iPosCnt > 1) {
						lInt_Tmp = ud_DistToChar(rChr_Msg,',');
						iLoc = lInt_Tmp;
						iArySize = STRTOUL( rChr_Msg+iLoc+1, (char **)&stopstring_1, 10 );	// ÿ���ϱ�����

						lInt_Tmp = ud_DistToChar(rChr_Msg+iLoc+1,',');
						iLoc += lInt_Tmp+1;
						iInterval = STRTOUL( rChr_Msg+iLoc+1, (char **)&stopstring, 10 );	// ��λ������

/*
						if (iInterval>0) {
							pMe->cGpsMode = '1';
							pMe->cAuthorize = '1';
							pMe->cCurAddr = '3';

							pMe->iInterval = iInterval;
							pMe->nArySize = iArySize;
							pMe->iPosCnt = iPosCnt;

							// Change by zjie 2012-10-08, �й����ŵ�������
							uf_ReStartGps(pMe);
						}
*/	
					}
					lChr_Rtn = '2';
				}
				goto ExitHere;
			case 'G':	
//				uf_ShowInfImage(pMe);
				lChr_Rtn = '2';
				goto ExitHere;
			case 'H':		// �Ƿ�д��־
//				gimis_Msg_H(pMe,(rChr_Msg+1));
				lChr_Rtn = '2';
				goto ExitHere;
			case 'I':	 // ���gimis�Ƿ��ڹ���״̬		// Add 2013-10-10
//				gimis_CheckWorkOn(pMe,(rChr_Msg+1));
				goto ExitHere;

		}	// switch lChr_Rtn

	}		// if -- 65
    else {
		// ���յ���GPSʱ����д�����ñ�ע��Ŀǰ����д���ǰ����������ļ�����д��һ��
		lInt_Tmp = STRTOUL( rChr_Msg, (char **)&stopstring, 10 );
		if ( lInt_Tmp >= 30) {			// Change by zjie  2012-09-21,     lInt_Tmp > 0 �ĳ�	lInt_Tmp >= 30
		    gimis_InitGPSSettings( pMe );
			pMe->iInterval = lInt_Tmp;
			gimis_SetGPSPara(pMe);	// д���µ�����			
		}
		lChr_Rtn = '1';
	}

ExitHere:
	return lChr_Rtn; 
}






// 30�������GPS

void GPS_Restart_fun(newgimis *pMe)
{
	newgimis_OpenGPS_tmp(pMe);
	pMe->m_FAIL_T = 0;  // 
	uf_RecLog(pMe,"coming in 30 seconds open GPS");
	return;
}


// 30�������GPS









// ����ͼƬ����
void Load_Image_fun(newgimis *pMe)
{
	
	if (pMe->pi_copy)
	{
		IIMAGE_Release(pMe->pi_copy); 
	}
	uf_RecLog(pMe,"Load_Image_fun" );
	pMe->pi_copy = ISHELL_LoadImage(pMe->a.m_pIShell, pMe->sFileName);
}



// �ж��Ƿ��� myiweb_start_xt ����ȡ������   1 - ��ʾ����   0 - ��ʾ������
int judgeSendorCancel(newgimis *pMe)
{
	uint32 si_tmp_t = 0;
	si_tmp_t = GETTIMESECONDS();
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"si_tmp_t = %ld",si_tmp_t);
//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pMe->m_T_H_G = %ld",pMe->m_T_H_G);
	if (si_tmp_t >= pMe->m_T_H_G )
	{
		if (si_tmp_t - pMe->m_T_H_G > 2) //˵�����Է�������
		{
			pMe->m_T_H_G = si_tmp_t;
			return 1;
		}
		else 
		{
			pMe->m_T_H_G = si_tmp_t - 10; 
			return 0;
		}
	}
	else
	{
		if (si_tmp_t > 3) //˵�����Է�������
		{
			pMe->m_T_H_G = si_tmp_t;
			return 1;
		}
		else
		{
			pMe->m_T_H_G = si_tmp_t - 10; 
			return 0;
		}
	}
}



// void judge_reset_TB(newgimis *pMe) // �ж��Ƿ���Ҫ���� ��������ʱ��
// {
// 	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
// 
// 	GETJULIANDATE(0,&dDate);		//	ȡ������
// 	if (pMe->iRestTb == 25)
// 	{
// 		pMe->iTBCT = dDate.wHour;
// 		return;
// 	}
// 
// 
// 	
// 	if (dDate.wHour == pMe->iRestTb) // ˵���������ʱ�����
// 	{
// 		if (pMe->iTBCT == dDate.wHour - 1) // ˵���Ǵ���һ��ʱ���������ʱ���
// 		{
// 			//�����ֻ�
// 			
// 			char *sc_tmp = NULL;
// 			uf_RecLog(pMe,"Time to restart");
// 			uf_RecLog(pMe,"Time to restart");
// 			STRCPY(sc_tmp,"123");
// 		}
// 	}
// 	else
// 	{
// 		pMe->iTBCT = dDate.wHour;
// 	}
// 
// 	return ;
// }
void judge_reset_TB(newgimis *pMe) // �ж��Ƿ���Ҫ���� ��������ʱ��
{
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	
	GETJULIANDATE(0,&dDate);		//	ȡ������
	if (pMe->iRestTb == 25)
	{
		pMe->iTBCT = dDate.wHour;
		return;
	}
	
	
	
	if (dDate.wHour == pMe->iRestTb) // ˵���������ʱ�����
	{
		if (dDate.wHour == 0 && pMe->iTBCT == 23 )
		{
			//�����ֻ�
			
			char *sc_tmp = NULL;
			uf_RecLog(pMe,"Time to restart");
			uf_RecLog(pMe,"Time to restart");
			STRCPY(sc_tmp,"123");
		}
		else
		{
			if (pMe->iTBCT == dDate.wHour - 1) // ˵���Ǵ���һ��ʱ���������ʱ���
			{
				//�����ֻ�
				
				char *sc_tmp = NULL;
				uf_RecLog(pMe,"Time to restart");
				uf_RecLog(pMe,"Time to restart");
				STRCPY(sc_tmp,"123");
			}
		}
		
	}
	else
	{
		pMe->iTBCT = dDate.wHour;
	}
	
	return ;
}


// �ж�GPSmode ʧ���� ����վ ������ ������ ��վ���� 
//  ����ֵ ��� == TRUE ��    == FALSE  ���� 
int Judge_GPS_Mode(newgimis *pMe)
{
	
	if (pMe->cPosDetMode == 0 || pMe->cPosDetMode == 1 || pMe->cPosDetMode == 2 || pMe->cPosDetMode == 6)
	{
		return 1;
	}
	return 0;
	
}


//  ��ȡGPS��Ӧ��ģʽ
int Get_GPS_Mode(newgimis *pMe)
{
	int si_Ret = 0;
	if ( pMe->cPosDetMode == 0 )
	{
		si_Ret = 3;
	}
	else if ( pMe->cPosDetMode == 1 )
	{
		si_Ret = 2;
	}
	else if ( pMe->cPosDetMode == 2 )
	{
		si_Ret = 0;
	}
	else if ( pMe->cPosDetMode == 6 )
	{
		si_Ret = 5;
	}	
	return si_Ret ;

}







///        �����ǹ���ͨѶ¼��
// ======================================================
// ���ڼ�����Ķ�ָ��ĺ���
//  �������
//     char *pszHaystack ָ���ַ���
//     pszNeedle Ҫ�Ƚϵ��ַ���(��ָ��)
//     sSplit  ָ��ǰ��ķָ���
//  ��������
//     >0 �ַ�����ָ�����ĵ�һ���ַ�λ��
//      0 �ַ����в�����Ҫ�ҵ�ָ�� 
// ======================================================
char *uf_STRSTR(const char *pszHaystack, const char *pszNeedle, const char *sSplit )
{	//
	char *sHead;
	int nHeadLen = 0;
	char *pszTok = NULL;

	nHeadLen = STRLEN(pszNeedle);
	sHead = (char *)MALLOC(nHeadLen+1+1);		// Ԥ��һ�����ָ�����
	MEMCPY(sHead,pszHaystack,nHeadLen);

	if (STRCMP(sHead,pszNeedle) == 0 ) {			// ָ�����ַ���ͷ�������
		pszTok = (char *)pszHaystack + STRLEN(sHead);
	}
	else {
		SPRINTF(sHead,"%s%s",sSplit,pszNeedle);				// ָ�����ַ����м�����
		pszTok = STRSTR( pszHaystack, sHead );
		if (pszTok)
			pszTok = pszTok+ STRLEN(sHead);
	}
	
	FREEIF(sHead);
	return pszTok;
	
}

/////  �����ǹ���QCHAT ��غ���   START      ///////////////////
void newgimis_WaitForQChat(newgimis *pMe)
{
	boolean bRtn = FALSE;
	int lInt_StartRun ;
	int nTimes = 0;

#ifdef PHONE_QCHAT_DUIJIANG

	bRtn = richptt_HandleEvent(pMe,EVT_APP_START,0,0);
	
	if (bRtn == FALSE)
	{
		if (pMe->pRichptt == NULL)
		{
//			ISHELL_SetTimer(pMe->a.m_pIShell, nTimes*1000, (PFNNOTIFY)gimis_StartApplet, (uint32*) pMe);
		}
		else 
		{
			if (pMe->m_WaitForQChat_Num < 2)
			{
				ISHELL_SetTimer(pMe->a.m_pIShell, 5*1000, (PFNNOTIFY)newgimis_WaitForQChat, (uint32*) pMe);
			}
		}
	}

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"newgimis_WaitForQChat, bRtn=%d,pRichptt addr=%d",bRtn,pMe->pRichptt);
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"newgimis_WaitForQChat, pMe->m_WaitForQChat_Num = %d",pMe->m_WaitForQChat_Num);
#endif

	if (STRLEN(pMe->szMobileID_Configer) < 1)
	{
		//��һ��ʹ�������ļ�û��imei������������Ȩ��
		if (STRLEN(pMe->MobileNoDJ) > 10 ){
			
			MEMSET(pMe->m_SQ1,0x00,250);
			SPRINTF(pMe->m_SQ1,"http://gps.richtalk.cn/brewGPS/BrewServlet?act=autdec&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->MobileNoDJ,pMe->szMobileID);
			MEMSET(pMe->m_SQ2,0x00,250);
			SPRINTF(pMe->m_SQ2,"http://zc.gota.richmap.cn/brewGPS/BrewServlet?act=autdec&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->MobileNoDJ,pMe->szMobileID);

		}
		else if (STRLEN(pMe->MyMobileNo) > 10 ){

			MEMSET(pMe->m_SQ1,0x00,250);
			SPRINTF(pMe->m_SQ1,"http://gps.richtalk.cn/brewGPS/BrewServlet?act=autdec&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->MyMobileNo,pMe->szMobileID);
			MEMSET(pMe->m_SQ2,0x00,250);
			SPRINTF(pMe->m_SQ2,"http://zc.gota.richmap.cn/brewGPS/BrewServlet?act=autdec&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->MyMobileNo,pMe->szMobileID);

		}else{

			MEMSET(pMe->m_SQ1,0x00,250);
			SPRINTF(pMe->m_SQ1,"http://gps.richtalk.cn/brewGPS/BrewServlet?act=autdec&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->szMobileID);
			MEMSET(pMe->m_SQ2,0x00,250);
			SPRINTF(pMe->m_SQ2,"http://zc.gota.richmap.cn/brewGPS/BrewServlet?act=autdec&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->szMobileID);

		}
	}
	else
	{
		//���ڱ���imei�������ļ�imei �Ƚ�	
		if (STRCMP(pMe->szMobileID,pMe->szMobileID_Configer) == 0)	//����������
		{
			if (STRLEN(pMe->MobileNoDJ) > 10){
					
				MEMSET(pMe->m_SQ1,0x00,250);
				SPRINTF(pMe->m_SQ1,"http://gps.richtalk.cn/brewGPS/BrewServlet?act=autdec&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->MobileNoDJ,pMe->szMobileID);
				MEMSET(pMe->m_SQ2,0x00,250);
				SPRINTF(pMe->m_SQ2,"http://zc.gota.richmap.cn/brewGPS/BrewServlet?act=autdec&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->MobileNoDJ,pMe->szMobileID);

			}else if (STRLEN(pMe->MyMobileNo) > 10 ){
				
				MEMSET(pMe->m_SQ1,0x00,250);
				SPRINTF(pMe->m_SQ1,"http://gps.richtalk.cn/brewGPS/BrewServlet?act=autdec&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->MyMobileNo,pMe->szMobileID);
				MEMSET(pMe->m_SQ2,0x00,250);
				SPRINTF(pMe->m_SQ2,"http://zc.gota.richmap.cn/brewGPS/BrewServlet?act=autdec&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->MyMobileNo,pMe->szMobileID);

			}else{

				MEMSET(pMe->m_SQ1,0x00,250);
				SPRINTF(pMe->m_SQ1,"http://gps.richtalk.cn/brewGPS/BrewServlet?act=autdec&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->szMobileID);
				MEMSET(pMe->m_SQ2,0x00,250);
				SPRINTF(pMe->m_SQ2,"http://zc.gota.richmap.cn/brewGPS/BrewServlet?act=autdec&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->szMobileID);

			}
		}
		else	//����ͬimei���ñ���api��ȡ��imei��Ȩ���������������ļ����ֻ�������Ȩ
		{
			if (STRLEN(pMe->MobileNoDJ) > 10){
				
				MEMSET(pMe->m_SQ1,0x00,250);
				SPRINTF(pMe->m_SQ1,"http://gps.richtalk.cn/brewGPS/BrewServlet?act=autdec&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->MobileNoDJ,pMe->szMobileID);
				MEMSET(pMe->m_SQ2,0x00,250);
				SPRINTF(pMe->m_SQ2,"http://zc.gota.richmap.cn/brewGPS/BrewServlet?act=autdec&tel=%s&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->MobileNoDJ,pMe->szMobileID);

			}else{
				
				MEMSET(pMe->m_SQ1,0x00,250);
				SPRINTF(pMe->m_SQ1,"http://gps.richtalk.cn/brewGPS/BrewServlet?act=autdec&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->szMobileID);
				MEMSET(pMe->m_SQ2,0x00,250);
				SPRINTF(pMe->m_SQ2,"http://zc.gota.richmap.cn/brewGPS/BrewServlet?act=autdec&imsi=%s&ver=3.1.2au&pp=FU/ctvH",pMe->szMobileID);

			}
		}
	}

	pMe->m_WaitForQChat_Num++;
	if (pMe->m_WaitForQChat_Num == 1)
	{
		pMe->m_SQ_C = 1; //��ʹ������ַ
		myiweb_Start(pMe,pMe->m_SQ1,0);
		ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),pMe->m_SQ1);
		uf_RecLog(pMe, pMe->m_SQ1 );
	}


	return;
}
///////////////        END        //////////////////////////////






// ���� �������йص�  ////////////
/*===========================================================================
===========================================================================*/
void *testgps_GetScreenData_task( newgimis *pMe )
{
	return pMe->pScreen_Task;
}

/*===========================================================================
===========================================================================*/
void *testgps_SetScreenData_task( newgimis *pMe, void *screenData )
{
	void *oldScreenData = pMe->pScreen_Task;
	pMe->pScreen_Task = screenData;
	return oldScreenData;
}




//////////////   ���ϱ����� GPS�����йغ� ��λ�쳣�ϱ��йص�  ///////////////
static void myiweb_Stop_E(newgimis *pwa)
{
   // this cancels any pending web transaction, or readable on the 
   // response body.  if nothing is pending, this has no effect
   CALLBACK_Cancel(&pwa->cb_E);
   
   // then clean up, if necessary
   if ((char *)0 != pwa->pszPostData_E) {

	   FREE(pwa->pszPostData_E);
		pwa->pszPostData_E = NULL;
//		pwa->iFileRead = 0;

//      *pwa->pszPostData_E = 1; // write delimiter back in, if any
 //     pwa->pszPostData_E = 0;
   }
 // ����  
   if (pwa->pipPostData_E != NULL)
	{
		IPEEK_Release(pwa->pipPostData_E);
		// Add by zjie 2011-01-30
		pwa->pipPostData_E = NULL;

	}
 // ����  
	if (pwa->pSource_E != NULL) {
		uint32 nErr = ISOURCE_Release(pwa->pSource_E);	// 2013-04-12 �� nErr
		//ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"myiweb_Stop_E ISOURCE_Release nErr = %d",nErr); 
		pwa->pSource_E = NULL;
	}

   
	if (pwa->piWResp_E)
	{
		IWEBRESP_Release(pwa->piWResp_E);
		pwa->piWResp_E= NULL;
	}

	if (pwa->m_pIWeb_E) 
	{
		int i,n;			// 2.6.4D2
		n = 0;
		i=IWEB_Release(pwa->m_pIWeb_E);
		while (i>0) {
			n++;
			i=IWEB_Release(pwa->m_pIWeb_E);
		}

		pwa->m_pIWeb_E = NULL;
	}

	if (pwa->m_BodyBuffer_E)
	{
		FREE(pwa->m_BodyBuffer_E);
		pwa->m_BodyBuffer_E = NULL;
	}
	pwa->m_BodySize_E = 0;
	pwa->m_BodyAllocSize_E = 0;

}




static void myiweb_Start_E(newgimis *pMe, char *pszUrl,int iPage)
{
   ISourceUtil *pisu;
   int    i = 0;
   WebOpt awo[10];

   //����ӿڱ��´���
   
	pMe->m_iAppStatus_E = 1;
   // look to see if there's POST data, this is totally non-standard, but 
   // easy to put into tests
   pMe->pszPostData_E = STRCHR(pszUrl, 1);  // �˺��������ַ����в����ַ� ���������뵥�ֽ��ַ� 
   
   // if there's post data, construct a streamD for IWeb to consume
   if ((char *)0 != pMe->pszPostData_E) 
   {
      *pMe->pszPostData_E = 0;
      if (SUCCESS == ISHELL_CreateInstance(pMe->a.m_pIShell,AEECLSID_SOURCEUTIL, (void **)&pisu)) 
	  {
         
         ISOURCEUTIL_PeekFromMemory(pisu, pMe->pszPostData_E + 1, 
                                    STRLEN(pMe->pszPostData_E + 1), 0, 0,
                                    &pMe->pipPostData_E);
         ISOURCEUTIL_Release(pisu);
      }
   }

   // initialize the callback, where I'll be called when the request
   // completes
   if(iPage == 0)
   {
	   CALLBACK_Cancel(&pMe->cb_E);
	   CALLBACK_Init(&pMe->cb_E, myiweb_GotResp_E, pMe);
   }
   pMe->uStart_E = GETUPTIMEMS();
   // start transaction, pass callbacks for web status, web headers
   // the extra WEBOPT_HEADER is used to help test what's sent 
   // (snoop.sh above shows all the headers)
   if(!pMe->m_pIWeb_E)
   {
	   ISHELL_CreateInstance(pMe->a.m_pIShell, AEECLSID_WEB, (void**)(&pMe->m_pIWeb_E));
	   // set the IWeb connect timeout to 10 seconds.  this also sets the 
		// failover timeout, if unset, or set to 0, IWeb uses the system 
		// default (30 seconds unless an OEM changes it)

	   
		awo[i].nId  = WEBOPT_CONNECTTIMEOUT;
		awo[i].pVal = (void *)20000;		// 2013-03-27 10000
		i++;
		
		//	2.6.4D1	awo[i].nId  = WEBOPT_FLAGS;
		//		awo[i].pVal = (void *)WEBREQUEST_NOKEEPALIVE;		// WEBREQUEST_NOWAITCONN;   Change  2013-04-26
		//		i++;
		
		//* ������в��ô��� zjie 
		// turn off HTTP over HTTP proxying
		awo[i].nId  = WEBOPT_PROXYSPEC;
		awo[i].pVal = (void *)"http:///http://10.0.0.200:80";
		i++;
		
		// Marks the end of the array of WebOpts
		awo[i].nId  = WEBOPT_END;

  
		
		
		// Add Options
		IWEB_AddOpt(pMe->m_pIWeb_E,awo);
   }
   if ((IPeek *)0 != pMe->pipPostData_E) {
      IWEB_GetResponse(pMe->m_pIWeb_E,
                       (pMe->m_pIWeb_E, &pMe->piWResp_E, &pMe->cb_E, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe,
                        WEBOPT_HEADER, "X-Method: POST\r\n", /* for kicks */
                        WEBOPT_HEADERHANDLER, myiweb_Header_E, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_E, 
                        WEBOPT_METHOD, "POST",
                        WEBOPT_BODY, pMe->pipPostData_E,
                        WEBOPT_CONTENTLENGTH, STRLEN(pMe->pszPostData_E + 1),
                        WEBOPT_END));
   } else {
      IWEB_GetResponse(pMe->m_pIWeb_E,
                       (pMe->m_pIWeb_E, &pMe->piWResp_E, &pMe->cb_E, pszUrl, 
                        WEBOPT_HANDLERDATA, pMe, 
                        WEBOPT_HEADER, "X-Method: GET \r\n",
						WEBOPT_METHOD, "GET",
                        WEBOPT_HEADERHANDLER, myiweb_Header_E, 
                        WEBOPT_STATUSHANDLER, myiweb_Status_E, 
                        WEBOPT_END));
   }
}






static void myiweb_GotResp_E(void *p)
{
	newgimis *pwa = (newgimis *)p;
	char lStr_msg[100];
	WebRespInfo *pwri;

	uf_RecLog( pwa , "into myiweb_GotResp_E");
//	MEMSET(pwa->copy_sUrl_E, 0x00, sizeof(pwa->copy_sUrl_E));
	
	pwri = IWEBRESP_GetInfo(pwa->piWResp_E);
	
	//   ND_Print(pMe, "** got response...\n** info code: %d\n", pwri->nCode);

	pwa->iPostPhTm_E = 0;
	ISHELL_SetTimer(pwa->a.m_pIShell, 30 * 1000, (PFNNOTIFY)uf_MonitorWait_E, pwa);

	pwa->m_T_E_C = 0; // ����ͳ���������� ��ȡʧ�ܼ��� ���������������ͣ��ȡ����ͣʱ��Ϊ200�룬Ȼ�������ȡ ����������������ϴ����������


	if (!WEB_ERROR_SUCCEEDED(pwri->nCode)) 
	{
		int nErr = 0;
		nErr = pwri->nCode;
		SPRINTF(lStr_msg, " myiweb_GotResp_E no response, code: %d",nErr);
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),lStr_msg); 

		// ����û�з��ͳɹ�
		if (nErr <0) 	nErr = (-1 * nErr);


		//////////////////////////GPS�������ݰ�////////////////
		if (pwa->mux_7Day == 0)
		{
			write_GPSData_7Day(pwa, pwa->sUrl);
		}
// 		//ԭ��������
// 		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"write File_Write_OfflineData_Gps"); 
// 		File_Write_OfflineData_Gps(pwa, pwa->sUrl);
// 		pwa->m_offlineData_gps = 1;
// 		//end
		
		pwa->m_Error = nErr;
//		if (pwa->m_WitchPag == 10)
		if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
		{		
	//		ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_PRINT,0, 0);
			ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
		}
		
		if (nErr == WEB_ERROR_CONNECT) {	// 1289
			char *sc_buf = NULL;
			//  ��������  gimis�˳�һ���Ӻ����� һ��ͨѶ��ָ�����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT  xin tiao");
//			IPOSDET_Release_fun(pwa);

			ud_RecLog_c(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"WEB_ERROR_CONNECT");
			if (pwa->cWatchDog != '0')
			{
				if (pwa->m_W_E++ > 3) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
				{

					uf_RecLog(pwa,"WatchDog Reboot");
					IPOSDET_Release_fun(pwa);
					STRCPY(sc_buf,"123"); //�������
				}
			}
			else
			{
				if (pwa->m_E_SC > 0)
				{
					if (pwa->m_W_E_P++ > pwa->m_E_SC) // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
					{

						uf_RecLog(pwa,"Play error Sound");
						SoundPlay_Start( pwa );
						pwa->m_W_E_P = 0;
					}
				}
			}
		}	// 1289
		else {
			// �������ֻ�����ԭ�� ����Ƿ�ѵ����
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"no response, nErr=%d",nErr);
			if (nErr == 554)
			{
				ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"no response, m_F_QF=%d",pwa->m_F_QF);
				pwa->m_F_QF = TRUE;  // ˵���ֻ�Ƿ��
				pwa->m_PosMode = TRUE; //ֻ����������
			}
		}
		// Add End

		myiweb_Stop_E(pwa);
		// ��HTTP���� 
		// // m_MUTEX; // 1 = ˵�����ܽ��з���HTTP �Ѿ���ռ����http����Դ  0= ˵������ʹ��http��Դ
//		pwa->m_MUTEX = 0;

		////////////////////////////GPS�������ݰ�//////////////////////////////////////////////
//		pwa->m_Mutex_offgps = 0; //����2015-12-18�޸����///
//		My_Log(pwa->a.m_pIShell,'1', &(pwa->nInd),"cb_jie suo *NO 2... m_Mutex_offgps = %d" ,pwa->m_Mutex_offgps);
		/////////////////////////////////////////////////////////////////////
		return ;
	}	

	pwa->m_Error = pwri->nCode;
	if (pwa->m_Error < 0)
	{
		pwa->m_Error = (-1 * pwa->m_Error);
	}
//	if (pwa->m_WitchPag == 10)
	if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
	{		
		ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
	}

	if ((ISource *)0 != pwri->pisMessage) 
	{
		pwa->m_W_E = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ���������ֻ�)
		pwa->m_W_E_P = 0; // ����ͳ��1289�������ֵĴ���  ������ 3 ��ʱ�򲥷�¼��
		
		pwa->m_PosMode = FALSE; //�������Ϊ ������Q3+���ֻ��ϻ�ͻȻ��ɴ�����ģʽ
		
		//  add 2015-11-4  start  //
		pwa->m_F_QF = FALSE;  // ˵���ֻ�Ƿ��
		pwa->m_PosMode = FALSE; //ֻ����������

		pwa->pSource_E = pwri->pisMessage;
		ISOURCE_AddRef(pwa->pSource_E);
		
		IWEBRESP_Release(pwa->piWResp_E);
		pwa->piWResp_E = 0;
		
		CALLBACK_Cancel(&pwa->cb_E);
		CALLBACK_Init(&pwa->cb_E, myiweb_E_ReadTaskBody, pwa);
		myiweb_E_ReadTaskBody(pwa);
	}	
}

static void myiweb_E_ReadTaskBody(newgimis *pwa)
{
	char *sBuf = NULL;
	char *sBuf1 = NULL;
	char *sBuf_T = NULL;
	
	char *sBuf_d = NULL;
	int nRead = 0;
	int si_Ret = 0;  //����ֵ
	char sc_Count[2];

	ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"coming myiweb_E_ReadTaskBody");
	
	sBuf = (char *)MALLOC(1024);
	MEMSET(sBuf,0x00,1024);

	while (!sBuf)
	{
		ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"myiweb_E_ReadTaskBody sBuf malloc fail ");
		sBuf = (char *)MALLOC(1024);
		MEMSET(sBuf,0x00,1024);
	}
	
	sBuf_d = (char *)MALLOC(102400);
	MEMSET(sBuf_d,0x00,102400);

	while (!sBuf_d)
	{
		ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"myiweb_E_ReadTaskBody sBuf_d malloc fail ");
		sBuf_d = (char *)MALLOC(102400);
		MEMSET(sBuf_d,0x00,102400);
	}
	
	nRead = ISOURCE_Read(pwa->pSource_E, sBuf, 1023);
	if(ISOURCE_WAIT == nRead) 
	{
		ISOURCE_Readable(pwa->pSource_E, &pwa->cb_E);
		FREE(sBuf);
		sBuf = NULL;
		FREE(sBuf_d);
		sBuf_d = NULL;
		
		if (pwa->m_F_R_E == 0)
		{
			FREE(pwa->m_HTTP_E);
			pwa->m_HTTP_E = NULL;
			pwa->m_F_R_E = 0;
		}
		return;
	}
	
	if (nRead > 0)		
	{
		DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   successful");
		DBGPRINTF(sBuf);
		//		IHTMLVIEWER_LoadSource(pwa->m_pHTMLViewer, pwri->pisMessage);
		SPRINTF(sc_Count,"%d",nRead);
		DBGPRINTF(sc_Count);
		//			ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"xin tiao kai shi"); 
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," nRead  E = %d",nRead); 
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf); 
		
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," nRead = %d",nRead); 
		
		if (nRead > 1)
		{
			sBuf1 = (char *)MALLOC(1024);
			sBuf_T = (char *)MALLOC(102400);
			
			while (!sBuf_T)
			{
				ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," sBuf_T E Failure to apply for space 1"); 
				sBuf_T = (char *)MALLOC(102400);
			}
			MEMSET(sBuf_T,0x00,102400);
			STRCPY(sBuf_T,sBuf);
		}
		else
		{
			sBuf_T = (char *)MALLOC(1024);
			MEMSET(sBuf_T,0x00,1024);
			STRCPY(sBuf_T,sBuf);
		}
		
		while (nRead > 1)
		{
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"ISOURCE_Read"); 
			MEMSET(sBuf1,0x00,1024);
			nRead = ISOURCE_Read(pwa->pSource_E, sBuf1, 1023);
			STRCAT(sBuf_T,sBuf1);

			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"while nRead  = %d",nRead); 
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),sBuf1); 
		}
		
		if(ISOURCE_WAIT == nRead) 
		{
			uf_RecLog(pwa," ISOURCE_Readable ");
			
			if (pwa->m_F_R_E == 1) // ˵��һ�������������ε� ISOURCE_WAIT �ⲻ�ǵ�һ�γ��� 
			{
				STRCAT(pwa->m_HTTP_E,sBuf_T);
			}
			else
			{
				pwa->m_HTTP_E = (char *) MALLOC(102400);
				while (!pwa->m_HTTP_E)
				{
					ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," m_HTTP_E malloc fail"); 
					pwa->m_HTTP_E = (char *)MALLOC(102400);
				}
				MEMSET(pwa->m_HTTP_E,0x00,102400);
				STRCPY(pwa->m_HTTP_E,sBuf_T);
			}
			
			pwa->m_F_R_E = 1; //

			ISOURCE_Readable(pwa->pSource_E, &pwa->cb_E);
			
			FREEIF(sBuf_T);
			sBuf_T = NULL ;
			
			FREEIF(sBuf1);
			sBuf1 = NULL ;
			
			FREE(sBuf);
			sBuf = NULL;
			FREE(sBuf_d);
			sBuf_d = NULL;
			return;
		}
		
		if (pwa->m_F_R_E == 1 && pwa->m_HTTP_E)
		{
			STRCAT(pwa->m_HTTP_E,sBuf_T);
			uf_RecLog(pwa,"wait time STRCAT HTTP CONTENT");
			uf_RecLog(pwa,"wait time STRCAT HTTP CONTENT");
			MEMSET(sBuf_T,0x00,102400);
			STRCPY(sBuf_T,pwa->m_HTTP_E);
			uf_RecLog(pwa,"wait time 1 STRCAT HTTP CONTENT");
			uf_RecLog(pwa,"wait time 1 STRCAT HTTP CONTENT");
			pwa->m_F_R_E = 0; //
			FREE(pwa->m_HTTP_E);
			pwa->m_HTTP_E = NULL;
		}
		
		ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd)," sBuf_T 1 = %d ",STRLEN(sBuf_T)); 
		uf_RecLog(pwa,sBuf_T);
		
		uf_fromESC(sBuf_T,sBuf_d);

		si_Ret = ParseStringandWriteToConfigure_xintiao_GPS(pwa,sBuf_d);

		FREEIF(sBuf_T);
		sBuf_T = NULL ;
		FREEIF(sBuf1);
		sBuf1 = NULL ;

//		uf_RecLog(pwa,"22222222222222222222222222222");
		
		// ����ֵ 5 -- ����Ȩ��δ��Ȩ ����ر�
		switch (si_Ret)  //˵���ɹ�
		{
		case 4:
			{
				// ˵������ʧ��
			}
			break;	
		case 1: // 
			{
				pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
				gimis_SetGPSPara(pwa); 
			}
			break;
		case 2: // 
			{
			}
			break;	
		case 5:
			{
				pwa->mInt_StartRun = 30;  //˵�����Ը��������ļ�
				gimis_SetGPSPara(pwa); 	
				// �رճ���
				pwa->m_RelyExit = TRUE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
				
				ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY) testgps_CloseApplet_WSQ, (uint32*) pwa); //�˳�����
				break;
			}
		}
		
		FREE(sBuf);
		sBuf = NULL;
		
		FREE(sBuf_d);
		sBuf_d = NULL;

		}
		
		myiweb_Stop_E(pwa);
		pwa->m_MUTEX = 0;
}



static void myiweb_Header_E(void *p, const char *cpszName, GetLine *pglVal)
{
   newgimis *pwa = (newgimis *)p;
  
  
 //  if (pMe->m_bRS) { // If response is to be displayed
      if ((char *)0 != cpszName) {
        // ND_Print(pMe, "%s:", cpszName);
      }
      //ND_Print(pMe, "%s\n", pglVal->psz);
  // }
}



static void myiweb_Status_E(void *p, WebStatus ws, void *pVal)
{
   AECHAR     szText[32];
   newgimis * pMe = (newgimis *)p;
   
   (void)pVal;

   switch (ws) {
   case WEBS_CANCELLED:
	  ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CANCELLED, szText, sizeof(szText));
      break;
   case WEBS_GETHOSTBYNAME:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GETHOSTBYNAME,szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   2");
      break;          
   case WEBS_CONNECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CONNECT, szText, sizeof(szText));
      break;
   case WEBS_SENDREQUEST:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_SENDREQUEST, szText, sizeof(szText));
	  pMe->iPostPhTm_E = GETTIMEMS();
		ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost_E, pMe);
      break;
   case WEBS_READRESPONSE:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_READRESPONSE, szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   3");
      break;
   case WEBS_GOTREDIRECT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_GOTREDIRECT, szText, sizeof(szText));
      break;
   case WEBS_CACHEHIT:
      ISHELL_LoadResString(pMe->a.m_pIShell, NEWGIMIS_RES_FILE, IDS_WEBS_CACHEHIT, szText, sizeof(szText));
	  DBGPRINTF("aaaaaaaaaaaaaaaaaaaaaaaaaaa   4");
      break;

	default:
		if (pMe->iPostPhTm_E == 0) {			//  Add 2013-09-23
		   pMe->iPostPhTm_E = GETTIMEMS();
		   ISHELL_SetTimer(pMe->a.m_pIShell, 60 * 1000, (PFNNOTIFY)uf_MonitorPost_E, pMe);
		}		// Add End
		break;
   }

   // show that status!
   if ((AECHAR *)0 != szText) 
   {
  //    myiweb_DisplayInfo(pMe,szText);
	  DBGPRINTF("bbbbbbbbbbbbbbbbbbbbbbbbbbb");
   }

}



/********************************************************************************************
	���http�Ƿ�ʱ
**********************************************************************************************/
void uf_MonitorPost_E(newgimis *pMe)                        // (WebAction * pwa)		// b61 2012-02-09		
{

//	AECHAR szText[6];
    AECHAR szFailure[] = {0x4E2D,0x5FC3,0x65E0,0x5E94,0x7B54,0x0000};	// ������Ӧ��

	
	if ( pMe->iPostPhTm_E == 0)
		return;

//	pMe->m_Mutex_offgps = 0;
	 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"uf_MonitorPost in uf_MonitorPost_E, http time out");

	 if (pMe->mux_7Day == 0)
	 {
		 write_GPSData_7Day(pMe, pMe->sUrl);
	 }

//	 uf_AfterSend(pMe,"0");				// 2013-10-01
	 myiweb_Stop_E(pMe);

}





void newgimis_NotifyShellKey_33( newgimis *pMe )  // ���ճ��������� �ֻ���ʶ��Ϊ 33 �� ��ֻ�ᴥ��������Ϣ 257 256 258 �������ֻ���һֱ����256
{
// 	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"jinlai");
// 	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"==m_PressSts = %d  , iGetkey = %d",pMe->m_PressSts,pMe->iGetkey);
	if(pMe->iGetkey > 0 && pMe->m_PressSts == 1)	
	{
		pMe->iGetkey = 0;
//		pMe->wParam = pNKeyEvn->wParam;			// �����ֻ�����

		pMe->m_PressSts = 0;// ˵���Ѿ�������

		if (pMe->wParam == AVK_3)  // һ���򿨣�����ת����
		{
			if (pMe->m_FOBFlag == 1)
			{
				pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
				pMe->m_Witch_Key = 11; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
				ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
			}
		}
		
		if (pMe->wParam == AVK_5)  // ������Ƭ
		{
			if (pMe->m_FOBFlag == 1)
			{
				pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
				pMe->m_Witch_Key = 5; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
				ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
			}
		}

		if (pMe->wParam == AVK_8) // ������Ƶ
		{
			if (pMe->m_FOBFlag == 1)
			{
				pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
				pMe->m_Witch_Key = 8; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
				ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
			}
		}

		if (pMe->wParam == AVK_2) // һ������
		{
			if (pMe->m_FOBFlag == 1)
			{
				pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
				pMe->m_Witch_Key = 2; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ   
				ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
			}
		}
	
		if (pMe->wParam == AVK_6) // �ճ�״̬0
		{
			if (pMe->m_FOBFlag == 1)
			{
				switch(pMe->cPicMode)
				{
				case '0':  //�ճ�  
					{
						pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
						pMe->m_Witch_Key = 4; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
						ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
					}
					break;
				case '1': // ����״̬0
					{
						pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
						pMe->m_Witch_Key = 10; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
						ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
					}
					break;
				}
			}
		}

		if (pMe->wParam == AVK_9) //�س� ״̬ 1
		{
			if (pMe->m_FOBFlag == 1)
			{
				switch(pMe->cPicMode)
				{
				case '0':    //�س�   
					{
						pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
						pMe->m_Witch_Key = 3; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
						ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
					}
					break;
				case '1': // ����״̬ 1
					{
						pMe->m_Flag_Key = 1;//������ǳ�������������  0 -- ���ǳ�����    1 -- ����������
						pMe->m_Witch_Key = 9; // ��������Ǵ���ʲô���ܵ�  2 -- һ������  5 -- ������Ƭ  8 -- ������Ƶ  
						ISHELL_StartApplet(pMe->a.m_pIShell, pMe->a.clsID);
					}
					break;
				}
			}
		}
	}
	return ;
}


boolean InputMode33(newgimis *pMe, ITextCtl	*textCtl,AEEEvent eCode, uint16 wParam, uint32 dwParam )
{
	cTaskApp *me = NULL;
	if(pMe->pTaskApp)	
		me = pMe->pTaskApp;
	else
	{
		//uf_RecLog(pMe,"ERR: InputMode33");
		return FALSE;
	}
	
	if (STRCMP(pMe->sMobileType,"33") != 0) //X3���뷨
	{
		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "ERR: InputMode33");
		return FALSE;
	}
	if (1 == pMe->m_Task_fh)
	{
		if (  wParam == AVK_END || wParam == AVK_LEFT || wParam == AVK_RIGHT /*||wParam == AVK_UP || wParam == AVK_DOWN*/) // AVK_SOFT1 :57398   AVK_SEND 57391 ͨ����
		{
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "��===================================");
			return TRUE;
		}
		//����* ��ʧȥ����
//		if (wParam == AVK_END ||wParam == AVK_CLR ||wParam == AVK_UP || wParam == AVK_DOWN || wParam == AVK_LEFT
//				|| wParam == AVK_RIGHT || (wParam >= AVK_1 && wParam <= AVK_9 && (eCode != EVT_KEY_HOOK_RELEASE && eCode != EVT_KEY_RELEASE) ))
		if ((wParam == AVK_END || wParam == AVK_CLR ||wParam == AVK_UP || wParam == AVK_DOWN || wParam == AVK_LEFT
					|| wParam == AVK_RIGHT || (wParam >= AVK_1 && wParam <= AVK_9))		 && (eCode != EVT_KEY_HOOK_RELEASE && eCode != EVT_KEY_RELEASE))
		{
			//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "m_Task_fh : wParam =  %d, eCode = %d ",wParam, eCode);

			if (ITEXTCTL_IsActive( textCtl))
			{
				ITEXTCTL_SetCursorPos(textCtl, TC_CURSOREND);//����������ı���ĩβ(���»�ȡ���� ȫѡ�ֱ�ɾ����)
			}
			else
			{
				ITEXTCTL_SetActive( textCtl, TRUE);
				ITEXTCTL_SetCursorPos(textCtl, TC_CURSOREND);
			}
		}	
	}

	
	if((textCtl!=NULL) &&  (ITEXTCTL_IsActive( textCtl)))
	{	
		//ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "wParam =  %d, eCode = %d ",wParam, eCode);

								if (wParam == AVK_POUND)  //#
								{
									if(EVT_KEY_PRESS == eCode || EVT_KEY_HOOK_PRESS == eCode)
									{
										AEERect re;
										//�����Ǽ����� EVT_KEY_HOOK_PRESS ����������������������������������������������������������������
										//AEETextInputMode   ImeNext;
										AEETextInputModeInfo CurIme;										
																	
										ITEXTCTL_GetInputMode(textCtl,&CurIme);
										//ImeNext = ITEXTCTL_GetInputMode(textCtl,&CurIme);									
										//uf_ShowInputMode(pMe,CurIme.tmMode);

										if (AEE_TM_PINYIN == CurIme.tmMode)
										{
											ITEXTCTL_SetInputMode(textCtl, AEE_TM_NUMBERS);
										}else if(AEE_TM_NUMBERS == CurIme.tmMode)
										{
											ITEXTCTL_SetInputMode(textCtl, AEE_TM_LETTERS); //AEE_TM_LETTERS
										}else if(AEE_TM_LETTERS == CurIme.tmMode)
										{
											ITEXTCTL_SetInputMode(textCtl, AEE_TM_STROKE); //AEE_TM_LETTERS AEE_TM_STROKE
										}else{
											ITEXTCTL_SetInputMode(textCtl, AEE_TM_PINYIN);
										}
										
										//����ɵ�����
										//SETAEERECT( &re, nCol * di.cxScreen / MAXCOLUMNS, nLine * LINEHEIGHT, pMe->DeviceInfo.cxScreen, LINEHEIGHT+2);
										if (me->pTaskRecB->bFlow)
										{
											SETAEERECT( &re, 1 * pMe->DeviceInfo.cxScreen / MAXCOLUMNS, 8 * LINEHEIGHT, pMe->DeviceInfo.cxScreen, LINEHEIGHT+4);
										}
										else
										{
											SETAEERECT( &re, 1 * pMe->DeviceInfo.cxScreen / MAXCOLUMNS, 8 * LINEHEIGHT, pMe->DeviceInfo.cxScreen, LINEHEIGHT+20);
										}
										
										IDISPLAY_EraseRect(pMe->a.m_pIDisplay, &re);
										//x3��Ƭ���뷨		
										{
											//AEETextInputModeInfo CurIme;											
											ITEXTCTL_GetInputMode( textCtl,&CurIme);									
											yb_ShowInputMode(pMe,CurIme.tmMode,2);
										}

									}
									return TRUE; //��Ҫ��handle����
								}  
								if ( wParam == AVK_STAR && (eCode != EVT_KEY_HOOK_RELEASE && eCode != EVT_KEY_RELEASE) )  //*
								{
									pMe->m_Task_fh = 1;
									ITEXTCTL_SetInputMode(textCtl, AEE_TM_SYMBOLS);							
								}
								
								if ( wParam == AVK_LEFT || wParam == AVK_RIGHT || wParam == AVK_SELECT)  //*
								{
									if (1 == pMe->m_Task_fh)
									{
										return TRUE;
									}
								}  

								if ( wParam == AVK_SELECT) 
								{			
									if (1 == pMe->m_Task_fh)
									{
										return TRUE;
									}
								} 

								if ( wParam == AVK_SEND || wParam == AVK_SOFT1 || wParam == AVK_END) // AVK_SOFT1 :57398   AVK_SEND 57391 ͨ����
								{
									return TRUE;
								}

		if (ITEXTCTL_HandleEvent(textCtl, eCode, wParam, dwParam ))
		{
			if ( wParam == AVK_CLR)  //*
			{
				IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );

				if (1 == pMe->m_Task_fh)
				{					
					pMe->m_Task_fh = 0;
					
					Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
					// ��������
					//Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
					// ����ǰ״̬
					Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);
					Task_RedrawScreen(pMe);	

					if (pMenuCtl)
					{
						IMENUCTL_Redraw( pMenuCtl );
					}
					//x3��Ƭ���뷨		
					{
						AEETextInputModeInfo CurIme;											
						ITEXTCTL_GetInputMode( textCtl,&CurIme);									
						yb_ShowInputMode(pMe,CurIme.tmMode,2);
					}
					return TRUE;
				}

			//	return TRUE;					

			} 

			if (wParam >= AVK_1 && wParam <= AVK_9)
			{
				IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );

				if (1 == pMe->m_Task_fh)
				{		
					pMe->m_Task_fh = 0;

					Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
					// ��������
					//Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
					// ����ǰ״̬
					Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);
					Task_RedrawScreen(pMe);	

					if (pMenuCtl)
					{					
						IMENUCTL_Redraw( pMenuCtl );
					}
					//x3��Ƭ���뷨		
					{
						AEETextInputModeInfo CurIme;											
						ITEXTCTL_GetInputMode(textCtl,&CurIme);									
						yb_ShowInputMode(pMe,CurIme.tmMode,2);
					}
					return TRUE;
				}
			}

			if (wParam == AVK_POUND)  //#
			{
				//ƴ��ģʽ  # *�Ǽ���������
				return TRUE;
			}
			if ( wParam == AVK_STAR)  //*
			{
				pMe->m_Task_fh = 1;
				return TRUE;
			} 	

			if (wParam == AVK_END)
			{
				return TRUE;
			}

			//if (wParam == AVK_SELECT && 0 == pMe->m_Task_fh)
			if ((wParam == AVK_CLR || wParam == AVK_UP ||wParam == AVK_DOWN ||wParam == AVK_SELECT ||wParam == AVK_LEFT || wParam == AVK_RIGHT)&& 0 == pMe->m_Task_fh )
			{
				IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );
				Task_Print_asc( pMe, 1, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskName);
				// ��������
				//Task_Print_asc( pMe, 2, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskContent);
				// ����ǰ״̬
				Task_Print_asc( pMe, 3, 1, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s",me->pTaskRecB->pTaskStatus);
				Task_RedrawScreen(pMe);	
				
				if (pMenuCtl)
				{					
					IMENUCTL_Redraw( pMenuCtl );
				}
				//x3��Ƭ���뷨		
				{
					AEETextInputModeInfo CurIme;											
					ITEXTCTL_GetInputMode(textCtl,&CurIme);									
					yb_ShowInputMode(pMe,CurIme.tmMode,2);
				}
				return TRUE;
				
			}

#ifdef PHONE_QCHAT_DUIJIANG
		
if (pMe->m_F_DJ == 1)
{
// ˵�����ڶԽ�״̬
if (wParam == AVK_END)
{
	return FALSE;
}
}
#else   
return TRUE;
#endif

//				ITEXTCTL_GetText(textCtl,pMe->pText,256);
//				WSTRTOSTR(pMe->pText,s_buf,256);
//				ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), " Last save pMe->pText : s_buf=%s ",s_buf);
			return TRUE;
		}
	}		

}


//������λ
int newgimis_Init_UKBD(newgimis * pMe)
{
	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"1 pMe->pIgpsext  = %p",pMe->pIgpsext);

	if (pMe->pIgpsext != NULL)
	{
		return 1;
	}
	
	if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_OEMTTSEXT  ,
		(void **)&pMe->pIgpsext ) == SUCCESS )
	{
		uf_RecLog(pMe,"create newgimis_OpenGPS_UKBD OK");
		return 1;
	}
	else
	{
		uf_RecLog(pMe,"create newgimis_OpenGPS_UKBD fail");
	}
	return 0;
}



//���� / �ر�GPS��λ
void newgimis_OpenGPS_UKBD(newgimis * pMe)
{
	int si_ret = 0;
	uf_RecLog(pMe,"newgimis_OpenGPS_UKBD coming");
		
	if (pMe->pIgpsext == NULL)
	{
		uf_RecLog(pMe,"create newgimis_OpenGPS_UKBD fail 0");
		return;
	}

	pMe->GPSFlag = TRUE;

	
	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"2 pMe->pIgpsext  = %p",pMe->pIgpsext);
	uf_RecLog(pMe,"newgimis_OpenGPS_UKBD coming 1");	  
	si_ret = ITTSEXT_Bd_AutoPositioning(pMe->pIgpsext ,/*30000*/pMe->iInterval * 1000);
	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"si_ret = %d",si_ret);
	MEMSET(pMe->m_Con,0x00,30);
	if (si_ret == BD_MODULE_EXISTED)
	{
		pMe->m_gpsFlag = 11; //��ʾ�Ǳ����ֻ�������ʹ�ñ���ģʽ
	}
	if (si_ret == BD_OPENFAILED ) //��������ʧ��
	{
		uf_RecLog(pMe,"Open Beidou POS fail");
		STRCPY(pMe->m_Con,"Open Beidou fail");
	}
	else if (si_ret == BD_OPEN_FIRST  ) //�����״ο������Ѿ���ͨ��Դ����δ���й���λ
	{
		uf_RecLog(pMe,"Open Beidou POS success BD_OPEN_FIRST");
		STRCPY(pMe->m_Con,"Open Beidou first");
	}
	else if (si_ret == BD_HAVEOPENED) //�����Ѿ���������
	{
		uf_RecLog(pMe,"Open Beidou POS success BD_HAVEOPENED");
		STRCPY(pMe->m_Con,"Beidou have open");
	}
}


void newgimis_CloseGPS_UKBD(newgimis * pMe)
{
	// BD_CLOSED_SUCCESSED(int 0):�����رճɹ�
    // BD_CLOSED_FAILED(int 1):�����ر�ʧ��
	int i= 0;

	int si_ret = 0;

	pMe->GPSFlag = FALSE;

	pMe->m_Fix_C = 0;//��λ������0

	SPRINTF(pMe->show_LatLon,"0x100001   0x100001");
	
	MEMSET(pMe->m_Con,0x00,30);
	if (pMe->pIgpsext == NULL)
	{
		return;
	}
	MEMSET(pMe->m_Con,0x00,30);
	si_ret = ITTSEXT_BdStopPositioning(pMe->pIgpsext);
	if (si_ret == BD_CLOSED_SUCCESSED)
	{
		uf_RecLog(pMe,"Close Beidou POS success BD_CLOSED_SUCCESSED");
		STRCPY(pMe->m_Con,"Beidou close success");
	}
	else if (si_ret == BD_CLOSED_FAILED)
	{
		uf_RecLog(pMe,"Close Beidou POS fail BD_CLOSED_FAILED");
		STRCPY(pMe->m_Con,"Beidou close fail");
	}

    ITTSEXT_Release(pMe->pIgpsext);
	pMe->pIgpsext = NULL;
// 	while (i>0) {
// 		i=ITTSEXT_Release(pMe->pIgpsext);
// 		uf_RecLog(pMe,"ITTSEXT_Release success");
// 	}
}



boolean SendBDData( newgimis *pMe )
{
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��
	char sc_buf1[10];
	char sc_buf2[10];
	AECHAR wcText[MAXTEXTLEN];

	GETJULIANDATE(0,&dDate);		//	ȡ������

	pMe->m_GPSGroup++;
	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd)," pMe->m_sendGPSGroup = %d",pMe->m_GPSGroup); 

	if (pMe->m_GPSGroup > pMe->nArySize) // �������Ŀ���Ƿ�ֹ�Ӹ������ĳɵ�������ʱ�������
	{
		pMe->m_GPSGroup = pMe->nArySize;
	}

	if (pMe->cPosDetMode == 13)
	{
		pMe->m_PM_BD = 13;
	}
	else
	{
		pMe->m_PM_BD = 14;
	}

// yyyy-mm-dd hh:MM:ss    ��19���ֽ�
//   SPRINTF(sDateTime,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
	pMe->i_HttpMod = 2; //HTTP���շ�������ģʽ ����	1 - ������ջ�������������Ȩ�� 2 - ���ջ�������GPS�����ϴ�����������
	
	MEMSET(pMe->sUrl,0x00,250);

	if(pMe->m_GPSGroup == 1)
	{
		SPRINTF(pMe->sDateTime1,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		//γ��
		STRCPY(pMe->latStr1, (char *)pMe->latStr_BD);
		ud_Trim(pMe->latStr1);	
		 // ����  			
		STRCPY(pMe->lonStr1, (char *)pMe->lonStr_BD);
		ud_Trim(pMe->lonStr1);
		//ˮƽ�ٶ�
		pMe->dVelHor1 = pMe->dVelHor_BD;

		STRCPY(pMe->lx_lon,pMe->lonStr1);
		STRCPY(pMe->lx_lat,pMe->latStr1);

		//////////  ����ǵ�һ�����������ͱ���ȥ START ///////////
		if (pMe->m_Fix_C == 1)
		{

			SPRINTF(pMe->show_Time,"%02d:%02d:%02d",dDate.wHour,dDate.wMinute,dDate.wSecond);
			if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
			{
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s;LA%s;VV%d;EP0;KS0;PM%d;TT%s",pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,pMe->lonStr1,pMe->latStr1,pMe->dVelHor1,pMe->m_PM_BD,pMe->sDateTime1);
			}
			else
			{
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&imsi=%s&pp=LO%s;LA%s;VV%d;EP0;KS0;PM%d;TT%s",pMe->strCenterIp,pMe->szMobileID,pMe->lonStr1,pMe->latStr1,pMe->dVelHor1,pMe->m_PM_BD,pMe->sDateTime1);
			}

			MEMSET(sc_buf1,0x00,10);
			MEMSET(sc_buf2,0x00,10);
			STRNCPY(sc_buf1,pMe->lonStr1,8);
			STRNCPY(sc_buf2,pMe->latStr1,7);
			SPRINTF(pMe->show_LatLon,"%s   %s",sc_buf1,sc_buf2);

			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),pMe->sUrl); 

			pMe->m_MUTEX = 2;

			if ((pMe->m_F_QF == FALSE) && pMe->m_F_R_GPS != 1)
			{
				myiweb_Stop_gps(pMe);
				myiweb_Start_gps(pMe,pMe->sUrl,0);
				pMe->m_GPSGroup = 0;
			}
			else
			{
				if (pMe->mux_7Day == 0)
				{
					write_GPSData_7Day(pMe, pMe->sUrl);
				}
			}
			
			return TRUE;
		}
		if (pMe->nArySize  == 1)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}

	}
	else if(pMe->m_GPSGroup == 2)
	{
		SPRINTF(pMe->sDateTime2,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		//γ��
		STRCPY(pMe->latStr2, (char *)pMe->latStr_BD);
		ud_Trim(pMe->latStr2);	
		 // ����  			
		STRCPY(pMe->lonStr2, (char *)pMe->lonStr_BD);
		ud_Trim(pMe->lonStr2);
		//ˮƽ�ٶ�
		pMe->dVelHor2 = pMe->dVelHor_BD;

		
		uf_RecLog(pMe,"���� m_GPSGroup =2 2");
		STRCPY(pMe->lx_lon,pMe->lonStr2);
		STRCPY(pMe->lx_lat,pMe->latStr2);
		Send_PM(pMe,2);
		uf_RecLog(pMe,"���� m_GPSGroup =2 3");

		if (pMe->nArySize  == 2)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	
	}
	else if (pMe->m_GPSGroup == 3)
	{
		SPRINTF(pMe->sDateTime3,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		//γ��
		STRCPY(pMe->latStr3, (char *)pMe->latStr_BD);
		ud_Trim(pMe->latStr3);	
		 // ����  			
		STRCPY(pMe->lonStr3, (char *)pMe->lonStr_BD);
		ud_Trim(pMe->lonStr3);
		//ˮƽ�ٶ�
		pMe->dVelHor3 = pMe->dVelHor_BD;

		STRCPY(pMe->lx_lon,pMe->lonStr3);
		STRCPY(pMe->lx_lat,pMe->latStr3);

		if (pMe->nArySize  == 3)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	}

	else if(pMe->m_GPSGroup == 4)
	{
		SPRINTF(pMe->sDateTime4,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		//γ��
		STRCPY(pMe->latStr4, (char *)pMe->latStr_BD);
		ud_Trim(pMe->latStr4);	
		 // ����  			
		STRCPY(pMe->lonStr4, (char *)pMe->lonStr_BD);
		ud_Trim(pMe->lonStr4);
		//ˮƽ�ٶ�
		pMe->dVelHor4 = pMe->dVelHor_BD;

		STRCPY(pMe->lx_lon,pMe->lonStr4);
		STRCPY(pMe->lx_lat,pMe->latStr4);

		if (pMe->nArySize  == 4)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	
	}
	else if(pMe->m_GPSGroup == 5)
	{
		SPRINTF(pMe->sDateTime5,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		//γ��
		STRCPY(pMe->latStr5, (char *)pMe->latStr_BD);
		ud_Trim(pMe->latStr5);	
		 // ����  			
		STRCPY(pMe->lonStr5, (char *)pMe->lonStr_BD);
		ud_Trim(pMe->lonStr5);
		//ˮƽ�ٶ�
		pMe->dVelHor5 = pMe->dVelHor_BD;

		STRCPY(pMe->lx_lon,pMe->lonStr5);
		STRCPY(pMe->lx_lat,pMe->latStr5);

		if (pMe->nArySize  == 5)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	
	}
	else if(pMe->m_GPSGroup == 6)
	{
		SPRINTF(pMe->sDateTime6,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		//γ��
		STRCPY(pMe->latStr6, (char *)pMe->latStr_BD);
		ud_Trim(pMe->latStr6);	
		 // ����  			
		STRCPY(pMe->lonStr6, (char *)pMe->lonStr_BD);
		ud_Trim(pMe->lonStr6);
		//ˮƽ�ٶ�
		pMe->dVelHor6 = pMe->dVelHor_BD;

		STRCPY(pMe->lx_lon,pMe->lonStr6);
		STRCPY(pMe->lx_lat,pMe->latStr6);

		if (pMe->nArySize  == 6)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	
	}
	else if(pMe->m_GPSGroup == 7)
	{
		SPRINTF(pMe->sDateTime7,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		//γ��
		STRCPY(pMe->latStr7, (char *)pMe->latStr_BD);
		ud_Trim(pMe->latStr7);	
		 // ����  			
		STRCPY(pMe->lonStr7, (char *)pMe->lonStr_BD);
		ud_Trim(pMe->lonStr7);
		//ˮƽ�ٶ�
		pMe->dVelHor7 = pMe->dVelHor_BD;

		STRCPY(pMe->lx_lon,pMe->lonStr7);
		STRCPY(pMe->lx_lat,pMe->latStr7);

		if (pMe->nArySize  == 7)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	
	}
	else if(pMe->m_GPSGroup == 8)
	{
		SPRINTF(pMe->sDateTime8,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);
		//γ��
		STRCPY(pMe->latStr8, (char *)pMe->latStr_BD);
		ud_Trim(pMe->latStr8);	
		 // ����  			
		STRCPY(pMe->lonStr8, (char *)pMe->lonStr_BD);
		ud_Trim(pMe->lonStr8);
		//ˮƽ�ٶ�
		pMe->dVelHor8 = pMe->dVelHor_BD;

		STRCPY(pMe->lx_lon,pMe->lonStr8);
		STRCPY(pMe->lx_lat,pMe->latStr8);

		if (pMe->nArySize  == 8)
		{
			pMe->m_GPSGroup = 0;
		}
		else
		{
			return TRUE;
		}
	
	}

	MEMSET(sc_buf1,0x00,10);
	MEMSET(sc_buf2,0x00,10);
	STRNCPY(sc_buf1,pMe->lx_lon,8);
	STRNCPY(sc_buf2,pMe->lx_lat,7);
	SPRINTF(pMe->show_LatLon,"%s   %s",sc_buf1,sc_buf2);

	SPRINTF(pMe->show_Time,"%02d:%02d:%02d",dDate.wHour,dDate.wMinute,dDate.wSecond);

	if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
	{
		switch(pMe->nArySize)
		{
		case 1:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s;LA%s;VV%d;EP0;KS0;PM%d;TT%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,pMe->lonStr1,pMe->latStr1,pMe->dVelHor1,pMe->m_PM_BD,pMe->sDateTime1);
			break;
		case 2:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s;LA%s,%s;VV%d,%d;EP0,0;KS0,0;PM%d,%d;TT%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->latStr1,\
				pMe->latStr2,pMe->dVelHor1,pMe->dVelHor2,\
				pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2);
			break;
		case 3:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s,%s;LA%s,%s,%s;VV%d,%d,%d;EP0,0,0;KS0,0,0;PM%d,%d,%d;TT%s,%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,\
				pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3);
			break;
		case 4:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s,%s,%s;LA%s,%s,%s,%s;VV%d,%d,%d,%d;EP0,0,0,0;KS0,0,0,0;PM%d,%d,%d,%d;TT%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,\
				pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4);
			break;
		case 5:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d;EP0,0,0,0,0;KS0,0,0,0,0;PM%d,%d,%d,%d,%d;TT%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,\
				pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5);
			break;
		case 6:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d,%d;EP0,0,0,0,0,0;KS0,0,0,0,0,0;PM%d,%d,%d,%d,%d,%d;TT%s,%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,pMe->lonStr6,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,pMe->latStr6,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,pMe->dVelHor6,\
				pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5,pMe->sDateTime6);
			break;
		case 7:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s,%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d,%d,%d;EP0,0,0,0,0,0,0;KS0,0,0,0,0,0,0;PM%d,%d,%d,%d,%d,%d,%d;TT%s,%s,%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,pMe->lonStr6,pMe->lonStr7,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,pMe->latStr6,pMe->latStr7,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,pMe->dVelHor6,pMe->dVelHor7,\
				pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5,pMe->sDateTime6,pMe->sDateTime7);
			break;
		case 8:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO%s,%s,%s,%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d,%d,%d,%d;EP0,0,0,0,0,0,0,0;KS0,0,0,0,0,0,0,0;PM%d,%d,%d,%d,%d,%d,%d,%d;TT%s,%s,%s,%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,\
				pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,pMe->lonStr6,pMe->lonStr7,pMe->lonStr8,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,pMe->latStr6,pMe->latStr7,pMe->latStr8,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,pMe->dVelHor6,pMe->dVelHor7,pMe->dVelHor8,\
				pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5,pMe->sDateTime6,pMe->sDateTime7,pMe->sDateTime8);
			break;
		}
	}
	else
	{
		switch(pMe->nArySize)
		{
		case 1:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s;LA%s;VV%d;EP0;KS0;PM%c;TT%s",pMe->strCenterIp,pMe->lonStr1,pMe->latStr1,pMe->dVelHor1,pMe->m_PM_BD,pMe->sDateTime1);
			break;
		case 2:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s;LA%s,%s;VV%d,%d;EP0,0;KS0,0;PM%c,%c;TT%s,%s",pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->latStr1,pMe->latStr2,pMe->dVelHor1,pMe->dVelHor2,pMe->m_PM_BD,pMe->m_PM_BD,pMe->sDateTime1,pMe->sDateTime2);
			break;
		case 3:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s,%s;LA%s,%s,%s;VV%d,%d,%d;EP0,0,0;KS0,0,0;PM%c,%c,%c;TT%s,%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,\
				pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3);
			break;
		case 4:
			SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s,%s,%s;LA%s,%s,%s,%s;VV%d,%d,%d,%d;EP0,0,0,0;KS0,0,0,0;PM%c,%c,%c,%c;TT%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,\
				pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4);
			break;
		case 5:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d;EP0,0,0,0,0;KS0,0,0,0,0;PM%c,%c,%c,%c,%c;TT%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,\
				pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5);
			break;
		case 6:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d,%d;EP0,0,0,0,0,0;KS0,0,0,0,0,0;PM%c,%c,%c,%c,%c.%c;TT%s,%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,pMe->lonStr6,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,pMe->latStr6,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,pMe->dVelHor6,\
				pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5,pMe->sDateTime6);
			break;
		case 7:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s,%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d,%d,%d;EP0,0,0,0,0,0,0;KS0,0,0,0,0,0,0;PM%c,%c,%c,%c,%c,%c,%c;TT%s,%s,%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,pMe->lonStr6,pMe->lonStr7,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,pMe->latStr6,pMe->latStr7,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,pMe->dVelHor6,pMe->dVelHor7,\
				pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5,pMe->sDateTime6,pMe->sDateTime7);
			break;
		case 8:
				SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&pp=LO%s,%s,%s,%s,%s,%s,%s,%s;LA%s,%s,%s,%s,%s,%s,%s,%s;VV%d,%d,%d,%d,%d,%d,%d,%d;EP0,0,0,0,0,0,0,0;KS0,0,0,0,0,0,0,0;PM%c,%c,%c,%c,%c,%c,%c,%c;TT%s,%s,%s,%s,%s,%s,%s,%s",\
				pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->lonStr4,pMe->lonStr5,pMe->lonStr6,pMe->lonStr7,pMe->lonStr8,\
				pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->latStr4,pMe->latStr5,pMe->latStr6,pMe->latStr7,pMe->latStr8,\
				pMe->dVelHor1,pMe->dVelHor2,pMe->dVelHor3,pMe->dVelHor4,pMe->dVelHor5,pMe->dVelHor6,pMe->dVelHor7,pMe->dVelHor8,\
				pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,pMe->m_PM_BD,\
				pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3,pMe->sDateTime4,pMe->sDateTime5,pMe->sDateTime6,pMe->sDateTime7,pMe->sDateTime8);
			break;
		}
//		SPRINTF(pMe->sUrl,"http://%s&ver=3.0.1w&pp=LO%s,%s,%s;LA%s,%s,%s;VV%d,0,0;EP0,0,0;KS0,0,0;PM%c,%c,%c;TT%s,%s,%s",pMe->strCenterIp,pMe->lonStr1,pMe->lonStr2,pMe->lonStr3,pMe->latStr1,pMe->latStr2,pMe->latStr3,pMe->m_PM1,pMe->m_PM2,pMe->m_PM3,pMe->sDateTime1,pMe->sDateTime2,pMe->sDateTime3);
	}

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),pMe->sUrl); 
	pMe->m_MUTEX = 2;

	if ((pMe->m_F_QF == FALSE) && (pMe->m_F_R_GPS != 1))
	{
		myiweb_Stop_gps(pMe);
		myiweb_Start_gps(pMe,pMe->sUrl,0);
	}
	else
	{
		if (pMe->mux_7Day == 0)
		{
			write_GPSData_7Day(pMe, pMe->sUrl);
		}
	}


	return TRUE;
}


boolean SendErrorBDData( newgimis *pMe )
{
	JulianType dDate;				// ��¼�յ���λ���ݵ�ʱ��

	////////////////// 2015-10-21 START ////////////////
	int si_ret = 0;
	si_ret = judgeSendorCancel( pMe ); // �ж��Ƿ��� myiweb_start_xt ����ȡ������   1 - ��ʾ����   0 - ��ʾ������
	if (si_ret == 0) // ��ʾ������
	{
		uf_RecLog(pMe,"judgeSendorCancel exit");
		return TRUE;
	}
	/////////////////////    END     ///////////////////////////////
	
	
	GETJULIANDATE(0,&dDate);		//	ȡ������

	pMe->i_HttpMod = 2; //HTTP���շ�������ģʽ ����	1 - ������ջ�������������Ȩ�� 2 - ���ջ�������GPS�����ϴ�����������
	
	MEMSET(pMe->sUrl,0x00,1024);

	SPRINTF(pMe->sDateTime1,"%04d%02d%02d%02d%02d%02d",dDate.wYear,dDate.wMonth,dDate.wDay,dDate.wHour,dDate.wMinute,dDate.wSecond);

	/*pMe->m_ycPM = '13';*/
	if (pMe->cPosDetMode == 13)
	{
		pMe->m_PM_BD = 13;
	}
	else
	{
		pMe->m_PM_BD = 14;
	}

	if (pMe->MyMobileNo != NULL)  // �е绰�����ʱ���Լ��ӵ绰���� û�е�ʱ��
	{
		SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&tel=%s&imsi=%s&pp=LO0x100004;LA0x100004;VV0;EP0;KS0;PM%d;TT%s",pMe->strCenterIp,pMe->MyMobileNo,pMe->szMobileID,pMe->m_PM_BD,pMe->sDateTime1);
	}
	else
	{
		SPRINTF(pMe->sUrl,"http://%s&ver=3.1.2au&imsi=%s&pp=LO0x100004;LA0x100004;VV0;EP0;KS0;PM%d;TT%s",pMe->strCenterIp,pMe->szMobileID,pMe->m_PM_BD,pMe->sDateTime1);

	}

	pMe->m_MUTEX = 1;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),pMe->sUrl); 

	MEMSET(pMe->show_Time,0x00,10); //����ڽ�������ʾ��ʱ��

	SPRINTF(pMe->show_Time,"%02d:%02d:%02d",dDate.wHour,dDate.wMinute,dDate.wSecond);
	SPRINTF(pMe->show_LatLon,"0x100004   0x100004");

	if (pMe->m_F_QF == FALSE)
	{
		uf_RecLog(pMe,"Send error GPS data");

		if (pMe->m_F_R_E == 0)
		{
			////////////////////////GPS�������ݰ�//////////////////////////////////
			myiweb_start_E_cb(pMe);
			////////////////////////////////////�޸Ĳ���/////////////////////////////
		}
	}
	else
	{
		if (pMe->mux_7Day == 0)
		{
			write_GPSData_7Day(pMe, pMe->sUrl);
		}
	}
	return TRUE;
}

int newgimis_Init_S958GPS(newgimis * pMe)
{
	//GPS_MODULE_INIT����δ��ʼGPSģ���Ƿ���ڵļ��
	//GPS_MODULE_WAITING��GPSģ���Ƿ���ڼ����
	//GPS_MODULE_SUCESSED�� GPSģ���Ƿ���ڼ��ɹ���ģ����� ���ſ�ִ��2��5�Ľӿ�
	//GPS_MODULE_EXISTED��GPSģ�鲻����
	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_Init_S958GPS pMe->pIgpsext  = %p",pMe->pS958gps);
	if (pMe->pS958gps != NULL)
	{
		return 1;
	}
	if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_OEMTTSEXTGPS ,
		(void **)&pMe->pS958gps ) == SUCCESS )
	{
		uf_RecLog(pMe,"create newgimis_Init_S958GPS OK");
		//�������ڼ��GPSģ���״̬
		while (GPS_MODULE_SUCESSED == ITTSEXTGPS_GpsModuleCheck(pMe->pS958gps)  )
		{
			uf_RecLog(pMe,"newgimis_Init_S958GPS GPS_MODULE_SUCESSED");
			return 1;
		}
	}
	else
	{
		uf_RecLog(pMe,"create newgimis_Init_S958GPS fail");
	}

	return 0;
}

void newgimis_OpenGPS_S958GPS(newgimis * pMe)
{
	int si_ret = 0;
	uf_RecLog(pMe,"newgimis_OpenGPS_S958GPS coming");
	if (pMe->pS958gps == NULL)
	{
		uf_RecLog(pMe,"newgimis_OpenGPS_S958GPS fail res pMe->pS958gps == NULL ");
		return;
	}
	pMe->GPSFlag = TRUE;
	
	ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd),"newgimis_OpenGPS_S958GPS pMe->pS958gps  = %p",pMe->pS958gps);

	si_ret = ITTSEXTGPS_Gps_AutoPositioning(pMe->pS958gps ,/*30000*/pMe->iInterval * 1000);
	//GPS_OPEN_FIRST :GPS�״ο������Ѿ���ͨ��Դ����δ���й���λ
    //GPS_HAVEOPENED:GPS�Ѿ���������

	if (si_ret == GPS_OPEN_FIRST ) //
	{
		uf_RecLog(pMe,"GPS_OPEN_FIRST,�Ѿ���ͨ��Դ,��δ���й���λ");
	}
	else if (si_ret == GPS_HAVEOPENED  ) //
	{
		uf_RecLog(pMe,"GPS_HAVEOPENED �Ѿ���������");
	}
}

void newgimis_CloseGPS_S958GPS(newgimis * pMe)
{
	int i= 0;
	
	int si_ret = 0;
	
	pMe->GPSFlag = FALSE;
	
	pMe->m_Fix_C = 0;//��λ������0
	
	SPRINTF(pMe->show_LatLon,"0x100001   0x100001");
	
	if (pMe->pS958gps == NULL)
	{
		return;
	}
	si_ret = ITTSEXTGPS_GpsStopPositioning(pMe->pS958gps);
	//GPS_CLOSED_SUCCESSED:GPS�رճɹ�
	//GPS_CLOSED_FAILED:GPS�ر�ʧ��
	if (si_ret == GPS_CLOSED_SUCCESSED)
	{
		uf_RecLog(pMe,"newgimis_CloseGPS_S958GPS success GPS_CLOSED_SUCCESSED");
	}
	else if (si_ret == GPS_CLOSED_FAILED)
	{
		uf_RecLog(pMe,"newgimis_CloseGPS_S958GPS fail GPS_CLOSED_FAILED");
	}
	
    ITTSEXTGPS_Release(pMe->pS958gps);
	pMe->pS958gps = NULL;
}

//////////////////////////////////////////////////
void uf_MonitorWait_HTTP(newgimis *pMe)
{
	pMe->m_F_R_C = 0;

	FREE(pMe->m_HTTP_C);
	pMe->m_HTTP_C = NULL;

	return;
}


void uf_MonitorWait_GPS(newgimis *pMe)
{
	pMe->m_F_R_GPS = 0;

	FREE(pMe->m_H_GPS);
	pMe->m_H_GPS = NULL;
	
	return;
}

void uf_MonitorWait_E(newgimis *pMe)
{
	pMe->m_F_R_E = 0;

	FREE(pMe->m_HTTP_E);
	pMe->m_HTTP_E = NULL;
	
	return;
}


unsigned short CRC16 (char *puchMsg, unsigned short usDataLen ) 
{
	//���ֽڱ�
	/* ��λ�ֽڵ�CRC ֵ */
	unsigned char auchCRCLo[] = 
	{
		0x00,0xC0,0xC1,0x01,0xC3,0x03,0x02,0xC2,0xC6,0x06,0x07,0xC7,0x05,0xC5,0xC4,0x04,
		0xCC,0x0C,0x0D,0xCD,0x0F,0xCF,0xCE,0x0E,0x0A,0xCA,0xCB,0x0B,0xC9,0x09,0x08,0xC8,
		0xD8,0x18,0x19,0xD9,0x1B,0xDB,0xDA,0x1A,0x1E,0xDE,0xDF,0x1F,0xDD,0x1D,0x1C,0xDC,
		0x14,0xD4,0xD5,0x15,0xD7,0x17,0x16,0xD6,0xD2,0x12,0x13,0xD3,0x11,0xD1,0xD0,0x10,
		0xF0,0x30,0x31,0xF1,0x33,0xF3,0xF2,0x32,0x36,0xF6,0xF7,0x37,0xF5,0x35,0x34,0xF4,
		0x3C,0xFC,0xFD,0x3D,0xFF,0x3F,0x3E,0xFE,0xFA,0x3A,0x3B,0xFB,0x39,0xF9,0xF8,0x38,
		0x28,0xE8,0xE9,0x29,0xEB,0x2B,0x2A,0xEA,0xEE,0x2E,0x2F,0xEF,0x2D,0xED,0xEC,0x2C,
		0xE4,0x24,0x25,0xE5,0x27,0xE7,0xE6,0x26,0x22,0xE2,0xE3,0x23,0xE1,0x21,0x20,0xE0,
		0xA0,0x60,0x61,0xA1,0x63,0xA3,0xA2,0x62,0x66,0xA6,0xA7,0x67,0xA5,0x65,0x64,0xA4,
		0x6C,0xAC,0xAD,0x6D,0xAF,0x6F,0x6E,0xAE,0xAA,0x6A,0x6B,0xAB,0x69,0xA9,0xA8,0x68,
		0x78,0xB8,0xB9,0x79,0xBB,0x7B,0x7A,0xBA,0xBE,0x7E,0x7F,0xBF,0x7D,0xBD,0xBC,0x7C,
		0xB4,0x74,0x75,0xB5,0x77,0xB7,0xB6,0x76,0x72,0xB2,0xB3,0x73,0xB1,0x71,0x70,0xB0,
		0x50,0x90,0x91,0x51,0x93,0x53,0x52,0x92,0x96,0x56,0x57,0x97,0x55,0x95,0x94,0x54,
		0x9C,0x5C,0x5D,0x9D,0x5F,0x9F,0x9E,0x5E,0x5A,0x9A,0x9B,0x5B,0x99,0x59,0x58,0x98,
		0x88,0x48,0x49,0x89,0x4B,0x8B,0x8A,0x4A,0x4E,0x8E,0x8F,0x4F,0x8D,0x4D,0x4C,0x8C,
		0x44,0x84,0x85,0x45,0x87,0x47,0x46,0x86,0x82,0x42,0x43,0x83,0x41,0x81,0x80,0x40
	};

	//���ֽڱ�
	/* ��λ�ֽڵ�CRC ֵ */
	unsigned char auchCRCHi[] = 
	{
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
		0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
		0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
		0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
		0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
		0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
		0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,
		0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
		0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40,0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,
		0x00,0xC1,0x81,0x40,0x01,0xC0,0x80,0x41,0x01,0xC0,0x80,0x41,0x00,0xC1,0x81,0x40
	};

	
	unsigned char uchCRCHi = 0xFF ; /* CRC �ĸ��ֽڳ�ʼ�� */
	unsigned char uchCRCLo = 0xFF ; /* CRC �ĵ��ֽڳ�ʼ�� */
	unsigned uIndex ; /* CRC ��ѯ������ */
	while (usDataLen--) /* ����������Ļ����� */
	{
		uIndex = uchCRCLo ^ *puchMsg++ ; /* ���� CRC */
		uchCRCLo = uchCRCHi ^ auchCRCHi[uIndex] ;
		uchCRCHi = auchCRCLo[uIndex] ;
	}

	return (uchCRCHi << 8 | uchCRCLo) ;
}

