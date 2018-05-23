#include "newgimis.bid"
#include "newgimis.h"
#include "gmTools.h"

#include "gm_FileUpload.h"  // Add 2012-09-21

#include "My_Function.h"







/*===========================================================================
                   M A I N   M E N U
===========================================================================*/
boolean SamplePosDet_More_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam )
{
   boolean bHandled = FALSE;
   IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );
   int si_Ret = 0;

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
			   
			   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_LOCAL_INFO,
                  SUBMENU_LOCAL_INFO, NULL, (uint32)pMenuCtl );  // ������Ϣ
			   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_NFC,
                  SUBMENU_NFC, NULL, (uint32)pMenuCtl );  // NFCˢ��
			   if (pMe->cPicMode == '1')
			   {
				   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_FS_ZT_1,
                  SUBMENU_FULL, NULL, (uint32)pMenuCtl );  // �����س�
			   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_FS_ZT_0,
                  SUBMENU_EMPTY, NULL, (uint32)pMenuCtl );  // ���Ϳճ�
			   }
			   else
			   {
				   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_SEND_FULL,
                  SUBMENU_FULL, NULL, (uint32)pMenuCtl );  // �����س�
			   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_SEND_EMPUT,
                  SUBMENU_EMPTY, NULL, (uint32)pMenuCtl );  // ���Ϳճ�
			   }
			   
				
			  				 
//////////////////////////////////////////////////////////////////////////
			   //#��������ͼ��#		����δ������񣬲˵�������ʾͼ��  2016-1-21
			   //IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_TASKLIST,SUBMENU_TASK_LIST, NULL, (uint32)pMenuCtl );  // �����б�
				 if (pMe->pTaskApp)
				 {
					 cTaskApp *me = pMe->pTaskApp;
					 if(me->p_head) 
					 {					 
						 MP_AddMenuItem(pMenuCtl, IDS_TASKLIST, NULL, IDB_HAVE_TASK,     SUBMENU_TASK_LIST, (uint32)pMenuCtl);
					 }else{
						 //IDS_TASKLIST �ĳ� IDS_NO_TASKLIST Ϊ�˲˵������ÿ�
						IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_NO_TASKLIST,SUBMENU_TASK_LIST, NULL, (uint32)pMenuCtl );  // �����б�
					 }
				 }
				 else
				 {
						IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_NO_TASKLIST,SUBMENU_TASK_LIST, NULL, (uint32)pMenuCtl );  // �����б� IDS_TASKLIST	
				 }
//////////////////////end//////////////////////////////////////////////
		   
			   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_SEND_LOG,
                  SUBMENU_SEND_LOG, NULL, (uint32)pMenuCtl );  //��־�ϴ�

			   IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_SPEAK_MANAGER,
                  SUBMENU_SPEAK_MANAGER, NULL, (uint32)pMenuCtl );  //��������

			   /////////   end ////
               IMENUCTL_SetTitle( pMenuCtl, NEWGIMIS_RES_FILE, IDS_MORE, NULL );

               testgps_SetScreenData( pMe, (void *)pMenuCtl );
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
	
         if( IMENUCTL_IsActive( pMenuCtl ) ) {
            IMENUCTL_Redraw( pMenuCtl );
			
         }
		 else
		 {
            IMENUCTL_SetActive( pMenuCtl, TRUE );  /* Also causes a menu draw */			 
		 }
      }

      break;

   case EVT_KEY:
	   bHandled = IMENUCTL_HandleEvent( pMenuCtl, eCode, wParam, dwParam );

//		ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "�����˵� EVT_KEY_RELEASE 1");
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
				
				//	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
				pMe->m_RelyExit = FALSE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
				ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
				pMe->m_FOBFlag = TRUE; //�ں�̨����
				pMe->m_WitchPag = 0;
				//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"��̨���� 6:%d",pMe->m_WitchPag);
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
				// ����ֱ�ӻص���̨
				  if (pMenuCtl)
				  {
					  IMENUCTL_Release( pMenuCtl );
				  }
				   
				   pMenuCtl = NULL;
				   

				   pMe->currentHandler = 0;
				   testgps_SetScreenData( pMe, 0 );

				   //	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
				   pMe->m_RelyExit = FALSE;//����Ƿ������˳�  TRUE -�����˳� FALSE -- ת���̨
				   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //ת��̨
				   pMe->m_FOBFlag = TRUE; //�ں�̨����
				   pMe->m_WitchPag = 0;
		//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"��̨���� 6:%d",pMe->m_WitchPag);
				   bHandled = TRUE;
				   return TRUE;
			}
		}
		//////////////////////////////////////
		//////////////////////////////////////





       if( wParam == AVK_END  || wParam == AVK_CLR ) 
	   {

		   pMe->m_WitchPag = 10; // ���ظ�����Ǹ�����
		   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );

		   pMe->m_WitchPag = 10;
//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"������� ҳ���������� 2:%d",pMe->m_WitchPag);
		   bHandled = TRUE;
	   }

/*
		 if( wParam == AVK_END  || wParam == AVK_CLR ) {

		   pMe->m_WitchPag = 4; // ���ظ�����Ǹ�����
		   testgps_GotoScreen( pMe, SCREENID_MORE, 0 );

		   pMe->m_WitchPag = 4;
		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"������� ҳ���������� 2:%d",pMe->m_WitchPag);
		   bHandled = TRUE;
	   }
*/

		
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
      if( dwParam == (uint32)pMenuCtl ) {
         switch( wParam ) {

        case SUBMENU_LOCAL_INFO:  // ������Ϣ
			{
   
		//		 pMe->m_WitchPag = 6;		 
		//		testgps_GotoScreen( pMe, SCREENID_SHOWGETGPSINFO, wParam );				
			 }
            return TRUE;
		case SUBMENU_NFC: // NFC ˢ��  cPicMode - '3'   ��ʾNFC����
			{
		//		pMe->m_WitchPag = 7;
				if (pMe->cPicMode != '3') // ��ʾ�޴˹���
				{
					pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
					without_this_feature(pMe,FALSE);
				}

			}
			return TRUE;

		case SUBMENU_FULL:
				{
					pMe->m_WitchPag = 8;
					if (pMe->cPicMode == '0') // �����س�
					{
						pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
						newgimis_SendFull(pMe, TRUE );
					}
					else if (pMe->cPicMode == '1')  // ����״̬ 1
					{
						pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
						newgimis_Send_ZT_1(pMe, TRUE );  
					}
					else	// ��ʾ�޴˹���
					{
						pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
						without_this_feature(pMe,FALSE);
					}
					
				}
			return TRUE;
		case SUBMENU_EMPTY:
				{
					pMe->m_WitchPag = 9;
//					newgimis_SendEmpty(pMe, TRUE );
					if (pMe->cPicMode == '0') // ���Ϳճ�
					{
						pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
						newgimis_SendEmpty(pMe, TRUE );
					}
					else if (pMe->cPicMode == '1')  // ����״̬ 0
					{
						pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
						newgimis_Send_ZT_0(pMe, TRUE );
					}
					else	// ��ʾ�޴˹���
					{
						pMe->m_WitchPag = 25;	//����  [3/23/2016 Administrator] 
						without_this_feature(pMe,FALSE);
					}
				}
			return TRUE;

		 case SUBMENU_TASK_LIST:  // �����б�
			 {
				 cTaskApp *me = pMe->pTaskApp;
				 
			//	 pMe->m_WitchPag = 1;
			//	 newgimis_forYJKQ(pMe,TRUE);
				 //if (pMe->pTaskApp) 
				if (pMe->pTaskApp)
				{
					if(me->p_head) 
					{
						 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Press TaskList, have task ,GotoScreen SCREENID_TASKLIST  ");

						 pMe->m_WitchPag = 16; 
						 pMe->m_Cur_Page = 1 ;// ��ʾÿ�ν�ȥ���ǵ�һҳ
						 testgps_GotoScreen( pMe, SCREENID_TASKLIST, wParam );
					}
					else
					{
						pMe->m_bMiss = 1;//  [3/23/2016 Administrator]
						ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Press TaskList, Display no task. ");
						uf_AEPrint_RWLB_Back_Two(pMe,FALSE);	
					}
				}
				else
				{
					pMe->m_bMiss = 1; 
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Press TaskList1, Display no task. ");
					uf_AEPrint_RWLB_Back_Two(pMe,FALSE);	
				}
				 
			 }
			
			return TRUE;
		case SUBMENU_SPEAK_MANAGER:  // ���Թ���
				{
			//		pMe->m_WitchPag = 2;
			//		newgimis_forphoto(pMe,TRUE);
				}
				return TRUE;
		case SUBMENU_SEND_LOG:
		{
			si_Ret = juage_logfile(pMe);
			if (!si_Ret)
			{
				pMe->m_WitchPag = 25;// ���� [3/16/2016 Administrator]
				uf_AEPrint_Sending(pMe,FALSE);  // ������ ..
				if ( FleUpd_HandleEvent(pMe,FLDUPD_EVT_START,0,0) )	 
				{
					ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"shang  chuan jin lai Audio");
					ISHELL_PostEvent(pMe->a.m_pIShell,pMe->a.clsID,FLDUPD_EVT_UPLOAD,51,0); // 51�����ֵ���Ǵ��� ��3�� ��־�ϴ�
				}
			}
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



//
// ================================================== //
// ��ʾ����  û��һ����������
// boolean bCallBack  TRUE �ص�, FALSE ���ûص�
// ================================================== //
void uf_AEPrint_RWLB_Back_Two(newgimis * pMe,boolean bCallBack)
{   // \u65e0\u4efb\u52a1 ������
	AECHAR szText[] = {0x65e0,0x4efb,0x52a1,0x0000}; // L"������";
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu_Two, pMe);

}


//
// ================================================== //
// ��ʾ����  û��һ����������
// boolean bCallBack  TRUE �ص�, FALSE ���ûص�
// ================================================== //
void uf_AEPrint_YJKQ_Back_Two(newgimis * pMe,boolean bCallBack)
{   // \u6682\u65e0\u47\u50\u53\u8003\u52e4\u6570\u636e  ����GPS��������
	AECHAR szText[] = {0x6682,0x65e0,0x47,0x50,0x53,0x8003,0x52e4,0x6570,0x636e,0x0000}; // L"����Ƭ";
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu_Two, pMe);

}

// ================================================== //
// ��ʾ����  �޴˹���
// boolean bCallBack  TRUE �ص�, FALSE ���ûص�
// ================================================== //
// without_this_feature start //
void without_this_feature(newgimis * pMe,boolean bCallBack)
{   // \u65e0\u6b64\u529f\u80fd �޴˹���
	AECHAR szText[] = {0x65e0,0x6b64,0x529f,0x80fd,0x0000}; // 
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu_Two, pMe);

}
// without_this_feature  end  //

// uf_ReturnMenu_Two  ���ظ���Ĳ˵�����
void uf_ReturnMenu_Two(newgimis * pMe)
{
	//##��;���
	pMe->m_bMiss = 0;

	pMe->m_bSendPF = FALSE; //��������Ƿ����ڷ���ͼƬ�ı�־λ TRUE -- ��ʾ���ڷ���״̬  FALSE -- ��ʾ����
	ISHELL_CancelTimer(pMe->a.m_pIShell,(PFNNOTIFY)uf_ReturnMenu_Two,pMe);
	pMe->m_WitchPag = 4;
	testgps_GotoScreen( pMe, SCREENID_MORE, 0 );
	
}



/********************************************************************************************
// ������ɺ�Ķ���  ���ظ�����Ǹ�����
// �������
  FileUpload * pMe	: 
********************************************************************************************/
void GPS_Afterpost_Two(newgimis *pMe)
{

	pMe->m_WitchPag = 4;
	testgps_GotoScreen( pMe, SCREENID_MORE, 0 );  //���ص�������

	return;
}



//
// ================================================== //
// ��ʾ���� ������  ���ظ���Ľ���
// boolean bCallBack  TRUE �ص�, FALSE ���ûص�
// ================================================== //
void uf_AEPrint_NO_NET_Back_Two(newgimis * pMe,boolean bCallBack)
{   // \u65e0\u7f51\u7edc  ������
	AECHAR szText[] = {0x65e0,0x7f51,0x7edc,0x0000}; // L"������";
	

	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = �����ַ����Ķ���ʾ
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);

	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu_Two, pMe);

}




// �ж���û����־����
/*
	����ֵ 0 -- ��ʾ������ļ�������������  > 0 ˵�����ش���
*/
uint32 juage_logfile( newgimis *pMe ) //�ж���û����־����  
{
   IFileMgr   *pIFileMgr = NULL;
   IFile      *pIConfigFile = NULL;
   uint32      nResult = 0;
   FileInfo fiInfo;

   // Create the instance of IFileMgr
   nResult = ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_FILEMGR, (void**)&pIFileMgr );
   if ( SUCCESS != nResult ) {
      return nResult;
   }

   nResult = IFILEMGR_Test( pIFileMgr, LOG_FILENAME );
   if ( SUCCESS == nResult ) 
   {
      pIConfigFile = IFILEMGR_OpenFile( pIFileMgr, LOG_FILENAME, _OFM_READ );
      if ( !pIConfigFile ) {
         nResult = EFAILED;
      }
      else 
	  {
		   if ( SUCCESS != IFILE_GetInfo( pIConfigFile, &fiInfo ) ) 
		   {
			  return EFAILED;
		   }

		   if ( fiInfo.dwSize == 0 ) 
		   {
			  return EFAILED;
		   }
	  }

   }

   if (pIConfigFile)
   {
	   IFILE_Release( pIConfigFile );
   }

   if (pIFileMgr)
   {
	   IFILEMGR_Release( pIFileMgr );
   }


//   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"�ж� rec_info.log file");

   return nResult;
}