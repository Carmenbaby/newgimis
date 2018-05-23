#ifndef AEEQDK30SOUNDCTL_H
#define AEEQDK30SOUNDCTL_H
/*=============================================================================
 
FILE: AEEQDK30SoundCtl.h

SERVICES: The IQDK30SoundCtl interface manages all the methods related to 
controlling the media and sound features of QChat.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IQDK30SOUNDCTL
   IQDK30SOUNDCTL_Init
   IQDK30SOUNDCTL_AddRef
   IQDK30SOUNDCTL_Release
   IQDK30SOUNDCTL_QueryInterface
   IQDK30SOUNDCTL_SetSoundMask
   IQDK30SOUNDCTL_GetSoundMask
   IQDK30SOUNDCTL_ClearSoundMask
   IQDK30SOUNDCTL_PauseMedia
   IQDK30SOUNDCTL_ResumeMedia
   IQDK30SOUNDCTL_SupressMedia  //featured define out of build for now

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/
/*=============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================*/
#include "AEE.h"
#include "AEEQDK30DataTypes.h"


/*=============================================================================
                    CLASS DEFINITIONS
=============================================================================*/
#define AEEIID_IQDK30SOUNDCTL 0x01075023

typedef struct IQDKSoundCtl IQDKSoundCtl;

#define INHERIT_IQDKSoundCtl(iname)\
    INHERIT_IQueryInterface(iname);\
        IQDK_ErrorType (*Init)(iname *po, AEECLSID appClsID);\
        IQDK_ErrorType (*ClearSoundMask)(iname *pMe, IQDK_ToneMask toneMask);\
        IQDK_ErrorType (*GetSoundMask)(iname *pMe, IQDK_ToneMask toneMask, IQDK_ToneMask *tone);\
        IQDK_ErrorType (*SetSoundMask)(iname *pMe, IQDK_ToneMask toneMask);\
        IQDK_ErrorType (*PauseMedia)(iname *pMe);\
        IQDK_ErrorType (*ResumeMedia)(iname *pMe)

#define INHERIT_IQDK30SoundCtl(iname)\
        INHERIT_IQDKSoundCtl(IQDK30SoundCtl)

typedef struct IQDK30SoundCtl IQDK30SoundCtl;

AEEINTERFACE(IQDK30SoundCtl)
{
   INHERIT_IQDK30SoundCtl(IQDK30SoundCtl);
};

#define IQDK30SOUNDCTL_Init(p,clsid)                   AEEGETPVTBL((p),IQDK30SoundCtl)->Init((p),(clsid))
#define IQDK30SOUNDCTL_AddRef(p)                       AEEGETPVTBL((p),IQDK30SoundCtl)->AddRef((p))
#define IQDK30SOUNDCTL_Release(p)                      AEEGETPVTBL((p),IQDK30SoundCtl)->Release((p))
#define IQDK30SOUNDCTL_QueryInterface(p,clsid,pp)      AEEGETPVTBL((p),IQDK30SoundCtl)->QueryInterface((p),(clsid),(pp))
#define IQDK30SOUNDCTL_SetSoundMask(p,t)               AEEGETPVTBL((p),IQDK30SoundCtl)->SetSoundMask((p),(t))
#define IQDK30SOUNDCTL_GetSoundMask(p,t,m)             AEEGETPVTBL((p),IQDK30SoundCtl)->GetSoundMask((p),(t),(m))
#define IQDK30SOUNDCTL_ClearSoundMask(p,t)             AEEGETPVTBL((p),IQDK30SoundCtl)->ClearSoundMask((p),(t))
#define IQDK30SOUNDCTL_PauseMedia(p)               AEEGETPVTBL((p),IQDK30SoundCtl)->PauseMedia((p))
#define IQDK30SOUNDCTL_ResumeMedia(p)              AEEGETPVTBL((p),IQDK30SoundCtl)->ResumeMedia((p))

#endif // AEEQDK30SOUNDCTL_H

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================


=======================================================================
  INTERFACES   DOCUMENTATION
=======================================================================

Interface Name: IQDK30SoundCtl

Description:
   IQDK30SoundCtl interface is a set of API methods that are used to pause
   and resume media, and to configure the tones that the QDK application
   would like the QChat Client to play.

   The IQDK30SoundCtl interface instance is obtained via ISHELL_CreateInstance()
   mechanism.

   The following header file is required:
   AEEQDK30SoundCtl.h

=============================================================================

IQDK30SOUNDCTL_AddRef()

Description:
   This method increments the reference count of the IQDK30SoundCtl Interface object.
   This allows the object to be shared by multiple callers. The object is freed when
   the reference count reaches 0 (zero).

Prototype:
   uint32 IQDK30SOUNDCTL_AddRef(IQDK30SoundCtl * po)

Parameters:
   po : Pointer to the IQDK30SoundCtl Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Side Effects:
   None

See Also:
   IQDK30SOUNDCTL_Release()

=============================================================================

IQDK30SOUNDCTL_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQDK30SOUNDCTL_Release(IQDK30SoundCtl * po)

Parameters:
   po : Pointer to the IQDK30SoundCtl Interface object

Return Value:
   Decremented reference count for the object. The object has been freed and is no
   longer valid if 0 (zero) is returned.

Comments:
   None

Side Effects:
   None

See Also:
   IQDK30SOUNDCTL_AddRef()

==============================================================================

IQDK30SOUNDCTL_QueryInterface()

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
   int IQDK30SOUNDCTL_QueryInterface(IQDK30SoundCtl *po, AEECLSID id, void **p)

Parameters:
   po : Pointer to IQDK30SoundCtl interface
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

==========================================================================================
IQDK30SOUNDCTL_PauseMedia()

Description:
   This function instructs the QChat Client to ignore incoming media.
   For example, while the user is participating in an active QChat call
   the QDK application could call IQDK30SOUNDCTL_PauseMedia() to discard QChat 
   voice packets while the user accepts a traditional voice call, without ever
   leaving the QChat call. The QDK application could later call
   IQDK30SOUNDCTL_ResumeMedia() to resume decoding and playback of media packets.

Prototype:
   IQDK_ErrorType IQDK30SOUNDCTL_PauseMedia(IQDK30SoundCtl *po)

Parameters:
   po: Pointer to the IQDK30SoundCtl Interface object.
   
Return Value:
   SUCCESS: The QChat Client was able to successfully pause the media stream.
   EFAILED : Pointer to IQDK30SoundCtl Interface object is NULL.
   EQDKNOTINITIALIZED: This status is returned when IQDK30SOUNDCTL_Init() has not been 
                       called by the QDK application, which causes IQDK30SoundCtl data  
                       to remain uninitialized.
  
Comments:
   IQDK30SOUNDCTL_PauseMedia() affects only the media and not the accompanying
   media signaling packets. While the media stream is paused, the QChat Client 
   will continue to process floor changes and other signaling that manages the 
   media stream.

Version:
   Introduced in QDK v 2.0

Side Effects:
   None
   
See Also:
   IQDK30SOUNDCTL_ResumeMedia()
   IQDK_ErrorType

================================================================================

IQDK30SOUNDCTL_ResumeMedia()

Description:
   This function instructs the QChat Client to resume processing the
   incoming media stream. For example, while the user is participating
   in an active QChat call, the QDK application could call IQDK30SOUNDCTL_PauseMedia()
   to discard QChat voice packets while the user accepts a traditional
   voice call, without ever leaving the QChat call. The QDK application
   could later call IQDK30SOUNDCTL_ResumeMedia() to resume decoding and playback
   of media packets.

Prototype:
   IQDK_ErrorType IQDK30SOUNDCTL_ResumeMedia(IQDK30SoundCtl *po)

Parameters:
   po:  Pointer to the IQDK30SoundCtl Interface object.

Return Value:
   SUCCESS: The QChat Client is able to successfully resume processing of the 
            media stream.
   EFAILED : Pointer to IQDK30SoundCtl Interface object is NULL.
   EQDKNOTINITIALIZED: This status is returned when IQDK30SOUNDCTL_Init() has not been 
                       called by the QDK application, which causes IQDK30SoundCtl data  
                       to remain uninitialized.

Comments:
   None

Version:
   Introduced in QDK v 2.0

See Also:
   IQDK30SOUNDCTL_PauseMedia()
   IQDK_ErrorType

================================================================================

IQDK30SOUNDCTL_SetSoundMask

Description:
   This function sets the sound mask according to the tone-type passed in. The 
   Tone Mask refers to a bitmask of the various QChat tones. It permits the QDK
   application to customize which QChat tones will be played. An example of such
   a tone is the PTT Floor Grant Tone, which is played when the user has been 
   granted the floor. A QDK application can turn this on by setting the 
   corresponding bit in the sound mask, or turn it off by clearing the bit in the 
   sound mask.
   
Prototype:
   IQDK_ErrorType IQDK30SOUNDCTL_SetSoundMask
   (
      IQDK30SoundCtl *po,
      IQDK_ToneMask toneMask
   )
  
Parameters:
   po:  Pointer to the IQDK30SoundCtl Interface object.
   toneMask:  The type of the tone that needs to be turned on.
 
Return Value:
   SUCCESS: The QChat Client is able to successfully set the tone mask.
   EQDKNOTINITIALIZED: This status is returned when IQDK30SOUNDCTL_Init() has not been 
                       called by the QDK application, which causes IQDK30SoundCtl data  
                       to remain uninitialized.

Comments:
   None

Version:
   Introduced in QDK v 2.0

See Also:
   IQDK30SOUNDCTL_GetSoundMask
   IQDK30SOUNDCTL_ClearSoundMask
   IQDK_ToneMask

================================================================================

IQDK30SOUNDCTL_GetSoundMask

Description:
   This function gets the sound mask according to the tone-type passed in. 
   The Sound Mask refers to a bitmask of the various QChat tones. It permits 
   the QDK application to customize which QChat tones will be played. 
   An example of such a tone is the PTT Floor Grant Tone, which is played when 
   the user has been granted the floor.  A QDK application can turn this on by 
   setting the corresponding bit in the sound mask, or turn it off by clearing 
   the bit in the sound mask.
               
Prototype:
   IQDK_ErrorType IQDK30SOUNDCTL_GetSoundMask
   (
      IQDK30SoundCtl *po,
      IQDK_ToneMask toneMask,
      IQDK_ToneMask *tone
   )
              
Parameters:
   po          :  Pointer to the IQDK30SoundCtl Interface object.
   toneMask    :  The type of the tone for which the tone needs to be retrieved. 
   tone        :  Pointer to the tone mask setting of the toneMask passed in.
                  returns IQDK_TONES_LAST, if IQDK30SoundCtl failed to get the tone mask for the
                  toneMask passed in.
             
Return Value:
   SUCCESS: The QChat Client is able to successfully set the tone mask.
   EQDKNOTINITIALIZED: This status is returned when IQDK30SOUNDCTL_Init() has not been 
                       called by the QDK application, which causes IQDK30SoundCtl data  
                       to remain uninitialized.
        
Comments:
   None
       
Version:
   Introduced in QDK v 2.0

See Also:
   IQDK30SOUNDCTL_SetSoundMask
   IQDK30SOUNDCTL_ClearSoundMask
   IQDK_ToneMask
      
================================================================================

IQDK30SOUNDCTL_Init

Description:
   This function associates application class id with the given instance of
   IQDK30SoundCtl interface. Any application that wishes to use IQDK30SoundCtl
   interface will have to invoke this method after creating instance
   of IQDK30SoundCtl interface and before calling any other IQDK30SoundCtl
   interface methods.

Prototype:
   IQDK_ErrorType Init
   (
      IQDK30SoundCtl *pMe,
      AEECLSID classId
   )

Parameters:
   pMe      :  Pointer to the IQDK30SoundCtl Interface object.
   classId  :  Class id of application that wishes to use the IQDK30SoundCtl
               interface.

Return Value:
   SUCCESS : Initialization is successful 
   EFAILED : Pointer to IQDK30SoundCtl Interface object is NULL 

Comments:
   None

Version:
   Introduced in QDK v 2.0

See Also:
   None

================================================================================

IQDK30SOUNDCTL_ClearSoundMask

Description:
   This function clears the sound mask according to the tone-type passed in. 
   The Sound Mask refers to a bitmask of the various QChat tones. It permits the 
   QDK application to customize which QChat tones will be played.  An example of 
   such a tone is the PTT Floor Grant Tone, which is played when the user has been 
   granted the floor.  A QDK application can turn this on by setting the corresponding 
   bit in the sound mask, or turn it off by zeroing the bit in the sound mask.
                
Prototype:
   IQDK_ErrorType IQDK30SOUNDCTL_SetSoundMask
   (
      IQDK30SoundCtl *po,
      IQDK_ToneMask toneMask
   )
                 
Parameters:
   po             :  Pointer to the IQDK30SoundCtl Interface object.
   toneMask       :  The type of the tone that needs to be turned on.
    
Return Value:
   SUCCESS: The QChat Client is able to successfully set the tone mask.
   EQDKNOTINITIALIZED: This status is returned when IQDK30SOUNDCTL_Init() has not been 
                       called by the QDK application, which causes IQDK30SoundCtl data  
                       to remain uninitialized.
     
Comments:
   None
      
Version:
   Introduced in QDK v 2.0

See Also:
   IQDK30SOUNDCTL_SetSoundMask
   IQDK30SOUNDCTL_GetSoundMask
   IQDK_ToneMask

================================================================================
*/
