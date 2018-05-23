/*======================================================
FILE:  SP_GetGPSInfo.c

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "newgimis.h"
#include "SP_Track.h"
#include "gmTools.h"  // ¼ÓÕâ¸öÍ·ÎÄ¼þÖ»ÊÇÎªÁËÈÕÖ¾

/*===========================================================================
                G E T G P S I N F O   S C R E E N
===========================================================================*/
/*
typedef enum {
   GETGPSINFO_PAINT_ALL = 0,
   GETGPSINFO_PAINT_FIXCOUNT,
   GETGPSINFO_PAINT_FIXDATA,
   GETGPSINFO_PAINT_FIXANIM,
   GETGPSINFO_PAINT_ERROR
} GetGPSInfo_PaintRegions;
*/

struct _GetGPSInfo {
   PositionData theInfo;
   IPosDet      *pPosDet;
   AEECallback  cbPosDet;
   AEECallback  cbProgressTimer;
   uint32       dwFixNumber;
   uint32       dwFixDuration;
   uint32       dwFail;
   uint32       dwTimeout;
   uint16       wProgress;
   boolean      bPaused;
   uint16       wMainMenuEntry;
   boolean      bAbort;
   TrackState   *pts;
};

/*===========================================================================
===========================================================================*/




static void Track_Standalone( TrackState *pts ) // ´¿ÎÀÐÇ
{
   AEEGPSConfig config;
   int si_nErr = 0;

   DBGPRINTF( "TRACK NETWORK" );
   MEMSET(&config, 0, sizeof(AEEGPSConfig));
   


   si_nErr = IPOSDET_GetGPSConfig( pts->pPos, &config );
   if (si_nErr  != SUCCESS )
   {
	   uf_RecLog(pts->m_pParent," Track_Standalone IPOSDET_GetGPSConfig fail");
   }

   config.server = pts->m_pParent->gpsSettings.server;
   config.qos = pts->m_pParent->gpsSettings.qos;
   config.optim = pts->m_pParent->gpsSettings.optim;
   config.mode = AEEGPS_MODE_TRACK_STANDALONE;

   si_nErr = IPOSDET_SetGPSConfig( pts->pPos, &config );
   if (si_nErr  != SUCCESS )
   {
	   uf_RecLog(pts->m_pParent," Track_Standalone IPOSDET_SetGPSConfig fail");
   }

   pts->bModeLocal = FALSE;
}

static void Track_One_Shot( TrackState *pts ) //´¿»ùÕ¾
{
   AEEGPSConfig config;
   int si_nErr = 0;

   DBGPRINTF( "TRACK NETWORK" );
   MEMSET(&config, 0, sizeof(AEEGPSConfig));

   
   si_nErr = IPOSDET_GetGPSConfig( pts->pPos, &config );
   if (si_nErr  != SUCCESS )
   {
	   uf_RecLog(pts->m_pParent," Track_One_Shot IPOSDET_GetGPSConfig fail");
   }


   si_nErr = Judge_GPS_Mode(pts->m_pParent);
   if (si_nErr == 0)
   {
	   if (pts->m_pParent->m_F_M == 1)
	   {
		   
		   if (pts->nTrackInterval !=  pts->m_pParent->iInterval)
		   {
			   uf_RecLog(pts->m_pParent,"Track_One_Shot change main mode posdet cycle ");
			   pts->nTrackInterval = pts->m_pParent->iInterval;
		   }
	   }
	   else
	   {
		   if (pts->nTrackInterval !=  pts->m_pParent->iAssInterval)
		   {
			   uf_RecLog(pts->m_pParent,"Track_One_Shot change polling mode posdet cycle ");
			   pts->nTrackInterval = pts->m_pParent->iAssInterval;
		   }
	   }
   }
   /////////////////////////////////////              END                    //////////////////////////////////

   
   config.server = pts->m_pParent->gpsSettings.server;
   config.qos = pts->m_pParent->gpsSettings.qos;
   config.optim = pts->m_pParent->gpsSettings.optim;
   
	//	  config.mode = AEEGPS_MODE_TRACK_NETWORK;
         config.mode = AEEGPS_MODE_ONE_SHOT;  // Õâ¸öÒ²»¹¿ÉÒÔ µ«ÊÇ¾õµÃÃ»ÓÐ OPTIMAL µÄºÃ


//   config.mode = AEEGPS_MODE_TRACK_STANDALONE;

   si_nErr = IPOSDET_SetGPSConfig( pts->pPos, &config );
   if (si_nErr  != SUCCESS )
   {
	   uf_RecLog(pts->m_pParent," Track_One_Shot IPOSDET_GetGPSConfig fail");
   }

   pts->bModeLocal = FALSE;
}



static void Track_Network( TrackState *pts )  //´¿ÍøÂç
{
   AEEGPSConfig config;
   int si_nErr = 0;

   DBGPRINTF( "TRACK NETWORK" );

	MEMSET(&config, 0, sizeof(AEEGPSConfig));
   
   si_nErr = IPOSDET_GetGPSConfig( pts->pPos, &config );
   if (si_nErr  != SUCCESS )
   {
	   uf_RecLog(pts->m_pParent," Track_Network IPOSDET_GetGPSConfig fail");
   }
   
   config.server = pts->m_pParent->gpsSettings.server;
   config.qos = pts->m_pParent->gpsSettings.qos;
   config.optim = pts->m_pParent->gpsSettings.optim;
   config.mode = AEEGPS_MODE_TRACK_NETWORK;

   si_nErr = IPOSDET_SetGPSConfig( pts->pPos, &config );
   if (si_nErr  != SUCCESS )
   {
	   uf_RecLog(pts->m_pParent," Track_Network IPOSDET_GetGPSConfig fail");
   }

   pts->bModeLocal = FALSE;
}

// AEEGPS_MODE_DLOAD_FIRST

static void Track_DLOAD_FIRST( TrackState *pts )  //Ê×ÏÈ
{
   AEEGPSConfig config;
   int si_nErr = 0;

   DBGPRINTF( "TRACK NETWORK" );


   MEMSET(&config, 0, sizeof(AEEGPSConfig));
   si_nErr = IPOSDET_GetGPSConfig( pts->pPos, &config );
   if (si_nErr  != SUCCESS )
   {
	   uf_RecLog(pts->m_pParent," Track_DLOAD_FIRST IPOSDET_GetGPSConfig fail");
   }

   config.server = pts->m_pParent->gpsSettings.server;
   config.qos = pts->m_pParent->gpsSettings.qos;
   config.optim = pts->m_pParent->gpsSettings.optim;
 //  config.mode = AEEGPS_MODE_DLOAD_FIRST;
   config.mode = AEEGPS_MODE_TRACK_NETWORK;

   si_nErr =  IPOSDET_SetGPSConfig( pts->pPos, &config );
   if (si_nErr  != SUCCESS )
   {
	   uf_RecLog(pts->m_pParent," Track_DLOAD_FIRST IPOSDET_GetGPSConfig fail");
   }

   pts->bModeLocal = FALSE;
}


static void Track_Local( TrackState *pts )  //»ùÕ¾¸¨Öú
{
   AEEGPSConfig config;
   int si_nErr = 0;

   DBGPRINTF( "TRACK LOCAL" );


	MEMSET(&config, 0, sizeof(AEEGPSConfig));
   si_nErr = IPOSDET_GetGPSConfig( pts->pPos, &config );
   if (si_nErr  != SUCCESS )
   {
	   uf_RecLog(pts->m_pParent," Track_Local IPOSDET_GetGPSConfig fail");
   }

   // ÒÔÏÂ´úÂëÌí¼ÓÈç¹ûÊÇÎª»ìºÏÄ£Ê½£¬ÇÒ²»ÊÇÖ÷Ä£Ê½µÄÊ±ºò Ôò½«¶¨Î»ÖÜÆÚÇÐ»»³É¸¨Öú¶¨Î»ÖÜÆÚ  START   ////////////////
   //		pts->nTrackInterval = nInterval; //Õâ¸ö»áÆðµ½ÑÓÊ±¶à¾ÃÔÙ´ÎÏìÓ¦  ÒÔÃëÎªµ¥Î»
   //		pMe->iInterval ²É¼¯ÖÜÆÚ
   //		pMe->iAssInterval ¸¨Öú¶¨Î»Ê±¼ä
   si_nErr = Judge_GPS_Mode(pts->m_pParent);
   if (si_nErr == 0)
   {
	   if (pts->m_pParent->m_F_M == 1)
	   {
		   
		   if (pts->nTrackInterval !=  pts->m_pParent->iInterval)
		   {
			   uf_RecLog(pts->m_pParent,"change main mode posdet cycle  3");
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
   /////////////////////////////////////              END                    //////////////////////////////////

   config.server = pts->m_pParent->gpsSettings.server;
   config.qos = pts->m_pParent->gpsSettings.qos;
   config.optim = pts->m_pParent->gpsSettings.optim;
   config.mode = AEEGPS_MODE_TRACK_LOCAL;

   si_nErr = IPOSDET_SetGPSConfig( pts->pPos, &config );
   if (si_nErr  != SUCCESS )
   {
	   uf_RecLog(pts->m_pParent," Track_Local IPOSDET_GetGPSConfig fail");
   }

   pts->bModeLocal = TRUE;
}

static void Track_Optmal( TrackState *pts )  //ÊÖ»ú×ÔÑ¡
{
   AEEGPSConfig config;
   int si_nErr = 0;

   DBGPRINTF( "TRACK LOCAL" );
   MEMSET(&config, 0, sizeof(AEEGPSConfig));


   si_nErr = IPOSDET_GetGPSConfig( pts->pPos, &config );
   if (si_nErr  != SUCCESS )
   {
	   uf_RecLog(pts->m_pParent," Track_Optmal IPOSDET_GetGPSConfig fail");
   }
	  
   config.server = pts->m_pParent->gpsSettings.server;
   config.qos = pts->m_pParent->gpsSettings.qos;
   config.optim = pts->m_pParent->gpsSettings.optim;
   config.mode = AEEGPS_MODE_TRACK_OPTIMAL;

   si_nErr = IPOSDET_SetGPSConfig( pts->pPos, &config );
   if (si_nErr  != SUCCESS )
   {
	   uf_RecLog(pts->m_pParent," Track_Optmal IPOSDET_GetGPSConfig fail");
   }

   pts->bModeLocal = TRUE;
}




/*===========================================================================
ÊÕµ½GPSÊý¾ÝºóµÄ»Øµ÷º¯Êý
===========================================================================*/
static void testgps_GetGPSInfo_Callback( newgimis *pMe )
{
   struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );
   GetGPSInfo_PaintRegions rgn = GETGPSINFO_PAINT_FIXDATA;

	// ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "½ÓÊÕµ½Êý¾Ý½øÀ´  1007");

   while (pGetGPSInfo == 0)
   {
	   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "GetGPSInfo pointer null 1");
	   pGetGPSInfo = testgps_GetScreenData_gps( pMe );
   }
//   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "Ö¸Õë»ñÈ¡µ½ 1");
   if( pGetGPSInfo->theInfo.nErr == SUCCESS ) {
      /* Process new data from IPosDet */
      pGetGPSInfo->dwFixNumber++;
      pGetGPSInfo->dwFixDuration += pGetGPSInfo->wProgress;
      pGetGPSInfo->wProgress = 0;
   }
   else if( pGetGPSInfo->theInfo.nErr == EIDLE ) {
      /* End of tracking */
      CALLBACK_Cancel( &pGetGPSInfo->cbProgressTimer );
   }
   else if( pGetGPSInfo->theInfo.nErr == AEEGPS_ERR_TIMEOUT ) {
      /* Record the timeout and perhaps re-try. */
      pGetGPSInfo->dwTimeout++;
      rgn = GETGPSINFO_PAINT_FIXCOUNT;
   }
   else {
      /* Something is not right here. Requires corrective action. Bailout */
      pGetGPSInfo->bAbort = TRUE;
      rgn = GETGPSINFO_PAINT_ERROR;
   }

 //  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"×´Ì¬£º%d  ",pMe->m_FOBFlag);
//	   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Ò³Êý:%d",pMe->m_WitchPag);
   if (!pMe->m_FOBFlag && pMe->m_WitchPag == 5)
   {
//	   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ÓÐÃ»ÓÐ½øÀ´ 1");
		testgps_DrawScreen( pMe, (uint32)rgn );
   }
   
}

/*===========================================================================
===========================================================================*/
/*===========================================================================
ÊÕµ½GPSÖ®Ç°»Øµ÷µÄ´ÎÊý£¬Ïàµ±ÓÚÊ±¼äµÄÁ÷ÊÅ
===========================================================================*/
static void testgps_GetGPSInfo_SecondTicker( newgimis *pMe )
{
	int si_Ret = 0;
   struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );

   while (pGetGPSInfo == 0)
   {
	   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "GetGPSInfo pointer null 2");
	   pGetGPSInfo = testgps_GetScreenData_gps( pMe );
   }
//   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "Ö¸Õë»ñÈ¡µ½ 2");

   si_Ret = Judge_GPS_Mode(pMe);
   if (si_Ret == 1)
   {
	   if( pGetGPSInfo->bPaused == FALSE ) 
	   {
		  pGetGPSInfo->wProgress++;
		  if (!pMe->m_FOBFlag && pMe->m_WitchPag == 5)
		  {
	//		  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ÓÐÃ»ÓÐ½øÀ´ 2");
			  testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_FIXANIM );
		  }
	   }

	   if( pGetGPSInfo->bAbort == FALSE ) 
	   {
	//	   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"×´Ì¬11£º%d  ",pMe->m_FOBFlag);
	//	   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Ò³Êý11:%d",pMe->m_WitchPag);
		   ISHELL_SetTimerEx( pMe->a.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
		   if (!pMe->m_FOBFlag && pMe->m_WitchPag == 5)
			{
	//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ÓÐÃ»ÓÐ½øÀ´ 2");
	//			ISHELL_SetTimerEx( pMe->a.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
		   }
	   }
	   return;

   }


//	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Ê±¼äÁ÷ÊÅ 1");
   if( pGetGPSInfo->bPaused == FALSE ) 
   {
      pGetGPSInfo->wProgress++;
	  if (!pMe->m_FOBFlag && pMe->m_WitchPag == 5)
	  {
//		  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ÓÐÃ»ÓÐ½øÀ´ 2");
		  testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_FIXANIM );
	  }



	  /*
	  	//////  ÒÔÏÂÊÇ¹ØÓÚGPSÓÅ»¯µÄÎÊÌâ START ////////
		int m_F_G_C; //ÓÃÀ´Í³¼ÆÓÐÃ»ÓÐÂÖÑ¯ÍêÒ»±é  Èç¹ûÓÐÔò ±¨Ò»´Î 1000004  m_F_G_C == 6 ÔòËµÃ÷ÂÖÑ¯ÍêÒ»±é  Èç¹û»ñÈ¡µ½ÁË  ÔòÖØÐÂ Çå 0
		int m_F_M; //ÓÃÀ´±ê¼ÇÔÚÊ²Ã´Ä£Ê½ÏÂ   µÈÓÚ 0 ÔòÎªÔÚÂÖÑ¯µÄÇé¿öÏÂ  Èç¹ûÎª 1 ÔòÔÚÆ½Ì¨ÏÂ·¢µÄÄ£Ê½ÏÂ 
		int m_F_P_V; //ÓÃÀ´±ê¼Ç¶¨Î»¾«¶È  AEEGPS_ACCURACY_LEVEL1 -- 1  µ½ AEEGPS_ACCURACY_LEVEL6 -- 6 £¨1-6´ú±íµÈ¼¶¾«¶È£©
		boolean m_F_Pos;  // ÓÃÀ´±ê¼ÇµÈ¼¶ÊÇ·ñÒª¼Ó Èç¹û¼ÓÍêÖ®ºó ÓÖ±ä³É²»Ôö¼ÓµÈ¼¶ TRUE == ±íÊ¾Ôö¼ÓµÈ¼¶  FLASE == ±íÊ¾²»Ôö¼ÓµÈ¼¶
		boolean m_F_P_F ;// ÓÃÀ´±ê¼ÇÊÇ²»ÊÇtrack_stop ºóµÚÒ»´ÎÓÃtrack_start  TRUE == ±íÊ¾  ÊÇ  FLASE = ±íÊ¾²»ÊÇ
		boolean m_Q_P; //ÓÃÀ´±ê¼ÇÊÇ·ñÊÇ³ÌÐò¿ªÆôÊ±¿ìËÙ¶¨Î»  TRUE -- ÊÇ  FLASE -- ²»ÊÇ
		int m_POS_M;// »ñÈ¡µ±Ç°ÊÖ»úÄÜ¶¨Î»µ½µÄÄ£Ê½
		int m_NOMAINPOS; //±íÊ¾ÊÇ·ñÐèÒªÔÚÂÖÑ¯Ä£Ê½ÏÂ¶¨Î»µ½µÄÊ±ºòÇÐ»»µ½Ö÷Ä£Ê½  == 0 ÔòÐèÒª  == 1 Ôò²»ÐèÒª
		boolean m_PosMode ;// ÓÃÀ´±ê¼ÇÊÇ·ñÊÇÖ»ÄÜÓÃ´¿ÎÀÐÇ¶¨Î» TRUE -- ±íÊ¾Ö»ÄÜÓÃ´¿ÎÀÐÇ  FLASH -- ²»ÊÇ »¹¿ÉÒÔ±ðµÄ·½Ê½
		///////          END        //////////////////
	  */

	  if (pMe->m_F_P_F) // TRUE
	  {
	//	  if (pMe->m_Q_P) // ´¦ÓÚ¿ìËÙ¶¨Î»
	//	  {
			  switch (pMe->m_gpsFlag)
			  {
			  case 0:
			  case 1:
			  case 2:
			  case 3:
			  case 4:
				  if (pGetGPSInfo->wProgress > 40) // ´óÓÚ40Ãë ÔòÄ¬ÈÏÊÕ²»µ½  
				  {


					  pGetGPSInfo->pts->pResp->nErr = AEEGPS_ERR_TIMEOUT;
					  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Obtain the GPS data more than 40 seconds long  pts->pResp->nErr = %d",pGetGPSInfo->pts->theInfo.status);
					  Track_Notify_Copy(pGetGPSInfo->pts);


					  pGetGPSInfo->wProgress = 0;

					  ////////////////  
						if (pMe->m_F_M == 1) //ÔòÔÚÖ÷Ä£Ê½ÏÂ ËµÃ÷ÔÚÖ÷Ä£Ê½ÏÂµÄÊ±ºò³öÏÖÁË±¨ 100004µÄ´íÎó Õâ¸öÊ±ºòÒª¸Ä³ÉÂÖÑ¯Ä£Ê½
						{
						   pMe->m_F_G_C = 0;
						   pMe->m_F_M = 0;
						   pMe->m_NOMAINPOS = 1;
						   pMe->m_gpsFlag = pMe->m_POS_M;
						   uf_Pos_Track_stopstart( pMe);
						   
						   uf_RecLog(pMe,"Locate more than 40 seconds");
						   // ¿ªÊ¼¶¨Ê±  10 ·ÖÖÓ  10·ÖÖÓÖ®ºó ¾ÍÒªÔÙ´ÎÇÐ»»µ½ Ö÷Ä£Ê½¿´ÄÜ·ñ»ñÈ¡µ½GPSÖµ
			//			   uf_RecLog(pMe,"½øÈëÊ±¼ä¶¨Ê± ¿ªÊ¼¶¨Ê± 2");
						   ISHELL_SetTimer(pMe->a.m_pIShell, 180000, (PFNNOTIFY)uf_Pos_Track_stopstart_time, pMe);  // ÇÐ»»Ä£Ê½
						}
					  ////////////////
						
						else
						{
						  pMe->m_F_G_C++;
						  pMe->m_gpsFlag++;
						  if (pMe->m_gpsFlag == 6)
						  {
							  pMe->m_gpsFlag = 0;
						  }
						  uf_Pos_Track_stopstart( pMe);
						  
							
						}
						if (pMe->m_WitchPag == 5) //ËµÃ÷ÔÚÏÔÊ¾GPSÒ³Ãæ
						{
							testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ALL );
						}

//						if (pMe->m_WitchPag == 10 )
						if ((pMe->m_WitchPag) == 10 && (pMe->m_F_DJ != 1))
						{
							// XTRA_EVT_USER_PRINT
							ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
						}
						
				  }
				  break;
			  case 5:
				  if (pGetGPSInfo->wProgress > 20) // ´óÓÚ20Ãë ÔòÄ¬ÈÏÊÕ²»µ½  ´¿ÎÀÐÇ¶¨Î»
				  {
					  pGetGPSInfo->pts->pResp->nErr = AEEGPS_ERR_TIMEOUT;
					  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Obtain the GPS data more than 20 seconds long  pts->pResp->nErr time out %d",pGetGPSInfo->pts->theInfo.status);

					  Track_Notify_Copy(pGetGPSInfo->pts);

					  pGetGPSInfo->wProgress = 0;

					  ////////////////  
					if (pMe->m_F_M == 1) //ÔòÔÚÖ÷Ä£Ê½ÏÂ ËµÃ÷ÔÚÖ÷Ä£Ê½ÏÂµÄÊ±ºò³öÏÖÁË±¨ 100004µÄ´íÎó Õâ¸öÊ±ºòÒª¸Ä³ÉÂÖÑ¯Ä£Ê½
					{
					   pMe->m_F_G_C = 0;
					   pMe->m_F_M = 0;
					   pMe->m_NOMAINPOS = 1;
					   pMe->m_gpsFlag = pMe->m_POS_M;
					   uf_RecLog(pMe,"Locate more than 20 seconds");
					   uf_Pos_Track_stopstart( pMe);
					   // ¿ªÊ¼¶¨Ê±  10 ·ÖÖÓ  10·ÖÖÓÖ®ºó ¾ÍÒªÔÙ´ÎÇÐ»»µ½ Ö÷Ä£Ê½¿´ÄÜ·ñ»ñÈ¡µ½GPSÖµ
			//		   uf_RecLog(pMe,"½øÈëÊ±¼ä¶¨Ê± ¿ªÊ¼¶¨Ê± 3");
					   ISHELL_SetTimer(pMe->a.m_pIShell, 180000, (PFNNOTIFY)uf_Pos_Track_stopstart_time, pMe);  // ÇÐ»»Ä£Ê½
					}
				  ////////////////
					
					else
					{

					  pMe->m_F_G_C++;
					  pMe->m_gpsFlag++;
					  if (pMe->m_gpsFlag == 6)
					  {
						  pMe->m_gpsFlag = 0;
					  }
					  uf_Pos_Track_stopstart( pMe);
					}
					if (pMe->m_WitchPag == 5) //ËµÃ÷ÔÚÏÔÊ¾GPSÒ³Ãæ
					{
						testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ALL );
					}

//					if (pMe->m_WitchPag == 10)
					if ((pMe->m_WitchPag) == 10 && (pMe->m_F_DJ != 1))
					{
						// XTRA_EVT_USER_PRINT
						ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
					}
				  }
				  break;
			  }

			  if (pMe->m_F_G_C >= 1)
			  {
				uint32 sl_tmp = 0;

				SPRINTF(pMe->lx_lat,"0x100004");
			    SPRINTF(pMe->lx_lon,"0x100004");
				SPRINTF(pMe->lx_vv,"0");
				
				sl_tmp = GETTIMESECONDS();
				if (pMe->m_GPS_E_T == 0)
				{
					pMe->m_GPS_E_T = sl_tmp;
					pMe->m_F_G_C = 0;
					uf_RecLog( pMe , "send error gps data 1");
					ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_ERR_GPS,0, 0);	 //Õë¶Ô±±¶·Ìí¼ÓµÄÒ»¸öÊÂ¼þ	
	//				ISHELL_PostEvent( pwa->a.m_pIShell,pwa->a.clsID, XTRA_EVT_USER_QQSQ_CG,0, 0);	 //ÇëÇóÊÚÈ¨³É¹¦µÄ·¢ËÍÊÂ¼þ	
	//				ISHELL_SetTimer(pMe->a.m_pIShell, 1050, (PFNNOTIFY) SendErrorGPSData, (uint32*) pMe);	
				}
				else
				{
					// m_GPS_E_T
					if (pMe->m_GPS_E_T > sl_tmp) // ËµÃ÷¸ÕºÃÊÇÔÚÍíÉÏµÄÁÙ½çµã´¦
					{
						pMe->m_GPS_E_T = sl_tmp;
					}
					else if (sl_tmp - pMe->m_GPS_E_T > pMe->iInterval )  // pMe->iInterval  > 15
					{
						pMe->m_GPS_E_T = sl_tmp; 
						// ·¢ËÍ ´íÎóGPS
						pMe->m_F_G_C = 0;
						uf_RecLog( pMe , "send error gps data 1A");
						ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_ERR_GPS,0, 0);	 //Õë¶Ô±±¶·Ìí¼ÓµÄÒ»¸öÊÂ¼þ
//						ISHELL_SetTimer(pMe->a.m_pIShell, 1050, (PFNNOTIFY) SendErrorGPSData, (uint32*) pMe);	
					}
						
				}

/*
				  pMe->m_F_G_C = 0;
				  SPRINTF(pMe->lx_lat,"0x100004");
			      SPRINTF(pMe->lx_lon,"0x100004");
				  SPRINTF(pMe->lx_vv,"0");
				  uf_RecLog( pMe , "send error gps data 1");
				  ISHELL_SetTimer(pMe->a.m_pIShell, 1050, (PFNNOTIFY) SendErrorGPSData, (uint32*) pMe);	
*/
			  }
			  
	//	  }
	  }
	  else  // FLASE 
	  {
		  // pwa->m_F_START = FALSE; //ÓÃÀ´±ê¼ÇÊÇ²»ÊÇ²Å·¢ÆðTrack_Start   TRUE --  ±íÊ¾¸Õ·¢Æð      FALSE -- ²»ÊÇ²»ÊÇ¸Õ·¢Æð  
		   switch (pMe->m_gpsFlag)
			  {
			  case 0:
			  case 1:
			  case 2:
			  case 3:
			  case 4:
				  if (pGetGPSInfo->wProgress > (40 + pMe->m_POS_T)) // ´óÓÚ40Ãë ÔòÄ¬ÈÏÊÕ²»µ½  
				  {
					  pGetGPSInfo->pts->pResp->nErr = AEEGPS_ERR_TIMEOUT;
					  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Obtain the GPS data more than 40 seconds long pts->pResp->nErr time out %d",pGetGPSInfo->pts->theInfo.status);

					  Track_Notify_Copy(pGetGPSInfo->pts);

					  pGetGPSInfo->wProgress = 0;

					  ////////////////  
					if (pMe->m_F_M == 1) //ÔòÔÚÖ÷Ä£Ê½ÏÂ ËµÃ÷ÔÚÖ÷Ä£Ê½ÏÂµÄÊ±ºò³öÏÖÁË±¨ 100004µÄ´íÎó Õâ¸öÊ±ºòÒª¸Ä³ÉÂÖÑ¯Ä£Ê½
					{
					   pMe->m_F_G_C = 0;
					   pMe->m_F_M = 0;
					   pMe->m_NOMAINPOS = 1;
					   pMe->m_gpsFlag = pMe->m_POS_M;
					   uf_Pos_Track_stopstart( pMe);
					   uf_RecLog(pMe,"Locate more than 40 + pMe->m_POS_T seconds");
					   // ¿ªÊ¼¶¨Ê±  10 ·ÖÖÓ  10·ÖÖÓÖ®ºó ¾ÍÒªÔÙ´ÎÇÐ»»µ½ Ö÷Ä£Ê½¿´ÄÜ·ñ»ñÈ¡µ½GPSÖµ
			//		   uf_RecLog(pMe,"½øÈëÊ±¼ä¶¨Ê± ¿ªÊ¼¶¨Ê± 4");
					   ISHELL_SetTimer(pMe->a.m_pIShell, 180000, (PFNNOTIFY)uf_Pos_Track_stopstart_time, pMe);  // ÇÐ»»Ä£Ê½
					}
				  ////////////////
					
					else
					{

					  pMe->m_F_G_C++;
					  pMe->m_gpsFlag++;
					  if (pMe->m_gpsFlag == 6)
					  {
						  pMe->m_gpsFlag = 0;
					  }
					  uf_Pos_Track_stopstart( pMe);
					  

					}			
					if (pMe->m_WitchPag == 5) //ËµÃ÷ÔÚÏÔÊ¾GPSÒ³Ãæ
					{
						testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ALL );
					}

//					if (pMe->m_WitchPag == 10)
					if ((pMe->m_WitchPag) == 10 && (pMe->m_F_DJ != 1))
					{
						// XTRA_EVT_USER_PRINT
						ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
					}
				  }
				  break;
			  case 5:
				  if (pGetGPSInfo->wProgress > (20 + pMe->m_POS_T)) // ´óÓÚ20Ãë ÔòÄ¬ÈÏÊÕ²»µ½  ´¿ÎÀÐÇ¶¨Î»
				  {
					  pGetGPSInfo->pts->pResp->nErr = AEEGPS_ERR_TIMEOUT;
					  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Obtain the GPS data more than 40 seconds long pts->pResp->nErr time out %d",pGetGPSInfo->pts->theInfo.status);

					  Track_Notify_Copy(pGetGPSInfo->pts);

					  pGetGPSInfo->wProgress = 0;
					////////////////  
					if (pMe->m_F_M == 1) //ÔòÔÚÖ÷Ä£Ê½ÏÂ ËµÃ÷ÔÚÖ÷Ä£Ê½ÏÂµÄÊ±ºò³öÏÖÁË±¨ 100004µÄ´íÎó Õâ¸öÊ±ºòÒª¸Ä³ÉÂÖÑ¯Ä£Ê½
					{
					   pMe->m_F_G_C = 0;
					   pMe->m_F_M = 0;
					   pMe->m_NOMAINPOS = 1;
					   pMe->m_gpsFlag = pMe->m_POS_M;
						uf_RecLog(pMe,"Locate more than 20 + pMe->m_POS_T seconds");
					   uf_Pos_Track_stopstart( pMe);
					   // ¿ªÊ¼¶¨Ê±  10 ·ÖÖÓ  10·ÖÖÓÖ®ºó ¾ÍÒªÔÙ´ÎÇÐ»»µ½ Ö÷Ä£Ê½¿´ÄÜ·ñ»ñÈ¡µ½GPSÖµ
//					   uf_RecLog(pMe,"½øÈëÊ±¼ä¶¨Ê± ¿ªÊ¼¶¨Ê± 5");

					   ISHELL_SetTimer(pMe->a.m_pIShell, 180000, (PFNNOTIFY)uf_Pos_Track_stopstart_time, pMe);  // ÇÐ»»Ä£Ê½
					}
				  ////////////////
					
					else
					{
						  pMe->m_F_G_C++;
						  pMe->m_gpsFlag++;
						  if (pMe->m_gpsFlag == 6)
						  {
							  pMe->m_gpsFlag = 0;
						  }
						  uf_Pos_Track_stopstart( pMe);
					}
					if (pMe->m_WitchPag == 5) //ËµÃ÷ÔÚÏÔÊ¾GPSÒ³Ãæ
					{
						testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ALL );
					}

//					if (pMe->m_WitchPag == 10)
					if ((pMe->m_WitchPag) == 10 && (pMe->m_F_DJ != 1))
					{
						// XTRA_EVT_USER_PRINT
						ISHELL_PostEvent( pMe->a.m_pIShell, AEECLSID_NEWGIMIS, EVT_SCREEN, XTRA_EVT_USER_PRINT,  0);		
					}
				  }
				  break;
			  }

			  if (pMe->m_F_G_C >= 1)
			  {

				  uint32 sl_tmp = 0;

				SPRINTF(pMe->lx_lat,"0x100004");
			    SPRINTF(pMe->lx_lon,"0x100004");
				SPRINTF(pMe->lx_vv,"0");
				
				sl_tmp = GETTIMESECONDS();
				if (pMe->m_GPS_E_T == 0)
				{
					pMe->m_GPS_E_T = sl_tmp;
					pMe->m_F_G_C = 0;
					uf_RecLog( pMe , "send error gps data 2");
					ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_ERR_GPS,0, 0);	 //Õë¶Ô±±¶·Ìí¼ÓµÄÒ»¸öÊÂ¼þ
//					ISHELL_SetTimer(pMe->a.m_pIShell, 1050, (PFNNOTIFY) SendErrorGPSData, (uint32*) pMe);	
				}
				else
				{
					// m_GPS_E_T
					if (pMe->m_GPS_E_T > sl_tmp) // ËµÃ÷¸ÕºÃÊÇÔÚÍíÉÏµÄÁÙ½çµã´¦
					{
						pMe->m_GPS_E_T = sl_tmp;
					}
					else if (sl_tmp - pMe->m_GPS_E_T > pMe->iInterval )  // pMe->iInterval  > 15
					{
						pMe->m_GPS_E_T = sl_tmp; 
						// ·¢ËÍ ´íÎóGPS
						pMe->m_F_G_C = 0;
						uf_RecLog( pMe , "send error gps data 2");
						ISHELL_PostEvent( pMe->a.m_pIShell,pMe->a.clsID, XTRA_EVT_USER_ERR_GPS,0, 0);	 //Õë¶Ô±±¶·Ìí¼ÓµÄÒ»¸öÊÂ¼þ
//						ISHELL_SetTimer(pMe->a.m_pIShell, 1050, (PFNNOTIFY) SendErrorGPSData, (uint32*) pMe);	
					}
						
				}


/*
				  pMe->m_F_G_C = 0;
				  SPRINTF(pMe->lx_lat,"0x100004");
			      SPRINTF(pMe->lx_lon,"0x100004");
				  SPRINTF(pMe->lx_vv,"0");
				  uf_RecLog( pMe , "send error gps data 2");
				  ISHELL_SetTimer(pMe->a.m_pIShell, 1050, (PFNNOTIFY) SendErrorGPSData, (uint32*) pMe);	
*/
			  }

	  }
      
   }
   if( pGetGPSInfo->bAbort == FALSE ) 
   {
//	   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"×´Ì¬11£º%d  ",pMe->m_FOBFlag);
//	   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Ò³Êý11:%d",pMe->m_WitchPag);
	   ISHELL_SetTimerEx( pMe->a.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
	   if (!pMe->m_FOBFlag && pMe->m_WitchPag == 5)
		{
//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"ÓÐÃ»ÓÐ½øÀ´ 2");
//			ISHELL_SetTimerEx( pMe->a.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
	   }
   }
}

/*===========================================================================
===========================================================================*/
static void testgps_GetGPSInfo_Paint( newgimis *pMe, GetGPSInfo_PaintRegions rgn )
{
   struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );

   while (pGetGPSInfo == 0)
   {
	   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "GetGPSInfo pointer null 3");
	   pGetGPSInfo = testgps_GetScreenData_gps( pMe );
   }
//   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "Ö¸Õë»ñÈ¡µ½ 3");

   if (pMe->m_WitchPag != 1 && pMe->m_FOBFlag == 5)
   {
	   pGetGPSInfo = pMe->Old_ScreenData;
   }

   if( rgn == GETGPSINFO_PAINT_ALL ) {
      IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
	  
	  /*
      testgps_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "GetGPSInfo" );
      if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_GPS ) {
         testgps_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "One Shot" );
      }
      else if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_SEECONFIG ) {
         testgps_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "Track L" );
      }
      else if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_YJKQ ) {
         testgps_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "Track N" );
      }
      else if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_FULL ) {
         testgps_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "Track A" );
      }

     */
   }

   if( rgn == GETGPSINFO_PAINT_FIXCOUNT || rgn == GETGPSINFO_PAINT_ALL ) {
      testgps_Printf( pMe, 1, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Failed : %d", pGetGPSInfo->dwFail+pGetGPSInfo->dwTimeout );
      testgps_Printf( pMe, 1, 4, AEE_FONT_NORMAL, IDF_ALIGN_RIGHT, "Timeout : %d", pGetGPSInfo->dwTimeout );
   }

   if( rgn == GETGPSINFO_PAINT_FIXDATA || rgn == GETGPSINFO_PAINT_ALL ) {
#define MAXTEXTLEN   22
      AECHAR wcText[MAXTEXTLEN];
      char   latlonStr[MAXTEXTLEN];

      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Fixes : %d", pGetGPSInfo->dwFixNumber );

      FLOATTOWSTR( pGetGPSInfo->theInfo.lat, wcText, MAXTEXTLEN * sizeof(AECHAR) );
      WSTR_TO_STR( wcText, latlonStr, MAXTEXTLEN );
      testgps_Printf( pMe, 3, 4, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s d", latlonStr );

      FLOATTOWSTR( pGetGPSInfo->theInfo.lon, wcText, MAXTEXTLEN * sizeof(AECHAR) );
      WSTR_TO_STR( wcText, latlonStr, MAXTEXTLEN );
      testgps_Printf( pMe, 4, 4, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s d", latlonStr );

      testgps_Printf( pMe, 5, 4, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%d m", pGetGPSInfo->theInfo.height );
      {
         double fv;
         fv = FASSIGN_INT( pGetGPSInfo->dwFixDuration );
         if( pGetGPSInfo->dwFixNumber ) {
            fv = FDIV(fv, pGetGPSInfo->dwFixNumber);
         }
         FLOATTOWSTR( fv, wcText, MAXTEXTLEN * sizeof(AECHAR) );
         WSTR_TO_STR( wcText, latlonStr, MAXTEXTLEN );
         testgps_Printf( pMe, 6, 4, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "Avg %ss", latlonStr );   
      }
   }

   if( rgn == GETGPSINFO_PAINT_ERROR ) {
      testgps_Printf( pMe, 6, 4, AEE_FONT_BOLD, IDF_ALIGN_BOTTOM|IDF_ALIGN_CENTER|IDF_TEXT_INVERTED, 
         "0x%x", pGetGPSInfo->theInfo.nErr );  // ABORTED
   }

   if( rgn == GETGPSINFO_PAINT_FIXANIM || rgn == GETGPSINFO_PAINT_ALL ) {
      testgps_Printf( pMe, 7, 4, AEE_FONT_NORMAL, IDF_ALIGN_BOTTOM|IDF_ALIGN_CENTER|IDF_RECT_FILL, 
         ".....%d.....", pGetGPSInfo->wProgress );
   }

}





/*===========================================================================
===========================================================================*/
boolean testgps_GetGPSInfo_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam )
{
//	char *sTmp = NULL;
   boolean bHandled = FALSE;
   int  m_nFontHeight = 0;
   AEECLSID pos_add = 0;
   

   
   struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );


   if (pMe->m_WitchPag != 1 && pMe->m_FOBFlag == 5)
   {
	   pGetGPSInfo = pMe->Old_ScreenData;
   }

   switch( eCode ) {

   case EVT_SCREEN:
      if( wParam == SCREEN_PARAM_INIT ) {
         if( pGetGPSInfo == 0 ) 
		 {
			pGetGPSInfo = MALLOC( sizeof( struct _GetGPSInfo ) );
            if( pGetGPSInfo ) {
			uint16 cPosDetMode = pMe->cPosDetMode;   // ¶¨Î»Ä£Ê½
               int nErr = SUCCESS;
               ZEROAT( pGetGPSInfo );

               pGetGPSInfo->theInfo.server = pMe->gpsSettings.server;
               pGetGPSInfo->theInfo.qos = pMe->gpsSettings.qos;
               pGetGPSInfo->theInfo.optim = pMe->gpsSettings.optim;

               pGetGPSInfo->wMainMenuEntry = (uint16)dwParam;
//			   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "ÉèÖÃ 1");
               testgps_SetScreenData_gps( pMe, (void *)pGetGPSInfo );


// 			   if (STRCMP(pMe->sMobileType,"03") == 0)
// 			   {
// 				   if (pMe->m_gpsFlag == 5)
// 				   {
// 						pos_add = AEECLSID_OEM_APP + 0x113 ;
// 				   }
// 				   else
// 				   {
// 					   pos_add = AEECLSID_POSDET;
// 				   }
// 			   }
// 			   else
// 			   {
// 				   pos_add = AEECLSID_POSDET;
// 			   }

			   pos_add = AEECLSID_POSDET;

			   // AEECLSID_POSDET_312  AEECLSID_POSDET   AEECLSID_OEM_APP + 0x113
			   if( ISHELL_CreateInstance( pMe->a.m_pIShell, pos_add,
                  (void **)&pGetGPSInfo->pPosDet ) == SUCCESS ) {
//               if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_OEM_APP + 0x113,
 //                 (void **)&pGetGPSInfo->pPosDet ) == SUCCESS ) {
//			   if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_POSDET_312,
//                  (void **)&pGetGPSInfo->pPosDet ) == SUCCESS ) {

                  CALLBACK_Init( &pGetGPSInfo->cbPosDet, testgps_GetGPSInfo_Callback, pMe );
                  CALLBACK_Init( &pGetGPSInfo->cbProgressTimer, testgps_GetGPSInfo_SecondTicker, pMe );

                  nErr = Track_Init( pMe, pGetGPSInfo->pPosDet, 
                                        &pGetGPSInfo->cbPosDet, &pGetGPSInfo->pts );

				  ///////////////  7-21 ////////////////////////////
				  // Æô¶¯GPS
				  // ²ÎÊýËµÃ÷  µÚÈý¸ö²ÎÊý ¶¨Î»´ÎÊý Èç¹û Îª n n > 0 Ôò¶¨Î»n´Î  Èç¹ûÊÇ0 ¾ÍÑ­»·¶¨Î»  
				  // µÚËÄ¸ö²ÎÊý ÔòÊÇ¶¨Î»Ê±¼ä Èç¹û Îª 0 ÔòÒ»Ö±¶¨Î»  ±ðµÄÊý×ÖÎªÃ¿¸ö¶àÉÙÃë¶¨Î»Ò»´Î µ¥Î»ÎªÃë
				  
				 
   //               testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ALL );
                  if( nErr != SUCCESS ) { //Èç¹û¿ªÆôGPSÊ§°Ü
                     pGetGPSInfo->theInfo.nErr = nErr;
   //                  testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ERROR );
                  }
                  else {
					  
                     ISHELL_SetTimerEx( pMe->a.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
                  }

                  bHandled = TRUE;
               }
            }
         }
     
      }
      else if( wParam == SCREEN_PARAM_CLOSE ) 
	  {

		  if( pGetGPSInfo ) 
		  {
			  int si_tmp = 0;
			  Track_ReConfig(pGetGPSInfo->pts);
			  if( pGetGPSInfo->pPosDet ) {
				  CALLBACK_Cancel( &pGetGPSInfo->cbProgressTimer );
				  CALLBACK_Cancel( &pGetGPSInfo->cbPosDet );
				  do 
				  {
					  si_tmp = IPOSDET_Release( pGetGPSInfo->pPosDet );
				  } while(si_tmp > 0);
				  
			  }
			  FREE( pGetGPSInfo );

			  
		  }
//		  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "ÉèÖÃ 2");
		  testgps_SetScreenData_gps( pMe, 0 );
		  pMe->currentHandler = 0;

         bHandled = TRUE;
      }
      else if( wParam == SCREEN_PARAM_PAINT ) {
		
         if( !pGetGPSInfo->bPaused ) {
  //          testgps_GetGPSInfo_Paint( pMe, (GetGPSInfo_PaintRegions)dwParam );

         }
		 
         bHandled = TRUE;
      }
	  
      break;

   case EVT_KEY:
	   /*
	   if( wParam == AVK_CLR || wParam == AVK_SELECT ) {

		   pMe->m_WitchPag = 0;
			testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );

		  
         
         bHandled = TRUE;
      }
	   // AVK_POWER   AVK_END
	   if( wParam == AVK_END  ) {
		   
	//	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
		   pMe->m_WitchPag = 1;
		   pMe->m_FOBFlag = TRUE; //ÔÚºóÌ¨ÔËÐÐ
		   uf_RecLog(pMe,"´ÓGPS½øÈëºóÌ¨" );
		   ISHELL_CloseApplet(pMe->a.m_pIShell,FALSE); //×ªºóÌ¨
		   bHandled = TRUE;
      }
	  */
      break;

   case EVT_DIALOG_START:
      pGetGPSInfo->bPaused = TRUE;
      bHandled = TRUE;
      break;

   case EVT_DIALOG_END:
      pGetGPSInfo->bPaused = FALSE;
  //    testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ALL );
      bHandled = TRUE;
      break;

   case EVT_COMMAND:
//	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
         bHandled = TRUE;
      break;
   }

   return bHandled;
}



/////////  ÒÔÏÂÊÇ¿ªÆô ºÍ ¹Ø±Õ GPS ³ÌÐòÆô¶¯µÄÊ±ºòÓÃµ½
void newgimis_OpenGPS(newgimis * pMe)
{
	boolean bHandled = FALSE;
	struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );
	AEECLSID pos_add = 0;

	ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"open gps pMe->cPosDetMode = %d",pMe->cPosDetMode);

	pMe->GPSFlag = TRUE;

	//±±¶·Ä£Ê½ÇÐ»»
	if ( pMe->cPosDetMode == 13 )
	{
		if (pMe->pIgpsext == NULL)
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"first goto beidou mode");
			newgimis_CloseGPS( pMe );
			newgimis_Init_UKBD( pMe );
			newgimis_OpenGPS_UKBD(pMe);
	    	return;
		}
		else
		{
			return;
		}
	}

	//S958GPSÇÐ»»
	if ( pMe->cPosDetMode == 14 )
	{
		if (pMe->pS958gps == NULL)
		{
			ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"first goto S958GPS mode");
			newgimis_CloseGPS_S958GPS(pMe);
			if (newgimis_Init_S958GPS( pMe ))
			{
				newgimis_OpenGPS_S958GPS(pMe);
			}
			return;
		}
		else
		{
			return;
		}
	}
	
	//Í¨ÓÃÄ£Ê½
	if( pGetGPSInfo == 0 ) 
		 {
			pGetGPSInfo = MALLOC( sizeof( struct _GetGPSInfo ) );
            if( pGetGPSInfo ) 
			{
			   uint16 cPosDetMode = pMe->cPosDetMode;   // ¶¨Î»Ä£Ê½
               int nErr = SUCCESS;
               ZEROAT( pGetGPSInfo );

               pGetGPSInfo->theInfo.server = pMe->gpsSettings.server;
               pGetGPSInfo->theInfo.qos = pMe->gpsSettings.qos;
               pGetGPSInfo->theInfo.optim = pMe->gpsSettings.optim;

			   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "create open gps pointer");
               testgps_SetScreenData_gps( pMe, (void *)pGetGPSInfo );

				

// 			   if (STRCMP(pMe->sMobileType,"03") == 0)
// 			   {
// 				   if (pMe->m_gpsFlag == 5)
// 				   {
// 						pos_add = AEECLSID_OEM_APP + 0x113 ;
// 				   }
// 				   else
// 				   {
// 					   pos_add = AEECLSID_POSDET;
// 				   }
// 			   }
// 			   else
// 			   {
// 				   pos_add = AEECLSID_POSDET;
// 			   }
			   
			   pos_add = AEECLSID_POSDET;

			   // AEECLSID_POSDET_312  AEECLSID_POSDET   AEECLSID_OEM_APP + 0x113
               if( ISHELL_CreateInstance( pMe->a.m_pIShell, pos_add,
                  (void **)&pGetGPSInfo->pPosDet ) == SUCCESS ) 
//				if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_OEM_APP + 0x113,
 //               (void **)&pGetGPSInfo->pPosDet ) == SUCCESS ) 
//				if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_POSDET_312,
//                  (void **)&pGetGPSInfo->pPosDet ) == SUCCESS ) 
				  
			   {

                  CALLBACK_Init( &pGetGPSInfo->cbPosDet, testgps_GetGPSInfo_Callback, pMe );
                  CALLBACK_Init( &pGetGPSInfo->cbProgressTimer, testgps_GetGPSInfo_SecondTicker, pMe );

                  nErr = Track_Init( pMe, pGetGPSInfo->pPosDet, 
                                        &pGetGPSInfo->cbPosDet, &pGetGPSInfo->pts );

				  ///////////////  7-21 ////////////////////////////
				  // Æô¶¯GPS
				  // ²ÎÊýËµÃ÷  µÚÈý¸ö²ÎÊý ¶¨Î»´ÎÊý Èç¹û Îª n n > 0 Ôò¶¨Î»n´Î  Èç¹ûÊÇ0 ¾ÍÑ­»·¶¨Î»  
				  // µÚËÄ¸ö²ÎÊý ÔòÊÇ¶¨Î»Ê±¼ä Èç¹û Îª 0 ÔòÒ»Ö±¶¨Î»  ±ðµÄÊý×ÖÎªÃ¿¸ö¶àÉÙÃë¶¨Î»Ò»´Î µ¥Î»ÎªÃë


				 pMe->m_F_P_F = TRUE;// ÓÃÀ´±ê¼ÇÊÇ²»ÊÇtrack_stop ºóµÚÒ»´ÎÓÃtrack_start  TRUE == ±íÊ¾  ÊÇ  FLASE = ±íÊ¾²»ÊÇ
				 pMe->m_Q_P = TRUE; //ÓÃÀ´±ê¼ÇÊÇ·ñÊÇ³ÌÐò¿ªÆôÊ±¿ìËÙ¶¨Î»  TRUE -- ÊÇ  FLASE -- ²»ÊÇ
				 pMe->m_F_P_V = pMe->m_min_Accuracy/*3*/; //×î¿ªÊ¼ÉèÖÃµÈ¼¶×îµÍ

				 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Positioning mode pMe->cPosDetMode = %d",pMe->cPosDetMode);
				 if (pMe->m_PosMode)
				 {
					 nErr = Track_Start( pGetGPSInfo->pts, TRACK_STANDALONE, 0, 10, &pGetGPSInfo->theInfo );
				 }
				 else
				 {
					 if (pMe->cPosDetMode == 0)
					 {
						 pMe->m_gpsFlag = Get_GPS_Mode(pMe);
						 nErr = Track_Start( pGetGPSInfo->pts, ONE_SHOT, 0, pMe->iInterval, &pGetGPSInfo->theInfo );
					 }
					 else if (pMe->cPosDetMode == 1)
					 {
						 pMe->m_gpsFlag = Get_GPS_Mode(pMe);
						 nErr = Track_Start( pGetGPSInfo->pts, TRACK_LOCAL, 0, pMe->iInterval, &pGetGPSInfo->theInfo );
					 }
					 else if (pMe->cPosDetMode == 2)
					 {
						 pMe->m_gpsFlag = Get_GPS_Mode(pMe);
						 nErr = Track_Start( pGetGPSInfo->pts, TRACK_NETWORK, 0, pMe->iInterval, &pGetGPSInfo->theInfo );
					 }
					 else if (pMe->cPosDetMode == 6)
					 {
//						 uf_RecLog(pMe," ½øÀ´ÕâÀï 1113 a ");
						 pMe->m_gpsFlag = Get_GPS_Mode(pMe);
//						 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"pMe->m_gpsFlag = %d",pMe->m_gpsFlag);
						 nErr = Track_Start( pGetGPSInfo->pts, TRACK_STANDALONE, 0, pMe->iInterval, &pGetGPSInfo->theInfo );
					 }
					 else
					 {
						nErr = Track_Start( pGetGPSInfo->pts, TRACK_NETWORK, 0, 10, &pGetGPSInfo->theInfo );	 
					 }
				 }
				 pMe->m_F_START = TRUE; //ÓÃÀ´±ê¼ÇÊÇ²»ÊÇ²Å·¢ÆðTrack_Start   TRUE --  ±íÊ¾¸Õ·¢Æð      FALSE -- ²»ÊÇ²»ÊÇ¸Õ·¢Æð  

				
   //             testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ALL );
                  if( nErr != SUCCESS ) { //Èç¹û¿ªÆôGPSÊ§°Ü
                     pGetGPSInfo->theInfo.nErr = nErr;
   //                  testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ERROR );
                  }
                  else {
//					  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Ê±¼äÁ÷ÊÅ 2");
                     ISHELL_SetTimerEx( pMe->a.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
                  }
			   }
			}
	}
    bHandled = TRUE;
}


void newgimis_CloseGPS(newgimis * pMe)
{
	
	struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );
//   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "Ö¸Õë»ñÈ¡µ½ 6");
	uf_RecLog(pMe,"newgimis_CloseGPS in");
	if (pMe->cPosDetMode == 13)
	{
		pMe->GPSFlag = TRUE;
		pMe->m_gpsFlag = 10 ;	//ÉèÖÃ±±¶·Ä£Ê½£¬ÓÃÓÚÏÔÊ¾ÔÚÊÖ»ú½çÃæ£¬·¢ËÍµ½Æ½Ì¨µÄÎªm_PM_BD
		pMe->m_Fix_C = 0;//¶¨Î»´ÎÊýÇå0
		SPRINTF(pMe->show_LatLon,"0x100001   0x100001");
	}
	else if (pMe->cPosDetMode == 14)
	{
		pMe->GPSFlag = TRUE;
		pMe->m_gpsFlag = 0 ;	//ÉèÖÃ±±¶·Ä£Ê½£¬ÓÃÓÚÏÔÊ¾ÔÚÊÖ»ú½çÃæ£¬·¢ËÍµ½Æ½Ì¨µÄÎªm_PM_BD
		pMe->m_Fix_C = 0;//¶¨Î»´ÎÊýÇå0
		SPRINTF(pMe->show_LatLon,"0x100001   0x100001");
	}
	else
	{
		pMe->GPSFlag = FALSE;
	}
		 
	if( pGetGPSInfo ) {
		int si_tmp = 0;

		Track_ReConfig(pGetGPSInfo->pts);

		if (pGetGPSInfo->pts)
		{
			Track_Stop(pGetGPSInfo->pts);
		}

		if( pGetGPSInfo->pPosDet ) {
			CALLBACK_Cancel( &pGetGPSInfo->cbProgressTimer );
			CALLBACK_Cancel( &pGetGPSInfo->cbPosDet );
			do 
			{
				si_tmp = IPOSDET_Release( pGetGPSInfo->pPosDet );
			} while(si_tmp > 0);
			
		}
		FREE( pGetGPSInfo );
//		uf_RecLog(pMe,"ÊÍ·ÅGPSÄÚ´æ³É¹¦1");
	    testgps_SetScreenData_gps( pMe, 0 );
	}
	//uf_RecLog(pMe,"recevice message 2016-3-7 newgimis_CloseGPS out");
}




// ÁÙÊ±¿ªÆôGPSµÄÊµÏÖº¯Êý
void newgimis_OpenGPS_tmp(newgimis * pMe)
{
	boolean bHandled = FALSE;
	struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );
	AEECLSID pos_add = 0;

	if (pMe->GPSFlag == TRUE)
	{
		if( pGetGPSInfo == 0 ) 
			 {
				pGetGPSInfo = MALLOC( sizeof( struct _GetGPSInfo ) );
				if( pGetGPSInfo ) 
				{
				   uint16 cPosDetMode = pMe->cPosDetMode;   // ¶¨Î»Ä£Ê½
				   int nErr = SUCCESS;
				   ZEROAT( pGetGPSInfo );

				   pGetGPSInfo->theInfo.server = pMe->gpsSettings.server;
				   pGetGPSInfo->theInfo.qos = pMe->gpsSettings.qos;
				   pGetGPSInfo->theInfo.optim = pMe->gpsSettings.optim;

				   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "create open gps pointer 1");
				   testgps_SetScreenData_gps( pMe, (void *)pGetGPSInfo );



// 			   if (STRCMP(pMe->sMobileType,"03") == 0)
// 			   {
// 				   if (pMe->m_gpsFlag == 5)
// 				   {
// 						pos_add = AEECLSID_OEM_APP + 0x113 ;
// 				   }
// 				   else
// 				   {
// 					   pos_add = AEECLSID_POSDET;
// 				   }
// 			   }
// 			   else
// 			   {
// 				   pos_add = AEECLSID_POSDET;
// 			   }
				   
			   pos_add = AEECLSID_POSDET;

			   // AEECLSID_POSDET_312  AEECLSID_POSDET   AEECLSID_OEM_APP + 0x113
//				   if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_OEM_APP + 0x113,
 //                 (void **)&pGetGPSInfo->pPosDet ) == SUCCESS ) 

				   if( ISHELL_CreateInstance( pMe->a.m_pIShell, pos_add,
					  (void **)&pGetGPSInfo->pPosDet ) == SUCCESS ) 
//					  if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_POSDET_312,
//					  (void **)&pGetGPSInfo->pPosDet ) == SUCCESS ) 
				
				   {

					  CALLBACK_Init( &pGetGPSInfo->cbPosDet, testgps_GetGPSInfo_Callback, pMe );
					  CALLBACK_Init( &pGetGPSInfo->cbProgressTimer, testgps_GetGPSInfo_SecondTicker, pMe );

					  nErr = Track_Init( pMe, pGetGPSInfo->pPosDet, 
											&pGetGPSInfo->cbPosDet, &pGetGPSInfo->pts );

					  ///////////////  7-21 ////////////////////////////
					  // Æô¶¯GPS
					  // ²ÎÊýËµÃ÷  µÚÈý¸ö²ÎÊý ¶¨Î»´ÎÊý Èç¹û Îª n n > 0 Ôò¶¨Î»n´Î  Èç¹ûÊÇ0 ¾ÍÑ­»·¶¨Î»  
					  // µÚËÄ¸ö²ÎÊý ÔòÊÇ¶¨Î»Ê±¼ä Èç¹û Îª 0 ÔòÒ»Ö±¶¨Î»  ±ðµÄÊý×ÖÎªÃ¿¸ö¶àÉÙÃë¶¨Î»Ò»´Î µ¥Î»ÎªÃë

					 pMe->m_F_P_V = /*3*/pMe->m_min_Accuracy; //×î¿ªÊ¼ÉèÖÃµÈ¼¶×îµÍ

					 nErr = Track_Start( pGetGPSInfo->pts, pMe->m_gpsFlag, 0, pMe->iInterval, &pGetGPSInfo->theInfo );


					 pMe->m_F_START = TRUE; //ÓÃÀ´±ê¼ÇÊÇ²»ÊÇ²Å·¢ÆðTrack_Start   TRUE --  ±íÊ¾¸Õ·¢Æð      FALSE -- ²»ÊÇ²»ÊÇ¸Õ·¢Æð  

					
	   //             testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ALL );
					  if( nErr != SUCCESS ) { //Èç¹û¿ªÆôGPSÊ§°Ü
						 pGetGPSInfo->theInfo.nErr = nErr;
	   //                  testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ERROR );
					  }
					  else {
	//					  ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"Ê±¼äÁ÷ÊÅ 2");
						 ISHELL_SetTimerEx( pMe->a.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
					  }
				   }
				}
		}
	}
    bHandled = TRUE;
}


//  ÁÙÊ±¹Ø±Õ GPSµÄÊµÏÖº¯Êý 
void newgimis_CloseGPS_tmp(newgimis * pMe)
{

	struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );
//   ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "Ö¸Õë»ñÈ¡µ½ 6");

	if( pGetGPSInfo ) {
		int si_tmp = 0;

		Track_ReConfig(pGetGPSInfo->pts);
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
		if (pGetGPSInfo->pts)
		{
			Track_Stop(pGetGPSInfo->pts);
		}
		if( pGetGPSInfo->pPosDet ) {
			CALLBACK_Cancel( &pGetGPSInfo->cbProgressTimer );
			CALLBACK_Cancel( &pGetGPSInfo->cbPosDet );
			do 
			{	
				si_tmp = IPOSDET_Release( pGetGPSInfo->pPosDet );
				ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), " IPOSDET_Release 2 = %d",si_tmp);
			} while(si_tmp > 0);
			
		}
		FREE( pGetGPSInfo );
//		uf_RecLog(pMe,"ÊÍ·ÅGPSÄÚ´æ³É¹¦1");
	    testgps_SetScreenData_gps( pMe, 0 );
	}
}



// uf_Pos_Track_stopstart  GPS¶¨Î»Í£Ö¹ºó¿ªÊ¼  ÓÃÓÚ¶¨Î»³¬Ê±µÄÊ±ºò
void uf_Pos_Track_stopstart(newgimis * pMe)
{
	int nErr = 0;
	struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );

	if (pGetGPSInfo)
	{
		nErr = Judge_GPS_Mode(pMe);
	    if (nErr == 1)
		{
			int si_tmp = 0;
			si_tmp = pMe->m_gpsFlag;
			pMe->m_gpsFlag = Get_GPS_Mode(pMe);
			if (si_tmp == pMe->m_gpsFlag)
			{
				pGetGPSInfo->pts->m_pParent = pMe;
				Track_cbInterval_copy( pGetGPSInfo->pts );
				return ;
			}
			
		}
	
			switch(pMe->m_gpsFlag) 
			{
			case 0:
			//	Track_Standalone( pts );
			//	pwa->m_gpsFlag = 1;
				Track_Network( pGetGPSInfo->pts );			
				break;
			case 1:
				Track_Optmal(pGetGPSInfo->pts); //ÊÖ»ú×ÔÑ¡			
				break;
			case 2:
				Track_Local( pGetGPSInfo->pts );
			//	pwa->m_gpsFlag = 2;
				break;
			case 3:
				Track_One_Shot( pGetGPSInfo->pts ); //´¿»ùÕ¾		
			//	pwa->m_gpsFlag = 0;
				break;
			case 4:
				Track_DLOAD_FIRST( pGetGPSInfo->pts ); // Ê×ÏÈ
				break;	
			case 5:
				Track_Standalone( pGetGPSInfo->pts );  // ´¿ÎÀÐÇ
				break;
			}

			
/*
			if (si_tmp_t > pMe->m_T_H_G)
			{
				if (si_tmp_t - pMe->m_T_H_G > 2) // ËµÃ÷¿ÉÒÔÇëÇóGPS¶¨Î»
				{
				}
				
			}
			else
			{
				if (si_tmp_t > 2) // ËµÃ÷¿ÉÒÔÇëÇóGPS¶¨Î»
				{
				}
				
			}
*/

			pGetGPSInfo->pts->m_pParent = pMe;
			Track_cbInterval_copy( pGetGPSInfo->pts );


			
//			Track_cbInterval( pGetGPSInfo->pts );
	}


/*
	if (pGetGPSInfo)
	{
	
		Track_Stop(pGetGPSInfo->pts);
		pMe->m_F_P_F = TRUE;// ÓÃÀ´±ê¼ÇÊÇ²»ÊÇtrack_stop ºóµÚÒ»´ÎÓÃtrack_start  TRUE == ±íÊ¾  ÊÇ  FLASE = ±íÊ¾²»ÊÇ
		pGetGPSInfo->wProgress = 0;
		if (pMe->iInterval < 5)
		{
			nErr = Track_Start( pGetGPSInfo->pts, pMe->m_gpsFlag, 0, 5, &pGetGPSInfo->theInfo );
			pMe->m_POS_T = 5;
		}
		else
		{
			nErr = Track_Start( pGetGPSInfo->pts, pMe->m_gpsFlag, 0, pMe->iInterval, &pGetGPSInfo->theInfo );
			pMe->m_POS_T = pMe->iInterval;
		}
		
		pMe->m_F_START =TRUE; //ÓÃÀ´±ê¼ÇÊÇ²»ÊÇ²Å·¢ÆðTrack_Start   TRUE --  ±íÊ¾¸Õ·¢Æð      FALSE -- ²»ÊÇ²»ÊÇ¸Õ·¢Æð  

		if( nErr != SUCCESS ) { //Èç¹û¿ªÆôGPSÊ§°Ü
		 pGetGPSInfo->theInfo.nErr = nErr;
		 uf_RecLog(pMe,"GPS reboot fail");
		}
		else {
		 ISHELL_SetTimerEx( pMe->a.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
		}

	}
	*/
}

// Õâ¸öº¯ÊýÊÇÓÃÓÚ¶¨Î»µ½Ö®ºó ÇÐ»»µ½Ö÷Ä£Ê½µÄ
void uf_Pos_Track_stopstart_has(newgimis * pMe)
{
	int nErr = 0;
	
	struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );

	if (pGetGPSInfo)
	{

		
		pMe->m_Q_P = FALSE;
		pMe->m_F_M = 1;
		
		pMe->m_NOMAINPOS = 1;  //±íÊ¾ÊÇ·ñÐèÒªÔÚÂÖÑ¯Ä£Ê½ÏÂ¶¨Î»µ½µÄÊ±ºòÇÐ»»µ½Ö÷Ä£Ê½  == 0 ÔòÐèÒª  == 1 Ôò²»ÐèÒª

		nErr = Judge_GPS_Mode(pMe);
	    if (nErr == 1)
		{
			int si_tmp = 0;
			si_tmp = pMe->m_gpsFlag;
			pMe->m_gpsFlag = Get_GPS_Mode(pMe);
			if (si_tmp == pMe->m_gpsFlag)
			{
				pGetGPSInfo->pts->m_pParent = pMe;
				Track_cbInterval_copy( pGetGPSInfo->pts );
				return ;
			}
			
		}

		switch(pMe->cPosDetMode)
		{
		case 0:
			pMe->m_gpsFlag = 3;
			break;
		case 1:
			pMe->m_gpsFlag = 2;
			break;
		case 2:
			pMe->m_gpsFlag = 0;
			break;
		case 3:
			pMe->m_gpsFlag = 2;
			break;
		case 4:
			pMe->m_gpsFlag = 3;
			break;
		case 5:
			pMe->m_gpsFlag = 1;
			break;
		case 6:
			pMe->m_gpsFlag = 5;
			break;
		case 7:
			pMe->m_gpsFlag = 3;
			break;
		case 8:
			pMe->m_gpsFlag = 5;
			break;
		case 9:
			pMe->m_gpsFlag = 5;
			break;
		case 10:
			pMe->m_gpsFlag = 5;
			break;
		default:
			pMe->m_gpsFlag = 5;
			break;
		}



		switch(pMe->m_gpsFlag) 
		{
		case 0:
		//	Track_Standalone( pts );
		//	pwa->m_gpsFlag = 1;
			Track_Network( pGetGPSInfo->pts );			
			break;
		case 1:
			Track_Optmal(pGetGPSInfo->pts); //ÊÖ»ú×ÔÑ¡			
			break;
		case 2:
			Track_Local( pGetGPSInfo->pts );
		//	pwa->m_gpsFlag = 2;
			break;
		case 3:
			Track_One_Shot( pGetGPSInfo->pts ); //´¿»ùÕ¾		
		//	pwa->m_gpsFlag = 0;
			break;
		case 4:
			Track_DLOAD_FIRST( pGetGPSInfo->pts ); // Ê×ÏÈ
			break;	
		case 5:
			Track_Standalone( pGetGPSInfo->pts );  // ´¿ÎÀÐÇ
			break;
		}

		pGetGPSInfo->pts->m_pParent = pMe;
//		uf_RecLog(pMe,"Track_cbInterval_copy 0");
		Track_cbInterval_copy( pGetGPSInfo->pts );
//			Track_cbInterval( pGetGPSInfo->pts );



	}






/*
	if (pGetGPSInfo)
	{

		
		Track_Stop(pGetGPSInfo->pts);
		pMe->m_F_P_F = TRUE;// ÓÃÀ´±ê¼ÇÊÇ²»ÊÇtrack_stop ºóµÚÒ»´ÎÓÃtrack_start  TRUE == ±íÊ¾  ÊÇ  FLASE = ±íÊ¾²»ÊÇ
		pMe->m_Q_P = FALSE;
		pMe->m_F_M = 1;
		
		pMe->m_NOMAINPOS = 1;  //±íÊ¾ÊÇ·ñÐèÒªÔÚÂÖÑ¯Ä£Ê½ÏÂ¶¨Î»µ½µÄÊ±ºòÇÐ»»µ½Ö÷Ä£Ê½  == 0 ÔòÐèÒª  == 1 Ôò²»ÐèÒª

		switch(pMe->cPosDetMode)
		{
		case '0':
			pMe->m_gpsFlag = 3;
			break;
		case '1':
			pMe->m_gpsFlag = 2;
			break;
		case '2':
			pMe->m_gpsFlag = 0;
			break;
		case '3':
			pMe->m_gpsFlag = 2;
			break;
		case '4':
			pMe->m_gpsFlag = 3;
			break;
		case '5':
			pMe->m_gpsFlag = 1;
			break;
		case '6':
			pMe->m_gpsFlag = 5;
			break;
		case '7':
			pMe->m_gpsFlag = 3;
			break;
		case '8':
			pMe->m_gpsFlag = 5;
			break;
		case '9':
			pMe->m_gpsFlag = 5;
			break;
		case 'A':
			pMe->m_gpsFlag = 5;
			break;
		default:
			pMe->m_gpsFlag = 5;
			break;
		}

		pGetGPSInfo->wProgress = 0;
		
		if (pMe->iInterval < 5)
		{
//			uf_RecLog(pMe,"¶¨Î»½øÀ´ÕâÀï 111");
			nErr = Track_Start( pGetGPSInfo->pts, pMe->m_gpsFlag, 0, 5, &pGetGPSInfo->theInfo );
			pMe->m_POS_T = 5;
		}
		else
		{
//			uf_RecLog(pMe,"¶¨Î»½øÀ´ÕâÀï 222");
			nErr = Track_Start( pGetGPSInfo->pts, pMe->m_gpsFlag, 0, pMe->iInterval, &pGetGPSInfo->theInfo );
			pMe->m_POS_T = pMe->iInterval;
		}

		pMe->m_F_START = TRUE; //ÓÃÀ´±ê¼ÇÊÇ²»ÊÇ²Å·¢ÆðTrack_Start   TRUE --  ±íÊ¾¸Õ·¢Æð      FALSE -- ²»ÊÇ²»ÊÇ¸Õ·¢Æð  

		if( nErr != SUCCESS ) { //Èç¹û¿ªÆôGPSÊ§°Ü
		 pGetGPSInfo->theInfo.nErr = nErr;
		 uf_RecLog(pMe,"GPS reboot fail");
		}
		else {
		 ISHELL_SetTimerEx( pMe->a.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
		}

	}*/
}



// ÔÚ½ÓÊÕµ½ 100004 µÄÊ±ºò ÖØÐÂ¹Ø±ÕÇëÇó È»ºóÔÙ¿ªÆôÇëÇó
void uf_Pos_Track_stopstart_error(newgimis * pMe)
{
	int nErr = 0;
	struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );

	/*
			switch(pwa->m_gpsFlag) 
			{
			case 0:
			//	Track_Standalone( pts );
			//	pwa->m_gpsFlag = 1;
				Track_Network( pts );			
				break;
			case 1:
				Track_Optmal(pts); //ÊÖ»ú×ÔÑ¡			
				break;
			case 2:
				Track_Local( pts );
			//	pwa->m_gpsFlag = 2;
				break;
			case 3:
				Track_One_Shot( pts ); //´¿»ùÕ¾		
			//	pwa->m_gpsFlag = 0;
				break;
			case 4:
				Track_DLOAD_FIRST( pts ); // Ê×ÏÈ
				break;	
			case 5:
				Track_Standalone( pts );  // ´¿ÎÀÐÇ
				break;
			}

			if (pwa->m_WitchPag == 5) //ËµÃ÷ÔÚÏÔÊ¾GPSÒ³Ãæ
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


	if (pGetGPSInfo)
	{
	
		nErr = Judge_GPS_Mode(pMe);
	    if (nErr == 1)
		{
			int si_tmp = 0;
			si_tmp = pMe->m_gpsFlag;
		
			pMe->m_gpsFlag = Get_GPS_Mode(pMe);
			if (si_tmp == pMe->m_gpsFlag)
			{
				pGetGPSInfo->pts->m_pParent = pMe;
				Track_cbInterval_copy( pGetGPSInfo->pts );
				return ;
			}
			
		}


		switch(pMe->m_gpsFlag) 
			{
			case 0:
			//	Track_Standalone( pts );
			//	pwa->m_gpsFlag = 1;
				Track_Network( pGetGPSInfo->pts );			
				break;
			case 1:
				Track_Optmal(pGetGPSInfo->pts); //ÊÖ»ú×ÔÑ¡			
				break;
			case 2:
				Track_Local( pGetGPSInfo->pts );
			//	pwa->m_gpsFlag = 2;
				break;
			case 3:
				Track_One_Shot( pGetGPSInfo->pts ); //´¿»ùÕ¾		
			//	pwa->m_gpsFlag = 0;
				break;
			case 4:
				Track_DLOAD_FIRST( pGetGPSInfo->pts ); // Ê×ÏÈ
				break;	
			case 5:
				Track_Standalone( pGetGPSInfo->pts );  // ´¿ÎÀÐÇ
				break;
			}

		pGetGPSInfo->pts->m_pParent = pMe;
		Track_cbInterval_copy( pGetGPSInfo->pts );
//		Track_cbInterval( pGetGPSInfo->pts );

/*
		Track_Stop(pGetGPSInfo->pts);
		pMe->m_F_P_F = TRUE;
		pGetGPSInfo->wProgress = 0;
		if (pMe->iInterval < 5)
		{
			nErr = Track_Start( pGetGPSInfo->pts, pMe->m_gpsFlag, 0, 5, &pGetGPSInfo->theInfo );
			pMe->m_POS_T = 5;
		}
		else
		{
			nErr = Track_Start( pGetGPSInfo->pts, pMe->m_gpsFlag, 0, pMe->iInterval, &pGetGPSInfo->theInfo );
			pMe->m_POS_T = pMe->iInterval;
		}

		pMe->m_F_START = TRUE; //ÓÃÀ´±ê¼ÇÊÇ²»ÊÇ²Å·¢ÆðTrack_Start   TRUE --  ±íÊ¾¸Õ·¢Æð      FALSE -- ²»ÊÇ²»ÊÇ¸Õ·¢Æð  

		if( nErr != SUCCESS ) { //Èç¹û¿ªÆôGPSÊ§°Ü
		 pGetGPSInfo->theInfo.nErr = nErr;
		 uf_RecLog(pMe,"GPS reboot fail 100004");
		}
		else {
		 ISHELL_SetTimerEx( pMe->a.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
		}
*/
  

	}

/*	
	if (pGetGPSInfo)
	{
	
		Track_Stop(pGetGPSInfo->pts);
		pMe->m_F_P_F = TRUE;
		pGetGPSInfo->wProgress = 0;
		if (pMe->iInterval < 5)
		{
			nErr = Track_Start( pGetGPSInfo->pts, pMe->m_gpsFlag, 0, 5, &pGetGPSInfo->theInfo );
			pMe->m_POS_T = 5;
		}
		else
		{
			nErr = Track_Start( pGetGPSInfo->pts, pMe->m_gpsFlag, 0, pMe->iInterval, &pGetGPSInfo->theInfo );
			pMe->m_POS_T = pMe->iInterval;
		}

		pMe->m_F_START = TRUE; //ÓÃÀ´±ê¼ÇÊÇ²»ÊÇ²Å·¢ÆðTrack_Start   TRUE --  ±íÊ¾¸Õ·¢Æð      FALSE -- ²»ÊÇ²»ÊÇ¸Õ·¢Æð  

		if( nErr != SUCCESS ) { //Èç¹û¿ªÆôGPSÊ§°Ü
		 pGetGPSInfo->theInfo.nErr = nErr;
		 uf_RecLog(pMe,"GPS reboot fail 100004");
		}
		else {
		 ISHELL_SetTimerEx( pMe->a.m_pIShell, 1000, &pGetGPSInfo->cbProgressTimer );
		}

	}
	*/
}

// ÔÚÖ÷Ä£Ê½ÏÂ ½ÓÊÕµ½ 100004 µÄÊ±ºò Ôò¶¨Ê± 10·ÖÖÓºóÔÙ´Î³¢ÊÔÖ÷Ä£Ê½ÄÜ·ñ¶¨Î»µ½
void uf_Pos_Track_stopstart_time(newgimis * pMe)
{
//	uf_RecLog(pMe,"¶¨Ê±Ê±¼äµ½ ÓÐ½øÀ´");
	pMe->m_NOMAINPOS = 0;
}

// ½øÈë¹Ø±ÕGPS¶¨Î»
/*
void newgimis_CloseGPS(newgimis * pMe)
{
	// \u6b63\u5728\u5173\u95ed\u47\u50\u53  ÕýÔÚ¹Ø±ÕGPS
	AECHAR szText[] = {0x6b63,0x5728,0x5173,0x95ed,0x47,0x50,0x53,0x0000}; // L"ÕÕÆ¬ÉÏ´«¹¦ÄÜÎ´¿ªÆô";
	struct _GetGPSInfo *pGetGPSInfo = pMe->Old_ScreenData;
	IMenuCtl *pMenuCtl = testgps_GetScreenData_gps( pMe );
	IDISPLAY_ClearScreen(pMe->a.m_pIDisplay);
	IDISPLAY_DrawText(pMe->a.m_pIDisplay, AEE_FONT_BOLD, szText, -1,    // -1 = ËùÓÐ×Ö·û´®µÄ¶¼±íÊ¾
						8, 3,  NULL, IDF_ALIGN_CENTER | IDF_ALIGN_MIDDLE);
	IDISPLAY_Update(pMe->a.m_pIDisplay);


//			void  *Old_ScreenData;  // ¾ÉµÄÒ³ÃæÖ¸Õë ÓÃÓÚ±£´æGPSÒ³ÃæµÄÖµ
//			void  *New_ScreenData; //ÐÂµÄÒ³ÃæÖ¸Õë ÓÃÓÚ±£´æµ±Ç°Ò³ÃæµÄÖ¸Õë µÈ¾ÉµÄÊÍ·ÅºóÖØÐÂ¸ø currentHandler
			 
			  if( pGetGPSInfo ) {
				  if( pGetGPSInfo->pPosDet ) {
					  CALLBACK_Cancel( &pGetGPSInfo->cbProgressTimer );
					  CALLBACK_Cancel( &pGetGPSInfo->cbPosDet );
					  IPOSDET_Release( pGetGPSInfo->pPosDet );
				  }
				  FREE( pGetGPSInfo );
				  uf_RecLog(pMe,"ÊÍ·ÅGPSÄÚ´æ³É¹¦1");
				  if (pMenuCtl)
				  {
					  IMENUCTL_Release( pMenuCtl );
					  testgps_SetScreenData( pMe, 0 );
					  uf_RecLog(pMe,"ÊÍ·ÅGPSÄÚ´æ³É¹¦2");
					  pMenuCtl = NULL;
				  }
				  
			  }

	pMe->Old_ScreenData =  NULL;
	ISHELL_SetTimer(pMe->a.m_pIShell, 3*1000, (PFNNOTIFY)uf_ReturnMenu, pMe);
}

*/




void IPOSDET_Release_fun(newgimis *pMe)
{
	struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );
	int si_tmp = 0;
	ud_RecLog_c(pMe->a.m_pIShell,1,&(pMe->nInd), "IPOSDET_Release_fun  IPOSDET_Release %d",si_tmp);
	do 
		{	
			si_tmp = IPOSDET_Release( pGetGPSInfo->pPosDet );
			ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), " IPOSDET_Release 2 = %d",si_tmp);
		} while(si_tmp > 1);

	ud_RecLog_c(pMe->a.m_pIShell,1,&(pMe->nInd), "IPOSDET_Release_fun  IPOSDET_Release %d",si_tmp);
}

  







