#ifndef AEEXTRA_H
#define AEEXTRA_H

/*===========================================================================
 FILE:  AEEXTRA.h

SERVICES:  eXTended Receiver Assistance (XTRA).

GENERAL DESCRIPTION:
        Interface definitions, data structures, etc. for XTRA.

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR

*====*====*====*====*====*====*====*====*====*====*====*====*====*====*====*/

/*===========================================================================
EDIT HISTORY FOR MODULE

$Header: //TBD/AEEXTRA.h#1 $

when       who     what, where, why
--------   ---     ----------------------------------------------------------
12/20/06   ADI     Initial
=============================================================================*/

#include "AEE.h"

/* error codes */
/* error code base generated from AEEIID_IXTRA 0x0104326d by shifting left
   8 bits this allows for 256 unique error codes - this is an accepted BREW
   practice */
#define AEE_XTRA_ERR_BASE                 0x04326d00
#define AEE_XTRA_ERR_DWNLDINTERVAL        AEE_XTRA_ERR_BASE
#define AEE_XTRA_ERR_INTERNAL_RESOURCE    AEE_XTRA_ERR_BASE + 1
#define AEE_XTRA_ERR_GPS_SUBSYS_BUSY      AEE_XTRA_ERR_BASE + 2
#define AEE_XTRA_ERR_BADSTATE             AEE_XTRA_ERR_BASE + 3
#define AEE_XTRA_ERR_BAD_CRC              AEE_XTRA_ERR_BASE + 4
#define AEE_XTRA_ERR_TIME_OF_VALIDATION   AEE_XTRA_ERR_BASE + 5
#define AEE_XTRA_ERR_TIME_INFO            AEE_XTRA_ERR_BASE + 6
#define AEE_XTRA_ERR_TIME_UNSUPPORTED     AEE_XTRA_ERR_BASE + 7
#define AEE_XTRA_ERR_FEATURE_UNSUPPORTED  AEE_XTRA_ERR_BASE + 8

/* callback events */
#define AEE_XTRA_EVENT_DOWNLOAD_REQ  0x01
#define AEE_XTRA_EVENT_STATUS        0x02
#define AEE_XTRA_EVENT_TIME_REQ      0x04

/* command callbacks */
#define AEE_XTRA_CMD_SET_DATA        0x01
#define AEE_XTRA_CMD_TRIGGERDOWNLOAD 0x02
#define AEE_XTRA_CMD_SET_PARAM       0x04
#define AEE_XTRA_CMD_DATA_VALIDITY   0x08
#define AEE_XTRA_CMD_TIME_INFO       0x10

/* XTRA parameters */
#define AEE_XTRA_PARAM_BASE 0x00
#define AEE_XTRA_PARAM_AUTODWNLD_STATUS   AEE_XTRA_PARAM_BASE
#define AEE_XTRA_PARAM_AUTODWNLD_INTERVAL AEE_XTRA_PARAM_BASE + 1

typedef int32 AEEXTRAEventType;
typedef int32 AEEXTRACmdType;
typedef void (*AEEXTRAEvtCbk)(void* pCtx, AEEXTRAEventType eventId);
typedef void (*AEEXTRACmdCbk)(void* pCtx, AEEXTRACmdType cmdId, int32 nErr);

typedef struct AEEXTRADataInfo
{
   boolean bDwnldSuccess;   /* TRUE when download successful */
   uint8*  pXTRAData;       /* XTRA data */
   uint32  dwXTRADataLen;   /* Length of XTRA data */
   uint8   cPartNum;        /* Data part number */
   uint8   cPartsTotal;     /* Total data parts */
   
} AEEXTRADataInfo;

typedef struct AEEXTRATime
{
   uint64   dTimeMsec;      /* Time in msec */
   uint32   dwTimeUncMsec;  /* Associated time uncertainty */
   boolean  bRefToUtcTime;  /* Referenced to UTC or GPS time */
                            /* TRUE means UTC, else GPS */
   boolean  bForceFlag;     /* Force acceptance of data    */
   
} AEEXTRATime;

typedef struct AEEXTRAServerInfo
{
   char*  pBuffer;         /* Buffer containing comma separated url's */
                           /* in decreasing order of priority */
   uint32 dwServerNum;     /* Number of server url's */
   uint32 dwBufferLen;     /* Size of pBuffer */
   
} AEEXTRAServerInfo;

typedef struct AEEXTRADwnldInfo 
{
   AEEXTRAServerInfo serverInfo; /* Primary and backup XTRA servers */
   uint32 dwMaxFilePartSize;     /* Max amount of XTRA data that can be */
                                 /* sent in a single IXTRA_SetData() call */
   uint32 dwMaxFileSize;         /* Max total size of XTRA file (data) */
   
} AEEXTRADwnldInfo;

typedef struct AEEXTRATimeInfo
{
   AEEXTRAServerInfo serverInfo;   /* Primary and backup NTP servers */
   uint32 dwOneWayDelayFailoverThreshold;  /* One-way delay failover threshold in msec */
   
} AEEXTRATimeInfo;

typedef struct AEEXTRAStatusInfo
{
   uint8  cStatus;              /* status value */
   uint16 wStartGpsWeek;        /* GPS week of start of current XTRA info */
   uint16 wStartGpsMinute;      /* GPS minutes of week of start of XTRA info */
   uint16 wValidDurationHours;  /* XTRA info is valid for this many hours */
   
} AEEXTRAStatusInfo;

/* interface ID */
#define AEEIID_IXTRA 0x0104326d

#define INHERIT_IXTRA(iname)\
   INHERIT_IQueryInterface(iname);\
   int (*TriggerDownload) (iname* p);\
   int (*SetData) (iname* p, AEEXTRADataInfo* pIXTRADataInfo);\
   int (*SetTime) (iname* p, AEEXTRATime* pIXTRATime);\
   int (*SetParam) (iname* p, int32 nParam, int32 p1, int32 p2);\
   int (*GetParam) (iname* p, int32 nParam, int32* p1, int32* p2); \
   int (*RegisterCallback) (iname* p, AEEXTRAEvtCbk pfn, AEEXTRAEventType eventId, void* pCtx);\
   int (*RegisterCmdCallback) (iname* p, AEEXTRACmdCbk pfn, AEEXTRACmdType eventId, void* pCtx);\
   int (*DataValidity) (iname* p);\
   int (*GetDwnldInfo)(iname* p, AEEXTRADwnldInfo* pDwnldInfo);\
   int (*GetTimeInfo)(iname* p, AEEXTRATimeInfo* pTimeInfo);\
   int (*GetStatusInfo)(iname* p, AEEXTRAStatusInfo* pStatusInfo)
   

// declare the actual interface
AEEINTERFACE_DEFINE(IXTRA);

static __inline uint32 IXTRA_AddRef(IXTRA *pif)
{
   return AEEGETPVTBL(pif,IXTRA)->AddRef(pif);
}

static __inline uint32 IXTRA_Release(IXTRA *pif)
{
   return AEEGETPVTBL(pif,IXTRA)->Release(pif);
}

static __inline int IXTRA_QueryInterface(IXTRA *pif, 
   AEECLSID cls, void **ppo)
{
   return AEEGETPVTBL(pif,IXTRA)->QueryInterface(pif, cls, ppo);
}

static __inline int IXTRA_TriggerDownload(IXTRA *pif)
{
   return AEEGETPVTBL(pif,IXTRA)->TriggerDownload(pif);
}

static __inline int IXTRA_SetData(IXTRA *pif, 
   AEEXTRADataInfo* pIXTRADataInfo)
{
   return AEEGETPVTBL(pif,IXTRA)->SetData(pif, pIXTRADataInfo);
}

static __inline int IXTRA_SetTime(IXTRA *pif, 
   AEEXTRATime* pIXTRATime)
{
   return AEEGETPVTBL(pif,IXTRA)->SetTime(pif, pIXTRATime);
}

static __inline int IXTRA_SetParam(IXTRA *pif, 
   int32 nParam, int32 p1, int32 p2)
{
   return 
     AEEGETPVTBL(pif,IXTRA)->SetParam(pif, nParam, p1, p2);
}

static __inline int IXTRA_GetParam(IXTRA *pif, 
   int32 nParam, int32* p1, int32* p2)
{
   return 
      AEEGETPVTBL(pif,IXTRA)->GetParam(pif, nParam, p1, p2);
}

static __inline int IXTRA_RegisterCallback(IXTRA *pif,
   AEEXTRAEvtCbk pfn, AEEXTRAEventType eventId, void* pCtx)
{
   return AEEGETPVTBL(pif,IXTRA)->RegisterCallback(pif, pfn, eventId, pCtx);
}

static __inline int IXTRA_RegisterCmdCallback(IXTRA *pif,
   AEEXTRACmdCbk pfn, AEEXTRACmdType cmdId, void* pCtx)
{
   return AEEGETPVTBL(pif,IXTRA)->RegisterCmdCallback(pif, pfn, cmdId, pCtx);
}

static __inline int IXTRA_DataValidity(IXTRA *pif)
{
   return AEEGETPVTBL(pif,IXTRA)->DataValidity(pif);
}

static __inline int IXTRA_GetDwnldInfo(IXTRA *pif, 
   AEEXTRADwnldInfo* pDwnldInfo)
{
   return AEEGETPVTBL(pif,IXTRA)->GetDwnldInfo(pif, pDwnldInfo);
}

static __inline int IXTRA_GetTimeInfo(IXTRA *pif, 
   AEEXTRATimeInfo* pTimeInfo)
{
   return AEEGETPVTBL(pif,IXTRA)->GetTimeInfo(pif, pTimeInfo);
}

static __inline int IXTRA_GetStatusInfo(IXTRA *pif, 
   AEEXTRAStatusInfo* pStatusInfo)
{
   return AEEGETPVTBL(pif,IXTRA)->GetStatusInfo(pif, pStatusInfo);
}

/*
===============================================================================
DATA STRUCTURES DOCUMENTATION
===============================================================================

AEEXTRADataInfo

Description:
   Structure used to pass the downloaded XTRA data to the gpsOne subsystem.
      
Definition:

   typedef struct AEEXTRADataInfo
   {
      boolean bDwnldSuccess;
      uint8*  pIXTRAData;      
      uint32  dwIXTRADataLen;
      uint8   cPartNum;        
      uint8   cPartsTotal;     
   
   } AEEXTRADataInfo;
   
Members:

   bDwnldSuccess   TRUE when download successful
   pIXTRAData      XTRA data
   dwIXTRADataLen   Length of XTRA data
   cPartNum        Data part number
   cPartsTotal     Total data parts

Comments:
    None

See Also:
    None
===============================================================================

AEEXTRATime

Description:
   Structure used to pass the acquired time information to the gpsOne subsystem
   For float support applications should link with the float.o file supplied 
   from BREW extranet.
   
Definition:

   typedef struct AEEXTRATime
   {
      double   timeMsec;
      float    timeUncMsec;
      boolean  bRefToUtcTime;
      boolean  bForceFlag;
   
   } AEEXTRATime;
   
Members:

   timeMsec       Time value in msec
   timeUncMsec    Associated time uncertainty.
   bRefToUtcTime  Referenced to UTC/GPS time
   bForceFlag     Force acceptance of data

Comments:
    None

See Also:
    None
===============================================================================

AEEXTRAServerInfo

Description:
   Structure used to retrieve the primary and backup server url's. When pBuffer
   is set to null as IN parameter, dwServerNum on OUT contains the number of
   total servers provisioned and dwBufferLen contains the total buffer needed
   for the server url's in a comma separated string starting with primary 
   server followed by secondary, tertiary and so on. The application is 
   responsible for allocating memory for pBuffer.
   
Definition:

   typedef struct AEEXTRAServerInfo
   {
      char*  pBuffer;         
      uint32 dwServerNum;
      uint32 dwBufferLen;
   
   } AEEXTRAServerInfo;
  
Members:

   pBuffer       Buffer containing comma separated server url's
                 in decreasing order of priority
   dwServerNum   Number of server url's
   dwBufferLen   Size of pBuffer

Comments:
    None

See Also:
    None
===============================================================================

AEEXTRADwnldInfo

Description:
   Structure used to retrieve the download configuration information. XTRA
   server url's and maximum file part and size information.
   
Definition:

   typedef struct AEEXTRADwnldInfo 
   {
      AEEXTRAServerInfo serverInfo; 
      uint32 dwMaxFilePartSize;        
      uint32 dwMaxFileSize;
   
   } AEEXTRADwnldInfo;

Members:

   serverInfo         Primary and backup XTRA server info
   dwMaxFilePartSize  Max amount of XTRA data that can be sent in a single 
                      IXTRA_SetData() call
   dwMaxFileSize      Max total size of XTRA file (data) supported

Comments:
    None

See Also:
    None
===============================================================================

AEEXTRATimeInfo
   
Description:
   Structure used to retrieve the NTP server url's and one way delay
   failover threshold information.
   
Definition:

   typedef struct AEEXTRATimeInfo
   {
      AEEXTRAServerInfo serverInfo;
      uint32 dwOneWayDelayFailoverThreshold;
   
   } AEEXTRATimeInfo;

Members:

   serverInfo                       Primary and backup NTP server info
   dwOneWayDelayFailoverThreshold   One-way delay failover threshold in msec??
   
Comments:
    None

See Also:
    None
===============================================================================

AEEXTRAStatusInfo
   
Description:
   Structure used to retrieve the GPS status information for the current XTRA
   data.
   
Definition:

   typedef struct AEEXTRAStatusInfo
   {
      uint16 wStartGpsWeek;
      uint16 wStartGpsMinute;
      uint16 wValidDurationHours;
   
   } AEEXTRAStatusInfo;

Members:

   wStartGpsWeek         GPS week of start of current XTRA info
   wStartGpsMinute       GPS minutes of week of start of XTRA info
   wValidDurationHours   XTRA info is valid for this many hours
   
Comments:
    None

See Also:
    None
===============================================================================
*/

/*
===============================================================================
MACROS DOCUMENTATION
===============================================================================
Error Codes

   AEE_XTRA_ERR_BASE
      Start of the error code range for the AEECLSID_IXTRA
      
   AEE_XTRA_ERR_DWNLDINTERVAL
      Download interval range is less than 1 hour or more than 168 hours.
      
   AEE_XTRA_ERR_INTERNAL_RESOURCE
      Internal resource unavailable
      
   AEE_XTRA_ERR_GPS_SUBSYS_BUSY
      gpsOne sub system is busy and unable to hanle request
      
   AEE_XTRA_ERR_BADSTATE
      Internal state error.
      
   AEE_XTRA_ERR_BAD_CRC
      CRC for data passed in bad.
      
   AEE_XTRA_ERR_TIME_OF_VALIDATION
      Time of applicability of XTRA data is in past.
      
   AEE_XTRA_ERR_TIME_INFO
      Client cannot acquire time from specified servers.
===============================================================================
Callback Events

   AEE_XTRA_EVENT_DOWNLOAD_REQ
      Callback event in response to IXTRA_TriggerDownload() or autonomously
      generated by gpsOne subsystem. Client is expected to start downloading 
      XTRA data on receiving this callback. Downloaded configuration 
      information is available via IXTRA_GetDwnldInfo() on receiving the 
      callback. The downloaded data (successful or unsuccessful) has to be 
      passed via IXTRA_SetData().
      
   AEE_XTRA_EVENT_STATUS
      Callback event in response to IXTRA_DataValidity() call. Client can fetch
      the current XTRA data's GPS information using IXTRA_GetStatusInfo().
      
   AEE_XTRA_EVENT_TIME_REQ
      Autonomous event callback generated from gpsOne subsystem. On this 
      callback the client is expected to acquire time from NTP server. The NTP
      configuration information can be obtained via IXTRA_GetTimeInfo()
      on receiving the callback. The acquired time info (successful or 
      unsuccessful) needs to be passed via IXTRA_SetTime(). Application can 
      also SetTime from other source than NTP server, if it has access to
      other sources of time information.
      
===============================================================================
XTRA_MAX_TIME_UNCERTAINITY

   Maximum uncertainity value for acquired time. Used to indicate failure in
   acquiring time.
===============================================================================
*/

/*
===============================================================================
INTERFACES DOCUMENTATION
===============================================================================

IXTRA Interface

Description:
   This interface allows to set the XTRA data and time information in the
   gpsOne subsystem. It allows to register for event callbacks from the 
   gpsOne subsystem related to XTRA functionality. It also allows to 
   trigger the callback for XTRA data download.

===============================================================================

IXTRA_AddRef()

Description:
   This function is inherited from IQI_AddRef().

See Also:
   IXTRA_Release()

===============================================================================

IXTRA_Release()

Description:
   This function is inherited from IQI_Release().

See Also:
   IXTRA_AddRef()

===============================================================================

IXTRA_QueryInterface()

Description:
   This function is inherited from IQI_QueryInterface().

===============================================================================

IXTRA_TriggerDownload()

Description:
   This function is used to trigger the download of extended receiver 
   assistance (XTRA) data file. Invoking this function will lead to generation 
   of a callback event: AEE_XTRA_EVENT_DOWNLOAD_REQ. The callback handler for this 
   event should download the XTRA file.
   
   On receiving the AEE_XTRA_EVENT_DOWNLOAD_REQ event callback the application
   needs to call IXTRA_GetDwnldInfo() to retrieve the XTRA download server and
   data partitioning information. This information does not change during the 
   lifetime of the application and hence the application can cache the 
   information. Once the application starts using the cached server url's it is
   expected that it will randomize the assignment of priority status (primary, 
   secondary, etc) to cached server url's to keep the load distributed amongst
   download servers.

Prototype:
   int IXTRA_TriggerDownload(IXTRA *pif, void* pCtx);

Parameters:
   pif:  Pointer to IXTRA interface
   pCtx: Pointer to client data, passed in the callback. 

Return Value:
   SUCCESS: If triggering is successful.
   AEE_XTRA_ERR_BADSTATE: Internal state error.
   AEE_XTRA_ERR_INTERNAL_RESOURCE: insufficient internal resources to complete
                                   action.
   AEE_XTRA_ERR_GPS_SUBSYS_BUSY: gpsOne subsystem is busy ans cannot accept the
                                 request.
   EFAILED: If the operation failed to trigger download.

Comments:
   None.
   
Side Effects:
   XTRA event (AEE_XTRA_EVENT_DOWNLOAD_REQ) callback is generated.

===============================================================================

IXTRA_SetData()

Description:
   This function is used to pass the downloaded XTRA data to the gpsOne 
   subsystem. A client should call this function even if the download fails.
   Status of download is indicated in AEEXTRADataInfo::nDwnldSuccess variable,
   it is set to FALSE if download failed and to TRUE otherwise. It may be 
   necessary to break the downloaded data into smaller chunks (parts) and not
   pass the entire downloaded data in one shot, this may be needed based on the
   dwMaxFilePartSize and dwMaxFileSize values in download configuration 
   information. If the data is broken up into parts, the 
   AEEXTRADataInfo::cPartsTotal variable indicates the total number of parts
   and the AEEXTRADataInfo::cPartNum variable indicates the current part being
   passed (starting from 1). When the data is not broken up, 
   AEEXTRADataInfo::cPartsTotal should be set to 1.
      
Prototype:
   int IXTRA_SetData(IXTRA *pif, 
      AEEXTRADataInfo* pIXTRADataInfo, void* pCtx);

Parameters:
   pif: Pointer to IXTRA interface
   pIXTRADataInfo: Pointer to AEEXTRADataInfo.
   pCtx: Pointer to client data, passed in the callback.

Return Value:
   SUCCESS: If the data was successfully sent.
   AEE_XTRA_ERR_BADSTATE: Internal state error.
   AEE_XTRA_ERR_BAD_CRC:  CRC of data is invalid.
   AEE_XTRA_ERR_TIME_OF_VALIDATION: Time of applicability of the data is 
                                    in past.
   AEE_XTRA_ERR_INTERNAL_RESOURCE: insufficient internal resources to complete
                                   action.
   EFAILED: otherwise.
   
Comments:

Side Effects:

===============================================================================

IXTRA_SetTime()

Description:
   This function is used to pass the acquired time information from the server
   (NTP server) to the gpsOne subsystem. This function should be called for
   both successful and unsuccessful cases in acquiring time from the server.
   
   Time information can be pushed autonomously also from the application. 
   the application can choose to acquire time from other sources than NTP. If 
   the application is unable to acquire time it should call IXTRA_SetTime() 
   with all values in pIXTRATime set to 0, except timeUncMsec which should be 
   set to maximum float value. IXTRA_SetTime() is only supported for
   UMTS networks.
   
Prototype:
   int IXTRA_SetTime(IXTRA *pif, 
      AEEXTRATime* pIXTRATime, void* pCtx);
      
Parameters:
   pif: Pointer to IXTRA interface
   pIXTRATime: Pointer to AEEXTRATime.
   pCtx: Pointer to client data, passed in the callback.

Return Value:
   SUCCESS: If no errors.
   AEE_XTRA_ERR_BADSTATE: Internal state error.
   AEE_XTRA_ERR_TIME_INFO: Client unable to acquire time from specified
                           servers.
   AEE_XTRA_ERR_TIME_UNSUPPORTED: IXTRA_SetTime() not supported for 
                                  CDMA networks. Supported for UMTS 
                                  only.                        
   EFAILED: otherwise.
   
Comments:

Side Effects:

===============================================================================

IXTRA_SetParam()

Description:
   This function allows to modify parameters related to automatic download 
   functionality for XTRA data. 
   
Prototype:
   int IXTRA_SetParam(IXTRA *pif, 
      boolean bAutoDwnld, uint16 wDwnldInterval, void* pCtx);

Parameters:
   pif: Pointer to IXTRA interface
   bAutoDwnld: Flag to enable (set to TRUE) or disable (set to FALSE) automatic 
               download feature.
   wDwnldInterval: number of hours between automatic downloads.
   pCtx: Pointer to client data, passed in the callback.

Return Value:
   SUCCESS: If no errors.
   AEE_XTRA_ERR_BADSTATE: Internal state error.
   AEE_XTRA_ERR_DWNLDINTERVAL: if wDwnldInterval range is < 1 or > 168.
   EFAILED: otherwise.
   
Comments:

Side Effects:

===============================================================================

IXTRA_RegisterCbk()

Description:
   This function allows an application to register callbacks for following
   events: AEE_XTRA_EVENT_DOWNLOAD_REQ, AEE_XTRA_EVENT_STATUS and AEE_XTRA_EVENT_TIME_REQ.
   
Prototype:
   int IXTRA_RegisterCbk(IXTRA *pif, 
      AEECallback* pcb, int nEvent);

Parameters:
   pif: Pointer to IXTRA interface
   pcb: Pointer to AEECallback for the event specified in nEvent.
   nEvent: one of AEE_XTRA_EVENT_DOWNLOAD_REQ, AEE_XTRA_EVENT_STATUS or 
           AEE_XTRA_EVENT_TIME_REQ

Return Value:
   SUCCESS: If no errors.
   EFAILED: otherwise.
   
Comments:

Side Effects:

===============================================================================

IXTRA_DataValidity()

Description:
   This function allows an application to request current XTRA data's validity 
   information from the gpsOne subsystem. The AEE_XTRA_EVENT_STATUS event 
   callback will be sent to the application when the information is ready. 
      
   On receiving the AEE_XTRA_EVENT_STATUS event callback the application needs
   to call IXTRA_GetStatusInfo() to retrieve the status information. Ths status
   information contains the starting GPS week and GPS minutes along with
   the duration of validity for the current extended assistance data.
   
Prototype:
   int IXTRA_DataValidity(IXTRA *pif, void* pCtx);

Parameters:
   pif: Pointer to IXTRA interface
   pCtx: Pointer to client data, passed in the callback.

Return Value:
   SUCCESS: if no errors
   AEE_XTRA_ERR_BADSTATE: Internal state error.
   EFAILED: otherwise
   
Comments:

Side Effects:

===============================================================================

IXTRA_GetDwnldInfo()

Description:
   This function is called on receiving the AEE_XTRA_EVENT_DOWNLOAD_REQ event
   callback. The purpose is to obtain download information 1) server url's
   (primary and backups), 2) maximum file size and 3) maximum file parts 
   allowed. As the download info does not change during the lifetime of the
   application, the application can cache this information locally and not
   retrieve this information every time AEE_XTRA_EVENT_DOWNLOAD_REQ callback is 
   received. Once the application starts using the cached server url's it is
   expected that it will randomize the assignment of priority status (primary,
   secondary, etc) to cached servers to keep the load distributed amongst 
   download servers.
   
Prototype:
   int IXTRA_GetDwnldInfo(IXTRA *pif, 
      AEEXTRADwnldInfo* pDwnldInfo);

Parameters:
   pif: Pointer to IXTRA interface
   pDwnldInfo:
   
Return Value:
   SUCCESS: If no errors.
   EFAILED: otherwise.
   
Comments:

Side Effects:

===============================================================================

IXTRA_GetTimeInfo()

   This function is called on receiving the AEE_XTRA_EVENT_TIME_REQ event
   callback. The purpose is to obtain ntp server information 1) server url's
   (primary and backups), and 2) one way delay failover threshold value.
   As this information does not change during the lifetime of the
   application, the application can cache this information locally and not
   retrieve this information every time AEE_XTRA_EVENT_TIME_REQ callback is 
   received. Once the application starts using the cached server url's it is
   expected that it will randomize the assignment of priority status (primary, 
   secondary, etc) to cached servers to keep the load distributed amongst 
   download servers.
   
Description:

Prototype:
   int IXTRA_GetTimeInfo(IXTRA *pif, 
      AEEXTRATimeInfo* pTimeInfo);

Parameters:
   pif: Pointer to IXTRA interface
   pTimeInfo:
   
Return Value:
   SUCCESS: If no errors.
   EFAILED: otherwise.
   
Comments:

Side Effects:

===============================================================================

IXTRA_GetStatusInfo()

Description:

   This function is called on receiving the AEE_XTRA_EVENT_STATUS event
   callback. The purpose is to obtain XTRA data status information.
      
Prototype:
   int IXTRA_GetStatusInfo(IXTRA *pif, 
      AEEXTRAStatusInfo* pStatusInfo);

Parameters:
   pif: Pointer to IXTRA interface
   pStatusInfo:
   
Return Value:
   SUCCESS: If no errors.
   EFAILED: otherwise.
   
Comments:

Side Effects:

===============================================================================
*/

#endif /* #ifndef AEEXTRA_H */
