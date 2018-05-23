#ifndef AEEQDKADDRESS_H
#define AEEQDKADDRESS_H

//===========================================================================
// FILE: AEEQDKAddress.h
// 
// SERVICES: Defines data types used by the OEM and Core QChat classes.
// 
// DESCRIPTION:
//    Types used by the OEM to provide PTT and target info to Core.
//
// PUBLIC FUNCTIONS:
//    None.
//
// LEGAL:
//    (c) Copyright 2005 QUALCOMM Incorporated.
//    All Rights Reserved.  QUALCOMM Proprietary.
//    This technology is controlled by the United States
//    Government.  Diversion contrary to U.S. law prohibited.
//===========================================================================

//===========================================================================
//   INCLUDES AND PUBLIC DATA DECLARATIONS
//===========================================================================

#include "AEEComdef.h"

//---------------------------------------------------------------------------
//   Defines
//---------------------------------------------------------------------------

#define IQDK_MAX_DOMAIN_SIZE             253
#define IQDK_MAX_ADDRNAME_SIZE           64
#define IQDK_CONF_ID_SIZE                6

#define CONFID_IS_VALID(confId) (confId[0] | confId[1] | confId[2] | confId[3] | confId[4])

typedef uint8 IQDKConfIdType[IQDK_CONF_ID_SIZE];

typedef enum
{
   IQDK_ADDRTYPE_UNSPECIFIED,
   IQDK_ADDRTYPE_USER_ADDRESS,
   IQDK_ADDRTYPE_PREDEFINED_GROUP_ADDRESS,
   IQDK_ADDRTYPE_CHATROOM
} IQDKAddressType;

typedef struct
{
   IQDKAddressType         addrType;    //Type of this address
   AECHAR *                name;    //data for this address. Contents depend on type
   AECHAR *                domain;      //NULL if no domain, otherwise domain name
} IQDKAddressData;

typedef struct
{
   uint16                  count;       //number of entries in the addresses data block
   IQDKAddressData *       addresses;   //data block containing IQDKAddressData entries
} IQDKAddressListType;

typedef struct
{
   uint8 count;
   IQDKConfIdType * confId;
}IQDKConfIdList;


/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================

CONFID_IS_VALID

Description:
   This Macro is used to find out if the given conference Id is a valid conference Id
   or not. QDK Applications can use this to find out if the conference Id that they are 
   passing is valid or not before they send it out to the Client.

Definition:
   #define CONFID_IS_VALID(confId) (confId[0] | confId[1] | confId[2] | confId[3] | confId[4])

See Also:
   IQDKConfIdType

================================================================================

IQDKConfIdType

Description:
   This is a data structure that defines the QChat Conference Id. The
   Conference ID is a data structure containing opaque data meaningful
   to the QAS.  Conference IDs may be used to restart a prior QChat Call.
   The QAS will use the conference ID to invite all of the participants
   that were in the original call.

   For a definition of the data item IQDK_CONF_ID_SIZE, please refer to the
   file AEEQDKAddress.h.

Definition:
   typedef uint8 IQDKConfIdType[IQDK_CONF_ID_SIZE];

================================================================================

IQDKAddressType

Description:
   This data type identifies the format of the targeting information
   stored in an IQDKAddressType data structure.  Targets may be
   formatted as user or group addresses.

Definition:
   typedef enum
   {
      IQDK_ADDRTYPE_UNSPECIFIED,
      IQDK_ADDRTYPE_USER_ADDRESS,
      IQDK_ADDRTYPE_PREDEFINED_GROUP_ADDRESS,
      IQDK_ADDRTYPE_CHATROOM
   } IQDKAddressType;

Members: 
   IQDK_ADDRTYPE_UNSPECIFIED   : unknown QChat address type.
   IQDK_ADDRTYPE_USER_ADDRESS  : Specifies that a target address is formatted as a QChat user address.
   IQDK_ADDRTYPE_PREDEFINED_GROUP_ADDRESS : Specifies that a target address is formatted
                                            as a QChat predefined group address, 
                                            Ad hoc group calls do not have a group address, 
                                            but are assigned a conference Id.  
   IQDK_ADDRTYPE_CHATROOM : Specifies that a target address is formatted as a QChat chatroom 
                            address.

See Also:
   IQDKAddressData

================================================================================

IQDKAddressData

Description:
   IQDKAddressData contains the data for a single address.

Definition:
   typedef struct
   {
      IQDKAddressType         addrType;
      AECHAR *                name;
      AECHAR *                domain;
   } IQDKAddressData;

Members:
   addrType : type of address stored in this record
   name : for all addrType values:
          name = NULL-terminated username string(AECHAR * only)
   domain : domain(if included)     = NULL-terminated domain string
            domain(if not included) = NULL

See Also:
   IQDKAddressType

================================================================================

IQDKAddressListType

Description:
   This struct contains a list of addresses.

Definition:
   typedef struct
   {
      uint16                  count;
      IQDKAddressData *       addresses;
   } IQDKAddressListType;

Members:
   count     :  number of entries in the addresses data block
   addresses :  data block containing IQDKAddressData entries

See Also:
   IQDKAddressData

================================================================================

IQDKConfIdList

Description:
   This struct contains a list of conference ID values.

Definition:
   typedef struct
   {
      uint8 count;
      IQDKConfIdType * confId;
   }IQDKConfIdList;

Members:
   count     :  number of entries in the confID data block
   confId    :  data block containing IQDKConfIdType entries

See Also:
   IQDKConfIdType

==============================================================================*/

#endif /* AEEQDKADDRESS_H */
