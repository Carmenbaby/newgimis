#ifndef AEEQDKMEMBERSHIP_H
#define AEEQDKMEMBERSHIP_H
/*=============================================================================
 
FILE: AEEQDKMembership.h

SERVICES: The IQDKMembership interface provides methods to retrieve user addresses
          from Predefined QChat Group Addresses and other utility functions.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IQDKMembership

        Copyright © 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================*/
#include "AEE.h"
#include "AEEQDKAddress.h"
#include "AEEIQDKMembership.bid"	//Backward_Comp_START_Apr_19

#ifdef __cplusplus
extern "C"
{
#endif

/*=============================================================================
                    CLASS DEFINITIONS
=============================================================================*/
#define AEEIID_IQDKMEMBERSHIP 0x01052f31 //Interface ID for 2.0 QueryInterface

typedef struct IQDKMembership IQDKMembership;

typedef struct
{
   boolean participant;
   IQDKAddressData address;
}IQDK_GroupMemberType;

typedef struct
{
   IQDK_RejectReasonType   failReason;
   uint16                  count;
   boolean                 truncated;
   boolean                 forCurrentCall;
   IQDK_GroupMemberType    *member;
}IQDK_GroupMemberListEventType;

typedef struct
{
   IQDK_RejectReasonType   failReason;
   IQDKAddressData         groupAddr;
} IQDK_GroupNameEventType;

typedef enum
{
   IQDK_USER_STATUS_PARTICIPATING,
   IQDK_USER_STATUS_LEAVE_CALL,
   IQDK_USER_STATUS_AVAILABLE,
   IQDK_USER_STATUS_BUSY,
   IQDK_USER_STATUS_VOCODER_INCOMPATIBLE,
   IQDK_USER_STATUS_NO_PRIVILEGE,
   IQDK_USER_STATUS_NO_CALLTYPE_CAPABILITY,
   IQDK_USER_STATUS_NOT_REGISTERED,
   IQDK_USER_STATUS_NOT_ANSWERING,
   IQDK_USER_STATUS_EXCLUDED_MEMBERSHIP,
   IQDK_USER_STATUS_UNKNOWN
} IQDK_UserParticipationStatus;

typedef struct
{
   IQDKAddressData              userAddr;
   uint32                       participationLeaveTime; 
   IQDK_UserParticipationStatus participationStatus; 
} IQDK_GroupCallMemberInfo; 

typedef struct 
{
   IQDKAddressData                originatorAddr; 
   IQDKAddressData                groupAddr;
   uint32                         numMemberInfo;
   IQDK_GroupCallMemberInfo       *memberInfoPtr; 
} IQDK_GroupCallMemberDetailsEventType;

typedef union
{
   IQDK_GroupMemberListEventType groupListEventType;
   IQDK_GroupNameEventType       groupNameEventType;
   IQDK_GroupCallMemberDetailsEventType groupCallMemberDetailsType;
} IQDK_MembershipEventType;

AEEINTERFACE(IQDKMembership)
{
   INHERIT_IQueryInterface(IQDKMembership);
   IQDK_ErrorType (*Init)(IQDKMembership *po, AEECLSID appClsID);
   IQDK_ErrorType (*GetListOfMembers)(IQDKMembership *po, IQDK_AddressOrConfIdType * type);
   IQDK_ErrorType (*GetListOfGroups)(IQDKMembership *po, IQDKAddressListType **pGroup);
   IQDK_ErrorType (*GetGroupName)(IQDKMembership *po, IQDKConfIdType confId);
   IQDK_ErrorType (*GetEventPayload)(IQDKMembership *pMe, uint32 interfaceId, uint16 eventType, IQDK_MembershipEventType **payload);
};


#define IQDKMEMBERSHIP_AddRef(p)                        AEEGETPVTBL((p),IQDKMembership)->AddRef((p))
#define IQDKMEMBERSHIP_Release(p)                       AEEGETPVTBL((p),IQDKMembership)->Release((p))
#define IQDKMEMBERSHIP_QueryInterface(p,id,obj)         AEEGETPVTBL((p),IQDKMembership)->QueryInterface((p),(id),(obj))
#define IQDKMEMBERSHIP_Init(p,clsid)                    AEEGETPVTBL((p),IQDKMembership)->Init((p),(clsid))
#define IQDKMEMBERSHIP_GetListOfMembers(p,t)            AEEGETPVTBL((p),IQDKMembership)->GetListOfMembers((p),(t))
#define IQDKMEMBERSHIP_GetListOfGroups(p,g)             AEEGETPVTBL((p),IQDKMembership)->GetListOfGroups((p),(g))
#define IQDKMEMBERSHIP_GetGroupName(p,c)                AEEGETPVTBL((p),IQDKMembership)->GetGroupName((p),(c))
#define IQDKMEMBERSHIP_GetEventPayload(p,i,a,b)         AEEGETPVTBL((p),IQDKMembership)->GetEventPayload(p,i,a,b)

#ifdef __cplusplus
}
#endif

#endif // AEEQDKMEMBERSHIP_H

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================

IQDK_GroupMemberType

Description:
   This data structure identifies a single group member returned in a
   group membership list.  Each member is identified by their user
   address, plus a "best guess estimate" as to
   whether or not the member is a current participant in the call.
   The "best guess estimate" is determined by the QChat Client based on
   those call members who have been granted floor control during the
   duration of the user's participation in the call.

Definition:
   typedef struct
   {
      boolean         participant;
      IQDKAddressData address;
   } IQDK_GroupMemberType;

Members:
   participant :  When this field is TRUE, the group member has been
      determined to be a participant in the active call, using a "best
      guess estimate" algorithm.  The "best guess estimate" is
      determined by the QChat Client based on those call members who
      have been granted floor control during the duration of the user's
      participation in the call.  It is important to note that the
      QChat Client does not have knowledge of floor control changes
      prior to the user joining an already active call.  Hence, though
      participant may be FALSE, this does not necessarily indicate that
      the member is not actually listening in on the call.
   address  :  Contains the QChat user address of the member
      identified by this data structure.

See Also:
   IQDKAddressData 
================================================================================

IQDK_GroupMemberListEventType

Description:
   This data structure contains the payload for an
   IQDK_EVT_MEMBER_LIST event.  The QChat Client sends this event
   when a group membership list has been returned from the QChat Application 
   Server.  The list includes information about whether or not the attempt to
   retrieve the membership list was successful and - 
   if the query completed successfully, the complete
   membership list for the call or group that was queried, is sent.

Definition:
   typedef struct
   {
      IQDK_RejectReasonType   failReason;
      uint16                  count;
      boolean                 truncated;
      boolean                 forCurrentCall;
      IQDK_GroupMemberType    *member;
   } IQDK_GroupMemberListEventType;

Members:
   failReason  :  This field contains the reason why a group membership
      request has failed. If the request did not fail, this field is
      set to IQDK_SUCCESS.
   count :  When the group membership information retrieval is successful, 
      this field contains the number of members returned in the member field,  
      otherwise, the count is set to zero.
   truncated :  When this field is TRUE, it means that only a portion of
      group membership list could be retrieved due to network limitations.
   forCurrentCall :  When this field is TRUE, the group membership information
      sent is for the current QChat call - i.e. the call is
      active and the application that receives this event could choose as to 
      how the membership list is displayed to the user, based on the current call
      state.
   member   :  When the group membership information retrieval is successful, 
      this field contains an array of IQDK_GroupMemberType records, 
      containing information about each member of the requested call.

See Also:
   IQDK_GroupMemberType
   IQDK_RejectReasonType
   IQDK_EVT_MEMBER_LIST

============================================================================

IQDK_GroupNameEventType

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
   } IQDK_GroupNameEventType;

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

IQDK_GroupCallMemberInfo

Description:
   The data structure provides information on the members in the group 
   call such as the address of each user, the participation or leave time 
   of each user and the participation status of each user at the time
   this event is sent.

Definition:
   typedef struct
   {
      IQDKAddressData     userAddr;
      uint32              participationLeaveTime; 
      IQDK_UserParticipationStatus participationStatus; 
   } IQDK_GroupCallMemberInfo; 

Members:
   userAddr               : Contains user address of the member of the group call 
   participationLeaveTime : Indicates the (GMT)time when the user joined the call or
                            the time when the user left the call. If set to zero,
                            it indicates that the user never participated in the 
                            call.
   participationStatus    : Indicates participation status of the user when 
                            the event is sent. The values are set as per the
                            IQDK_UserParticipationStatus list.

See Also:
   IQDKAddressData
  
=======================================================================

IQDK_UserParticipationStatus

Description:
   The IQDK_UserParticipationStatus data type identifies the status of an 
   individual member of the group call.

Definition:
   typedef enum
   {
      IQDK_USER_STATUS_PARTICIPATING,
      IQDK_USER_STATUS_LEAVE_CALL,
      IQDK_USER_STATUS_AVAILABLE,
      IQDK_USER_STATUS_BUSY,
      IQDK_USER_STATUS_VOCODER_INCOMPATIBLE,
      IQDK_USER_STATUS_NO_PRIVILEGE,
      IQDK_USER_STATUS_NO_CALLTYPE_CAPABILITY,
      IQDK_USER_STATUS_NOT_REGISTERED,
      IQDK_USER_STATUS_NOT_ANSWERING,
      IQDK_USER_STATUS_EXCLUDED_MEMBERSHIP,
      IQDK_USER_STATUS_UNKNOWN
   } IQDK_UserParticipationStatus;
Values:
   IQDK_USER_STATUS_PARTICIPATING  : The user is currently participating in the call.
   IQDK_USER_STATUS_LEAVE_CALL     : The user was participating in the call but has currently left the call.
   IQDK_USER_STATUS_AVAILABLE      : The user is available but is not yet participating in the call.
   IQDK_USER_STATUS_BUSY : The user is busy is another call or the user has set the QChat presence 
                           state to one of the reject states - to 'Away from Phone' or 'Do not disturb'.       
   IQDK_USER_STATUS_VOCODER_INCOMPATIBLE  : The vocoder setting on the users phone is incompatible with the
                                        Vocoder selected for the active call.
   IQDK_USER_STATUS_NO_PRIVILEGE    : The user has call restrictions set against the originator.
   IQDK_USER_STATUS_NO_CALLTYPE_CAPABILITY : The user is not provisioned with the associated call type 
                                        capability
                                         
   IQDK_USER_STATUS_NOT_REGISTERED : The user is not registered in the QChat System
   IQDK_USER_STATUS_NOT_ANSWERING  : The user is not responding to QChat call invite signalling.
   IQDK_USER_STATUS_EXCLUDED_MEMBERSHIP : The user was not included in the membership of the call. For 
                                     example, when add member request contains a foreign dispatch user 
                                     address, the foreign dispatch user is not added to the membership
                                     list of the call.
   IQDK_USER_STATUS_UNKNOWN : The user status is not known.

============================================================================

IQDK_GroupCallMemberDetailsEventType

Description:
   This event contains information about the group that is currently in a call, 
   based on the information sent by the network. The data structure provides 
   user address of the group call originator, user address of the last user 
   that added a user to the group call, address of the group, if it is a 
   predefined or chat room group call.
   The data structure also provides information on the members in the group 
   call such as the address of each user, the participation or leave time of
   each user and the participation status of each user at the time this event
   is sent.
   
Definition:
   typedef enum
   {
      IQDK_USER_STATUS_PARTICIPATING,
      IQDK_USER_STATUS_LEAVE_CALL,
      IQDK_USER_STATUS_AVAILABLE,
      IQDK_USER_STATUS_BUSY,
      IQDK_USER_STATUS_VOCODER_INCOMPATIBLE,
      IQDK_USER_STATUS_NO_PRIVILEGE,
      IQDK_USER_STATUS_INVALID_OPCODE,
      IQDK_USER_STATUS_BAD_SYNTAX,
      IQDK_USER_STATUS_NO_CALLTYPE_CAPABILITY,
      IQDK_USER_STATUS_NOT_REGISTERED,
      IQDK_USER_STATUS_NOT_ANSWERING,
      IQDK_USER_STATUS_EXCLUDED_MEMBERSHIP,
      IQDK_USER_STATUS_UNKNOWN
   } IQDK_UserParticipationStatus;

   typedef struct
   {
      IQDKAddressData     userAddr;
      uint32              participationLeaveTime; 
      IQDK_UserParticipationStatus participationStatus; 
   } IQDK_GroupCallMemberInfo; 

   typedef struct 
   {
      IQDKAddressData     originatorAddr; 
      IQDKAddressData     groupAddr;
      uint32              numMemberInfo;
      IQDK_GroupCallMemberInfo *memberInfoPtr; 
   } IQDK_GroupCallMemberDetailsEventType;

Members:
   originatorAddr : Contains user address of the originator of the group call 
   groupAddr      : Contains group address of the group call, if it is a 
                    predefined or chat room group call
   numMemberInfo  : Indicates the number of members of the group call  
                    whose information is packed into this event starting
                    with the memberInfoPtr
   memberInfoPtr  : Contains information on the user address, participation 
                    or leave time of the user and participation status of 
                    the user

See Also:
   IQDK_GroupCallMemberInfo 
   IQDKAddressData 
   
================================================================================
  INTERFACES   DOCUMENTATION
================================================================================

Interface Name: IQDKMembership

Description:
The IQDKMembership interface provides a set of API methods to retrieve
QChat user addresses from predefined QChat group membership lists and other 
utility functions.

The IQDKMembership interface instance is obtained via ISHELL_CreateInstance()
mechanism.

The following header file is required:~
AEEQDKMembership.h

================================================================================

Function: IQDKMEMBERSHIP_QueryInterface()

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

   int IQDKMEMBERSHIP_QueryInterface(IQDKMembership *po, AEECLSID id, void **p)

Parameters:
   po : Pointer to IQDKMembership Interface object.
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
   IQDKMEMBERSHIP_AddRef()
   IQDKMEMBERSHIP_Release()

================================================================================

IQDKMEMBERSHIP_AddRef()

Description:
   This method increments the reference count of the IQDKMembership Interface 
   object. This allows the object to be shared by multiple callers. 
   The object is freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKMEMBERSHIP_AddRef(IQDKMembership *po)

Parameters:
   po : Pointer to the IQDKMembership Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDKMEMBERSHIP_Release()

================================================================================

IQDKMEMBERSHIP_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKMEMBERSHIP_Release(IQDKMembership *po)

Parameters:
   po : Pointer to the IQDKMembership Interface object

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
   IQDKMEMBERSHIP_AddRef()

================================================================================

IQDKMEMBERSHIP_Init

Description:
   This function associates application class id with the given instance of
   IQDKMembership interface. Any application that wishes to use IQDKMembership
   interface will have to invoke this method after creating instance
   of IQDKMembership interface and before calling any other IQDKMembership
   interface methods.

Prototype:
   IQDK_ErrorType Init
   (
      IQDKMembership *pMe,
      AEECLSID classId
   )

Parameters:
   pMe      :  Pointer to the IQDKMembership Interface object.
   classId  :  Class id of application that wishes to use the IQDKMembership
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

IQDKMEMBERSHIP_GetListOfMembers

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

   When an application calls IQDKMEMBERSHIP_GetListOfMembers(), the QChat
   Client requests the membership list from the QChat Application
   Server and notifies the QDK applications by issuing the event
   IQDK_EVT_MEMBER_LIST when it receives the response from the QAS.
   The QDK application can then simply enumerate through the membership 
   list provided in the payload of IQDK_EVT_MEMBER_LIST event.

Prototype:
   IQDK_ErrorType (*GetListOfMembers)(IQDKMembership *po, IQDK_AddressOrConfIdType * type);

Parameters:
   po : Pointer to the IQDKMembership Interface object
   type : Pointer to either a group address or a ConfId information of a call

Return Value:
   SUCCESS:  The member list request started successfully, and the
      QChat Client will send the event IQDK_EVT_MEMBER_LIST once it 
      receives a response from the QAS.
   EBADCLASS - if the IQDKMembership Interface object is NULL
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
   
IQDKMEMBERSHIP_GetListOfGroups

Description:
   This interface returns the list of predefined groups, provisioned into the phone. 

Prototype:
   IQDK_ErrorType (*GetListOfGroups)(IQDKMembership *po, IQDKAddressListType **pGroup);

Parameters:
   po : Pointer to the IQDKMembership Interface object
   pGroup : Pointer to a structure containing addresses of predefined groups

Return Value:
   SUCCESS - if the request completes successfully
   EBADCLASS - if the IQDKMembership Interface object is NULL

Comments:
   None

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDKAddressListType 

================================================================================
   
IQDKMEMBERSHIP_GetGroupName

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
   IQDK_ErrorType (*GetGroupName)(IQDKMembership *po, IQDKConfIdType confId);

Parameters:
   po : Pointer to the IQDKMembership Interface object
   confId : ConfId information of the group call, for which the information is
            being requested

Return Value:
   SUCCESS - if the request completes successfully
   EBADCLASS - if the IQDKMembership Interface object is NULL
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

IQDKMEMBERSHIP_GetEventPayload

Description:
   The event payload is not sent as part of the event.
   The application can get the event payload through the GetEventPayload API. This method allows backward
   compatibility. The event payload data structure could change from one QChat client release to the next. But
   the event does not have to be changed.

   This interface returns the payload of any of the events requested in the eventType.
   The payload returned is for any one of the following events:

  IQDK_EVT_MEMBER_LIST
  IQDK_EVT_DETAIL_GRP_CALL_INFO
  IQDK_EVT_GROUP_NAME_AVAILABLE

   The GetEventPayload allows QDK applications to request the event information
   explicitly from the QChat Client, for a specific event type and for a specific
   version of that event.
   
Prototype:
   IQDK_ErrorType IQDKMEMBERSHIP_GetEventPayload(
   		IQDKMembership *pMe,
   		uint32 interfaceId,
   		uint16 eventType,
   		IQDK_MembershipEventType  **payload);

Parameters:

   pMe :		    Pointer to the IQDKMembership Interface object.
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
   IQDK_GroupNameEventType
   IQDK_GroupCallMemberDetailsEventType
   IQDK_MembershipEventType
   
================================================================================

IQDK_MembershipEventType

Description:
   This structure contains the event payload retrieved with an 
   IQDKMEMBERSHIP_GetEventPayload API call. The payload will be for one of the
   events defined by the union.

Definition:

typedef union
{
   IQDK_GroupMemberListEventType groupListEventType;
   IQDK_GroupNameEventType       groupNameEventType;
   IQDK_GroupCallMemberDetailsEventType groupCallMemberDetailsType;
} IQDK_MembershipEventType;

Members:
   groupListEventType : contains fields relating to the IQDK_GroupMemberListEventType
                        such as count and group member information
   groupNameEventType : contains fields relating to the IQDK_GroupNameEventType
                        such as the group address
   groupCallMemberDetailsType : contais fields relating to the IQDK_GroupCallMemberDetailsEventType
                                such as the originator address, group address, and 
                                number of members

See Also:
   IQDK_GroupMemberListEventType
   IQDK_GroupNameEventType
   IQDK_GroupCallMemberDetailsEventType

=====================================================================*/
