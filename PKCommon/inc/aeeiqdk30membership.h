#ifndef AEEQDK30MEMBERSHIP_H
#define AEEQDK30MEMBERSHIP_H
/*=============================================================================
 
FILE: AEEQDKMembership.h

SERVICES: The IQDK30Membership interface provides methods to retrieve user addresses
          from Predefined QChat Group Addresses and other utility functions.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IQDK30Membership

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================*/
#include "AEE.h"
#include "AEEQDKAddress.h"
#include "AEEIQDKMembership.h"
#include "AEEIQDK30Provision.h"
//#include "AEEIQDKMembership.bid"	//Backward_Comp_START_Apr_19


#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
                    CLASS DEFINITIONS
=============================================================================*/
#define AEEIID_IQDK30MEMBERSHIP 0x01074d43

#define IQDK_MAX_GROUP_RESTRICTIONS       100

typedef struct
{
   IQDK_RejectReasonType   failReason;
   IQDKAddressData         groupAddr;
   AECHAR                  *adhocGroupName;
} IQDK30_GroupNameEventType;

typedef union
{
   IQDK_GroupMemberListEventType          groupListEventType;
   IQDK30_GroupNameEventType              groupNameEventType;
   IQDK_GroupCallMemberDetailsEventType   groupCallMemberDetailsType;
} IQDK30_MembershipEventType;

typedef struct
{
   IQDKAddressData                groupName;
   IQDKConfIdType				       confId;
   IQDK_AvailabilityActionType	 actionType;
}IQDK_GroupPermissionsInfo;


#define INHERIT_IQDKMembership(iname)\
   INHERIT_IQueryInterface(iname);\
   IQDK_ErrorType (*Init)(iname *po, AEECLSID appClsID);\
   IQDK_ErrorType (*GetListOfMembers)(iname *po, IQDK_AddressOrConfIdType * type);\
   IQDK_ErrorType (*GetListOfGroups)(iname *po, IQDKAddressListType **pGroup);\
   IQDK_ErrorType (*GetGroupName)(iname *po, IQDKConfIdType confId);\
   IQDK_ErrorType (*GetEventPayload)(iname *pMe, uint32 interfaceId, uint16 eventType, IQDK_MembershipEventType **payload)

typedef struct IQDK30Membership IQDK30Membership;

#define INHERIT_IQDK30Membership(iname)\
   INHERIT_IQDKMembership(iname);\
   IQDK_ErrorType (*GetListOfMulticastGroups)(iname *po, IQDKAddressListType **pGroup);\
   IQDK_ErrorType (*Qdk30GetEventPayload)(iname *pMe, uint32 interfaceId, uint16 eventType, IQDK30_MembershipEventType **payload);\
   IQDK_ErrorType (*GetGroupCount)(iname *pMe, uint32 *count);\
   IQDK_ErrorType (*GetGroupByIndex)(iname *pMe, IQDK_GroupPermissionsInfo *talkgroups, uint32 index);\
   IQDK_ErrorType (*GetListOfLocalizedGroups)(iname *po, struct IQDKLocalizedAddressListType **pGroup)

AEEINTERFACE(IQDK30Membership)
{
   INHERIT_IQDK30Membership(IQDK30Membership);
};

#define IQDK30MEMBERSHIP_AddRef(p)                        AEEGETPVTBL((p),IQDK30Membership)->AddRef((p))
#define IQDK30MEMBERSHIP_Release(p)                       AEEGETPVTBL((p),IQDK30Membership)->Release((p))
#define IQDK30MEMBERSHIP_QueryInterface(p,id,obj)         AEEGETPVTBL((p),IQDK30Membership)->QueryInterface((p),(id),(obj))
#define IQDK30MEMBERSHIP_Init(p,clsid)                    AEEGETPVTBL((p),IQDK30Membership)->Init((p),(clsid))
#define IQDK30MEMBERSHIP_GetListOfMembers(p,t)            AEEGETPVTBL((p),IQDK30Membership)->GetListOfMembers((p),(t))
#define IQDK30MEMBERSHIP_GetListOfGroups(p,g)             AEEGETPVTBL((p),IQDK30Membership)->GetListOfGroups((p),(g))
#define IQDK30MEMBERSHIP_GetListOfMulticastGroups(p,g)    AEEGETPVTBL((p),IQDK30Membership)->GetListOfMulticastGroups((p),(g))
#define IQDK30MEMBERSHIP_GetGroupName(p,c)                AEEGETPVTBL((p),IQDK30Membership)->GetGroupName((p),(c))
#define IQDK30MEMBERSHIP_GetEventPayload(p,i,a,b)         AEEGETPVTBL((p),IQDK30Membership)->Qdk30GetEventPayload(p,i,a,b)
#define IQDK30MEMBERSHIP_GetGroupCount(p,a)               AEEGETPVTBL((p),IQDK30Membership)->GetGroupCount(p,a)
#define IQDK30MEMBERSHIP_GetGroupByIndex(p,a,b)           AEEGETPVTBL((p),IQDK30Membership)->GetGroupByIndex(p,a,b)
#define IQDK30MEMBERSHIP_GetListOfLocalizedGroups(p,g)    AEEGETPVTBL((p),IQDK30Membership)->GetListOfLocalizedGroups((p),(g))
#ifdef __cplusplus
}
#endif

#endif // AEEQDKMEMBERSHIP_H

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================


============================================================================

IQDK30_GroupNameEventType

Description:
   This data structure contains the payload for an IQDK_EVT_MEMBER_LIST event.
   The QChat Client sends this event when the group identity has been 
   returned from the QChat Application Server.  
   The information specifies whether or not the attempt to
   retrieve the group identity was successful and - 
   if the query completed successfully, the group address 
   for the call, is sent.

Definition:
   typedef struct
   {
      IQDK_RejectReasonType   failReason;
      IQDKAddressData         groupAddr;
   } IQDK30_GroupNameEventType;

Members:
   failReason  :  This field contains the reason why the group identity
      request has failed. If the request did not fail, this field is
      set to IQDK_SUCCESS.
   groupAddr   :  When the group identity information retrieval is successful, 
      this field contains the group address of the group call. 

See Also:
   IQDK_RejectReasonType
   IQDKAddressData
   IQDK_EVT_MEMBER_LIST

============================================================================

IQDK_GroupPermissionsInfo

Description:
   The data structure provides information about Talk Group permissions 
   such as name of the Talk Group, static confid of the corresponding talk group
   and whether if it is an allow list or deny list.

Definition:
	typedef struct
	{
	   IQDKAddressData                groupName;
	   IQDKConfIdType				       confId;
	   IQDK_AvailabilityActionType    actionType;
	}IQDK_GroupPermissionsInfo;

	typedef enum
	{
	   IQDK_AVAILABILITY_ACTION_ACCEPT_CALLS,
	   IQDK_AVAILABILITY_ACTION_REJECT_CALLS
	} IQDK_AvailabilityActionType;

Members:
   groupName  :  This field contains the name of the talk group
   confId     :  This field contains the static conference id of the talk group
   actionType :  This field tells whether the talk group entry is a part of allow list or deny list.

See Also:
   IQDKAddressData
   IQDK_AvailabilityActionType
   IQDKConfIdType
 
================================================================================
  INTERFACES   DOCUMENTATION
================================================================================

Interface Name: IQDK30Membership

Description:
The IQDK30Membership interface provides a set of API methods to retrieve
QChat user addresses from predefined QChat group membership lists and other 
utility functions.

The IQDK30Membership interface instance is obtained via ISHELL_CreateInstance()
mechanism.

The following header file is required:~
AEEQDKMembership.h

================================================================================

Function: IQDK30MEMBERSHIP_QueryInterface()

Description:

   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID. This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interace is returned AddRef'ed.  The caller
   is responsible for calling Release() at some point in the future.
   One exception is when the pointer returned is not an interface pointer.
   In that case, the memory will share the lifetime of the object being
   queried, and the returned pointer will not be used to free or release
   the object.

Prototype:

   int IQDK30MEMBERSHIP_QueryInterface(IQDK30Membership *po, AEECLSID id, void **p)

Parameters:
   po : Pointer to IQDK30Membership Interface object.
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
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDK30MEMBERSHIP_AddRef()
   IQDK30MEMBERSHIP_Release()

================================================================================

IQDK30MEMBERSHIP_AddRef()

Description:
   This method increments the reference count of the IQDK30Membership Interface 
   object. This allows the object to be shared by multiple callers. 
   The object is freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IQDK30MEMBERSHIP_AddRef(IQDK30Membership *po)

Parameters:
   po : Pointer to the IQDK30Membership Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDK30MEMBERSHIP_Release()

================================================================================

IQDK30MEMBERSHIP_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQDK30MEMBERSHIP_Release(IQDK30Membership *po)

Parameters:
   po : Pointer to the IQDK30Membership Interface object

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
   IQDK30MEMBERSHIP_AddRef()

================================================================================

IQDK30MEMBERSHIP_Init

Description:
   This function associates application class id with the given instance of
   IQDK30Membership interface. Any application that wishes to use IQDK30Membership
   interface will have to invoke this method after creating instance
   of IQDK30Membership interface and before calling any other IQDK30Membership
   interface methods.

Prototype:
   IQDK_ErrorType Init
   (
      IQDK30Membership *pMe,
      AEECLSID classId
   )

Parameters:
   pMe      :  Pointer to the IQDK30Membership Interface object.
   classId  :  Class id of application that wishes to use the IQDK30Membership
               interface.

Return Value:
   SUCCESS 

Comments:
   None

Version:
   Introduced in QDK v 2.0

See Also:
   None

================================================================================

IQDK30MEMBERSHIP_GetListOfMembers

Description:
   This interface returns the list of members that belong to a group call. The 
   information returned is based on an input of either a group address or the 
   confId of the call. 

   Applications would call this function in preparation for
   retrieving the membership list for a predefined or ad hoc group call. 
   The membership list of a group includes all of those users included as
   group members in the group definition found on the QChat
   Application Server.  Likewise, the membership list of a QChat call
   includes all of the members included as targets of the call,
   regardless of whether or not they are actually participating in the
   call.  For example, the membership list of a group call would consist
   of all of the members subscribing to that group, not just the members
   who happened to join the call as participants. Similarly, the
   membership list of a QChat call would include all of the users
   invited to participate in that call, whether they actually join the
   call or are even present to participate in the call. To illustrate,
   if Frank sets up a QChat call to Dean, Jerry, Sammy and Peter, but
   only Dean and Sammy are able to accept and join the call, the
   membership list would still consist of the full list of Frank, Dean,
   Jerry, Sammy and Peter.

   When an application calls IQDK30MEMBERSHIP_GetListOfMembers(), the QChat
   Client requests the membership list from the QChat Application
   Server and notifies the QDK applications by issuing the event
   IQDK_EVT_MEMBER_LIST when it receives the response from the QAS.
   The QDK application can then simply enumerate through the membership 
   list provided in the payload of IQDK_EVT_MEMBER_LIST event.

Prototype:
   IQDK_ErrorType (*GetListOfMembers)(IQDK30Membership *po, IQDK_AddressOrConfIdType * type);

Parameters:
   po : Pointer to the IQDK30Membership Interface object
   type : Pointer to either a group address or a ConfId information of a call

Return Value:
   SUCCESS:  The member list request started successfully, and the
      QChat Client will send the event IQDK_EVT_MEMBER_LIST once it 
      receives a response from the QAS.
   EBADCLASS - if the IQDK30Membership Interface object is NULL
   EQDKQCHATSERVICEUNAVAILABLE - if there is no QChat Service at the time of this request
   EBADPARM - if the 'type' input parameter is NULL
   EQDKINVALIDQCHATADDRESS - if the address information provided is invalid
   EQDKINVALIDCONFERENCEID - if the conference ID information provided is invalid 
   ENOMEMORY - insufficient memory on heap to complete request

Comments:
   In all the cases, where return values are not SUCCESS, the QChat Client will not send 
   out the IQDK_EVT_MEMBER_LIST event.

Version:
   Introduced in QDK v 2.0

See Also:
   IQDK_AddressOrConfIdType

================================================================================
   
IQDK30MEMBERSHIP_GetListOfGroups

Description:
   This interface returns the list of predefined groups, provisioned into the phone. 

Prototype:
   IQDK_ErrorType (*GetListOfGroups)(IQDK30Membership *po, IQDKAddressListType **pGroup);

Parameters:
   po : Pointer to the IQDK30Membership Interface object
   type : Pointer to either a group address or a ConfId information of a call

Return Value:
   SUCCESS - if the request completes successfully
   EBADCLASS - if the IQDK30Membership Interface object is NULL

Comments:
   None

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDKAddressListType 

================================================================================
   
IQDK30MEMBERSHIP_GetListOfLocalizedGroups

Description:
   This interface returns the list of localized groups, provisioned into the phone. 

Prototype:
   IQDK_ErrorType (*GetListOfLocalizedGroups)(IQDK30Membership *po, struct IQDKLocalizedAddressListType **pGroup)

Parameters:
   po : Pointer to the IQDK30Membership Interface object
   pGroup : Returns a list of localized groups

Return Value:
   EQDKNOTINITIALIZED - if QChat has not been initialized successfully
   SUCCESS - if the request completes successfully
   EBADCLASS - if the IQDK30Membership Interface object is NULL

Comments:
   None

Version:
   Introduced in QDK v 3.0

Side Effects:
   None

See Also:
   IQDKLocalizedAddressListType 

================================================================================
   
IQDK30MEMBERSHIP_GetListOfMulticastGroups

Description:
   This interface returns the list of predefined multicast groups, provisioned into the phone. 

Prototype:
   IQDK_ErrorType (*GetListOfMulticastGroups)(IQDK30Membership *po, IQDKAddressListType **pGroup);

Parameters:
   po : Pointer to the IQDK30Membership Interface object
   type : Pointer to either a group address or a ConfId information of a call

Return Value:
   SUCCESS - if the request completes successfully
   EBADCLASS - if the IQDK30Membership Interface object is NULL

Comments:
   None

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDKAddressListType 

================================================================================

IQDK30MEMBERSHIP_GetGroupName

Description:
   This function is used by QDK applications to retrieve the name of 
   the group that is associated with a call. When the QDK application
   receives a call, it has no knowledge of the name of the predefined 
   group/chatroom call. So the QDK application will call this API to 
   get the name of the group/chatroom call. In response to this, the 
   client will send the event IQDK_EVT_GROUP_NAME_AVAILABLE when it 
   has the name of the group.

   CAUTION: This API should only be used when absolute necessary 
   otherwise it will result in a lot of traffic being sent over
   the wireless channel to retrieve the name of the group.

Prototype:
   IQDK_ErrorType (*GetGroupName)(IQDK30Membership *po, IQDKConfIdType confId);

Parameters:
   po : Pointer to the IQDK30Membership Interface object
   confId : ConfId information of the group call, for which the information is
            being requested

Return Value:
   SUCCESS - if the request completes successfully
   EBADCLASS - if the IQDK30Membership Interface object is NULL
   EQDKBADVALUE - if the conference ID information is not of a predefined 
                  group or chatroom call 

Comments:
   None

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDKConfIdType

================================================================================

IQDK30MEMBERSHIP_GetEventPayload

Description:
   The event payload is not sent as part of the event.
   The application can get the event payload through the GetEventPayload API. This method allows backward
   compatibility. The event payload data structure could change from one QChat client release to the next. But
   the event does not have to be changed.

   This interface returns the payload of any of the events requested in the eventType.
   The payload returned is for any one of the following events:
   
   typedef union
   {
      IQDK_GroupMemberListEventType groupListEventType;
      IQDK30_GroupNameEventType       groupNameEventType;
      IQDK_GroupCallMemberDetailsEventType groupCallMemberDetailsType;
   } IQDK30_MembershipEventType;

   The GetEventPayload allows QDK applications to request the event information
   explicitly from the QChat Client, for a specific event type and for a specific
   version of that event.
   
Prototype:
   IQDK_ErrorType IQDK30MEMBERSHIP_GetEventPayload(
   		IQDK30Membership *pMe,
   		uint32 interfaceId,
   		uint16 eventType,
   		IQDK30_MembershipEventType  **payload);

Parameters:

   pMe :		    Pointer to the IQDK30Membership Interface object.
   interfaceId:	    ID number of the Membership interface
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

Side Effects:
   None

See Also:
   IQDK_GroupMemberListEventType
   IQDK30_GroupNameEventType
   IQDK_GroupCallMemberDetailsEventType
   
================================================================================
   
IQDK30MEMBERSHIP_GetGroupCount

Description:
   This interface returns the number of talk groups,
   which can be allowed/denied to participate in the call. 

Prototype:
   IQDK_ErrorType (*GetGroupCount)(IQDK30Membership *pMe, uint32 *count);

Parameters:
   po : Pointer to the IQDK30Membership Interface object
   count : Pointer to number of talk groups.

Return Value:
   SUCCESS - if the request completes successfully
   EFAILED:		        Generic failure

Comments:
   None

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   GetGroupByIndex 

================================================================================
   
IQDK30MEMBERSHIP_GetGroupByIndex

Description:
   This interface provides the information about the TalkGroup Entry based
    on the Index.  This api will give the following informations 
	1. Talk Group Name based on the index
	2. Corresponding conf ID
	3. Whether the Talk Group is a part of Allow list or deny list
 
Prototype:
   IQDK_ErrorType (*GetGroupByIndex)(IQDK30Membership *pMe, IQDK_GroupPermissionsInfo *talkgroups, uint32 index);

Parameters:
   po : Pointer to the IQDK30Membership Interface object
   talkgroups : Pointer to talk groups information.
   index : index of the talk group list, whose information is required.

Return Value:
   SUCCESS - if the request completes successfully
   EFAILED - Generic failure
   EQDKOUTOFRANGE - If the table has no entries or the index given is out of range
   EBADPARAM - If either of the pointer to interface obj or the pointer to 
               talk groups is not valid.

Comments:
   None

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   GetGroupCount 

===================================================================== 

IQDK_MAX_GROUP_RESTRICTIONS

Description:
   Maximum number of group restrictions entries that can be allowed or denied.

=====================================================================*/
