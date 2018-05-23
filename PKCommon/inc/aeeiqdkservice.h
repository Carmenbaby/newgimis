#ifndef AEEQDKSERVICE_H
#define AEEQDKSERVICE_H
/*=============================================================================
 
FILE: AEEIQDKService.h

SERVICES: The IQDKService interface provides functions to manage QChat service 
          connectivity.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IQDKService
   IQDKSERVICE_AddRef
   IQDKSERVICE_Release
   IQDKSERVICE_Online
   IQDKSERVICE_Offline
   IQDKSERVICE_Reset

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================*/
#include "AEE.h"
#include "AEEQDKDataTypes.h"
#include "AEEIQDKService.bid"	//Backward_Comp_START_Apr_19

#ifdef __cplusplus
extern "C"
{
#endif

#define AEEIID_IQDKSERVICE 0x01052f36 //Interface ID for 2.0 QueryInterface

/*=============================================================================
                    CLASS DEFINITIONS
=============================================================================*/

typedef struct IQDKService IQDKService;

AEEINTERFACE(IQDKService)
{
   INHERIT_IQueryInterface(IQDKService);
   IQDK_ErrorType (*Init)(IQDKService *po, AEECLSID appClsID);
   IQDK_ErrorType (*Online)(IQDKService *pMe);
   IQDK_ErrorType (*Offline)(IQDKService *pMe);
   IQDK_ErrorType (*Reset)(IQDKService *pMe);
   IQDK_ErrorType (*Suspend)(IQDKService *pMe);
   IQDK_ErrorType (*Resume)(IQDKService *pMe);
};

#define IQDKSERVICE_Init(p,clsid)  AEEGETPVTBL((p),IQDKService)->Init((p),(clsid))
#define IQDKSERVICE_AddRef(p)      AEEGETPVTBL((p),IQDKService)->AddRef((p))
#define IQDKSERVICE_Release(p)     AEEGETPVTBL((p),IQDKService)->Release((p))
#define IQDKSERVICE_QueryInterface(p,i,p2)   AEEGETPVTBL((p),IQDKService)->QueryInterface((p),(i),(p2))
#define IQDKSERVICE_Online(p)      AEEGETPVTBL((p),IQDKService)->Online((p))
#define IQDKSERVICE_Offline(p)     AEEGETPVTBL((p),IQDKService)->Offline((p))
#define IQDKSERVICE_Reset(p)       AEEGETPVTBL((p),IQDKService)->Reset((p))
#define IQDKSERVICE_Suspend(p)   AEEGETPVTBL((p),IQDKService)->Suspend((p))
#define IQDKSERVICE_Resume(p)   AEEGETPVTBL((p),IQDKService)->Resume((p))

#ifdef __cplusplus
}
#endif

#endif // AEEQDKSERVICE_H

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================

================================================================================
  INTERFACES   DOCUMENTATION
================================================================================

Interface Name: IQDKService

Description:

The IQDKService interface provides a set of API methods that provide access to
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

IQDKSERVICE_AddRef()

Description:
   This method increments the reference count of the IQDKService Interface 
   object. This allows the object to be shared by multiple callers. 
   The object is freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKSERVICE_AddRef(IQDKService *po)

Parameters:
   po : Pointer to the IQDKService Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Version:
   Introduced in QDK v 1.0

Side Effects:
   None

See Also:
   IQDKSERVICE_Release()

================================================================================

IQDKSERVICE_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKSERVICE_Release(IQDKService *po)

Parameters:
   po: Pointer to the IQDKService Interface object

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
   IQDKSERVICE_AddRef()

================================================================================

IQDKSERVICE_Online()

Description:
   This function instructs the QChat Client to reconnect to the QChat
   Service.

Prototype:
   IQDK_ErrorType IQDKSERVICE_Online(IQDKService *pIQDKService)

Parameters:
   pIQDKService :  Pointer to the IQDKService Interface object.

Return Value:
   Returns EQDKNOTINITIALIZED if IQDKSERVICE_Init() has not been called

Comments:
   Calling IQDKSERVICE_Online() will force the QChat Client to initiate
   registration with the Regional Location Server.

   Calling IQDKSERVICE_Online() when the QChat Client is already online
   has no effect.

Version:
   Introduced in QDK v 1.0

See Also:
   IQDKSERVICE_Offline()
 
================================================================================ 

IQDKSERVICE_Offline()

Description:
   This function instructs the QChat Client to disconnect from the
   QChat Service.

Prototype:
   IQDK_ErrorType IQDKSERVICE_Offline(IQDKService *pIQDKService)

Parameters:
   pIQDKService:  Pointer to the IQDKService Interface object.

Return Value:
   Returns EQDKNOTINITIALIZED if IQDKSERVICE_Init() has not been called

Comments:
   The QChat Client will remove its registration from the Regional
   Location Server and will no longer be able to participate in QChat
   services until it is either restarted or brought back online via
   IQDKSERVICE_Online().  While the device is offline the user will not be
   able to make or receive QChat calls.

   Calling IQDKSERVICE_Offline() when the QChat Client is already
   offline has no effect.

Version:
   Introduced in QDK v 1.0

See Also:
   IQDKSERVICE_Online()

================================================================================ 
IQDKSERVICE_Init

Description:
   This function associates application class id with the given instance of
   IQDKService interface. Any application that wishes to use IQDKService
   interface will have to invoke this method after creating instance
   of IQDKService interface and before calling any other IQDKService
   interface methods.

Prototype:
   IQDK_ErrorType Init
   (
      IQDKService *pMe,
      AEECLSID classId
   )

Parameters:
   pMe      :  Pointer to the IQDKService Interface object.
   classId  :  Class id of application that wishes to use the IQDKService
               interface.

Return Value:
   On success, SUCCESS, otherwise EFAILED.

Comments:

Version:
   Introduced in QDK v 1.0

See Also:
   none

================================================================================

IQDKSERVICE_Reset()

Description:
   This function resets the provisioned data on the client by 1) erasing the
   provisioned files; and 2) resetting the provisioned data in memory to 
   default values.  

Prototype:
   IQDK_ErrorType IQDKSERVICE_Reset(IQDKService *pIQDKService)

Parameters:
   pIQDKService:  Pointer to the IQDKService Interface object.

Return Value:
   SUCCESS: reset successful
   EFAILED: reset failed

Comments:
   None

Version:
   Introduced in QDK v 1.0

==============================================================================

IQDKSERVICE_Suspend()

Description:
   Suspends the qchat service, client stays registered with the server but will not accept incoming calls
   or make outgoing calls. The caller has to ensure that the client is not in a call.

Prototype:
    IQDK_ErrorType Suspend (IQDKService *pMe)

Parameters:IQDKService *pMe

Return Value: IQDK_ErrorType
                   EBADCLASS if pMe is NULL
                   EBADSTATE if client is in a non-idle state (such as in call, offline, in error etc.)
                   SUCCESS otherwise

Comments:
   None

Version:
   Introduced in QDK v 2.0

See also:
    IQDKSERVICE_Resume()
    IQDKCOMMON_GetQChatStatus()

==============================================================================

IQDKSERVICE_Resume()

Description:
   Resumes QChat service from suspended state.

Prototype:
   IQDK_ErrorType Suspend (IQDKService *pMe)

Parameters: IQDKService *pMe

Return Value: IQDK_ErrorType
                   EBADCLASS if pMe is NULL
                   SUCCESS, otherwise

Comments:
   None

Version:
   Introduced in QDK v 2.0

See also:
    IQDKSERVICE_Suspend()
==============================================================================*/

