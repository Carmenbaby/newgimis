#ifndef AEEQDK30MANAGER_H 
#define AEEQDK30MANAGER_H
/*=============================================================================
 
FILE: AEEIQDK30Manager.h

SERVICES: The IQDK30Manager interface manages the interfaces that are common to all
          applications.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IQDK30Manager

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================*/
#include "AEE.h"
#include "AEEQDKAddress.h"
#include "AEEQDK30DataTypes.h"
#include "AEEIQDKManager.h"
//#include "AEEIQDKManager.bid"


#define IQDK_MAX_VERSION      16
#define IQDK_MAX_PREFIX       16
#define IQDK_MAX_MIMETYPE    256

#define AEEIID_IQDK30MANAGER 0x01074d42

/*==============================================================================
   DATA STRUCTURES
==============================================================================*/


typedef union
{
   IQDK30_QChatStatusEventType statusEventType; 
} IQDK30_ManagerEvtPayload;

/*=============================================================================
                    CLASS DEFINITIONS
=============================================================================*/
typedef struct IQDK30Manager IQDK30Manager;

#define INHERIT_IQDKManager(iname)\
   INHERIT_IQueryInterface(iname);\
   IQDK_ErrorType (*Init)(iname *pMe, AEECLSID appClsID);\
   IQDK_ErrorType (*GetQChatStatus)(iname *p, IQDK_QChatStatusEventType *status);\
   IQDK_ErrorType (*GetSetting)(iname *p, IQDKMGR_Setting setting, IQDKMGR_SettingType * settingData);\
   IQDK_ErrorType (*GetPref)(iname *p, IQDKMGR_Pref pref, IQDKMGR_PrefType * prefData);\
   IQDK_ErrorType (*SetPref)(iname *p, IQDKMGR_Pref pref, IQDKMGR_PrefType * prefData);\
   IQDK_ErrorType (*GetQChatAddressByIndex)(iname *pMe, uint8 addrIndex, IQDKAddressData *addr);\
   IQDK_ErrorType (*GetQChatAddressCount)(iname *pMe, uint8 *addrCount);\
   IQDK_ErrorType (*GetAvailabilityStateCount)(iname* pMe, uint8* count);\
   IQDK_ErrorType (*GetAvailabilityStateEntryByIndex)(iname* pMe, uint8 index,IQDK_AvailabilityEntry **entry);\
   IQDK_ErrorType (*GetCallTypeCapabilities)(iname* pMe, uint16* callTypeCapab);\
   IQDK_ErrorType (*GetEventPayload)(iname *pMe, uint32 interfaceId, uint16 eventType, IQDK_ManagerEvtPayload **payload)

#define INHERIT_IQDK30Manager(iname)\
   INHERIT_IQDKManager(IQDK30Manager);\
   IQDK_ErrorType (*Qdk30GetEventPayload)(iname *pMe, uint32 interfaceId, uint16 eventType, IQDK30_ManagerEvtPayload **payload);\
   IQDK_ErrorType (*Qdk30GetQChatStatus)(iname *p, IQDK30_QChatStatusEventType *status)


AEEINTERFACE(IQDK30Manager)
{
   INHERIT_IQDK30Manager(IQDK30Manager);
};

#define IQDK30MANAGER_AddRef(p)                                AEEGETPVTBL((p),IQDK30Manager)->AddRef((p))
#define IQDK30MANAGER_Release(p)                               AEEGETPVTBL((p),IQDK30Manager)->Release((p))
#define IQDK30MANAGER_QueryInterface(p,clsid,pp)               AEEGETPVTBL((p),IQDK30Manager)->QueryInterface((p),(clsid),(pp))
#define IQDK30MANAGER_Init(p,clsid)                            AEEGETPVTBL((p),IQDK30Manager)->Init((p),(clsid))
#define IQDK30MANAGER_GetSetting(p,s,d)                        AEEGETPVTBL((p),IQDK30Manager)->GetSetting((p),(s),(d))
#define IQDK30MANAGER_SetPref(p,pf,d)                          AEEGETPVTBL((p),IQDK30Manager)->SetPref((p),(pf),(d))
#define IQDK30MANAGER_GetPref(p,pf,d)                          AEEGETPVTBL((p),IQDK30Manager)->GetPref((p),(pf),(d))
#define IQDK30MANAGER_GetQChatAddressByIndex(p,i,a)            AEEGETPVTBL((p),IQDK30Manager)->GetQChatAddressByIndex((p),(i),(a))
#define IQDK30MANAGER_GetQChatAddressCount(p,i)                AEEGETPVTBL((p),IQDK30Manager)->GetQChatAddressCount((p),(i))
#define IQDK30MANAGER_GetAvailabilityStateCount(p,c)           AEEGETPVTBL((p),IQDK30Manager)->GetAvailabilityStateCount((p),(c))
#define IQDK30MANAGER_GetAvailabilityStateEntryByIndex(p,i,e)  AEEGETPVTBL((p),IQDK30Manager)->GetAvailabilityStateEntryByIndex((p),(i),(e))
#define IQDK30MANAGER_GetEventPayload(p,i,e,a)                 AEEGETPVTBL((p),IQDK30Manager)->Qdk30GetEventPayload((p),(i),(e),(a))
#define IQDK30MANAGER_GetCallTypeCapabilities(p,c)             AEEGETPVTBL((p),IQDK30Manager)->GetCallTypeCapabilities((p),(c))
#define IQDK30MANAGER_GetQChatStatus(p, s)                     AEEGETPVTBL((p),IQDK30Manager)->Qdk30GetQChatStatus((p),(s))

#endif // AEEQDK30MANAGER_H

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================

================================================================================

IQDK30_ManagerEvtPayload

Description:
   This structure contains the event payload retrieved with an 
   IQDK30MANAGER_GetEventPayload API call.

Definition:
   typedef union
   {
      IQDK_QChatStatusEventType statusEventType; 
   } IQDK30_ManagerEvtPayload;

Members:
   statusEventType - Contains fields relating to the status and 
                     user preferences of the QChat Client

See Also:
   IQDK30MANAGER_GetEventPayload()
   IQDK_QChatStatusEventType 

================================================================================
  INTERFACES   DOCUMENTATION
================================================================================

Interface Name: IQDK30Manager

Description:
    IQDK30Manager interface is a set of common API methods that are intended to be
used by any application that wants to use the services of the QChat System. Some
of the APIs in this interface include proper initialization with the QChat Client
and querying the state of the QChat Client.

The IQDK30Manager interface instance is obtained via ISHELL_CreateInstance()
mechanism.

================================================================================

IQDK30MANAGER_AddRef()

Description:
   This method increments the reference count of the IQDK30Manager Interface
   object.  This allows the object to be shared by multiple callers. The
   object is freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IQDK30MANAGER_AddRef(IQDK30Manager * po)

Parameters:

   po [in]: Pointer to the IQDK30Manager Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Version:
   Introduced in QDK v 3.0

Side Effects:
   None

See Also:
   IQDK30MANAGER_Release()

=============================================================================

IQDK30MANAGER_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQDK30MANAGER_Release(IQDK30Manager * po)

Parameters:

   po [in]: Pointer to the IQDK30Manager Interface object

Return Value:
   Decremented reference count for the object. The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Version:
   Introduced in QDK v 3.0

Side Effects:
   None

See Also:
   IQDK30MANAGER_AddRef()

==============================================================================

IQDK30MANAGER_QueryInterface()

Description:
   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID.  This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interace is returned AddRef'ed.  The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:
   int IQDK30MANAGER_QueryInterface(IQDK30Manager *po, AEECLSID id, void **p)

Parameters:
   po [in]: Pointer to IQDK30Manager interface

   id [in]: A globally unique id to identify the entity (interface or data) that we
            are trying to query.

   p [out]: Pointer to the data or interface that we want to retrieve.  If the interface
            is not available, this is set to NULL.

Return Value:
   Return SUCCESS on success,
   ECLASSNOTSUPPORT if class ID not supported

Comments:
   On failure, QueryInterface() must set *p to NULL.

Version:
   Introduced in QDK v 3.0

Side Effects:
   None

See Also:
   None

=============================================================================

IQDK30MANAGER_Init()
         
Description:
   This function must be called by any application using the IQDK30Manager
   interface and should be called immediately after the IQDK object is
   created so that the QChat Client can perform initialization. 
                                                         
Prototype:
   IQDK_ErrorType IQDK30MANAGER_Init
   (
      IQDK30Manager *pIQDK30Manager,
      AEECLSID appClsID,
   )

Parameters:
   pIQDK30Manager [in]   :  Pointer to the IQDK30Manager Interface object.
   appClsID [in]:  Class ID of the application intending to use the IQDK
                   interface.
      
Return Value:
   SUCCESS: The operation was successful.
   EFAILED: General failure
     
Version:
   Introduced in QDK v 3.0

See Also:
   EVT_QCHAT_STATUS

=====================================================================

IQDK30MANAGER_GetQChatStatus()
              
Description:
   This function is called to discover the current operating status of
   the QChat Client.  The QDK application typically calls
   IQDK30MANAGER_GetQChatStatus() when the QDK application has been brought to the
   foreground as the frontmost BREW application.  The QDK application may
   then update its display to reflect the current operating condition of
   QChat.
             
Prototype:
   IQDK_ErrorType (*GetQChatStatus)(IQDK30Manager *p, IQDK_QChatStatusEventType *status);
            
Parameters:
   p  [in] :  Pointer to the IQDK30Manager Interface object.
   status [out] :  Pointer to an IQDK_QChatStatusEventType data structure where
      information about the current operational status of the QChat
      Core will be stored.  The status of QChat is reflected in the
      state field of the IQDK_QChatStatusEventType structure.

      The IQDK_QChatStatusEventType data structure also provides information
      about user preferences with regard to the preferred addressing, 
      sound mode and termination options. 
      The structure also provides information on availability of
      upgraded version of the QChat Client software.
                
Return Value:
   SUCCESS   :  The QChat Client was able to successfully
      retrieve status of the QChat service.
   EFAILED   :  There was an error retrieving the QChat status.
              
Comments:
   IQDK30MANAGER_GetQChatStatus() is intended to be used by the QDK application when
   it is brought to the foreground as a means of learning the
   current operational status of the QChat Client.  The QDK application should
   not use this routine to poll for status changes.  Rather, the QDK application is
   encouraged to respond to status changes sent to the QDK application from
   the QChat Client in IQDK_EVT_QCHAT_STATUS events.

Version:
   Introduced in QDK v 3.0
             
See Also:
   IQDK_EVT_QCHAT_STATUS
   IQDK_ErrorType
   IQDK_QChatServiceType
   IQDK_QChatStatusEventType
   
=====================================================================

IQDK30MANAGER_GetSetting()

Description:
   This function is used to get a specified carrier-defined setting of the QChat Client.

Prototype:
   IQDK_ErrorType IQDK30MANAGER_GetSetting(IQDK30Manager *p, IQDKMGR_Setting setting, IQDKMGR_SettingType * settingData);

Parameters:
   p [in] : Pointer to the IQDK30Manager interface object
   setting [in] : The setting type to retrieve
   settingData [out] : The retrieved setting is populated in this field

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the QChat setting.
   EBADCLASS: The IQDK30Manager's class ID is not supported.
   EBADPARM: The requested setting is not supported.
   EFAILED   :  There was an error retrieving the QChat setting.
   
Comments:
  None

Version:
  Introduced in QDK v 3.0

See Also:
IQDKMGR_Setting
IQDKMGR_SettingType

=====================================================================

IQDK30MANAGER_GetPref()

Description:
   This function is used to get a specified preference of the QChat Client.

Prototype:
   IQDK_ErrorType IQDK30MANAGER_GetPref(IQDK30Manager *p, IQDKMGR_Pref pref, IQDKMGR_PrefType * prefData);

Parameters:
   p [in] : Pointer to the IQDK30Manager interface object
   pref [in] : The preference type to retrieve
   prefData [out] : The retrieved preference is populated in this field

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the QChat preference.
   EBADCLASS: The IQDK30Manager's class ID is not supported.
   EBADPARM: The requested preference is not supported.
   EFAILED   :  There was an error retrieving the QChat preference.
   
Comments:
  None

Version:
  Introduced in QDK v 3.0

See Also:
IQDKMGR_Pref
IQDKMGR_PrefType

=====================================================================

IQDK30MANAGER_SetPref()

Description:
   This function is used to set a specified preference of the QChat Client.

Prototype:
   IQDK_ErrorType IQDK30MANAGER_SetPref(IQDK30Manager *p, IQDKMGR_Pref pref, IQDKMGR_PrefType * prefData);

Parameters:
   p [in] : Pointer to the IQDK30Manager interface object
   pref [in] : The preference type to set
   prefData [in] : The preference data 

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the QChat preference.
   EBADCLASS: The IQDK30Manager's class ID is not supported.
   EBADPARM: The requested preference is not supported.
   EFAILED   :  There was an error retrieving the QChat preference.
   
Comments:
  None

Version:
  Introduced in QDK v 3.0

See Also:
IQDKMGR_Pref
IQDKMGR_PrefType

=====================================================================

IQDK30MANAGER_GetQChatAddressByIndex()

Description:
   This function is used to retrieve a QChat address given the address' index.

Prototype:
   IQDK_ErrorType IQDK30MANAGER_GetQChatAddressByIndex(IQDK30Manager *pMe,
                                            uint8 addrIndex,
                                            IQDKAddressData *addr);
Parameters:
   pMe [in] : Pointer to the IQDK30Manager interface object
   addrIndex [in] : The index of the address to retrieve
   addr [out] : The retrieved address data

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the specified QChat address.
   EBADCLASS: The IQDK30Manager's class ID is not supported.
   EBADPARM: The requested address parameter is not supported.
   EFAILED   :  There was an error retrieving the QChat address.
   
Comments:
  None

Version:
  Introduced in QDK v 3.0

=====================================================================

IQDK30MANAGER_GetQChatAddressCount()

Description:
   This function is used to retrieve the number of provisioned QChat addresses available.

Prototype:
   IQDK_ErrorType IQDK30MANAGER_GetQChatAddressCount(IQDK30Manager *pMe, uint8 *addrCount);

Parameters:
   pMe [in] : Pointer to the IQDK30Manager interface object
   addrCount [out] : The retrieved number of addresses is populated in this field

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the QChat address count.
   EBADCLASS: The IQDK30Manager's class ID is not supported.
   EFAILED   :  There was an error retrieving the QChat address count.
   
Comments:
  None

Version:
  Introduced in QDK v 3.0

=====================================================================

IQDK30MANAGER_GetAvailabilityStateCount()

Description:
   This function is used to retrieve the number of provisioned QChat availability states.

Prototype:
   IQDK_ErrorType IQDK30MANAGER_GetAvailabilityStateCount(IQDK30Manager* pMe, uint8* count);

Parameters:
   pMe [in] : Pointer to the IQDK30Manager interface object
   count [out] : The retrieved number of availability states is populated in this field

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the QChat availability state count.
   EBADCLASS: The IQDK30Manager's class ID is not supported.
   EBADPARM: The requested availability state count parameter is not supported.
   EFAILED   :  There was an error retrieving the QChat availability state count.
   
Comments:
  None

Version:
  Introduced in QDK v 3.0

=====================================================================

IQDK30MANAGER_GetAvailabilityStateEntryByIndex()

Description:
   This function is used to retrieve the QChat availability state given the state's index.

Prototype:
   IQDK_ErrorType IQDK30MANAGER_GetAvailabilityStateEntryByIndex(IQDK30Manager* pMe,
                                                      uint8 index,
                                                      IQDK_AvailabilityEntry **entry);

Parameters:
   pMe [in] : Pointer to the IQDK30Manager interface object
   index [in] : The index of the availability state to retrieve
   entry [out] : The retrieved availability state is populated in this field

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the QChat availability state.
   EBADCLASS: The IQDK30Manager's class ID is not supported.
   EBADPARM: The requested availability state parameter is not supported.
   EFAILED   :  There was an error retrieving the QChat availability state.
   
Comments:
  None

Version:
  Introduced in QDK v 3.0

=====================================================================

IQDK30MANAGER_GetEventPayload()

Description:
   The event payload is not sent as part of the event.
   The application can get the event payload through the GetEventPayload API. This method allows backward
   compatibility. The event payload data structure could change from one QChat client release to the next. But
   the event does not have to be changed.

   This interface returns the payload of any of the events requested in the eventType.
   The payload returned is for any one of the following events:

   typedef union
   {
      IQDK_QChatStatusEventType statusEventType; 
   } IQDK30_ManagerEvtPayload;
   
   The GetEventPayload allows QDK applications to request the event information
   explicitly from the QChat Client, for a specific event type and for a specific
   version of that event.

Prototype:
  IQDK_ErrorType IQDK30MANAGER_GetEventPayload(
       IQDK30Manager *pMe,
       uint32 interfaceId,
       uint16 eventType,
       IQDK30_ManagerEvtPayload **payload);

Parameters:
  
   pMe :		    Pointer to the IQDK30Manager Interface object.
   interfaceId:	    ID number of the Manager interface
   eventType :	    This field specifies the type of event to retrieve.  The
				    value specified in the eventType field is used to select one
					of the event structures within the payload union.
   payload:		    This field is populated by the payload of the event 
					associated with the eventType. While the 'payload' pointer 
	                itself must not be NULL, the pointer pointed to by the payload
	                pointer must be NULL. This is the event payload that will be 
	                allocated by the QChat Client and deleted by the QDK Application
	                calling this API.
Return Value:

   SUCCESS:		        Successful retrieval of the event payload.
   EQDKNOTINITIALIZED:	pMe is not initialised.
   EBADPARM:		    If the input parameters are not valid.
   EFAILED:		        Generic failure

   
Comments:
  The payload pointer is deleted by the QDK application that requests the information.

Version:
  Introduced in QDK v 3.0

=====================================================================

IQDK30MANAGER_SetForegroundAppClassId()

Description:
   This function is used to set the class ID of the QDK application that expects
   to take the foreground. In the case that multiple QDK applications are running 
   concurrently within QChat, this API allows one of these applications to register
   itself as the one to come to the foreground to handle events.  It is suggested 
   that applications check the current registration with IQDK30MANAGER_GetForegroundAppClassId(), 
   and make a decision about whether to overwrite the currently-registered application's
   next-to-foreground status with its own class ID.

Prototype:
   IQDK_ErrorType IQDK30MANAGER_SetForegroundAppClassId(IQDK30Manager *pMe, uint32 classId);

Parameters:
   pMe [in] : Pointer to the IQDK30Manager interface object
   classId [in] : The class ID of the application that expects to take the foreground next.

Return Value:
   SUCCESS  :  The QChat Client was able to successfully set the foreground app's class ID.
   EFAILED  :  There was an error setting the forground app's class ID.
   
Comments:
  None

Version:
  Introduced in QDK v 2.0

See Also:
   IQDK30MANAGER_GetForegroundAppClassID()

=====================================================================

IQDK30MANAGER_GetForegroundAppClassId()

Description:
   This function is used to retrieve the class ID of the QDK application that expects to
   take the foreground.  In the case that multiple QDK applications are running concurrently
   within QChat, this API in conjunction with IQDK30MANAGER_SetForegroundAppClassId() allows 
   one of these applications to register itself as the one to come to the foreground to handle 
   events.  It is suggested that applications check the current registration first, and make 
   a decision about whether to overwrite the currently-registered application's next-to-foreground 
   status with its own class ID.

Prototype:
   IQDK_ErrorType IQDK30MANAGER_GetForegroundAppClassId(IQDK30Manager *pMe, uint32 * classId);

Parameters:
   pMe [in]: Pointer to the IQDK30Manager interface object
   classId [out] : The retrieved class ID of the application that expects to take the foreground next.

Return Value:
   SUCCESS   :  The QChat Client was able to successfully
      retrieve the event payload.
   EFAILED   :  There was an error retrieving the event payload.
   
Comments:
  None

Version:
  Introduced in QDK v 3.0

See Also:
   IQDK30MANAGER_SetForegroundAppClassID()

=====================================================================

IQDK30MANAGER_GetCallTypeCapabilities() 

Description:
   This function is used to retrieve the call type capability currently
   provisioned into the QChat Client.

Prototype:
   IQDK_ErrorType IQDK30MANAGER_GetCallTypeCapabilities(IQDK30Manager* pMe, uint16* callTypeCapab)

Parameters:
   pMe [in] : Pointer to the IQDK30Manager interface object
   callTypeCapab [out] : The retrieved call type capability value populated
   by the QChat Client.

Return Value:
   SUCCESS:  The QChat Client was able to successfully respond with 
             the call type capability.
   EBADPARM:  There was an error in the input parameters to the 
              API. Either the IQDK30Manager interface object is NULL or 
              the callTypeCapab field is NULL.
   EQDKNOTINITIALIZED: This status is returned when IQDK30MANAGER_Init() has not been 
                       called by the QDK application, which causes IQDK30Manager data  
                       to remain uninitialized.

Comments:
   The call type capability value is a 16 bit mask. The mask is defined 
   as indicated below.
   The four hex digits represent 16 bits, one bit per call type. 
   Starting from the least significant bit, each bit represents the following 
   call types in order:
   Alert origination, Alert participation, Direct Call Origination, Direct Call participation, 
   Ad hoc group call origination, Ad hoc call participation, Predefined group call origination,
   Predefined group call participation, Chat room call origination, Chat room call participation 
   The remaining bits are reserved for future use.

   The bits in the bitmask correspond to call types like this:
   Alert origination                   binary : 0000000000000001   hex: 0x0001
   Alert participation                 binary : 0000000000000010   hex: 0x0002
   Direct Call Origination             binary : 0000000000000100   hex: 0x0004
   Direct Call participation           binary : 0000000000001000   hex: 0x0008
   Ad hoc group call origination       binary : 0000000000010000   hex: 0x0010
   Ad hoc call participation           binary : 0000000000100000   hex: 0x0020
   Predefined group call origination   binary : 0000000001000000   hex: 0x0040
   Predefined group call participation binary : 0000000010000000   hex: 0x0080
   Chat room call origination          binary : 0000000100000000   hex: 0x0100
   Chat room call participation        binary : 0000001000000000   hex: 0x0200
   Multicast group call origination    binary : 0000010000000000   hex: 0x0400
   Multicast group call participation  binary : 0000100000000000   hex: 0x0800

   Default value is 0x0FFF

Version:
  Introduced in QDK v 3.0

See Also:
   None. 

=====================================================================*/
