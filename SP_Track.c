/*======================================================
FILE:  SP_Track.c

SERVICES: Tracking using IPosDet.
  Track_Init
  Track_Start
  Track_Stop

GENERAL DESCRIPTION:
	Sample code to demonstrate services of IPosDet. See SP_Track.h for the
   description of exported functions.

        Copyright ?2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEComdef.h"
#include "BREWVersion.h"
#include "AEEStdLib.h"
#include "SP_Track.h"

#include "gmTools.h"  //ֻ�����ڴ�ӡ��־


static void Track_Notify( TrackState *pts )
{
   pts->bInNotification = TRUE;
   pts->pcbResp->pfnNotify( pts->pcbResp->pNotifyData );
   pts->bInNotification = FALSE;
}

static void Track_Cancel( AEECallback *pcb )
{
   TrackState *pts = (TrackState *)pcb->pCancelData;
   int si_tmp = 0;

   if( TRUE == pts->bInNotification ) {
      /* It is not safe to cleanup from a notification. Defer it. */
      pts->bSetForCancellation = TRUE;
      return;
   }

   /* Kill any ongoing process */
   CALLBACK_Cancel( &pts->cbInfo );
   CALLBACK_Cancel( &pts->cbIntervalTimer );

   pts->pcbResp->pfnCancel = 0;
   pts->pcbResp->pCancelData = 0;

   do 
   {
	   si_tmp = IPOSDET_Release( pts->pPos );
   } while(si_tmp > 0);
   
   si_tmp = 0;
   do 
   {
	   si_tmp = ISHELL_Release( pts->pShell );
   } while(si_tmp > 0);
   

   FREE( pts );
}




// ���ռ���Ļص����� 
static void Track_cbInterval( TrackState *pts )
{
   /* Cancel if it was deferred. */
	newgimis *pnewgimis = pts->m_pParent;
   if( TRUE == pts->bSetForCancellation ) 
   {
      Track_Cancel( pts->pcbResp );
      return;
   }


	//uf_RecLog( pnewgimis , "0x100001 next Track_cbInterval");

   DBGPRINTF( "TRACK : %d %d", pts->bModeAuto, pts->bModeLocal );

   /*
      if( TRUE == pts->bInProgress && SUCCESS != IPOSDET_GetGPSInfo( pts->pPos, 
       AEEGPS_GETINFO_LOCATION|AEEGPS_GETINFO_ALTITUDE|AEEGPS_GETINFO_VELOCITY, AEEGPS_ACCURACY_LEVEL1, 
      &pts->theInfo, &pts->cbInfo ) ) 
   */
/*
   if( TRUE == pts->bInProgress && SUCCESS != IPOSDET_GetGPSInfo( pts->pPos, 
       AEEGPS_GETINFO_LOCATION|AEEGPS_GETINFO_ALTITUDE|AEEGPS_GETINFO_VELOCITY, pnewgimis->m_F_P_V, 
      &pts->theInfo, &pts->cbInfo ) ) 
*/
      if( TRUE == pts->bInProgress && SUCCESS != IPOSDET_GetGPSInfo( pts->pPos, 
       AEEGPS_GETINFO_LOCATION|AEEGPS_GETINFO_ALTITUDE|AEEGPS_GETINFO_VELOCITY, pnewgimis->m_F_P_V, 
      &pts->theInfo, &pts->cbInfo ) ) 
   {

      /* Report a failure and bailout */
      pts->pResp->nErr = AEEGPS_ERR_GENERAL_FAILURE;

      Track_Notify( pts );

      Track_Stop( pts );
   }
}



static void Track_Standalone( TrackState *pts ) // ������
{
   AEEGPSConfig config;

   DBGPRINTF( "TRACK NETWORK" );
   
   (void) IPOSDET_GetGPSConfig( pts->pPos, &config );

   config.mode = AEEGPS_MODE_TRACK_STANDALONE;

   (void) IPOSDET_SetGPSConfig( pts->pPos, &config );

   pts->bModeLocal = FALSE;
}

static void Track_One_Shot( TrackState *pts ) //����վ
{
   AEEGPSConfig config;

   DBGPRINTF( "TRACK NETWORK" );
   
   (void) IPOSDET_GetGPSConfig( pts->pPos, &config );

   
	//	  config.mode = AEEGPS_MODE_TRACK_NETWORK;
         config.mode = AEEGPS_MODE_ONE_SHOT;  // ���Ҳ������ ���Ǿ���û�� OPTIMAL �ĺ�


//   config.mode = AEEGPS_MODE_TRACK_STANDALONE;

   (void) IPOSDET_SetGPSConfig( pts->pPos, &config );

   pts->bModeLocal = FALSE;
}



static void Track_Network( TrackState *pts )  //������
{
   AEEGPSConfig config;

   DBGPRINTF( "TRACK NETWORK" );
   
   (void) IPOSDET_GetGPSConfig( pts->pPos, &config );

   config.mode = AEEGPS_MODE_TRACK_NETWORK;

   (void) IPOSDET_SetGPSConfig( pts->pPos, &config );

   pts->bModeLocal = FALSE;
}

// AEEGPS_MODE_DLOAD_FIRST

static void Track_DLOAD_FIRST( TrackState *pts )  //����
{
   AEEGPSConfig config;

   DBGPRINTF( "TRACK NETWORK" );
   
   (void) IPOSDET_GetGPSConfig( pts->pPos, &config );

   config.mode = AEEGPS_MODE_DLOAD_FIRST;

   (void) IPOSDET_SetGPSConfig( pts->pPos, &config );

   pts->bModeLocal = FALSE;
}


static void Track_Local( TrackState *pts )  //��վ����
{
   AEEGPSConfig config;

   DBGPRINTF( "TRACK LOCAL" );

   (void) IPOSDET_GetGPSConfig( pts->pPos, &config );

   config.mode = AEEGPS_MODE_TRACK_LOCAL;

   (void) IPOSDET_SetGPSConfig( pts->pPos, &config );

   pts->bModeLocal = TRUE;
}

static void Track_Optmal( TrackState *pts )  //�ֻ���ѡ
{
   AEEGPSConfig config;

   DBGPRINTF( "TRACK LOCAL" );

   (void) IPOSDET_GetGPSConfig( pts->pPos, &config );

   config.mode = AEEGPS_MODE_TRACK_OPTIMAL;

   (void) IPOSDET_SetGPSConfig( pts->pPos, &config );

   pts->bModeLocal = TRUE;
}

// �յ����ݺ�Ļص�����
static void Track_cbInfo( TrackState *pts )
{
/*	//////  �����ǹ���GPS�Ż������� START ////////
	int m_F_G_C; //����ͳ����û����ѯ��һ��  ������� ��һ�� 1000004  m_F_G_C == 6 ��˵����ѯ��һ��  �����ȡ����  ������ �� 0
	int m_F_M; //���������ʲôģʽ��   ���� 0 ��Ϊ����ѯ�������  ���Ϊ 1 ����ƽ̨�·���ģʽ�� 
	int m_F_P_V; //������Ƕ�λ����  AEEGPS_ACCURACY_LEVEL1 -- 1  �� AEEGPS_ACCURACY_LEVEL6 -- 6 ��1-6����ȼ����ȣ�
	boolean m_F_Pos;  // ������ǵȼ��Ƿ�Ҫ�� �������֮�� �ֱ�ɲ����ӵȼ� TRUE == ��ʾ���ӵȼ�  FLASE == ��ʾ�����ӵȼ�
	boolean m_F_P_F ;// ��������ǲ���track_stop ���һ����track_start  TRUE == ��ʾ  ��  FLASE = ��ʾ����
	boolean m_Q_P; //��������Ƿ��ǳ�����ʱ���ٶ�λ  TRUE -- ��  FLASE -- ����
	int m_POS_M;// ��ȡ��ǰ�ֻ��ܶ�λ����ģʽ
	int m_NOMAINPOS; //��ʾ�Ƿ���Ҫ����ѯģʽ�¶�λ����ʱ���л�����ģʽ  == 0 ����Ҫ  == 1 ����Ҫ
	boolean m_PosMode ;// ��������Ƿ���ֻ���ô����Ƕ�λ TRUE -- ��ʾֻ���ô�����  FLASH -- ���� �����Ա�ķ�ʽ
	///////          END        //////////////////
*/

	newgimis *pwa = pts->m_pParent;
	int si_Ret = 0;
// �����������з�ɢ�� if else �Ľ����ط���2015-9-23
//	pwa->m_F_START = FALSE; //��������ǲ��ǲŷ���Track_Start   TRUE --  ��ʾ�շ���      FALSE -- ���ǲ��Ǹշ���  
//	pwa->m_F_P_F = FALSE;  // ��������ǲ���track_stop ���һ����track_start  TRUE == ��ʾ  ��  FLASE = ��ʾ����

   if( pts->theInfo.status == AEEGPS_ERR_NO_ERR 
      || (pts->theInfo.status == AEEGPS_ERR_INFO_UNAVAIL && pts->theInfo.fValid) ) 
   {
	   pwa->m_bGetGPSF = FALSE;//������Ǵ�ʱ��ȡGPS��״̬  TRUE -- ��ʾ��ȡ�쳣 FLASE -- ��ȡ����
	   ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"GPS data theinfo.fValid 1 = %x",pts->theInfo.fValid);
//		pts->theInfo.fValid = AEEGPS_VALID_MASK;
//	   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPS data theinfo.fValid 1 %x",pts->theInfo.fValid);

#if MIN_BREW_VERSION(2,1)
      pts->pResp->lat = WGS84_TO_DEGREES( pts->theInfo.dwLat );
      pts->pResp->lon = WGS84_TO_DEGREES( pts->theInfo.dwLon );

	  pwa->lat = pts->pResp->lat;
	  pwa->lon = pts->pResp->lon;

#else
      double    wgsFactor;
      wgsFactor = FASSIGN_STR("186413.5111");
      pts->pResp->lat = FASSIGN_INT(pts->theInfo.dwLat);
      pts->pResp->lat = FDIV(pts->pResp->lat, wgsFactor);

      pts->pResp->lon = FASSIGN_INT(pts->theInfo.dwLon);
      pts->pResp->lon = FDIV(pts->pResp->lon, wgsFactor);

#endif /* MIN_BREW_VERSION 2.1 */

	  	  ////////// ����ӵ�  ///////////
	  if  (pts->theInfo.fValid|AEEGPS_VALID_HVEL )
	  {
		  pts->pResp->dVelHor = pts->theInfo.wVelocityHor;	// (��λ0.25 meters/second )
		  pts->pResp->dVelVer = pts->theInfo.bVelocityVer;	// (��λ0.5 meters/second )
	  }
	  else
	  {
		  ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"��Чˮƽ�ٶ�");
		  pts->pResp->dVelHor = 0;				
		  pts->pResp->dVelVer = 0;
	  }
	  pwa->dVelHor = pts->pResp->dVelHor;  
	  pwa->dVelVer = pts->pResp->dVelVer;
	  ////////////////////////////////



	  ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"pts->pResp->dVelHor = %d",pwa->dVelHor);
	  ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPS accuracy is %d",pwa->m_F_P_V);

      pts->pResp->height = pts->theInfo.wAltitude - 500;

      pts->pResp->dwFixNum++;

	  pts->pResp->nErr = pts->theInfo.status;  // ����Ӧ�������ģ����滻���������Ǿ� Ŀ���Ǽ���1289����
//      pts->pResp->nErr = SUCCESS;  // ����Ӧ�������ģ����滻���������Ǿ� Ŀ���Ǽ���1289����
//	  pts->pResp->nErr = WEB_ERROR_CONNECT;

	  ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"Get GPS data nalmal pts->theInfo.status = %x",pts->theInfo.status);
      Track_Notify( pts );
	  pwa->m_Fix_C++; //����ͳ�ƻ�ȡ�����ܶ�λ��  ���ڻ�ȡ����һ�鷢�����ݺ������ϴ�


	  si_Ret = Judge_GPS_Mode(pts->m_pParent);
	   if (si_Ret == 0)
	   {
		   if (pts->m_pParent->m_F_M == 1)
		   {
			   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"The current positioning cycle pts->nTrackInterval 5 = %d",pts->nTrackInterval);
			   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"platform positioning report period pts->iInterval 5 = %d",pts->m_pParent->iInterval);
			   if (pts->nTrackInterval !=  pts->m_pParent->iInterval)
			   {
				   uf_RecLog(pts->m_pParent," change main mode posdet cycle 4");
				   pts->nTrackInterval = pts->m_pParent->iInterval;
			   }
		   }
		   else
		   {
			   if (pts->nTrackInterval !=  pts->m_pParent->iAssInterval)
			   {
				   uf_RecLog(pts->m_pParent," change polling mode posdet cycle ");
				   pts->nTrackInterval = pts->m_pParent->iAssInterval;
			   }
		   }
	   }
	   else
	   {
		   uf_RecLog(pts->m_pParent," change main mode posdet cycle 1");
		   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"The current positioning cycle pts->nTrackInterval = %d",pts->nTrackInterval);
		   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"platform positioning report period pts->iInterval 6 = %d",pts->m_pParent->iInterval);
		  if (pts->nTrackInterval != pwa->iInterval)
		  {
			  uf_RecLog(pts->m_pParent," change main mode posdet cycle 2");
			  pts->nTrackInterval = pwa->iInterval; //���������ʱ����ٴ���Ӧ  ����Ϊ��λ
		  }
	   }


/*
	  if (pts->nTrackInterval != pwa->iInterval)
	  {
		  pts->nTrackInterval = pwa->iInterval; //���������ʱ����ٴ���Ӧ  ����Ϊ��λ
	  }
*/	  

	  ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_NAL_GPS,0, 0);	 //��Ա�����ӵ�һ���¼�
//	  ISHELL_SetTimer(pwa->a.m_pIShell, 50, (PFNNOTIFY) SendGPSData, (uint32*) pwa);
      if( (!pts->nPendingFixes || --pts->nPendingFixes > 0) 
         && FALSE == pts->bSetForCancellation ) {

         if( pts->bModeAuto && pts->bModeLocal == FALSE ) {

            /* Try with local first */  
	//		ISHELL_SetTimer(pwa->a.m_pIShell, 50, (PFNNOTIFY) SendGPSData, (uint32*) pwa);
		//	myiweb_Start(pMe,TEST_URL,0);
            Track_Local( pts );
//			ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"ģʽ�� 1");
         }
	//	ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"��������  1");
         ISHELL_SetTimerEx( pts->pShell, pts->nTrackInterval * 1000, &pts->cbIntervalTimer ); // �����������ʱ�ģ���һ�λ�ȡGPSֵ
      }
      else {
	//	 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"NMASK_SHELL_INIT,mInt_StartRun=%d",pMe->mInt_StartRun);
//		 ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"ֹͣ  1");
         Track_Stop( pts );
      }

	  ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPS data theinfoEx.status 1 = %x",pts->theinfoEx.method);

	  if (pwa->m_PosMode) // ֻ���ô�����
	  {
		  // pMe->m_Acc
//		  if (pwa->m_F_P_V < 5)  // ���ҵĽ��ȵȼ��� 5
//		  ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"pwa->m_Acc two = %x",pwa->m_Acc);
		  if (pwa->m_F_P_V < pwa->m_Acc)  // ���ҵĽ��ȵȼ��� 5
		  {
			  pwa->m_F_P_V++;
		//	  pwa->m_F_Pos = FALSE;
		  }
	  }
	  else
	  {

		  if (pwa->m_F_P_F && pwa->m_Q_P ) // ���ٶ�λ ��һ��
		  {
			 pwa->m_F_G_C = FALSE;
			 pwa->m_Q_P = FALSE;
			 pwa->m_F_P_V = /*3*/pwa->m_min_Accuracy;
			 pwa->m_POS_M = pwa->m_gpsFlag;// ��ȡ��ǰ�ֻ��ܶ�λ����ģʽ
			 pwa->m_NOMAINPOS = 0;

//			 IPOSDET_ExtractPositionInfo(pts->pPos,&pts->theInfo,&pts->theinfoEx);
			 if (pwa->m_gpsFlag == 5)
			 {
				 IPOSDET_ExtractPositionInfo(pts->pPos,&pts->theInfo,&pts->theinfoEx);
				 ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPS data theinfoEx.status 2 = %x",pts->theinfoEx.method);
			 }

			 ISHELL_SetTimer(pwa->a.m_pIShell, 100, (PFNNOTIFY)uf_Pos_Track_stopstart_has, pwa);  // �л�����ģʽ 

//			 ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GETTIMESECONDS %ld",sl_tmp);
			 pwa->m_F_START = FALSE; //��������ǲ��ǲŷ���Track_Start   TRUE --  ��ʾ�շ���      FALSE -- ���ǲ��Ǹշ���  
			 pwa->m_F_P_F = FALSE;  // ��������ǲ���track_stop ���һ����track_start  TRUE == ��ʾ  ��  FLASE = ��ʾ����


			 pwa->m_FAIL_T = 0;
			 return ;
		  }

		  if (pwa->m_gpsFlag == 5)
		   {
			   IPOSDET_ExtractPositionInfo(pts->pPos,&pts->theInfo,&pts->theinfoEx);
			   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPS data theinfoEx.status 3 = %x",pts->theinfoEx.method);
		   }

		  if (pwa->m_NOMAINPOS == 1) //��ʾ�Ƿ���Ҫ����ѯģʽ�¶�λ����ʱ���л�����ģʽ  == 0 ����Ҫ  == 1 ����Ҫ)
		  {
			  pwa->m_F_G_C = 0;
			  // ��ȡ�ɹ�֮����߾���
//			  if (pwa->m_F_P_V < 5)  // ���ҵĽ��ȵȼ��� 5  
//			  ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"pwa->m_Acc one = %x",pwa->m_Acc);
			  if (pwa->m_F_P_V < pwa->m_Acc)  // ���ҵĽ��ȵȼ��� 5  
			  {
				  pwa->m_F_P_V++;
			//	  pwa->m_F_Pos = FALSE;
			  }

			  
		  }
		  else if (pwa->m_NOMAINPOS == 0) //˵������Ҫ����ѯģʽ�л�����ģʽ��ʱ����  �պõ�һ�ζ�λ��֮��ͻ��������
		  {
			     pwa->m_F_G_C = 0;
				 pwa->m_F_P_V = /*3*/pwa->m_min_Accuracy;
				 pwa->m_POS_M = pwa->m_gpsFlag;// ��ȡ��ǰ�ֻ��ܶ�λ����ģʽ
				 uf_RecLog(pwa,"back to the main mode");

				ISHELL_SetTimer(pwa->a.m_pIShell, 100, (PFNNOTIFY)uf_Pos_Track_stopstart_has, pwa);  // �л�ģʽ 		 
		  }

	  }

	  	pwa->m_F_START = FALSE; //��������ǲ��ǲŷ���Track_Start   TRUE --  ��ʾ�շ���      FALSE -- ���ǲ��Ǹշ���  
		pwa->m_F_P_F = FALSE;  // ��������ǲ���track_stop ���һ����track_start  TRUE == ��ʾ  ��  FLASE = ��ʾ����
	

		pwa->m_FAIL_T = 0;
   }
   else 
   {  // ���� 0x100004��ʱ�� ���������
//		struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData( pwa );
//		pGetGPSInfo->wProgress = 0;

	   uint32 sl_tmp = 0;
	   sl_tmp = GETTIMESECONDS();

	   // m_FAIL_T; //������Ž�����ȡGPS�쳣�����
//	   if (pwa->m_FAIL_T < 1)
//	   {
//		   pwa->m_FAIL_T = sl_tmp;
//	   }
//	   else
//	   {
/*
		   if (sl_tmp - pwa->m_FAIL_T > 120)  // ˵������2���� �����ʱ���ֹͣ��ȡGPS ��ֹͣ 2���Ӻ�ʼ��ȡGPS
		   {
			   newgimis_CloseGPS_tmp(pwa);
			   ISHELL_SetTimer(pwa->a.m_pIShell, 300000, (PFNNOTIFY)GPS_Restart_fun,pwa);
//			   ISHELL_SetTimer(pwa->a.m_pIShell, 20000, (PFNNOTIFY)GPS_Restart_fun,pwa);  // �о�ʱ�����˷���������  �� 100004 �ܾö�û������
			   return ;
		   }
*/
//	   }

//	   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GETTIMESECONDS %ld",sl_tmp);

	   /*
	    108[2015-09-28 23:40:13]:GETTIMEMS 85213565
		109[2015-09-28 23:40:13]:GETTIMESECONDS 1127518813   31536000

		112[2015-09-28 23:40:18]:GETTIMEMS 85218141
		113[2015-09-28 23:40:18]:GETTIMESECONDS 1127518818

		116[2015-09-28 23:40:21]:GETTIMEMS 85221526
		117[2015-09-28 23:40:21]:GETTIMESECONDS 1127518821

		120[2015-09-28 23:40:24]:GETTIMEMS 85224946
		121[2015-09-28 23:40:24]:GETTIMESECONDS 1127518824

		124[2015-09-28 23:40:28]:GETTIMEMS 85228299
		125[2015-09-28 23:40:28]:GETTIMESECONDS 1127518828
	   */
			
	   // ��¼��־    pts->theInfo.status
//	   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"ʧ�ܽ���һ��  %s",pwa->lx_lat);
	   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPSģʽ״̬ %d",pwa->m_PosMode);
	   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPS mode status %d",pwa->m_PosMode);
	   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPS accuracy is %d",pwa->m_F_P_V);
	   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPS data theInfo.status %x",pts->theInfo.status);
	   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPS data theinfo.fValid %x",pts->theInfo.fValid);
	   SPRINTF(pwa->lx_lat,"0x%x",pts->theInfo.status);
	   SPRINTF(pwa->lx_lon,"0x%x",pts->theInfo.status);
	   SPRINTF(pwa->lx_vv,"0");
	   pwa->m_Error = pts->theInfo.status;

	   pwa->m_T_E_C++; // ����ͳ���������� ��ȡʧ�ܼ��� ���������������ͣ��ȡ����ͣʱ��Ϊ200�룬Ȼ�������ȡ ����������������ϴ����������

	   if (pwa->m_gpsFlag == 5)
	   {
		   IPOSDET_ExtractPositionInfo(pts->pPos,&pts->theInfo,&pts->theinfoEx);
		   ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPS data theinfoEx.status 4 %x",pts->theinfoEx.method);
	   }
	   

	   


//	   if (pwa->m_T_E_C > 2)
//	   {
//		   	ISHELL_SetTimer(pwa->a.m_pIShell, 200000, (PFNNOTIFY)GPS_Restart_fun,pts);
//			IPOSDET_Release_fun(pwa);
//			return ;
//	   }

	   /*
	   pts->pResp->nErr = SUCCESS;
//	   pts->pResp->nErr = WEB_ERROR_CONNECT;

       Track_Notify( pts );
	   */

	   if (0x100001 == pts->theInfo.status) // ˵�����ֵ��� 100001�Ĵ��� ֱ�������ϱ�
	   {
		   uf_RecLog( pwa , "send error gps data 3");

		   ISHELL_SetTimer(pwa->a.m_pIShell, pwa->iInterval*1000, (PFNNOTIFY) Track_cbInterval, (uint32*) pts);

		   ISHELL_SetTimer(pwa->a.m_pIShell, pwa->iInterval*1000, (PFNNOTIFY) SendErrorGPSData_0, (uint32*) pwa);
		   
		   //Track_cbInterval( pts );

		   return;
	   }

	   

	   if (pwa->m_PosMode) // ֻ���ô�����
	   {

		   	   pts->pResp->nErr = SUCCESS;
		//	   pts->pResp->nErr = WEB_ERROR_CONNECT;

		    Track_Notify( pts );

		    if (pwa->m_F_P_V > /*1*//*3*/pwa->m_min_Accuracy)  // ���ҵĽ��ȵȼ��� 5
			{
			  pwa->m_F_P_V--;
			//	  pwa->m_F_Pos = FALSE;
			}

			pwa->m_gpsFlag = 5;
			if (/*!pwa->m_F_QF*/1) // �ֻ�û��Ƿ�� 20170613�޸� �ڷ�����ͷ�ж�
			{
				uf_RecLog( pwa , "send error gps data 4");
				ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_ERR_GPS,0, 0);	 //��Ա�����ӵ�һ���¼�
//				ISHELL_SetTimer(pwa->a.m_pIShell, 1050, (PFNNOTIFY) SendErrorGPSData, (uint32*) pwa);
			}

//		    uf_RecLog(pwa,"ֻ�Ǵ����ǽ���");
			uf_RecLog(pwa,"Only the pure in satellite mode");
		   
			ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPS mode 1 m_gpsFlag = %d",pwa->m_gpsFlag);

			
		   	ISHELL_SetTimer(pwa->a.m_pIShell, 10000, (PFNNOTIFY)uf_Pos_Track_stopstart_error, pwa);  // �л�ģʽ

			if (pwa->m_WitchPag == 5) //˵������ʾGPSҳ��
			{
				testgps_DrawScreen( pwa, (uint32)GETGPSINFO_PAINT_ALL );
			}

//			if (pwa->m_WitchPag == 10)
			if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))
			{
				// XTRA_EVT_USER_PRINT
				ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
			}

	   }
	   else
	   {
		   
	   // һ������ 100004�Ĵ��� �����Ͻ��Ͷ�λ����
		   
		   
		   pwa->m_bGetGPSF = TRUE;//������Ǵ�ʱ��ȡGPS��״̬  TRUE -- ��ʾ��ȡ�쳣 FLASE -- ��ȡ����

		   
/*
		   if (pwa->m_F_P_F == FALSE && pwa->m_Q_P == FALSE) // ���ٶ�λ ��һ��
		   {

			   if (pwa->m_NOMAINPOS == 0) //˵������Ҫ����ѯģʽ�л�����ģʽ��ʱ����
			   {
				 pwa->m_F_G_C = 0;
				 pwa->m_F_P_V = 1;
				 pwa->m_POS_M = pwa->m_gpsFlag;// ��ȡ��ǰ�ֻ��ܶ�λ����ģʽ
				 uf_RecLog(pwa,"back to the main mode 1");
				 ud_RecLog(pwa->a.m_pIShell,1,&(pwa->nInd),"GPS mode 2 m_gpsFlag = %d",pwa->m_gpsFlag);
				 ISHELL_SetTimer(pwa->a.m_pIShell, 100, (PFNNOTIFY)uf_Pos_Track_stopstart_has, pwa);  // �л�ģʽ
				 return;
			   }
		   }
*/
		   
		   if (pwa->m_F_M == 1) //������ģʽ�� ˵������ģʽ�µ�ʱ������˱� 100004�Ĵ��� ���ʱ��Ҫ�ĳ���ѯģʽ
		   {
			   pwa->m_F_M = 0;
			   pwa->m_NOMAINPOS = 1;
			   pwa->m_gpsFlag = pwa->m_POS_M;

			   /*////////////////////////    start    2015-10-24  ////////////////////////////
			   pts->pResp->nErr = AEEGPS_ERR_GENERAL_FAILURE;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_TIMEOUT;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_ACCURACY_UNAVAIL;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_INFO_UNAVAIL;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_PRIVACY_REFUSED;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_SRV_UNREACHABLE;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_LINK_FAILED;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_REJECTED;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_OUTOF_RESOURCES;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_STALE_BS_INFO;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_RECEIVER_BUSY;
				Track_Notify( pts );

			   ///////////////////////////////   END   /////////////////////////////////////*/
			   // ��ʼ��ʱ  10 ����  10����֮�� ��Ҫ�ٴ��л��� ��ģʽ���ܷ��ȡ��GPSֵ
//			   uf_RecLog(pwa,"����ʱ�䶨ʱ ��ʼ��ʱ 1");



			   ISHELL_SetTimer(pwa->a.m_pIShell, 180000, (PFNNOTIFY)uf_Pos_Track_stopstart_time, pwa);  // �л�ģʽ
		   }
		   else  // ˵���Ѿ�����ѯģʽ����
		   {
			   si_Ret = Judge_GPS_Mode(pwa);
			   if (si_Ret == 0)
			   {
				   pwa->m_gpsFlag++;
			   }
			   
		   }

			
	//		pMe->m_gpsFlag;		// �������GPS��ģʽѡ��  0-����  1- local 2-network
			

			if(pwa->m_gpsFlag == 6)
			{
				pwa->m_gpsFlag = 0;
			}

			pwa->m_F_P_V = /*3*/pwa->m_min_Accuracy;

			pwa->m_F_G_C++;

			
//			pts->pResp->nErr = SUCCESS;
//	   pts->pResp->nErr = WEB_ERROR_CONNECT;
			pts->pResp->nErr = pts->theInfo.status;
			Track_Notify( pts );

/*			if (pwa->m_gpsFlag == 5 || pwa->m_gpsFlag == 3)
			{
//				IPOSDET_Release_fun(pwa);
				pts->pResp->nErr = AEEGPS_ERR_GENERAL_FAILURE;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_TIMEOUT;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_ACCURACY_UNAVAIL;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_INFO_UNAVAIL;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_PRIVACY_REFUSED;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_SRV_UNREACHABLE;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_LINK_FAILED;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_REJECTED;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_OUTOF_RESOURCES;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_STALE_BS_INFO;
				Track_Notify( pts );

				pts->pResp->nErr = AEEGPS_ERR_RECEIVER_BUSY;
				Track_Notify( pts );


/*
#define  AEEGPS_ERR_GENERAL_FAILURE    (AEEGPS_ERR_BASE + 1)
#define  AEEGPS_ERR_TIMEOUT            (AEEGPS_ERR_BASE + 2)
#define  AEEGPS_ERR_ACCURACY_UNAVAIL   (AEEGPS_ERR_BASE + 3)
#define  AEEGPS_ERR_INFO_UNAVAIL       (AEEGPS_ERR_BASE + 4)
#define  AEEGPS_ERR_PRIVACY_REFUSED    (AEEGPS_ERR_BASE + 5)
#define  AEEGPS_ERR_SRV_UNREACHABLE    (AEEGPS_ERR_BASE + 6)
#define  AEEGPS_ERR_LINK_FAILED        (AEEGPS_ERR_BASE + 7)
#define  AEEGPS_ERR_REJECTED           (AEEGPS_ERR_BASE + 8)
#define  AEEGPS_ERR_OUTOF_RESOURCES    (AEEGPS_ERR_BASE + 9)
#define  AEEGPS_ERR_STALE_BS_INFO      (AEEGPS_ERR_BASE + 10)
#define  AEEGPS_ERR_RECEIVER_BUSY      (AEEGPS_ERR_BASE + 11)

			}
*/	   
			/////////////////////////////////////  add 2015-11-4 start ////////////////////////////////////////
			ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"GPS mode 3 m_gpsFlag = %d",pwa->m_gpsFlag);
			if (pwa->m_FAIL_T < 1)
			{
			   pwa->m_FAIL_T = sl_tmp;
			}
			else
			{

				si_Ret = Judge_GPS_Mode( pwa );
				if (si_Ret == 0)
				{
					if (sl_tmp - pwa->m_FAIL_T > 1200)  // ˵������2���� �����ʱ���ֹͣ��ȡGPS ��ֹͣ 2���Ӻ�ʼ��ȡGPS
				   {
					  ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"newgimis_CloseGPS_tmp in");
					  newgimis_CloseGPS_tmp(pwa);
					  ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"GPS_Restart_fun in");
					  ISHELL_SetTimer(pwa->a.m_pIShell, 30000, (PFNNOTIFY)GPS_Restart_fun,pwa); 
					  return ;
				   }
				}
			   

			}
	   
			
			/////////////////////////////////////         end          ////////////////////////////////////////
			
			si_Ret = Judge_GPS_Mode(pwa);
			if (si_Ret == 1)
			{
				si_Ret = Get_GPS_Mode(pwa);
				if (si_Ret == 3 /*|| si_Ret == 0*/) // ˵���Ǵ���վ /*���ߴ�����*/
				{
					ISHELL_SetTimer(pwa->a.m_pIShell, 30000, (PFNNOTIFY)uf_Pos_Track_stopstart_error, pwa);  // �л�ģʽ
				}
				else
				{
					ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY)uf_Pos_Track_stopstart_error, pwa);  // �л�ģʽ
				}
			}
			else
			{
				ISHELL_SetTimer(pwa->a.m_pIShell, 5000, (PFNNOTIFY)uf_Pos_Track_stopstart_error, pwa);  // �л�ģʽ
			}

//			ISHELL_SetTimer(pwa->a.m_pIShell, 3000, (PFNNOTIFY)uf_Pos_Track_stopstart_error, pwa);  // �л�ģʽ
			if (pwa->m_F_G_C >= 1)
			{
				// sl_tmp = GETTIMESECONDS();
				JulianType dDate;
				GETJULIANDATE(0,&dDate);
				
				sl_tmp = GETTIMESECONDS();
				if (pwa->m_GPS_E_T == 0)
				{
					pwa->m_GPS_E_T = sl_tmp;
					pwa->m_F_G_C = 0;
					uf_RecLog( pwa , "send error gps data 5A");
					ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_ERR_GPS,0, 0);	 //��Ա�����ӵ�һ���¼�
//					ISHELL_SetTimer(pwa->a.m_pIShell, 1050, (PFNNOTIFY) SendErrorGPSData, (uint32*) pwa);	
				}
				else
				{
					// m_GPS_E_T
					if (pwa->m_GPS_E_T > sl_tmp) // ˵���պ��������ϵ��ٽ�㴦
					{
						pwa->m_GPS_E_T = sl_tmp;
					}
					else if (sl_tmp - pwa->m_GPS_E_T > pwa->iInterval )  // pMe->iInterval  > 15
					{
						pwa->m_GPS_E_T = sl_tmp; 
						// ���� ����GPS
						pwa->m_F_G_C = 0;
						uf_RecLog( pwa , "send error gps data 5C");
						ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_ERR_GPS,0, 0);	 //��Ա�����ӵ�һ���¼�
//						ISHELL_SetTimer(pwa->a.m_pIShell, 1050, (PFNNOTIFY) SendErrorGPSData, (uint32*) pwa);	
					}
						
				}

//				pwa->m_F_G_C = 0;
//				uf_RecLog( pwa , "send error gps data 5");
//				ISHELL_SetTimer(pwa->a.m_pIShell, 7050, (PFNNOTIFY) SendErrorGPSData, (uint32*) pwa);	
			}
			
			

			if (pwa->m_WitchPag == 5) //˵������ʾGPSҳ��
			{
				testgps_DrawScreen( pwa, (uint32)GETGPSINFO_PAINT_ALL );
			}

//			if (pwa->m_WitchPag == 10)
			if ((pwa->m_WitchPag) == 10 && (pwa->m_F_DJ != 1))	
			{
				// XTRA_EVT_USER_PRINT
				ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
			}
/*
			switch(pwa->m_gpsFlag) 
			{
			case 0:
			//	Track_Standalone( pts );
			//	pwa->m_gpsFlag = 1;
				Track_Network( pts );			
				break;
			case 1:
				Track_Optmal(pts); //�ֻ���ѡ			
				break;
			case 2:
				Track_Local( pts );
			//	pwa->m_gpsFlag = 2;
				break;
			case 3:
				Track_One_Shot( pts ); //����վ		
			//	pwa->m_gpsFlag = 0;
				break;
			case 4:
				Track_DLOAD_FIRST( pts ); // ����
				break;	
			case 5:
				Track_Standalone( pts );  // ������
				break;
			}

			if (pwa->m_WitchPag == 5) //˵������ʾGPSҳ��
			{
				testgps_DrawScreen( pwa, (uint32)GETGPSINFO_PAINT_ALL );
			}

			if (pwa->m_WitchPag == 0)
			{
				// XTRA_EVT_USER_PRINT
				ISHELL_PostEvent( pwa->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
			}
			Track_cbInterval( pts );
*/	
	   }

	   	pwa->m_F_START = FALSE; //��������ǲ��ǲŷ���Track_Start   TRUE --  ��ʾ�շ���      FALSE -- ���ǲ��Ǹշ���  
		pwa->m_F_P_F = FALSE;  // ��������ǲ���track_stop ���һ����track_start  TRUE == ��ʾ  ��  FLASE = ��ʾ����
   }
}

/*======================================================================= 
Function: Track_Init()

Description: 
   Creates and initializes a handle for tracking.

Prototype:

   int Track_Init( IShell *pIShell, IPosDet *pIPos, AEECallback *pcb, TrackState **po )

Parameters:
   pIShell: [in]. IShell instance.
   pIPos: [in]. IPosDet instance.
   pcb: [in]. A persistent callback invoked on a position response.
   po: [out]. Handle to internal object.

Return Value:

   SUCCESS
   EBADPARM - One or more of the Invalid arguments
   ENOMEMORY - When system is out of memory.
 
Comments:  
   Invoke the CALLBACK_Cancel( pcb ) to destroy TrackState object.

Side Effects: 
   None

See Also:
   None
=======================================================================*/
int Track_Init( newgimis *pwe, IPosDet *pIPos, AEECallback *pcb, TrackState **po )
{
   int nErr = SUCCESS;
   TrackState *pts = NULL;

   IShell *pIShell = NULL;		// Add by ---- 2012-05-10
   pIShell = pwe->a.m_pIShell;
   

   if( !pIShell || !pIPos || !pcb || !po ) {

      nErr = EBADPARM;

   }
   else if( NULL == (pts = MALLOC( sizeof(TrackState) )) ){

      nErr = ENOMEMORY;

   }
   else {

      ZEROAT( pts );
	  pts->m_pParent = pwe;   // ����Ǻ������ȥ��	
      pts->pShell = pIShell;
      ISHELL_AddRef( pIShell );

      pts->pPos = pIPos;
      IPOSDET_AddRef( pIPos );

      /* Install the notification cb */
      CALLBACK_Cancel( pcb );
      pts->pcbResp = pcb;
      pts->pcbResp->pfnCancel   = Track_Cancel;
      pts->pcbResp->pCancelData = pts;

      CALLBACK_Init( &pts->cbIntervalTimer, Track_cbInterval, pts ); // // ���ռ���Ļص�����
      CALLBACK_Init( &pts->cbInfo, Track_cbInfo, pts );					// �յ����ݺ�Ļص�����

   }

   *po = pts;
   return nErr;

}

/*======================================================================= 
Function: Track_Stop()

Description: 
   Stops the tracking, does not clean up the object, it can be
   further used with Track_Start. Only CALLBACK_Cancel(pcb) releases
   the object.

Prototype:

   int Track_Stop( TrackState *pts );

Parameters:
   pts: [in]. TrackState object created using Track_Init().

Return Value:

   SUCCESS
   EBADPARM - One or more of the Invalid arguments
 
Comments:  
   Invoke the CALLBACK_Cancel( pcb ) to destroy TrackState object.

Side Effects: 
   None

See Also:
   Track_Init()
=======================================================================*/
int Track_Stop( TrackState *pts )
{
   if( !pts ) {
      return EBADPARM;
   }

   pts->bInProgress = FALSE;

   /* Kill any ongoing process */
   CALLBACK_Cancel( &pts->cbInfo );
   CALLBACK_Cancel( &pts->cbIntervalTimer );

   /* Report that Tracking is halted */
   pts->pResp->nErr = EIDLE;
 //  pts->pResp->nErr = SUCCESS;
 //  pts->pResp->nErr = WEB_ERROR_CONNECT;
   
   Track_Notify( pts );

   if( TRUE == pts->bSetForCancellation ) {

      Track_Cancel( pts->pcbResp );
   }
   
   return SUCCESS;
}

/*======================================================================= 
Function: Track_Start()

Description: 
   Starts the tracking using the object created in Track_Init().

Prototype:

   int Track_Start( TrackState *pts, TrackType t, int nFixes, int nInterval, PositionData *pData );

Parameters:
   pts: [in]. TrackState object created using Track_Init().
   t: [in]. Type of tracking.
   nFixes: [in]. Number of fixes.
   nInterval: [in]. Interval between fixes in seconds.
   pData: [in]. Memory in which the Position response is to be filled.

Return Value:

   SUCCESS
   EBADPARM - One or more of the Invalid arguments
   EUNSUPPORTED - Unimplemented
   ENOMEMORY - When system is out of memory.
   EFAILED - General failure.
   EALREADY - When tracking is already in progress.
 
Comments:  
   Invoke the CALLBACK_Cancel( pcb ) to destroy TrackState object.

Side Effects: 
   None

See Also:
   Track_Init()
=======================================================================*/
// �ɵģ�ע�͵���//

int Track_Start( TrackState *pts, TrackType t, int nFixes, 
                   int nInterval, PositionData *pData )
{

	
   int nErr = SUCCESS;
	DBGPRINTF("come in ��������������������������������������������");
   if( !pts || !pData ) {

      nErr = EBADPARM;
   }
   else if( TRUE == pts->bInProgress ) {

      nErr = EALREADY;
   }
   else {

      AEEGPSConfig config;

      pData->dwFixNum     = 0;

      pts->pResp          = pData;
      pts->nPendingFixes  = nFixes;
      pts->nTrackInterval = nInterval; //���������ʱ����ٴ���Ӧ  ����Ϊ��λ

      pts->bModeAuto      = FALSE;
      pts->bModeLocal     = FALSE;

	  MEMSET(&config, 0, sizeof(AEEGPSConfig));
      nErr = IPOSDET_GetGPSConfig( pts->pPos, &config );

	  if (nErr  != SUCCESS )
	  {
		uf_RecLog(pts->m_pParent," Track_Start IPOSDET_GetGPSConfig fail");
	  }

	  /*
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig AEEGPSMode = %d",config.mode);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig nFixes = %d",config.nFixes);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig nInterval = %d",config.nInterval);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig optim = %d",config.optim);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig qos = %d",config.qos);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig AEEGPSServer server AEEGPSSvrType = %d",config.server.svrType);

	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig AEEGPSServer server INAddr = %d",config.server.svr.ipsvr.addr);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig AEEGPSServer server INPort = %d",config.server.svr.ipsvr.port);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig AEEGPSServer server uint16 = %d",config.server.svr.ipsvr.reserved);

      */
      /* Configure the IPosDet Instance */

      if( t == TRACK_NETWORK ) {

		  //  AEEGPS_MODE_TRACK_STANDALONE
	//	 config.mode = AEEGPS_MODE_TRACK_STANDALONE;
         config.mode = AEEGPS_MODE_TRACK_NETWORK;
    //     pts->bModeLocal = TRUE;
      }
      else if( t == TRACK_OPTIMAL ){
		  
		  config.mode = AEEGPS_MODE_TRACK_OPTIMAL;
      }
      else if( t == TRACK_LOCAL ){
		  
		  config.mode = AEEGPS_MODE_TRACK_LOCAL;
      }
	  else if( t == ONE_SHOT ){
		  
		  config.mode = AEEGPS_MODE_ONE_SHOT;
      }
	  else if( t == TRACK_FIRST ){
		  
		  config.mode = AEEGPS_MODE_DLOAD_FIRST;
      }
	  else if( t == TRACK_STANDALONE ){
		  
		  config.mode = AEEGPS_MODE_TRACK_STANDALONE;
      }


      config.nFixes = nFixes;
      config.nInterval = nInterval;  // �����������ʱ������������

      // ADDING SUPPORT FOR USER-DEFINED PDE IP ADDRESS AND QOS
      config.server = pData->server;
  //    config.optim = pData->optim;
	  config.optim = AEEGPS_OPT_SPEED;
      config.qos = pData->qos;

      nErr = IPOSDET_SetGPSConfig( pts->pPos, &config );
	  if (nErr  != SUCCESS )
	  {
		uf_RecLog(pts->m_pParent," Track_Start IPOSDET_SetGPSConfig fail");
	  }

      if( nErr == EUNSUPPORTED && pts->bModeAuto ) {

		  

         /* As TRACK_LOCAL is unsupported on certain devices. If this is auto mode 
         ** and we tried to track locally, change it network based tracking. */

         pts->bModeAuto = FALSE;
         pts->bModeLocal = FALSE;

         config.mode = AEEGPS_MODE_TRACK_NETWORK;

         nErr = IPOSDET_SetGPSConfig( pts->pPos, &config );



	  /*
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig AEEGPSMode = %d",config.mode);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig nFixes = %d",config.nFixes);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig nInterval = %d",config.nInterval);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig optim = %d",config.optim);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig qos = %d",config.qos);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig AEEGPSServer server AEEGPSSvrType = %d",config.server.svrType);

	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig AEEGPSServer server INAddr = %d",config.server.svr.ipsvr.addr);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig AEEGPSServer server INPort = %d",config.server.svr.ipsvr.port);
	  ud_RecLog(pts->m_pParent->a.m_pIShell,1,&(pts->m_pParent->nInd),"AEEGPSConfig AEEGPSServer server uint16 = %d",config.server.svr.ipsvr.reserved);

      */
      }
//	  uf_RecLog(pts->m_pParent,"������ 1");

      if( nErr == SUCCESS ) {

//		  uf_RecLog(pts->m_pParent,"������ 2");
         pts->bInProgress    = TRUE;

         Track_cbInterval( pts );
      }
//	  uf_RecLog(pts->m_pParent,"������ 3");

   }
   return nErr;
}




int Track_ReConfig( TrackState *pts )
{
	int si_Err = 0;
	AEEGPSConfig config;
	MEMSET(&config, 0, sizeof(AEEGPSConfig));
	si_Err = IPOSDET_GetGPSConfig( pts->pPos, &config );
	if (si_Err != SUCCESS)
	{
		uf_RecLog(pts->m_pParent," Track_ReConfig IPOSDET_GetGPSConfig fail");
	}


	config.mode = 1;
	config.nFixes = 1;
    config.nInterval = 1;  // �����������ʱ������������

      // ADDING SUPPORT FOR USER-DEFINED PDE IP ADDRESS AND QOS
    config.optim = 0;
    config.qos = 20;

	si_Err = IPOSDET_SetGPSConfig( pts->pPos, &config );

	if (si_Err != SUCCESS)
	{
		uf_RecLog(pts->m_pParent," Track_ReConfig IPOSDET_SetGPSConfig fail");
	}

	return si_Err;
}


// Track_cbInterval_copy  GPS��λֹͣ��ʼ  ��������ڳ�ʱ��ʱ��
void Track_cbInterval_copy(void * pMe)
{
	int nErr = 0;
	TrackState *pts = (TrackState *)( pMe );
	
	newgimis *pwa = pts->m_pParent;
	uint32 si_tmp_t = 0;
	
//	ud_RecLog(pts->m_pParent->a.m_pIShell,pts->m_pParent->cWrtLog,&(pts->m_pParent->nInd),"Track_cbInterval_copy pwa = %p",pts->m_pParent);
//	ud_RecLog(pts->m_pParent->a.m_pIShell,pts->m_pParent->cWrtLog,&(pts->m_pParent->nInd),"Track_cbInterval_copy pwa = %p",pwa);
	if (pwa->m_MUTEX == 0)
	{
		si_tmp_t = GETTIMESECONDS();
//		pwa->m_T_H_G = si_tmp_t;
//		ud_RecLog(pts->m_pParent->a.m_pIShell,pts->m_pParent->cWrtLog,&(pts->m_pParent->nInd),"Track_cbInterval_copy m_T_H_G = %d",pwa->m_T_H_G);

		Track_cbInterval(  pts );
		return;
	}

//	uf_RecLog(pwa,"Track_cbInterval_copy 1");
	ISHELL_SetTimer(pwa->a.m_pIShell, 2000, (PFNNOTIFY)TimetoTrack_cbInterval,pts);
	return;
	
}


void Track_Notify_Copy(void *pts)
{
	Track_Notify( (TrackState *)pts );
}



// ��ʱ������������GPS ��λ
void TimetoTrack_cbInterval(void *pts)
{
	TrackState *s_pts = (TrackState *)pts;
	newgimis *pMe = s_pts->m_pParent;
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


/*
void GPS_Restart_fun(void *pts)
{
	TrackState *pts_ = (TrackState *)pts;
	newgimis *pwa = pts_->m_pParent;
	pts_->pResp->nErr = pts_->theInfo.status;
	Track_Notify( pts );

	ud_RecLog(pwa->a.m_pIShell,pwa->cWrtLog,&(pwa->nInd),"GPS_Restart_fun restart pos = %d",pwa->m_gpsFlag);
	ISHELL_SetTimer(pwa->a.m_pIShell, 100, (PFNNOTIFY)uf_Pos_Track_stopstart_error, pwa);  // �л�ģʽ
	return;
}

  */
