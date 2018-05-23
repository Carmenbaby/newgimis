#ifndef AEEQDKMANAGER_H 
#define AEEQDKMANAGER_H
/*=============================================================================
 
FILE: AEEIQDKManager.h

SERVICES: The IQDKManager interface manages the interfaces that are common to all
          applications.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IQDKManager

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================*/
#include "AEE.h"
#include "AEEQDKAddress.h"
#include "AEEQDKDataTypes.h"
#include "AEEIQDKManager.bid"

#define IQDK_MAX_VERSION      16
#define IQDK_MAX_PREFIX       16
#define IQDK_MAX_MIMETYPE    256

#define AEEIID_IQDKMANAGER 0x01052f30 //Interface ID for 2.0 QueryInterface

/*==============================================================================
   DATA STRUCTURES
==============================================================================*/

typedef uint16     IQDKMGR_Setting;
#define IQDKMGR_SETTING_QCHAT_VERSION                            0
#define IQDKMGR_SETTING_QCHAT_MIME_TYPE                          1
#define IQDKMGR_SETTING_PREFIX_PREDEF_GROUP                      2
#define IQDKMGR_SETTING_PREFIX_CHATROOM                          3
#define IQDKMGR_SETTING_PRIV_ADD_MEMBER_DISABLE                  4
#define IQDKMGR_SETTING_DEFAULT_ADD_MEMBER_DISABLE               5

typedef AECHAR     IQDKMGR_Version[IQDK_MAX_VERSION];
typedef AECHAR     IQDKMGR_GroupPrefix[IQDK_MAX_PREFIX];
typedef AECHAR     IQDKMGR_ChatroomPrefix[IQDK_MAX_PREFIX];
typedef AECHAR     IQDKMGR_MimeType[IQDK_MAX_MIMETYPE];

typedef union
{
   IQDKMGR_Version          version;
   IQDKMGR_GroupPrefix      groupPrefix;
   IQDKMGR_ChatroomPrefix   chatPrefix;
   IQDKMGR_MimeType         mimetype;
   boolean                  addMemberDisablePriv;
   boolean                  defaultAddMemberDisableOption;
} IQDKMGR_SettingType;

typedef union
{
   IQDK_QChatStatusEventType statusEventType; 
} IQDK_ManagerEvtPayload;

typedef uint16     IQDKMGR_Pref;
#define IQDKMGR_PREF_AVAILABILITY_STATE                          0
#define IQDKMGR_PREF_POWER_LEVEL                                 1
#define IQDKMGR_PREF_VOICEMAIL_TREATMENT                         2
#define IQDKMGR_PREF_GRP_CALL_MEMBERS_DETAIL_EVENT_ENABLE        3
//#define IQDKMGR_PREF_RECALL_FEATURE_SUPPORT						 4   //To enable or disable the RECALL Feature

typedef union
{
   IQDK_AvailabilityStateType  availabilityState;
   IQDK_PowerMgmtLevel         powerLevel;
   boolean                     voiceMailTreatment;
   boolean                     grpCallMemDetailEvtEnable;
} IQDKMGR_PrefType;

/*=============================================================================
                    CLASS DEFINITIONS
=============================================================================*/

typedef struct IQDKManager IQDKManager;

AEEINTERFACE(IQDKManager)
{
   INHERIT_IQueryInterface(IQDKManager);

   IQDK_ErrorType (*Init)(IQDKManager *pMe, AEECLSID appClsID);

   IQDK_ErrorType (*GetQChatStatus)(IQDKManager *p, IQDK_QChatStatusEventType *status);

   IQDK_ErrorType (*GetSetting)(IQDKManager *p, IQDKMGR_Setting setting, IQDKMGR_SettingType * settingData);
   IQDK_ErrorType (*GetPref)(IQDKManager *p, IQDKMGR_Pref pref, IQDKMGR_PrefType * prefData);
   IQDK_ErrorType (*SetPref)(IQDKManager *p, IQDKMGR_Pref pref, IQDKMGR_PrefType * prefData);

   IQDK_ErrorType (*GetQChatAddressByIndex)(IQDKManager *pMe,
                                            uint8 addrIndex,
                                            IQDKAddressData *addr);
   IQDK_ErrorType (*GetQChatAddressCount)(IQDKManager *pMe, uint8 *addrCount);

   IQDK_ErrorType (*GetAvailabilityStateCount)(IQDKManager* pMe, uint8* count);
   IQDK_ErrorType (*GetAvailabilityStateEntryByIndex)(IQDKManager* pMe,
                                                      uint8 index,
                                                      IQDK_AvailabilityEntry **entry);
   IQDK_ErrorType (*GetCallTypeCapabilities)(IQDKManager* pMe, uint16* callTypeCapab);

   IQDK_ErrorType (*GetEventPayload)(IQDKManager *pMe, uint32 interfaceId, uint16 eventType, IQDK_ManagerEvtPayload **payload);
};

#define IQDKMANAGER_AddRef(p)                                AEEGETPVTBL((p),IQDKManager)->AddRef((p))
#define IQDKMANAGER_Release(p)                               AEEGETPVTBL((p),IQDKManager)->Release((p))
#define IQDKMANAGER_QueryInterface(p,clsid,pp)               AEEGETPVTBL((p),IQDKManager)->QueryInterface((p),(clsid),(pp))
#define IQDKMANAGER_Init(p,clsid)                            AEEGETPVTBL((p),IQDKManager)->Init((p),(clsid))
#define IQDKMANAGER_GetQChatStatus(p, s)                     AEEGETPVTBL((p),IQDKManager)->GetQChatStatus((p),(s))
#define IQDKMANAGER_GetSetting(p,s,d)                        AEEGETPVTBL((p),IQDKManager)->GetSetting((p),(s),(d))
#define IQDKMANAGER_SetPref(p,pf,d)                          AEEGETPVTBL((p),IQDKManager)->SetPref((p),(pf),(d))
#define IQDKMANAGER_GetPref(p,pf,d)                          AEEGETPVTBL((p),IQDKManager)->GetPref((p),(pf),(d))
#define IQDKMANAGER_GetQChatAddressByIndex(p,i,a)            AEEGETPVTBL((p),IQDKManager)->GetQChatAddressByIndex((p),(i),(a))
#define IQDKMANAGER_GetQChatAddressCount(p,i)                AEEGETPVTBL((p),IQDKManager)->GetQChatAddressCount((p),(i))
#define IQDKMANAGER_GetAvailabilityStateCount(p,c)           AEEGETPVTBL((p),IQDKManager)->GetAvailabilityStateCount((p),(c))
#define IQDKMANAGER_GetAvailabilityStateEntryByIndex(p,i,e)  AEEGETPVTBL((p),IQDKManager)->GetAvailabilityStateEntryByIndex((p),(i),(e))
#define IQDKMANAGER_GetEventPayload(p,i,e,a)                 AEEGETPVTBL((p),IQDKManager)->GetEventPayload((p),(i),(e),(a))
#define IQDKMANAGER_GetCallTypeCapabilities(p,c)             AEEGETPVTBL((p),IQDKManager)->GetCallTypeCapabilities((p),(c))

#endif // AEEQDKMANAGER_H

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================

================================================================================

IQDKMGR_Setting

Description:
   This field is a bitmask containing information on the QChat manager settings.

Definition:
   typedef uint16 IQDKMGR_Setting;
   #define IQDKMGR_SETTING_QCHAT_VERSION                            0
   #define IQDKMGR_SETTING_QCHAT_MIME_TYPE                          1
   #define IQDKMGR_SETTING_PREFIX_PREDEF_GROUP                      2
   #define IQDKMGR_SETTING_PREFIX_CHATROOM                          3
   #define IQDKMGR_SETTING_PRIV_ADD_MEMBER_DISABLE                  4
   #define IQDKMGR_SETTING_DEFAULT_ADD_MEMBER_DISABLE               5

Members:
   IQDKMGR_SETTING_QCHAT_VERSION:              QChat Client current version
   IQDKMGR_SETTING_QCHAT_MIME_TYPE:            QChat Client MIME type information
   IQDKMGR_SETTING_PREFIX_PREDEF_GROUP:        QChat predefined group prefix
   IQDKMGR_SETTING_PREFIX_CHATROOM:            QChat chatroom address prefix
   IQDKMGR_SETTING_PRIV_ADD_MEMBER_DISABLE:    Whether the Add Member Disable feature can be disabled
   IQDKMGR_SETTING_DEFAULT_ADD_MEMBER_DISABLE: Default value for Add Member Disable feature
See Also:


================================================================================
IQDKMGR_Version

Description:
   This field contains information on the QChat Client version.

Definition:
   typedef AECHAR     IQDKMGR_Version[IQDK_MAX_VERSION];

Members:
   N/A

See Also:
   IQDKMGR_SettingType

================================================================================

IQDKMGR_GroupPrefix

Description:
   This field contains information on the QChat predefined group 
   prefix.  For example, in the group name "group23.qualcomm.com," 
   the predefined group prefix is "group23." 

Definition:
   typedef AECHAR     IQDKMGR_GroupPrefix[IQDK_MAX_PREFIX];

Members:
   N/A

See Also:
   IQDKMGR_SettingType

================================================================================
IQDKMGR_ChatroomPrefix

Description:
   This field contains information on the QChat chatroom prefix.
   For example, in the group name "chatroom.qualcomm.com," the
   chatroom prefix is "chatroom." 

Definition:
   typedef AECHAR     IQDKMGR_ChatroomPrefix[IQDK_MAX_PREFIX];

Members:
   N/A

See Also:
   IQDKMGR_SettingType

================================================================================
IQDKMGR_MimeType

Description:
   This field contains the MIME-Type information for the QChat
   Client. 

Definition:
   typedef AECHAR     IQDKMGR_MimeType[IQDK_MAX_MIMETYPE];

Members:
   N/A

See Also:
   IQDKMGR_SettingType

================================================================================
IQDKMGR_SettingType

Description:
   This structure contains information on the QChat Client settings.
   These settings are expected to be provisioned by the carrier, not
   by the QChat Client.
   Note that this structure is a union, and only one setting can be
   accessed at a time. 

Definition:
   typedef union
   {
      IQDKMGR_Version          version;
      IQDKMGR_GroupPrefix      groupPrefix;
      IQDKMGR_ChatroomPrefix   chatPrefix;
      IQDKMGR_MimeType         mimetype;
      boolean                  addMemberDisablePriv;
      boolean                  defaultAddMemberDisableOption;
   } IQDKMGR_SettingType;

Members:
   version - The version of the currently-running QChat Client
   groupPrefix - The prefix string for predefined groups
   chatPrefix - The prefix string for chatrooms
   mimetype - The QChat Client's MIME-Type information
   addMemberDisablePriv - Whether the Add Member Disable feature can be disabled by the client
   defaultAddMemberDisableOption - Default value for Add Member Disable feature

See Also:
   IQDKMGR_Version
   IQDKMGR_GroupPrefix
   IQDKMGR_ChatroomPrefix
   IQDKMGR_MimeType

================================================================================

IQDK_ManagerEvtPayload

Description:
   This structure contains the event payload retrieved with an 
   IQDKMANAGER_GetEventPayload API call.

Definition:
   typedef union
   {
      IQDK_QChatStatusEventType statusEventType; 
   } IQDK_ManagerEvtPayload;

Members:
   statusEventType - Contains fields relating to the status and 
                     user preferences of the QChat Client

See Also:
   IQDKMANAGER_GetEventPayload()
   IQDK_QChatStatusEventType 

================================================================================

IQDKMGR_Pref

Description:
   This field is a bitmask containing QChat Manager preferences. 

Definition:
   typedef uint16     IQDKMGR_Pref;
   #define IQDKMGR_PREF_AVAILABILITY_STATE                          0
   #define IQDKMGR_PREF_POWER_LEVEL                                 1
   #define IQDKMGR_PREF_VOICEMAIL_TREATMENT                         2
   #define IQDKMGR_PREF_GRP_CALL_MEMBERS_DETAIL_EVENT_ENABLE        3

Members:
   IQDKMGR_PREF_AVAILABILITY_STATE - QChat Client availability state
   IQDKMGR_PREF_POWER_LEVEL - QChat Client power level
   IQDKMGR_PREF_VOICEMAIL_TREATMENT - Voicemail enabled/disabled
   IQDKMGR_PREF_GRP_CALL_MEMBERS_DETAIL_EVENT_ENABLE - Group in-Call Member Details event status (Enabled/Disabled)
See Also:
   IQDKMGR_PrefType

================================================================================
IQDKMGR_PrefType

Description:
   This structure allows access to QChat Manager preferences.  Note
   that the structure is a union; access to only one preference at a
   time is allowed. 

Definition:
   typedef union
   {
      IQDK_AvailabilityStateType  availabilityState;
      IQDK_PowerMgmtLevel         powerLevel;
      boolean                     voiceMailTreatment;
      boolean                     grpCallMemDetailEvtEnable;
   } IQDKMGR_PrefType;

Members:
   availabilityState - The availability state of the QChat Client
   powerLevel - The power level at which the QChat Client is operating
   voiceMailTreatment - Disable/Enable voicemail 
   grpCallMemDetailEvtEnable - Disable/Enable group in-call status events 

See Also:
   IQDK_AvailabilityStateType
   IQDK_PowerMgmtLevel
   IQDK_EVT_DETAIL_GRP_CALL_INFO


================================================================================
  INTERFACES   DOCUMENTATION
================================================================================

Interface Name: IQDKManager

Description:
    IQDKManager interface is a set of common API methods that are intended to be
used by any application that wants to use the services of the QChat System. Some
of the APIs in this interface include proper initialization with the QChat Client
and querying the state of the QChat Client.

The IQDKManager interface instance is obtained via ISHELL_CreateInstance()
mechanism.

================================================================================

IQDKMANAGER_AddRef()

Description:
   This method increments the reference count of the IQDKManager Interface
   object.  This allows the object to be shared by multiple callers. The
   object is freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKMANAGER_AddRef(IQDKManager * po)

Parameters:

   po [in]: Pointer to the IQDKManager Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDKMANAGER_Release()

=============================================================================

IQDKMANAGER_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKMANAGER_Release(IQDKManager * po)

Parameters:

   po [in]: Pointer to the IQDKManager Interface object

Return Value:
   Decremented reference count for the object. The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDKMANAGER_AddRef()

==============================================================================

IQDKMANAGER_QueryInterface()

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
   int IQDKMANAGER_QueryInterface(IQDKManager *po, AEECLSID id, void **p)

Parameters:
   po [in]: Pointer to IQDKManager interface

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
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   None

=============================================================================

IQDKMANAGER_Init()
         
Description:
   This function must be called by any application using the IQDKManager
   interface and should be called immediately after the IQDK object is
   created so that the QChat Client can perform initialization. 
                                                         
Prototype:
   IQDK_ErrorType IQDKMANAGER_Init
   (
      IQDKManager *pIQDKManager,
      AEECLSID appClsID,
   )

Parameters:
   pIQDK [in]   :  Pointer to the IQDKManager Interface object.
   appClsID [in]:  Class ID of the application intending to use the IQDK
                   interface.
      
Return Value:
   SUCCESS: The operation was successful.
   EFAILED: General failure
     
Version:
   Introduced in QDK v 2.0

See Also:
   EVT_QCHAT_STATUS

=====================================================================

IQDKMANAGER_GetQChatStatus()
              
Description:
   This function is called to discover the current operating status of
   the QChat Client.  The QDK application typically calls
   IQDKMANAGER_GetQChatStatus() when the QDK application has been brought to the
   foreground as the frontmost BREW application.  The QDK application may
   then update its display to reflect the current operating condition of
   QChat.
             
Prototype:
   IQDK_ErrorType (*GetQChatStatus)(IQDKManager *p, IQDK_QChatStatusEventType *status);
            
Parameters:
   p  [in] :  Pointer to the IQDKManager Interface object.
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
   IQDKMANAGER_GetQChatStatus() is intended to be used by the QDK application when
   it is brought to the foreground as a means of learning the
   current operational status of the QChat Client.  The QDK application should
   not use this routine to poll for status changes.  Rather, the QDK application is
   encouraged to respond to status changes sent to the QDK application from
   the QChat Client in IQDK_EVT_QCHAT_STATUS events.

Version:
   Introduced in QDK v 2.0
             
See Also:
   IQDK_EVT_QCHAT_STATUS
   IQDK_ErrorType
   IQDK_QChatServiceType
   IQDK_QChatStatusEventType
   
=====================================================================

IQDKMANAGER_GetSetting()

Description:
   This function is used to get a specified carrier-defined setting of the QChat Client.

Prototype:
   IQDK_ErrorType IQDKMANAGER_GetSetting(IQDKManager *p, IQDKMGR_Setting setting, IQDKMGR_SettingType * settingData);

Parameters:
   p [in] : Pointer to the IQDKManager interface object
   setting [in] : The setting type to retrieve
   settingData [out] : The retrieved setting is populated in this field

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the QChat setting.
   EBADCLASS: The IQDKManager's class ID is not supported.
   EBADPARM: The requested setting is not supported.
   EFAILED   :  There was an error retrieving the QChat setting.
   
Comments:
  None

Version:
  Introduced in QDK v 2.0

See Also:
IQDKMGR_Setting
IQDKMGR_SettingType

=====================================================================

IQDKMANAGER_GetPref()

Description:
   This function is used to get a specified preference of the QChat Client.

Prototype:
   IQDK_ErrorType IQDKMANAGER_GetPref(IQDKManager *p, IQDKMGR_Pref pref, IQDKMGR_PrefType * prefData);

Parameters:
   p [in] : Pointer to the IQDKManager interface object
   pref [in] : The preference type to retrieve
   prefData [out] : The retrieved preference is populated in this field

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the QChat preference.
   EBADCLASS: The IQDKManager's class ID is not supported.
   EBADPARM: The requested preference is not supported.
   EFAILED   :  There was an error retrieving the QChat preference.
   
Comments:
  None

Version:
  Introduced in QDK v 2.0

See Also:
IQDKMGR_Pref
IQDKMGR_PrefType

=====================================================================

IQDKMANAGER_SetPref()

Description:
   This function is used to set a specified preference of the QChat Client.

Prototype:
   IQDK_ErrorType IQDKMANAGER_SetPref(IQDKManager *p, IQDKMGR_Pref pref, IQDKMGR_PrefType * prefData);

Parameters:
   p [in] : Pointer to the IQDKManager interface object
   pref [in] : The preference type to set
   prefData [in] : The preference data 

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the QChat preference.
   EBADCLASS: The IQDKManager's class ID is not supported.
   EBADPARM: The requested preference is not supported.
   EFAILED   :  There was an error retrieving the QChat preference.
   
Comments:
  None

Version:
  Introduced in QDK v 2.0

See Also:
IQDKMGR_Pref
IQDKMGR_PrefType

=====================================================================

IQDKMANAGER_GetQChatAddressByIndex()

Description:
   This function is used to retrieve a QChat address given the address' index.

Prototype:
   IQDK_ErrorType IQDKMANAGER_GetQChatAddressByIndex(IQDKManager *pMe,
                                            uint8 addrIndex,
                                            IQDKAddressData *addr);
Parameters:
   pMe [in] : Pointer to the IQDKManager interface object
   addrIndex [in] : The index of the address to retrieve
   addr [out] : The retrieved address data

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the specified QChat address.
   EBADCLASS: The IQDKManager's class ID is not supported.
   EBADPARM: The requested address parameter is not supported.
   EFAILED   :  There was an error retrieving the QChat address.
   
Comments:
  None

Version:
  Introduced in QDK v 2.0

=====================================================================

IQDKMANAGER_GetQChatAddressCount()

Description:
   This function is used to retrieve the number of provisioned QChat addresses available.

Prototype:
   IQDK_ErrorType IQDKMANAGER_GetQChatAddressCount(IQDKManager *pMe, uint8 *addrCount);

Parameters:
   pMe [in] : Pointer to the IQDKManager interface object
   addrCount [out] : The retrieved number of addresses is populated in this field

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the QChat address count.
   EBADCLASS: The IQDKManager's class ID is not supported.
   EFAILED   :  There was an error retrieving the QChat address count.
   
Comments:
  None

Version:
  Introduced in QDK v 2.0

=====================================================================

IQDKMANAGER_GetAvailabilityStateCount()

Description:
   This function is used to retrieve the number of provisioned QChat availability states.

Prototype:
   IQDK_ErrorType IQDKMANAGER_GetAvailabilityStateCount(IQDKManager* pMe, uint8* count);

Parameters:
   pMe [in] : Pointer to the IQDKManager interface object
   count [out] : The retrieved number of availability states is populated in this field

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the QChat availability state count.
   EBADCLASS: The IQDKManager's class ID is not supported.
   EBADPARM: The requested availability state count parameter is not supported.
   EFAILED   :  There was an error retrieving the QChat availability state count.
   
Comments:
  None

Version:
  Introduced in QDK v 2.0

=====================================================================

IQDKMANAGER_GetAvailabilityStateEntryByIndex()

Description:
   This function is used to retrieve the QChat availability state given the state's index.

Prototype:
   IQDK_ErrorType IQDKMANAGER_GetAvailabilityStateEntryByIndex(IQDKManager* pMe,
                                                      uint8 index,
                                                      IQDK_AvailabilityEntry **entry);

Parameters:
   pMe [in] : Pointer to the IQDKManager interface object
   index [in] : The index of the availability state to retrieve
   entry [out] : The retrieved availability state is populated in this field

Return Value:
   SUCCESS   :  The QChat Client was able to successfully retrieve the QChat availability state.
   EBADCLASS: The IQDKManager's class ID is not supported.
   EBADPARM: The requested availability state parameter is not supported.
   EFAILED   :  There was an error retrieving the QChat availability state.
   
Comments:
  None

Version:
  Introduced in QDK v 2.0

=====================================================================

IQDKMANAGER_GetEventPayload()

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
   } IQDK_ManagerEvtPayload;
   
   The GetEventPayload allows QDK applications to request the event information
   explicitly from the QChat Client, for a specific event type and for a specific
   version of that event.

Prototype:
  IQDK_ErrorType IQDKMANAGER_GetEventPayload(
       IQDKManager *pMe,
       uint32 interfaceId,
       uint16 eventType,
       IQDK_ManagerEvtPayload **payload);

Parameters:
  
   pMe :		    Pointer to the IQDKManager Interface object.
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
  Introduced in QDK v 2.0

=====================================================================

IQDKMANAGER_SetForegroundAppClassId()

Description:
   This function is used to set the class ID of the QDK application that expects
   to take the foreground. In the case that multiple QDK applications are running 
   concurrently within QChat, this API allows one of these applications to register
   itself as the one to come to the foreground to handle events.  It is suggested 
   that applications check the current registration with IQDKMANAGER_GetForegroundAppClassId(), 
   and make a decision about whether to overwrite the currently-registered application's
   next-to-foreground status with its own class ID.

Prototype:
   IQDK_ErrorType IQDKMANAGER_SetForegroundAppClassId(IQDKManager *pMe, uint32 classId);

Parameters:
   pMe [in] : Pointer to the IQDKManager interface object
   classId [in] : The class ID of the application that expects to take the foreground next.

Return Value:
   SUCCESS  :  The QChat Client was able to successfully set the foreground app's class ID.
   EFAILED  :  There was an error setting the forground app's class ID.
   
Comments:
  None

Version:
  Introduced in QDK v 2.0

See Also:
   IQDKMANAGER_GetForegroundAppClassID()

=====================================================================

IQDKMANAGER_GetForegroundAppClassId()

Description:
   This function is used to retrieve the class ID of the QDK application that expects to
   take the foreground.  In the case that multiple QDK applications are running concurrently
   within QChat, this API in conjunction with IQDKMANAGER_SetForegroundAppClassId() allows 
   one of these applications to register itself as the one to come to the foreground to handle 
   events.  It is suggested that applications check the current registration first, and make 
   a decision about whether to overwrite the currently-registered application's next-to-foreground 
   status with its own class ID.

Prototype:
   IQDK_ErrorType IQDKMANAGER_GetForegroundAppClassId(IQDKManager *pMe, uint32 * classId);

Parameters:
   pMe [in]: Pointer to the IQDKManager interface object
   classId [out] : The retrieved class ID of the application that expects to take the foreground next.

Return Value:
   SUCCESS   :  The QChat Client was able to successfully
      retrieve the event payload.
   EFAILED   :  There was an error retrieving the event payload.
   
Comments:
  None

Version:
  Introduced in QDK v 2.0

See Also:
   IQDKMANAGER_SetForegroundAppClassID()

=====================================================================

IQDKMANAGER_GetCallTypeCapabilities() 

Description:
   This function is used to retrieve the call type capability currently
   provisioned into the QChat Client.

Prototype:
   IQDK_ErrorType IQDKMANAGER_GetCallTypeCapabilities(IQDKManager* pMe, uint16* callTypeCapab)

Parameters:
   pMe [in] : Pointer to the IQDKManager interface object
   callTypeCapab [out] : The retrieved call type capability value populated
   by the QChat Client.

Return Value:
   SUCCESS:  The QChat Client was able to successfully respond with 
             the call type capability.
   EBADPARM:  There was an error in the input parameters to the 
              API. Either the IQDKManager interface object is NULL or 
              the callTypeCapab field is NULL.
   EQDKNOTINITIALIZED: This status is returned when IQDKMANAGER_Init() has not been 
                       called by the QDK application, which causes IQDKManager data  
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
   Default value is 0x03FF

Version:
  Introduced in QDK v 2.0

See Also:
   None. 

=====================================================================*/
