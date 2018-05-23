#ifndef AEEQDKALERT_H
#define AEEQDKALERT_H
/*==============================================================================
 
FILE: AEEQDKAlert.h

SERVICES: The IQDKAlert interface manages all the methods related to 
Alert features of QChat.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IQDKAlert

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
==============================================================================*/

/*==============================================================================
                     INCLUDE FILES FOR MODULE
==============================================================================*/
#include "AEE.h"
#include "AEEIQDKAlert.bid"		//Backward_Comp_START_Apr_19
#include "AEEQDKDataTypes.h"

/*==============================================================================
   DATA STRUCTURES and DEFINES
==============================================================================*/

#define AEEIID_IQDKALERT 0x0104efd0 //Interface ID for 2.0 QueryInterface

typedef uint16 IQDK_AlertIdType;

typedef struct
{
   IQDKAddressData       originatorsAddress;
   uint8                 alertMsgIndex;
   uint32                alertRecdTime;
   uint8                 aliasIndexForWhomMessageIntended;
} IQDK_AlertReceivedEventType;

typedef struct
{
   IQDK_RejectReasonType rejectReason;
   boolean               bVoicemail; // Is failed alert call eligible for voicemail. do we need this?
} IQDK_AlertStatusEventType;

typedef union
{
  IQDK_AlertReceivedEventType alertReceivedEvent;
  IQDK_AlertStatusEventType   alertStatusEvent;
} IQDK_AlertEventPayload;


/*==============================================================================
                    CLASS DEFINITIONS
==============================================================================*/

typedef struct IQDKAlert IQDKAlert;

AEEINTERFACE(IQDKAlert)
{
   INHERIT_IQueryInterface(IQDKAlert);
   IQDK_ErrorType (*Init)(IQDKAlert *po, AEECLSID appClsID);
   IQDK_ErrorType (*SendAlert)(IQDKAlert *po, IQDKAddressData *addr, uint8 alertIndex, uint8 preferredAddrIndex, boolean waitForPTTPress); 
   IQDK_ErrorType (*GetAlertTextByIndex)(IQDKAlert *po, uint8 alertIndex, AECHAR** message);
   IQDK_ErrorType (*GetAlertCount)(IQDKAlert *po, uint8* pCount);
   IQDK_ErrorType (*GetEventPayload)(IQDKAlert *pMe, uint32 interfaceId, uint16 eventType, IQDK_AlertEventPayload **payload);
};

#define IQDKALERT_AddRef(p)                    AEEGETPVTBL(p,IQDKAlert)->AddRef(p)
#define IQDKALERT_Release(p)                   AEEGETPVTBL(p,IQDKAlert)->Release(p)
#define IQDKALERT_QueryInterface(p,clsid,pp)   AEEGETPVTBL(p,IQDKAlert)->QueryInterface(p,clsid,pp)
#define IQDKALERT_Init(p,clsid)                AEEGETPVTBL((p),IQDKAlert)->Init((p),(clsid))
#define IQDKALERT_SendAlert(p,a,i,x, y)        AEEGETPVTBL(p,IQDKAlert)->SendAlert(p,a,i,x,y)
#define IQDKALERT_GetAlertTextByIndex(p, i, m) AEEGETPVTBL(p,IQDKAlert)->GetAlertTextByIndex(p, i, m)
#define IQDKALERT_GetAlertCount(p,i)           AEEGETPVTBL((p),IQDKAlert)->GetAlertCount((p), (i))
#define IQDKALERT_GetEventPayload(p,i,a,b)     AEEGETPVTBL((p),IQDKAlert)->GetEventPayload(p,i,a,b)

#endif // AEEQDKALERT_H

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================

IQDK_AlertIdType

Description:
   Defines the supported AlertIds. Typedef'ed to uint16.

================================================================================
IQDK_AlertReceivedEventType

Description:
   This data structure contains the payload for an IQDK_EVT_ALERT_RECEIVED event.  
   The QChat Client sends this event to the QDK application when a new alert has arrived.
   The details of the alert notification are contained in the
   IQDK_AlertReceivedEventType data structure, providing the QDK application
   with information about the alert and its origin and destination.

Definition:
typedef struct
{
   IQDKAddressData       originatorsAddress;
   uint8                 alertMsgIndex;
   uint32                alertRecdTime;
   uint8                 aliasIndexForWhomMessageIntended;
} IQDK_AlertReceivedEventType;

Members:
   originatorsAddress:	     This field contains the QChat user address of the user who originated the alert.  
   alertMsgIndex:            This field contains the alert message ID number. If this field contains a value of 0xff(255), then the 
                             message index is invalid and the UI/QDK app may display only that an alert has been recievied without an 
                             actual alert string.

   alertRecdTime:            This field contains the time that the alert was received.  
										The time is expressed as the number of seconds since 
										January 6, 1980 in the local time of the device.
   aliasIndexForWhomMessageIntended:This field contains the alias index for
										whom the message was intended.

See Also:
  IQDKAddressData
  IQDK_RejectReasonType

================================================================================
IQDK_AlertStatusEventType

Description:
   This data structure contains the payload for an IQDK_EVT_ALERT_SENT_STATUS event.  
   The QChat Client sends this event so that the QDK application 
   can track the status of an alert that is sent.
   The details of the alert notification are contained in the
   IQDK_AlertStatusEventType data structure, providing the QDK application with a
   reason for rejection and whether the alert call is eligible for voicemail.

Definition:
typedef struct
{
   IQDK_RejectReasonType rejectReason;
   boolean               bVoicemail; 
} IQDK_AlertStatusEventType;

Members:
   rejectReason: This field contains the reason for rejection of the sent alert.
   bVoicemail:	 This field indicates if the alert that failed is eligible for a voicemail.

See Also:
   IQDKAddressData
   IQDK_RejectReasonType

================================================================================
IQDK_AlertEventPayload

Description:
   This data structure contains the payload for an IQDK_EVT_ALERT_SENT_STATUS or
   IQDK_EVT_ALERT_RECEIVED event.  The union field to be selected is specified in
   the eventType field in the call to IQDKALERT_GetEventPayload().

Definition:
typedef union
{
  IQDK_AlertReceivedEventType alertReceivedEvent;
  IQDK_AlertStatusEventType   alertStatusEvent;
} IQDK_AlertEventPayload;

Members:
   alertReceivedEvent:  This field contains the payload for an IQDK_EVT_ALERT_RECEIVED event.
   alertStatusEvent: This field contains the payload for an IQDK_EVT_ALERT_SENT_STATUS event.

See Also:
   IQDKALERT_GetEventPayload()
   IQDK_EVT_ALERT_RECEIVED
   IQDK_EVT_ALERT_SENT_STATUS 


================================================================================
================================================================================
  INTERFACES   DOCUMENTATION
================================================================================

Interface Name: IQDKAlert

Description:
IQDKAlert interface is a set of common API methods that are intended to be
used by applications that want to use the Alert feature of QChat.

Alerts are a mechanism by which QChat users can quickly contact another user
without actually initiating a QChat call.  Typically, an alert is used as a
polite way to let another user know that the originator would like to engage
in a QChat call - sort of the QChat equivalent of ringing the phone prior to
pickup. Rather than push-to-talk, a user would issue an alert to a target -
the alert appearing on the target handset, along with a notification beep and,
optionally, a short text message. The person receiving the alert could then
call back the originator by pressing the PTT button.

The IQDKAlert interface instance is obtained via ISHELL_CreateInstance()
mechanism.  

The QChat Client uses BREW INotifier interface to send Alert related events to
the QDK applications. QDK applications will have to specify NMASK_QDK_ALERT
mask value via dwNotifyMask field in IQDKMANAGER_Init() API method in order
to receive alert related notification events from the QChat Client.

Payload of the notifier event from the QChat Client carries following event:

IQDK_EVT_ALERT_SENT_STATUS: This event notifies application about the result 
								of the user-initiated alert request.

IQDK_EVT_ALERT_RECEIVED:	This event notifies the application about an 
								incoming alert.

The following header file is required:
AEEIQDKAlert.h

================================================================================

IQDKALERT_AddRef()

Description:
   This method increments the reference count of the IQDKAlert Interface object.
   This allows the object to be shared by multiple callers. The object is
   freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKALERT_AddRef(IQDKAlert * po)

Parameters:

   po :Pointer to the IQDKAlert Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Version:
   Introduced in QDK v 1.0

Side Effects:
   None

See Also:
   IQDKALERT_Release()
   IQDKALERT_QueryInterface()

================================================================================

IQDKALERT_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKALERT_Release(IQDKAlert * po)

Parameters:

   po : Pointer to the IQDKAlert Interface object

Return Value:
   Decremented reference count for the object. The object has been freed
   and is no longer valid if 0 (zero) is returned.

Comments:
   None

Version:
   Introduced in QDK v 1.0

Side Effects:
   None

See Also:
   IQDKALERT_AddRef()
   IQDKALERT_QueryInterface()

================================================================================

Function: IQDKALERT_QueryInterface()

Description:

   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID. This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interface is returned AddRef'ed.  The caller
   is responsible for calling Release() at some point in the future.
   One exception is when the pointer returned is not an interface pointer.
   In that case, the memory will share the lifetime of the object being
   queried, and the returned pointer will not be used to free or release
   the object.

Prototype:
   int IQDKALERT_QueryInterface(IQDKAlert *po, AEECLSID id, void **p)

Parameters:
   po : Pointer to IQDKAlert interface object.

   id : A globally unique id to identify the entity (interface or data)
            that we are trying to query.

   p : Pointer to the data or interface that we want to retrieve.
            If the interface is not available, this is set to NULL.

Return Value:
   Return SUCCESS on success,
   ECLASSNOTSUPPORT if class ID not supported

Comments:
   On failure, QueryInterface() must set *p to NULL.

Version:
   Introduced in QDK v 1.0

Side Effects:
   None

See Also:
   IQDKALERT_AddRef()
   IQDKALERT_Release()


================================================================================

IQDKALERT_GetAlertCount()

Description:
    This function passes the total number of alert messages that are available. 
    Typically, the message ID numbers start at 0 and increment by one for each 
    additional message.  An error value is returned indicating that the function 
	was able to get the total count of available alert messages or an error code
	indicating the reason for which it was not able to get the count. The value 
	of the total count of alert messages is stored in pCount which is passed by 
	reference while calling this function.

Prototype:
    IQDK_ErrorType * GetAlertCount (
          IQDKAlert *po, 
          uint8* pCount) 

Parameters:
    po:	Pointer to the IQDKAlert Interface object.

    pCount:	This field is populated by the function with the number of alert messages available

Return Value:
    SUCCESS:			On success.
    EMEMPTR:			po is not initialized. //TODO
	EQDKNOTINITIALIZED: if IQDKAlert interface is not initialised.
	EBADCLASS:			if po is null.
    EFAILED:			Other failure.

Comments:
    None

Version:
    Introduced in QDK v 1.1. 

See Also:
IQDKALERT_SendAlert()
IQDKALERT_GetAlertTextByIndex()



================================================================================

IQDKALERT_GetAlertTextByIndex()

Description:
   This function passes back the text associated with specified alert msg Id.
   Typically, application starts with alert msg Id 0 and keeps calling
   this function with incremental alert msg id values. An error value is
   returned when application specifies alert msg id that is greater than
   highest valid alert msg id. Essentially, this function allows applications
   to get Alert texts that are associated with the valid alert msd Ids.

Prototype:
    IQDK_ErrorType * GetAlertTextByIndex (
          IQDKAlert *po, 
          uint8 alertIndex, 
          AECHAR** message)

Parameters:
   po :			Pointer to the IQDKAlert Interface object.

   alertIndex : The ID of the alert message whose text to retrieve

   message :	This field is populated by the function with the text associated
					with the alertMsgId.

Return Value:
    SUCCESS:		On success.
    EBADCLASS:		po is NULL.
    EMEMPTR:		Uninitialized po.  // TODO: 
    EBADPARM:		Invalid parameters.
    EQDKOUTOFRANGE: alertIndex out of range.
    EFAILED:		Other failure.

Comments:
   The memory pointed to by the message pointer is owned by IQDKAlert 
   interface and caller is not allowed to either modify or release this memory.

Version:
   Introduced in QDK v 2.0

See Also:
IQDKALERT_SendAlert()
IQDKALERT_GetAlertCount()

================================================================================
IQDKALERT_Init()

Description:
   This function associates application class id with the given instance of
   IQDKAlert interface. Any application that wishes to use IQDKAlert
   interface will have to invoke this method after creating instance
   of IQDKAlert interface and before calling any other IQDKAlert
   interface methods.

Prototype:
   IQDK_ErrorType Init
   (
      IQDKAlert *pMe,
      AEECLSID classId
   )

Parameters:
   pMe      :  Pointer to the IQDKAlert Interface object.
   classId  :  Class id of application that wishes to use the IQDKAlert
               interface.

Return Value:
   On success, SUCCESS, otherwise EFAILED.

Comments:

Version:
   Introduced in QDK v 1.0

See Also:
   none

================================================================================

IQDKALERT_SendAlert()
             
Description:
   This function allows applications to send an alert to another
   QChat user. 

   The application would provide the user interfaces for selecting
   the target and selecting the alert string that it fetched using
   IQDKALERT_GetAlertTextByIndex() method. Target information and alert msg
   Id associated with the selected alert string is then specified in
   IQDKALERT_SendAlert() method to send an alert to selected target.

Prototype:
   IQDK_ErrorType IQDKALERT_SendAlert
   (
      IQDKAlert *po,
      IQDKAddressData *addrs,
      uint8 alertMsgId,
	  uint8 preferredAddrIndex, 
	  boolean waitForPTTPress
   )
 
Parameters:
  
   po :					Pointer to the IQDKAlert Interface object.

   addrs :				Pointer to an IQDKAddressData data structure containing
						  addressing information for the target of the alert. Refer to
						  documentation of IQDKAddressData structure for detailed description
						  of different fields in IQDKAddressData structure and memory management
						  associated with it.

   alertMsgId :			Specifies the alert index. 

   preferredAddrIndex:	Address index used to identify the user during the alert.

   waitForPTTPress:		Flag indicating if the PTT button needs to be pressed for
							the alert to be sent.

Return Value:
   SUCCESS:						The alert has been successfully initiated.
   EQDKBADVALUE:				An invalid/unsupported alertMsgId value was specified.
   EQDKINVALIDQCHATADDRESS :	Invalid target address.
   EQDKNOCALLTYPECAPABILITY:	No Alert origination capability.
   EQDKQCHATSERVICEUNAVAILABLE: The QChat client is currently not registered with the server.
   EBADSTATE:					Qchat is in a suspended state.
   EQDKRESTRICTIONSET:			If the target user has  been blocked.
   EQDKNOTINITIALIZED:			'po' is null or not initialised.
   EFAILED:						Generic failure.

Comments:
   If IQDKALERT_SendAlert() returns SUCCESS, then the QChat Client has
   successfully initiated an attempt to send an alert to another QChat user.
   This does not guarantee that the alert will be successfully delivered
   to the designated target. Notification of the success or failure of
   an alert delivery attempt will be returned to the application by QChat Client
   via IQDK_EVT_ALERT_SENT_STATUS notification event.

Version:
   Introduced in QDK v 2.0

See Also:
   IQDKAlert_GetAlertTextByIndex()
   IQDK_EVT_ALERT_SENT_STATUS
   IQDKAddressData


================================================================================

IQDKALERT_GetEventPayload()
             
Description:
   The event payload is not sent as part of the event.
   The application can get the event payload through the GetEventPayload API. This method allows backward
   compatibility. The event payload data structure could change from one QChat client release to the next. But
   the event does not have to be changed.
   
   This interface returns the payload of any of the events requested in the eventType.
   The payload returned is for any one of the following events:
   
   typedef union
   {
      IQDK_AlertReceivedEventType alertReceivedEvent;
      IQDK_AlertStatusEventType   alertStatusEvent;
   } IQDK_AlertEventPayload;

   The GetEventPayload allows QDK applications to request the event information
   explicitly from the QChat Client, for a specific event type and for a specific
   version of that event.

Prototype:
   IQDK_ErrorType IQDKALERT_GetEventPayload(
         IQDKAlert *pMe, 
         uint32 interfaceId, 
         uint16 eventType, 
         IQDK_AlertEventPayload **payload)
 
Parameters:
  
   pMe :		    Pointer to the IQDKAlert Interface object.
   interfaceId:	    ID number of the Alert interface
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

See Also:
==============================================================================*/
