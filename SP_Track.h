#ifndef __SP_TRACK_H__
#define __SP_TRACK_H__

/*======================================================
FILE:  SP_Track.h

SERVICES: Tracking

GENERAL DESCRIPTION:
	Sample code to demonstrate services of IPosDet.

        Copyright � 2003 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/
#include "AEEComdef.h"
#include "AEEPosDet.h"
#include "newgimis.h"

typedef struct _TrackState TrackState;
typedef enum {
//   TRACK_LOCAL,      /* Uses AEEGPS_TRACK_LOCAL */
//   TRACK_NETWORK,     /* Uses AEEGPS_TRACK_NETOWORK */
//   TRACK_AUTO        /* Attempts using AEEGPS_TRACK_LOCAL if it fails uses AEEGPS_TRACK_NETWORK */
	TRACK_NETWORK,     /* Uses AEEGPS_TRACK_NETOWORK */
	TRACK_OPTIMAL,	  /* Uses AEEGPS_MODE_TRACK_OPTIMAL  */	
	TRACK_LOCAL,      /* Uses AEEGPS_TRACK_LOCAL */
	ONE_SHOT,		 /* Uses  AEEGPS_MODE_ONE_SHOT  */  
	TRACK_FIRST,
	TRACK_STANDALONE,	/* Uses AEEGPS_MODE_TRACK_STANDALONE  */
	TRACK_AUTO,        /* Attempts using AEEGPS_TRACK_NETWORK  if it fails uses AEEGPS_TRACK_LOCAL */
	TRACK_AUTO_1,      /* Attempts using AEEGPS_TRACK_NETWORK  if it fails uses AEEGPS_MODE_ONE_SHOT */
	ONE_SHOT_1,			/* Attempts using  AEEGPS_MODE_ONE_SHOT,if it fails uses AEEGPS_MODE_TRACK_STANDALONE  */  
	TRACK_STANDALONE_1,	/* �������ǣ����ǲ���תΪ ONE_SHOT */	
	TRACK_STANDALONE_2,	/* �������ǣ����ǲ���תΪ NETOWORK */	
	TRACK_STANDALONE_3	/* �������ǣ����ǲ���תΪ ONE_SHOT, �����Ƿ�ɹ���ת�����Ƕ�λ�����λ�վ��λʱ���ɸ�����λʱ��ȷ�� */
} TrackType;

typedef struct {
   int    nErr;      /* SUCCESS or AEEGPS_ERR_* */
   uint32 dwFixNum;  /* fix number in this tracking session. */
   double lat;       /* latitude on WGS-84 Geoid */
   double lon;       /* longitude on WGS-84 Geoid */
   short  height;    /* Height from WGS-84 Geoid */


   // Add by ---- 2010-07-08  ����ȡ�����±���
   uint16 dVelHor;	/*  ˮƽ�����ٶ� ��������0.25 meters/second Ϊ��λ  */
   int8   dVelVer;	/*  ��ֱ�����ٶ� ��������0.5 meters/second Ϊ��λ*/	
// Add End 


   AEEGPSServer server;
   AEEGPSQos qos;
   AEEGPSOpt optim;
}PositionData;



struct _TrackState{
	boolean     bInNotification;     /* When the state machine is notifying the client */
	boolean     bSetForCancellation; /* Track is meant to be cancelled. do so when it is safe. */
	boolean     bInProgress;         /* when tracking is in progress. */
	/**************************/
	
	/* For TRACK_AUTO */
	boolean     bModeAuto;
	boolean     bModeLocal;
	
	/* Private members to work with IPosDet */
	AEECallback cbIntervalTimer;
	AEECallback cbInfo;
	AEEGPSInfo theInfo; 
	/**************************/
	
	/* Clients response members. */
	AEECallback *pcbResp;
	PositionData *pResp;
	/**************************/
	
	/* Client passed members. */
	int nPendingFixes;
	int nTrackInterval;
	IPosDet    *pPos;
	IShell     *pShell;
	/**************************/

	/* For TRACK_AUTO */
	int     iModeAuto;				// 0 - ��ʹ���Զ�ת��ģʽ, 1 - local �� network ģʽ֮��ת��, 2 - oneshot ��network ģʽ֮��ת��
									// 3 - oneshot ��standlone ģʽ֮��ת��, 4 - standlone �� oneshot֮��ģʽ�л�, 5- standlone��NetWork֮��ģʽ�л�
	newgimis		*m_pParent;			// Add by ---- 2012-05-10	cAccType �� pMe->cAccType �õ�

	AEEPositionInfoEx theinfoEx;
};



#ifdef __cplusplus
extern "C" {
#endif

   /* Return Values : 
             SUCCESS
             EBADPARM - One or more of the Invalid arguments
             EUNSUPPORTED - Unimplemented
             ENOMEMORY - When system is out of memory.
             EFAILED - General failure.
             EALREADY - When tracking is already in progress.


      Values in PositionData::nErr :
             SUCCESS
             AEEGPS_ERR_*
             EIDLE - When a session is halted or done.
   */


   /* Creates and initializes a handle for tracking. 
   ** Invoke the CALLBACK_Cancel( pcb ) to destroy this object. */
   int Track_Init( newgimis *pwe, IPosDet *pIPos, AEECallback *pcb,
      TrackState **po );

   /* Starts the tracking using the object created in Track_Init */
   // ����˵��  ���������� ��λ���� ��� Ϊ n n > 0 ��λn��  �����0 ��ѭ����λ  
   // ���ĸ����� ���Ƕ�λʱ�� ��� Ϊ 0 ��һֱ��λ  �������Ϊÿ�������붨λһ�� ��λΪ��
   int Track_Start( TrackState *pts, TrackType t, 
      int nFixes, int nInterval, PositionData *pData );

   /* Stops the tracking, does not clean up the object, it can be
   ** further used with Track_Start. Only CALLBACK_Cancel(pcb) releases
   ** the object. */
   int Track_Stop( TrackState *pts );

#ifdef __cplusplus
}
#endif


int Track_ReConfig( TrackState *pts );
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
#endif