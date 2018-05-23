#ifndef AEEQDKPRESENCEONDEMAND_H
#define AEEQDKPRESENCEONDEMAND_H
/*==============================================================================
 
FILE: AEEIQDKPresenceOnDemand.h

SERVICES: The IQDKPresenceOnDemand interface manages all the methods related to 
Presence/Availability feature of QChat.

GENERAL DESCRIPTION:

PUBLIC METHODS:
   IQDKPRESENCEONDEMAND_AddRef()
   IQDKPRESENCEONDEMAND_Release()
   IQDKPRESENCEONDEMAND_QueryInterface()
   IQDKPRESENCEONDEMAND_Init()
   IQDKPRESENCEONDEMAND_RetrievePresenceInfo()
   IQDKPRESENCEONDEMAND_GetEventPayload
   
   

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
==============================================================================*/
/*==============================================================================
                     INCLUDE FILES FOR MODULE
==============================================================================*/
#include "AEE.h"
#include "AEEIQDK30Call.h"
#include "AEEIQDKPresenceOnDemand.bid"	//Backward_Comp_START_Apr_19

/*==============================================================================
   DATA STRUCTURES and DEFINES
==============================================================================*/

#define AEEIID_IQDKPRESENSEONDEMAND 0x01052f33 //Interface ID for 2.0 QueryInterface

#define IQDK_MAX_PRESENCE_STRING           80

// The MSB of the value indicats whether this is a valid presence state.
#define IS_VALID_PRESENCE_STATE(value) ((uint8)(value) & 0x80)
// The lower 5 bits is the actual presence state
#define GET_PRESENCE_STATE(value) ((uint8)(value) & 0x1F)

typedef uint16 IQDK_PresenceStateType;


// Presence related types
typedef struct
{
   uint8                     availabilityState;
   IQDK_AddressOrConfIdType  addrOrConfIdtype;
   IQDK_RejectReasonType     rejectReason;
} IQDK_PresenceInfoType;

typedef struct
{
   uint16                 count;
   IQDK_PresenceInfoType* presenceInfo;
} IQDK_PresenceInfoEventType;

typedef union
{
   IQDK_PresenceInfoEventType presenceInfoEventType;
} IQDK_PresenceEventType;

/*==============================================================================
                    CLASS DEFINITIONS
==============================================================================*/

typedef struct IQDKPresenceOnDemand IQDKPresenceOnDemand;

AEEINTERFACE(IQDKPresenceOnDemand)
{
   INHERIT_IQueryInterface(IQDKPresenceOnDemand);
   IQDK_ErrorType (*Init)(IQDKPresenceOnDemand *pMe,
                          AEECLSID classId);
   IQDK_ErrorType (*RetrievePresenceInfo)(IQDKPresenceOnDemand *pMe, 
                                          IQDKAddressListType *addrList, 
                                          IQDKConfIdList *confIdList);
   IQDK_ErrorType (*GetEventPayload)(IQDKPresenceOnDemand *pMe, uint32 interfaceId, uint16 eventType, IQDK_PresenceEventType **payload);
   
};

#define IQDKPRESENCEONDEMAND_AddRef(p)                  AEEGETPVTBL((p),IQDKPresenceOnDemand)->AddRef((p))
#define IQDKPRESENCEONDEMAND_Release(p)                 AEEGETPVTBL((p),IQDKPresenceOnDemand)->Release((p))
#define IQDKPRESENCEONDEMAND_QueryInterface(p,clsid,pp) AEEGETPVTBL((p),IQDKPresenceOnDemand)->QueryInterface((p),(clsid),(pp))
#define IQDKPRESENCEONDEMAND_Init(p, c)                 AEEGETPVTBL((p),IQDKPresenceOnDemand)->Init((p), (c))
#define IQDKPRESENCEONDEMAND_RetrievePresenceInfo(p,i, c) AEEGETPVTBL((p),IQDKPresenceOnDemand)->RetrievePresenceInfo((p),(i),(c))
#define IQDKPRESENCEONDEMAND_GetEventPayload(p,i,a,b)   AEEGETPVTBL((p),IQDKPresenceOnDemand)->GetEventPayload(p,i,a,b)


#endif // AEEQDKPRESENCEONDEMAND_H

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================

IQDK_PresenceStateType

Description:
   This data structure defines different presence states a client can be in.
   This is typedef'ed to uint16.

================================================================================

IQDK_PresenceInfoType

Description:
   This data structure is used by application to pass user address information
   to QChat Client. This same structure is used by QChat Client to return presence 
   state of the user back to the application.

Definition:
   typedef struct
   {
       uint8                     availabilityState;
       IQDK_AddressOrConfIdType  addrOrConfIdtype;
       IQDK_RejectReasonType     rejectReason;
   } IQDK_PresenceInfoType;

Members:
      availabilityState: The availability state of the QChat Client
                     
      addrOrConfIdtype : lists two types of addressing options to choose from.
	                     IQDK_ADDR_TYPE: This type makes use of a client address containing a url.
	                     IQDK_CONFID_TYPE: This type makes use of a conference ID.                     
      
      rejectReason     : returns the reason why the QChat client was unable to fetch the 
	                     presence information of the target. If the request did not fail, this field is
                         set to IQDK_SUCCESS.

See Also:
   IQDK_PresenceStateType
   IQDK_PresenceInfoEventType
   IQDK_AddressOrConfIdType

================================================================================

IQDK_PresenceInfoEventType

Description:
   This structure is used as a parameter to IQDKPRESENCEONDEMAND_RetrievePresenceInfo() method and
   also used as a payload of IQDK_EVT_PRESENCE_ON_DEMAND event. The application that calls
   IQDKPRESENCEONDEMAND_RetrievePresenceInfo() is responsible for allocating memory for this
   structure and keeping it allocated until either IQDKPRESENCEONDEMAND_RetrievePresenceInfo()
   fails or a IQDK_EVT_PRESENCE_ON_DEMAND event is received.

Definition:
   typedef struct
   {
      uint16                 count;
      IQDK_PresenceInfoType* presenceInfo;
   } IQDK_PresenceInfoEventType;

Members:
      count        : Size of presenceInfo array.
      presenceInfo : Array of IQDK_PresenceInfoType entries. Each entry
                     holds address information of a user whose presence
                     information needs to be fetched.

See Also
   IQDK_EVT_PRESENCE_ON_DEMAND
   IQDKPRESENCEONDEMAND_RetrievePresenceInfo

================================================================================

IQDK_PresenceEventType

Description:
   This union contains the presence info event type - which is used as a payload of IQDK_EVT_PRESENCE_ON_DEMAND event
Definition:
   typedef union
   {
      IQDK_PresenceInfoEventType presenceInfoEventType;
   } IQDK_PresenceEventType;

Members:
   presenceInfoEventType : structure which helps to get the event payload of IQDK_EVT_PRESENCE_ON_DEMAND

See Also
   IQDK_PresenceInfoEventType
   IQDK_EVT_PRESENCE_ON_DEMAND
   IQDKPRESENCEONDEMAND_RetrievePresenceInfo

================================================================================

IQDK_PresenceAction

Description:
   This enum is used to list the various actions that can associated with a presence
         entry. Each status option has an action - accept or reject calls associated with it.

Definition:
   typdedef enum
   {
        IQDK_PRESENCE_ACCEPT,
        IQDK_PRESENCE_REJECT;
   } IQDK_PresenceAction;

Members:
   IQDK_PRESENCE_ACCEPT : accept calls
   IQDK_PRESENCE_REJECT : reject calls

See Also:
   IQDK_PresenceInfoType
   IQDK_PresenceInfoEventType

================================================================================
  INTERFACES   DOCUMENTATION
================================================================================

Interface Name: IQDKPresenceOnDemand

Description:
IQDKPresenceOnDemand interface provides a set of API methods that are intended to 
be used by applications that want to use the Presence/Availability feature
of QChat.

The IQDKPresenceOnDemand interface instance is obtained via ISHELL_CreateInstance()
mechanism.  

The following header file is required:~
AEEQDKPresence.h

================================================================================

IQDKPRESENCEONDEMAND_AddRef()

Description:
   This method increments the reference count of the IQDKPresenceOnDemand Interface
   object. This allows the object to be shared by multiple callers. The object 
   is freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKPRESENCEONDEMAND_AddRef(IQDKPresenceOnDemand * po)

Parameters:
   po : Pointer to the IQDKPresenceOnDemand Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Version:
   Introduced in QDK v 1.0

Side Effects:
   None

See Also:
   IQDKPRESENCEONDEMAND_Release()
   IQDKPRESENCEONDEMAND_QueryInterface()

================================================================================

IQDKPRESENCEONDEMAND_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKPRESENCEONDEMAND_Release(IQDKPresenceOnDemand * po)

Parameters:
   po : Pointer to the IQDKPresenceOnDemand Interface object

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
   IQDKPRESENCEONDEMAND_AddRef()
   IQDKPRESENCEONDEMAND_QueryInterface()

================================================================================

IQDKPRESENCEONDEMAND_QueryInterface()

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
   int IQDKPRESENCEONDEMAND_QueryInterface(IQDKPresenceOnDemand *po, AEECLSID id, void **p)

Parameters:
   po : Pointer to IQDKPresenceOnDemand interface
   id : A globally unique id to identify the entity (interface or data)
            that we are trying to query.
   p  : Pointer to pointer to the data or interface that we want to retrieve.
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
   IQDKPRESENCEONDEMAND_AddRef()
   IQDKPRESENCEONDEMAND_Release()

================================================================================

IQDKPRESENCEONDEMAND_Init

Description:
   This function associates application class id with the given instance of
   IQDKPresenceOnDemand interface. Any application that wishes to use IQDKPresenceOnDemand
   interface will have to invoke this method after creating instance
   of IQDKPresenceOnDemand interface and before calling any other IQDKPresenceOnDemand
   interface methods.

Prototype:
   IQDK_ErrorType Init
   (
      IQDKPresenceOnDemand *pMe, 
      AEECLSID classId
   )

Parameters:
   pMe      :  Pointer to the IQDKPresenceOnDemand Interface object.
   classId  :  Class id of application that wishes to use the IQDKPresenceOnDemand
               interface.

Return Value:
   On success, SUCCESS, otherwise EFAILED.

Comments:

Version:
   Introduced in QDK v 1.0

See Also:
   none

================================================================================

IQDKPRESENCEONDEMAND_RetrievePresenceInfo

Description:
   This function starts presence retrieval transaction. It allows the caller
   to query presence state of other users on the system. Successful return
   value from this function call denotes that query to fetch user status
   was successfully generated by QChat Client. Upon successful reception of
   user status from network, or if response is not received after predefined
   timeout period, QChat Client sends IQDK_EVT_PRESENCE_ON_DEMAND event to the 
   calling application with the presence state results.

Prototype:
   IQDK_ErrorType IQDKPRESENCEONDEMAND_RetrievePresenceInfo
   (
      IQDKPresenceOnDemand *pMe, 
      IQDKAddressListType *addrList, 
      IQDKConfIdList *confIdList
   )

Parameters:
   pMe :       Pointer to the IQDKPresenceOnDemand Interface object.
   addrList:   Pointer to list of QChat addresses for which the QChat Client  
               retrieves presence info.
   confIdList: Pointer to list of confIds. The QChat Client retrieves presence
               info of the groups corresponding to the confIds.

Return Value:
   SUCCESS: On success.
   EQDKNOTINITIALIZED : pMe is null or not initialised.
   EQDKQCHATSERVICEUNAVAILABLE: QChat service is unavailable.
   EQDKNOCALLTYPECAPABILITY: No capability to retrieve presence.
   ENOMEMORY: No memory.
   EBADPARM: Some or all parameters are NULL.
   EBADCLASS: pIQDKPresenceOnDemand is NULL.
   EFAILED: Other error.

Comments:
   Caller must make sure that presenceInfo is preserved until either
   a EFAILED code is returned from this call or a IQDK_EVT_PRESENCE_ON_DEMAND
   event is received.

Version:
   Introduced in QDK v 2.0

See Also:
   IQDKPRESENCEONDEMAND_GetCount()
   IQDKPRESENCEONDEMAND_GetPresenceEntryByIndex()
   IQDKPRESENCEONDEMAND_GetMyState()
   IQDKPRESENCEONDEMAND_SetMyState()
   IQDKPRESENCEONDEMAND_CancelRetrieval()

================================================================================

IQDKPRESENCEONDEMAND_GetEventPayload

Description:
   The event payload is not sent as part of the event.
   The application can get the event payload through the GetEventPayload API. This method allows backward
   compatibility. The event payload data structure could change from one QChat client release to the next. But
   the event does not have to be changed.

   This interface returns the payload of any of the events requested in the eventType.
   The payload returned is for any one of the following events:

   typedef union
   {
      IQDK_PresenceInfoEventType presenceInfoEventType;
   } IQDK_PresenceEventType;

   The GetEventPayload allows QDK applications to request the event information
   explicitly from the QChat Client, for a specific event type and for a specific
   version of that event.
   
Prototype:
   IQDK_ErrorType IQDKPRESENCEONDEMAND_GetEventPayload(
         IQDKPresenceOnDemand *pMe, 
         uint32 interfaceId, 
         uint16 eventType, 
         IQDK_PresenceEventType **payload)

Parameters:
   pMe :           Pointer to the IQDKPresenceOnDemand Interface object.
   interfaceId:    ID number of the PresenceOnDemand interface
   eventType :     This field specifies the type of event to retrieve.  The
                   value specified in the eventType field is used to select one
                   of the event structures within the payload union.
   payload:        This field is populated by the payload of the event 
                   associated with the eventType. While the 'payload' pointer 
                   itself must not be NULL, the pointer pointed to by the payload
                   pointer must be NULL. This is the event payload that will be 
                   allocated by the QChat Client and deleted by the QDK Application
                   calling this API.

Return Value:
   SUCCESS:             Successful retrieval of the event payload.
   EQDKNOTINITIALIZED:	pMe is not initialised.
   EBADPARM:            If the input parameters are not valid.
   EFAILED:             Generic failure.

Comments:
   The payload pointer is deleted by the QDK application that requests the information. 

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDK_PresenceInfoEventType
==============================================================================*/
