#ifndef  IQDK30CALL_H
#define  IQDK30CALL_H
/*=================================================================================
FILE:          IQDK30Call.h

SERVICES:      IQDK30Call interface

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
#include "AEEQDK30DataTypes.h"
#include "AEEIQDKCall.h"



//*********************************************************************************
//
// IQDK30Call Interface
//
//*********************************************************************************

#define AEEIID_IQDK30CALL 0x01074d41

/* Inherit IQDK30_AdhocGroupCallPrefs from IQDK_AdhocGroupCallPrefs */
typedef struct
{
   IQDK_CommonCallPrefs  common;
   boolean addMemberDisableOption;
   IQDK_CallTerminationOption callTerminationOption;
   boolean startGroupIfNotRunning;
   AECHAR* adhocGroupName;
   boolean inviteMembers;
}IQDK30_AdhocGroupCallPrefs;

typedef union
{
   IQDK_DirectCallPrefs             directCallPrefs;
   IQDK_ChatRoomCallPrefs           chatroomCallPrefs;
   IQDK_PredefinedGroupCallPrefs    predefinedGroupCallPrefs;
   IQDK30_AdhocGroupCallPrefs       adhocGroupCallPrefs;
} IQDK30_CallPreferences; 


#define INHERIT_IQDKCall(iname)\
   INHERIT_IQueryInterface(iname);\
   IQDK_ErrorType (*Init)(iname *po, AEECLSID appClsID);  \
   IQDK_ErrorType (*StartCall)(iname *po, IQDK_CallType callType, IQDKAddressListType *addrs, IQDKConfIdList *confIdList, IQDK_CallPreferences * prefs);  \
   IQDK_ErrorType (*InviteToCall)(iname *po, IQDKAddressListType *addrs, IQDKConfIdList *confIdList);  \
   IQDK_ErrorType (*RequestFloor)(iname *po);  \
   IQDK_ErrorType (*ReleaseFloor)(iname *po);  \
   IQDK_ErrorType (*EndCall)(iname *po);  \
   IQDK_ErrorType (*InviteMembersToChatroom)(iname *po);\
   IQDK_ErrorType (*ChangeRunningCallSettings)(iname *po, IQDK_ChangeCallOptions *options);\
   IQDK_ErrorType (*GetEventPayload)(iname *pMe, uint32 interfaceId, uint16 eventType, IQDK_CallEventPayloadType **payload)

#define INHERIT_IQDK30Call(iname)\
   INHERIT_IQDKCall(IQDK30Call);\
   IQDK_ErrorType (*Qdk30StartCall)(iname *po, IQDK_CallType callType, IQDKAddressListType *addrs, IQDKConfIdList *confIdList, IQDK30_CallPreferences * prefs);\
   IQDK_ErrorType (*ClearPendingTargetAddress)(iname *po)
typedef struct IQDK30Call IQDK30Call;

AEEINTERFACE(IQDK30Call)
{
   INHERIT_IQDK30Call(IQDK30Call);
};

#define  IQDK30CALL_AddRef(p)                        AEEGETPVTBL(p,IQDK30Call)->AddRef(p)
#define  IQDK30CALL_Release(p)                       AEEGETPVTBL(p,IQDK30Call)->Release(p)
#define  IQDK30CALL_QueryInterface(p,i,p2)           AEEGETPVTBL(p,IQDK30Call)->QueryInterface((p),(i),(p2))
#define  IQDK30CALL_Init(p,i)                        AEEGETPVTBL(p,IQDK30Call)->Init(p,i)
// qdk2.0 version of start call will not be exposed by qdk3.0 interface
//#define  IQDK30CALL_StartCall(p,t,a,c,pf)            AEEGETPVTBL(p,IQDK30Call)->StartCall(p,t,a,c,pf)
#define  IQDK30CALL_InviteToCall(p,a,c)              AEEGETPVTBL(p,IQDK30Call)->InviteToCall(p,a,c)
#define  IQDK30CALL_RequestFloor(p)                  AEEGETPVTBL(p,IQDK30Call)->RequestFloor(p)
#define  IQDK30CALL_ReleaseFloor(p)                  AEEGETPVTBL(p,IQDK30Call)->ReleaseFloor(p)
#define  IQDK30CALL_EndCall(p)                       AEEGETPVTBL(p,IQDK30Call)->EndCall(p)
#define  IQDK30CALL_InviteMembersToChatroom(p)       AEEGETPVTBL(p,IQDK30Call)->InviteMembersToChatroom(p)
#define  IQDK30CALL_ChangeRunningCallSettings(p, o)  AEEGETPVTBL(p,IQDK30Call)->ChangeRunningCallSettings(p,o)
#define  IQDK30CALL_StartCall(p,t,a,c,pf)            AEEGETPVTBL(p,IQDK30Call)->Qdk30StartCall(p,t,a,c,pf)
#define  IQDK30CALL_GetEventPayload(p,i,e,u)         AEEGETPVTBL(p,IQDK30Call)->GetEventPayload(p,i,e,u)
#define  IQDK30CALL_ClearPendingTargetAddress(p)     AEEGETPVTBL(p,IQDK30Call)->ClearPendingTargetAddress(p)

///////////////////
/* TODO Comments needs to be edited accordingly */

/*============================================================================
   INTERFACE DOCUMENTATION
==============================================================================

IQDK30Call Interface

Description:
The IQDK30Call interface provides a set of API methods to manage QChat calls. This
includes APIs to get QChat Call related
information and methods to control the participation in QChat Calls.

The IQDK30Call interface instance is obtained via ISHELL_CreateInstance()
mechanism.

The QChat Client uses the IQDKNotifier interface to send events to the QDK
applications.  

The following header file is required:~
AEEQDKCall.h

==============================================================================
Function: IQDK30CALL_AddRef()

Description:
   This method increments the reference count of the IQDK30Call Interface object.
   This allows the object to be shared by multiple callers. The object is
   freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IQDK30CALL_AddRef(IQDK30Call *po)

Parameters:

   po : Pointer to the IQDK30Call Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Version:
   Introduced in QDK v 3.0

Side Effects:
   None

See Also:
   IQDK30CALL_Release()

==============================================================================
Function: IQDK30CALL_Release()

Description:
   This method decrements the reference count of an object. The object is
   freed from memory and is no longer valid once the reference count reaches 0
   (zero).

Prototype:
   uint32 IQDK30CALL_Release(IQDK30Call *po)

Parameters:

   po : Pointer to the IQDK30Call Interface object

Return Value:
   Decremented reference count for the object. The object has been freed
   and is no longer valid if 0 (zero) is returned.

Comments:
   None

Version:
   Introduced in QDK v 3.0

Side Effects:
   None

See Also:
   IQDK30CALL_AddRef()

=============================================================================
IQDK30CALL_Init()

Description:
   This function must be the first API that is called by the QDK application that
   instantiates an object of this interface. This function gives the QChat Client
   a chance to intialize variables for this interface.

Prototype:
   IQDK_ErrorType IQDK30CALL_Init(IQDK30CALL *po, AEECLSID appClsID)

Parameters:
   po       : Pointer to the IQDK30CALL Interface object.
   appClsId : Class ID of the QDK application that is calling this API.

Return Value:
   SUCCESS   : The interface has been successfully initiated
   ENOMEMORY : Memory allocation failure
   EFAILED   : Unknown failure
   EBADCLASS : The po pointer is NULL

Version:
   Introduced in QDK v 3.0

==============================================================================

IQDK30CALL_QueryInterface()

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
   int IQDK30CALL_QueryInterface(IQDK30Call *po, AEECLSID id, void **p)

Parameters:
   po : Pointer to IQDK30Call interface
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
   Introduced in QDK v 3.0

Side Effects:
   None

See Also:
   None

==============================================================================
Function: IQDK30CALL_StartCall()

Description:
   This function is used to initiate a call. The same function is also used 
   to restart a call as well as join a chatroom. This API is also used to 
   update the target address in case of optimized PTT.

Prototype:
   IQDK_ErrorType IQDK30CALL_StartCall(IQDK30Call *po,
                               IQDK_CallType callType,
                               IQDKAddressListType *addrs,
                               IQDKConfIdList *confIdList,
                               IQDK30_CallPreferences * prefs);  

Parameters:
   po          [in]: Pointer to IQDK30Call interface.
   callType    [in]: call type can be IQDK_UNSPECIFIED_TYPE, IQDK_DIRECT_CALL, 
IQDK_ADHOC_CALL, IQDK_PREDEFINED_GROUP_CALL, IQDK_CHATROOM_CALL
   addrs       [in]: Pointer to IQDKAddressListType
   confIdList  [in]: Pointer to IQDKConfIdList
   prefs       [in]: Pointer to IQDK30_CallPreferences
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDK30Call interface (po) is NULL 
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered
   EBADSTATE: if the system is in a bad state
   EBADPARAM: if any of the params passed is NULL
   EBOCALLTYPECAPABILITY: if the user does not have the capability to 
originate the type of call that callType is set to.
   EQDKNOTINITIALIZED: if the IQDK30Call Interface is not initialized
   EQDKALREADYINACALL: If already in call, cannot clear pending address
   EQDKNOTALKGROUPPERMISSIONS: if the group call cannot be placed due to talk group restriction
   EQDKRESTRICTIONSET: if the target user or address has  been blocked.
   EQDKFEATUREUNAVAILABLE: if the feature is not availble. For example: The addMemberDisableOption field
                           specifies whether the user desires to disable the add member feature. Note that
                           the add member disable privilege parameter (Key=47) which can only 
                           be provisioned by the carrier must be set to TRUE in order 
                           to enable this option/set to TRUE. Otherwise, the call will 
                           fail with an error return code of EQDKFEATUREUNAVAILABLE.


Comments:
   
Side Effects:
   None

Version:
   Introduced in QDK ver 3.0

See Also:
   

==============================================================================

Function: IQDK30CALL_InviteToCall()

Description:
   This function is used to invite a target to the call in progress. 

Prototype:
   IQDK_ErrorType IQDK30CALL_InviteToCall(IQDK30Call *po, IQDKAddressListType *addrs, IQDKConfIdList *confIdList)  

Parameters:
   po          [in]: Pointer to IQDK30Call interface.
   addrs       [in]: Pointer to IQDKAddressListType
   confIdList  [in]: Pointer to IQDKConfIdList
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDK30Call interface (po) is NULL 
   EQDKNOTINITIALIZED: if the IQDK30Call Interface is not initialized
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered
   EQDKBADVALUE: if the function parameters have the wrong value
   EBOCALLTYPECAPABILITY: if the user does not have the capability to 
                       originate adhoc group call - since inviting a user to the current call will convert
                       the call into an adhoc group call.
   EQDKNOTINACALL: if user is not in a call, the user cannot invite another user into the call
   EQDKCANTADDTOCALL: if addmemberDisable bit is set for that call, user will not be able to invite another 
                      user into the call.
   EQDKRESTRICTIONSET: if restriction is set against the target for an adhoc group call

Comments:
   Invite is not sent out to users who are already members or participants in the call. 
Side Effects:
   None

Version:
   Introduced in QDK ver 3.0.

See Also:
============================================================================
Function: IQDK30CALL_RequestFloor()

Description:
   This function is used to request the floor when the user is in a call

Prototype:
   IQDK_ErrorType IQDK30CALL_RequestFloor(IQDK30Call *po)  

Parameters:
   po          [in]: Pointer to IQDK30Call interface.
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDK30Call interface (po) is NULL 
   EQDKNOTINITIALIZED: if the IQDK30Call Interface is not initialized
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered
   EQDKNOTINACALL: if the client is not in a call
   EQDKACTIONNOTCURRENTLYAVAILABLE: if the floor is already held by client

Comments:
   
Side Effects:
   None

Version:
   Introduced in QDK ver 3.0.

See Also:
  IQDK30CALL_ReleaseFloor()

============================================================================
Function: IQDK30CALL_ReleaseFloor()

Description:
   This function is used to release the floor when the user is in a call

Prototype:
   IQDK_ErrorType IQDK30CALL_ReleaseFloor(IQDK30Call *po)

Parameters:
   po          [in]: Pointer to IQDK30Call interface.
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDK30Call interface (po) is NULL 
   EQDKNOTINITIALIZED: if the IQDK30Call Interface is not initialized
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered
   QDKNOTINACALL: if the client is not in a call
   EQDKACTIONNOTCURRENTLYAVAILABLE: if the floor is not currently held by client

Comments:
   
Side Effects:
   None

Version:
   Introduced in QDK ver 3.0.

See Also:
  IQDK30CALL_RequestFloor()

============================================================================
Function: IQDK30CALL_EndCall()

Description:
   This function is used to exit out of a call.

Prototype:
   IQDK_ErrorType IQDK30CALL_EndCall(IQDK30Call *po);  

Parameters:
   po          [in]: Pointer to IQDK30Call interface.
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDK30Call interface (po) is NULL 
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered

Comments:
   
Side Effects:
   None

Version:
   Introduced in QDK ver 3.0.

See Also:

============================================================================
Function: IQDK30CALL_InviteMembersToChatroom()

Description:
   This function is used to send out an invite to other members of a chatroom. This is useful only if the invite was initially not sent as part of the arguments to the API IQDK30CALL_StartCall. 

Prototype:
   IQDK_ErrorType IQDK30CALL_InviteMembersToChatroom(IQDK30Call *po);     

Parameters:
   po          [in]: Pointer to IQDK30Call interface.
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDK30Call interface (po) is NULL 
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered

Comments: Only users who are already members can be invited to the chatroom. 
   
Side Effects:
   None

Version:
   Introduced in QDK ver 3.0.

See Also:

============================================================================
Function: IQDK30CALL_ChangeRunningCallSettings()

Description:
   This function can be used to change call settings when the call is active

Prototype:
   IQDK_ErrorType IQDK30CALL_ChangeRunningCallSettings(IQDK30Call *po, IQDK_ChangeCallOptions *options); 

Parameters:
   po          [in]: Pointer to IQDK30Call interface.
   options     [in]: Pointer to IQDK_ChangeCallOptions
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDK30Call interface (po) is NULL 
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered

Comments: 
   
Side Effects:
   None

Version:
   Introduced in QDK ver 3.0.

See Also:

============================================================================
Function: IQDK30CALL_GetEventPayload

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

   IQDK_ErrorType IQDK30CALL_GetEventPayload(
        IQDK30Call *pMe, uint32 interfaceId,
        uint16 eventType, 
        IQDK_CallEventPayloadType **payload)   

Parameters:
  
   pMe :		    Pointer to the IQDK30Call Interface object.
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
   EQDKNOTINITIALIZED:	if the IQDK30Call Interface is not initialized.
   EBADPARM:		    If the input parameters are not valid.
   EFAILED:		        Generic failure


Comments: 
   The payload pointer is deleted by the QDK application that requests the information.

Side Effects:
   None

Version:
   Introduced in QDK ver 3.0.

See Also:

============================================================================
IQDK_ChangeCallOptions

Description:
   The IQDK_ChangeCallOptions structure contains call options that can be changed on the fly, while the call is proceeding. 
   This structure is an argument to the IQDK30CALL_ChangeRunningCallSettings() API.

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
   Introduced in QDK ver 3.0.

See also:
   IQDK_CallTerminationOption 
   IQDK30CALL_ChangeRunningCallSettings
============================================================================

IQDK30_AdhocGroupCallPrefs

Description:
   This data structure is used to specify preferences related to an ad-hoc group call.

Definition:
typedef struct
{
   IQDK_CommonCallPrefs  common;
   boolean addMemberDisableOption;
   IQDK_CallTerminationOption callTerminationOption;
   boolean startGroupIfNotRunning;
   AECHAR* adhocGroupName;
   boolean inviteMembers;
} IQDK30_AdhocGroupCallPrefs;

Members:
   common:  The common call preferences are included.
   addMemberDisableOption: The addMemberDisableOption field specifies whether the user 
                           desires to disable the add member feature. Note that the add 
                           member disable privilege parameter (Key=47) which can only 
                           be provisioned by the carrier must be set to TRUE in order 
                           to enable this option/set to TRUE. Otherwise, the call will 
                           fail with an error return code of EQDKFEATUREUNAVAILABLE.  
                           Please refer to QChat 3.2 Handset Provisioning Guide-80-D1918-1 
                           for more info.
   callTerminationOption: The callTerminationOption field allows the user to specify how 
                          ended calls are to be handled.  
   startGroupIfNotRunning: The startGroupIfNotRunning field specifes whether the predefined 
                           group should be started if it is not running.
   adhocGroupName: Adhoc group name
   inviteMembers: Invite Members


Comments:

Version:
   Introduced in QDK ver 3.0.

See also:
   IQDK_CommonCallPrefs

============================================================================
IQDK30_CallPreferences

Description:
   This data structure is a union of the available call preferences for each of the 
   call types.  When specifying call preferences for a particular call, the parameter 
   corresponding to the type of call must be populated.


Definition:
typedef union
{
   IQDK_DirectCallPrefs             directCallPrefs;
   IQDK_ChatRoomCallPrefs           chatroomCallPrefs;
   IQDK_PredefinedGroupCallPrefs    predefinedGroupCallPrefs;
   IQDK30_AdhocGroupCallPrefs         adhocGroupCallPrefs;
} IQDK30_CallPreferences; 
 
Members:
   directCallPrefs:  The preferences related to a direct call
   chatroomCallPrefs:   The preferences related to a chat-room call
   predefinedGroupCallPrefs:  The preferences related to a predefined group call
   adhocGroupCallPrefs: The preferences related to a ad-hoc group call

Comments:

Version:
   Introduced in QDK ver 3.0.

See also:
   IQDK_CommonCallPrefs
   IQDK_DirectCallPrefs
   IQDK_ChatRoomCallPrefs
   IQDK_PredefinedGroupCallPrefs
   IQDK30_AdhocGroupCallPrefs
   
============================================================================
Function: IQDK30CALL_ClearPendingTargetAddress()

Description:
   This function clears the pending target address (if any) once the call 
   has ended and the user exits the contacts list.

Prototype:
   IQDK_ErrorType IQDK30CALL_ClearPendingTargetAddress(IQDK30Call *po); 

Parameters:
   po          [in]: Pointer to IQDK30Call interface.
   
Return Value:
   SUCCESS: if function executed correctly.~
   EBADCLASS: if Pointer to IQDK30Call interface (po) is NULL 
   EQDKQCHATSERVICEUNAVAILABLE: if the client is not registered
   EQDKALREADYINACALL : If already in call, cannot clear pending address

Comments: 
   
Side Effects:
   None

Version:
   Introduced in QDK ver 3.0.

See Also:
============================================================================*/

#endif /* IQDK30CALL_H */

