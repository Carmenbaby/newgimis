#ifndef AEEIQDK30NOTIFIER_H
#define AEEIQDK30NOTIFIER_H

//===========================================================================
// FILE: AEEIQDK30Notifier.h
// 
// SERVICES: QDK Client Service Notifier Interface
// 
// DESCRIPTION:
//    IQDK30Notifier is the published notifier interface to the QChat 
//    Client Service.
//
// PUBLIC FUNCTIONS:
//    IQDK30NOTIFIER_AddRef  - Inc ref count
//    IQDK30NOTIFIER_Release - Dec ref count, release on zero
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
#include "AEEIQDKNotifier.h"

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/

/*  Notification Masks...
qdk2.0  Notification Masks are defined in AEEIQDKNotifier.h. 
qdk3.0 additional Notifications masks are defined below 

#define NMASK_QDK_MANAGER             0x00000001
#define NMASK_QDK_CALL                0x00000002
#define NMASK_QDK_ALERT               0x00000004
#define NMASK_QDK_MEMBERSHIP          0x00000008
#define NMASK_QDK_PROVISION           0x00000010
#define NMASK_QDK_PRESENCE_ON_DEMAND  0x00000020
#define NMASK_QDK_AVAILABILITY_NOTIFICATION  0x00000040 */

#define NMASK_QDK_ENHANCED_STATUS     0x00000080

/*===================================================================
  DATA STRUCTURE DOCUMENTATION
=====================================================================*/

#endif // AEEIQDK30NOTIFIER_H
