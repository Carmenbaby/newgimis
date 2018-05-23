#ifndef AEEIQDKNOTIFIER_H
#define AEEIQDKNOTIFIER_H

//===========================================================================
// FILE: AEEIQDKNotifier.h
// 
// SERVICES: QDK Client Service Notifier Interface
// 
// DESCRIPTION:
//    IQDKNotifier is the published notifier interface to the QChat 
//    Client Service.
//
// PUBLIC FUNCTIONS:
//    IQDKNOTIFIER_AddRef  - Inc ref count
//    IQDKNOTIFIER_Release - Dec ref count, release on zero
// 
// LEGAL:
//    (c) Copyright 2007 QUALCOMM Incorporated.
//    All Rights Reserved.  QUALCOMM Proprietary.
//    This technology is controlled by the United States
//    Government.  Diversion contrary to U.S. law prohibited.
//===========================================================================


//===========================================================================
//   INCLUDES AND PUBLIC DATA DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
//   Defines
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
//   Include Files
//---------------------------------------------------------------------------
#include "AEE.h"
#include "AEEQDKDataTypes.h"
#include "AEEIQDKNotifier.bid"		//Backward_Comp_START_Apr_19

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

typedef struct IQDKNotifier IQDKNotifier;

/*-------------------------------------------------------------------
      Function Declarations
-------------------------------------------------------------------*/

/*-----------------------------------------------------------------
       Interface functions
-------------------------------------------------------------------*/

// Notification Masks...
#define NMASK_QDK_MANAGER             0x00000001
#define NMASK_QDK_CALL                0x00000002
#define NMASK_QDK_ALERT               0x00000004
#define NMASK_QDK_MEMBERSHIP          0x00000008
#define NMASK_QDK_PROVISION           0x00000010
#define NMASK_QDK_PRESENCE_ON_DEMAND  0x00000020
#define NMASK_QDK_AVAILABILITY_NOTIFICATION  0x00000040

// Defines the structure for the event notifier pData.
typedef struct
{
   IQDK_EventType evtCode;
   const void*    evtPayload;
   AEECLSID       classId; 
}QDKNotifierEventType;

// Inherit from INotifier
AEEINTERFACE(IQDKNotifier) {
   INHERIT_INotifier(IQDKNotifier);
};


#define IQDKNOTIFIER_AddRef(p)  AEEGETPVTBL((p),IQDKNotifier)->AddRef((p))  
#define IQDKNOTIFIER_Release(p) AEEGETPVTBL((p),IQDKNotifier)->Release((p))

/*===================================================================
  DATA STRUCTURE DOCUMENTATION
===================================================================== 
Notification Data

   Applications can register to receive certain notification from the
   QChat Client. Registration for events can be done by calling
   ISHELL_RegisterNotify() with class type AEECLSID_QDKNOTIFIER and 
   specifying the appropriate mask, depending on which notification 
   events are desired.

See Also:
 Notification Masks
===================================================================== 
Notification Masks

Description:
   Notification masks are used to indicate which types of notifications 
   are desired. When a notification event is received, the 
   AEENotify.pData field contains a pointer to QDKEventType
   data structure. This data structure holds information about
   notification event that is sent. The payload associated
   with that notification event can be requested using the GetEventPayload
   API with the event type set to the notification event code.

Values:
===H2>
   NMASK_QDK_MANAGER
===H2>
      Event code field (evtCode) in QDKNotifierEventType can hold one of the
      following values for this notification mask.
      1) IQDK_EVT_QCHAT_STATUS: For this event, the associated event payload 
         data structure is IQDK_QChatStatusEventType. This payload can be 
         requested through the IQDKMANAGER_GetEventPayload API.

      2) IQDK_EVT_PROV_ITEMS_CHANGED_RESTART: For this event, there is no 
         associated event payload. Whenever the QChat Client has 
         provisionable items that have been updated, it sends this event to 
         other QDK applications. Other QDK applications may either restart 
         or at least free all the QDK interfaces and re-instantiate them so 
         that they can get updated with provisioned values from the QChat Client.

      3) IQDK_EVT_USER_ACTION : For this event, there is no associated 
         event payload. If the QChat Client needs to Start a call or Send an Alert
         and it does not have target address information, it sends out this event.

===H2>
   NMASK_QDK_CALL
===H2>
      Event code field (evtCode) in QDKNotifierEventType can hold one of the
      following values for this notification mask.
      1) IQDK_EVT_CALL_NOTICE: For this event, evtPayload in 
         QDKNotifierEventType points to IQDK_CallNoticeEventType data 
         structure.

      2) IQDK_EVT_CALL_CONNECTED: For this event, the associated event payload 
         data structure is IQDK_CallConnectedEventType. This payload can be 
         requested through the IQDKCALL_GetEventPayload API.

      3) IQDK_EVT_END_CALL: For this event, the associated event payload 
         data structure is IQDK_EndCallEventType. This payload can be requested
         through the IQDKCALL_GetEventPayload. 

      4) IQDK_EVT_CALL_STATUS: For this event, the associated event payload 
         data structure is IQDK_CallStatusEventType. This payload can be requested
         through the IQDKCALL_GetEventPayload.

      5) IQDK_EVT_MEMBERS_INVITED: For this event, the associated event payload 
         data structure is IQDK_MembersInvitedEventType. This payload can be requested
         through the IQDKCALL_GetEventPayload. 

      6) IQDK_EVT_INVITE_RECEIVED: For this event, the associated event payload 
         data structure is IQDK_InviteReceivedEventType. This payload can be 
         requested through the IQDKCALL_GetEventPayload. 

===H2>
   NMASK_QDK_ALERT
===H2>
      Event code field (evtCode) in QDKNotifierEventType can hold one of the
      following values for this notification mask.
      1) IQDK_EVT_ALERT_SENT_STATUS: For this event, the associated event payload 
         data structure is IQDK_AlertStatusEventType. This payload can be 
         requested through the IQDKALERT_GetEventPayload. 

      2) IQDK_EVT_ALERT_RECEIVED: For this event, the associated event payload 
         data structure is IQDK_AlertReceivedEventType. This payload can be 
         requested through the IQDKALERT_GetEventPayload.

===H2>
   NMASK_QDK_MEMBERSHIP
===H2>
      Event code field (evtCode) in QDKNotifierEventType can hold one of the
      following values for this notification mask.
      1) IQDK_EVT_PREDEF_GROUP_LIST: For this event, there is no associated 
         event payload. This event merely notifies the application that the list 
         of predefined groups has changed. The application might choose 
         to respond to this notification by retrieving the updated list of groups.

      2) IQDK_EVT_MEMBER_LIST: For this event, the associated event payload 
         data structure is IQDK_GroupMemberListEventType. This payload can be 
         requested through the IQDKMEMBERSHIP_GetEventPayload.  

      3) IQDK_EVT_DETAIL_GRP_CALL_INFO: For this event, the associated event payload
         data structure is IQDK_GroupCallMemberDetailsEventType. This payload can be
         requested through the IQDKMEMBERSHIP_GetEventPayload.  

      4) IQDK_EVT_GROUP_NAME_AVAILABLE: For this event, the associated event payload
         data structure is IQDK_GroupNameEventType. This payload can be 
         requested through the IQDKMEMBERSHIP_GetEventPayload.  

===H2>
   NMASK_QDK_PROVISION
===H2>
      Event code field (evtCode) in QDKNotifierEventType can hold one of the
      following values for this notification mask.
      1) IQDK_EVT_PROVISION_SYNC: For this event, the associated event payload data
         structure is IQDK_ProvisionSyncPayload. This payload can be 
         requested through the IQDKPROVISION_GetEventPayload. 

      2) IQDK_EVT_PROVISION_CANCEL_SYNC: For this event, there is no associated 
         event payload. This event notifies applications that the 
         QChat Client is cancelling the IQDK_EVT_PROVISION_SYNC event it sent 
         earlier.

===H2>
   NMASK_QDK_PRESENCE_ON_DEMAND
===H2>
      Event code field (evtCode) in QDKNotifierEventType can hold the following value 
      for this notification mask.
      1) IQDK_EVT_PRESENCE_ON_DEMAND: For this event, the associated event payload data
         structure is IQDK_PresenceInfoEventType. This payload can be requested through 
         the IQDKPRESENCEONDEMAND_GetEventPayload. 

Version:
   Introduced in QDK v 2.0
        
Side Effects:
   None
   
See Also:
   AEENotify
   IQDK_EventType
   ISHELL_RegisterNotify()
   IQDK_QChatStatusEventType
   IQDKMANAGER_GetEventPayload
   IQDK_CallNoticeEventType
   IQDK_CallConnectedEventType
   IQDK_EndCallEventType
   IQDK_CallStatusEventType
   IQDK_MembersInvitedEventType
   IQDK_InviteReceivedEventType
   IQDKCALL_GetEventPayload
   IQDK_AlertStatusEventType
   IQDK_AlertReceivedEventType
   IQDKALERT_GetEventPayload
   IQDK_GroupMemberListEventType
   IQDK_GroupCallMemberDetailsEventType
   IQDK_GroupNameEventType
   IQDKMEMBERSHIP_GetEventPayload
   IQDK_ProvisionSyncPayload
   IQDKPROVISION_GetEventPayload
   IQDK_PresenceInfoEventType
   IQDKPRESENCEONDEMAND_GetEventPayload
   
 
===================================================================== 
QDKNotifierEventType

Description:
   When a notification event is received, the AEENotify.pData field 
   contains a pointer to QDKNotifierEventType data structure.

Definition:
   typedef struct
   {
      IQDK_EventType evtCode;
      const void* evtPayload;
      AEECLSID    classId;
   }QDKNotifierEventType;

Members:
   evtCode : Indicates which notification event (type IQDK_EventType) is received
   evtPayload : Pointer to optional additional data for the event
   classId : Specifies the class identifier of the QDK application that the 
             QChat Client wants to come to the foreground. The QChat Client
             runs an algorithm to decide which QDK application should be
             allowed to come to the foreground.

Comments:
   None

Version:
   Introduced in QDK v 2.0

Side Effects:
   None
   
See Also:
   IQDK_EventType
   QDKNotifierEventType
 
=====================================================================*/

#endif // AEEQCHATNOTIFIER_H
