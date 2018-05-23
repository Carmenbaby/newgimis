/*======================================================
FILE:  SP_GetGPSInfo.c

        Copyright © 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "newgimis.h"
#include "SP_Track.h"
#include "gmTools.h"  // º”’‚∏ˆÕ∑Œƒº˛÷ª «Œ™¡À»’÷æ

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




/*===========================================================================
===========================================================================*/
static void testgps_GetGPSInfo_Paint( newgimis *pMe, GetGPSInfo_PaintRegions rgn )
{
   struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );

   // \u83b7\u53d6\u6b63\u5e38
   AECHAR strerr[] = {0x83b7,0x53d6,0x5f02,0x5e38,0x0000};//  ªÒ»°“Ï≥£
   AECHAR strnal[] = {0x83b7,0x53d6,0x6b63,0x5e38,0x0000};//  ªÒ»°“Ï≥£
   // \u536b\u661f\u20\u57fa\u7ad9\u20\u7f51\u7edc
   AECHAR strWX[] = {0x536b,0x661f,0x0000};  // Œ¿–«
   AECHAR strJZ[] = {0x57fa,0x7ad9,0x0000};  // ª˘’æ
   AECHAR strWL[] = {0x7f51,0x7edc,0x0000};  // Õ¯¬Á

   int si_tmp = 0;

   while (pGetGPSInfo == 0)
   {
	   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd), "GetGPSInfo pointer null 7");
	   pGetGPSInfo = testgps_GetScreenData_gps( pMe );
	   if (si_tmp++ > 10)
	   {
		   return;
	   }
	   
	   
   }
 //  ud_RecLog(pMe->a.m_pIShell,1,&(pMe->nInd), "÷∏’ÎªÒ»°µΩ 7");


   if (!pGetGPSInfo ) //Àµ√˜√ª”–ø™∆ÙGPS
   {
	   IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
       testgps_Printf( pMe, 3, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "GPS not open" );
	  return;
   }

   if( rgn == GETGPSINFO_PAINT_ALL ) {

//	   uf_RecLog(pMe,"÷ÿªÊ 1");

	  IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
	  testgps_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "Version:3.1.2PAYY" );
      testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "GetGPSInfo" );
      if( pMe->m_gpsFlag  == 0 ) {  // Àµ√˜ «Œ¿–«
         testgps_Printf_hanzi( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT,strWX );
      }
      else if(  pMe->m_gpsFlag == 1 ) {
         testgps_Printf_hanzi( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, strJZ );
      }
      else if( pMe->m_gpsFlag == 2 ) {
         testgps_Printf_hanzi( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, strWL );
      }


      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Failed : %d", pGetGPSInfo->dwFail+pGetGPSInfo->dwTimeout );
      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_RIGHT, "Timeout : %d", pGetGPSInfo->dwTimeout );


      testgps_Printf( pMe, 3, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Fixes : %d", pGetGPSInfo->dwFixNumber );

	  testgps_Printf( pMe, 7, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, 
         "..%d..", pGetGPSInfo->wProgress );




/*
	  if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_GPS ) {
         testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "One Shot" );
      }
      else if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_SEECONFIG ) {
         testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "Track L" );
      }
      else if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_YJKQ ) {
         testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "Track N" );
      }
      else if( pGetGPSInfo->wMainMenuEntry == MAINMENU_ITEM_FULL ) {
         testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "Track A" );
      }
*/
   }

   if( rgn == GETGPSINFO_PAINT_FIXCOUNT ) {
/*
	  IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
	  testgps_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "Version:2.6.8" );
      testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "GetGPSInfo" );
      if( pMe->m_gpsFlag  == 1 ) {  // Àµ√˜ «Œ¿–«
         testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "wei  xing" );
      }
      else if(  pMe->m_gpsFlag == 2 ) {
         testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "ji zhan" );
      }
      else if( pMe->m_gpsFlag == 3 ) {
         testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "wang luo" );
      }

      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Failed : %d", pGetGPSInfo->dwFail+pGetGPSInfo->dwTimeout );
      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_RIGHT, "Timeout : %d", pGetGPSInfo->dwTimeout );
*/
   }

   if( rgn == GETGPSINFO_PAINT_FIXDATA  ) {
#define MAXTEXTLEN   22
      AECHAR wcText[MAXTEXTLEN];
      char   latlonStr[MAXTEXTLEN];
//		uf_RecLog(pMe,"÷ÿªÊ 2");

	  IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
	  testgps_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "Version:3.1.2PAYY" );
      testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "GetGPSInfo" );
      if( pMe->m_gpsFlag  == 0 ) {  // Àµ√˜ «Œ¿–«
         testgps_Printf_hanzi( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT,strWX );
      }
      else if(  pMe->m_gpsFlag == 1 ) {
         testgps_Printf_hanzi( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, strJZ );
      }
      else if( pMe->m_gpsFlag == 2 ) {
         testgps_Printf_hanzi( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, strWL );
      }


      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Failed : %d", pGetGPSInfo->dwFail+pGetGPSInfo->dwTimeout );
      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_RIGHT, "Timeout : %d", pGetGPSInfo->dwTimeout );





      testgps_Printf( pMe, 3, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Fixes : %d", pGetGPSInfo->dwFixNumber );

      FLOATTOWSTR( pGetGPSInfo->theInfo.lat, wcText, MAXTEXTLEN * sizeof(AECHAR) );
      WSTR_TO_STR( wcText, latlonStr, MAXTEXTLEN );
      testgps_Printf( pMe, 5, 4, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s d", latlonStr );

      FLOATTOWSTR( pGetGPSInfo->theInfo.lon, wcText, MAXTEXTLEN * sizeof(AECHAR) );
      WSTR_TO_STR( wcText, latlonStr, MAXTEXTLEN );
      testgps_Printf( pMe, 6, 4, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s d", latlonStr );



	  testgps_Printf( pMe, 7, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, 
         "..%d..", pGetGPSInfo->wProgress );
	  // m_bGetGPSF;//”√¿¥±Íº«¥À ±ªÒ»°GPSµƒ◊¥Ã¨  TRUE -- ±Ì æªÒ»°“Ï≥£ FLASE -- ªÒ»°’˝≥£
	   if (pMe->m_bGetGPSF)
	   {
		   testgps_Printf_hanzi( pMe, 8, 3, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, strerr );
//		   testgps_Printf( pMe, 8, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "ERR:0x%x", pGetGPSInfo->theInfo.nErr );
	   }
	   else
	   {
		   testgps_Printf_hanzi( pMe, 8, 3, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, strnal );
//		   testgps_Printf( pMe, 8, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "" );
	   }


   }

   if( rgn == GETGPSINFO_PAINT_ERROR ) {
//	   uf_RecLog(pMe,"÷ÿªÊ 3");
      IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
	  testgps_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "Version:3.1.2PAYY" );
      testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "GetGPSInfo" );
      if( pMe->m_gpsFlag  == 0 ) {  // Àµ√˜ «Œ¿–«
         testgps_Printf_hanzi( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT,strWX );
      }
      else if(  pMe->m_gpsFlag == 1 ) {
         testgps_Printf_hanzi( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, strJZ );
      }
      else if( pMe->m_gpsFlag == 2 ) {
         testgps_Printf_hanzi( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, strWL );
      }


      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Failed : %d", pGetGPSInfo->dwFail+pGetGPSInfo->dwTimeout );
      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_RIGHT, "Timeout : %d", pGetGPSInfo->dwTimeout );





      testgps_Printf( pMe, 3, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Fixes : %d", pGetGPSInfo->dwFixNumber );


	  testgps_Printf( pMe, 6, 4, AEE_FONT_BOLD, IDF_ALIGN_LEFT, 
         "%s", pMe->lx_lat );



	  testgps_Printf( pMe, 7, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, 
         "..%d..", pGetGPSInfo->wProgress );
	  // m_bGetGPSF;//”√¿¥±Íº«¥À ±ªÒ»°GPSµƒ◊¥Ã¨  TRUE -- ±Ì æªÒ»°“Ï≥£ FLASE -- ªÒ»°’˝≥£
	   if (pMe->m_bGetGPSF)
	   {
		   testgps_Printf_hanzi( pMe, 8, 3, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, strerr );
//		   testgps_Printf( pMe, 8, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "ERR:0x%x", pGetGPSInfo->theInfo.nErr );
	   }
	   else
	   {
		   testgps_Printf_hanzi( pMe, 8, 3, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, strnal );
//		   testgps_Printf( pMe, 8, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "" );
	   }
   }

   if( rgn == GETGPSINFO_PAINT_FIXANIM  ) {
#define MAXTEXTLEN   22
      AECHAR wcText[MAXTEXTLEN];
      char   latlonStr[MAXTEXTLEN];   
//	  uf_RecLog(pMe,"÷ÿªÊ 4");
	  // m_bGetGPSF;//”√¿¥±Íº«¥À ±ªÒ»°GPSµƒ◊¥Ã¨  TRUE -- ±Ì æªÒ»°“Ï≥£ FLASE -- ªÒ»°’˝≥£
	  IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
	  testgps_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "Version:3.1.2PAYY" );
      testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "GetGPSInfo" );
      if( pMe->m_gpsFlag  == 0 ) {  // Àµ√˜ «Œ¿–«
         testgps_Printf_hanzi( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT,strWX );
      }
      else if(  pMe->m_gpsFlag == 1 ) {
         testgps_Printf_hanzi( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, strJZ );
      }
      else if( pMe->m_gpsFlag == 2 ) {
         testgps_Printf_hanzi( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, strWL );
      }


      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Failed : %d", pGetGPSInfo->dwFail+pGetGPSInfo->dwTimeout );
      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_RIGHT, "Timeout : %d", pGetGPSInfo->dwTimeout );





      testgps_Printf( pMe, 3, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Fixes : %d", pGetGPSInfo->dwFixNumber );

      FLOATTOWSTR( pGetGPSInfo->theInfo.lat, wcText, MAXTEXTLEN * sizeof(AECHAR) );
      WSTR_TO_STR( wcText, latlonStr, MAXTEXTLEN );
	  if (pMe->m_bGetGPSF)
	  {
		  testgps_Printf( pMe, 6, 4, AEE_FONT_BOLD, IDF_ALIGN_LEFT, 
         "%s", pMe->lx_lat );
	  }
	  else
	  {
		  
		  testgps_Printf( pMe, 5, 4, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s d", latlonStr );

		  FLOATTOWSTR( pGetGPSInfo->theInfo.lon, wcText, MAXTEXTLEN * sizeof(AECHAR) );
		  WSTR_TO_STR( wcText, latlonStr, MAXTEXTLEN );
		  testgps_Printf( pMe, 6, 4, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s d", latlonStr );
	  }

	  



	  testgps_Printf( pMe, 7, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, 
         "..%d..", pGetGPSInfo->wProgress );
	  // m_bGetGPSF;//”√¿¥±Íº«¥À ±ªÒ»°GPSµƒ◊¥Ã¨  TRUE -- ±Ì æªÒ»°“Ï≥£ FLASE -- ªÒ»°’˝≥£
	   if (pMe->m_bGetGPSF)
	   {
		   testgps_Printf_hanzi( pMe, 8, 3, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, strerr );
//		   testgps_Printf( pMe, 8, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "ERR:0x%x", pGetGPSInfo->theInfo.nErr );
	   }
	   else
	   {
		   testgps_Printf_hanzi( pMe, 8, 3, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, strnal );
//		   testgps_Printf( pMe, 8, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "" );
	   }
   }


/*
	  IDISPLAY_ClearScreen( pMe->a.m_pIDisplay );
	  testgps_Printf( pMe, 0, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "Version:2.6.8" );
      testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_LEFT, "GetGPSInfo" );
      if( pMe->m_gpsFlag  == 1 ) {  // Àµ√˜ «Œ¿–«
         testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "wei  xing" );
      }
      else if(  pMe->m_gpsFlag == 2 ) {
         testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "ji zhan" );
      }
      else if( pMe->m_gpsFlag == 3 ) {
         testgps_Printf( pMe, 1, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "wang luo" );
      }


      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Failed : %d", pGetGPSInfo->dwFail+pGetGPSInfo->dwTimeout );
      testgps_Printf( pMe, 2, 4, AEE_FONT_NORMAL, IDF_ALIGN_RIGHT, "Timeout : %d", pGetGPSInfo->dwTimeout );





      testgps_Printf( pMe, 3, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, "Fixes : %d", pGetGPSInfo->dwFixNumber );

      FLOATTOWSTR( pGetGPSInfo->theInfo.lat, wcText, MAXTEXTLEN * sizeof(AECHAR) );
      WSTR_TO_STR( wcText, latlonStr, MAXTEXTLEN );
      testgps_Printf( pMe, 5, 4, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s d", latlonStr );

      FLOATTOWSTR( pGetGPSInfo->theInfo.lon, wcText, MAXTEXTLEN * sizeof(AECHAR) );
      WSTR_TO_STR( wcText, latlonStr, MAXTEXTLEN );
      testgps_Printf( pMe, 6, 4, AEE_FONT_BOLD, IDF_ALIGN_CENTER|IDF_RECT_FILL, "%s d", latlonStr );
   

	  testgps_Printf( pMe, 7, 4, AEE_FONT_BOLD, IDF_ALIGN_LEFT, 
         "ABORTED 0x%x", pGetGPSInfo->theInfo.nErr );



	  testgps_Printf( pMe, 7, 4, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, 
         "..%d..", pGetGPSInfo->wProgress );
	  // m_bGetGPSF;//”√¿¥±Íº«¥À ±ªÒ»°GPSµƒ◊¥Ã¨  TRUE -- ±Ì æªÒ»°“Ï≥£ FLASE -- ªÒ»°’˝≥£
	   if (pMe->m_bGetGPSF)
	   {
		   testgps_Printf_hanzi( pMe, 8, 3, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, strerr );
//		   testgps_Printf( pMe, 8, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "ERR:0x%x", pGetGPSInfo->theInfo.nErr );
	   }
	   else
	   {
		   testgps_Printf_hanzi( pMe, 8, 3, AEE_FONT_NORMAL, IDF_ALIGN_LEFT, strnal );
//		   testgps_Printf( pMe, 8, 3, AEE_FONT_BOLD, IDF_ALIGN_RIGHT, "" );
	   }
   
   */





}





/*===========================================================================
===========================================================================*/
boolean testgps_ShowGetGPSInfo_HandleEvent( newgimis *pMe, AEEEvent eCode,
                                                 uint16 wParam, uint32 dwParam )
{
//	char *sTmp = NULL;
   boolean bHandled = FALSE;
   int  m_nFontHeight = 0;
   

   

//   struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pMe );
   IMenuCtl *pMenuCtl = testgps_GetScreenData( pMe );

   switch( eCode ) {

   case EVT_SCREEN:
      if( wParam == SCREEN_PARAM_INIT ) 
	  {
		 if( pMenuCtl ) 
		 {  /* This is not expected to happen */
			bHandled = TRUE;
		 }
		 else 
		 {
			 if( ISHELL_CreateInstance( pMe->a.m_pIShell, AEECLSID_MENUCTL,  (void **)&pMenuCtl ) == SUCCESS )
			 {
					IMENUCTL_AddItem( pMenuCtl, NEWGIMIS_RES_FILE, IDS_BACK, 0 , NULL, (uint32)pMenuCtl );
					// SETAEERECT(&rc, 0, pApp->m_rScreenRect.y, pApp->m_rScreenRect.dx, rcm.y - pApp->m_rScreenRect.y);
//						SETAEERECT(&pMe->m_rScreenRect, 45, pMe->DeviceInfo.cyScreen - 50, pMe->DeviceInfo.cxScreen-90, 30);
			//		SETAEERECT( &pMe->m_rScreenRect, 0, 20, 100, 20 );
					
					m_nFontHeight = IDISPLAY_GetFontMetrics( pMe->a.m_pIDisplay, AEE_FONT_NORMAL, NULL, NULL );
		//			 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_FONT_NORMAL£∫%d  ",m_nFontHeight);
					//	AEE_FONT_BOLD  AEE_FONT_LARGE
					m_nFontHeight = IDISPLAY_GetFontMetrics( pMe->a.m_pIDisplay, AEE_FONT_BOLD, NULL, NULL );
		//			 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_FONT_BOLD£∫%d  ",m_nFontHeight);
					m_nFontHeight = IDISPLAY_GetFontMetrics( pMe->a.m_pIDisplay, AEE_FONT_LARGE, NULL, NULL );
		//			 ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"AEE_FONT_LARGE£∫%d  ",m_nFontHeight);

//						IMENUCTL_GetRect(pMenuCtl, &rcm);
//						SETAEERECT(&rc, 0, pApp->m_rScreenRect.y, pApp->m_rScreenRect.dx, rcm.y - pApp->m_rScreenRect.y);

//					SETAEERECT(&pMe->m_rScreenRect, 45, pMe->DeviceInfo.cyScreen - 50, pMe->DeviceInfo.cxScreen-90, m_nFontHeight+10);
					SETAEERECT(&pMe->m_rScreenRect, 45, pMe->DeviceInfo.cyScreen - pMe->m_MenuH, pMe->DeviceInfo.cxScreen-90, pMe->m_MenuH);

					IMENUCTL_SetRect( pMenuCtl, &pMe->m_rScreenRect );
					testgps_SetScreenData( pMe, (void *)pMenuCtl );

					testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ALL );
					bHandled = TRUE;
					// pMe->DeviceInfo
			 }

		 }

            
         
         if( !bHandled ) 
		 {
			 pMe->m_WitchPag = 10;
            testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
            bHandled = TRUE;
         }
      }
      else if( wParam == SCREEN_PARAM_CLOSE ) 
	  {

				  
		if (pMenuCtl)
		{
		  IMENUCTL_Release( pMenuCtl );
		  testgps_SetScreenData( pMe, 0 );
		}

		pMe->currentHandler = 0;
		  
         
         bHandled = TRUE;
      }
      else if( wParam == SCREEN_PARAM_PAINT ) {
		  IMENUCTL_SetActive( pMenuCtl, TRUE );

        testgps_GetGPSInfo_Paint( pMe, (GetGPSInfo_PaintRegions)dwParam );
		IMENUCTL_Redraw( pMenuCtl );
		 
         bHandled = TRUE;
      }
	  
      break;

   case EVT_KEY:
	   if( wParam == AVK_CLR || wParam == AVK_SELECT  || wParam == AVK_END) {
			
		   pMe->m_WitchPag = 10;
//		   ud_RecLog(pMe->a.m_pIShell,pMe->cWrtLog,&(pMe->nInd),"∑µªÿ÷˜ΩÁ√Ê 1:%d",pMe->m_WitchPag);
			testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );

		  
         
         bHandled = TRUE;
      }

      break;

   case EVT_DIALOG_START:

      bHandled = TRUE;
      break;

   case EVT_DIALOG_END:

      testgps_DrawScreen( pMe, (uint32)GETGPSINFO_PAINT_ALL );
      bHandled = TRUE;
      break;

   case EVT_COMMAND:
//	   testgps_GotoScreen( pMe, SCREENID_MAINMENU, 0 );
         bHandled = TRUE;
      break;
   }

   return bHandled;
}



// œ‘ æ≤Èø¥GPS≈‰÷√ΩÁ√Ê
int Show_GPS_Fun(newgimis *pnewgimis)
{
	
   struct _GetGPSInfo *pGetGPSInfo = testgps_GetScreenData_gps( pnewgimis );
   IMenuCtl *pMenuCtl = testgps_GetScreenData( pnewgimis );
   if ((pGetGPSInfo != NULL) && (pMenuCtl != NULL))
   {
	   testgps_GetGPSInfo_Paint( pnewgimis, (GetGPSInfo_PaintRegions)GETGPSINFO_PAINT_ALL );
		IMENUCTL_Redraw( pMenuCtl );
		pGetGPSInfo = NULL;
		pMenuCtl = NULL;
	   return 1;
   }

   pGetGPSInfo = NULL;
   pMenuCtl = NULL;
   return 0;
}







