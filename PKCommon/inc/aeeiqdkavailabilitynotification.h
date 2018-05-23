#ifndef AEEQDKAVAILABILITYNOTIFICATION_H
#define AEEQDKAVAILABILITYNOTIFICATION_H
/*==============================================================================
 
FILE: AEEQDKAvailabilityNotification.h

SERVICES: The IQDKAvailabilityNotification interface manages all the methods related to 
Availability Notification feature of QChat.

GENERAL DESCRIPTION:

PUBLIC METHODS:
   IQDKAvailabilityNotification_Init()
   IQDKAvailabilityNotification_CheckAvailabilityStatus()
   IQDKAvailabilityNotification_SendStatusUpdate()
   IQDKAVAILABILITYNOTIFICATION_GetEventPayload()
   

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
==============================================================================*/
/*==============================================================================
                     INCLUDE FILES FOR MODULE
==============================================================================*/
#include "AEE.h"
#include "AEEQDKDataTypes.h"
#include "AEEIQDKAvailabilityNotification.bid"

/*==============================================================================
   DATA STRUCTURES and DEFINES
==============================================================================*/

#define AEEIID_IQDKAVAILABILITYNOTIFICATION 0x01052f18 //Interface ID for 2.0 QueryInterface

typedef struct
{
   IQDK_RejectReasonType       rejectReason; 
   IQDK_AvailabilityStateType  stateOfTarget; 
} IQDK_CheckAvailabilityStatusType;  

typedef struct
{
   IQDKAddressData *          originatorAddress;
   uint32                     receiveTime;
   uint8                      aliasIndexQueried;
} IQDK_AvailabilityQueriedType;  

typedef struct
{
   IQDKAddressData *          originatorAddress; 
   IQDK_AvailabilityStateType state;         
   uint32                     receiveTime;
   uint8                      aliasIndexForWhomMessageIntended;
} IQDK_AvailabilityStatusUpdateReceivedType; 

typedef struct
{
   IQDK_RejectReasonType      rejectReason; 
} IQDK_SendAvailabilityUpdateStatusType;  

typedef union
{
   IQDK_CheckAvailabilityStatusType          checkAvailStatusType;
   IQDK_AvailabilityQueriedType              statusQueriedType;
   IQDK_AvailabilityStatusUpdateReceivedType statusReceivedType;
   IQDK_SendAvailabilityUpdateStatusType     sendAvailabilityStatusType;
} IQDK_AvailabilityNotificationEventType;

/*==============================================================================
                    CLASS DEFINITIONS
==============================================================================*/

typedef struct IQDKAvailabilityNotification IQDKAvailabilityNotification;

AEEINTERFACE(IQDKAvailabilityNotification)
{
   INHERIT_IQueryInterface(IQDKAvailabilityNotification);
   IQDK_ErrorType (*Init)(IQDKAvailabilityNotification *pMe, AEECLSID classId);
   
   IQDK_ErrorType (*CheckAvailabilityStatus)(IQDKAvailabilityNotification *pMe,
                                             IQDKAddressData * targetAddress,
                                             uint8 aliasIndexToBeKnownAs);
   IQDK_ErrorType (*SendAvailabilityUpdate)(IQDKAvailabilityNotification* pMe,
                                            IQDKAddressData * targetAddress,
                                            uint8 aliasIndexToBeKnownAs);
   IQDK_ErrorType (*GetEventPayload)(IQDKAvailabilityNotification *pMe, uint32 interfaceId, uint16 eventType, IQDK_AvailabilityNotificationEventType **payload);
};

#define IQDKAVAILABILITYNOTIFICATION_Init(p, c)                    AEEGETPVTBL((p),IQDKAvailabilityNotification)->Init((p), (c))
#define IQDKAVAILABILITYNOTIFICATION_AddRef(p)                      AEEGETPVTBL((p),IQDKAvailabilityNotification)->AddRef((p))
#define IQDKAVAILABILITYNOTIFICATION_Release(p)                     AEEGETPVTBL((p),IQDKAvailabilityNotification)->Release((p))
#define IQDKAVAILABILITYNOTIFICATION_QueryInterface(p,clsid,pp) AEEGETPVTBL((p),IQDKAvailabilityNotification)->QueryInterface((p),(clsid), (pp))
#define IQDKAVAILABILITYNOTIFICATION_CheckAvailabilityStatus(p,i,u)  AEEGETPVTBL((p),IQDKAvailabilityNotification)->CheckAvailabilityStatus((p), (i), (u))
#define IQDKAVAILABILITYNOTIFICATION_SendAvailabilityUpdate(p,t,u) AEEGETPVTBL((p),IQDKAvailabilityNotification)->SendAvailabilityUpdate((p),(t),(u))
#define IQDKAVAILABILITYNOTIFICATION_GetEventPayload(p,i,a,b)     AEEGETPVTBL((p),IQDKAvailabilityNotification)->GetEventPayload(p,i,a,b)

#endif // AEEQDKAvailabilityNotification_H

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================

IQDK_CheckAvailabilityStatusType

Description:
   This data structure defines availability status received by a client upon 
   querying the state of a target address.

   This structure is associated with the IQDK_EVT_AVAILABILITY_REQUEST_STATUS
   event.

Definition:
   typedef struct
   {
      IQDK_RejectReasonType         rejectReason;
      IQDK_AvailabilityStateType    stateOfTarget;
   }
   IQDK_CheckAvailabilityStatusType;



typedef struct
{
   IQDK_RejectReasonType       rejectReason; // will be set to busy or the error or success.
   IQDK_AvailabilityStateType  stateOfTarget; // set to the availability state of target. WILL Not be set if target is busy.
} IQDK_CheckAvailabilityStatusType;  // this event is for orig to let it know the status of the CheckAvailstatus API.



Members:
      rejectReason : The rejectReason is SUCCESS, if the availability information of the target
                     was successfully retrieved. Otherwise, the rejectReason specifies the error
                     code.
      stateOfTarget : If the rejectReason is SUCCESS, stateOfTarget specifies the availability
                      status of the target address that was queried.
      

See Also:

================================================================================

IQDK_AvailabilityQueriedType

Description:
   An event is sent to the PTT UI using this structure whenever the client receives
   an availability query. If the client is busy when an availability query is received, 
   the originator address, etc are stored until it is time to send an availability status
   update to the query originator.

   This structure is associated with the IQDK_EVT_AVAILABILITY_REQUEST_RECEIVED
   event.

Definition:
   typedef struct
{
   IQDKAddressData       *originatorAddress;
   uint32                receiveTime;
   uint8                 aliasIndexQueried;
} IQDK_AvailabilityQueriedType;


Members:
      originatorAddress : Pointer to the address of the Qchat client that sent the 
                          availability query.
      receiveTime : The time when the client received an availability query, expressed as 
                    the number of seconds since 1980/01/06 00:00:00.
      aliasIndexQueried : The index to the query recepient's list of alternate addresses.

See Also:
   

================================================================================
IQDK_SendAvailabilityUpdateStatusType

Description:
   This structure is used to notify a PTT UI whether or not an availability update 
   was successfully sent or not.

   If the client is busy (say, in a call) at the time of receiving an availability query, 
   an availability status update is sent back to the availability query originator after
   the client becomes available (say, after the call ends).

   This structure is associated with the IQDK_EVT_AVAILABILITY_UPDATE_STATUS
   event.

Definition:
   typedef struct
{
   IQDK_RejectReasonType rejectReason;
} IQDK_SendAvailabilityUpdateStatusType;


Members:
      rejectReason : The rejectReason is SUCCESS, if the availability update status was 
                     successfully sent. Otherwise, rejectReason specifies the error code.

See Also:
  
================================================================================

IQDK_AvailabilityStatusUpdateReceivedType

Description:
   This structure is used to notify a PTT UI as soon as an availability status update 
   is received.

   This structure is associated with the IQDK_EVT_AVAILABILITY_UPDATE_RECEIVED
   event.

Definition:
   typedef struct
{
   IQDKAddressData            *originatorAddress;
   IQDK_AvailabilityStateType state;
   uint32                     receiveTime;
   uint8                      aliasIndexForWhomMessageIntended;
} IQDK_AvailabilityStatusUpdateReceivedType;


Members:
      originatorAddress : Pointer to the address of the Qchat client that sent the 
                          availability update status.
      state : state specifies the availability status of the Qchat client that sent the 
              availability update status.
      receiveTime : The time when the availability status was received, expressed as the 
                    number of seconds since 1980/01/06 00:00:00.
      aliasIndexForWhomMessageIntended : The index to the availability status recepient's 
                                         list of alternate addresses.
      

See Also:
  

================================================================================
  INTERFACES   DOCUMENTATION
================================================================================

Interface Name: IQDKAvailabilityNotification

Description:
IQDKAvailabilityNotification interface provides a set of API methods that are intended to 
be used by applications that want to use the Availability feature of QChat.

A typical sequence of methods to send and retrieve status information would be as follows:

(a) Invoke IQDKAvailabilityNotification_Init on the availability query sender and the 
    availability query recepient.
(b) Invode IQDKAvailabilityNotification_SendAvailabilityUpdate on sender.
(c) Handle IQDK_EVT_AVAILABILITY_REQUEST_RECEIVED event on the recepient. 
(d) Handle IQDK_EVT_AVAILABILITY_REQUEST_STATUS on the sender to retrieve the
    availability information.        

If the recepient is busy (say, in a call) at the time of receiving the availability query,
it may send an availability update after it becomes available (say, after call ends). A
typical sequence of API invocations and event handling would be as follows:

(e) Invoke IQDKAvailabilityNotification_SendAvailabilityUpdate on the recepient.
(f) Handle IQDK_EVT_AVAILABILITY_UPDATE_STATUS on the recepient.
(g) Handle IQDK_EVT_AVAILABILITY_UPDATE_RECEIVED on the original query sender. 



The IQDKAvailabilityNotification interface instance is obtained via ISHELL_CreateInstance()
mechanism.  


The following header file is required:~
AEEQDKAvailabilityNotification.h


================================================================================

IQDKAvailabilityNotification_Init

Description:
   This function associates application class id with the given instance of
   IQDKAvailabilityNotification interface. Any application that wishes to use IQDKAvailabilityNotification
   interface will have to invoke this method after creating instance
   of IQDKAvailabilityNotification interface and before calling any other IQDKAvailabilityNotification
   interface methods.

Prototype:
   IQDK_ErrorType Init
   (
      IQDKAvailabilityNotification *pMe, 
      AEECLSID classId
   )

Parameters:
   pMe      :  Pointer to the IQDKAvailabilityNotification Interface object.
   classId  :  Class id of application that wishes to use the IQDKAvailabilityNotification
               interface.

Return Value:
   On success, SUCCESS, otherwise EFAILED.

Comments:

Version:
   Introduced in QDK v 2.0

See Also:
   none




================================================================================

IQDKAvailabilityNotification_CheckAvailabilityStatus

Description:
   This function starts Availability retrieval transaction. It allows the caller
   to query Availability state of other users on the system. Successful return
   value from this function call denotes that query to fetch user status
   was successfully queued for transmission. 
   
   
Prototype:
   IQDK_ErrorType (*CheckAvailabilityStatus)
   (
      IQDKAvailabilityNotification *pMe,
      IQDKAddressData       targetAddress
      uint8 aliasIndexToBeKnownAs);
   )

Parameters:
   pMe :  Pointer to the IQDKAvailabilityNotification Interface object.
   targetAddress  :  Target client to send the availability query.
   aliasIndexToBeKnownAs : The index to the availability query sender's list of
                           alternate addresses.


Return Value:
   SUCCESS: if the availability query was successfully queued for transmission. 
   EQDKBADVALUE: if NULL pointers are passed as arguments.
   EQDKQCHATSERVICEUNAVAILABLE: if the QChat client is not registered.
   EQDKINVALIDQCHATADDRESS: if targetAddress is invalid.
   EQDKNOCALLTYPECAPABILITY: if the client is restricted from sending
                             an availability query to targetAddress.
   EFAILED: Other error.
   
   Other error codes are also possible.


Version:
   Introduced in QDK v 2.0

See Also:

================================================================================

IQDKAvailabilityNotification_SendAvailabilityUpdate

Description:
   This function is used to send an availability update. An availability update is
   sent by a client under the following circumstance. 

   If the client is busy (say, in a call) at the time of receiving an availability query, 
   an availability status update is sent back to the availability query originator after
   the client becomes available (say, after the call ends).

   Successful return value from this function call denotes that availability update was
   successfully queued for transmission. 


Prototype:
   IQDK_ErrorType (*SendAvailabilityUpdate)(IQDKAvailabilityNotification* pMe,
                                             IQDKAddressData       targetAddress
                                             uint8 aliasIndexToBeKnownAs);
Parameters:
   pMe :  Pointer to the IQDKAvailabilityNotification Interface object.
   targetAddress  :  Target client to send the availability status update.
   aliasIndexToBeKnownAs : The index to the availability update sender's list of
                           alternate addresses.

Return Value:
   SUCCESS: if the availability update was successfully queued for transmission.
   EQDKBADVALUE: if NULL pointers are passed as arguments.
   EQDKQCHATSERVICEUNAVAILABLE: if the QChat client is not registered.
   EQDKINVALIDQCHATADDRESS: if targetAddress is invalid.
   EFAILED: Other error.
   
   Other error codes are also possible.

Version:
   Introduced in QDK v 2.0

See Also:

================================================================================

IQDKAVAILABILITYNOTIFICATION_GetEventPayload
             
Description:
   The event payload is not sent as part of the event.
   The application can get the event payload through the GetEventPayload API. This method allows backward
   compatibility. The event payload data structure could change from one QChat client release to the next. But
   the event does not have to be changed.

   This interface returns the payload of any of the events requested in the eventType.
   The payload returned is for any one of the following events:
   
   typedef union
   {
      IQDK_CheckAvailabilityStatusType          checkAvailStatusType;
      IQDK_AvailabilityQueriedType              statusQueriedType;
      IQDK_AvailabilityStatusUpdateReceivedType statusReceivedType;
      IQDK_SendAvailabilityUpdateStatusType     sendAvailabilityStatusType;
   } IQDK_AvailabilityNotificationEventType;
   
   The GetEventPayload allows QDK applications to request the event information
   explicitly from the QChat Client, for a specific event type and for a specific
   version of that event.

Prototype:
   IQDK_ErrorType IQDKAVAILABILITYNOTIFICATION_GetEventPayload(
         IQDKAlert *pMe, 
         uint32 interfaceId, 
         uint16 eventType, 
         IQDK_AvailabilityNotificationEventType **payload)
 
Parameters:
     
   pMe :		    Pointer to the IQDKAvailabilityNotification Interface object.
   interfaceId:		ID number of the AvailabilityNotification interface
   eventType :	    This field specifies the type of event to retrieve.  The
			        value specified in the eventType field is used to select one
					of the event structures within the payload union.
   payload:			This field is populated by the payload of the event 
					associated with the eventType. While the 'payload' pointer 
                    itself must not be NULL, the pointer pointed to by the payload
                    pointer must be NULL. This is the event payload that will be 
                    allocated by the QChat Client and deleted by the QDK Application
                    calling this API.
Return Value:

   SUCCESS:		        Successful retrieval of the event payload.
   EQDKNOTINITIALIZED:	pMe is not initialised.
   EBADPARM:			If the input parameters are not valid.
   EFAILED:				Generic failure.

Comments:
   The payload pointer is deleted by the QDK application that requests the information.

Version:
   Introduced in QDK v 2.0

See Also:
==============================================================================*/
