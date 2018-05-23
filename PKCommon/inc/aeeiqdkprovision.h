#ifndef AEEQDKPROVISION_H
#define AEEQDKPROVISION_H
/*=============================================================================
 
FILE: AEEIQDKProvision.h

PROVISIONS: The IQDKProvision interface provides functions to manage QChat Client
          provisioning.

GENERAL DESCRIPTION:

PUBLIC CLASSES AND STATIC FUNCTIONS:
   IQDKProvision
   IQDKPROVISION_AddRef
   IQDKPROVISION_Release
   IQDKPROVISION_Init
   IQDKPROVISION_SyncResults
   IQDKPROVISION_InformOfUpdate
   IQDKPROVISION_GetNumProvKeys
   IQDKPROVISION_GetProvEntry_Details
   IQDKPROVISION_GetProvEntry
   IQDKPROVISION_SetProvEntry
   IQDKPROVISION_GetAddressListSize
   IQDKPROVISION_GetProvEntry_AddressList
   IQDKPROVISION_SetProvEntry_AddressList
   IQDKPROVISION_GetTableSize
   IQDKPROVISION_GetProvEntry_Table
   IQDKPROVISION_SetProvEntry_Table

        Copyright © 2007 QUALCOMM Incorporated.
               All Rights Reserved.
            QUALCOMM Proprietary
=============================================================================*/

/*=============================================================================
                     INCLUDE FILES FOR MODULE
=============================================================================*/
#include "AEE.h"
#include "AEEQDKDataTypes.h"
#include "AEEVocoder.h"
#include "AEEQDKAddress.h"
#include "AEEIQDKProvision.bid"	//Backward_Comp_START_Apr_19

/*=============================================================================
                    CLASS DEFINITIONS
=============================================================================*/

#define AEEIID_IQDKPROVISION 0x01052f34 //Interface ID for 2.0 QueryInterface

#define IQDK_PROV_AUTHKEY_LENGTH                            20

typedef struct IQDKProvision IQDKProvision;

typedef enum
{
   IQDK_PROV_RESPCODE_SUCCESSFUL_SYNC,
   IQDK_PROV_RESPCODE_PARTIAL_SYNC,
   IQDK_PROV_RESPCODE_UNKNOWN,
   IQDK_PROV_RESPCODE_NO_UPDATES,
   IQDK_PROV_RESPCODE_NO_DELTA,
   IQDK_PROV_RESPCODE_MAX
} IQDK_ProvResponseCodeType;

typedef enum
{
   IQDK_PROV_ADDRESSES_PREDEFINED_GROUPS,
   IQDK_PROV_ADDRESSES_ALTERNATE_USER,
   IQDK_PROV_ADDRESSES_MAX
} IQDK_ProvAddressListType;

typedef enum
{
   IQDK_PROV_TABLETYPE_DOMAIN_MAPPING_TABLE,
   IQDK_PROV_TABLETYPE_FOREIGN_DOMAIN_MAPPING_TABLE,
   IQDK_PROV_TABLETYPE_VOCODER_MAPPING_TABLE,
   IQDK_PROV_TABLETYPE_ALERT_MAPPING_TABLE,
   IQDK_PROV_TABLETYPE_PRESENCE_MAPPING_TABLE,
   IQDK_PROV_TABLETYPE_CALL_RESTRICTION_TABLE,
   IQDK_PROV_TABLETYPE_MAX
} IQDK_ProvTableType;

typedef AECHAR     IQDK_ProvDomainEntryType[IQDK_MAX_DOMAIN_SIZE];
typedef AECHAR     IQDK_ProvForeignDomainEntryType[IQDK_MAX_DOMAIN_SIZE];
typedef AECHAR     IQDK_ProvAlertEntryType[IQDK_ALERT_MAX_STRING_SIZE];
typedef AECHAR     IQDK_ProvAuthKey[IQDK_PROV_AUTHKEY_LENGTH];

typedef struct
{
   uint16                                    playoutBufferSize;
   uint8                                     vocoderId;
   uint8                                     vocoderMode;
   uint8                                     interleavingFactor;
   uint8                                     bundlingFactor;
   DataRateType                              minOperatingRate;
   DataRateType                              maxOperatingRate;
} IQDK_ProvVocoderTableEntryType;

typedef union
{
   IQDK_ProvDomainEntryType                  domainEntry;
   IQDK_ProvForeignDomainEntryType           foreignDomainEntry;       
   IQDK_ProvAlertEntryType                   alertEntry;
   IQDK_AvailabilityEntry                    preseneceEntry; 
   IQDK_ProvVocoderTableEntryType            vocoderEntry;
   IQDK_CallRestrictionEntry                 restrictionEntry;
} IQDK_ProvTableEntry;

typedef struct
{
   IQDK_ProvTableEntry                     * entries;
   uint16                                    numEntries;
} IQDK_ProvTable;

typedef struct
{
   uint32                                 * provisioningTimer;
} IQDK_ProvisionSyncPayload;

typedef union
{
   IQDK_ProvisionSyncPayload                provisionSyncPayload;
} IQDK_ProvisionEventPayload;

typedef enum
{
   PROV_BOOL,
   PROV_UINT32,
   PROV_STR,
   PROV_BIN,
   PROV_LAST
} ProvTypeEnum;

/*-------------------------------------------------------------------
      Function Declarations
-------------------------------------------------------------------*/

/*-----------------------------------------------------------------
       Interface functions
-------------------------------------------------------------------*/

AEEINTERFACE(IQDKProvision)
{
   INHERIT_IQueryInterface(IQDKProvision);

   IQDK_ErrorType (*Init)(IQDKProvision *pMe, AEECLSID appClsID);

   IQDK_ErrorType (*SyncResults)(IQDKProvision *po,
                                 IQDK_ProvResponseCodeType resp);

   IQDK_ErrorType (*InformOfUpdate)(IQDKProvision *po);


   IQDK_ErrorType (*GetNumProvKeys)(IQDKProvision *pMe, int16 *numKeys);
   IQDK_ErrorType (*GetProvEntry_Details)(IQDKProvision *po,
                                          uint16 key,
                                          uint16 * dataSize,
                                          uint32 * minValue,
                                          uint32 * maxValue,
                                          ProvTypeEnum* provType);
   IQDK_ErrorType (*GetProvEntry)(IQDKProvision *po,
                                  uint16 key,
                                  uint8 ** value);
   IQDK_ErrorType (*SetProvEntry)(IQDKProvision *po,
                                  uint16 key,
                                  uint8 * value);

   IQDK_ErrorType (*GetAddressListSize)(IQDKProvision *pMe,
                                        IQDK_ProvAddressListType listType, int16 *listSize);
   IQDK_ErrorType (*GetProvEntry_AddressList)(IQDKProvision *pMe,
                                              IQDK_ProvAddressListType addressGroup,
                                              IQDKAddressListType *addresses);

   IQDK_ErrorType (*SetProvEntry_AddressList)(IQDKProvision *pMe,
                                              IQDK_ProvAddressListType addressGroup,
                                              IQDKAddressListType *addresses);

   IQDK_ErrorType (*GetTableSize)(IQDKProvision *pMe,
                                  IQDK_ProvTableType tableType, int16 *tableSize);
   IQDK_ErrorType (*GetProvEntry_Table)(IQDKProvision *pMe,
                                        IQDK_ProvTable * pointerToTable,
                                        IQDK_ProvTableType tableType);
   IQDK_ErrorType (*SetProvEntry_Table)(IQDKProvision * pMe,
                                        IQDK_ProvTable * pointerToTable,
                                        IQDK_ProvTableType tableType);
   IQDK_ErrorType (*GetEventPayload)(IQDKProvision *pMe, 
                                     uint32 interfaceId, uint16 eventType, 
                                     IQDK_ProvisionEventPayload **payload);
};

#define IQDKPROVISION_AddRef(p)                                  AEEGETPVTBL((p),IQDKProvision)->AddRef((p))
#define IQDKPROVISION_Release(p)                                 AEEGETPVTBL((p),IQDKProvision)->Release((p))
#define IQDKPROVISION_QueryInterface(p,i,p2)                     AEEGETPVTBL((p),IQDKProvision)->QueryInterface((p),(i),(p2))
#define IQDKPROVISION_Init(p,clsid)                              AEEGETPVTBL((p),IQDKProvision)->Init((p),(clsid))
#define IQDKPROVISION_SyncResults(p,r)                           AEEGETPVTBL((p),IQDKProvision)->SyncResults((p),(r))
#define IQDKPROVISION_InformOfUpdate(p)                          AEEGETPVTBL((p),IQDKProvision)->InformOfUpdate((p))
#define IQDKPROVISION_GetNumProvKeys(p,r)                        AEEGETPVTBL((p),IQDKProvision)->GetNumProvKeys((p),(r))
#define IQDKPROVISION_GetProvEntry_Details(p,k,s,mn,mx, pt)      AEEGETPVTBL((p),IQDKProvision)->GetProvEntry_Details((p),(k),(s),(mn),(mx), (pt))
#define IQDKPROVISION_GetProvEntry(p,k,v)                        AEEGETPVTBL((p),IQDKProvision)->GetProvEntry((p),(k),(v))
#define IQDKPROVISION_SetProvEntry(p,k,v)                        AEEGETPVTBL((p),IQDKProvision)->SetProvEntry((p),(k),(v))
#define IQDKPROVISION_GetAddressListSize(p,l,t)                  AEEGETPVTBL((p),IQDKProvision)->GetAddressListSize((p),(l),(t))
#define IQDKPROVISION_GetProvEntry_AddressList(p,g,a)            AEEGETPVTBL((p),IQDKProvision)->GetProvEntry_AddressList((p),(g),(a))
#define IQDKPROVISION_SetProvEntry_AddressList(p,g,a)            AEEGETPVTBL((p),IQDKProvision)->SetProvEntry_AddressList((p),(g),(a))
#define IQDKPROVISION_GetTableSize(p,t,r)                        AEEGETPVTBL((p),IQDKProvision)->GetTableSize((p),(t),(r))
#define IQDKPROVISION_GetProvEntry_Table(p,pt,t)                 AEEGETPVTBL((p),IQDKProvision)->GetProvEntry_Table((p),(pt),(t))
#define IQDKPROVISION_SetProvEntry_Table(p,pt,t)                 AEEGETPVTBL((p),IQDKProvision)->SetProvEntry_Table((p),(pt),(t))
#define IQDKPROVISION_GetEventPayload(p,i,a,b)                   AEEGETPVTBL((p),IQDKProvision)->GetEventPayload(p,i,a,b)


#endif // AEEQDKPROVISION_H

/*==============================================================================
  DATA STRUCTURE DOCUMENTATION
================================================================================

================================================================================

IQDK_ProvAddressListType

Description:
  This data structure specifies the type of Address list that is being used.

Definition:
typedef enum
{
   IQDK_PROV_PREDEFINED_GROUPS,
   IQDK_PROV_ALTERNATE_USER_ADDRESSES
} IQDK_ProvAddressListType;

Members:
   IQDK_PROV_PREDEFINED_GROUPS: specifies whether the list is of type Predefined group addresses.
   IQDK_PROV_ALTERNATE_USER_ADDRESSES: Specifies whether the list is of type QChat User Addresses.

Version:
  Introduced in QDK version 2.0

See Also:
  None

================================================================================

IQDK_ProvTableType

Description:
   This data structure specifies the type of Table that is being used.

Definition:
typedef enum
{
   IQDKPROV_TABLETYPE_DOMAIN_MAPPING_TABLE,
   IQDKPROV_TABLETYPE_FOREIGN_DOMAIN_MAPPING_TABLE,
   IQDKPROV_TABLETYPE_VOCODER_MAPPING_TABLE,
   IQDKPROV_TABLETYPE_ALERT_MAPPING_TABLE,
   IQDKPROV_TABLETYPE_PRESENCE_MAPPING_TABLE,
   IQDKPROV_TABLETYPE_CALL_RESTRICTION_TABLE
} IQDK_ProvTableType;

Members:
 IQDKPROV_TABLETYPE_DOMAIN_MAPPING_TABLE: Specifies whether the given table is a domain mapping table. Max size is 64
 IQDKPROV_TABLETYPE_FOREIGN_DOMAIN_MAPPING_TABLE: Specifies whether the given table is a foreign domain mapping table. Max size is 256
 IQDKPROV_TABLETYPE_VOCODER_MAPPING_TABLE: Specifies whether the given table is a vocoder mapping table. Max size is 16  
 IQDKPROV_TABLETYPE_ALERT_MAPPING_TABLE: Specifies whether the given table is an alert mapping table. Max size is 32
 IQDKPROV_TABLETYPE_PRESENCE_MAPPING_TABLE: Specifies whether the given table is a presence mapping table. Max size is 32
 IQDKPROV_TABLETYPE_CALL_RESTRICTION_TABLE: Specifies whether the given table is a carrier level call restriction table. Max size is 20

Version:
  Introduced in QDK version 2.0

See Also:
  None

================================================================================

IQDK_ProvDomainEntryType

Description:
  This data structure is used to store the Provisionable Domain String.

Definition:
typedef AECHAR IQDK_ProvDomainEntryType[IQDK_MAX_DOMAIN_SIZE]

Members:
  None

Version:
  Introduced in QDK version 2.0

See Also:
  None

================================================================================

IQDK_ProvForeignDomainEntryType

Description:
  This data structure is used to store the Provisionable Foreign Domain String.

Definition:
typedef AECHAR IQDK_ProvForeignDomainEntryType[IQDK_MAX_DOMAIN_SIZE]

Members:
  None

Version:
  Introduced in QDK version 2.0

See Also:
  None

================================================================================

IQDK_ProvAlertEntryType

Description:
  This data structure is used to store the Provisionable Alerts as a String.

Definition:
typedef AECHAR IQDK_ProvAlertEntryType[IQDK_ALERT_MAX_STRING_SIZE]

Members:
  None

Version:
  Introduced in QDK version 2.0

See Also:
  None

================================================================================

IQDK_ProvPresenceEntryType

Description:
  This data structure is used to store the Provisionable Presences as a String.

Definition:
typedef AECHAR IQDK_ProvPresenceEntryType[IQDK_MAX_PRESENCE_STRING]

Members:
  None

Version:
  Introduced in QDK version 2.0

See Also:
  None

================================================================================

IQDK_ProvAuthKey

Description:
  This data structure is used to store the Provisionable Authentication Key as String.

Definition:
typedef AECHAR IQDK_ProvAuthKey[IQDK_PROV_AUTHKEY_LENGTH]

Members:
  None

Version:
  Introduced in QDK version 2.0

See Also:
  None

================================================================================

IQDK_ProvVocoderTableEntryType

Description:
This data structure defines a single entry of a vocoder mapping table.

Definition:
typedef struct
{
   uint16               playoutBufferSize;
   uint8                vocoderId;
   uint8                vocoderMode;
   uint8                interleavingFactor;
   uint8                bundlingFactor;
   DataRateType         minOperatingRate;
   DataRateType         maxOperatingRate;
} IQDK_ProvVocoderTableEntryType;

Members:
  playoutBufferSize: Specifies the size of the media buffer for packets received at the QChat Client for the Vocoder specified in Vocoder identifier field.
  vocoderId: Id of the particular vocoder.
  vocoderMode: Specifies the mode of the vocoder in Vocoder identifier field. 
  interleavingFactor: Specifies the interleaving factor of the Vocoder in Vocoder identifier field. 
  bundlingFactor: Specifies the bundling factor of the Vocoder specified in Vocoder identifier field. 
  minOperatingRate: Specifies the minimum operating rate of the Vocoder in Vocoder identifier field. 
  maxOperatingRate: Specifies the maximum operating rate of the Vocoder in Vocoder identifier field. 

Version:
  Introduced in QDK version 2.0

See Also:
  None

================================================================================

IQDK_ProvTableEntry

Description:
This data structure defines the list of tables that the QChat Client supports for provisioning.

Definition:
typedef union
{
   IQDK_ProvDomainEntryType                  domainEntry;
   IQDK_ProvForeignDomainEntryType           foreignDomainEntry;
   IQDK_ProvAlertEntryType                   alertEntry;
   IQDK_ProvPresenceEntryType                preseneceEntry;
   IQDK_ProvVocoderTableEntryType            vocoderEntry;
   IQDK_CallRestrictionEntry                 restrictionEntry;
} IQDK_ProvTableEntry;

Members:
   alertEntry:          Specifies whether the alert mapping table is going to be used.
   presencecwEntry:     Specifies whether the presence mapping table is going to be used.
   restrictionEntry: Specifies whether the call restriction mapping table is going to be used.
   domainEntry: Specifies whether the domain mapping table is going to be used.
   vocoderEntry:        Specifies whether the vocoder mapping table is going to be used.
   foreignDomainEntry: Specifies whether the foreign domain mapping table is going to be used.

Version:
  Introduced in QDK version 2.0

See Also:
  None

================================================================================

IQDK_ProvTable

Description:
typedef struct
{
  IQDK_ProvTableEntry                     * entries;
  uint16                                    numEntries;
} IQDK_ProvTable;

Members:
   entries: array of entries 
   numEntries: num of entries used

Members:
   Introduced in QDK version 2.0

See Also:
   None

=================================================================================

IQDK_ProvResponseCodeType

Description:
   This data structure defines the response code that the QDK provisioning 
   application uses to inform the QChat Client about the status of the last
   provisioninig session between the QChat Client and the QDK provisioning application.
   This data structure is used as an argument in the QDK API IQDKPROVISION_SyncResults()
   based upon which the QChat Client will take the appropriate action.

Definition:
typedef enum
{
   IQDK_PROV_RESPCODE_SUCCESSFUL_SYNC,
   IQDK_PROV_RESPCODE_PARTIAL_SYNC,
   IQDK_PROV_RESPCODE_UNKNOWN,
   IQDK_PROV_RESPCODE_NO_UPDATES,
   IQDK_PROV_RESPCODE_NO_DELTA,
} IQDK_ProvResponseCodeType;

Members:
   IQDK_PROV_RESPCODE_SUCCESSFUL_SYNC : Sync was succesful

   IQDK_PROV_RESPCODE_PARTIAL_SYNC : If amount of time QChat has specified is running out, 
                                   the QDK Application can call IQDKPROVISION_SyncResults 
                                   with this response code to provide what it has so far
                                   and effectively ask for more time.  It can assume the QChat 
                                   Client has restarted the timeout period. The QChat Client 
                                   can send the IQDK_EVT_PROVISION_CANCEL_SYNC if it really 
                                   doesn't want the QDK Provisioning Application to continue. This 
                                   can also be used periodically to optimize performance between the two.
                                   The client sends the IQDK_EVT_PROVISION_SYNC if the client wants the
                                   provisioning app to continue.

   IQDK_PROV_RESPCODE_UNKNOWN : Unknown error. The QChat Client will use the current provisionable 
                              parameters (most of which maybe set as default).

   IQDK_PROV_RESPCODE_NO_UPDATES : No updates are available. This value basically means 
                                 that the QChat Client has the current information available.  

   IQDK_PROV_RESPCODE_NO_DELTA : If full synchronization was performed instead of just the deltas.

Version:
  Introduced in QDK version 2.0

See Also:
  None

=================================================================================

ProvTypeEnum

Description:
   This enumeration defines the different types of provisioning keys.

Definition:
typedef enum
{
   PROV_BOOL,
   PROV_UINT32,
   PROV_STR,
   PROV_BIN,
   PROV_LAST
} ProvTypeEnum;

Members:
   PROV_BOOL      :  the key is of type boolean
   PROV_UINT32    :  the key is of type integer
   PROV_STR       :  the key is of type string
   PROV_BIN       :  the key is of type binary

Version:
  Introduced in QDK version 2.0

See Also:
  None

================================================================================
  INTERFACES   DOCUMENTATION
================================================================================

Interface Name: IQDKProvision

Description:
The IQDKProvision interface provides a set of API methods allowing a BREW
application to learn of and update the QChat Client's provisioning info.

To access this interface, an application must have the corresponding 
privilege in its MIF file.

The following header file is required:~
AEEQDKProvision.h

================================================================================

IQDKPROVISION_AddRef()

Description:
   This method increments the reference count of the IQDKProvision Interface 
   object. This allows the object to be shared by multiple callers. 
   The object is freed when the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKPROVISION_AddRef(IQDKProvision *po)

Parameters:
   po : Pointer to the IQDKProvision Interface object

Return Value:
   Incremented reference count for the object

Comments:
   A valid object returns a positive reference count.

Version:
   Introduced in QDK v 2.0

Side Effects:
   None

See Also:
   IQDKPROVISION_Release()

================================================================================

IQDKPROVISION_Release()

Description:
   This method decrements the reference count of an object. The object is freed
   from memory and is no longer valid once the reference count reaches 0 (zero).

Prototype:
   uint32 IQDKPROVISION_Release(IQDKProvision *po)

Parameters:
   po: Pointer to the IQDKProvision Interface object

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
   IQDKPROVISION_AddRef()

================================================================================

IQDKPROVISION_Init

Description:
   This function associates application class id with the given instance of
   IQDKProvision interface. Any application that wishes to use IQDKProvision
   interface will have to invoke this method after creating instance
   of IQDKProvision interface and before calling any other IQDKProvision
   interface methods.

Prototype:
   IQDK_ErrorType Init
   (
      IQDKProvision *pMe,
      AEECLSID classId
   )

Parameters:
   pMe            :  Pointer to the IQDKProvision Interface object.
   classId        :  Class id of application that wishes to use the IQDKProvision
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

IQDKPROVISION_SyncResults

Description:
   This function is used by the QDK Provisioning application to notify the QChat Client
   that it has set all the provisionable items along with the end result of the 
   synchronization.

Prototype:
   IQDK_ErrorType IQDKPROVISION_SyncResults(IQDKProvision *po,
                                            IQDK_ProvResponseCodeType resp);

Parameters:
   po:  Pointer to the IQDKProvision Interface object.
   resp: Result of the synchronization that the QDK provisioning application sends to
      the QChat Client.

Return Value:
   SUCCESS: operation successful
   EBADCLASS : po pointer is NULL
   EBADSTATE : client is not willing to accept provisioning updates
   EQDKOUTOFRANGE: resp parameter is invalid
   EQDKNOTINITIALIZED: IQDKPROVISION_Init() was not called first

Comments:
   None

Version:
   Introduced in QDK v 2.0

================================================================================

IQDKPROVISION_InformOfUpdate

Description:
   This function is used by the QDK Provisioning Applciation to inform the QChat Client 
   of possible updates that the QDK Provisioning Application has. The QChat Client can 
   can respond to this by sending an event IQDK_EVT_PROVISION_SYNC.

Prototype:
   IQDK_ErrorType (*InformOfUpdate)(IQDKProvision *po);

Parameters:
   po:  Pointer to the IQDKProvision Interface object.

Return Value:
   SUCCESS: operation successful
   EBADCLASS : po pointer is NULL
   EQDKNOTINITIALIZED : IQDKPROVISION_Init() was not called first

Comments:
   None

Version:
   Introduced in QDK v 2.0

================================================================================

IQDKPROVISION_SetProvEntry_Table

Description:
   This function sets all the entries of a particular table in the QChat client
   based on the values specified in the field tableType.

Prototype:
   IQDK_ErrorType IQDKPROVISION_SetProvEntry_Table(IQDKProvision *pMe,
                                                   IQDK_ProvTableEntry* pointerToTable,
                                                   IQDK_ProvTableType tableType);

Parameters:
   pMe:  Pointer to the IQDKProvision Interface object.
   pointerToTable : pointer to the table to be set.
   tableType: One of the types of tables listed in the enumeration IQDK_ProvTableType

Return Value:
   SUCCESS: operation successful
   EBADCLASS : po pointer is NULL
   EQDKNOTINITIALIZED : IQDKPROVISION_Init() was not called first
   EBADSTATE : client is not willing to accept provisioning updates
   EBADPARAM: pointerToTable is NULL
   EQDKOUTOFRANGE : tableType is invalid.
   EQDKBADVALUE : when setting a user level call restriction with type set to allow always

Comments:
   None

Version:
   Introduced in QDK v 2.0

================================================================================

IQDKPROVISION_GetProvEntry_Table

Description:
   This function returns all the entries of a particular table to the QDK Provisioning application
   based on the value specified in the field tableType.

Prototype:
   IQDK_ErrorType IQDKPROVISION_GetProvEntry_Table(IQDKProvision *pMe,
                                                   IQDK_ProvTableEntry* pointerToTable,
                                                   IQDK_ProvTableType tableType);

Parameters:
   pMe:  Pointer to the IQDKProvision Interface object.
   pointerToTable : Pointer to the table that the QChat Client fills with the table values for the QDK provisioning application.
   tableType: One of the types of tables listed in the enumeration IQDK_ProvTableType

Return Value:
   SUCCESS: operation successful
   EBADCLASS : po pointer is NULL
   EBADPARAM: pointerToTable is NULL. 
   EQDKOUTOFRANGE : tableType is out of range
   EQDKNOTINITIALIZED: if the IQDKPROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. If the state
                       received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.
                       
Comments:
   None

Version:
   Introduced in QDK v 2.0

================================================================================

IQDKPROVISION_SetProvEntry_AddressList()

Description:
   This function is used by the QDK provisioning application to set a list of 
   provisionable QDK addresses in the QChat Client. 

Prototype:
   IQDK_ErrorType IQDKPROVISION_SetProvEntry_AddressList(IQDKProvision *pMe,
                                                         IQDK_ProvAddressListType addressGroup,
                                                         IQDKAddressListType *addresses);

Parameters:
   pMe:  Pointer to the IQDKProvision Interface object.
   addressGroup: One of the types of Lists accepted by the QChat Client as listed in IQDK_ProvAddressListType.
   addresses: Pointer to a list of QChat addresses that the QChat Client accepts from the QDK provisioning application.

Return Value:
   SUCCESS: operation successful
   EBADCLASS : po pointer is NULL
   EQDKNOTINITIALIZED : IQDKPROVISION_Init() was not called first
   EBADSTATE : client is not willing to accept provisioning updates
   EBADPARAM: invalid addresses pointer.
   EQDKOUTOFRANGE : addressGroup is out of range.
   EQDKINVALIDQCHATADDRESS : if the primary QChat user address being set is domainless.

Comments:
   None

Version:
   Introduced in QDK v 2.0

================================================================================

IQDKPROVISION_GetProvEntry_AddressList()

Description:
   This function is used by the QDK provisioning application to get a list of 
   provisionable QDK addresses from the QChat Client. 

Prototype:
   IQDK_ErrorType IQDKPROVISION_GetProvEntry_AddressList(IQDKProvision *pMe,
                                                         IQDK_ProvAddressListType addressGroup,
                                                         IQDKAddressListType *addresses);
Parameters:
   pMe:  Pointer to the IQDKProvision Interface object.
   addressGroup: One of the types of Lists accepted by the QChat Client as listed in IQDK_ProvAddressListType.
   addresses: Pointer to a list of QChat addresses that the QChat Client fills with the list of QDK addresses for the QDK provisioning application.

Return Value:
   SUCCESS: operation successful
   EBADCLASS: pMe pointer is NULL
   EBADPARAM: addresses pointer is NULL
   EQDKOUTOFRANGE : addressGroup is out of range.
   EQDKNOTINITIALIZED: if the IQDKPROVISION_Init() has not been called or if the client wasn't done initializing. 
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

IQDKPROVISION_GetNumProvKeys

Description:
   This function is used by the QDK provisioning application to get the number
   of custom provisionable items

Prototype:
   IQDK_ErrorType IQDKPROVISION_GetNumProvKeys(IQDKProvision *pMe, int16 *numKeys);

Parameters:
   pMe: Pointer to the IQDKProvision Interface object
   numKeys : Pointer to the number of provisionable items

Return Value:
   SUCCESS: operation successful
   EBADCLASS: pMe pointer is NULL
   EBADPARAM : numKeys pointer is NULL.
   EQDKNOTINITIALIZED: if the IQDKPROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. If the state
                       received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.
Comments:
   The caller is responsible for allocating memory for parameter numKeys.

Versions:
   Introduced in QDK v 2.0

================================================================================

IQDKPROVISION_GetProvEntry_Details

Description:
   This function is used to get the details of the custom provisionable entry given the key

Prototype:
   IQDK_ErrorType IQDKPROVISION_GetProvEntry_Details(IQDKProvision *po,
                                                     uint16 key,
                                                     uint16 * dataSize,
                                                     uint32 * minValue,
                                                     uint32 * maxValue
                                                     ProvTypeEnum* provType);

Parameters:
   po: Pointer to the IQDKProvision Interface object
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
   EQDKNOTINITIALIZED: if the IQDKPROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. If the state
                       received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.

Comments:
   The caller is responsible for allocating memory for parameters dataSize,
   minValue, maxValue and provType

Versions:
   Introduced in QDK v 2.0

================================================================================

IQDKPROVISION_GetProvEntry

Description:
   This function is used to get the value associated with key for the custom provisioning item

Prototype:
   IQDK_ErrorType IQDKPROVISION_GetProvEntry)(IQDKProvision *po,
                                  uint16 key,
                                  uint8 ** value);
Parameters:
   po: Pointer to the IQDKProvision Interface object
   key: provisionable item's key for which the QDK application requires the value
   value: the provisionable entry's value is returned via this parameter

Return Value:
   SUCCESS: operation successful
   EBADCLASS: po pointer is NULL
   EBADPARAM: value pointer is NULL
   EQDKOUTOFRANGE : key is out of range.
   EQDKNOTINITIALIZED: if the IQDKPROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. If the state
                       received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.

Comments:
   The caller is responsible for allocating & deallocating memory for the value

Versions:
   Introduced in QDK v 2.0

================================================================================

IQDKPROVISION_SetProvEntry

Description:
   This function is used to set the value for the custom provisioning item identified by the key.

Prototype:
   IQDK_ErrorType IQDKPROVISION_SetProvEntry(IQDKProvision *po,
                                            uint16 key,
                                            uint8 * value);
Parameters:
   po: Pointer to the IQDKProvision Interface object
   key: provisionable item's key for which the QDK application wishes to set the value
   value: the value to be set for the provisioning item identified by the key

Return Value:
   SUCCESS: operation successful
   EBADCLASS : po pointer is NULL
   EQDKNOTINITIALIZED : IQDKPROVISION_Init() was not called first
   EBADSTATE : client is not willing to accept provisioning updates
   EQDKOUTOFRANGE: key is not between min and max values

Comments:
   The caller is responsible for allocating & deallocating memory for the value

Versions:
   Introduced in QDK v 2.0

=================================================================================

IQDKPROVISION_GetAddressListSize

Description:
   This function is used to get the number of predefined group addresses or alternate
   user addresses depending on the Address List Type passed

Prototype:
   IQDK_ErrorType IQDKPROVISION_GetAddressListSize(IQDKProvision *pMe,
                               IQDK_ProvAddressListType listType, int16 *listSize);

Parameters:
   pMe: Pointer to the IQDKProvision Interface object
   listType: Can either be predefined group or alternate user addresses
   listSize : Pointer to the number of predefined group or alternate user addresses

Return Value:
   SUCCESS: operation successful
   EBADCLASS : pMe pointer is NULL
   EQDKOUTOFRANGE: listType is out of range
   EBADPARAM : listSize pointer is NULL.
   EQDKNOTINITIALIZED: if the IQDKPROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. If the state
                       received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.
Comments:
   The caller is responsible for allocating memory for listSize.

Versions:
   Introduced in QDK v 2.0

=================================================================================

IQDKPROVISION_GetTableSize

Description:
   This function is used to get the number of entries for the table defined by
   IQDK_ProvTableType

Prototype:
   IQDK_ErrorType IQDKPROVISION_GetTableSize(IQDKProvision *pMe,
                         IQDK_ProvTableType tableType, int16 *tableSize);

Parameters:
   pMe: Pointer to the IQDKProvision Interface object
   tableType: can be domain, foreign domain, presence, alert, call restrictions or presence table
   tableSize: Pointer to the number of entries in the table defined by tableType

Return Value:
   SUCCESS: operation successful
   EBADCLASS : pMe pointer is NULL
   EQDKOUTOFRANGE: tableType is out of range
   EBADPARAM : tableSize pointer is NULL.
   EQDKNOTINITIALIZED: if the IQDKPROVISION_Init() has not been called or if the client wasn't done initializing. 
                       If EQDKNOTINITIALIZED is received because the client didn't finish
                       initializing, then the app must monitor the IQDK_EVT_QCHAT_STATUS event. The QChat client
                       sends this event each time its state changes. If the app receives an event with the state set to
                       IQDK_QCHAT_STATE_OBTAINING_SERVICE, IQDK_QCHAT_STATE_ONLINE_IDLE, IQDK_QCHAT_STATE_ONLINE_QCHAT_CALL,
                       or IQDK_QCHAT_STATE_ONLINE_SUSPENDED then the app can retry the GetProvEntry_Table API. If the state
                       received is IQDK_QCHAT_STATE_ERROR, IQDK_QCHAT_STATE_NO_DIGITAL_SERVICE or IQDK_QCHAT_STATE_OFFLINE
                       then the app has to wait before retrying.
Comments:
   The caller is responsible for allocating memory for tableSize.

Versions:
   Introduced in QDK v 2.0

==================================================================================

Function: IQDKPROVISION_GetEventPayload

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
   IQDK_ErrorType IQDKPROVISION_GetEventPayload(
         IQDKProvision *pMe,
         uint32 interfaceId,
         uint16 eventType,
        IQDK_ProvisionEventPayload **payload)
        
Parameters:

   pMe :		    Pointer to the IQDKProvision Interface object.
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


*/
