/*===========================================================================
FILE: OEMTTSEXT.h

SERVICES: OEM Functions TTS Supporter

===========================================================================*/

#ifndef __OEMTTSEXT__H__
#define __OEMTTSEXT__H__
/*===============================================================================
*************************** Include Head Files **********************************
===============================================================================*/
#include "AEE.h"
#include "OEMTTSEXT.bid"


/*===============================================================================
*************************** MACRO Declarations **********************************
===============================================================================*/
#define EVENT_GETBD_DATA EVT_USER+206// Show that the BD has positioning successfully 
#define BD_OPENFAILED 0
#define BD_OPEN_FIRST 1 
#define BD_HAVEOPENED  2
#define BD_DATA_SUCCESSED 1  
#define BD_DATA_FAILED_BDOPENED 0
#define BD_DATA_FAILED_BDCLOSED 2
#define BD_CLOSED_FAILED 1
#define BD_CLOSED_SUCCESSED 0
#define BD_MODULE_EXISTED -1  //BD模块不存在

typedef struct BdBdPositioningdate
{
  unsigned char Bd_STATUS[10];//V为数据无效 A为数据有效
  unsigned char N_S[15];// N or S
  unsigned char W_E[15];// W or E
  unsigned char lat[15];				   
  unsigned char lon[15];	
  uint16 SPD;
}BdBdPositioningdate_n ;
extern uint8 UI_CB_Type;
extern int   BD_CT;


/*===============================================================================
*************************** Type Declarations ***********************************
===============================================================================*/

typedef struct ITTSEXT ITTSEXT;
AEEINTERFACE(ITTSEXT)
{
   DECLARE_IBASE(ITTSEXT)
   int (*TTS_Generate)(ITTSEXT *pMe, char * pText,int pTextlength ,char * vol);
   int (*TTS_SwitchtoQchat)(ITTSEXT *pMe);
   int (*TTS_BdPositioning)(ITTSEXT *pMe);//gufeng 
   int (*TTS_Bd_AutoPositioning)(ITTSEXT *pMe , int ct);//xulirui
   int (*TTS_BdStopPositioning)(ITTSEXT *pMe);//gufeng 
   int (*TTS_GetPositioning)(ITTSEXT *pMe , BdBdPositioningdate_n *data);// GET  bd positioning//gufeng 
};

/*===============================================================================
*************************** Access Macros ***************************************
===============================================================================*/
#define ITTSEXT_AddRef(p)      AEEGETPVTBL((p), ITTSEXT)->AddRef((p))
#define ITTSEXT_Release(p)      AEEGETPVTBL((p), ITTSEXT)->Release((p))
#define ITTSEXT_Generate(p, n,n1,n2)      AEEGETPVTBL((p), ITTSEXT)->TTS_Generate((p), (n),(n1),(n2))
#define ITTSEXT_SwitchtoQchat(p)      AEEGETPVTBL((p), ITTSEXT)->TTS_SwitchtoQchat(p)
#define ITTSEXT_Bd_AutoPositioning(p,n)   AEEGETPVTBL((p), ITTSEXT)->TTS_Bd_AutoPositioning((p), (n)) //xulirui 
#define ITTSEXT_BdStopPositioning(p)      AEEGETPVTBL((p), ITTSEXT)->TTS_BdStopPositioning(p)//gufeng 
#define ITTSEXT_BdPositioning(p)      AEEGETPVTBL((p), ITTSEXT)->TTS_BdPositioning(p)//gufeng 
#define ITTSEXT_GetPositioning(p, n)      AEEGETPVTBL((p), ITTSEXT)->TTS_GetPositioning((p), (n))// GET  bd positioning//gufeng 

/*===============================================================================
********************* ITTSEXT Extension Interface Explanation********************
===============================================================================*/
/*===============================================================================
ITTSEXT_AddRef

Description:
    This function increments the reference count for the ITTSEXT object

Prototype:
    uint32 ITTSEXT_AddRef(ITTSEXT * pMe)

Parameters:
    pMe: Pointer to the ITTSEXT interface
    
Return Value:
    The updated reference count

Comments: None

Side Effects: None
===============================================================================*/

/*===============================================================================
ITTSEXT_Release

Description:
    This function decrements the reference count for the ITTSEXT object 
    and does appropriate cleanup if the reference count reaches zero.

Prototype:
    uint32 ITTSEXT_Release(ITTSEXT * pMe)

Parameters:
    pMe: Pointer to the ITTSEXT interface

Return Value:
    The updated reference count

Comments:  None

Side Effects: None
===============================================================================*/



/*===============================================================================
ITTSEXT_SwitchtoQchat


Description:
    This function decrements the reference count for the ITTSEXT object 
    and does appropriate cleanup if the reference count reaches zero.

Prototype:
    uint32 ITTSEXT_SwitchtoQchat(ITTSEXT * pMe)

Parameters:
    pMe: Pointer to the ITTSEXT interface

Return Value:
    The updated reference count

Comments:  None

Side Effects: None
===============================================================================*/

/*===============================================================================
TTSEXT_Bd_Start_Position


Description:
    This function is used to open BD and return BD state .

Prototype:
    int TTSEXT_Bd_Start_Position(ITTSEXT * po )

Parameters:
    po: Pointer to the ITTSEXT interface

Return Value:
    The BD state
    BD_OPENFAILED (int 0):BD openfailed
    BD_OPEN_FIRST (int 1):BD have opened but couldn't communication
    BD_HAVEOPENED(int 2):BD have opened

Comments:  None

Side Effects: None
===============================================================================*/
/*===============================================================================
    
    int TTSEXT_Bd_Auto_Start_Position(ITTSEXT * po ,int ct )  自启动北斗定位
    
    
    
    Description:
        This function is used to open BD and return BD state .
    
    Prototype:
        int TTSEXT_Bd_Start_Position(ITTSEXT * po )
    
    Parameters:
        po: Pointer to the ITTSEXT interface
        ct :time to get new data
    Return Value:
        The BD state
        BD_OPENFAILED (int 0):BD openfailed
        BD_OPEN_FIRST (int 1):BD have opened but couldn't communication
        BD_HAVEOPENED(int 2):BD have opened

 Comments:  None
    
Side Effects: None
 ===============================================================================*/

/*===============================================================================
TTSEXT_Get_Position_info

Description:
    This function is used to Access to location data .

Prototype:
    int TTSEXT_Get_Position_info(ITTSEXT * po ,BdBdPositioningdate_n *bd_value)

Parameters:
    po: Pointer to the ITTSEXT interface
    BdBdPositioningdate_n *bd_value:The pointer to store data 

Return Value:
    The data state
    STATUS_FLAG:
    
    BD_DATA_FAILED_BDOPENED(int 0):Failed to get the data, the positioning is on but there is no fixed in position
    BD_DATA_SUCCESSED(int 1): get the data successfully
    BD_DATA_FAILED_BDCLOSED(int 2):Failed to get data, positioning is not open

Comments:  None

Side Effects: None
===============================================================================*/

/*===============================================================================
TTSEXT_Bd_Stop_Position

Description:
    This function is used to close BD and return BD state .

Prototype:
    int TTSEXT_Bd_Stop_Position(ITTSEXT * po)

Parameters:
    po: Pointer to the ITTSEXT interface

Return Value:
    The BD state
    
    BD_CLOSED_SUCCESSED(int 0):BD close successed
    BD_CLOSED_FAILED(int 1):BD close failed
   
Comments:  None

Side Effects: None
===============================================================================*/



#endif
