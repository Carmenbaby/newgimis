#ifndef AEEIQDK30DATATYPES_H
#define AEEIQDK30DATATYPES_H

/*=============================================================================

FILE: AEEQDK30DataTypes.h

SERVICES: 

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:

        Copyright Ac 2005 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================*/
#include "AEEQDKAddress.h"
#include "AEEQDKDataTypes.h"

typedef enum
{
   IQDK_ONLINE_SUCCESS,
   IQDK_ONLINE_FAILURE_NO_RESPONSE_FROM_SERVER,
   IQDK_ONLINE_FAILURE_SERVING_SYSTEM_UNAVAILABLE,
   IQDK_ONLINE_FAILURE_DATA_NETWORK_UNAVAILABLE,
   IQDK_ONLINE_FAILURE_RLS_ERROR,
   IQDK_ONLINE_FAILURE_SERVICE_UNAVAILABLE,
   IQDK_ONLINE_FAILURE_DNS_FAILURE,
   IQDK_ONLINE_SERVICE_NOT_WANTED
} IQDK_OnlineFailureReason;

typedef enum
{
   IQDK_SERVICE_UNAVAILABLE_NONE,
   IQDK_SERVICE_UNAVAILABLE_UNKNOWN_USER,
   IQDK_SERVICE_UNAVAILABLE_NETWORK_DELAY,
   IQDK_SERVICE_UNAVAILABLE_VER_NOT_SUPPORTED,
   IQDK_SERVICE_UNAVAILABLE_NON_PAB_COMPLIANT_SERVER
} IQDK_ServiceUnavailableReason;

typedef struct
{
   IQDK_QChatStateEnum     state;
   IQDK_QChatUserPrefs     userPrefs;
   boolean                 upgradeAvailable;
   boolean                 IsUsingOlderQChatService;
   IQDK_OnlineFailureReason        onlineFailureReason;
   IQDK_ServiceUnavailableReason   serviceUnavailableReason; 
} IQDK30_QChatStatusEventType;

typedef struct
{
   IQDKAddressData                  token;
   IQDK_CallRestrictionLevel        level;
   IQDK_CallRestrictionType         type;
   IQDK_CallRestrictionDirection    direction;
   IQDK_ServicesType                services;
   boolean                          asteriskIsLiteral;
} IQDK30_CallRestrictionEntry;

/*  The following events are defined in qdk 2.0
qdk3.0 additional are defined below */

/********************** qdk 2.0 events *******************
// The following events will be received by the QDK app only if
// the NMASK_QDK_MANAGER mask is specified in IQDKMANAGER_Init()
#define IQDK_EVT_COMMON_BASE                     (EVT_USER)
#define IQDK_EVT_QCHAT_STATUS                    (0x0 + IQDK_EVT_COMMON_BASE)
#define IQDK_EVT_PROV_ITEMS_CHANGED_RESTART      (0x1 + IQDK_EVT_COMMON_BASE)
#define IQDK_EVT_USER_ACTION                     (0x2 + IQDK_EVT_COMMON_BASE)
#define IQDK_EVT_QCHAT_TERMINATE                 (0x3 + IQDK_EVT_COMMON_BASE)
*********************/

/********************** qdk 3.0 events *******************/
#define IQDK_EVT_QCHAT_INITIALIZATION_COMPLETE (0x4 + IQDK_EVT_COMMON_BASE)

#endif /* AEEIQDK30DATATYPES_H */



/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

IQDK_ServiceUnavailableReason

Description:
    This enum specifies the various QChat service unavailable reasons that are sent with the
        IQDK_QChatStatusEventType data structure.

Definition:
typedef enum
{

   IQDK_SERVICE_UNAVAILABLE_NONE,                      //default service unavailable reason
   IQDK_SERVICE_UNAVAILABLE_UNKNOWN_USER,              //service unavailable info unknown user.
   IQDK_SERVICE_UNAVAILABLE_NETWORK_DELAY,             //service unavailable info network delay.
   IQDK_SERVICE_UNAVAILABLE_VER_NOT_SUPPORTED,         //service unavailable info server version not supported.
   IQDK_SERVICE_UNAVAILABLE_NON_PAB_COMPLIANT_SERVER   //service unavailable info non pab compaliant server.
} IQDK_ServiceUnavailableReason;

Comments:
   Only when OnlineFailureReason is IQDK_ONLINE_FAILURE_SERVICE_UNAVAILABLE, this parameter has a meaningful value.
Members:
        None

Version:
   Introduced in QDK v 3.0

See Also:
  IQDK30_QChatStatusEventType

=============================================================================

IQDK_OnlineFailureReason

Description:
   This enum specifies the various reasons of registration failure.

Definition:
typedef enum
{
   IQDK_ONLINE_SUCCESS,                               //registration success
   IQDK_ONLINE_FAILURE_NO_RESPONSE_FROM_SERVER,       //registration timeout due to no response from server
   IQDK_ONLINE_FAILURE_SERVING_SYSTEM_UNAVAILABLE,    //no digitial service
   IQDK_ONLINE_FAILURE_DATA_NETWORK_UNAVAILABLE,      //failure due to PPP
   IQDK_ONLINE_FAILURE_RLS_ERROR,                     //failure due to SIP
   IQDK_ONLINE_FAILURE_SERVICE_UNAVAILABLE,           //received error from QChat server, error specified in IQDK_ServiceUnavailableReason
   IQDK_ONLINE_FAILURE_DNS_FAILURE,                   //failure due to dns failures.
   IQDK_ONLINE_SERVICE_NOT_WANTED                     //user called IQDKService_Offline before calling IQDKENHANCEDSTATUS_GetLastQChatOnlineStatus()
} IQDK_OnlineFailureReason;

Comments:
        None

Members:
        None

Version:
   Introduced in QDK v 3.0

See Also:
   IQDK30_QChatStatusEventType
=============================================================================

IQDK30_QChatStatusEventType

Description:
   This data structure contains information about the current
   operational status of the QChat Client, including details of
   user preferences, upgrades, version, online failure reason, and 
   service unavailable reason of the QChat client.

Definition:
typedef struct
{
   IQDK_QChatStateEnum     state;
   IQDK_QChatUserPrefs     userPrefs;
   boolean                 upgradeAvailable;
   boolean                 IsUsingOlderQChatService;
   IQDK_OnlineFailureReason        onlineFailureReason;
   IQDK_ServiceUnavailableReason   serviceUnavailableReason; 
} IQDK30_QChatStatusEventType;

Members:
   state: This field indicates the current operational state of the client.

   userPrefs: This field holds a struct containing user defined preferences that
		include information on call termination, preferred address index, sound mode, 
		and group call member details event enabling.

   upgradeAvailable: Boolean flag to indicate if an upgrade is available or not.

   IsUsingOlderQChatService: Boolean flag to indicate if the client version
		is out of date.

  onlineFailureReason: Specifies the reason of registration failure

  serviceUnavailableReason: Specifies the reason of QChat service being unavailable
   
Version:
   Introduced in QDK v 3.0

See Also:
	IQDK_QChatStateEnum
	IQDK_QChatUserPrefs

===================================================================== 

IQDK_EVT_QCHAT_INITIALIZATION_COMPLETE

Description:
   This event is sent by the QChat Client when QChat initialization is complete. 
   Upon receiving this event, the QDK application can proceed with resume operation 
   such as creating QDK interfaces. This event is especially useful for QDK 
   background applications, since they do not receive the EVT_APP_RESUME event from 
   BREW. Foreground QDK applications do receive the EVT_APP_RESUME event from BREW, 
   however, we recommend that all QDK applications exclusively use the 
   IQDK_EVT_QCHAT_INITIALIZATION_COMPLETE event as an indication that QChat 
   initialization has completed and to proceed with the creation of QDK Interfaces.

   
Parameters:
   None

Comments:
   This event is delivered via AEEQDKNotifier
   (notify-mask |= NMASK_QDK_MANAGER) interface.

Version:
   Introduced in QDK v 3.0

See Also:
   IQDKCommon_Init()

=======================================================================*/

