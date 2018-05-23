#ifndef OEMWIFILOC_H
#define OEMWIFILOC_H

#include "AEE.h"
#include "OEMWifiLocation.bid"
#include "wifi.h"
/*===============================================================================
*************************** Type Declarations ***********************************
===============================================================================*/

typedef struct IOEMWIFILOC IOEMWIFILOC;

AEEINTERFACE(IOEMWIFILOC)
{
   DECLARE_IBASE(IOEMWIFILOC)
   int (*AEEWifiLoc_Open)(IOEMWIFILOC *pMe, int flag, unsigned long mstime);
   int (*AEEWifiLoc_GetData)(IOEMWIFILOC *pMe, wifi_data *buf, unsigned int num);
   int (*AEEWifiLoc_Close)(IOEMWIFILOC *pMe);
   int (*AEEWifiLoc_State)(IOEMWIFILOC *pMe , int flag);
};

#define IOEMWifiLoc_AddRef(p)      				AEEGETPVTBL((p), IOEMWIFILOC)->AddRef((p))
#define IOEMWifiLoc_Release(p)      			AEEGETPVTBL((p), IOEMWIFILOC)->Release((p))
#define IOEMWifiLoc_Open(p, flag, mstime)  		AEEGETPVTBL((p), IOEMWIFILOC)->AEEWifiLoc_Open((p), (flag), (mstime))
#define IOEMWifiLoc_GetData(p,buf,num) 			AEEGETPVTBL((p), IOEMWIFILOC)->AEEWifiLoc_GetData((p), (buf), (num)) 
#define IOEMWifiLoc_Close(p)     				AEEGETPVTBL((p), IOEMWIFILOC)->AEEWifiLoc_Close((p)) 
#define IOEMWifiLoc_State(p,flag)     			AEEGETPVTBL((p), IOEMWIFILOC)->AEEWifiLoc_State((p), (flag))

#endif
/*===============================================================================
********************* IOEMWIFILOC Extension Interface Explanation********************
===============================================================================*/
/*===============================================================================
IOEMWifiLoc_AddRef

Description:
    This function increments the reference count for the IOEMWIFILOC object

Prototype:
    uint32 IOEMWifiLoc_AddRef(IOEMWIFILOC * pMe)

Parameters:
    pMe: Pointer to the IOEMWIFILOC interface
    
Return Value:
    The updated reference count

Comments: None

Side Effects: None
===============================================================================*/

/*===============================================================================
IOEMWifiLoc_Release

Description:
    This function decrements the reference count for the IOEMWIFILOC object 
    and does appropriate cleanup if the reference count reaches zero.

Prototype:
    uint32 OEMWifiLoc_Release(IOEMWIFILOC * po);

Parameters:
    pMe: Pointer to the IOEMWIFILOC interface

Return Value:
    The updated reference count

Comments:  None

Side Effects: None
===============================================================================*/
/*===============================================================================
 IOEMWifiLoc_Open
    
    
Description:
    This function is used to open the wifi location module by Determine the pattern of flag
    
Prototype:
    int OEMWifiLoc_Open(IOEMWIFILOC * po, int flag, unsigned long mstime);
    
Parameters:
    po: Pointer to the IOEMWIFILOC interface
    
    flag: the wifi operater mode
    
		WIFI_FORCE_OPEN,    wifi will always running, until the IOEMWifiLoc_Close() function be called
							the parameter of mstime will be ignored
							
		WIFI_TIMER_MODE,	wifi will run for a period of time 
							and the IOEMWifiLoc_Close() will be called in tne end, 
							the running time is depend on mstime parameter
							
	mstime: WIFI_TIMER_MODE running time ms.
Return Value:
		if open succeed return 0,
        WIFI_ALREADY_RUNNING  the wifi is running you can try to get the data
        WIFI_GPS_RUNNING_ERR  wifi open error
        WIFI_INVALID_MODE_ERR you have input invalid flag parameter
Comments:  None
    
  Side Effects: None
===============================================================================*/
/*===============================================================================
    IOEMWifiLoc_GetData
        
        
Description:
       This function is used to get wifi data and return wifi data number .
Prototype:
          int OEMWifiLoc_GetData(IOEMWIFILOC * po , wifi_data *buf, unsigned int num);
        
Parameters:
          po: Pointer to the IOEMWIFILOC interface
          buf: the buffer where you want to store the wifi data
          		the wifi data format is
          		
          		typedef struct wifi_data_t {
					unsigned char mac[MAC_SIZE]; //MAC_SIZE = 6
    				unsigned char  signal;
				} wifi_data;

		 num: The num of wifi data you want to get.
		 	  if not have enough data to get,it will replaced by zero
   Return Value:
            if successed, return  wifi data number .
            if the num bigger than MAX_WIFI_DATA_NUM(maybe 200) it will return WIFI_INVALIDE_NUM
        
   Comments:  None
        
   Side Effects: None
 ===============================================================================*/
/*===============================================================================
    IOEMWifiLoc_Close
        
        
Description:
       This function is used to close wifi
Prototype:
          int OEMWifiLoc_Close(IOEMWIFILOC * po);
        
Parameters:
          po: Pointer to the IOEMWIFILOC interface

Return Value:
            if successed, return  0
            if wifi is already closed return WIFI_ALREADY_OFF
        
   Comments:  None
        
   Side Effects: None
 ===============================================================================*/
        
/*===============================================================================
IOEMWifiLoc_State
        
Description:
        This function is used get wifi info .
        
Prototype:
     int OEMWifiLoc_State(IOEMWIFILOC * po, int flag)
        
Parameters:
       po: Pointer to the IOEMWIFILOC interface
       flag: the info type
       		WIFI_DATA_NUM
    		WIFI_CURENT_STATE

        
 Return Value:
 		if flag is WIFI_DATA_NUM,return wifi date num
        if flag is WIFI_CURENT_STATE return wifi current state they are:
	        WIFI_SIO_OPEN_ERR			wifi init error
			WIFI_INIT_SUCCEED			wifi init succeed
			WIFI_RUNNING				wifi is running
			WIFI_CLOSED					wifi is closed
 Comments:  None
        
 Side Effects: None
 ===============================================================================*/
        

