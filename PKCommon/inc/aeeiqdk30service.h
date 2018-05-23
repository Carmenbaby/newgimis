#ifndef AEEQDK30SERVICE_H
#define AEEQDK30SERVICE_H
/*=============================================================================
 
FILE: AEEIQDK30Service.h

SERVICES: The IQDK30Service interface provides functions to manage QChat service 
          connectivity.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IQDK30Service
   IQDK30SERVICE_Init
   IQDK30SERVICE_AddRef
   IQDK30SERVICE_Release
   IQDK30SERVICE_Online
   IQDK30SERVICE_Offline
   IQDK30SERVICE_Reset
   IQDK30SERVICE_Suspend
   IQDK30SERVICE_Resume
   IQDK30SERVICE_GetCurrentDNSInfo
   IQDK30SERVICE_GetCurrentRLSInfo
   IQDK30SERVICE_ReleaseQChatNetworkResources
   IQDK30SERVICE_GetLastQChatOnlineStatus
   IQDK30SERVICE_GetEventPayload


        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================*/
#include "AEE.h"
#include "AEEQDK30DataTypes.h"
#include "AEEIQDKService.h"
//#include "AEEIQDKService.bid"	//Backward_Comp_START_Apr_19
#include "AEEQDKAddress.h"
#include "AEENetworkTypes.h"


#ifdef __cplusplus
extern "C"
{
#endif

/*==============================================================================
   DATA STRUCTURES and DEFINES
==============================================================================*/

#define AEEIID_IQDK30SERVICE 0x01074d46

typedef AECHAR     IQDK_HostEntryType[IQDK_MAX_DOMAIN_SIZE];

#define IQDK_EVT_ENHANCEDSTATUS_DETAILS_BASE                (0x1000 + EVT_USER)
#define IQDK_EVT_OFFLINE_COMPLETE                      (0x1 + IQDK_EVT_ENHANCEDSTATUS_DETAILS_BASE)
#define IQDK_EVT_NETWORK_STATUS                        (0x2 + IQDK_EVT_ENHANCEDSTATUS_DETAILS_BASE)
#define IQDK_EVT_QCHAT_SSINFO                          (0x3 + IQDK_EVT_ENHANCEDSTATUS_DETAILS_BASE)

typedef struct 
{
   IQDK_HostEntryType DNSServerName;
   IPAddr DNSServerIP;
   uint32 DNSTTL; //in seconds
   IQDK_HostEntryType SRV_UDP;
   IQDK_HostEntryType SRV_SIP;
} IQDK_DNSInfoType;

typedef struct
{
   IQDK_HostEntryType RLSName;
   IPAddr RLSServerIP;
   INPort RLSPort;
   uint16 LastSIPRspCode;
   uint32 RegTTL; //in seconds
} IQDK_RLSInfoType;

typedef enum
{
   IQDK_NETWORK_READY,
   IQDK_NETWORK_NOT_READY
} IQDK_NetworkStatus;

typedef struct
{
   IQDK_NetworkStatus networkStatus;
} IQDK_NetworkStatusEventType;

typedef enum
{
   IQDK_QCHAT_COVERAGE_1X,
   IQDK_QCHAT_COVERAGE_DO,
   IQDK_QCHAT_COVERAGE_MAX
} IQDK_QchatCoverageType;

typedef struct
{
   IQDK_QchatCoverageType   QchatCoverage;
} IQDK_QchatSSInfoEventType;



typedef union
{
   IQDK_NetworkStatusEventType networkStatusEventType;
   IQDK_QchatSSInfoEventType qchatSSInfoEventType;
} IQDK_ServiceEventType;


/*=============================================================================
                    CLASS DEFINITIONS
=============================================================================*/

typedef struct IQDK30Service IQDK30Service;

#define INHERIT_IQDKService(iname)\
   INHERIT_IQueryInterface(iname);\
   IQDK_ErrorType (*Init)(iname *po, AEECLSID appClsID);\
   IQDK_ErrorType (*Online)(iname *pMe);\
   IQDK_ErrorType (*Offline)(iname *pMe);\
   IQDK_ErrorType (*Reset)(iname *pMe);\
   IQDK_ErrorType (*Suspend)(iname *pMe);\
   IQDK_ErrorType (*Resume)(iname *pMe)

#define INHERIT_IQDK30Service(iname)\
   INHERIT_IQDKService(iname);\
   IQDK_ErrorType (*GetCurrentDNSInfo)(iname *po, IQDK_DNSInfoType *pPrimary, IQDK_DNSInfoType *pSecondary, boolean *primaryUsed);\
   IQDK_ErrorType (*GetCurrentRLSInfo)(iname *po, IQDK_RLSInfoType *pRLSInfo);\
   IQDK_ErrorType (*ReleaseQChatNetworkResources)(iname *po);\
   IQDK_ErrorType (*GetLastQChatOnlineStatus) (iname *po, IQDK_OnlineFailureReason *status, IQDK_ServiceUnavailableReason *srvUnavail);\
   IQDK_ErrorType (*GetEventPayload) (iname *po, uint32 interfaceId,uint16 eventType,IQDK_ServiceEventType **payload)


AEEINTERFACE(IQDK30Service)
{
   INHERIT_IQDK30Service(IQDK30Service);
};

#define IQDK30SERVICE_Init(p,clsid)                        AEEGETPVTBL((p),IQDK30Service)->Init((p),(clsid))
#define IQDK30SERVICE_AddRef(p)                            AEEGETPVTBL((p),IQDK30Service)->AddRef((p))
#define IQDK30SERVICE_Release(p)                           AEEGETPVTBL((p),IQDK30Service)->Release((p))
#define IQDK30SERVICE_QueryInterface(p,i,p2)               AEEGETPVTBL((p),IQDK30Service)->QueryInterface((p),(i),(p2))
#define IQDK30SERVICE_Online(p)                            AEEGETPVTBL((p),IQDK30Service)->Online((p))
#define IQDK30SERVICE_Offline(p)                           AEEGETPVTBL((p),IQDK30Service)->Offline((p))
#define IQDK30SERVICE_Reset(p)                             AEEGETPVTBL((p),IQDK30Service)->Reset((p))
#define IQDK30SERVICE_Suspend(p)                           AEEGETPVTBL((p),IQDK30Service)->Suspend((p))
#define IQDK30SERVICE_Resume(p)                            AEEGETPVTBL((p),IQDK30Service)->Resume((p))
#define IQDK30SERVICE_GetCurrentDNSInfo(p, pri, sec, b)    AEEGETPVTBL(p,IQDK30Service)->GetCurrentDNSInfo(p, pri, sec, b)
#define IQDK30SERVICE_GetCurrentRLSInfo(p, info)           AEEGETPVTBL(p,IQDK30Service)->GetCurrentRLSInfo(p, info)
#define IQDK30SERVICE_ReleaseQChatNetworkResources(p)      AEEGETPVTBL(p,IQDK30Service)->ReleaseQChatNetworkResources(p)
#define IQDK30SERVICE_GetLastQChatOnlineStatus(p, s, un)   AEEGETPVTBL(p,IQDK30Service)->GetLastQChatOnlineStatus(p, s, un)
#define IQDK30SERVICE_GetEventPayload(p,i,a,b)             AEEGETPVTBL((p),IQDK30Service)->GetEventPayload(p,i,a,b)

#ifdef __cplusplus
}
#endif

#endif // AEEQDK30SERVICE_H

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================
================================================================================
  INTERFACES   DOCUMENTATION
=============================================================================== 

Interface Name: IQDK30Service

Description:

The IQDK30Service interface provides a set of API methods that provide access to
QChat Carrier related services. Typically this interface should only be used by an 
application that is going to implement the QDK application.

The interface includes methods for connecting/disconnecting from the QChat
service as well as methods to reset the provisioned data on QChat Client.

An OEM may wish to provide a hidden menu or some other local provisioning
interface in order to bring QChat Service offline and back online. Offline
mode can be used for device servicing or diagnostic testing that require
the device to cease all network, file system access, or sound path activity.

To access this interface, an application must have the corresponding 
privilege in its MIF file.

The following header file is required:~
AEEQDKService.h

================================================================================

IQDK30SERVICE_AddRef()

Description:
   This method increments the reference count of the IQDK30Service Interface 
   object. This allows the object to be shared by multiple callers. 
   The object is freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IQDK30SERVICE_AddRef(IQDK30Service *po)

Parameters:
   po : Pointer to the IQDK30Service Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Version:
   Introduced in QDK v 1.0

Side Effects:
   None

See Also:
   IQDK30SERVICE_Release()

================================================================================

IQDK30SERVICE_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQDK30SERVICE_Release(IQDK30Service *po)

Parameters:
   po: Pointer to the IQDK30Service Interface object

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
   IQDK30SERVICE_AddRef()

================================================================================

IQDK30SERVICE_Online()

Description:
   This function instructs the QChat Client to reconnect to the QChat
   Service.

Prototype:
   IQDK_ErrorType IQDK30SERVICE_Online(IQDK30Service *po)

Parameters:
   po :  Pointer to the IQDK30Service Interface object.

Return Value:
   Returns EQDKNOTINITIALIZED if IQDK30SERVICE_Init() has not been called

Comments:
   Calling IQDK30SERVICE_Online() will force the QChat Client to initiate
   registration with the Regional Location Server.

   Calling IQDK30SERVICE_Online() when the QChat Client is already online
   has no effect.

Version:
   Introduced in QDK v 1.0

See Also:
   IQDK30SERVICE_Offline()
 
================================================================================ 

IQDK30SERVICE_Offline()

Description:
   This function instructs the QChat Client to disconnect from the
   QChat Service.

Prototype:
   IQDK_ErrorType IQDK30SERVICE_Offline(IQDK30Service *po)

Parameters:
   po:  Pointer to the IQDK30Service Interface object.

Return Value:
   Returns EQDKNOTINITIALIZED if IQDK30SERVICE_Init() has not been called

Comments:
   The QChat Client will remove its registration from the Regional
   Location Server and will no longer be able to participate in QChat
   services until it is either restarted or brought back online via
   IQDK30SERVICE_Online().  While the device is offline the user will not be
   able to make or receive QChat calls.

   Calling IQDK30SERVICE_Offline() when the QChat Client is already
   offline has no effect.

Version:
   Introduced in QDK v 1.0

See Also:
   IQDK30SERVICE_Online()

================================================================================ 
IQDK30SERVICE_Init

Description:
   This function associates application class id with the given instance of
   IQDK30Service interface. Any application that wishes to use IQDK30Service
   interface will have to invoke this method after creating instance
   of IQDK30Service interface and before calling any other IQDK30Service
   interface methods.

Prototype:
   IQDK_ErrorType Init
   (
      IQDK30Service *pMe,
      AEECLSID classId
   )

Parameters:
   pMe      :  Pointer to the IQDK30Service Interface object.
   classId  :  Class id of application that wishes to use the IQDK30Service
               interface.

Return Value:
   On success, SUCCESS, otherwise EFAILED.

Comments:

Version:
   Introduced in QDK v 1.0

See Also:
   none

================================================================================

IQDK30SERVICE_Reset()

Description:
   This function resets the provisioned data on the client by 1) erasing the
   provisioned files; and 2) resetting the provisioned data in memory to 
   default values.  

Prototype:
   IQDK_ErrorType IQDK30SERVICE_Reset(IQDK30Service *po)

Parameters:
   po:  Pointer to the IQDK30Service Interface object.

Return Value:
   SUCCESS: reset successful
   EFAILED: reset failed

Comments:
   None

Version:
   Introduced in QDK v 1.0

==============================================================================

IQDK30SERVICE_Suspend()

Description:
   Suspends the qchat service, client stays registered with the server but will not accept incoming calls
   or make outgoing calls. The caller has to ensure that the client is not in a call.

Prototype:
    IQDK_ErrorType Suspend (IQDK30Service *po)

Parameters:
    po:  Pointer to the IQDK30Service Interface object.

Return Value: IQDK_ErrorType
                   EBADCLASS if pMe is NULL
                   EBADSTATE if client is in a non-idle state (such as in call, offline, in error etc.)
                   SUCCESS otherwise

Comments:
   None

Version:
   Introduced in QDK v 2.0

See also:
    IQDK30SERVICE_Resume()
    IQDKCOMMON_GetQChatStatus()

==============================================================================

IQDK30SERVICE_Resume()

Description:
   Resumes QChat service from suspended state.

Prototype:
   IQDK_ErrorType Suspend (IQDK30Service *po)

Parameters: 
    po:  Pointer to the IQDK30Service Interface object.

Return Value: IQDK_ErrorType
                   EBADCLASS if pMe is NULL
                   SUCCESS, otherwise

Comments:
   None

Version:
   Introduced in QDK v 2.0

See also:
    IQDK30SERVICE_Suspend()

===================================================================================

IQDK30SERVICE_GetCurrentDNSInfo()

Description:
   This function is used to get the primary and secondary DNS info

Prototype:
   IQDK_ErrorType IQDK30SERVICE_GetCurrentDNSInfo(IQDK30Service *po,
                                       IQDK_DNSInfoType *pPrimary,
                                       IQDK_DNSInfoType *pSecondary,
                                       boolean *primaryUsed)
Parameters:
   po     :      Pointer to the IQDK30Service Interface object
   pPrimary :    Returns primary DNS infomation
   pSecondary :  Returns secondary DNS information
   primaryUsed:  Returns a boolean value if primary dns is being used

Return Value:
   SUCCESS : Either primary or second DNS is the current used DNS
   EFAILED:  no DNS available.
Comments:
  None
See also:
  IQDK_DNSInfoType

===============================================================================
IQDK30SERVICE_GetCurrentRLSInfo()

Description:
   This function is used to get the RLS server info

Prototype:
   IQDK_ErrorType IQDKSERVICE_GetCurrentRLSInfo(IQDK30Service *po, IQDK_RLSInfoType *pRLSInfo)
Parameters:
   po     :      Pointer to the IQDK30Service Interface object
   pRLSInfo :    Returns RLS info

Return Value:
   SUCCESS 

Comments:
  None
See also:
  IQDK_RLSInfoType
================================================================================

IQDK30SERVICE_ReleaseQChatNetworkResources
Description:
   Release the IP Net work resources such as, sockets.

Prototype:
   IQDK_ErrorType IQDK30SERVICE_ReleaseQChatNetworkResources (IQDK30Service *po)
Parameters:
   po     :      Pointer to the IQDK30Service Interface object

Return Value:
   SUCCESS 

Comments:
  None
See also:
  IQDK30SERVICE_ReleaseQChatNetworkResources

===============================================================================
IQDK30SERVICE_GetLastQChatOnlineStatus
Description:
   This function is used to get the last qchat online status

Prototype:
   IQDK_ErrorType IQD30KSERVICE_GetLastQChatOnlineStatus (iname *po, IQDK_OnlineFailureReason *status, IQDK_ServiceUnavailableReason *srvUnavail)
Parameters:
   po     :      Pointer to the IQDK30Service Interface object
   IQDK_OnlineFailureReason :    Online failure reason
   IQDK_ServiceUnavailableReason : Service unavailable reason

Return Value:
   SUCCESS 

Comments:
  None
See also:
  IQDK_ServiceUnavailableReason
===========================================================================================

IQDK30SERVICE_GetEventPayload

Description:
   The event payload is not sent as part of the event.
   The application can get the event payload through the GetEventPayload API. 

   This interface returns the payload of any of the events requested in the eventType.
   The payload returned is for any one of the following events:
   
  typedef union
  {
     IQDK_NetworkStatusEventType networkStatusEventType;
  }  IQDK_ServiceEventType;

   The GetEventPayload allows QDK applications to request the event information
   explicitly from the QChat Client, for a specific event type and for a specific
   version of that event.
   
Prototype:
  IQDK_ErrorType GetEventPayload(
     IQDK30Service *pMe, 
     uint32 interfaceId, 
     uint16 eventType, 
     IQDK_ServiceEventType **payload);
  

Parameters:

   pMe :		    Pointer to the IQDK30Service Interface object.
   interfaceId:	    ID number of the Service interface
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
   Introduced in QDK v 3.0

Side Effects:
   None

See Also:
   IQDK_NetworkStatusEventType
      IQDK_QchatSSInfoEventType
   
================================================================================
IQDK_NetworkStatusEventType

Description:
   This event is sent by the Client to indicate whether the Network is ready or not ready.
If network is ready, the CLient will be in a state to start/receive a QChat call. If network
is not ready, the Client will not be in state to support QChat calls. 

Definition:

typedef enum
{
   IQDK_NETWORK_READY,
   IQDK_NETWORK_NOT_READY
} IQDK_NetworkStatus;

typedef struct
{
   IQDK_NetworkStatus networkStatus;
} IQDK_NetworkStatusEventType;
Members:
   IQDK_NetworkStatus

================================================================================
IQDK_NetworkStatus

Description:
   Data structure that has the enums for network ready/not ready

Definition:

typedef enum
{
   IQDK_NETWORK_READY,
   IQDK_NETWORK_NOT_READY
} IQDK_NetworkStatus;

Members:
   IQDK_NETWORK_READY  - Client is network ready
   IQDK_NETWORK_NOT_READY  - Client is not network ready
================================================================================
IQDK_QchatSSInfoEventType

Description:
   This event is received when the serving system changes 

Definition:

typedef struct
{
   IQDK_QchatCoverageType   QchatCoverage;
} IQDK_QchatSSInfoEventType;

Members:
   IQDK_QChatCoverageType

================================================================================
IQDK_QchatCoverageType

Description:
   This data structure contains the enumerations for different serving systems

Definition:

typedef enum
{
   IQDK_QCHAT_COVERAGE_1X,
   IQDK_QCHAT_COVERAGE_DO,
   IQDK_QCHAT_COVERAGE_MAX
}IQDK_QchatCoverageType;

Members:
   IQDK_QCHAT_COVERAGE_1X - Serving system is 1X
   IQDK_QCHAT_COVERAGE_DO - Serving system is DO RevA
   IQDK_QCHAT_COVERAGE_MAX - Placeholder for Max

================================================================================*/
