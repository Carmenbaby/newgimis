#ifndef  IQDKCALL_H
#define  IQDKCALL_H
/*=================================================================================
FILE:          IQDKCall.h

SERVICES:      IQDKCall interface

DESCRIPTION:   

PUBLIC CLASSES:

INITIALIAZTION AND SEQUENCEING REQUIREMENTS:

        Copyright © 2004 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=================================================================================*/

/*---------------------------------------------------------------------------------
      Include Files
---------------------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEQDKDataTypes.h"
#include "AEEIQDKCall.bid"

//*********************************************************************************
//
// IQDKCall Interface
//
//*********************************************************************************

#define AEEIID_IQDKCALL 0x01052f2d //Interface ID for 2.0 QueryInterface
typedef enum
{
  IQDK_CALL_TERMINATE_IMMEDIATELY,
  IQDK_CALL_TERMINATE_AFTER_PARTICIPANTS_LEAVE,
  IQDK_CALL_TERMINATE_OPTION_INVALID 
} IQDK_CallTerminationOption;

typedef enum
{
   IQDK_UNDEFINED_TYPE,
   IQDK_ADHOC_CALL,
   IQDK_DIRECT_CALL,
   IQDK_PREDEFINED_GROUP_CALL,
   IQDK_PRIVATE_CHATROOM_CALL
}IQDK_CallType;

typedef struct 
{
   uint8   preferredAddrIndex;
   boolean waitForPTTPress;
} IQDK_CommonCallPrefs;

typedef struct 
{
   IQDK_CommonCallPrefs  common;
   boolean addMemberDisableOption;
   IQDK_CallTerminationOption callTerminationOption;
} IQDK_DirectCallPrefs;

typedef struct
{
   IQDK_CommonCallPrefs  common;
   boolean inviteMembers;
   boolean startGroupIfNotRunning;
} IQDK_ChatRoomCallPrefs;

typedef struct
{
   IQDK_CommonCallPrefs  common;
   boolean addMemberDisableOption;
   IQDK_CallTerminationOption callTerminationOption;
   boolean startGroupIfNotRunning;
} IQDK_PredefinedGroupCallPrefs;  

typedef struct
{
   IQDK_CommonCallPrefs  common;
   boolean addMemberDisableOption;
   IQDK_CallTerminationOption callTerminationOption;
   boolean startGroupIfNotRunning;
} IQDK_AdhocGroupCallPrefs;

typedef union
{
   IQDK_DirectCallPrefs             directCallPrefs;
   IQDK_ChatRoomCallPrefs           chatroomCallPrefs;
   IQDK_PredefinedGroupCallPrefs    predefinedGroupCallPrefs;
   IQDK_AdhocGroupCallPrefs         adhocGroupCallPrefs;
} IQDK_CallPreferences; 
  
typedef enum
{
   IQDK_CALLRESULTS_SUCCESS,
   IQDK_CALLRESULTS_FAILED,
   IQDK_CALLRESULTS_MISSED
}IQDK_CallResultsType;

typedef enum
{
   IQDK_FLOOR_GRANT,
   IQDK_FLOOR_RELEASE,
   IQDK_FLOOR_DENIED_IN_USE,
   IQDK_FLOOR_DENIED_NOTAPARTICIPANT,
   IQDK_FLOOR_DENIED_LONELY_USER,
   IQDK_FLOOR_DENIED_NO_PRIVILEGE,
   IQDK_FLOOR_REVOKED_TIMEOUT,
   IQDK_FLOOR_REVOKED_INTERRUPT,
   IQDK_FLOOR_NO_CHANGE
} IQDK_FloorActionType;

typedef struct 
{
   IQDKAddressListType * targetAddrList; 
   uint32                startTime;
   IQDKConfIdType        confId;
} IQDK_CallNoticeEventType; 

typedef struct 
{
   IQDKAddressData       originatorAddr; 
   uint32                connectTime; 
   IQDK_RejectReasonType rejectReason; 
   IQDK_CallType         callType; 
   IQDK_CallResultsType  results; 
   IQDKConfIdType        confId; 
   IQDKAddressData       groupName;
   uint8                 userIdentityIndex;
   boolean               AddMemberDisableOption;
   boolean               bVoicemail; 
} IQDK_CallConnectedEventType; 


typedef struct
{
   IQDK_RejectReasonType rejectReason;
   IQDKConfIdType        confId;
   IQDKAddressData       adderAddr;
   IQDK_CallType         callType;
} IQDK_MembersInvitedEventType; 

typedef struct 
{
   IQDKAddressData       originatorAddr; 
   uint32                receiveTime; 
   IQDK_RejectReasonType rejectReason; 
   IQDK_CallType         callType; 
   IQDKConfIdType        confId; 
   IQDKAddressData       groupName; 
   uint8                 userIdentityIndex;
} IQDK_InviteReceivedEventType;

typedef struct
{
   uint32                callDuration;
   IQDKConfIdType        confId;
   IQDK_CallType         callType;
} IQDK_EndCallEventType;

typedef struct
{
   boolean               myAction;
   boolean               paused;
   IQDK_FloorActionType  floorAction;
   IQDKAddressData       talker;
   uint16                memberCount;
   uint16                participantCount;
   boolean               voiceRelated;
} IQDK_CallStatusEventType;

typedef struct
{
   IQDK_CallTerminationOption option;
   boolean               grpCallMemDetailEvtEnable;
} IQDK_ChangeCallOptions;

typedef union
{
   IQDK_CallNoticeEventType callNoticeType;
   IQDK_CallConnectedEventType callConnectedType;
   IQDK_MembersInvitedEventType memberInvitedType;
   IQDK_InviteReceivedEventType inviteReceivedType;
   IQDK_EndCallEventType endCallType;
   IQDK_CallStatusEventType callStatusType;
} IQDK_CallEventPayloadType;

typedef struct IQDKCall IQDKCall;

AEEINTERFACE(IQDKCall)
{
   INHERIT_IQueryInterface(IQDKCall);
   IQDK_ErrorType (*Init)(IQDKCall *po, AEECLSID appClsID);  
   IQDK_ErrorType (*StartCall)(IQDKCall *po,
                               IQDK_CallType callType,
                               IQDKAddressListType *addrs,
                               IQDKConfIdList *confIdList,
                               IQDK_CallPreferences * prefs);  
   IQDK_ErrorType (*InviteToCall)(IQDKCall *po, IQDKAddressListType *addrs, IQDKConfIdList *confIdList);  
   IQDK_ErrorType (*RequestFloor)(IQDKCall *po);  
   IQDK_ErrorType (*ReleaseFloor)(IQDKCall *po);  
   IQDK_ErrorType (*EndCall)(IQDKCall *po);  
   IQDK_ErrorType (*InviteMembersToChatroom)(IQDKCall *po);
   IQDK_ErrorType (*ChangeRunningCallSettings)(IQDKCall *po, IQDK_ChangeCallOptions *options);
   IQDK_ErrorType (*GetEventPayload)(IQDKCall *pMe, uint32 interfaceId, uint16 eventType, IQDK_CallEventPayloadType **payload);
};

#define  IQDKCALL_AddRef(p)                      AEEGETPVTBL(p,IQDKCall)->AddRef(p)
#define  IQDKCALL_Release(p)                     AEEGETPVTBL(p,IQDKCall)->Release(p)
#define  IQDKCALL_QueryInterface(p,i,p2)         AEEGETPVTBL(p,IQDKCall)->QueryInterface((p),(i),(p2))
#define  IQDKCALL_Init(p,i)                      AEEGETPVTBL(p,IQDKCall)->Init(p,i)
#define  IQDKCALL_StartCall(p,t,a,c,pf)          AEEGETPVTBL(p,IQDKCall)->StartCall(p,t,a,c,pf)
#define  IQDKCALL_InviteToCall(p,a,c)            AEEGETPVTBL(p,IQDKCall)->InviteToCall(p,a,c)
#define  IQDKCALL_RequestFloor(p)                AEEGETPVTBL(p,IQDKCall)->RequestFloor(p)
#define  IQDKCALL_ReleaseFloor(p)                AEEGETPVTBL(p,IQDKCall)->ReleaseFloor(p)
#define  IQDKCALL_EndCall(p)                     AEEGETPVTBL(p,IQDKCall)->EndCall(p)
#define  IQDKCALL_InviteMembersToChatroom(p)     AEEGETPVTBL(p,IQDKCall)->InviteMembersToChatroom(p)
#define  IQDKCALL_ChangeRunningCallSettings(p, o)  AEEGETPVTBL(p,IQDKCall)->ChangeRunningCallSettings(p,o)
#define  IQDKCALL_GetEventPayload(p,i,e,u)       AEEGETPVTBL(p,IQDKCall)->GetEventPayload(p,i,e,u)

/*============================================================================
   INTERFACE DOCUMENTATION
==============================================================================

IQDKCall Interface

Description:
The IQDKCall interface provides a set of API methods to manage QChat calls. This
includes APIs to get QChat Call related
information and methods to control the participation in QChat Calls.

The IQDKCall interface instance is obtained via ISHELL_CreateInstance()
mechanism.

The QChat Client uses the IQDKNotifier interface to send events to the QDK
applications.  

The following header file is required:~
AEEQDKCall.h

==============================================================================
Function: IQDKCALL_AddRef()

Description:
   This method increments the reference count of the IQDKCall Interface object.
   This allows the object to be shared by multiple callers. The object is
   freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKCALL_AddRef(IQDKCall *po)

Parameters:

   po : Pointer to the IQDKCall Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDKCALL_Release()

==============================================================================
Function: IQDKCALL_Release()

Description:
   This method decrements the reference count of an object. The object is
   freed from memory and is no longer valid once the reference count reaches 0
   (zero).

Prototype:
   uint32 IQDKCALL_Release(IQDKCall *po)

Parameters:

   po : Pointer to the IQDKCall Interface object

Return Value:
   Decremented reference count for the object. The object has been freed
   and is no longer valid if 0 (zero) is returned.

Comments:
   None

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDKCALL_AddRef()

=============================================================================
IQDKCALL_Init()

Description:
   This function must be the first API that is called by the QDK application that
   instantiates an object of this interface. This function gives the QChat Client
   a chance to intialize variables for this interface.

Prototype:
   IQDK_ErrorType IQDKCALL_Init(IQDKCALL *po, AEECLSID appClsID)

Parameters:
   po       : Pointer to the IQDKCALL Interface object.
   appClsId : Class ID of the QDK application that is calling this API.

Return Value:
   SUCCESS   : The interface has been successfully initiated
   ENOMEMORY : Memory allocation failure
   EFAILED   : Unknown failure
   EBADCLASS : The po pointer is NULL

Version:
   Introduced in QDK v 2.0

==============================================================================

IQDKCALL_QueryInterface()

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
   int IQDKCALL_QueryInterface(IQDKCall *po, AEECLSID id, void **p)

Parameters:
   po : Pointer to IQDKCall interface
   id : A globally unique id to identify the entity (interface or data) that we
           are trying to query.
   p  : Pointer to the data or interface that we want to retrieve.  If the interface
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

==============================================================================
Function: IQDKCALL_StartCall()

Description:
   This function is used to initiate a call. The same function is also used 
   to restart a call as well as join a chatroom. This API is also used to 
   update the target address in case of optimized PTT.

Prototype:
   IQDK_ErrorType IQDKCALL_StartCall(IQDKCall *po,
                               IQDK_CallType callType,
                               IQDKAddressListType *addrs,
                               IQDKConfIdList *confIdList,
                               IQDK_CallPreferences * prefs);  

Parameters:
   po          [in]: Pointer to IQDKCall interface.
   callType    [in]: call type can be IQDK_UNSPECIFIED_TYPE, IQDK_DIRECT_CALL, 
IQDK_ADHOC_CALL, IQDK_PREDEFINED_GROUP_CALL, IQDK_CHATROOM_CALL
   addrs       [in]: Pointer to IQDKAddressListType
   confIdList  [in]: Pointer to IQDKConfIdList
   prefs       [in]: Pointer to IQDK_CallPreferences
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDKCall interface (po) is NULL 
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered
   EBADSTATE: if the system is in a bad state
   EBADPARAM: if any of the params passed is NULL
   EBOCALLTYPECAPABILITY: if the user does not have the capability to 
originate the type of call that callType is set to.

Comments:
   
Side Effects:
   None

Version:
   Introduced in QDK ver 2.0

See Also:
   

==============================================================================

IQDK_CallType

Description:
   This ENUM distinguishes between the various types of calls the QChat system
   supports.

Definition:
typedef enum
{
   IQDK_UNDEFINED_TYPE,
   IQDK_ADHOC_CALL,
   IQDK_DIRECT_CALL,
   IQDK_PREDEFINED_GROUP_CALL,
   IQDK_PRIVATE_CHATROOM_CALL
}IQDK_CallType;

Members:
   IQDK_UNDEFINED_TYPE        : Invalid call type
   IQDK_ADHOC_CALL            : More than two members, but not predefined
   IQDK_DIRECT_CALL           : Call between two members
   IQDK_PREDEFINED_GROUP_CALL : Address known by QAS that contains a group of members
   IQDK_PRIVATE_CHATROOM_CALL : Group call with set member list, add member not permitted

See Also:
   IQDK_CallResultsType
   IQDK_CallConnectedEventType

================================================================================
IQDK_CallResultsType

Description:
   The ENUM indicates the outcome of a QChat call.

Definition:
typedef enum
{
   IQDK_CALLRESULTS_SUCCESS,
   IQDK_CALLRESULTS_FAILED,
   IQDK_CALLRESULTS_MISSED
}IQDK_CallResultsType;

Members:
   IQDK_CALLRESULTS_SUCCESS    : Call was successful 
   IQDK_CALLRESULTS_FAILED     : Call was not setup
   IQDK_CALLRESULTS_MISSED     : Call was missed by target

See Also:
   IQDK_CallType
   IQDK_CallConnectedEventType

================================================================================
IQDK_FloorActionType

Description:
   The ENUM indicates reason for floor state changes.

Definition:
typedef enum
{
   IQDK_FLOOR_GRANT,
   IQDK_FLOOR_RELEASE,
   IQDK_FLOOR_DENIED_IN_USE,
   IQDK_FLOOR_DENIED_NOTAPARTICIPANT,
   IQDK_FLOOR_DENIED_LONELY_USER,
   IQDK_FLOOR_DENIED_NO_PRIVILEGE,
   IQDK_FLOOR_REVOKED_TIMEOUT,
   IQDK_FLOOR_REVOKED_INTERRUPT,
   IQDK_FLOOR_NO_CHANGE
} IQDK_FloorActionType;

Members:
   IQDK_FLOOR_GRANT : A participant in the call has been granted the floor and 
                      the QDK application may choose to display or otherwise 
                      identify the name of the current talker.  When myAction is 
                      TRUE, it indicates that the user has pressed PTT and was 
                      granted control of the floor.

   IQDK_FLOOR_RELEASE : The talker has released control of the floor and the QDK
                        application may choose to update the display to reflect 
                        the new floor state.

   IQDK_FLOOR_DENIED_IN_USE : The user pressed PTT and was denied control of the 
                              floor because another user of equal or higher 
                              priority currently has the floor.

   IQDK_FLOOR_DENIED_NOTAPARTICIPANT : The user pressed PTT and was denied control
                                       of the floor because he or she is no longer
                                       actually in the call (a condition that can 
                                       arise during a network fade condition).

   IQDK_FLOOR_DENIED_LONELY_USER : The user pressed PTT and was denied control of 
                                   the floor because there are no other users 
                                   participating in the call.  
   IQDK_FLOOR_DENIED_NO_PRIVILEGE : The user pressed PTT and was denied control of
                                    the floor because he or she does not have 
                                    sufficient permission to participate in the 
                                    call as a speaker. The server decides the user
                                    privileges based on the talker preemption rules
                                    located on the server.

   IQDK_FLOOR_REVOKED_TIMEOUT : The user has lost control of the floor because the
                                talk spurt has exceeded the maximum allotted 
                                speaking time.

   IQDK_FLOOR_REVOKED_INTERRUPTED : The user has lost control of the floor because 
                                    another participant with higher speaking 
                                    priority has seized the floor.

   IQDK_FLOOR_NO_CHANGE: There is no change in the floor state since last time the event was sent out.

See Also:
   IQDK_CallStatusEventType

================================================================================

IQDK_CallNoticeEventType

Description:
   This data structure is the payload of the event type IQDK_EVT_CALL_NOTICE.

Definition:
typedef struct
{
   IQDKAddressListType * targetAddrList;
   uint32                startTime;
} IQDK_CallNoticeEventType;

Members:
   targetAddrList : List of targets to place call to. This memory MUST be freed by
                    the QDK application after it has used it otherwise it will result
                    in a memory leak.
   startTime      : time of PTT Press.  In seconds since 1/6/80 00:00:00 GMT

See Also:
   IQDK_CallConnectedEventType
   IQDK_EVT_CALL_NOTICE

================================================================================

IQDK_InviteReceivedEventType

Description:
   This data structure is the payload of the event type IQDK_EVT_INVITE_RECEIVED.


Definition:
typedef struct
{
   IQDKAddressData       originatorAddr;
   uint32                receiveTime;
   IQDK_RejectReasonType rejectReason;
   IQDK_CallType         callType;
   IQDKConfIdType        confId;
   IQDKAddressData       groupName;
   uint8                 userIdentityIndex;
} IQDK_InviteReceivedEventType;


Members:
   originatorAddr : Originator's address. Filled in even if recipient is the originator
   receiveTime    : Time of receipt of invitation. In secs since midnight 1/6/80
   rejectReason   : Provides reason for rejection, or IQDK_SUCCESS when successful
   callType       : Type of call invite, or IQDK_UNDEFINED_TYPE if in error
   confId         : Conference ID of call
   groupName  : The group URL of the call
   userIdentityIndex : Index of user's identity that is used for the duration of the call

Version:
   Introduced in QDK ver. 2.0

See Also:
   IQDK_EVT_INVITE_RECEIVED

================================================================================
IQDK_CallConnectedEventType

Description:
   This data structure is the payload of the event type IQDK_EVT_CALL_CONNECTED.


Definition:
typedef struct
{
   IQDKAddressData       originatorAddr;
   uint32                connectTime;
   IQDK_RejectReasonType rejectReason;
   IQDK_CallType         callType;
   IQDK_CallResultsType  results;
   IQDKConfIdType        confId;
   IQDKAddressData       groupName;
   uint8                 userIdentityIndex;
   boolean               AddMemberDisableOption;
   boolean               bVoicemail;
} IQDK_CallConnectedEventType;

Members:
   originatorAddr : Originator's address. The username and domain value are NULL for this field if the recipient is the originator
   connectTime    : Time of Call start(when server responds). In secs since midnight 1/6/80
   rejectReason   : Provides reason for rejection, or IQDK_SUCCESS when successful
   callType       : Type of call setup, or IQDK_UNDEFINED_TYPE if in error
   results        : Outcome of call setup
   confId         : Conference ID of call
   groupName  : The group URL of the call
   userIdentityIndex : Index of user's identity that is used for the duration of the call
   AddMemberDisableOption : This option specifies whether the originator of the call has allowed other participants of the call the ability to add a member or not. If it set to 1, then no memers can be added to the call. If it is set to 0, then members can be added to the current call.
                            Note that the add member disable privilege parameter (Key=47) which can only be provisioned by the carrier must be set to TRUE in order to enable this option/set to TRUE. 
                            Otherwise, the call will fail with an error return code of EQDKFEATUREUNAVAILABLE.  Please refer to QChat 3.2 Handset Provisioning Guide-80-D1918-1 for more info.
   bVoicemail     : Whether there is a voicemail message available

See Also:
   IQDK_CallNoticeEventType
   IQDK_CallType
   IQDK_CallResultsType
   IQDK_EVT_CALL_CONNECTED
   IQDK_EVT_CALL_NOTICE

================================================================================

IQDK_EndCallEventType

Description:
   This event is received when a call ends.

Definition:
typedef struct
{
   uint32                callDuration;
   IQDKConfIdType        confId;
   IQDK_CallType         callType;
} IQDK_EndCallEventType;

Members:
   callDuration : duration of the call in seconds
   confId       : conference ID of the call
   callType		: Type of call setup, or IQDK_UNDEFINED_TYPE if in error

See Also:
   IQDK_EVT_END_CALL

================================================================================

IQDK_MembersInvitedEventType

Description:
   This event is received when a call ends.

Definition:

typedef struct
{
   IQDK_RejectReasonType rejectReason;
   IQDKConfIdType        confId;
   IQDKAddressData       adderAddr;
   IQDK_CallType         callType;
} IQDK_MembersInvitedEventType;

Members:
   rejectReason   : Provides reason for rejection, or IQDK_SUCCESS when successful
   confId         : Conference ID of call
   adderAddr      : contains the address of member to be added
   callType       : Type of call setup, or IQDK_UNDEFINED_TYPE if in error

See Also:
   IQDK_EVT_MEMBERS_INVITED

================================================================================

IQDK_CallStatusEventType

Description:
   This event is received when the status of the call has changed including change
   in floor action or the number of members or participants.

Definition:

typedef struct
{
   boolean               myAction;
   boolean               paused;
   IQDK_FloorActionType  floorAction;
   IQDKAddressData       talker;
   uint16                memberCount;
   uint16                participantCount;
   boolean               voiceRelated;
} IQDK_CallStatusEventType;

Members:
    myAction : When this field is TRUE, the event has been sent to reflect a change in
               the floor state of this user, as opposed to another participant in the
               call.  The user interface may wish to handle "myAction" floor change
               events differently than it might for every other call participant.

    paused : When this field is TRUE, media received by the QChat Client is currently
             being ignored.  Media continues to be received by the QChat Client, but it
             is not processed (i.e. voice media would not be played).

    floorAction : This field indicates the floor change action that has just transpired
                  in the active call.  The QDK application may choose to update its
                  display or in some other way reflect floor change events to the user
                  depending on the action.

    talker : Contains the QChat address of the current talker, or NULL if no call
             participants are speaking.

  memberCount : Number of members in the call. Member count is set only for predefined
                  group calls and adhoc calls.

    participantCount : Actual number of users participating in the call. Participant
                       count is only set for predefined group calls and adhoc calls.
    
    voiceRelated: Reserved for future use.

See Also:
   IQDK_EVT_CALL_STATUS

================================================================================

IQDK_CallTerminationOption

Description:
 This ENUM describes the different call termination options for the user. The
 call termination options apply only for the originator of the call.

Definition:
typedef enum
{
   IQDK_CALL_TERMINATE_IMMEDIATELY,
   IQDK_CALL_TERMINATE_AFTER_PARTICIPANTS_LEAVE
} IQDK_CallTerminationOption;

Members:
 IQDK_CALL_TERMINATE_IMMEDIATELY: terminate the call as soon as the originator
            ends the call
 IQDK_CALL_TERMINATE_AFTER_PARTICIPANTS_LEAVE: terminate the call only after
            all the particpants leave the call

See Also:

================================================================================

Function: IQDKCALL_InviteToCall()

Description:
   This function is used to invite a target to the call in progress. 

Prototype:
   IQDK_ErrorType IQDKCALL_InviteToCall(IQDKCall *po, IQDKAddressListType *addrs, IQDKConfIdList *confIdList)  

Parameters:
   po          [in]: Pointer to IQDKCall interface.
   addrs       [in]: Pointer to IQDKAddressListType
   confIdList  [in]: Pointer to IQDKConfIdList
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDKCall interface (po) is NULL 
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered
   EQDKBADVALUE: if the function parameters have the wrong value
   EBOCALLTYPECAPABILITY: if the user does not have the capability to 
originate the type of call that callType is set to.

Comments:
   Invite is not sent out to users who are already members or participants in the call. 
Side Effects:
   None

Version:
   Introduced in QDK ver 2.0.

See Also:
============================================================================
Function: IQDKCALL_RequestFloor()

Description:
   This function is used to request the floor when the user is in a call

Prototype:
   IQDK_ErrorType IQDKCALL_RequestFloor(IQDKCall *po)  

Parameters:
   po          [in]: Pointer to IQDKCall interface.
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDKCall interface (po) is NULL 
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered

Comments:
   
Side Effects:
   None

Version:
   Introduced in QDK ver 2.0.

See Also:
  IQDKCALL_ReleaseFloor()

============================================================================
Function: IQDKCALL_ReleaseFloor()

Description:
   This function is used to release the floor when the user is in a call

Prototype:
   IQDK_ErrorType IQDKCALL_ReleaseFloor(IQDKCall *po)

Parameters:
   po          [in]: Pointer to IQDKCall interface.
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDKCall interface (po) is NULL 
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered

Comments:
   
Side Effects:
   None

Version:
   Introduced in QDK ver 2.0.

See Also:
  IQDKCALL_RequestFloor()

============================================================================
Function: IQDKCALL_EndCall()

Description:
   This function is used to exit out of a call.

Prototype:
   IQDK_ErrorType IQDKCALL_EndCall(IQDKCall *po);  

Parameters:
   po          [in]: Pointer to IQDKCall interface.
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDKCall interface (po) is NULL 
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered

Comments:
   
Side Effects:
   None

Version:
   Introduced in QDK ver 2.0.

See Also:

============================================================================
Function: IQDKCALL_InviteMembersToChatroom()

Description:
   This function is used to send out an invite to other members of a chatroom. This is useful only if the invite was initially not sent as part of the arguments to the API IQDKCALL_StartCall. 

Prototype:
   IQDK_ErrorType IQDKCALL_InviteMembersToChatroom(IQDKCall *po);     

Parameters:
   po          [in]: Pointer to IQDKCall interface.
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDKCall interface (po) is NULL 
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered

Comments: Only users who are already members can be invited to the chatroom. 
   
Side Effects:
   None

Version:
   Introduced in QDK ver 2.0.

See Also:

============================================================================
Function: IQDKCALL_ChangeRunningCallSettings()

Description:
   This function can be used to change call settings when the call is active

Prototype:
   IQDK_ErrorType IQDKCALL_ChangeRunningCallSettings(IQDKCall *po, IQDK_ChangeCallOptions *options); 

Parameters:
   po          [in]: Pointer to IQDKCall interface.
   options     [in]: Pointer to IQDK_ChangeCallOptions
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDKCall interface (po) is NULL 
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered

Comments: 
   
Side Effects:
   None

Version:
   Introduced in QDK ver 2.0.

See Also:

============================================================================
Function: IQDKCALL_GetEventPayload

Description:
   The event payload is not sent as part of the event.
   The application can get the event payload through the GetEventPayload API. This method allows backward
   compatibility. The event payload data structure could change from one QChat client release to the next. But
   the event does not have to be changed.

   This interface returns the payload of any of the events requested in the eventType.
   The payload returned is for any one of the following events:
 
   typedef union
   {
      IQDK_CallNoticeEventType callNoticeType;
      IQDK_CallConnectedEventType callConnectedType;
      IQDK_MembersInvitedEventType memberInvitedType;
      IQDK_InviteReceivedEventType inviteReceivedType;
      IQDK_EndCallEventType endCallType;
      IQDK_CallStatusEventType callStatusType;
   } IQDK_CallEventPayloadType;
   
   The GetEventPayload allows QDK applications to request the event information
   explicitly from the QChat Client, for a specific event type and for a specific
   version of that event.

Prototype:

   IQDK_ErrorType IQDKCALL_GetEventPayload(
        IQDKCall *pMe, uint32 interfaceId,
        uint16 eventType, 
        IQDK_CallEventPayloadType **payload)   

Parameters:
  
   pMe :		    Pointer to the IQDKCall Interface object.
   interfaceId:	    ID number of the Call interface
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

Side Effects:
   None

Version:
   Introduced in QDK ver 2.0.

See Also:

============================================================================
IQDK_ChangeCallOptions

Description:
   The IQDK_ChangeCallOptions structure contains call options that can be changed on the fly, while the call is proceeding.
   This structure is an argument to the IQDKCALL_ChangeRunningCallSettings() API.

Definition:
typedef struct
{
   IQDK_CallTerminationOption option;
   boolean               grpCallMemDetailEvtEnable;
} IQDK_ChangeCallOptions;


Members:
   option:  The call termination option
   grpCallMemDetailEvtEnable: Whether detailed in-call membership information is enabled for this call

Comments:

Version:
   Introduced in QDK ver 2.0.

See also:
   IQDK_CallTerminationOption 
   IQDKCALL_ChangeRunningCallSettings
============================================================================
IQDK_CallEventPayload

Description:
   This union is used when retrieving a payload of a call.  
   The field populated depends on the type of event payload retrieved.

Definition:
typedef union
{
   IQDK_CallNoticeEventType callNoticeType;
   IQDK_CallConnectedEventType callConnectedType;
   IQDK_MembersInvitedEventType memberInvitedType;
   IQDK_InviteReceivedEventType inviteReceivedType;
   IQDK_EndCallEventType endCallType;
   IQDK_CallStatusEventType callStatusType;
} IQDK_CallEventPayloadType;

Members:
   callNoticeType     : The payload of an IQDK_EVT_CALL_NOTICE event 
   callConnectedType  : The payload of an IQDK_EVT_CALL_CONNECTED event 
   memberInvitedType  : The payload of an IQDK_EVT_MEMBERS_INVITED event
   inviteReceivedType : The payload of an IQDK_EVT_INVITE_RECEIVED event
   endCallType        : The payload of an IQDK_EVT_END_CALL event
   callStatusType     : The payload of an IQDK_EVT_CALL_STATUS event

Comments:

Version:
   Introduced in QDK ver. 2.0

See also:
   IQDK_CallNoticeEventType
   IQDK_CallConnectedEventType
   IQDK_MembersInvitedEventType
   IQDK_InviteReceivedEventType
   IQDK_EndCallEventType 
   IQDK_CallStatusEventType 
   IQDK_EVT_CALL_NOTICE 
   IQDK_EVT_CALL_CONNECTED 
   IQDK_EVT_MEMBERS_INVITED
   IQDK_EVT_INVITE_RECEIVED 
   IQDK_EVT_END_CALL 
   IQDK_EVT_CALL_STATUS 
============================================================================

IQDK_CommonCallPrefs

Description:
   The common call preferences are those used by each call type.  
   These preferences give the QChat Client variable functionality for each
   call.

Definition:
typedef struct 
{
   uint8   preferredAddrIndex;
   boolean waitForPTTPress;
} IQDK_CommonCallPrefs;

Members:
   preferredAddrIndex: The preferred address index parameter is the index of the 
   address used to identify the QChat user for the duration of the call.  The 
   preferred addresses are provisionable items. waitForPTTPress: The waitForPTTPress 
   parameter indicates whether the call should be started immediately, or 
   whether it is necessary to wait for the next PTT press.  A waitForPTTPress value 
   of TRUE indicates optimized PTT addressing is being used.

Comments:

Version:
   Introduced in QDK ver 2.0.

See also:

============================================================================
IQDK_DirectCallPrefs

Description:

Definition:
typedef struct 
{
   IQDK_CommonCallPrefs  common;
   boolean addMemberDisableOption;
   IQDK_CallTerminationOption callTerminationOption;
} IQDK_DirectCallPrefs;

Members:
   common:  The common call preferences are included.
   addMemberDisableOption: The addMemberDisableOption field specifies whether the 
                           user desires to disable the add member feature.  
                           Note that the add member disable privilege parameter 
                           (Key=47) which can only be provisioned by the carrier 
                           must be set to TRUE in order to enable this option/set 
                           to TRUE. Otherwise, the call will fail with an error return 
                           code of EQDKFEATUREUNAVAILABLE.  Please refer to QChat 3.2 
                           Handset Provisioning Guide-80-D1918-1 for more info.
   callTerminationOption: The callTerminationOption field allows the user to specify 
                          how ended calls are to be handled.

Comments:

Version:
   Introduced in QDK ver 2.0.

See also:
   IQDK_CommonCallPrefs
============================================================================
IQDK_ChatRoomCallPrefs

Description:
   This data structure is contains the preferences related to a chat-room call.

Definition:
typedef struct
{
   IQDK_CommonCallPrefs  common;
   boolean inviteMembers;
   boolean startGroupIfNotRunning;
} IQDK_ChatRoomCallPrefs;

Members:
   common:  The common call preferences are included.
   inviteMembers: The inviteMembers field allows the user to specify whether all 
                  members of the chat-room should be sent an invitation to join.  
   startGroupIfNotRunning: The startGroupIfNotRunning field specifes whether the 
                           chat-room should be started if it is not running.

Comments:

Version:
   Introduced in QDK ver 2.0.

See also:
   IQDK_CommonCallPrefs
============================================================================
IQDK_PredefinedGroupCallPrefs

Description:
   This data structure is used to specify preferences related to a predefined group call.

Definition:
typedef struct
{
   IQDK_CommonCallPrefs  common;
   boolean addMemberDisableOption;
   IQDK_CallTerminationOption callTerminationOption;
   boolean startGroupIfNotRunning;
} IQDK_PredefinedGroupCallPrefs;  

Members:
   common:  The common call preferences are included.
   addMemberDisableOption: The addMemberDisableOption field specifies whether 
                           the user desires to disable the add member feature. 
                           Note that the add member disable privilege parameter 
                           (Key=47) which can only be provisioned by the carrier 
                           must be set to TRUE in order to enable this option/set 
                           to TRUE. Otherwise, the call will fail with an error 
                           return code of EQDKFEATUREUNAVAILABLE.  Please refer to 
                           QChat 3.2 Handset Provisioning Guide-80-D1918-1 for more info.
   callTerminationOption: The callTerminationOption field allows the user to specify 
                          how ended calls are to be handled.  
   startGroupIfNotRunning: The startGroupIfNotRunning field specifes whether the 
                           predefined group should be started if it is not running.

Comments:

Version:
   Introduced in QDK ver 2.0.

See also:
   IQDK_CommonCallPrefs

============================================================================
IQDK_AdhocGroupCallPrefs

Description:
   This data structure is used to specify preferences related to an ad-hoc group call.

Definition:
typedef struct
{
   IQDK_CommonCallPrefs  common;
   boolean addMemberDisableOption;
   IQDK_CallTerminationOption callTerminationOption;
   boolean startGroupIfNotRunning;
} IQDK_AdhocGroupCallPrefs;

Members:
   common:  The common call preferences are included.
   addMemberDisableOption: The addMemberDisableOption field specifies whether 
                           the user desires to disable the add member feature. 
                           Note that the add member disable privilege parameter 
                           (Key=47) which can only be provisioned by the carrier 
                           must be set to TRUE in order to enable this option/set 
                           to TRUE. Otherwise, the call will fail with an error 
                           return code of EQDKFEATUREUNAVAILABLE.  Please refer to 
                           QChat 3.2 Handset Provisioning Guide-80-D1918-1 for more info.
   callTerminationOption: The callTerminationOption field allows the user to specify 
                          how ended calls are to be handled.  
   startGroupIfNotRunning: The startGroupIfNotRunning field specifes whether the predefined 
                           group should be started if it is not running.

Comments:

Version:
   Introduced in QDK ver 2.0.

See also:
   IQDK_CommonCallPrefs

============================================================================
IQDK_CallPreferences

Description:
   This data structure is a union of the available call preferences for each of the call types.  When specifying call preferences for a particular call, the parameter corresponding to the type of call must be populated.


Definition:
typedef union
{
   IQDK_DirectCallPrefs             directCallPrefs;
   IQDK_ChatRoomCallPrefs           chatroomCallPrefs;
   IQDK_PredefinedGroupCallPrefs    predefinedGroupCallPrefs;
   IQDK_AdhocGroupCallPrefs         adhocGroupCallPrefs;
} IQDK_CallPreferences; 
 
Members:
   directCallPrefs:  The preferences related to a direct call
   chatroomCallPrefs:   The preferences related to a chat-room call
   predefinedGroupCallPrefs:  The preferences related to a predefined group call
   adhocGroupCallPrefs: The preferences related to a ad-hoc group call

Comments:

Version:
   Introduced in QDK ver 2.0.

See also:
   IQDK_CommonCallPrefs
   IQDK_DirectCallPrefs
   IQDK_ChatRoomCallPrefs
   IQDK_PredefinedGroupCallPrefs
   IQDK_AdhocGroupCallPrefs
============================================================================*/

#endif /* IQDKCALL_H */

