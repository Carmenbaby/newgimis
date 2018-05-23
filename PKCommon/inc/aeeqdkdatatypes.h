#ifndef AEEIQDKDATATYPES_H
#define AEEIQDKDATATYPES_H

/*=============================================================================

FILE: AEEQDKCommon.h

SERVICES: The IQDKCommon interface manages the interfaces that are common to all
          applications.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IQDKCommon

        Copyright Ac 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================*/
#include "AEEQDKAddress.h"

#define IQDK_MAX_AVAILABILITY_STRING           80
#define IQDK_ALERT_MAX_STRING_SIZE      120

// The MSB of the value indicats whether this is a valid availability state.
#define IS_VALID_AVAILABILITY_STATE(value) ((uint8)(value) & 0x80)
// The lower 5 bits is the actual presence state
#define GET_AVAILABILITY_STATE(value) ((uint8)(value) & 0x1F)

typedef uint16 IQDK_AvailabilityStateType;

// The following events will be received by the QDK app only if
// the NMASK_QDK_MANAGER mask is specified in IQDKMANAGER_Init()
#define IQDK_EVT_COMMON_BASE                     (EVT_USER)
#define IQDK_EVT_QCHAT_STATUS                    (0x0 + IQDK_EVT_COMMON_BASE)
#define IQDK_EVT_PROV_ITEMS_CHANGED_RESTART      (0x1 + IQDK_EVT_COMMON_BASE)
#define IQDK_EVT_USER_ACTION                     (0x2 + IQDK_EVT_COMMON_BASE)
#define IQDK_EVT_QCHAT_TERMINATE                 (0x3 + IQDK_EVT_COMMON_BASE)

// The following events will be received by the QDK app only if
// the NMASK_QDK_CALL mask is specified in IQDKMANAGER_Init()
#define IQDK_EVT_CALL_BASE                       (0x100 + EVT_USER)
#define IQDK_EVT_CALL_NOTICE                     (0x0 + IQDK_EVT_CALL_BASE)
#define IQDK_EVT_CALL_CONNECTED                  (0x1 + IQDK_EVT_CALL_BASE)
#define IQDK_EVT_END_CALL                        (0x2 + IQDK_EVT_CALL_BASE)
#define IQDK_EVT_CALL_STATUS                     (0x3 + IQDK_EVT_CALL_BASE)
#define IQDK_EVT_MEMBERS_INVITED                 (0x4 + IQDK_EVT_CALL_BASE)
#define IQDK_EVT_INVITE_RECEIVED                 (0x5 + IQDK_EVT_CALL_BASE)

// The following events will be received by the QDK app only if
// the NMASK_QDK_ALERT mask is specified in IQDKMANAGER_Init()
#define IQDK_EVT_ALERT_BASE                      (0x200 + EVT_USER)
#define IQDK_EVT_ALERT_SENT_STATUS               (0x0 + IQDK_EVT_ALERT_BASE)
#define IQDK_EVT_ALERT_RECEIVED                  (0x1 + IQDK_EVT_ALERT_BASE)

// The following events will be received by the QDK app only if
// the NMASK_QDK_MEMBERSHIP mask is specified in IQDKMANAGER_Init()
#define IQDK_EVT_MEMBERSHIP_BASE                 (0x300 + EVT_USER)
//#define IQDK_EVT_PREDEF_GROUP_LIST             (0x0 + IQDK_EVT_MEMBERSHIP_BASE)
#define IQDK_EVT_MEMBER_LIST                     (0x1 + IQDK_EVT_MEMBERSHIP_BASE)
#define IQDK_EVT_DETAIL_GRP_CALL_INFO            (0x2 + IQDK_EVT_MEMBERSHIP_BASE)
#define IQDK_EVT_GROUP_NAME_AVAILABLE            (0x3 + IQDK_EVT_MEMBERSHIP_BASE)

// The following events will be received by the QDK app only if
// the NMASK_QDK_PRESENCE mask is specified in IQDKMANAGER_Init()
#define IQDK_EVT_PRESENCE_BASE                   (0x400 + EVT_USER)
#define IQDK_EVT_PRESENCE_ON_DEMAND              (0x0 + IQDK_EVT_PRESENCE_BASE)

// The following events will be received by the QDK app only if
// the NMASK_QDK_PROVISION mask is specified in IQDKMANAGER_Init()
#define IQDK_EVT_PROVISION_BASE                  (0x500 + EVT_USER)
#define IQDK_EVT_PROVISION_SYNC                  (0x0 + IQDK_EVT_PROVISION_BASE)
#define IQDK_EVT_PROVISION_CANCEL_SYNC           (0x1 + IQDK_EVT_PROVISION_BASE)

#define IQDK_EVT_PTTFWD_BASE                     (0x600 + EVT_USER)
#define IQDK_EVT_PTTFWD_PRIVATE_BASE             (0x50 + IQDK_EVT_PTTFWD_BASE)

#define IQDK_EVT_VMA_BASE                        (0x700 + EVT_USER)
#define IQDK_EVT_VMA_PRIVATE_BASE                (0x0 + IQDK_EVT_VMA_BASE)

// The following events will be received by the QDK app only if
// the NMASK_QDK_AVAILABILITY_NOTIFICATION mask is specified in IQDKCOMMON_Init()
#define IQDK_EVT_AVAILABILITY_BASE               (0x900 + EVT_USER)
#define IQDK_EVT_AVAILABILITY_REQUEST_RECEIVED   (0x0 + IQDK_EVT_AVAILABILITY_BASE)
#define IQDK_EVT_AVAILABILITY_REQUEST_STATUS     (0x1 + IQDK_EVT_AVAILABILITY_BASE)
#define IQDK_EVT_AVAILABILITY_UPDATE_RECEIVED    (0x2 + IQDK_EVT_AVAILABILITY_BASE)
#define IQDK_EVT_AVAILABILITY_UPDATE_STATUS      (0x3 + IQDK_EVT_AVAILABILITY_BASE)


// The following events will be received by the QDK app only if
// the NMASK_QDK_ENHANCED_STATUS mask, 0x1000 is taken, 
// actual definition present in AEEIQDK30Service.h
// 
// #define IQDK_EVT_ENHANCEDSTATUS_DETAILS_BASE                (0x1000 + EVT_USER)
// #define IQDK_EVT_OFFLINE_COMPLETE                      (0x1 + IQDK_EVT_ENHANCEDSTATUS_DETAILS_BASE)
// #define IQDK_EVT_NETWORK_STATUS                        (0x2 + IQDK_EVT_ENHANCEDSTATUS_DETAILS_BASE)
// 
// =============================================================================
// PRIVATE EVENTS - these are the events used between QDK apps.
//
#define IQDK_PRIVATE_EVT_BASE                    (0x2000 + EVT_USER)
#define IQDK_PRIVATE_EVT_VMA_EXIT                (0x0 + IQDK_PRIVATE_EVT_BASE)

typedef uint16 IQDK_ErrorType;
typedef uint16 IQDK_EventType;

typedef enum
{
  IQDK_PWR_MGMT_LEVEL_HIGH,    
  IQDK_PWR_MGMT_LEVEL_MEDIUM,      
  IQDK_PWR_MGMT_LEVEL_LOW,   
  IQDK_PWR_MGMT_LEVEL_MAX
} IQDK_PowerMgmtLevel;

typedef enum
{
   IQDK_SOUND_SILENT_MODE,
   IQDK_SOUND_MANNER_MODE,
   IQDK_SOUND_SPEAKER_MODE
} IQDK_SoundMode;

typedef enum
{
   IQDK_AVAILABILITY_ACTION_ACCEPT_CALLS,
   IQDK_AVAILABILITY_ACTION_REJECT_CALLS
} IQDK_AvailabilityActionType;

typedef struct
{
   AECHAR                          text[IQDK_MAX_AVAILABILITY_STRING];
   IQDK_AvailabilityActionType     action;
   boolean                         userSettable;
} IQDK_AvailabilityEntry;

typedef enum
{
   IQDK_CARRIER,
   IQDK_USER
} IQDK_CallRestrictionLevel;

typedef enum
{
   IQDK_ALWAYS_ALLOW,
   IQDK_ALLOW,
   IQDK_DENY
} IQDK_CallRestrictionType;

typedef enum
{
   IQDK_INBOUND,
   IQDK_OUTBOUND,
   IQDK_INBOUND_AND_OUTBOUND,
   IQDK_NUM_RESTRICTION_DIRECTIONS
} IQDK_CallRestrictionDirection;

typedef uint16 IQDK_ServicesType;

typedef enum
{
   IQDK_ALERTS,
   IQDK_DIRECTCALLS,
   IQDK_GROUPCALLS,
   IQDK_ADHOCGROUPCALLS,
   IQDK_CHATROOMCALLS,
   IQDK_PRESENCEONDEMAND_QUERIES,
   IQDK_AVAILABILITY_INFO,
   IQDK_MAXSERVICES
} IQDK_ServicesEnum;

typedef struct
{
   IQDKAddressData                  token;
   IQDK_CallRestrictionLevel        level;
   IQDK_CallRestrictionType         type;
   IQDK_CallRestrictionDirection    direction;
   IQDK_ServicesType                services;
} IQDK_CallRestrictionEntry;

typedef enum
{
   IQDK_SUCCESS=0,
   IQDK_REJECT_TOO_MANY_TARGETS,
   IQDK_REJECT_USER_UNKNOWN,
   IQDK_REJECT_INVALID_CONF_ID,
   IQDK_REJECT_INVALID_ADDRESS,
   IQDK_REJECT_VOCODER_INCOMPATIBLE,
   IQDK_REJECT_VER_NOT_SUPPORTED,
   IQDK_REJECT_NO_RESOURCES_AVAILABLE,
   IQDK_REJECT_TARGET_BUSY,
   IQDK_REJECT_NOT_A_MEMBER,
   IQDK_REJECT_CALL_NOT_RUNNING,
   IQDK_REJECT_UNSUPPORTED,
   IQDK_REJECT_BLOCKED_USER,
   IQDK_REJECT_UNKNOWN_ERROR,
   IQDK_REJECT_TARGETS_NOT_REACHABLE,
   IQDK_REJECT_ALL_TARGETS_REJECT,
   IQDK_REJECT_NO_CALL_TYPE_CAPABILITY,
   IQDK_REJECT_ALL_INCOMING_CALLS,
   IQDK_REJECT_ALERT_NO_TEXT,
   IQDK_REJECT_NETWORK_DELAY_TIMEOUT,
   IQDK_REJECT_NO_TARGETS_REGISTERED,
   IQDK_REJECT_NO_REPLY,
   IQDK_REJECT_INSUFFICIENT_BANDWIDTH,
   IQDK_REJECT_OUT_OF_FIXED_CLUSTER,
   IQDK_REJECT_ALREADY_A_MEMBER,
   IQDK_REJECT_QOS_RESOURCES_NOT_AVAILABLE,
   IQDK_REJECT_ADDMEMBER_UNSUCCESSFUL,
   IQDK_REJECT_INCOMING_CALL_FAILED
} IQDK_RejectReasonType;

typedef enum
{
   IQDK_ADDR_TYPE,
   IQDK_CONFID_TYPE
}IQDK_AddressOrConfIdEnumType;

typedef union
{
   IQDKAddressData url;
   IQDKConfIdType confId;
} IQDK_AddressOrConfIdDataType;

typedef struct
{
   IQDK_AddressOrConfIdEnumType    type;
   IQDK_AddressOrConfIdDataType    data;
} IQDK_AddressOrConfIdType;

typedef enum
{
   IQDK_QCHAT_STATE_ERROR, 
   IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE, 
   IQDK_QCHAT_STATE_OFFLINE, 
   IQDK_QCHAT_STATE_OBTAINING_SERVICE,
   IQDK_QCHAT_STATE_ONLINE_IDLE,      
   IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
   IQDK_QCHAT_STATE_ONLINE_SUSPENDED,
   IQDK_QCHAT_STATE_MAX 
} IQDK_QChatStateEnum;

enum
{
   IQDK_TONES_ALERT_ARRIVAL      = 0x0001,
   IQDK_TONES_CALL_ARRIVAL       = 0x0002,
   IQDK_TONES_CALL_FAIL          = 0x0004,
   IQDK_TONES_ALERT_FAIL         = 0x0008,
   IQDK_TONES_FLOOR_DENY         = 0x0010,
   IQDK_TONES_FLOOR_GRANT        = 0x0020,
   IQDK_TONES_FLOOR_INTERRUPTION = 0x0040,
   IQDK_TONES_ADD_MEMBER         = 0x0080,
   IQDK_TONES_CALL_SUCCESS       = 0x0100,
   IQDK_TONES_ALERT_SUCCESS      = 0x0200,
   IQDK_TONES_TALK_ANNOUNCE      = 0x0400,
   IQDK_TONES_FLOOR_AVAILABLE    = 0x0800,
   IQDK_TONES_LAST               = 0x1000
};

typedef uint16 IQDK_ToneMask;

typedef struct
{
   boolean                 terminationOptions;
   IQDK_SoundMode          soundMode; 
   boolean                 grpCallMemberDetailsEventEnabled;
   IQDK_ToneMask           toneMask;
} IQDK_QChatUserPrefs;

typedef struct
{
   IQDK_QChatStateEnum     state;
   IQDK_QChatUserPrefs     userPrefs;
   boolean                 upgradeAvailable;
   boolean                 IsUsingOlderQChatService;
} IQDK_QChatStatusEventType;

#endif /* AEEIQDKDATATYPES_H */



/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
QChat Events

Description:
   The following sections describe BREW events sent to QDK Applications
   from the QChat Client in order to provide feedback to the end user.

   These events fall into two categories: 
      1. Events sent from the QChat Client when user interaction is
         required
      2. Events sent from the QChat Client to notify the user
         interface of conditions and events received by the client from
         QChat Service such as call and alert arrivals, floor changes, and
         changes in the operational condition of QChat.
   
   QDK Applications can register for these private notification events by
   calling ISHELL_RegisterNotify with class type AEECLSID_IQDKNOTIFIER
   and including the desired masks. When a private notification is received,
   the pData field points to a QDKNotifierEventType data structure.
   It has an evtCode field indicating which event of type IQDK_EventType was
   sent and an evtPayload field which may point to additional data.

   Note that all of the events sent by the QChat Client are sent as a part of 
   EVT_NOTIFY. The QDK application must handle these events explicitly in its 
   HandleEvent() method.

   The following is a list of the events posted by the QChat Client
   to the QDK Apps via the IQDKNOTIFIER interface or using user-defined events:


  IQDK_EVT_QCHAT_STATUS

  IQDK_EVT_PROV_ITEMS_CHANGED_RESTART

  IQDK_EVT_CALL_NOTICE

  IQDK_EVT_CALL_CONNECTED

  IQDK_EVT_END_CALL

  IQDK_EVT_CALL_STATUS

  IQDK_EVT_GROUP_NAME_AVAILABLE

  IQDK_EVT_USER_ACTION 

  IQDK_EVT_MEMBERS_INVITED

  IQDK_EVT_INVITE_RECEIVED
  
  IQDK_EVT_ALERT_SENT_STATUS
  
  IQDK_EVT_ALERT_RECEIVED

  IQDK_EVT_PREDEF_GROUP_LIST

  IQDK_EVT_MEMBER_LIST

  IQDK_EVT_DETAIL_GRP_CALL_INFO
  
  IQDK_EVT_GROUP_NAME_AVAILABLE

  IQDK_EVT_PRESENCE_ON_DEMAND

  IQDK_EVT_PROVISION_SYNC

  IQDK_EVT_PROVISION_CANCEL_SYNC

  IQDK_EVT_AVAILABILITY_REQUEST_RECEIVED

  IQDK_EVT_AVAILABILITY_REQUEST_STATUS
 
  IQDK_EVT_AVAILABILITY_UPDATE_RECEIVED

  IQDK_EVT_AVAILABILITY_UPDATE_STATUS

===================================================================== 

IQDK_EVT_PREDEF_GROUP_LIST

Description:
   The QChat Client posts this event to the QDK applications to notify
   the application that the list of predefined group memberships has
   changed. The application might choose to respond to this
   notification by retrieving the updated list of group memberships.

Parameters:
   None

Comments:
   This event is delivered via AEEQDKNotifier
   (notify-mask |= NMASK_QDK_MEMBERSHIP) interface.

See Also:

=====================================================================

IQDK_EVT_MEMBERS_INVITED

Description:
   The QChat Client posts this event to the QDK application when the 
   QChat Application Server reports to it whether the attempt to invite 
   a member to the QChat running call was successful or not. The QDK
   application can use this information to inform the user using its 
   UI of the new member in the call.

Parameters:
   None

Comments:
   This event is delivered via AEEQDKNotifier
   (notify-mask |= NMASK_QDK_CALL) interface.

See Also:
   None

=====================================================================

IQDK_EVT_USER_ACTION 

Description:
   The QChat Client posts this event to the QDK application to notify
   it that it needs the QChat Target information to proceed with placing
   the QChat Call. The QDK application in response to this event will call
   the corresponding StartCall() API along with the QChat Target address information 
   if it wants to start a call or the IQDKALERT_SendAlert() API with the QChat target
   address information if it wants to send an alert.

Parameters:
   None

Comments:
   This event is delivered via AEEQDKNotifier
   (notify-mask |= NMASK_QDK_MANAGER) interface.

See Also:
   IQDKALERT_SendAlert

===================================================================== 

IQDK_EVT_ALERT_SENT_STATUS

Description:
   This event is posted to the QDK application when the QChat Client 
   needs to inform the UI of status of the outgoing Alert.

Parameters:
   None

Comments:
   This event is delivered via AEEQDKNotifier 
   (notify-mask |= NMASK_QDK_ALERT) interface.
   
   Alerts are generally used as a polite mechanism to inform another
   user that communication is desired.  Typically, the QDK applications would
   display the text of a received alert and allow the user to call back
   the originator by pressing PTT.

See Also:

===================================================================== 

IQDK_EVT_ALERT_RECEIVED

Description:
   The QChat Client posts this event to the QDK application to notify
   it of an incoming QChat alert.

Parameters:
   None.

Comments:
   This event is delivered via AEEQDKNotifier
   (notify-mask |= NMASK_QDK_ALERT) interface.

   Alerts are generally used as a polite mechanism to inform another
   user that communication is desired.  Typically, the QDK applications would
   display the text of a received alert and allow the user to call back
   the originator by pressing PTT.

See Also:

=====================================================================
IQDK_EVT_INVITE_RECEIVED

Description:
   The QChat Client posts this event to the QDK application to notify
   it of an incoming QChat call invitation.

Parameters:
   None.

Comments:
   This event is delivered via AEEQDKNotifier
   (notify-mask |= NMASK_QDK_CALL) interface.

   The QDK Application on receipt of this event can then call the corresponding
   StartCall() API and pass the confId that has been passed as the payload of this event
   to join the call.

See Also:

=====================================================================
IQDK_EVT_CALL_CONNECTED

Description:
   The QChat Client posts this event to the QDK application when the QAS responds 
   to a call setup attempt. Originator's UI receives this event as a follow up 
   to the IQDK_CallNoticeEventType.  Target receives this event for an incoming
   call. This event is also sent when the target recieves notification of a call
   and it is already in a call with some other QChat user. In this case, the 
   results field of IQDK_CallConnectedEventType contains the field 
   IQDK_CALLRESULTS_MISSED.

Parameters:
   None

Comments:
   This event is delivered via AEEQDKNotifier
   (notify-mask |= NMASK_QDK_CALL) interface.

See Also:
   IQDK_CallConnectedEventType
   IQDK_CallType
   IQDK_CallResultsType
   IQDK_EVT_CALL_NOTICE

=====================================================================

IQDK_EVT_CALL_NOTICE

Description:
   The QChat Client posts this event to the QDK applications to notify 
   them of the status of an originated QChat call. This event informs
   the QDK applications that the QChat Client is going to attempt the QChat
   call with the QChat service and the QDK applications can choose to display 
   a message on their UI informing the user that the Call to the target addresses
   is being attempted. Call success is not certain at the time of this event, and a
   follow up event, IQDK_EVT_CALL_CONNECTED, will be sent to indicate status.
   This event is only used when a user is originating a call, not for receiving.

Parameters:
   None

Comments:
   This event is delivered via AEEQDKNotifier 
   (notify-mask |= NMASK_QDK_CALL) interface.
   
See Also:
   IQDKCALL_AddToCall()
   IQDK_CallNoticeEventType
 
===================================================================== 
IQDK_EVT_END_CALL

Description:
   The QChat Client posts this event to the QDK applications
   when the user's participation in an active QChat call has ended.

Parameters:
   None.

Comments:
   This event is delivered via AEEQDKNotifier
   (notify-mask |= NMASK_QDK_CALL) interface.

   The memory occupied by IQDK_EndCallEventType is dynamically allocated
   by the QChat Client and must be disposed of by the QDK application.
 
===================================================================== 
IQDK_EVT_CALL_STATUS

Description:
   The QChat Client posts this event to the QDK applications
   to notify them of any changes to the QChat call while
   is active. During the course of an active QChat call, the 
   QChat Client is able to determine the identity of the current 
   speaker, and provide this information to the QDK application so that 
   the QDK application may indicate who currently has the floor.  For 
   example, the QDK application may choose to display a message such as 
   "Jack is talking", or - while a clamshell device is closed - 
   display the name of the talker on the device's secondary display.  
   In addition to providing the identity of the current talker, the 
   QChat Client sends IQDK_EVT_CALL_STATUS events to notify the 
   QDK application of other conditions that involve floor control.  These would 
   include conditions in which the user has lost control of the floor 
   or is attempting to gain control of the floor when no other users 
   are participating in the call.

   The QChat Client also posts this event to update the number of members
   and/or participants of a predefined group call/adhoc call.

Parameters:
   None.

Comments:
   This event is delivered via AEEQDKNotifier 
   (notify-mask |= NMASK_QDK_CALL) interface.

See Also:
   IQDK_CallStatusEventType
 
===================================================================== 
IQDK_EVT_MEMBER_LIST

Description:
   The QChat Client posts this event to the QDK application to deliver
   the results of a previous group membership query that originated from
   the QDK application itself. QDK 
   provides the ability for the QDK application to retrieve the membership
   list of a QChat call - i.e. the list of users who are considered
   members of the call.  For example, the membership list of a call
   between two QChat users would include each of the two parties
   participating in the call.  The membership list of a group call would
   consist of all of the members subscribing to that group, not just the
   members who happened to join the call as participants.

Parameters:

   evtPayload : Pointer to an IQDK_GroupMemberListEventType data
      structure.  

Comments:
   This event is sent as part of EVT_NOTIFY.

   The memory occupied by IQDK_GroupMemberListEventType, including the
   address fields of each record in the member array, is
   dynamically allocated by the QChat Client and must be disposed of 
   by the QDK application.

See Also:
   IQDKMEMBERSHIP_EnumMemberInit()
   IQDK_GroupMemberListEventType
   IQDK_GroupMemberType
 
===================================================================== 

IQDK_EVT_PROV_ITEMS_CHANGED_RESTART

Description:
   This event is sent by the QChat Client when some of the provisionable
   items have changed in the QChat Client. This event is sent to other QDK
   applications so that they re-initialize their interface instances
   and receive an updated set of provisioning data from the QChat Client.
   For example, a QDK application may call the different APIS provided
   in the IQDKAlert interface and cache the alert mapping table in its 
   memory. If another QDK provisioning application changes the provisioning
   information in the QChat Client so that the alert mapping table changes, 
   then the QChat Client will send this event to all QDK applications that 
   are registerd with it. These applications should then re-initialize their
   instances with the QChat Client so that they get an updated set of data 
   (in this case the alert mapping table).
   
Parameters:
   None

Comments:
   This event is delivered via AEEQDKNotifier
   (notify-mask |= NMASK_QDK_MANAGER) interface.

See Also:
   IQDKCommon_Init()

=====================================================================

IQDK_EVT_PRESENCE_ON_DEMAND

Description:
   The QChat Client posts this event to the QDK application when status 
   of a presence retrieval has been returned from the QChat service.

Parameters:
   None

Comments:
   This event is sent as part of EVT_NOTIFY.

   When the presence information is requested, this event will be sent 
   directly to the requestor.
   
   Presence feature is generally used as a mechanism to obtain presence
   information for individual users and predefined groups.  Typically, 
   the QDK applications would display the presence state of the individual users
   and predefined groups in the addressbook.  

See Also:
   IQDK_PresenceInfoEventType

=====================================================================
IQDK_EVT_QCHAT_STATUS

Description:
   The QChat Client posts this event to the QDK applications to notify 
   them  of changes to the current operating status of QChat.  For
   example, if the device falls out of network coverage, or QChat
   services become temporarily unavailable, the client will issue an
   IQDK_EVT_QCHAT_STATUS event.  The QDK application may then choose to
   update its display or interact with the user in some way, as dictated
   by the new operating condition of QChat.

Parameters:

Comments:
      This event is delivered via AEEQDKNotifier 
      (notify-mask |= NMASK_QDK_MANAGER) interface.

See Also:
   IQDKCOMMON_GetQChatStatus()
   IQDK_ErrorType
   IQDK_QChatServiceType
   IQDK_QChatStatusEventType

=====================================================================
IQDK_EVT_PROVISION_SYNC

Description:
   The QChat Client posts this event to the QDK applications to notify them 
   that the Client is reserving some time for a provisioning synchronization.  
   In response to this event, QChat applications should use the IQDKProvision 
   API to synchronize the QDK to the new provisioned data.

Parameters:
   None

Comments:
      This event is delivered via AEEQDKNotifier 
      (notify-mask |= NMASK_QDK_PROVISION) interface.

See Also:
   IQDKCOMMON_SyncResults()
   IQDK_ProvSyncEventType
   IQDK_EVT_PROVISION_CANCEL_SYNC

=====================================================================
IQDK_EVT_PROVISION_CANCEL_SYNC

Description:
   The QChat Client posts this event to the QDK applications to notify 
   them that the Client is no longer willing to wait for a sync update
   and that the QDK application should abort the effort.

Parameters:
   None

Comments:
      This event is delivered via AEEQDKNotifier 
      (notify-mask |= NMASK_QDK_PROVISION) interface.

See Also:
   IQDKCOMMON_SyncResults()
   IQDK_ProvSyncEventType
   IQDK_EVT_PROVISION_SYNC

=====================================================================
IQDK_PowerMgmtLevel

Description:
   This enum defines the different power level options at which the QChat Client can operate.

Definition:
typedef enum
{
  IQDK_PWR_MGMT_LEVEL_HIGH,    
  IQDK_PWR_MGMT_LEVEL_MEDIUM,      
  IQDK_PWR_MGMT_LEVEL_LOW,   
  IQDK_PWR_MGMT_LEVEL_MAX
} IQDK_PowerMgmtLevel;

Members:
  IQDK_PWR_MGMT_LEVEL_HIGH: Specifies that the power used by the phone will be high.
  
  IQDK_PWR_MGMT_LEVEL_MEDIUM: Specifies that the power used by the phone will be medium.

  IQDK_PWR_MGMT_LEVEL_LOW: Specifies that the power used by the phone will be high.
   
  IQDK_PWR_MGMT_LEVEL_MAX: Specifies the maximum value of this enumeration.


See Also:
  
=======================================================================
IQDK_AvailabilityActionType

Description:
   This enum is used to indicate if calls are to be accepted or rejected when the client is in a particular presence state.

Definition:
typedef enum
{
   IQDK_AVAILABILITY_ACTION_ACCEPT_CALLS,
   IQDK_AVAILABILITY_ACTION_REJECT_CALLS
} IQDK_AvailabilityActionType;

Members:
   IQDK_AVAILABILITY_ACTION_ACCEPT_CALLS: This option instructs the Qchat client to accept all calls.

   IQDK_AVAILABILITY_ACTION_REJECT_CALLS: This option instructs the Qchat client to reject all calls.


See Also:
  
=======================================================================
IQDK_AvailabilityEntry

Description:
   This struct holds the presence state informaiton.   

Definition:
{
   AECHAR                          text[IQDK_MAX_AVAILABILITY_STRING];
   IQDK_AvailabilityActionType     action;
   boolean                         userSettable;
} IQDK_AvailabilityEntry;

Members:
  text[IQDK_MAX_AVAILABILITY_STRING]: This field holds the text that is used to describe a particular presence state. 
  
  action: This field is used to indicate what action needs to be taken by the QChat client when in a particular state. 
	This could be to accept or reject calls.

  userSettable: This field is used to specify if the availability entry can be set by the user or not. 


See Also:
  
=======================================================================
IQDK_CallRestrictionLevel

Description:
   This data type specifies the level of a particular call restriction.
   Both users and carriers can separately add call restrictions.  In
   the case of conflicting restrictions, carrier level restrictions
   supercede user level restrictions.

Definition:
typedef enum
{
   IQDK_CARRIER,
   IQDK_USER
} IQDK_CallRestrictionLevel;

Values:
   IQDK_CARRIER : Call Restrictions provisioned by the carrier
   IQDK_USER    : Call Restrictions provisioned by the user 

See Also:
  
=======================================================================
IQDK_CallRestrictionType

Description:
   This data type specifies the type of a particular restriction.  The type
   is used to express if a user, domain, direction, or service should be
   denied, allowed, or always allowed(meaning that it cannot be overridden
   by the user).  Only carrier-level restrictions can "always allow", and
   can override their own "always allow" rule with a "deny" rule.  However,
   User-level restrictions cannot override carrier-level "always allow"
   rules.  When conflicts are encountered, Carrier-level "always allow" rules
   are taken into consideration first, otherwise the order of entry is used,
   with the later rules taking precedence over earlier rules.  This can mean
   that restrictions exist in the list that have no impact if later or
   carrier-level "allow always" restrictions that override them exist.  It is
   encumbent on the user or carrier to do any desired cleanup in this case.

Definition:
typedef enum
{
   IQDK_ALWAYS_ALLOW,
   IQDK_ALLOW,
   IQDK_DENY
} IQDK_CallRestrictionType;

Values:
   IQDK_ALWAYS_ALLOW : Type of restriction used to always explicitly allow
                       something. (Cannot be overriden by user-level "deny")
   IQDK_ALLOW        : Type of restriction that explicitly allows something.
                       (Can be overrideen by user-level "deny")
   IQDK_DENY         : Type of restriction that denies something. 


See Also:
  
=======================================================================
IQDK_CallRestrictionDirection

Description:
   This data type specifies the enforcement direction of a QChat call
   restriction.  Call restrictions may be enforced against either
   inbound or outbound calls.  An inbound call restriction would be
   applied when a call or alert is received, while an outbound call
   restriction would be applied when the user attempts to originate a
   call or send an alert.

Definition:
   typedef enum
   {
      IQDK_INBOUND,
      IQDK_OUTBOUND,
      IQDK_INBOUND_AND_OUTBOUND,
      IQDK_NUM_RESTRICTION_DIRECTIONS
   } IQDK_CallRestrictionDirection;

Values:
   IQDK_INBOUND :  The call restriction is to be applied to an inbound
                   call - i.e. a call that would otherwise be received by the
                   QChat Client.
   IQDK_OUTBOUND :  The call restriction is to be applied to an
                    outbound call - i.e. a call that would otherwise be attempted by
                    the QChat Client..
   IQDK_INBOUND_AND_OUTBOUND : The call restriction is to be applied to both
                               inbound and outbound calls
   IQDK_NUM_RESTRICTION_DIRECTIONS   :  Represents the number of 
                                        IQDK_CallRestrictionDirection constants 
                                        defined in the data structures.


See Also:
  
=======================================================================
IQDK_ServicesType;

Description:
   

Definition:
typedef uint16 IQDK_ServicesType;

Members:
	N/A


See Also:
  
=======================================================================
IQDK_ServicesEnum

Description:
	This enum enumerates the various services that are supported by the 
	QChat client.

Definition:
typedef enum
{
   IQDK_ALERTS,
   IQDK_DIRECTCALLS,
   IQDK_GROUPCALLS,
   IQDK_ADHOCGROUPCALLS,
   IQDK_CHATROOMCALLS,
   IQDK_PRESENCEONDEMAND_QUERIES,
   IQDK_MAXSERVICES
} IQDK_ServicesEnum;

Members:
   IQDK_ALERTS: This enum option means that alerts are supported as part of the services.

   IQDK_DIRECTCALLS: This enum option means that direct calls are supported as part of the services.
.

   IQDK_GROUPCALLS: This enum option means that predefined group calls are supported as part of the services.

   IQDK_ADHOCGROUPCALLS: This enum option means that adhoc groups are supported as part of the services.

   IQDK_CHATROOMCALLS: This enum option means that chatroom calls are supported as part of the services.

   IQDK_PRESENCEONDEMAND_QUERIES: This enum option means that presence on demand services are supported as part of the services.

   IQDK_MAXSERVICES: This marks the maximum number of services supported.


See Also:
  
=======================================================================
IQDK_CallRestrictionEntry

Description:
   This data structure identifies a single call restriction.  Each call
   restriction is expressed by direction (inbound or outbound),
   type (deny, allow or always allow) and a token against which QChat
   addresses will be matched.  For example, an application might choose
   to define a restriction that would prevent the user from calling
   other users in a specific domain.  This would be an outbound, deny,
   restriction against a particular addressing domain.  Similarly, a
   restriction could be defined to always accept incoming calls from a
   specific addresses - regardless of any other user level restrictions
   that may have been established.  This would be an inbound, always
   allow restriction for a specific user address.
   
   The data structure also includes fields indicating whether or not the
   restriction is currently enabled.

Definition:
   typedef struct
   {
      IQDKAddressData                * token;
      IQDK_CallRestrictionLevel        level;
      IQDK_CallRestrictionType         type;
      IQDK_CallRestrictionDirection    direction;
      IQDK_ServicesType                service;
      boolean                          asteriskIsLiteral;
   } IQDK_CallRestrictionEntry;

Members:
   token              : This string contains the token the QChat Client will use to determine 
                        whether or not a given call should be restricted.  QChat attempts to 
                        match this string to the originator's address when enforcing an inbound 
                        call restriction, or to each address in the target list when enforcing an 
                        outbound call restriction. 
                        Note: The client will always ignore the IQDKAddressType in the IQDKAddressData 
                        structure when accepting call restrictions. This helps avoid confusion in cases where
			restrictions with wild characters such as *@baddomain.com or *@*.baddomain.com need to be set.
   level	      : This field indicates if it is a carrier or user level call restriction		
   type               : This field indicates the type of restriction to be enforced.
   direction          : This field indicates the direction of the call to
                        which the call restriction will be applied.
   service            : This field indicates if the restriction is to be applied to an alert,
                        direct, ad hoc group, group calls or to all of them	
   asteriskIsLiteral  : a '*' char within the address will be treated as a literal 
                        if this boolean is set to true, else it can be used as a wildcard


See Also:
IQDK_CallRestrictionDirection
IQDK_RestrictionType

=======================================================================

IQDK_RejectReasonType

Description:
   The IQDK_RejectReasonType data type identifies why a QChat
   operation has failed.  Operations that may fail include call or alert
   attempts, group membership queries, and any communication between the
   QChat Client and the QChat Application Server.

Definition:
   typedef enum
   {
     IQDK_SUCCESS=0,
     IQDK_REJECT_TOO_MANY_TARGETS,
     IQDK_REJECT_USER_UNKNOWN,
     IQDK_REJECT_INVALID_CONF_ID,
     IQDK_REJECT_INVALID_ADDRESS,
     IQDK_REJECT_VOCODER_INCOMPATIBLE,
     IQDK_REJECT_VER_NOT_SUPPORTED,
     IQDK_REJECT_NO_RESOURCES_AVAILABLE,
     IQDK_REJECT_TARGET_BUSY,
     IQDK_REJECT_NOT_A_MEMBER,
     IQDK_REJECT_CALL_NOT_RUNNING,
     IQDK_REJECT_UNSUPPORTED,
     IQDK_REJECT_BLOCKED_USER,
     IQDK_REJECT_UNKNOWN_ERROR,
     IQDK_REJECT_TARGETS_NOT_REACHABLE,
     IQDK_REJECT_ALL_TARGETS_REJECT, 
     IQDK_REJECT_NO_CALL_TYPE_CAPABILITY,
     IQDK_REJECT_ALL_INCOMING_CALLS,
	 IQDK_REJECT_ALERT_NO_TEXT,
     IQDK_REJECT_NETWORK_DELAY_TIMEOUT,
     IQDK_REJECT_NO_TARGETS_REGISTERED,
     IQDK_REJECT_NO_REPLY,
     IQDK_REJECT_INSUFFICIENT_BANDWIDTH,
     IQDK_REJECT_OUT_OF_FIXED_CLUSTER,
     IQDK_REJECT_ALREADY_A_MEMBER,
     IQDK_REJECT_QOS_RESOURCES_NOT_AVAILABLE,
     IQDK_REJECT_ADDMEMBER_UNSUCCESSFUL
   } IQDK_RejectReasonType;
Values:
   IQDK_SUCCESS: Operation was succesful
   IQDK_REJECT_TOO_MANY_TARGETS: More targets were selected than could 
                                 fit into a single call request message.
   IQDK_REJECT_USER_UNKNOWN :  The user is not able to communicate through the QChat 
                               Application Server because his or her address is not known 
                               to the server (and is possibly not registered with the Regional 
                               Location Server).
   IQDK_REJECT_INVALID_CONF_ID :  The QChat Client attempted to use 
                                  a conference ID for a call that is no longer known to the QChat 
                                  Application Server.
   IQDK_REJECT_INVALID_ADDRESS :  The QChat Client attempted to use a user or group address that 
                                  was not recognized by the QChat Application Server.
   IQDK_REJECT_VOCODER_INCOMPATIBLE  :  An attempt to setup a QChat call was rejected because 
                                        the vocoder proposed by the originator is not compatible 
                                        with other members of the call.
   IQDK_REJECT_VOCODER_INCOMPATIBLE  :  An attempt to setup a QChat call was rejected because the 
                                        vocoder proposed by the originator is not compatible with 
                                        other members of the call.
   IQDK_REJECT_VER_NOT_SUPPORTED  :  The QChat Client is attempting to communicate with the QChat 
                                     Application Server using a protocol version that is not supported 
                                     by the server.
   IQDK_REJECT_NO_RESOURCES_AVAILABLE   :  The QChat Application Server is not able to communicate 
                                           with the QChat Client at this time due to a lack of resources 
                                           on the server.
   IQDK_REJECT_TARGET_BUSY  :  The target QChat Client rejected the call because it is already 
                               participating in another call.
   IQDK_REJECT_NOT_A_MEMBER :  The user is not a member of the group for which a group membership 
                               request has been made.
   IQDK_REJECT_CALL_NOT_RUNNING   :  An attempt to join or add new members to a call failed 
                                     because the call is no longer active and running in the 
                                     QChat Application Server.
   IQDK_REJECT_UNSUPPORTED: The QChat Application server does not support this feature/message.
   IQDK_REJECT_BLOCKED_USER: An attempt to communicate via a QChat call or a QChat alert has 
                             been rejected due to the application of call restrictions.
   IQDK_REJECT_UNKNOWN_ERROR: An unknown error has occurred.
   IQDK_REJECT_TARGETS_NOT_REACHABLE :  None of the targets specified when setting up the call 
                                        were found to be reachable from the QChat Application Server.
   IQDK_REJECT_ALL_TARGETS_REJECT: All the targets rejected the incoming call.
   IQDK_REJECT_NO_CALL_TYPE_CAPABILITY: The QChat client does not have capability to make a call.
   IQDK_REJECT_ALL_INCOMING_CALLS: The QChat Client is configured to reject all incoming calls.
   IQDK_REJECT_ALERT_NO_TEXT: Alert successful but Alert text was not delivered.
   IQDK_REJECT_NETWORK_DELAY_TIMEOUT : Network delay is causing the call to fail
   IQDK_REJECT_NO_TARGETS_REGISTERED : target(s) is/are not registered
   IQDK_REJECT_NO_REPLY              : Target did not respond to the QChat call type announce.
   IQDK_REJECT_INSUFFICIENT_BANDWIDTH : Network is not able to sustain the call due to insufficient bandwidth
   IQDK_REJECT_OUT_OF_FIXED_CLUSTER   : Client is unable to place the call because it is out of the 
                                        supported service area
   IQDK_REJECT_ALREADY_A_MEMBER       : Client tries to invite a QChat user to a call and server detects that 
                                        the target is already a member of that group.
   IQDK_REJECT_QOS_RESOURCES_NOT_AVAILABLE : The QChat Application Server is not able to communicate 
                                           with the QChat Client at this time due to a lack of QOS resources
   IQDK_REJECT_ADDMEMBER_UNSUCCESSFUL : The QChat Application Server was not able service the Client's request
                                        to add members to the existing call.

=====================================================================
IQDK_AddressOrConfEnumIdType

Description:
   This enum lists two types of addressing options to choose from.

Definition:
typedef enum
{
   IQDK_ADDR_TYPE,
   IQDK_CONFID_TYPE
}IQDK_AddressOrConfIdEnumType;

Members:
	IQDK_ADDR_TYPE: This type makes use of a client address containing a url.

	IQDK_CONFID_TYPE: This type makes use of a conference ID.
   
See Also:
	IQDKAddressData
	IQDKConfIdType
	
=====================================================================

IQDK_AddressOrConfIdDataType

Description:
   This union defines two different addressing formats to choose from.

Definition:
typedef union
{
   IQDKAddressData url;
   IQDKConfIdType confId;
} IQDK_AddressOrConfIdDataType

Members:
	url: This field is used to specify a IQDKAddressData value 
		containing the clent address.

	confId: This field is used to specify a IQDKConfIdType value
		containing the conference ID. 
   
See Also:
	IQDKAddressData
	IQDKConfIdType
	
=====================================================================
IQDK_AddressOrConfIdType

Description:
   This data structure lists the different ways of specifying addresses in the Qchat client.

Definition:
typedef struct
{
   IQDK_AddressOrConfIdEnumType    type;
   IQDK_AddressOrConfIdDataType    data;
} IQDK_AddressOrConfIdType;

Members:
	type: This field is used to specify which of the two, a QChat User address or a 
		conference ID is to be used.

	data: This field is used to specify the url or a conference id depending on which
		of the two options is desired. 
   
See Also:
	
=====================================================================
IQDK_QChatStateEnum

Description:
   This enum lists the different operational modes of the Qchat client.

Definition:
typedef enum
{
   IQDK_QCHAT_STATE_ERROR, 
   IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE, 
   IQDK_QCHAT_STATE_OFFLINE, 
   IQDK_QCHAT_STATE_OBTAINING_SERVICE,
   IQDK_QCHAT_STATE_ONLINE_IDLE,      
   IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
   IQDK_QCHAT_STATE_ONLINE_SUSPENDED,
   IQDK_QCHAT_STATE_MAX 
} IQDK_QChatStateEnum;

Members:
   IQDK_QCHAT_STATE_ERROR: The QChat client is in some erroneous state.

   IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE: The QChat client does not have digital CDMA/EV-DO Rev A service or QChat service is not supported in CDMA 1x when the
           user is moving from EV-DO into CDMA 1x serving system while in a QChat call.

   IQDK_QCHAT_STATE_OFFLINE:The QChat client has service but is not currently registered
		with the QChat server.

   IQDK_QCHAT_STATE_OBTAINING_SERVICE: The QChat client is currently obataining registering 
		with the QChat server. 

   IQDK_QCHAT_STATE_ONLINE_IDLE: The QChat client is registered but is in an idle state.
   
   IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL: The QChat client is registered and participating in a call.

   IQDK_QCHAT_STATE_ONLINE_SUSPENDED: The QChat client is registered and in the suspended mode. 

   IQDK_QCHAT_STATE_MAX: This is used to mark the maximum number of states that are currently supported. 
   
See Also:
	
=====================================================================

IQDK_QChatUserPrefs

Description:
   This data structure allows user to specify preferences including details of
   termination options, preferred address index, sound mode and a choice of wether
   to enable or disable group call member details events. Everytime a QDK application changes
   any of these preferences, the QChat Client will send this structure to all of the QDK applications
   so that they can update their state.

typedef struct
{
   boolean                 terminationOptions;
   uint8                   preferredAddrIndex;
   IQDK_SoundMode          soundMode;
   boolean                 grpCallMemberDetailsEventEnabled;
} IQDK_QChatUserPrefs;

Members:
   terminationOptions: This field indicates that the user wants to specify the way a call should
		be terminated.

   preferredAddrIndex: This fiels indicates the index of the address that is preferred by the user.

   soundMode: This field is used to indicate the sound mode to be used.

   grpCallMemberDetailsEventEnabled: Boolean flag to indicate if the client wants to recive group 
		member call events.
   
See Also:
	

=======================================================================

IQDK_SoundMode

Description:
    This enum defines the different sound modes supported in the QChat
    Client.

Definition:
   typedef enum
   {
      IQDK_SOUND_SILENT_MODE,
      IQDK_SOUND_MANNER_MODE,
      IQDK_SOUND_SPEAKER_MODE
   } IQDK_SoundMode;

Members:
   IQDK_SOUND_SILENT_MODE: This option instructs the QChat Client to
   mute the media packets that it receives.

  IQDK_SOUND_MANNER_MODE: This option instructs the QChat Client to
  play the media packets through the earpiece of the handset.

  IQDK_SOUND_SPEAKER_MODE: This option instructs the QChat Client to
  play the media packets through the speakerphone of the handset.

See Also:
   IQDKSOUNDCTL_SetMode
   IQDKSOUNDCTL_GetMode

=======================================================================

IQDK_QChatStatusEventType

Description:
   This data structure contains information about the current
   operational status of the QChat Client, including details of
   user preferences, upgrades and version of the QChat client.

Definition:
typedef struct
{
   IQDK_QChatStateEnum     state;
   IQDK_QChatUserPrefs     userPrefs;
   boolean                 upgradeAvailable;
   boolean                 IsUsingOlderQChatService;
} IQDK_QChatStatusEventType;

Members:
   state: This field indicates the current operational state of the client.

   userPrefs: This field holds a struct containing user defined preferences that
		include information on call termination, preferred address index, sound mode, 
		and group call member details event enabling.

   upgradeAvailable: Boolean flag to indicate if an upgrade is available or not.

   IsUsingOlderQChatService: Boolean flag to indicate if the client version
		is out of date.
   
See Also:
	IQDK_QChatStateEnum
	IQDK_QChatUserPrefs
	

=======================================================================*/

