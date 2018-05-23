#ifndef AEEVOCPLUGIN_H
#define AEEVOCPLUGIN_H

/*===========================================================================
FILE:      AEEIVocPlugin.h

SERVICES:
   Provide frame-level access to the vocoder plugin (voice encoder/decoder) 

DESCRIPTION:
   The IVocPlugin class provides streaming access to output from the voice
   encoder and supports streaming of frames to the voice decoder for playout

PUBLIC CLASSES:  IVocPlugin

INITIALIZATION AND SEQUENCING REQUIREMENTS: NA

        Copyright © 2008 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
===========================================================================*/

/*===============================================================================
    INCLUDES AND VARIABLE DEFINITIONS
===============================================================================*/

/*-------------------------------------------------------------------
            Defines
-------------------------------------------------------------------*/

/*-------------------------------------------------------------------
      Include Files
-------------------------------------------------------------------*/

#include "AEE.h"
#include "AEEShell.h"
#include "AEEModGen.h"

#include "AEEIVocPlugin.bid"

#define NUM_VOC_PLUGINS 6


/*-------------------------------------------------------------------
      Type Declarations
-------------------------------------------------------------------*/
typedef enum {
   READY,
   SHUT_DOWN,
   SUSPENDED
} VocPlugState;

typedef enum {
   ACQUIRE,
   PLAYOUT
} opType;


typedef void (*VOCPLUGPFNREADY)(void * usrPtr, uint32 status, VocPlugState ivc);
typedef void (*VOCPLUGPFNDATA)(uint16 numFrames, void * usrPtr);


//When any vocoder is mapped on to EVRC, these rates are used.
enum {
   VOCPLUG_BLANK_RATE,          // Indicates data was blanked, during DTX
   VOCPLUG_EIGHTH_RATE,         // Indicates rate 1/8 data
   VOCPLUG_QUARTER_RATE,        // Indicates rate 1/4 data
   VOCPLUG_HALF_RATE,           // Indicates rate 1/2 data
   VOCPLUG_FULL_RATE,           // Indicates rate 1   data
   VOCPLUG_ERASURE,             // Indicates erasure frame 
   VOCPLUG_MAX_RATE
};

#ifdef WIN32
typedef unsigned VocPlugDataRateType;
#else
typedef int8 VocPlugDataRateType;
#endif

typedef struct
{
   void * usrPtr;
   VOCPLUGPFNREADY StatusCB;
   VOCPLUGPFNDATA FrameAcquiredCB;
   VOCPLUGPFNDATA FrameRequiredCB;
   VOCPLUGPFNDATA FramePlayedCB;
} IVocPluginInitConfig;  

typedef struct
{
   uint32 mode;                  // mode
   VocPlugDataRateType maxrate;
   VocPlugDataRateType minrate;   
   boolean overwrite;
   uint32 buffersize;            //watermark
} IVocPluginConfigType;

typedef struct 
{   
   uint8 dtxmin;
   uint8 dtxmax;
   uint8 hangover;
} DTX_MODE;



//******************************************************************************
//
// IVocPlugin Interface
//
//******************************************************************************
#ifdef ACM_FEATURE
#define INHERIT_IVocPlugin(iname)  \
   INHERIT_IQueryInterface(iname);  \
   int16   (*VocPlugin_SetDTXMode)(IVocPlugin *pVoc, DTX_MODE *pDTXmode); \
   int16   (*VocPlugin_Init)(IVocPlugin * pVoc, IVocPluginInitConfig config, AEECLSID appid); \
   int16   (*VocPlugin_SetBufferSize)(IVocPlugin * pVoc, uint32 bufferSize, opType opt); \
   int16   (*VocPlugin_Power)(IVocPlugin *pVoc, boolean power, IVocPluginConfigType *config); \
   int16   (*VocPlugin_Start)(IVocPlugin * pVoc, opType opt); \
   int16   (*VocPlugin_Stop)(IVocPlugin * pVoc, opType opt); \
   int16   (*VocPlugin_FlushBuffer)(IVocPlugin * pVoc, opType opt); \
   int16   (*VocPlugin_AcquireFrame)(IVocPlugin * pVoc, VocPlugDataRateType * pRate,uint16 * pLength, uint8 * pData); \
   int16   (*VocPlugin_PlayFrame)(IVocPlugin * pVoc,VocPlugDataRateType rate,uint16 length,const uint8 * pData); \
   int16   (*VocPlugin_GetBufferDepth)(IVocPlugin * pVoc, uint16 * pDepth, opType opt); \
   int16   (*VocPlugin_GetFrameLength)(IVocPlugin * pVoc, VocPlugDataRateType rate, uint16 * pLength) \
   int16   (*VocPlugin_GetFrameDuration)(IVocPlugin *pVoc, uint16 *pDuration) \
   int16   (*VocPlugin_GetMaxFrameSize)(IVocPlugin *pVoc, uint16 *maxframesize) \
   int16   (*VocPlugin_InUse(AEECLSID &usingApp); \
   int16   (*VocPlugin_Suspend)(IVocPlugin *pVoc); \
   int16   (*VocPlugin_Resume)(IVocPlugin *pVoc);

#else

#define INHERIT_IVocPlugin(iname)  \
   INHERIT_IQueryInterface(iname); \
   int16   (*VocPlugin_SetDTXMode)(IVocPlugin *pVoc, DTX_MODE *pDTXmode);\
   int16   (*VocPlugin_Init)(IVocPlugin * pVoc, IVocPluginInitConfig config, AEECLSID appid); \
   int16   (*VocPlugin_SetBufferSize)(IVocPlugin * pVoc, uint32 bufferSize, opType opt); \
   int16   (*VocPlugin_Power)(IVocPlugin *pVoc, boolean power, IVocPluginConfigType *config); \
   int16   (*VocPlugin_Start)(IVocPlugin * pVoc, opType opt); \
   int16   (*VocPlugin_Stop)(IVocPlugin * pVoc, opType opt); \
   int16   (*VocPlugin_FlushBuffer)(IVocPlugin * pVoc, opType opt); \
   int16   (*VocPlugin_AcquireFrame)(IVocPlugin * pVoc, VocPlugDataRateType * pRate,uint16 * pLength, uint8 * pData); \
   int16   (*VocPlugin_PlayFrame)(IVocPlugin * pVoc,VocPlugDataRateType rate,uint16 length,const uint8 * pData); \
   int16   (*VocPlugin_GetBufferDepth)(IVocPlugin * pVoc, uint16 * pDepth, opType opt); \
   int16   (*VocPlugin_GetFrameLength)(IVocPlugin * pVoc, VocPlugDataRateType rate, uint16 * pLength); \
   int16   (*VocPlugin_GetFrameDuration)(IVocPlugin *pVoc, uint16 *pDuration); \
   int16   (*VocPlugin_GetMaxFrameSize)(IVocPlugin *pVoc, uint16 *maxframesize)
#endif

/* declare the actual interface */
AEEINTERFACE_DEFINE(IVocPlugin);


static __inline uint32 IVOCPLUGIN_AddRef(IVocPlugin *me)
{
   return AEEGETPVTBL(me, IVocPlugin)->AddRef(me);
}

static __inline uint32 IVOCPLUGIN_Release(IVocPlugin *me)
{
   return AEEGETPVTBL(me, IVocPlugin)->Release(me);
}

static __inline int IVOCPLUGIN_QueryInterface(IVocPlugin *me, AEECLSID cls, void **ppo)
{
   return AEEGETPVTBL(me, IVocPlugin)->QueryInterface(me, cls, ppo);
}

static __inline int16 IVOCPLUGIN_SetDTXMode(IVocPlugin *me, DTX_MODE *pDTXMode)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_SetDTXMode(me, pDTXMode);
}

static __inline int16 IVOCPLUGIN_Init(IVocPlugin * me,
                           IVocPluginInitConfig config, AEECLSID appid)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_Init(me, config, appid);
}

static __inline int16 IVOCPLUGIN_SetBufferSize(IVocPlugin * me,
                           uint32 bufferSize, opType opt)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_SetBufferSize(me, bufferSize, opt);
}

static __inline int16 IVOCPLUGIN_Power(IVocPlugin *me,  boolean power, IVocPluginConfigType *config)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_Power(me, power, config);
}

static __inline int16 IVOCPLUGIN_Start(IVocPlugin *me, opType opt)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_Start(me, opt);
}

static __inline int16 IVOCPLUGIN_Stop(IVocPlugin *me, opType opt)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_Stop(me, opt);
}

static __inline int16 IVOCPLUGIN_FlushBuffer(IVocPlugin *me, opType opt)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_FlushBuffer(me, opt);
}


static __inline int16  IVOCPLUGIN_AcquireFrame(IVocPlugin * me,
                                                         VocPlugDataRateType * pRate,
                                                         uint16 * pLength,
                                                         uint8 * pData)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_AcquireFrame(me, pRate, pLength, pData);
}

static __inline int16 IVOCPLUGIN_PlayFrame(IVocPlugin *me,
                                                     VocPlugDataRateType rate,
                                                     uint16 length,
                                                     const uint8 * pData)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_PlayFrame(me, rate, length, pData);
}


static __inline int16 IVOCPLUGIN_GetBufferDepth(IVocPlugin * me,
                         uint16 * pDepth, opType opt)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_GetBufferDepth(me, pDepth, opt);
}

static __inline int16 IVOCPLUGIN_GetFrameLength(IVocPlugin *me,                                                       
                                                      VocPlugDataRateType rate,
                                                      uint16 *pLength)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_GetFrameLength(me, rate, pLength);
}

static __inline int16 IVOCPLUGIN_GetFrameDuration(IVocPlugin *me,                                                       
                                                      uint16 *pDuration)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_GetFrameDuration(me, pDuration);
}

static __inline int16 IVOCPLUGIN_GetMaxFrameSize(IVocPlugin *me,                                                       
                                                      uint16 *maxframesize)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_GetMaxFrameSize(me, maxframesize);
}

#ifdef ACM_FEATURE
static __inline int16 IVOCPLUGIN_InUse(IVocPlugin *me, AEECLSID &usingApp)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_InUse(me, usingApp);
}

static __inline int16 IVOCPLUGIN_Suspend(IVocPlugin *me)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_Suspend(me);
}

static __inline int16 IVOCPLUGIN_Resume(IVocPlugin *me)
{
   return AEEGETPVTBL(me, IVocPlugin)->VocPlugin_Resume(me);
}
#endif

/*=====================================================================
  DATA STRUCTURE DOCUMENTATION
=======================================================================

IVocPluginConfigType

Description:

   The IVocPluginConfigType structure supplies the information
   required to operate the vocoder device and supply/obtain
   frames

Definition:

   typedef struct
   {
      void * usrPtr;
      VOCPLUGPFNREADY ReadyCB;
      VOCPLUGPFNDATA FrameAcquiredCB;      
      VOCPLUGPFNDATA FramePlayedCB;
      VOCPLUGPFNDATA FrameRequiredCB;
      uint32 buffersize;            //watermark
      uint32 dwParam;                //mode
      VocPlugDataRateType maxrate;
      VocPlugDataRateType minrate;   
      boolean overwrite;            //Encoder overflow to 
                                    //discard newest (FALSE) or oldest (TRUE) 
                                    //frames
   } IVocPluginConfigType;


Members:
   usrPtr:             User data to be passed back to application in callbacks
   ReadyCB:            Callback function invoked when vocoder is ready for use
   FrameAcquiredCB:    Callback function invoked when encoder has frames available
   FrameRequiredCB:    Callback function invoked when decoder requires more frames
   FramePlayedCB:      Callback function invoked when decoder has played one+ frames
   buffersize:         Frame count to be accumulated before starting decoder sound
                       path, the size of both the in and out buffers, and non-real
                       time operation compensation.
                    (* more below)
   dwParam:            Mode of vocoder operation (** more below)
   max:                Maximum encoder data rate, 
                       For VOC_AMR vocoder it specifies the mode to use and                   
   min:                Minimum encoder data rate  
   overwrite:          Encoder overflow to discard newest (FALSE) or oldest (TRUE)
                       frames. When AMR vocoder is used the field is also used to 
		  set the bit flags to IVOC_AMR_SCRMODE and IVOC_AMR_DTXMODE
		  to indicate SCR on and DTX on for AMR Vocoder.

   * The buffersize value is used in many ways.  As its name implies, playout of
     frames will not start until enough frames to meet the watermark have been
     queued for playout.  For example, if the watermark value is 30, playout of
     frames will not start until VocOutWrite() has been called 30 times.

     The watermark value is also used to determine the size of the encode and
     decode queues.  An attempt to queue another frame will fail and return a
     BUFFER_FULL error code.  When encoding, a new frame will either overwrite
     the oldest frame or be thrown away.  This is dependent upon the 'overwrite'
     field. The overwrite field is used as a bit mask of flags when AMR vocoder
     is used. In addition to bit 0 used to indicate "frame overwrite", the 
     IVOC_AMR_SCRMODE and IVOC_AMR_DTXMODE can be used as bit 1 and 2 flags
     for specifying SCR and DTX on for AMR vocoder.

     Another definition of the watermark value is a compensation for operating in
     non real time.  Voice quality may be impacted if the watermark value is set
     too low, and media latency grows as watermark value is increased.  Some tuning
     on this parameter will be necessary.
     
  ** wParam is vocoder specific.  It really means the mode the vocoder is in.
     For instance, the SMV vocoder has six modes available(values of 0 - 5).
     
     For 4GV, DTX mode is one of the modes available in COP7, where COP is 
     capacity operation point.

See Also:
    VocPlugDataRateType

=============================================================================

PFNDATA

Description:
    This data type provides a frame count to the have data or need data callback
function. For the haveDataCB, numFrames is the number of encoded frames that are
queued. For the needDataCB, numFrames is the number of erasures that have been collected
while data was not available.

Definition:
    typedef void (*PFNDATA)(uint16 numFrames, void * usrPtr);

Members
    None

Comments:
Be aware that due to BREW latency these numbers may not reflect current
conditions by the time the application callback is invoked. In particular,
the haveDataCB may wish to continue reading until it receives
AEE_VOC_EBUFFER_EMPTY return value.

See Also:
    None

=============================================================================

    VocPlugDataRateType

Description:
    This enum specifies the various data rates a vocoder frame may have. Frames
rates correspond to frame lengths and are vocoder specific. 


//When any vocoder is mapped on to EVRC, these rates are used.
enum {
   VOCPLUG_BLANK_RATE,          // Indicates data was blanked, during DTX
   VOCPLUG_EIGHTH_RATE,         // Indicates rate 1/8 data
   VOCPLUG_QUARTER_RATE,        // Indicates rate 1/4 data
   VOCPLUG_HALF_RATE,           // Indicates rate 1/2 data
   VOCPLUG_FULL_RATE,           // Indicates rate 1   data
   VOCPLUG_ERASURE,             // Indicates erasure frame 
   MAX_RATE
}
*


Comments:
    None

See Also:
    None
============================================================================

typedef struct 
{   
   uint8 dtxmin,
   uint8 dtxmax,
   uint8 hangover
} DTX_MODE;

1. ENCODER REQUIREMENTS

1.1 SID Frame Format
The vocoder’s existing eighth rate packet format shall be used as the SID frame format. SID frames shall
not consist of full, half, or quarter rate packets, nor shall a separate packet format be defined for the
purpose of providing SID updates.

1.2 Eighth-Rate Hangover
The encoder shall implement eighth-rate hangover. The length of the hangover is negotiated in signaling,
which is beyond the scope of this document.

1.3 DTX Parameters
Signaling may be used to negotiate the use of DTX, following an offer/answer model. An endpoint
capable of DTX shall communicate the following parameters:
-- Willingness to use DTX (i.e. on/off) [Default value: on for RFC 3558[3], off for all other RTP
formats].
-- Minimum DTX Update Interval [Default value: 12 frames].
-- Maximum DTX Update Interval [Default value: 32 frames]. The DTX max shall be greater than
or equal to the DTX min.
-- Eighth-Rate Hangover [Default value: 1 frame].

1.3.1 The values for DTX max, DTX min, and eighth-rate hangover shall be represented as unsigned 8-
bit integers, i.e. limited to the range 0-255.
1.3.2 For vocoders using the RTP format defined in RFC 3558 [3], the default for willingness to use
DTX support defined in this specification shall be “on”, and the default values for DTX min, DTX
max, and eighth-rate hangover shall be used, unless negotiated otherwise. That is to say, for those
vocoders that use the RTP format in RFC 3558 [3], DTX support defined in this specification is
always assumed enabled unless the negotiation specifically disables it. For all other vocoders, if
all four of the DTX parameters defined above are absent from either the offer or the answer, it is
assumed that DTX support defined in this specification is not available in the endpoint(s).
1.3.3 If the offer as well as the answer contains DTX parameters, the value of DTX min in the answer
shall be greater than or equal to the value of DTX Min in the offer, and the value of DTX max in
the answer shall be less than or equal to the value of DTX max in the offer. Furthermore, the
eighth-rate hangover in the answer shall be greater than or equal to the eighth-rate hangover in the
offer.
1.3.4 If both offer and answer contain DTX parameters, then both sides shall pick values for DTX
parameters that are proposed in the answer.

1.3.5 If none or only one of offer or answer contains DTX parameters:

-- The default values for the DTX parameters shall initially be used by both sides.

-- For vocoders using the RTP format defined in RFC 3558 [3], if at any time during the flow, a
vocoder detects the remote vocoder as DTX-incapable, it should stop sending SID frames. That is
to say that the vocoder shall still continue to send non-eighth rate frames as well as eighth rate
hangover frames, but should not send eighth rate frames as SID frames. Silence should be
suppressed from the end of the eighth rate hangover until the beginning of the next talk spurt.
It may be determined that a vocoder is DTX incapable if the number of eighth rate frames with
contiguous RTP timestamps exceeds the default value of the eighth rate hangover.

For vocoders using the RTP format defined in RFC 3558 [3], if a vocoder is suppressing SID
frames because it detected that the remote vocoder is not DTX capable and subsequently detects
that the remote vocoder is DTX capable, the default values for the DTX parameters shall be used.

It may be determined that a vocoder is DTX capable if either of the following is true:
1. The marker bit is set in the RTP header of any packet or
2. RTP Sequence Number (SN) and Timestamp (TS) do not increase in step for any RTP
header, i.e., if RTP SN increases by ‘n’ between any two headers, but RTP TS increases by
more than ‘n * STRIDE’, where STRIDE is the regular increase in TS between two
consecutive headers (value of STRIDE is 160 for 8kHz vocoders producing 20 msec
packets).

1.3.6 If it is determined through signaling that either endpoint is unwilling to use DTX, the resulting
session shall not use DTX.

2  DECODER REQUIREMENTS
2.1 Decoding DTX Streams
The decoder shall be capable of decoding DTX streams without a priori knowledge of the DTX
parameters, i.e., without knowledge of whether DTX is on, DTX min, DTX max, or eighth-rate hangover.
2.2 Missing SID frames
The decoder shall be capable of generating comfort noise without receiving any eighth-rate frames,
regardless of the values that were negotiated for the DTX parameters.

=====================================================================
  INTERFACES   DOCUMENTATION
=====================================================================

IVocPlugin Interface


The IVocPlugin interface uses the default sound path to capture and play vocoder frames.

=============================================================================

IVOCPLUGIN_AddRef()

Description:
    This function is inherited from IBASE_AddRef(). 

See Also:
    IVOCPLUGIN_Release()

=============================================================================

IVOCPLUGIN_Release()

Description:
    This function is inherited from IBASE_Release(). 

See Also:
    IVOCPLUGIN_AddRef()

=============================================================================
IVOCPLUGIN_VocPlugin_SetDTXMode()

Description: 
   This function is used to set the DTX mode. If the pointer is NULL then DTX is off
   if ptr is not NULL DTX is on.

Prototype: 
   int16 IVOCPLUGIN_VocPlugin_SetDTXMode(IVocPLugin *pVoc, DTX_MODE *pDTX)

Parameters: 
   pVoc : Pointer to an IVocPlugin object
   pDTX : Pointer to the DTX_MODE structure

Return Value:
   Success if the DTX mode is set.

Comments: Sets up the DTX mode.

Side Effects: None.

====================================================================================

IVOCPLUGIN_VocPlugin_Init()

Description:
   This API that takes these params: 
   void * usrPtr, VOCPLUGPFNREADY StatusCB, VOCPLUGPFNDATA FrameAcquiredCB, 
   VOCPLUGPFNDATA FrameRequiredCB;
   VOCPLUGPFNDATA FramePlayedCB;

These will never change even if the various modes are changed, so it’s a one-time init.  
It would not lead to power up of DSP.  The using AppID may also be required here for ACM purposes.


Prototype:
   int16 IVOCPLUGIN_VocPlugin_Init(IVocPlugin* pVoc,
                               IVocPluginConfigType config, AEECLSID appid);
                               
Parameters:
   pVoc:     Pointer to an IVocPlugin object
   config:   Configuration parameters for the plugin vocoder object
   appid:    Using App ID, for ACM purposes.
Return Value:

   SUCCESS                    :Operation succeeded
   EBADPARM                   :zero buffersize value. Must be positive
   ENOMEMORY                  :Not able to allocate memory for vocoder data buffers
   VOC_ENOT_AVAIL             :Specified vocoder is not available
   ENOMEMORY:                 :Insufficient memory in the system. The previous configuration of the 
                               vocoder will be 
                               shutdown.
   EFAILED:                   :Configuration Failed.The previous configuration of the vocoder will be 
                               shutdown.


Comments:
  one-time initialization

Side Effects:

Version:

See Also:
   IVocPluginInitConfig
=============================================================================

IVOCPLUGIN_VocPlugin_SetBufferSize()
Description:
   This function sets up the watermark or the buffersize.

Prototype:
   int16 IVOCPLUGIN_VocPlugin_SetBufferSize(IVocPlugin* pVoc,
                               uint32 buffersize);
                               
Parameters:
   pVoc:     Pointer to an IVocPlugin object
   buffersize: Frame count to be accumulated before starting decoder sound
               path, the size of both the in and out buffers, and non-real
               time operation compensation.
   
Return Value:

   SUCCESS                    :Operation succeeded
   EBADPARM                   :zero buffersize value. Must be positive


Comments:
  sets the buffer size
Side Effects:

Version:

See Also:
   
=============================================================================

IVOCPLUGIN_VocPlugin_Power()

Description:
   this API would turn on/off the vocoder, and lead to the StatusCB firing when its done 
   It also configures the vocoder object to use a specific vocoder. Attempting to re-configure an already configured
   vocoder may first result in the previous configuration to be shutdown before attempting the new
   configuration. 
   To shutdown the vocoder, invoke this function with configuration set to VOC_NONE (i.e. with config.config set to VOC_NONE)

Prototype:
   int16 IVOCPLUGIN_VocPlugin_Power(IVocPlugin * pVoc, boolean power, IVocPluginConfigType *config);
Parameters:
   pVoc:     Pointer to an IVocPlugin object
   config:   Configuration parameters for the plugin vocoder object
   power: whether to TUN on.
Return Value:
   SUCCESS                    :Operation succeeded   
   ENOMEMORY                  :Not able to allocate memory for vocoder data buffers
   VOC_ENOT_AVAIL             :Specified vocoder is not available
   VOC_ERATE_OUT_OF_RANGE     :Specified min or max rate is not supported by the
                               vocoder
   VOC_EBAD_MODE              :Inappropriate mode value
   VOC_IN_USE                 :Vocoder is currently being used by a different instance of IVocPlugin. 
   ENOMEMORY:                 :Insufficient memory in the system. The previous configuration of the vocoder will be 
                              shutdown.
   EFAILED:                   :Configuration Failed.The previous configuration of the vocoder will be 
                               shutdown.

Comments:
  configure/load the vocoder
Side Effects:
   Begins accepting and queueing encoded frames from the vocoder
Version:
   
See Also:
   FrameAcquiredCB
=============================================================================

IVOCPLUGIN_VocPlugin_Start()
Description:
   This function starts encoding or decoding voice frames.
Prototype:
   int16 IVOCPLUGIN_VocPlugin_Start(IVocPlugin * pVoc, opType opt);
Parameters:
   pVoc:     Pointer to an IVocPlugin object
   opt :     Operation Type = ACQUIRE or PLAYOUT

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocPlugin ptr
   VOC_EIN_USE                :IVocPlugin in use by other application
   VOC_ENOT_CONFIGED          :IVocPlugin not configured

Comments:
   Does not clear the queue of encoded frames
Side Effects:
   Ceases accepting and queueing encoded frames from the vocoder
Version:
   Introduced BREW Client 2.0
See Also:
   FrameAcquiredCB
=============================================================================
IVOCPLUGIN_VocPlugin_FlushBuffer()
Description:
   This function clears the queue of encoded vocoder frames
Prototype:
   int16 IVOCPLUGIN_VocPlugin_FlushBuffer(IVocPlugin * pVoc, opType opt);
Parameters:
   pVoc:     Pointer to an IVocPlugin object
   opt :     Operation Type = ACQUIRE or PLAYOUT

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocPlugin ptr
   VOC_EIN_USE                :IVocPlugin in use by other application
   VOC_ENOT_CONFIGED          :IVocPlugin not configured

Comments:
   This operation will resume prior operation
Side Effects:
    Ceases accepting and queueing encoded frames from the vocoder and frees queued
frames
Version:
   
See Also:
   IVOCPLUGIN_VocPlugin_Start()
=============================================================================
IVOCPLUGIN_VocPlugin_AcquireFrame()
Description:
   This functions obtains the oldest encoded voice frame from the queue
Prototype:
   int16 IVOCPLUGIN_VocPlugin_AcquireFrame(IVocPlugin* pVoc,
                            VocPlugDataRateType* pRate,
                            uint16* pLength,
                            uint8* pData);
Parameters:
   pVoc:     Pointer to an IVocPlugin object
   pRate:    Pointer to storage to accept data rate of the frame

   pLength:  Pointer to storage to accept size in bytes of the frame. This parameter must be non-NULL. On input, *pLength
   must specify the size of the buffer pData. On return, *pLength will contain the actual size of the data filled in the buffer
   pData by this function.
   If the size of pData is lesser than the actual size of the frame, *pLength will be set to the actual size of the frame on return
   but there will be a data loss i.e. pData will be filled only with as much space as the size of pData 
   (i.e value of *pLength on input).

   pData:    Pointer to storage to accept the encoded frame bytes
Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocPlugin ptr
   VOC_EIN_USE                :IVocPlugin in use by other application
   VOC_ENOT_CONFIGED          :IVocPlugin not configured
   VOC_EBUFFER_EMPTY          :No encoded frame is queued
Comments:
   The buffer must be large enough to support the maximum frame rate for
   the vocoder, not merely the maximum used by the current rate limits
Side Effects:
   None
Version:
   
See Also:
   FrameAcquiredCB
=============================================================================
IVOCPLUGIN_VocPlugin_PlayFrame()
Description:
   This function starts the decoding and playout of queued frames
Prototype:
   int16 IVOCPLUGIN_VocPlugin_PlayFrame(IVocPlugin * pVoc);
Parameters:
   pVoc:     Pointer to an IVocPlugin object
Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocPlugin ptr
   AEE_VOC_EIN_USE            :IVocPlugin in use by other application
   AEE_VOC_ENOT_CONFIGED      :IVocPlugin not configured
Comments:
   The vocoder will not be started until the watermark of queued
   frames for decoding has been reached.  It may be necessary to append enough
   ERASURE frames to the queue in order to meet the watermark and start playout
Side Effects:
   None
Version:
   Introduced BREW Client 2.0
See Also:
   IVOCPLUGIN_VocPluginOutWrite()
   FrameRequiredCB

=============================================================================

IVOCPLUGIN_VocPlugin_GetBufferDepth()
Description:
   This function discovers how many frames are queued for playout
Prototype:
   int16 IVOCPLUGIN_VocPlugin_GetBufferDepth(IVocPlugin* pVoc, uint16* pDepth, opType opt);
Parameters:
   pVoc:     Pointer to an IVocPlugin object
   pDepth:   Pointer to storage to accept count of queued frames
   opt   :   Operation Type = ACQUIRE or PLAYOUT

Return Value:
   SUCCESS                    :Operation succeeded
   EBADCLASS                  :NULL IVocPlugin ptr
   EBADPARM                   :Bad input parameter pDepth
   VOC_EIN_USE                :IVocPlugin in use by other application
   VOC_ENOT_CONFIGED          ;IVocPlugin not configured
Comments:
   None
Side Effects:
   None
Version:
   
See Also:
   None

===============================================================================
IVOCPLUGIN_VocPlugin_GetFrameLength()
Description:
   This function provides the size of a specified vocoder frame at a given rate
Prototype:
   int16 IVOCPLUGIN_VocPlugin_GetFrameLength(IVocPlugin * pVoc,                                   
                                   VocPlugDataRateType rate,
                                   uint16 * pLength);
Parameters:
   pVoc:     Pointer to an IVocPlugin object
   vocoder:  Vocoder type of the frame
   rate:     Data rate of the frame
   pLength:  Pointer to storage to accept size of the frame
Return Value:
   size in bytes of a frame for given vocoder/rate, 
   
   or the value -1 if bad parameters are provided.
Comments:
   Vocoder need not be configured to utilize this method
Side Effects:
   None
Version:
   
See Also:
   None
==============================================================================
IVOCPLUGIN_VocPlugin_GetFrameDuration()
Description:
   This function provides the duration of the specified vocoder frame 
Prototype:
   int16 IVOCPLUGIN_VocPlugin_GetFrameDuration(IVocPlugin * pVoc,                                   
                                   uint16 * pDuration);
Parameters:
   pVoc:     Pointer to an IVocPlugin object
   vocoder:  Vocoder type of the frame
   pDuration:  Pointer to storage to accept duration of the frame
Return Value:
    size in bytes of a frame for given vocoder/rate, or the value -1 if bad parameters are provided.
   Comments:
	  Vocoder need not be configured to utilize this method
Side Effects:
   None
Version:
   
See Also:
   None
==============================================================================
IVOCPLUGIN_VocPlugin_GetMaxFrameSize()
Description:
   This function provides the size of a specified vocoder frame at a given rate
Prototype:
   int16 IVOCPLUGIN_VocPlugin_GetFrameDuration(IVocPlugin * pVoc,   
                                               uint16 * pmaxframesize);
Parameters:
   pVoc:     Pointer to an IVocPlugin object
   vocoder:  Vocoder type of the frame
   pmaxframesize:  Pointer to storage to accept the max frame size
Return Value:
   size in bytes of a frame for given vocoder/rate, 
   
   or the value -1 if bad parameters are provided.
Comments:
   Vocoder need not be configured to utilize this method
Side Effects:
   None
Version:
   
See Also:
   None
*/

#endif /* AEEVOCPLUGIN_H */

   