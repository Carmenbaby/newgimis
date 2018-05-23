/*===========================================================================
FILE: OEMTTSEXTGPS.h

SERVICES: OEM Functions TTS Supporter

===========================================================================*/

#ifndef __OEMTTSEXTGPS__H__
#define __OEMTTSEXTGPS__H__
/*===============================================================================
*************************** Include Head Files **********************************
===============================================================================*/
#include "AEE.h"
#include "OEMTTSExt_Gps.bid"

#define EVENT_GETGPS_DATA  EVT_USER+204// Show that the GPS has positioning successfully 

typedef enum { 
    CODE_ILLEGAL_ERR= -1,
    GPS_OPEN_FIRST,
    GPS_HAVEOPENED,
    GPS_DATA_SUCCESSED,
    GPS_DATA_FAILED_GPSOPENED,
    GPS_DATA_FAILED_GPSCLOSED,
    GPS_CLOSED_SUCCESSED,
    GPS_CLOSED_FAILED,
    GPS_MODULE_INIT,//模块未开始检测
    GPS_MODULE_WAITING,//模块检测中
    GPS_MODULE_SUCESSED,//模块检测成功
    GPS_MODULE_EXISTED,//模块不存在
}GPS_ALL_STATE_TYPE;
typedef struct GPSPositioningdate
{
  unsigned char Bd_STATUS[10];//V为数据无效 A为数据有效
  unsigned char N_S[15];// N or S
  unsigned char W_E[15];// W or E
  unsigned char lat[15];				   
  unsigned char lon[15];
  uint16     SPD;	//地面速率单位0.25m/s
}GPSPositioningdate_n ;
extern uint8 UI_CB_Type;
extern int   GPS_CT;

/*===============================================================================
*************************** Type Declarations ***********************************
===============================================================================*/

typedef struct ITTSEXTGPS ITTSEXTGPS;
AEEINTERFACE(ITTSEXTGPS)
{
   DECLARE_IBASE(ITTSEXTGPS)
   int (*TTS_GpsModuleCheck)(ITTSEXTGPS *pMe);
   int (*TTS_GpsPositioning)(ITTSEXTGPS *pMe);
   int (*TTS_Gps_AutoPositioning)(ITTSEXTGPS *pMe , int ct);//xulirui
   int (*TTS_GpsStopPositioning)(ITTSEXTGPS *pMe);
   int (*TTS_GetPositioning)(ITTSEXTGPS *pMe , GPSPositioningdate_n *data);// GET  gps positioning
};

/*===============================================================================
*************************** Access Macros ***************************************
===============================================================================*/
#define ITTSEXTGPS_AddRef(p)      AEEGETPVTBL((p), ITTSEXTGPS)->AddRef((p))
#define ITTSEXTGPS_Release(p)      AEEGETPVTBL((p), ITTSEXTGPS)->Release((p))
#define ITTSEXTGPS_GpsModuleCheck(p)      AEEGETPVTBL((p), ITTSEXTGPS)->TTS_GpsModuleCheck(p)
#define ITTSEXTGPS_Gps_AutoPositioning(p,n)   AEEGETPVTBL((p), ITTSEXTGPS)->TTS_Gps_AutoPositioning((p), (n)) 
#define ITTSEXTGPS_GpsStopPositioning(p)      AEEGETPVTBL((p), ITTSEXTGPS)->TTS_GpsStopPositioning(p) 
#define ITTSEXTGPS_GpsPositioning(p)      AEEGETPVTBL((p), ITTSEXTGPS)->TTS_GpsPositioning(p)
#define ITTSEXTGPS_GetPositioning(p, n)      AEEGETPVTBL((p), ITTSEXTGPS)->TTS_GetPositioning((p), (n))// GET  bd positioning
/*===============================================================================
********************* ITTSEXTGPS Extension Interface Explanation********************
===============================================================================*/
/*===============================================================================
ITTSEXTGPS_AddRef

Description:
    This function increments the reference count for the ITTSEXTGPS object

Prototype:
    uint32 ITTSEXTGPS_AddRef(ITTSEXTGPS * pMe)

Parameters:
    pMe: Pointer to the ITTSEXTGPS interface
    
Return Value:
    The updated reference count

Comments: None

Side Effects: None
===============================================================================*/

/*===============================================================================
ITTSEXTGPS_Release

Description:
    This function decrements the reference count for the ITTSEXTGPS object 
    and does appropriate cleanup if the reference count reaches zero.

Prototype:
    uint32 ITTSEXTGPS_Release(ITTSEXTGPS * pMe)

Parameters:
    pMe: Pointer to the ITTSEXTGPS interface

Return Value:
    The updated reference count

Comments:  None

Side Effects: None
===============================================================================*/
/*===============================================================================
 TTSEXT_Gps_Module_Check
    
    
Description:
    This function is used to check GPS module exist or not
 Prototype:
    int  TTSEXT_Gps_Module_Check(ITTSEXT * po )
    
Parameters:
    po: Pointer to the ITTSEXT interface
    
Return Value:
        GPS_MODULE_INIT,//模块未开始检测
        GPS_MODULE_WAITING,//模块检测中
        GPS_MODULE_SUCESSED,//模块检测成功
        GPS_MODULE_EXISTED,//模块不存在
    
Comments:  None
    
  Side Effects: None
===============================================================================*/
/*===============================================================================
    TTSEXTGPS_Gps_Start_Position
        
        
   Description:
       This function is used to open Gps and return Gps state .
   Prototype:
          int TTSEXTGPS_Gps_Start_Position(ITTSEXTGPS * po )
        
  Parameters:
          po: Pointer to the ITTSEXTGPS interface
        
   Return Value:
            The Gps state
            //GPS_OPENFAILED (int 0):Gps openfailed
            GPS_OPEN_FIRST:Gps have opened but couldn't communication
            GPS_HAVEOPENED:Gps have opened
        
   Comments:  None
        
   Side Effects: None
 ===============================================================================*/
 /*===============================================================================
            
 int TTSEXTGPS_Gps_Auto_Start_Position(ITTSEXTGPS * po ,int ct )  自启动GPS定位
            
            
           
 Description:
        This function is used to open Gps and return Gps state .
            
Prototype:
       int TTSEXTGPS_Gps_Start_Position(ITTSEXTGPS * po )
            
Parameters:
        po: Pointer to the ITTSEXTGPS interface
        ct :time to get new data
 Return Value:
      The Gps state
            GPS_OPEN_FIRST :Gps have opened but couldn't communication
            GPS_HAVEOPENED:Gps have opened
        
 Comments:  None
            
Side Effects: None
===============================================================================*/
        
/*===============================================================================
TTSEXTGPS_Get_Position_info
        
Description:
        This function is used to Access to location data .
        
Prototype:
     int TTSEXTGPS_Get_Position_info(ITTSEXTGPS * po ,GPSPositioningdate_n *gps_value)
        
Parameters:
       po: Pointer to the ITTSEXTGPS interface
       GPSPositioningdate_n *bd_value:The pointer to store data 
        
 Return Value:
         The data state
         STATUS_FLAG:
            
        //GPS_DATA_FAILED_GPSOPENED:Failed to get the data, the positioning is on but there is no fixed in position
        GPS_DATA_SUCCESSED: get the data successfully
        //GPS_DATA_FAILED_GPSCLOSED(int 2):Failed to get data, positioning is not open
        
 Comments:  None
        
 Side Effects: None
 ===============================================================================*/
        
/*===============================================================================
 TTSEXTGPS_Gps_Stop_Position
 Description:
      This function is used to close GPS and return GPS state .
        
 Prototype:
       int TTSEXTGPS_Gps_Stop_Position(ITTSEXTGPS * po)
        
Parameters:
     po: Pointer to the ITTSEXTGPS interface
        
Return Value:
        The Gps state
            
         GPS_CLOSED_SUCCESSED:Gps close successed
         GPS_CLOSED_FAILED:Gps close failed
           
 Comments:  None
        
 Side Effects: None       
        
===============================================================================*/


#endif
