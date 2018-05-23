#ifndef AEEIQDKPTTFORWARDER_H
#define AEEIQDKPTTFORWARDER_H

//===========================================================================
// FILE: AEEIQDKPTTForwarder.h
// 
// SERVICES: QDK Client PTT forwarder Interface
// 
// DESCRIPTION:
//    IQDKPTTForwarder is the published notifier interface to the QChat 
//    Client Service.
//
// GENERAL DESCRIPTION:
//
// PUBLIC CLASSES AND STATIC FUNCTIONS:
//    IQDKPTTForwarder_Init
//    IQDKPTTForwarder_IsPTTAvailable
//    IQDKPTTForwarder_RegisterForPTT
//
// Copyright © 2006 QUALCOMM Incorporated.
// All Rights Reserved.
// QUALCOMM Proprietary
//===========================================================================


//===========================================================================
//   INCLUDES AND PUBLIC DATA DECLARATIONS
//===========================================================================

//---------------------------------------------------------------------------
//   Defines
//---------------------------------------------------------------------------
#define IQDK_EVT_PTT_EVENT    (0 + IQDK_EVT_PTTFWD_PRIVATE_BASE)

//---------------------------------------------------------------------------
//   Include Files
//---------------------------------------------------------------------------
#include "AEE.h"
#include "AEEQDKDataTypes.h"
#include "AEEIQDKPTTForwarder.bid"		//Backward_Comp_START_Apr_19

/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
#define AEEIID_IQDKPTTFORWARDER 0x01052f35 //Interface ID for 2.0 QueryInterface

typedef struct IQDKPTTForwarder IQDKPTTForwarder;

/*-------------------------------------------------------------------
      Function Declarations
-------------------------------------------------------------------*/

/*-----------------------------------------------------------------
       Interface functions
-------------------------------------------------------------------*/
typedef enum
{
   IQDKPTTEVENT_PTT_RELEASE,
   IQDKPTTEVENT_PTT_PRESS,
   IQDKPTTEVENT_PTT_MAX
} IQDKPTTEvent;

AEEINTERFACE(IQDKPTTForwarder)
{
   INHERIT_IQueryInterface(IQDKPTTForwarder);
   IQDK_ErrorType (*Init)(IQDKPTTForwarder *po, AEECLSID classId);
   IQDK_ErrorType (*IsPTTAvailable)(const IQDKPTTForwarder *pMe, boolean *bReturnStatus);
   IQDK_ErrorType (*RegisterForPTT)(IQDKPTTForwarder *pMe, boolean *bReturnStatus);
   IQDK_ErrorType (*DeregisterForPTT)(IQDKPTTForwarder *pMe);
};


#define IQDKPTTFORWARDER_AddRef(p)  AEEGETPVTBL((p),IQDKPTTForwarder)->AddRef((p))  
#define IQDKPTTFORWARDER_Release(p) AEEGETPVTBL((p),IQDKPTTForwarder)->Release((p))
#define IQDKPTTFORWARDER_Init(p, c) AEEGETPVTBL((p),IQDKPTTForwarder)->Init((p), (c))
#define IQDKPTTFORWARDER_IsPTTAvailable(p, r) AEEGETPVTBL((p),IQDKPTTForwarder)->IsPTTAvailable((p), (r))
#define IQDKPTTFORWARDER_RegisterForPTT(p, r) AEEGETPVTBL((p),IQDKPTTForwarder)->RegisterForPTT((p), (r))
#define IQDKPTTFORWARDER_DeregisterForPTT(p) AEEGETPVTBL((p),IQDKPTTForwarder)->DeregisterForPTT((p))

/*===================================================================
  DATA STRUCTURE DOCUMENTATION
===================================================================== 


=====================================================================
  INTERFACES   DOCUMENTATION
================================================================================



==============================================================================*/
#endif // AEEIQDKPTTFORWARDER_H

