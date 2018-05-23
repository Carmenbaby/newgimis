#ifndef AEEQDK30PROVISION_H
#define AEEQDK30PROVISION_H
/*=============================================================================
 
FILE: AEEIQDK30Provision.h

PROVISIONS: The IQDK30Provision interface provides functions to manage QChat Client
          provisioning.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IQDK30Provision
   IQDK30PROVISION_AddRef
   IQDK30PROVISION_Release
   IQDK30PROVISION_Init
   IQDK30PROVISION_SyncResults
   IQDK30PROVISION_InformOfUpdate
   IQDK30PROVISION_GetNumProvKeys
   IQDK30PROVISION_GetProvEntry_Details
   IQDK30PROVISION_GetProvEntry
   IQDK30PROVISION_SetProvEntry
   IQDK30PROVISION_GetAddressListSize
   IQDK30PROVISION_GetProvEntry_AddressList
   IQDK30PROVISION_SetProvEntry_AddressList
   IQDK30PROVISION_GetTableSize
   IQDK30PROVISION_GetProvEntry_Table
   IQDK30PROVISION_SetProvEntry_Table

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================*/
#include "AEE.h"
#include "AEEQDK30DataTypes.h"
#include "AEEVocoder.h"
#include "AEEQDKAddress.h"
#include "AEEIQDKProvision.h"

/*=============================================================================
                    CLASS DEFINITIONS
=============================================================================*/

#define AEEIID_IQDK30PROVISION 0x01074d45

#define IQDK_PROV_MULTICAST_GSK_LENGTH                      32    // 256-bit GSK
#define IQDK_PROV_MULTICAST_GSK_TIME_LENGTH                  4    // 32-bit GSK time
#define IQDK_PROV_MULTICAST_ENCRYPTED_GSK_TIME_LENGTH       16    // 128-bit (32-bit time + 96 bit padding)
#define IQDK_PROV_MULTICAST_CNONCE_LENGTH                   16    // 128-bit cnonce

// 3.0CT Fixed clustering
#define IQDK_PROV_FIXEDCLUSTERING_MAX_NUM_SUBNETS            4    // maximum number of subnets
#define IQDK_PROV_FIXEDCLUSTERING_MAX_NUM_SECTORS           64    // maximum number of subnets
#define IQDK_PROV_FIXEDCLUSTERING_SUBNET_LENGTH             13    // 128-bit subnet-sectorID, 104-bit subnetID
#define IQDK_PROV_FIXEDCLUSTERING_SECTORID_LENGTH           3     // 128-bit subnet-sectorID, 24-bit sectorID

typedef struct IQDK30Provision IQDK30Provision;

typedef enum
{
   IQDK30_PROV_TABLETYPE_DOMAIN_MAPPING_TABLE,
   IQDK30_PROV_TABLETYPE_FOREIGN_DOMAIN_MAPPING_TABLE,
   IQDK30_PROV_TABLETYPE_VOCODER_MAPPING_TABLE,
   IQDK30_PROV_TABLETYPE_ALERT_MAPPING_TABLE,
   IQDK30_PROV_TABLETYPE_PRESENCE_MAPPING_TABLE,
   IQDK30_PROV_TABLETYPE_CALL_RESTRICTION_TABLE,
   IQDK30_PROV_TABLETYPE_GROUP_RESTRICTION_TABLE,
   IQDK30_PROV_TABLETYPE_MULTICAST_GROUP_MAPPING_TABLE,
   IQDK30_PROV_TABLETYPE_MAX
} IQDK30_ProvTableType;

typedef enum
{
   IQDK30_PROV_ADDRESSES_PREDEFINED_GROUPS,
   IQDK30_PROV_ADDRESSES_ALTERNATE_USER,
   IQDK30_PROV_ADDRESSES_PREDEFINED_LOCALIZED_GROUPS,
   IQDK30_PROV_ADDRESSES_MAX
} IQDK30_ProvAddressListType;

typedef AECHAR     IQDK_ProvDomainEntryType[IQDK_MAX_DOMAIN_SIZE];
typedef AECHAR     IQDK_ProvForeignDomainEntryType[IQDK_MAX_DOMAIN_SIZE];
typedef AECHAR     IQDK_ProvAlertEntryType[IQDK_ALERT_MAX_STRING_SIZE];
typedef AECHAR     IQDK_ProvAuthKey[IQDK_PROV_AUTHKEY_LENGTH];
typedef uint8      IQDK_ProvMulticastGSKType[IQDK_PROV_MULTICAST_GSK_LENGTH];
typedef uint8      IQDK_ProvMulticastCnonceType[IQDK_PROV_MULTICAST_CNONCE_LENGTH];

struct IQDKLocalizedAddressListType
{
   char* localizedAddress;
   struct IQDKLocalizedAddressListType* next;
};

typedef union
{
   IQDKAddressListType                       addressListEntry;
   struct IQDKLocalizedAddressListType*             localizedAddressListEntry;       
} IQDK30_ProvListEntry;

typedef struct
{
   IQDKAddressData                  groupName;
   IQDKConfIdType                   confID;
} IQDK_GroupRestrictionTableEntry;

struct FCSectorInfoType
{
	uint8 sectorID[IQDK_PROV_FIXEDCLUSTERING_SECTORID_LENGTH];
   int sectorLen;
	struct FCSectorInfoType* next;
};

struct FCSubnetInfoType
{
	uint8 subnetID[IQDK_PROV_FIXEDCLUSTERING_SUBNET_LENGTH];
   int subnetLen;
	boolean entireSubnetSupported;
	struct FCSectorInfoType* supportedSectorsList;
   struct FCSubnetInfoType* next;
};

#define IQDK_PROV_MULTICAST_GROUP_NAME_LENGTH   256
typedef struct 
{
    AECHAR                       groupName[IQDK_PROV_MULTICAST_GROUP_NAME_LENGTH + 1];
    uint32                       multicastIP;
    uint16                       multicastAnnounceLocalPort;
    uint16                       multicastInCalSigLocalPort;
    uint16                       multicastMediaLocalPort;
	 IQDKConfIdType               multicastConfId;
    uint16                       BCMCSFlowPriority;
    boolean   fixedClusteringSupported;
    boolean   premiumUser;
    struct FCSubnetInfoType* supportedSubnetList;
    
} IQDK_ProvMulticastGroupTableEntryType;
typedef struct
{
   IQDKConfIdType                   confId;        
   IQDK_ProvMulticastGSKType        decryptedGSK;
   uint32                           time;    
} IQDK_ProvMulticastGSKTableEntryType;

typedef union
{
   IQDK_ProvDomainEntryType                  domainEntry;
   IQDK_ProvForeignDomainEntryType           foreignDomainEntry;       
   IQDK_ProvAlertEntryType                   alertEntry;
   IQDK_AvailabilityEntry                    preseneceEntry; 
   IQDK_ProvVocoderTableEntryType            vocoderEntry;
   IQDK30_CallRestrictionEntry                 restrictionEntry;
   IQDK_GroupRestrictionTableEntry           talkGroupEntry;
   IQDK_ProvMulticastGroupTableEntryType     multicastGroupEntry;
   IQDK_ProvMulticastGSKTableEntryType       multicastGSKEntry;
} IQDK30_ProvTableEntry;

typedef struct
{
   IQDK30_ProvTableEntry                     * entries;
   uint16                                    numEntries;
} IQDK30_ProvTable;

typedef struct
{
    IQDK_ProvMulticastCnonceType            cnonce;
} IQDK_ProvGSKRequestParamsType;

typedef struct
{
    IQDK_ProvMulticastGSKType               encryptedGSK;
    uint8                                   encryptedTime[IQDK_PROV_MULTICAST_ENCRYPTED_GSK_TIME_LENGTH];
    IQDKConfIdType                          confId;
} IQDK_ProvGSKEntry;

typedef struct
{
    IQDK_ProvGSKEntry                       *gskList;
    uint32                                  count;
    IQDK_ProvMulticastCnonceType            cnonce;
} IQDK_ProvGSKInfoType;


#define INHERIT_IQDKProvision(iname)\
   INHERIT_IQueryInterface(iname);\
   IQDK_ErrorType (*Init)(iname *pMe, AEECLSID appClsID);\
   IQDK_ErrorType (*SyncResults)(iname *po, IQDK_ProvResponseCodeType resp);\
   IQDK_ErrorType (*InformOfUpdate)(iname *po);\
   IQDK_ErrorType (*GetNumProvKeys)(iname *pMe, int16 *numKeys);\
   IQDK_ErrorType (*GetProvEntry_Details)(iname *po, uint16 key, uint16 * dataSize, uint32 * minValue, uint32 * maxValue, ProvTypeEnum* provType);\
   IQDK_ErrorType (*GetProvEntry)(iname *po, uint16 key, uint8 ** value);\
   IQDK_ErrorType (*SetProvEntry)(iname *po, uint16 key, uint8 * value);\
   IQDK_ErrorType (*GetAddressListSize)(iname *pMe, IQDK_ProvAddressListType listType, int16 *listSize);\
   IQDK_ErrorType (*GetProvEntry_AddressList)(iname *pMe, IQDK_ProvAddressListType addressGroup, IQDKAddressListType *addresses);\
   IQDK_ErrorType (*SetProvEntry_AddressList)(iname *pMe, IQDK_ProvAddressListType addressGroup, IQDKAddressListType *addresses);\
   IQDK_ErrorType (*GetTableSize)(iname *pMe, IQDK_ProvTableType tableType, int16 *tableSize);\
   IQDK_ErrorType (*GetProvEntry_Table)(iname *pMe, IQDK_ProvTable * pointerToTable, IQDK_ProvTableType tableType);\
   IQDK_ErrorType (*SetProvEntry_Table)(iname * pMe, IQDK_ProvTable * pointerToTable, IQDK_ProvTableType tableType);\
   IQDK_ErrorType (*GetEventPayload)(iname *pMe,  uint32 interfaceId, uint16 eventType,  IQDK_ProvisionEventPayload **payload)

#define INHERIT_IQDK30Provision(iname)\
   INHERIT_IQDKProvision(iname);\
   IQDK_ErrorType (*Qdk30GetAddressListSize)(iname *pMe, IQDK30_ProvAddressListType listType, int16 *listSize);\
   IQDK_ErrorType (*Qdk30GetProvEntry_AddressList)(iname *pMe, IQDK30_ProvAddressListType addressGroup, IQDK30_ProvListEntry *addresses);\
   IQDK_ErrorType (*Qdk30SetProvEntry_AddressList)(iname *pMe, IQDK30_ProvAddressListType addressGroup, IQDK30_ProvListEntry *addresses);\
   IQDK_ErrorType (*Qdk30GetTableSize)(iname *pMe, IQDK30_ProvTableType tableType, int16 *tableSize);\
   IQDK_ErrorType (*Qdk30GetProvEntry_Table)(iname *pMe, IQDK30_ProvTable * pointerToTable, IQDK30_ProvTableType tableType);\
   IQDK_ErrorType (*Qdk30SetProvEntry_Table)(iname * pMe, IQDK30_ProvTable * pointerToTable, IQDK30_ProvTableType tableType);\
   IQDK_ErrorType (*GetGSKRequestParams)(iname *pMe,IQDK_ProvGSKRequestParamsType *gskRequestParams);\
   IQDK_ErrorType (*UpdateGSKInfo)(iname *pMe, IQDK_ProvGSKInfoType *gskInfo)

typedef struct IQDK30Provision IQDK30Provision;
AEEINTERFACE(IQDK30Provision)
{
   INHERIT_IQDK30Provision(IQDK30Provision);
};
/*-------------------------------------------------------------------
      Function Declarations
-------------------------------------------------------------------*/

/*-----------------------------------------------------------------
       Interface functions
-------------------------------------------------------------------*/


#define IQDK30PROVISION_AddRef(p)                                  AEEGETPVTBL((p),IQDK30Provision)->AddRef((p))
#define IQDK30PROVISION_Release(p)                                 AEEGETPVTBL((p),IQDK30Provision)->Release((p))
#define IQDK30PROVISION_QueryInterface(p,i,p2)                     AEEGETPVTBL((p),IQDK30Provision)->QueryInterface((p),(i),(p2))
#define IQDK30PROVISION_Init(p,clsid)                              AEEGETPVTBL((p),IQDK30Provision)->Init((p),(clsid))
#define IQDK30PROVISION_SyncResults(p,r)                           AEEGETPVTBL((p),IQDK30Provision)->SyncResults((p),(r))
#define IQDK30PROVISION_InformOfUpdate(p)                          AEEGETPVTBL((p),IQDK30Provision)->InformOfUpdate((p))
#define IQDK30PROVISION_GetNumProvKeys(p,r)                        AEEGETPVTBL((p),IQDK30Provision)->GetNumProvKeys((p),(r))
#define IQDK30PROVISION_GetProvEntry_Details(p,k,s,mn,mx, pt)      AEEGETPVTBL((p),IQDK30Provision)->GetProvEntry_Details((p),(k),(s),(mn),(mx), (pt))
#define IQDK30PROVISION_GetProvEntry(p,k,v)                        AEEGETPVTBL((p),IQDK30Provision)->GetProvEntry((p),(k),(v))
#define IQDK30PROVISION_SetProvEntry(p,k,v)                        AEEGETPVTBL((p),IQDK30Provision)->SetProvEntry((p),(k),(v))
#define IQDK30PROVISION_GetAddressListSize(p,l,t)                  AEEGETPVTBL((p),IQDK30Provision)->Qdk30GetAddressListSize((p),(l),(t))
#define IQDK30PROVISION_GetProvEntry_AddressList(p,g,a)            AEEGETPVTBL((p),IQDK30Provision)->Qdk30GetProvEntry_AddressList((p),(g),(a))
#define IQDK30PROVISION_SetProvEntry_AddressList(p,g,a)            AEEGETPVTBL((p),IQDK30Provision)->Qdk30SetProvEntry_AddressList((p),(g),(a))
#define IQDK30PROVISION_GetTableSize(p,t,r)                        AEEGETPVTBL((p),IQDK30Provision)->Qdk30GetTableSize((p),(t),(r))
#define IQDK30PROVISION_GetProvEntry_Table(p,pt,t)                 AEEGETPVTBL((p),IQDK30Provision)->Qdk30GetProvEntry_Table((p),(pt),(t))
#define IQDK30PROVISION_SetProvEntry_Table(p,pt,t)                 AEEGETPVTBL((p),IQDK30Provision)->Qdk30SetProvEntry_Table((p),(pt),(t))
#define IQDK30PROVISION_GetEventPayload(p,i,a,b)                   AEEGETPVTBL((p),IQDK30Provision)->GetEventPayload(p,i,a,b)
#define IQDK30PROVISION_GetGSKRequestParams(p,a)                   AEEGETPVTBL((p),IQDK30Provision)->GetGSKRequestParams(p,a)
#define IQDK30PROVISION_UpdateGSKInfo(p,a)                         AEEGETPVTBL((p),IQDK30Provision)->UpdateGSKInfo(p,a)
#endif // AEEQDK30PROVISION_H30

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================

================================================================================

IQDK30_ProvTableType

Description:
   This data structure specifies the type of Table that is being used.

Definition:
typedef enum
{
   IQDK30_PROV_TABLETYPE_DOMAIN_MAPPING_TABLE,
   IQDK30_PROV_TABLETYPE_FOREIGN_DOMAIN_MAPPING_TABLE,
   IQDK30_PROV_TABLETYPE_VOCODER_MAPPING_TABLE,
   IQDK30_PROV_TABLETYPE_ALERT_MAPPING_TABLE,
   IQDK30_PROV_TABLETYPE_PRESENCE_MAPPING_TABLE,
   IQDK30_PROV_TABLETYPE_CALL_RESTRICTION_TABLE,
   IQDK30_PROV_TABLETYPE_GROUP_RESTRICTION_TABLE,
   IQDK30_PROV_TABLETYPE_MULTICAST_GROUP_MAPPING_TABLE,
   IQDK30_PROV_TABLETYPE_MAX
} IQDK30_ProvTableType;

Members:
 IQDK30_PROV_TABLETYPE_DOMAIN_MAPPING_TABLE: Specifies whether the given table is a domain mapping table. Max size is 64
 IQDK30_PROV_TABLETYPE_FOREIGN_DOMAIN_MAPPING_TABLE: Specifies whether the given table is a foreign domain mapping table. Max size is 256
 IQDK30_PROV_TABLETYPE_VOCODER_MAPPING_TABLE: Specifies whether the given table is a vocoder mapping table. Max size is 16
 IQDK30_PROV_TABLETYPE_ALERT_MAPPING_TABLE: Specifies whether the given table is an alert mapping table. Max size is 32
 IQDK30_PROV_TABLETYPE_PRESENCE_MAPPING_TABLE: Specifies whether the given table is a presence mapping table. Max size is 32
 IQDK30_PROV_TABLETYPE_CALL_RESTRICTION_TABLE: Specifies whether the given table is a carrier level call restriction table. Max size is 20
 IQDK30_PROV_TABLETYPE_GROUP_RESTRICTION_TABLE: Specifies whether the given table is a group restriction table. Max size is 512
 IQDK30_PROV_TABLETYPE_MULTICAST_GROUP_MAPPING_TABLE: Specifies whether the table is multicast group table. Max size is 25

Version:
  Introduced in QDK version 2.0.  Updated in QDK 3.0.

See Also:
  None

================================================================================

IQDKLocalizedAddressListType

Description:
  This data structure is used to store the localized address list

Definition:
struct IQDKLocalizedAddressListType
{
   char* localizedAddress;
   struct IQDKLocalizedAddressListType* next;
};

Members:
  localizedAddress - localized address
  next             - pointer to the next localized address
Version:
  Introduced in QDK version 3.0

See Also:
  IQDK30_ProvListEntry

================================================================================

IQDK30_ProvAddressListType

Description:
  This data structure specifies the type of Address list that is being used.

Definition:
typedef enum
{
   IQDK30_PROV_ADDRESSES_PREDEFINED_GROUPS,
   IQDK30_PROV_ADDRESSES_ALTERNATE_USER,
   IQDK30_PROV_ADDRESSES_PREDEFINED_LOCALIZED_GROUPS
} IQDK30_ProvAddressListType;

Members:
   IQDK30_PROV_ADDRESSES_PREDEFINED_GROUPS: Specifies whether the list is of type Predefined group addresses.
   IQDK30_PROV_ADDRESSES_ALTERNATE_USER: Specifies whether the list is of type QChat User Addresses.
   IQDK30_PROV_ADDRESSES_PREDEFINED_LOCALIZED_GROUPS: Specifies whether the list is of type Localized group addresses.

Version:
  Introduced in QDK version 3.0

See Also:
  None

================================================================================

IQDK30_ProvListEntry

Description:
  This data structure is used to store either an address list format or the localized address list

Definition:
typedef union
{
   IQDKAddressListType                       addressListEntry;
   struct IQDKLocalizedAddressListType*      localizedAddressListEntry;       
} IQDK30_ProvListEntry;

Members:
  addressListEntry             - List of addresses
  localizedAddressListEntry    - List of localized addresses
Version:
  Introduced in QDK version 3.0

See Also:
  IQDKLocalizedAddressListType
  IQDKAddressListType

================================================================================

IQDK_GroupRestrictionTableEntry

Description:
  This data structure is used to store the single entry of group restrictions.

Definition:
typedef struct
{
   IQDKAddressData                  groupName;
   IQDKConfIdType                   confID;
} IQDK_GroupRestrictionTableEntry;

Members:
  groupName - Specifies the group name.
  confID    - Specifies the corresponding confid of the groupname
Version:
  Introduced in QDK version 3.0

See Also:
  IQDK_ProvTableType
================================================================================

IQDK_ProvMulticastGroupTableEntryType

Description:
This data structure defines a single entry of the multicast group table
(IQDK30_PROV_TABLETYPE_MULTICAST_GROUP_MAPPING_TABLE).

Definition:
typedef struct 
{
   AECHAR          groupName[IQDK_PROV_MULTICAST_GROUP_NAME_LENGTH];
   uint32          multicastIP;
   uint16          multicastAnnounceLocalPort;
   uint16          multicastInCalSigLocalPort;
   uint16          multicastMediaLocalPort;
   IQDKConfIdType  multicastConfId;
} IQDK_ProvMulticastGroupTableEntryType;

Members:
   groupName: Name of the multicast closed group (of form group@domain)
   multicastIP: Multicast IP address associated with the closed group
   multicastAnnounceLocalPort: Port associated with multicast group for 
                               receiving call setup signaling
   multicastInCalSigLocalPort: Port associated with multicast group for
                               receiving in-call signalling
   multicastMediaLocalPort:    Port associated with multicast group for
                               receiving media
   multicastConfId:            Conference Id associated with multicast group

Version:
   Introduced in QDK 3.0

================================================================================

IQDK_ProvMulticastGSKType

Description:
Data structure for multicast group session key.

Definition:
typedef uint8      IQDK_ProvMulticastGSKType[IQDK_PROV_MULTICAST_GSK_LENGTH];

Version:
   Introduced in QDK 3.0

================================================================================

IQDK_ProvMulticastCnonceType

Description:
Data structure for nonce value used during multicast group session key provisioning.

Version:
   Introduced in QDK 3.0

================================================================================

IQDK_ProvGSKRequestParamsType

Description:
Data structure filled out by QChat client when IQDK30PROVISION_GetGSKRequestParams
is called.  These values are needed for over-the-air multicast group session key 
provisioning by the carrier provisioning server.

Definition:
typedef struct
{
   IQDK_ProvMulticastCnonceType            cnonce;
} IQDK_ProvGSKRequestParamsType;

Members:
   cnonce: nonce value generated by QChat client.  

Version:
   Introduced in QDK 3.0

================================================================================
IQDK_ProvGSKEntry

Description:
This data structure is used within IQDK_ProvGSKInfoType.  It defines one
entry of the group session key list that is encapsulated in IQDK_ProvGSKInfoType. 
During multicast group session key provisioning, a list of encrypted GSKs is 
passed to the client.

Definition:
typedef struct
{
   IQDK_ProvMulticastGSKType               encryptedGSK;
   uint8                                   encryptedTime[IQDK_PROV_MULTICAST_ENCRYPTED_GSK_TIME_LENGTH];
   IQDKConfIdType                          confId;
} IQDK_ProvGSKEntry;

Members:
   encryptedGSK:     encrypted multicast group session key
   encryptedTime:    encrypted time at which group session key activates
   confId:           confId for the group this GSK is for

Version:
   Introduced in QDK 3.0

================================================================================
IQDK_ProvGSKInfoType

Description:
During multicast group session key provisioning, a list of encrypted GSKs
and activation times is passed to the QChat client in this data structure using the 
IQDK30PROVISION_UpdateGSKInfo API.
The carrier provisioning server should send a packet of this form to the handset
during over-the-air GSK provisioning.

Definition:
typedef struct
{
   IQDK_ProvGSKEntry                       *gskList;
   uint32                                  count;
   IQDK_ProvMulticastCnonceType            cnonce;
} IQDK_ProvGSKInfoType;

Members:
   gskList:       list of {encrypted GSK, encrypted GSK activation time, group conference ID}
   count:         number of entries in gskList
   cnonce:        nonce value used for over-the-air GSK provisioning.

Version:
   Introduced in QDK 3.0           

================================================================================
IQDK_ProvMulticastGSKTableEntryType

Description:
This data structure defines a single entry of the multicast group session key 
table.  This table is maintained by the client for keeping track of multicast GSKs.

Definition:
typedef struct
{
   IQDKConfIdType                   confId;        
   IQDK_ProvMulticastGSKType        decryptedGSK;
   uint32                           time;    
} IQDK_ProvMulticastGSKTableEntryType;

Members:
   confId:        conference Id for the multicast group
   decryptedGSK:  multicast group session key for the group
   time:          time at which the group session key becomes active

Version:
   Introduced in QDK 3.0

================================================================================
IQDK30_ProvTableEntry

Description:
This data structure defines the list of tables that the QChat Client supports for provisioning.

Definition:
typedef union
{
   IQDK_ProvDomainEntryType                  domainEntry;
   IQDK_ProvForeignDomainEntryType           foreignDomainEntry;
   IQDK_ProvAlertEntryType                   alertEntry;
   IQDK_AvailabilityEntry                    preseneceEntry;   
   IQDK_ProvVocoderTableEntryType            vocoderEntry;
   IQDK_CallRestrictionEntry                 restrictionEntry;
   IQDK_GroupRestrictionTableEntry           talkGroupEntry;
   IQDK_MulticastGroupTableEntryType         multicastGroupEntry;
   IQDK_ProvMulticastGSKTableEntryType       multicastGSKEntry;
} IQDK_ProvTableEntry;

Members:
   domainEntry:         Specifies whether the domain mapping table is going to be used.
   foreignDomainEntry:  Specifies whether the foreign domain mapping table is going to be used.
   alertEntry:          Specifies whether the alert mapping table is going to be used.
   presencecwEntry:     Specifies whether the presence mapping table is going to be used.
   vocoderEntry:        Specifies whether the vocoder mapping table is going to be used.
   restrictionEntry:    Specifies whether the call restriction mapping table is going to be used.
   talkGroupEntry:      Specifies whether the group restrictions table is going to be used.
   multicastGroupEntry: Specifies whether the multicast group mapping table is going to be used.
   multicastGSKEntry:   Specifies whether the multicast GSK mapping table is going to be used.    

Version:
  Introduced in QDK version 2.0.  Updated in QDK 3.0.

See Also:
  None

================================================================================

IQDK_ProvisionSyncPayload

Description:
Data structure for payload associated with IQDK_EVT_PROVISION_SYNC event sent by
the QChat client.

=================================================================================

IQDK_ProvisionEventPayload

Description:
Data structure for payload associated with a provisioning event sent by the QChat
client.

=================================================================================

================================================================================
  INTERFACES   DOCUMENTATION
================================================================================

Interface Name: IQDK30Provision

Description:
The IQDK30Provision interface provides a set of API methods allowing a BREW
application to learn of and update the QChat Client's provisioning info.

To access this interface, an application must have the corresponding 
privilege in its MIF file.

The following header file is required:~
AEEQDKProvision.h

================================================================================

IQDK30PROVISION_AddRef()

Description:
   This method increments the reference count of the IQDK30Provision Interface 
   object. This allows the object to be shared by multiple callers. 
   The object is freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IQDK30PROVISION_AddRef(IQDK30Provision *po)

Parameters:
   po : Pointer to the IQDK30Provision Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDK30PROVISION_Release()

================================================================================

IQDK30PROVISION_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQDK30PROVISION_Release(IQDK30Provision *po)

Parameters:
   po: Pointer to the IQDK30Provision Interface object

Return Value:
   Decremented reference count for the object. The object has been freed
   and is no longer valid if 0 (zero) is returned.

Comments:
   None

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDK30PROVISION_AddRef()

================================================================================

IQDK30PROVISION_Init

Description:
   This function associates application class id with the given instance of
   IQDK30Provision interface. Any application that wishes to use IQDK30Provision
   interface will have to invoke this method after creating instance
   of IQDK30Provision interface and before calling any other IQDK30Provision
   interface methods.

Prototype:
   IQDK_ErrorType Init
   (
      IQDK30Provision *pMe,
      AEECLSID classId
   )

Parameters:
   pMe            :  Pointer to the IQDK30Provision Interface object.
   classId        :  Class id of application that wishes to use the IQDK30Provision
                     interface.

Return Value:
   SUCCESS  : operation successful
   EBADCLASS: pMe pointer is NULL

Comments:

Version:
   Introduced in QDK v 2.0

See Also:
   none

================================================================================

IQDK30PROVISION_SyncResults

Description:
   This function is used by the QDK Provisioning application to notify the QChat Client
   that it has set all the provisionable items along with the end result of the 
   synchronization.

Prototype:
   IQDK_ErrorType IQDK30PROVISION_SyncResults(IQDK30Provision *po,
                                            IQDK_ProvResponseCodeType resp);

Parameters:
   po:  Pointer to the IQDK30Provision Interface object.
   resp: Result of the synchronization that the QDK provisioning application sends to
      the QChat Client.

Return Value:
   SUCCESS: operation successful
   EBADCLASS : po pointer is NULL
   EBADSTATE : client is not willing to accept provisioning updates
   EQDKOUTOFRANGE: resp parameter is invalid
   EQDKNOTINITIALIZED: IQDK30PROVISION_Init() was not called first

Comments:
   None

Version:
   Introduced in QDK v 2.0

================================================================================

IQDK30PROVISION_InformOfUpdate

Description:
   This function is used by the QDK Provisioning Applciation to inform the QChat Client 
   of possible updates that the QDK Provisioning Application has. The QChat Client can 
   can respond to this by sending an event IQDK_EVT_PROVISION_SYNC.

Prototype:
   IQDK_ErrorType (*InformOfUpdate)(IQDK30Provision *po);

Parameters:
   po:  Pointer to the IQDK30Provision Interface object.

Return Value:
   SUCCESS: operation successful
   EBADCLASS : po pointer is NULL
   EQDKNOTINITIALIZED : IQDK30PROVISION_Init() was not called first

Comments:
   None

Version:
   Introduced in QDK v 2.0

================================================================================

IQDK30PROVISION_SetProvEntry_Table

Description:
   This function sets all the entries of a particular table in the QChat client
   based on the values specified in the field tableType.

Prototype:
   IQDK_ErrorType IQDK30PROVISION_SetProvEntry_Table(IQDK30Provision *pMe,
                                                   IQDK_ProvTableEntry* pointerToTable,
                                                   IQDK_ProvTableType tableType);

Parameters:
   pMe:  Pointer to the IQDK30Provision Interface object.
   pointerToTable : pointer to the table to be set.
   tableType: One of the types of tables listed in the enumeration IQDK_ProvTableType

Return Value:
   SUCCESS: operation successful
   EBADCLASS : po pointer is NULL
   EQDKNOTINITIALIZED : IQDK30PROVISION_Init() was not called first
   EBADSTATE : client is not willing to accept provisioning updates
   EBADPARAM: pointerToTable is NULL or tableType is invalid/ out of range
   EQDKBADVALUE : when setting a user level call restriction with type set to allow always
   EQDKTOOMANYENTRIES : Too many entries specified - exceeds the max limit for the table type

Comments:
   None

Version:
   Introduced in QDK v 2.0

================================================================================

IQDK30PROVISION_GetProvEntry_Table

Description:
   This function returns all the entries of a particular table to the QDK Provisioning application
   based on the value specified in the field tableType.

Prototype:
   IQDK_ErrorType IQDK30PROVISION_GetProvEntry_Table(IQDK30Provision *pMe,
                                                   IQDK_ProvTableEntry* pointerToTable,
                                                   IQDK_ProvTableType tableType);

Parameters:
   pMe:  Pointer to the IQDK30Provision Interface object.
   pointerToTable : Pointer to the table that the QChat Client fills with the table values for the QDK provisioning application.
   tableType: One of the types of tables listed in the enumeration IQDK_ProvTableType

Return Value:
   SUCCESS: operation successful
   EBADCLASS : po pointer is NULL
   EBADPARAM: pointerToTable is NULL or tableType is invalid/out of range
 EQDKNOTINITIALIZED: if the IQDK30PROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. 
    EQDKCOREOFLINE:    If the state received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.
                       
Comments:
   None

Version:
   Introduced in QDK v 2.0

================================================================================

IQDK30PROVISION_SetProvEntry_AddressList()

Description:
   This function is used by the QDK provisioning application to set a list of 
   provisionable QDK addresses in the QChat Client. 

Prototype:
   IQDK_ErrorType IQDK30PROVISION_SetProvEntry_AddressList(IQDK30Provision *pMe,
                                                         IQDK30_ProvAddressListType addressGroup,
                                                         IQDK30_ProvListEntry *addresses);

Parameters:
   pMe:  Pointer to the IQDK30Provision Interface object.
   addressGroup: One of the types of Lists accepted by the QChat Client as listed in IQDK30_ProvAddressListType.
   addresses: Pointer to a list of QChat addresses that the QChat Client accepts from the QDK provisioning application.

Return Value:
   SUCCESS: operation successful
   EBADCLASS : po pointer is NULL
   EQDKNOTINITIALIZED : IQDK30PROVISION_Init() was not called first
   EBADSTATE : client is not willing to accept provisioning updates
   EBADPARAM: invalid addresses pointer.
   EQDKOUTOFRANGE : addressGroup is out of range.
   EQDKINVALIDQCHATADDRESS : if the primary QChat user address being set is domainless.
   EQDKTOOMANYENTRIES : Too many entries specified - exceeds the max limit for the addresslist.

Comments:
   None

Version:
   Introduced in QDK v 2.0

================================================================================

IQDK30PROVISION_GetProvEntry_AddressList()

Description:
   This function is used by the QDK provisioning application to get a list of 
   provisionable QDK addresses from the QChat Client. 

Prototype:
   IQDK_ErrorType IQDK30PROVISION_GetProvEntry_AddressList(IQDK30Provision *pMe,
                                                         IQDK30_ProvAddressListType addressGroup,
                                                         IQDK30_ProvListEntry *addresses);
Parameters:
   pMe:  Pointer to the IQDK30Provision Interface object.
   addressGroup: One of the types of Lists accepted by the QChat Client as listed in IQDK30_ProvAddressListType.
   addresses: Pointer to a list of QChat addresses that the QChat Client fills with the list of QDK addresses for the QDK provisioning application.

Return Value:
   SUCCESS: operation successful
   EBADCLASS: pMe pointer is NULL
   EBADPARAM: addresses pointer is NULL
   EQDKOUTOFRANGE : addressGroup is out of range.
   EQDKNOTINITIALIZED: if the IQDK30PROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. If the state
                       received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.

Comments:
   None

Verions:
   Introduced in QDK v 2.0

================================================================================

IQDK30PROVISION_GetNumProvKeys

Description:
   This function is used by the QDK provisioning application to get the number
   of custom provisionable items

Prototype:
   IQDK_ErrorType IQDK30PROVISION_GetNumProvKeys(IQDK30Provision *pMe, int16 *numKeys);

Parameters:
   pMe: Pointer to the IQDK30Provision Interface object
   numKeys : Pointer to the number of provisionable items

Return Value:
   SUCCESS: operation successful
   EBADCLASS: pMe pointer is NULL
   EBADPARAM : numKeys pointer is NULL.
   EQDKNOTINITIALIZED: if the IQDK30PROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. 
    EQDKCOREOFLINE:    If the state received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.
Comments:
   The caller is responsible for allocating memory for parameter numKeys.

Versions:
   Introduced in QDK v 2.0

================================================================================

IQDK30PROVISION_GetProvEntry_Details

Description:
   This function is used to get the details of the custom provisionable entry given the key

Prototype:
   IQDK_ErrorType IQDK30PROVISION_GetProvEntry_Details(IQDK30Provision *po,
                                                     uint16 key,
                                                     uint16 * dataSize,
                                                     uint32 * minValue,
                                                     uint32 * maxValue
                                                     ProvTypeEnum* provType);

Parameters:
   po: Pointer to the IQDK30Provision Interface object
   key: provisionable item's key for which the QDK application requires details
   dataSize: pointer to the size of the provisionable entry value
   minValue: pointer to the minimum value for the provisionable entry
   maxValue: pointer to the maximum value for the provisionable entry
    provType: pointer to the key type (boolean, int or string)

Return Value:
   SUCCESS: operation successful
   EBADCLASS: po pointer is NULL
   EBADPARAM: dataSize, minValue or maxValue pointer is NULL.
   EQDKOUTOFRANGE : invalid key
   EQDKNOTINITIALIZED: if the IQDK30PROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. 
    EQDKCOREOFLINE:    If the state received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.

Comments:
   The caller is responsible for allocating memory for parameters dataSize,
   minValue, maxValue, and provType

Versions:
   Introduced in QDK v 2.0

================================================================================

IQDK30PROVISION_GetProvEntry

Description:
   This function is used to get the value associated with key for the custom provisioning item

Prototype:
   IQDK_ErrorType IQDK30PROVISION_GetProvEntry)(IQDK30Provision *po,
                                  uint16 key,
                                  uint8 ** value);
Parameters:
   po: Pointer to the IQDK30Provision Interface object
   key: provisionable item's key for which the QDK application requires the value
   value: the provisionable entry's value is returned via this parameter

Return Value:
   SUCCESS: operation successful
   EBADCLASS: po pointer is NULL
   EBADPARAM: value pointer is NULL
   EQDKOUTOFRANGE : key is out of range.
   EQDKNOTINITIALIZED: if the IQDK30PROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. 
    EQDKCOREOFLINE:    If the state received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.
Comments:
   The caller is responsible for allocating & deallocating memory for the value

Versions:
   Introduced in QDK v 2.0

================================================================================

IQDK30PROVISION_SetProvEntry

Description:
   This function is used to set the value for the custom provisioning item identified by the key.

Prototype:
   IQDK_ErrorType IQDK30PROVISION_SetProvEntry(IQDK30Provision *po,
                                            uint16 key,
                                            uint8 * value);
Parameters:
   po: Pointer to the IQDK30Provision Interface object
   key: provisionable item's key for which the QDK application wishes to set the value
   value: the value to be set for the provisioning item identified by the key

Return Value:
   SUCCESS: operation successful
   EBADCLASS : po pointer is NULL
   EQDKNOTINITIALIZED : IQDK30PROVISION_Init() was not called first
   EBADSTATE : client is not willing to accept provisioning updates
   EQDKOUTOFRANGE: key is not between min and max values

Comments:
   The caller is responsible for allocating & deallocating memory for the value

Versions:
   Introduced in QDK v 2.0

=================================================================================

IQDK30PROVISION_GetAddressListSize

Description:
   This function is used to get the number of predefined group addresses or localized group addresses or alternate
   user addresses depending on the Address List Type passed

Prototype:
   IQDK_ErrorType IQDK30PROVISION_GetAddressListSize(IQDK30Provision *pMe,
                               IQDK30_ProvAddressListType listType, int16 *listSize);

Parameters:
   pMe: Pointer to the IQDK30Provision Interface object
   listType: Can either be predefined or localized group or alternate user addresses
   listSize : Pointer to the number of predefined or localized group or alternate user addresses

Return Value:
   SUCCESS: operation successful
   EBADCLASS : pMe pointer is NULL
   EQDKOUTOFRANGE: listType is out of range
   EBADPARAM : listSize pointer is NULL.
   EQDKNOTINITIALIZED: if the IQDK30PROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. 
    EQDKCOREOFLINE:    If the state received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.
Comments:
   The caller is responsible for allocating memory for listSize.

Versions:
   Introduced in QDK v 2.0

=================================================================================

IQDK30PROVISION_GetTableSize

Description:
   This function is used to get the number of entries for the table defined by
   IQDK_ProvTableType

Prototype:
   IQDK_ErrorType IQDK30PROVISION_GetTableSize(IQDK30Provision *pMe,
                         IQDK_ProvTableType tableType, int16 *tableSize);

Parameters:
   pMe: Pointer to the IQDK30Provision Interface object
   tableType: can be domain, foreign domain, presence, alert, call restrictions or presence table
   tableSize: Pointer to the number of entries in the table defined by tableType

Return Value:
   SUCCESS: operation successful
   EBADCLASS : pMe pointer is NULL
   EBADPARAM : tableSize pointer is NULL or tableType is invalid/out of range
   EQDKNOTINITIALIZED: if the IQDK30PROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. 
    EQDKCOREOFLINE:    If the state received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.
Comments:
   The caller is responsible for allocating memory for tableSize.

Versions:
   Introduced in QDK v 2.0

==================================================================================

Function: IQDK30PROVISION_GetEventPayload

Description:
   The event payload is not sent as part of the event.
   The application can get the event payload through the GetEventPayload API. This method allows backward
   compatibility. The event payload data structure could change from one QChat client release to the next. But
   the event does not have to be changed.
   
   This interface returns the payload of any of the events requested in the eventType.
   The payload returned is for any one of the following events:
   
   typedef union
   {
      IQDK_ProvisionSyncPayload                provisionSyncPayload;
   } IQDK_ProvisionEventPayload;

   The GetEventPayload allows QDK applications to request the event information
   explicitly from the QChat Client, for a specific event type and for a specific
   version of that event.
   
Prototype:
   IQDK_ErrorType IQDK30PROVISION_GetEventPayload(
         IQDK30Provision *pMe,
         uint32 interfaceId,
         uint16 eventType,
        IQDK_ProvisionEventPayload **payload)
        
Parameters:

   pMe :		    Pointer to the IQDK30Provision Interface object.
   interfaceId:	    ID number of the Provision interface
   eventType :	    This field specifies the type of event to retrieve.  The
                value specified in the eventType field is used to select one
               of the event structures within the payload union.
   payload:		    This field is populated by the payload of the event 
               associated with the eventType. While the 'payload' pointer 
                   itself must not be NULL, the pointer pointed to by the payload
                   pointer must be NULL. This is the event payload that will be 
                   allocated by the QChat Client and deleted by the QDK Application
                   calling this API.
   
Return Value:

   SUCCESS:		        Successful retrieval of the event payload.
   EQDKNOTINITIALIZED:	pMe is not initialised.
   EBADPARM:		    If the input parameters are not valid.
   EFAILED:		        Generic failure

Comments:
   The payload pointer is deleted by the QDK application that requests the information.
   
Side Effects:
   None

Version:
   Introduced in QDK v 2.0

See Also:

=================================================================================

IQDK30PROVISION_GetGSKRequestParams

Description:
   This function is used to request parameters from the QChat client that 
   are needed for over-the-air multicast group session key provisioning.

Prototype:
   IQDK_ErrorType IQDK30PROVISION_GetGSKRequestParams(IQDK30Provision *pMe,
                                                    IQDK_ProvGSKRequestParamsType *gskRequestParams);

Parameters:
   pMe:                 Pointer to the IQDK30Provision Interface object
   gskRequestParams:    Data structure filled out by client with parameters
                        needed for over-the-air GSK provisioning. 

Return Value:
   SUCCESS:   Operation successful
   EBADSTATE: The QChat client was not in the correct state when this API was called.  IQDK30PROVISION_InformOfUpdate must
              be called prior to using this API.
   EBADCLASS: pMe pointer is NULL
   EBADPARAM: gskRequestParams is NULL.
   EQDKNOTINITIALIZED: if the IQDK30PROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. If the state
                       received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.
   EFAILED: Other failure.

Version:
   Introduced in QDK 3.0

==================================================================================

IQDK30PROVISION_UpdateGSKInfo

Description:
   This function is used to update the QChat client with new multicast group
   session keys received from the carrier provisioning server.  This information
   is received encrypted from the carrier provisioning server and is decrypted and stored
   by the QChat client.


Prototype:
   IQDK_ErrorType IQDK30PROVISION_UpdateGSKInfo(IQDK30Provision *pMe, 
                                              IQDK_ProvGSKInfoType *gskInfo);

Parameters:
   pMe:       Pointer to the IQDK30Provision Interface object
   gskInfo:   Data structure with the list of encrypted GSK data
    

Return Value:
   SUCCESS:   Operation successful
   EBADSTATE: The QChat client was not in the correct state when this API was called.  IQDK30PROVISION_InformOfUpdate must
              be called prior to using this API.
   EBADCLASS: pMe pointer is NULL
   EQDKNOTINITIALIZED: if the IQDK30PROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. If the state
                       received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.
   EFAILED: Other failure.

Version:
   Introduced in QDK 3.0

==================================================================================

*/
