#ifndef STUDIO_RTPS_BASE_H
#define STUDIO_RTPS_BASE_H

#include <stdlib.h>

#include <dds/dds_seq.h>
#include <dds/dds_types.h>
#include <dds/dds_consts.h>

#include "list.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#if defined( _WIN32 )
#  define TOC_INLINE __inline
#else
#  define TOC_INLINE inline
#endif

#define TOC_MALLOC( A, B, C )    malloc(A)
#define TOC_CALLOC( A, B, C, D ) calloc(A,B)
#define TOC_FREE( A )            free(A)
#define toc_memset(A, B, C)      memset(A, B, C)
#define toc_sprintf(A, ...)      sprintf(A, __VA_ARGS__ )
  
    /* BITWISE_OR 'ENCODING' with RTPS_ENCAP to get encap_encoding */
#if   (__BYTE_ORDER == __BIG_ENDIAN)
#  define RTPS_ENCODING         0
#  define HOST_ENDIAN           0
#else
#  define RTPS_ENCODING         1
#  define HOST_ENDIAN           1
#endif

#define RTPS_ENCODING_FLAG    1

  /* flags to marshal/unmarshal */
#define RTPS_BIG_ENDIAN       0
#define RTPS_LITTLE_ENDIAN    1
#define RTPS_HOST_ENDIAN      HOST_ENDIAN

#define RTPS_JUST_KEYS        1
#define RTPS_ALL_DATA         0

  /* utilize CoreDX DDS extension: */
#define COREDX_DO_RTPS_BATCH  1
#define COREDX_DO_DATAFRAG    1
  
  /* keep this in sync with header structure and protocol! */
# define RTPS_MSG_HEADER_MARSHALLED_SIZE (20)

  typedef intptr_t RTPS_InstanceHandle;
  
  typedef uint8_t RTPS_Octet;
  

  typedef RTPS_Octet        RTPS_GuidPrefix_t[12];
  typedef RTPS_GuidPrefix_t RTPS_GuidPrefix;
  
  typedef struct RTPS_EntityId_t
  {
    RTPS_Octet entityKey[3];
    RTPS_Octet entityKind;
  } RTPS_EntityId;

  typedef struct RTPS_GUID_t
  {
    RTPS_GuidPrefix  guidPrefix;
    RTPS_EntityId    entityId;
  } RTPS_GUID;

#define RTPS_GUIDPREFIX_UNKNOWN\
 {0x00,0x00,0x00,0x00,\
  0x00,0x00,0x00,0x00,\
  0x00,0x00,0x00,0x00}

#define RTPS_ENTITYID_UNKNOWN_INIT                                { {0x00, 0x00, 0x00}, 0x00 }
#define RTPS_ENTITYID_PARTICIPANT_INIT                            { {0x00, 0x00, 0x01}, 0xc1 }
#define RTPS_ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER_INIT        { {0x00, 0x01, 0x00}, 0xc2 } 
#define RTPS_ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER_INIT        { {0x00, 0x01, 0x00}, 0xc7 }
#define RTPS_ENTITYID_SEDP_BUILTIN_TOPIC_WRITER_INIT              { {0x00, 0x00, 0x02}, 0xc2 }
#define RTPS_ENTITYID_SEDP_BUILTIN_TOPIC_READER_INIT              { {0x00, 0x00, 0x02}, 0xc7 }
#define RTPS_ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER_INIT       { {0x00, 0x00, 0x03}, 0xc2 }
#define RTPS_ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER_INIT       { {0x00, 0x00, 0x03}, 0xc7 }
#define RTPS_ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER_INIT      { {0x00, 0x00, 0x04}, 0xc2 }
#define RTPS_ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER_INIT      { {0x00, 0x00, 0x04}, 0xc7 }
#define RTPS_ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER_INIT { {0x00, 0x02, 0x00}, 0xC2 }
#define RTPS_ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER_INIT { {0x00, 0x02, 0x00}, 0xC7 }

#define RTPS_ENTITYID_TL_SVC_REQ_WRITER_INIT                      { {0x00, 0x03, 0x00}, 0xC3 }
#define RTPS_ENTITYID_TL_SVC_REQ_READER_INIT                      { {0x00, 0x03, 0x00}, 0xC4 }
#define RTPS_ENTITYID_TL_SVC_REPLY_WRITER_INIT                    { {0x00, 0x03, 0x01}, 0xC3 }
#define RTPS_ENTITYID_TL_SVC_REPLY_READER_INIT                    { {0x00, 0x03, 0x01}, 0xC4 }

  typedef struct RTPS_ProtocolVersion_t
  {
    RTPS_Octet  major;
    RTPS_Octet  minor;
  } RTPS_ProtocolVersion; 

#define RTPS_PROTOCOLVERSION_1_0 {1,0}
#define RTPS_PROTOCOLVERSION_1_1 {1,1}
#define RTPS_PROTOCOLVERSION_1_2 {1,2}
#define RTPS_PROTOCOLVERSION_2_0 {2,0}
#define RTPS_PROTOCOLVERSION_2_1 {2,1}
#define RTPS_PROTOCOLVERSION_2_2 {2,2}
#define RTPS_PROTOCOLVERSION     {2,2}
  
  typedef struct RTPS_VendorId_t
  {
    RTPS_Octet  vendorId[2];
  } RTPS_VendorId;

#define RTPS_VENDORID_UNKNOWN {{0,0}}
#define RTPS_VENDORID_RTI     {{1,1}}  /* RTI:       NDDS, Connext DDS */
#define RTPS_VENDORID_PT      {{1,2}}  /* PT:        OpenSplice, etal? */
#define RTPS_VENDORID_OCI     {{1,3}}  /* OCI:       OpenDDS  */
#define RTPS_VENDORID_KDA     {{1,5}}  /* Kongsberg: InterCOM DDS */
#define RTPS_VENDORID_TOC     {{1,6}}  /* TwinOaks:  CoreDX DDS */
#define RTPS_VENDORID_CYCLONE {{1,16}}  /* PrismTech/AdLink/ZettaScale/WhoKnow?:  Cyclone DDS */

#define RTPS_VENDORID_MATCH(a, b) ((a.vendorId[0] == b.vendorId[0]) && (a.vendorId[1] == b.vendorId[1]))

  typedef struct RTPS_SequenceNumber_t
  {
    int32_t          high;
    uint32_t         low;
  } RTPS_SequenceNumber;

#define RTPS_SEQUENCENUMBER_UNKNOWN { -1, 0 }
#define RTPS_SEQUENCENUMBER_ZERO    { 0, 0 }
#define SN_TO_LL(snPtr) (((int64_t)(snPtr)->low) + (((int64_t)(snPtr)->high)<<32))
#define SN_FROM_LL(sn,l) (sn).high=(int32_t)(l>>32),(sn).low=(uint32_t)(l&0xFFFFFFFF)
#define SN_SET(s,h,l) (s).high=h,(s).low=l

/* SequenceNumber helpers */
#define RTPS_SequenceNumber_less(a,b)    ((a.high<b.high)||((a.high==b.high)&&(a.low<b.low)))
#define RTPS_SequenceNumber_greater(a,b) ((a.high>b.high)||((a.high==b.high)&&(a.low>b.low)))
#define RTPS_SequenceNumber_ge(a,b)      ((a.high>b.high)||((a.high==b.high)&&(a.low>=b.low)))
#define RTPS_SequenceNumber_equal(a,b)   (((a).high==(b).high)&&((a).low==(b).low))
  
  typedef struct RTPS_SequenceNumberSet_t
  {
    RTPS_SequenceNumber   bitmapBase;
    uint32_t              numBits; 
    uint32_t              bitmap[8];
  } RTPS_SequenceNumberSet;

#define RTPS_SNS_NUM_LONGS(nbits) (((nbits)+31)>>5)

  typedef uint32_t RTPS_FragmentNumber_t;

  typedef struct RTPS_FragmentNumberSet_t
  {
    RTPS_SequenceNumber   bitmapBase;
    uint32_t              numBits;  /* missing from spec ( ? ) */
    int32_t             * bitmap;
  } RTPS_FragmentNumberSet;

  typedef struct RTPS_Time_t
  {
    int32_t      seconds;   /* in secconds     */
    uint32_t     fraction;  /* in 1/2^32 seconds */
  } RTPS_Time;

  typedef struct RTPS_Timestamp_t
  {
    RTPS_Time value;
  } RTPS_Timestamp;

  typedef int32_t RTPS_Count;

  typedef struct RTPS_Locator_t
  {
    int32_t                  kind;
    uint32_t                 port;
    RTPS_Octet               address[16];
    struct list_head         list;
  } RTPS_Locator;

  typedef unsigned char RTPS_StatusInfo[4];
#define RTPS_STATUS_INFO_ALIVE         0x00
#define RTPS_STATUS_INFO_DISPOSED      0x01
#define RTPS_STATUS_INFO_UNREGISTERED  0x02

  typedef enum RTPS_ChangeKind_t
    {
      ALIVE_CHANGEKIND = 0x00,
      NOT_ALIVE_DISPOSED_CHANGEKIND = 0x01,
      NOT_ALIVE_UNREGISTERED_CHANGEKIND = 0x02,
      GAP_CHANGEKIND = 0x04 /* internal special case */
    } RTPS_ChangeKind;

extern char * RTPS_locatorToString(RTPS_Locator *l, char *buf, size_t buflen); 
  /* Where we re-build a sample from Fragments */
  typedef struct RTPS_FragSample
  {
    RTPS_GUID               src_guid;     /* key / and used by StatelessReader */
    uint64_t                sn;           /* sequence number of sample we are reconstructing */
    unsigned char         * data;         /* buffer to hold complete sample */
    struct timespec         source_time;  /* source time stamp (may be invalid) */
    struct timespec         update_time;  /* last time we added to this sample */

    uint32_t                sample_size;  /* for sanity check */
    uint32_t                frag_size;    /* for sanity check */
    uint32_t                frags_total;  /* for sanity check */

    uint32_t                num_maps; /* how many maps required to indicate all frag bits */
    uint32_t                first_fm; /* the map that has 'first' missing fragment */
    RTPS_SequenceNumberSet *frag_map; /* map for 'rxd' fragments (base is 64bit, but we use only 32bits) */

    unsigned char         * inline_qos;
    uint32_t                inline_qos_len;
    unsigned char           data_is_key;
  } RTPS_FragSample_t;

  
  /* MESSAGE PARAMETER ID's */

#define RTPS_PID_PAD                             0x0000  /* N/A */
#define RTPS_PID_SENTINEL                        0x0001  /* N/A */
#define RTPS_PID_PARTICIPANT_LEASE_DURATION      0x0002  /* Duration_t */
#define   RTPS_PID_PERSISTENCE                     0x0003  /* DEPRECATED */
#define RTPS_PID_TIME_BASED_FILTER               0x0004  /* TimeBasedFilterQosPolicy */
#define RTPS_PID_TOPIC_NAME                      0x0005  /* string<256> */
#define RTPS_PID_OWNERSHIP_STRENGTH              0x0006  /* OwnershipStrengthQosPolicy */
#define RTPS_PID_TYPE_NAME                       0x0007  /* string<256> */
#define   RTPS_PID_TYPE_CHECKSUM                   0x0008  /* DEPRECATED */
#define   RTPS_PID_TYPE2_NAME                      0x0009  /* DEPRECATED */
#define   RTPS_PID_TYPE2_CHECKSUM                  0x000a  /* DEPRECATED */
#define   RTPS_PID_METATRAFFIC_MULTICAST_IPADDRESS 0x000b  /* DEPRECATED IPv4Address_t */
#define   RTPS_PID_DEFAULT_UNICAST_IPADDRESS       0x000c  /* DEPRECATED IPv4Address_t */
#define   RTPS_PID_METATRAFFIC_UNICAST_PORT        0x000d  /* DEPRECATED Port_t */
#define   RTPS_PID_DEFAULT_UNICAST_PORT            0x000e  /* DEPRECATED Port_t */
#define RTPS_PID_DOMAIN_ID                         0x000f  /* DomainId_t */

#define   RTPS_PID_EXPECTS_ACK                     0x0010  /* DEPRECATED */
#define   RTPS_PID_MULTICAST_IPADDRESS             0x0011  /* DEPRECATED IPv4Address_t */
#define   RTPS_PID_MANAGER_KEY                     0x0012  /* DEPRECATED */
#define   RTPS_PID_SEND_QUEUE_SIZE                 0x0013  /* DEPRECATED */
#define   RTPS_PID_RELIABILTY_ENABLED              0x0014  /* DEPRECATED */
#define RTPS_PID_PROTOCOL_VERSION                0x0015  /* ProtocolVersion_t */
#define RTPS_PID_VENDORID                        0x0016  /* VendorId_t */
#define   RTPS_PID_VARCAPPS_SEQ_NUM_LAST           0x0017  /* DEPRECATED */
#define   RTPS_PID_RECV_QUEUE_SIZE                 0x0018  /* DEPRECATED */
#define   RTPS_PID_RELIABILITY_OFFERED             0x0019  /* DEPRECATED */
#define RTPS_PID_RELIABILITY                     0x001a  /* ReliabilityQosPolicy */
#define RTPS_PID_LIVELINESS                      0x001b  /* LivelinessQosPolicy */
#define RTPS_PID_DURABILITY                      0x001d  /* DurabilityQosPolicy */
#define RTPS_PID_DURABILITY_SERVICE              0x001e  /* DurabilityServiceQosPolicy */
#define RTPS_PID_OWNERSHIP                       0x001f  /* OwnershipQosPolicy */

#define RTPS_PID_PRESENTATION                    0x0021  /* PresentationQosPolicy */
#define RTPS_PID_DEADLINE                        0x0023  /* DeadlineQosPolicy */
#define RTPS_PID_DESTINATION_ORDER               0x0025  /* DestinationOrderQosPolicy */
#define RTPS_PID_LATENCY_BUDGET                  0x0027  /* LatencyBudgetQosPolicy */
#define RTPS_PID_PARTITION                       0x0029  /* PartitionQosPolicy */
#define RTPS_PID_LIFESPAN                        0x002b  /* LifespanQosPolicy */
#define RTPS_PID_USER_DATA                       0x002c  /* UserDataQosPolicy */
#define RTPS_PID_GROUP_DATA                      0x002d  /* GroupDataQosPolicy */
#define RTPS_PID_TOPIC_DATA                      0x002e  /* TopicDataQosPolicy */
#define RTPS_PID_UNICAST_LOCATOR                 0x002f  /* Locator_t */
  
#define RTPS_PID_MULTICAST_LOCATOR               0x0030  /* Locator_t */
#define RTPS_PID_DEFAULT_UNICAST_LOCATOR         0x0031  /* Locator_t */
#define RTPS_PID_METATRAFFIC_UNICAST_LOCATOR     0x0032  /* Locator_t */
#define RTPS_PID_METATRAFFIC_MULTICAST_LOCATOR   0x0033  /* Locator_t */
#define RTPS_PID_PARTICIPANT_MANUAL_LIVELINESS_COUNT 0x0034  /* Count_t */
#define RTPS_PID_CONTENT_FILTER_PROPERTY         0x0035  /* ContentFilterProperty_t */

#define RTPS_PID_HISTORY                         0x0040  /* HistoryQosPolicy */
#define RTPS_PID_RESOURCE_LIMITS                 0x0041  /* ResourceLimitsQosPolicy */
#define RTPS_PID_EXPECTS_INLINE_QOS              0x0043  /* boolean */
#define   RTPS_PID_PARTICIPANT_BUILTIN_ENDPOINTS   0x0044  /* DEPRECATED unsigned int32 */
#define   RTPS_PID_METATRAFFIC_UNICAST_IPADDRESS   0x0045  /* DEPRECATED IPv4Address_t */
#define   RTPS_PID_METATRAFFIC_MULTICAST_PORT      0x0046  /* DEPRECATED Port_t */
#define RTPS_PID_DEFAULT_MULTICAST_LOCATOR       0x0048  /* Locator_t */
#define RTPS_PID_TRANSPORT_PRIORITY              0x0049  /* TransportPriorityQoSPolicy */

#define RTPS_PID_PARTICIPANT_GUID                0x0050  /* GUID_t */
#define   RTPS_PID_PARTICIPANT_ENTITYID            0x0051  /* DEPRECATED EntityId_t */
#define RTPS_PID_GROUP_GUID                      0x0052  /* GUID_t */
#define   RTPS_PID_GROUP_ENTITYID                  0x0053  /* DEPRECATED EntityId_t */
#define RTPS_PID_CONTENT_FILTER_INFO             0x0055  /* ContentFilterInfo_t */
#define RTPS_PID_COHERENT_SET                    0x0056  /* SequenceNumber_t */
#define RTPS_PID_DIRECTED_WRITE                  0x0057  /* GUID_t */
#define RTPS_PID_BUILTIN_ENDPOINT_SET            0x0058  /* BuiltinEndpointSet_t */
#define RTPS_PID_PROPERTY_LIST                   0x0059  /* sequence<Property_t> */
#define RTPS_PID_ENDPOINT_GUID                   0x005A  /* sequence<Property_t> */

#define RTPS_PID_TYPE_MAX_SIZE_SERIALIZED        0x0060  /* int32 */
#define RTPS_PID_ORIGINAL_WRITER_INFO            0x0061  /* OriginalWriterInfo_t */
#define RTPS_PID_ENTITY_NAME                     0x0062  /* EntityName_t */
#define RTPS_PID_GROUP_COHERENT_SET              0x0063  /* SequenceNumber_t */
#define RTPS_PID_GROUP_SEQ_NUM                   0x0064  /* SequenceNumber_t */
#define RTPS_PID_WRITER_GROUP_INFO               0x0065  /* WriterGroupInfo_t */
#define RTPS_PID_SECURE_WRITER_GROUP_INFO        0x0066  /* WriterGroupInfo_t */


#define RTPS_PID_TYPEID_V1                       0x0069  /* TypeIdV1 -- X-Types 1.1  -- we don't support */
#define RTPS_PID_KEY_HASH                        0x0070  /* KeyHash_t */
#define RTPS_PID_STATUS_INFO                     0x0071  /* StatusInfo_t */
#define RTPS_PID_TYPE_OBJECT                     0x0072  /* TypeObject -- X-Types 1.1 -- we support */
#define RTPS_PID_DATA_REPRESENTATION             0x0073  /* DataRepresentationQosPolicy */
#define RTPS_PID_TYPE_CONSISTENCY_ENFORCEMENT    0x0074  /* TypeConsistencyEnforcementQosPolicy */
#define RTPS_PID_TYPE_INFORMATION                0x0075  /* XTypes::TypeInformation -- X-Types 1.2 -- we don't (yet) support */
#define RTPS_PID_BASE_TYPE_NAME                  0x0076  /* XTypes 1.1 [DEPRECATED in v1.2] not supported */
#define RTPS_PID_BUILTIN_ENDPOINT_QOS            0x0077  /* BuiltinEndpointQos_t / or ENABLE_ENCRYPTION? */


  /* RPC over DDS extensions: not sure about numbering here.... */
#define RTPS_PID_SERVICE_INSTANCE_NAME           0x0080  /* string<256> */
#define RTPS_PID_RELATED_ENTITY_GUID             0x0081  /* GUID_t (16 bytes) */
#define RTPS_PID_TOPIC_ALIASES                   0x0082  /* sequence<string<256>> */
#define RTPS_PID_RELATED_SAMPLE_ID               0x0083  /* SampleIdentity */

  /* SECURE DDS extensions: */
#define  RTPS_PID_SECURITY_IDENTITY_TOKEN        0x1001
#define  RTPS_PID_SECURITY_PERMISSIONS_TOKEN     0x1002
#define  RTPS_PID_SECURITY_DATA_TAGS             0x1003
#define  RTPS_PID_ENDPOINT_SECURITY_INFO         0x1004
#define  RTPS_PID_PARTICIPANT_SECURITY_INFO      0x1005
#define  RTPS_PID_IDENTITY_STATUS_TOKEN          0x1006

  /* for these, see: https://issues.omg.org/browse/DDSSEC12-90 */
#define RTPS_PID_PARTICIPANT_SECURITY_DIGITAL_SIGNATURE_ALGORITHM_INFO             0x1010 /* in TBD security spec */
#define RTPS_PID_PARTICIPANT_SECURITY_KEY_ESTABLISHMENT_ALGORITM_INFO              0x1011 /* in TBD security spec */
#define RTPS_PID_PARTICIPANT_SECURITY_BUILTIN_EP_SYMMETRIC_CIPHER_ALGORITHM_INFO   0x1012 /* in TBD security spec */
#define RTPS_PID_ENDPOINT_SECURITY_SYMMETRIC_CIPHER_ALGORITHM_INFO                 0x1013 /* in TBD security spec */
  
  /* OTHERS: */
#define  RTPS_PID_DOMAIN_TAG                     0x4014 /* THIS IS BAD ( has must-understand bit, but collides with PID 0x0014 )
                                                           a string 'tag' used for some matching stuff */

  /* TOC VENDOR SPECIFIC: */
#define TOC_COREDX_PID_PRODUCT_VERSION           0x8000 /* COREDX PRODUCT VERSION */
#define TOC_COREDX_PID_RES1                      0x8001 /* COREDX PID RESERVED */
#define TOC_COREDX_PID_RES2                      0x8002 /* COREDX PID RESERVED */
#define TOC_COREDX_PID_RES3                      0x8003 /* COREDX PID RESERVED */
#define TOC_COREDX_PID_TYPECODE                  0x8004 /* COREDX TypeCode (compatible with RTI(?)) */
#define TOC_COREDX_PID_RES5                      0x8005 /* COREDX PID RESERVED */
#define TOC_COREDX_PID_RES6                      0x8006 /* COREDX PID RESERVED */
#define TOC_COREDX_PID_ENABLE_BATCH              0x8007 /* COREDX ENABLE BATCH */
#define TOC_COREDX_PID_REQUIRE_ACKS              0x8008 /* COREDX WRITER REQUIRE ACKS */

  
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_ANNOUNCER (0x00000001 << 0)
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_DETECTOR (0x00000001 << 1)
#define DISC_BUILTIN_ENDPOINT_PUBLICATION_ANNOUNCER (0x00000001 << 2)
#define DISC_BUILTIN_ENDPOINT_PUBLICATION_DETECTOR (0x00000001 << 3)
#define DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_ANNOUNCER (0x00000001 << 4)
#define DISC_BUILTIN_ENDPOINT_SUBSCRIPTION_DETECTOR (0x00000001 << 5)
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_PROXY_ANNOUNCER (0x00000001 << 6)
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_PROXY_DETECTOR (0x00000001 << 7)
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_STATE_ANNOUNCER (0x00000001 << 8)
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_STATE_DETECTOR (0x00000001 << 9)
#define BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_WRITER (0x00000001 << 10)
#define BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_DATA_READER (0x00000001 << 11)

#define DISC_BUILTIN_ENDPOINT_TYPE_LOOKUP_SERVICE_REQUEST_WRITER (0x00000001 << 12)
#define DISC_BUILTIN_ENDPOINT_TYPE_LOOKUP_SERVICE_REQUEST_READER (0x00000001 << 13)
#define DISC_BUILTIN_ENDPOINT_TYPE_LOOKUP_SERVICE_REPLY_WRITER (0x00000001 << 14)
#define DISC_BUILTIN_ENDPOINT_TYPE_LOOKUP_SERVICE_REPLY_READER (0x00000001 << 15)

#define DISC_BUILTIN_ENDPOINT_PUBLICATIONS_SECURE_WRITER (0x00000001 << 16)
#define DISC_BUILTIN_ENDPOINT_PUBLICATIONS_SECURE_READER (0x00000001 << 17)
#define DISC_BUILTIN_ENDPOINT_SUBSCRIPTIONS_SECURE_WRITER (0x00000001 << 18)
#define DISC_BUILTIN_ENDPOINT_SUBSCRIPTIONS_SECURE_READER (0x00000001 << 19)

#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_SECURE_WRITER (0x00000001 << 20)
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_SECURE_READER (0x00000001 << 21)
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_STATELESS_SECURE_WRITER (0x00000001 << 22)
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_MESSAGE_STATELESS_SECURE_READER (0x00000001 << 23)
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_VOLATILE_MESSAGE_SECURE_WRITER (0x00000001 << 24)
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_VOLATILE_MESSAGE_SECURE_READER (0x00000001 << 25)
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_SECURE_WRITER (0x00000001 << 26)
#define DISC_BUILTIN_ENDPOINT_PARTICIPANT_SECURE_READER (0x00000001 << 27) 

#define DISC_BUILTIN_ENDPOINT_TOPIC_ANNOUNCER (0x00000001 << 28)
#define DISC_BUILTIN_ENDPOINT_TOPIC_DETECTOR (0x00000001 << 29) 



#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif

