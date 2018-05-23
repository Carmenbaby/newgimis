#ifndef AEEQDKERROR_H
#define AEEQDKERROR_H

/*======================================================
FILE:  AEEIQDKError.h

SERVICES:  QDK Error Codes

GENERAL DESCRIPTION:

        Copyright c 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary/GTDR
=====================================================*/

// Basic QDK Error Codes
#define EQDKUNSUPPORTED                    (ERROR_USER + 0)
#define EQDKFEATUREUNAVAILABLE             (ERROR_USER + 1)
#define EQDKACTIONNOTCURRENTLYAVAILABLE    (ERROR_USER + 2)
#define EQDKCOREONLINE                     (ERROR_USER + 3)
#define EQDKCOREOFFLINE                    (ERROR_USER + 4)
#define EQDKQCHATSERVICEUNAVAILABLE        (ERROR_USER + 5)
#define EQDKDIGITALSERVICEUNAVAILABLE      (ERROR_USER + 6)
#define EQDKCANTADDTOCALL                  (ERROR_USER + 7)
#define EQDKNOCALLTYPECAPABILITY           (ERROR_USER + 8)
#define EQDKRESTRICTIONSET                 (ERROR_USER + 9)
#define EQDKALREADYINACALL                 (ERROR_USER + 10)
#define EQDKNOTINACALL                     (ERROR_USER + 11)
/* Talk_Group Feature TBD: whether to add it as a basic error code? */
#define EQDKNOTALKGROUPPERMISSIONS		   (ERROR_USER + 12)

//Errors related to API call parameters provided
#define PARAM_ERROR_BASE                   (ERROR_USER + 1000)
#define EQDKBADVALUE                       (PARAM_ERROR_BASE + 0)
#define EQDKTOOMANYTARGETS                 (PARAM_ERROR_BASE + 1)
#define EQDKINVALIDADDRESSLIST             (PARAM_ERROR_BASE + 2)
#define EQDKINVALIDQCHATADDRESS            (PARAM_ERROR_BASE + 3)
#define EQDKINVALIDCONFERENCEID            (PARAM_ERROR_BASE + 4)
#define EQDKOUTOFRANGE                     (PARAM_ERROR_BASE + 5)
#define EQDKTOOMANYENTRIES                 (PARAM_ERROR_BASE + 6)
#define EQDKRESTRICTIONCONFLICT            (PARAM_ERROR_BASE + 7)
#define EQDKRESTRICTIONNOTALLOWED          (PARAM_ERROR_BASE + 8)
#define EQDKNOTINITIALIZED                 (PARAM_ERROR_BASE + 9)
#define EQDKPDUSIZEEXCEEDED                (PARAM_ERROR_BASE + 10)

// For some reason, BREW has spelled EBADPARAM as EBADPARM
#define EBADPARAM                          EBADPARM
#endif


/*============================================================================
   DATA STRUCTURE DOCUMENTATION
==============================================================================
Error Codes

This section lists the user-defined error codes that are returned by the QDK
Client and the corresponding description of the error codes. Note that in 
addition to these user-defined error codes, QDK also uses the standard error
codes that are defined in BREW.
===H2>
List of errors
===/H2>

EQDKUNSUPPORTED : The API is unsupported by QDK.
===BR>
EQDKFEATUREUNAVAILABLE : The feature is unsupported by QDK.
===BR>
EQDKACTIONNOTCURRENTLYAVAILABLE : The action is not currently available.
===BR>
EQDKCOREONLINE: Error since the QChat Client is currently online.
===BR>
EQDKCOREOFFLINE : Error since the QChat Client is currently offline.
===BR>
EQDKQCHATSERVICEUNAVAILABLE: QChat Service unavailable.
===BR>
EQDKDIGITALSERVICEUNAVAILABLE: Digital Service unavailable.
===BR>
EQDKCANTADDTOCALL: Can not add user to call
===BR>
EQDKNOCALLTYPECAPABILITY: Cannot originate the call type or participate in the call type
===BR>
EQDKRESTRICTIONSET: Restriction is set
===BR>
EQDKALREADYINACALL: QChat Client is already in a call and hence is unable to service the particular request 
===BR>
EQDKNOTINACALL: QChat Client is not in a call and hence is unable to service the particular request 


related to API call parameters
===BR>
EQDKBADVALUE: Bad value in API.
===BR>
EQDKTOOMANYTARGETS: Too many targets.
===BR>
EQDKINVALIDADDRESSLIST: Invalid target address list.
===BR>
EQDKINVALIDQCHATADDRESS: Invalid QChat address.
===BR>
EQDKINVALIDCONFERENCEID: Invalid conference Id.
===BR>
EQDKOUTOFRANGE: Out of Range.
===BR>
EQDKTOOMANYENTRIES: Too many entries specified.
===BR>
EQDKRESTRICTIONCONFLICT: User tries to add a new call restriction that conflicts 
                         with an existing carrier level call restriction or is 
                         identical to a carrier level call restriction
===BR>
EQDKRESTRICTIONNOTALLOWED: Restriction is not allowed on a specific entry .
===BR>
EQDKNOTINITIALIZED: QDK App has not called the Init method.
===BR>
EQDKPDUSIZEEXCEEDED: If the size of the message that has to be transferred becuase of an API call, 
                     exceeds the PDU size, the Client will send this error code to the QDK application.
========================================================================= */
