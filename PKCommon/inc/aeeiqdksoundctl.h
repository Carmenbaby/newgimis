#ifndef AEEQDKSOUNDCTL_H
#define AEEQDKSOUNDCTL_H
/*=============================================================================
 
FILE: AEEQDKSoundCtl.h

SERVICES: The IQDKSoundCtl interface manages all the methods related to 
controlling the media and sound features of QChat.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IQDKSoundCtl
   IQDKSOUNDCTL_AddRef
   IQDKSOUNDCTL_Release
   IQDKSOUNDCTL_QueryInterface
   IQDKSOUNDCTL_SetSoundMask
   IQDKSOUNDCTL_GetSoundMask
   IQDKSOUNDCTL_ClearSoundMask
   IQDKSOUNDCTL_PauseMedia
   IQDKSOUNDCTL_ResumeMedia

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================*/
#include "AEE.h"
#include "AEEQDKDataTypes.h"
#include "AEEIQDKSoundCtl.bid"


/*=============================================================================
                    CLASS DEFINITIONS
=============================================================================*/
#define AEEIID_IQDKSOUNDCTL 0x01052f37 //Interface ID for 2.0 QueryInterface

typedef struct IQDKSoundCtl IQDKSoundCtl;


AEEINTERFACE(IQDKSoundCtl)
{
   INHERIT_IQueryInterface(IQDKSoundCtl);
   IQDK_ErrorType (*Init)(IQDKSoundCtl *po, AEECLSID appClsID);
   IQDK_ErrorType (*ClearSoundMask)(IQDKSoundCtl *pMe, IQDK_ToneMask toneMask);
   IQDK_ErrorType (*GetSoundMask)(IQDKSoundCtl *pMe, IQDK_ToneMask toneMask, IQDK_ToneMask *tone);
   IQDK_ErrorType (*SetSoundMask)(IQDKSoundCtl *pMe, IQDK_ToneMask toneMask);
   IQDK_ErrorType (*PauseMedia)(IQDKSoundCtl *pMe);
   IQDK_ErrorType (*ResumeMedia)(IQDKSoundCtl *pMe);
   IQDK_ErrorType (*GetMode)(IQDKSoundCtl *pMe, IQDK_SoundMode * mode);
   IQDK_ErrorType (*SetMode)(IQDKSoundCtl *pMe, IQDK_SoundMode mode);
};

#define IQDKSOUNDCTL_Init(p,clsid)                   AEEGETPVTBL((p),IQDKSoundCtl)->Init((p),(clsid))
#define IQDKSOUNDCTL_AddRef(p)                       AEEGETPVTBL((p),IQDKSoundCtl)->AddRef((p))
#define IQDKSOUNDCTL_Release(p)                      AEEGETPVTBL((p),IQDKSoundCtl)->Release((p))
#define IQDKSOUNDCTL_QueryInterface(p,clsid,pp)      AEEGETPVTBL((p),IQDKSoundCtl)->QueryInterface((p),(clsid),(pp))
#define IQDKSOUNDCTL_SetSoundMask(p,t)               AEEGETPVTBL((p),IQDKSoundCtl)->SetSoundMask((p),(t))
#define IQDKSOUNDCTL_GetSoundMask(p,t,m)             AEEGETPVTBL((p),IQDKSoundCtl)->GetSoundMask((p),(t),(m))
#define IQDKSOUNDCTL_ClearSoundMask(p,t)             AEEGETPVTBL((p),IQDKSoundCtl)->ClearSoundMask((p),(t))
#define IQDKSOUNDCTL_PauseMedia(p)                   AEEGETPVTBL((p),IQDKSoundCtl)->PauseMedia((p))
#define IQDKSOUNDCTL_ResumeMedia(p)                  AEEGETPVTBL((p),IQDKSoundCtl)->ResumeMedia((p))
#define IQDKSOUNDCTL_SetMode(p,m)                    AEEGETPVTBL((p),IQDKSoundCtl)->SetMode((p),(m))
#define IQDKSOUNDCTL_GetMode(p,m)                    AEEGETPVTBL((p),IQDKSoundCtl)->GetMode((p),(m))

#endif // AEEQDKSOUNDCTL_H

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================
IQDK_ToneMask

Description:
   The data type defines the tones that are supported by the QChat Client.

Definition:
   typedef uint16 IQDK_ToneMask;
   It is a 16-bit mask value wherein each of the following bit settings can be used to set one or more corresponding
   tones.

   IQDK_TONES_ALERT_ARRIVAL
   IQDK_TONES_CALL_ARRIVAL
   IQDK_TONES_CALL_FAIL
   IQDK_TONES_ALERT_FAIL
   IQDK_TONES_FLOOR_DENY
   IQDK_TONES_FLOOR_GRANT
   IQDK_TONES_FLOOR_INTERRUPTION
   IQDK_TONES_ADD_MEMBER
   IQDK_TONES_CALL_SUCCESS
   IQDK_TONES_ALERT_SUCCESS
   IQDK_TONES_TALK_ANNOUNCE
   IQDK_TONES_FLOOR_AVAILABLE
   IQDK_TONES_LAST

Members:
   None

See Also:
   None

=======================================================================

=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IQDKSoundCtl

Description:
   IQDKSoundCtl interface is a set of API methods that are used to pause
   and resume media, and to configure the tones that the QDK application
   would like the QChat Client to play.

   The IQDKSoundCtl interface instance is obtained via ISHELL_CreateInstance()
   mechanism.

   The following header file is required:
   AEEQDKSoundCtl.h

=============================================================================

IQDKSOUNDCTL_AddRef()

Description:
   This method increments the reference count of the IQDKSoundCtl Interface object.
   This allows the object to be shared by multiple callers. The object is freed when
   the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKSOUNDCTL_AddRef(IQDKSoundCtl * po)

Parameters:
   po : Pointer to the IQDKSoundCtl Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

See Also:
   IQDKSOUNDCTL_Release()

=============================================================================

IQDKSOUNDCTL_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKSOUNDCTL_Release(IQDKSoundCtl * po)

Parameters:
   po : Pointer to the IQDKSoundCtl Interface object

Return Value:
   Decremented reference count for the object. The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

See Also:
   IQDKSOUNDCTL_AddRef()

==============================================================================

IQDKSOUNDCTL_QueryInterface()

Description:
   This method retrieves a pointer to an interface conforming to the
   definition of the specified class ID. This can be used to query for
   extended functionality, like future versions or proprietary extensions.

   Upon a successful query, the interace is returned AddRef'ed.  The caller is
   responsible for calling Release() at some point in the future.  One
   exception is when the pointer returned is not an interface pointer.  In
   that case, the memory will share the lifetime of the object being queried,
   and the returned pointer will not be used to free or release the object.

Prototype:
   int IQDKSOUNDCTL_QueryInterface(IQDKSoundCtl *po, AEECLSID id, void **p)

Parameters:
   po : Pointer to IQDKSoundCtl interface
   id : A globally unique id to identify the entity (interface or data) that we
        are trying to query.
    p : Pointer to the data or interface that we want to retrieve. If the interface
        is not available, this is set to NULL.

Return Value:
   SUCCESS: on success
   ECLASSNOTSUPPORT: if class ID not supported

Comments:
   On failure, QueryInterface() must set *p to NULL.

Version:
   Introduced in QDK v 2.0
   
Side Effects:
   None

See Also:
   None

=============================================================================

IQDKSOUNDCTL_PauseMedia()

Description:
   This function instructs the QChat Client to ignore incoming media.
   For example, while the user is participating in an active QChat call
   the QDK application could call IQDKSOUNDCTL_PauseMedia() to discard QChat 
   voice packets while the user accepts a traditional voice call, without ever
   leaving the QChat call. The QDK application could later call
   IQDKSOUNDCTL_ResumeMedia() to resume decoding and playback of media packets.

Prototype:
   IQDK_ErrorType IQDKSOUNDCTL_PauseMedia(IQDKSoundCtl *pIQDKSoundCtl)

Parameters:
   pIQDKSoundCtl: Pointer to the IQDKSoundCtl Interface object.
   
Return Value:
   SUCCESS: The QChat Client was able to successfully pause the media stream.
   EFAILED : Pointer to IQDKSoundCtl Interface object is NULL.
   EQDKNOTINITIALIZED: This status is returned when IQDKSOUNDCTL_Init() has not been 
                       called by the QDK application, which causes IQDKSoundCtl data  
                       to remain uninitialized.
  
Comments:
   IQDKSOUNDCTL_PauseMedia() affects only the media and not the accompanying
   media signaling packets. While the media stream is paused, the QChat Client 
   will continue to process floor changes and other signaling that manages the 
   media stream.

Version:
   Introduced in QDK v 2.0

Side Effects:
   None
   
See Also:
   IQDKSOUNDCTL_ResumeMedia()
   IQDK_ErrorType

================================================================================

IQDKSOUNDCTL_ResumeMedia()

Description:
   This function instructs the QChat Client to resume processing the
   incoming media stream. For example, while the user is participating
   in an active QChat call, the QDK application could call IQDKSOUNDCTL_PauseMedia()
   to discard QChat voice packets while the user accepts a traditional
   voice call, without ever leaving the QChat call. The QDK application
   could later call IQDKSOUNDCTL_ResumeMedia() to resume decoding and playback
   of media packets.

Prototype:
   IQDK_ErrorType IQDKSOUNDCTL_ResumeMedia(IQDKSoundCtl *pIQDKSoundCtl)

Parameters:
   pIQDKSoundCtl:  Pointer to the IQDKSoundCtl Interface object.

Return Value:
   SUCCESS: The QChat Client is able to successfully resume processing of the 
            media stream.
   EFAILED : Pointer to IQDKSoundCtl Interface object is NULL.
   EQDKNOTINITIALIZED: This status is returned when IQDKSOUNDCTL_Init() has not been 
                       called by the QDK application, which causes IQDKSoundCtl data  
                       to remain uninitialized.

Comments:
   None

Version:
   Introduced in QDK v 2.0

See Also:
   IQDKSOUNDCTL_PauseMedia()
   IQDK_ErrorType

================================================================================

IQDKSOUNDCTL_SetSoundMask

Description:
   This function sets the sound mask according to the tone-type passed in. The 
   Tone Mask refers to a bitmask of the various QChat tones. It permits the QDK
   application to customize which QChat tones will be played. An example of such
   a tone is the PTT Floor Grant Tone, which is played when the user has been 
   granted the floor. A QDK application can turn this on by setting the 
   corresponding bit in the sound mask, or turn it off by clearing the bit in the 
   sound mask.
   
Prototype:
   IQDK_ErrorType IQDKSOUNDCTL_SetSoundMask
   (
      IQDKSoundCtl *pIQDKSoundCtl,
      IQDK_ToneMask toneMask
   )
  
Parameters:
   pIQDKSoundCtl:  Pointer to the IQDKSoundCtl Interface object.
   toneMask:  The type of the tone that needs to be turned on.
 
Return Value:
   SUCCESS: The QChat Client is able to successfully set the tone mask.
   EQDKNOTINITIALIZED: This status is returned when IQDKSOUNDCTL_Init() has not been 
                       called by the QDK application, which causes IQDKSoundCtl data  
                       to remain uninitialized.

Comments:
   None

Version:
   Introduced in QDK v 2.0

See Also:
   IQDKSOUNDCTL_GetSoundMask
   IQDKSOUNDCTL_ClearSoundMask
   IQDK_ToneMask

================================================================================

IQDKSOUNDCTL_GetSoundMask

Description:
   This function gets the sound mask according to the tone-type passed in. 
   The Sound Mask refers to a bitmask of the various QChat tones. It permits 
   the QDK application to customize which QChat tones will be played. 
   An example of such a tone is the PTT Floor Grant Tone, which is played when 
   the user has been granted the floor.  A QDK application can turn this on by 
   setting the corresponding bit in the sound mask, or turn it off by clearing 
   the bit in the sound mask.
               
Prototype:
   IQDK_ErrorType IQDKSOUNDCTL_GetSoundMask
   (
      IQDKSoundCtl *pIQDKSoundCtl,
      IQDK_ToneMask toneMask,
      IQDK_ToneMask *tone
   )
              
Parameters:
   pIQDKSoundCtl  :  Pointer to the IQDKSoundCtl Interface object.
   toneMask    :  The type of the tone for which the tone needs to be retrieved. 
   tone        :  Pointer to the tone mask setting of the toneMask passed in.
                  returns IQDK_TONES_LAST, if IQDKSOUNDCTL failed to get the tone mask for the
                  toneMask passed in.
             
Return Value:
   SUCCESS: The QChat Client is able to successfully set the tone mask.
   EQDKNOTINITIALIZED: This status is returned when IQDKSOUNDCTL_Init() has not been 
                       called by the QDK application, which causes IQDKSoundCtl data  
                       to remain uninitialized.
        
Comments:
   None
       
Version:
   Introduced in QDK v 2.0

See Also:
   IQDKSOUNDCTL_SetSoundMask
   IQDKSOUNDCTL_ClearSoundMask
   IQDK_ToneMask
      
================================================================================

IQDKSOUNDCTL_Init

Description:
   This function associates application class id with the given instance of
   IQDKSoundCtl interface. Any application that wishes to use IQDKSoundCtl
   interface will have to invoke this method after creating instance
   of IQDKSoundCtl interface and before calling any other IQDKSoundCtl
   interface methods.

Prototype:
   IQDK_ErrorType Init
   (
      IQDKSoundCtl *pMe,
      AEECLSID classId
   )

Parameters:
   pMe      :  Pointer to the IQDKSoundCtl Interface object.
   classId  :  Class id of application that wishes to use the IQDKSoundCtl
               interface.

Return Value:
   SUCCESS : Initialization is successful 
   EFAILED : Pointer to IQDKSoundCtl Interface object is NULL 

Comments:
   None

Version:
   Introduced in QDK v 2.0

See Also:
   None

================================================================================

IQDKSOUNDCTL_ClearSoundMask

Description:
   This function clears the sound mask according to the tone-type passed in. 
   The Sound Mask refers to a bitmask of the various QChat tones. It permits the 
   QDK application to customize which QChat tones will be played.  An example of 
   such a tone is the PTT Floor Grant Tone, which is played when the user has been 
   granted the floor.  A QDK application can turn this on by setting the corresponding 
   bit in the sound mask, or turn it off by zeroing the bit in the sound mask.
                
Prototype:
   IQDK_ErrorType IQDKSOUNDCTL_SetSoundMask
   (
      IQDKSoundCtl *pIQDKSoundCtl,
      IQDK_ToneMask toneMask
   )
                 
Parameters:
   pIQDKSoundCtl  :  Pointer to the IQDKSoundCtl Interface object.
   toneMask       :  The type of the tone that needs to be turned on.
    
Return Value:
   SUCCESS: The QChat Client is able to successfully set the tone mask.
   EQDKNOTINITIALIZED: This status is returned when IQDKSOUNDCTL_Init() has not been 
                       called by the QDK application, which causes IQDKSoundCtl data  
                       to remain uninitialized.
     
Comments:
   None
      
Version:
   Introduced in QDK v 2.0

See Also:
   IQDKSOUNDCTL_SetSoundMask
   IQDKSOUNDCTL_GetSoundMask
   IQDK_ToneMask

================================================================================

IQDKSOUNDCTL_GetMode

Description:
   This function gets the current sound mode that is set. The various modes include
   silent, speaker and manner mode

Prototype:
   IQDK_ErrorType IQDKSOUNDCTL_GetMode
   (
      IQDKSoundCtl *pMe,
      IQDK_SoundMode *mode
   )

Parameters:
   pMe  :  Pointer to the IQDKSoundCtl Interface object.
   mode :  pointer used to return the current sound mode

Return Value:
   SUCCESS: The QChat Client is able to successfully retrieve current sound mode. 
   EFAILED: Atleast one of the input parameters to the API is NULL.
   EQDKNOTINITIALIZED: This status is returned when IQDKSOUNDCTL_Init() has not been 
                       called by the QDK application, which causes IQDKSoundCtl data  
                       to remain uninitialized.

Comments:
   None

Version:
   Introduced in QDK v 2.0

See Also:
   IQDKSOUNDCTL_SetMode
   IQDK_SoundMode

================================================================================

IQDKSOUNDCTL_SetMode

Description:
   This function sets the sound mode. The various modes include silent, speaker 
   and manner mode.

Prototype:
   IQDK_ErrorType IQDKSOUNDCTL_SetMode
   (
      IQDKSoundCtl *pMe,
      IQDK_SoundMode mode
   )

Parameters:
   pMe  :  Pointer to the IQDKSoundCtl Interface object.
   mode :  The sound mode to be set

Return Value:
   SUCCESS : The QChat Client is able to set the sound mode successfully.
   EFAILED : Pointer to IQDKSoundCtl Interface object is NULL.
   EQDKNOTINITIALIZED : This status is returned when IQDKSOUNDCTL_Init() has not been 
                        called by the QDK application, which causes IQDKSoundCtl data  
                        to remain uninitialized.

Comments:
   None

Version:
   Introduced in QDK v 2.0

See Also:
   IQDKSOUNDCTL_GetMode
   IQDK_SoundMode

================================================================================
*/
