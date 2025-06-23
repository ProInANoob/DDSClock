
#ifndef EXTENDED_DISCOVERY_TYPES_H
#define EXTENDED_DISCOVERY_TYPES_H

#if defined(__linux__)
#  include <netinet/in.h>
#  include <net/if.h>
#elif defined(_WIN32)
#    define WIN32_LEAN_AND_MEAN
#  include <winsock2.h>
#  include <windows.h>
#endif

#include "list.h"

#include "rtps.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /*********************************************************************
   */
  
  typedef struct RTPS_SPDPdiscoveredParticipantData
  {
    /* DDS ParticipantBuiltinTopicData: */
    struct DDS_BuiltinTopicKey_t  key;

    DDS_DomainId_t                domain_id;
    char                        * domain_tag;
  
    struct DDS_UserDataQosPolicy  user_data;

    /* additional (new rtps): */
    struct DDS_PropertyQosPolicy  properties;

    /* TOC additions: */
    char                         *entity_name;

    /* EXTENSTIONS -- only available to RTPS -- not present in DDS types (via builtin reader): */
    /* RTPS WriterProxy: */
  
    /* RTPS ParticipantProxy: */
    struct RTPS_ProtocolVersion_t  protocolVersion;
    struct RTPS_VendorId_t         vendorId;
    uint32_t                       productVersion[3]; 
    unsigned char                  expectsInlineQos;
    uint32_t                       availableBuiltinEndpoints;

    struct list_head  metatrafficUnicastLocatorList;
    struct list_head  metatrafficMulticastLocatorList;
    struct list_head  defaultUnicastLocatorList;
    struct list_head  defaultMulticastLocatorList;

    RTPS_Count        manualLivelinessCount;

    /* SPDPdiscoveredParticipantData: */
    DDS_Duration_t    leaseDuration;

  

  } RTPS_SPDPdiscoveredParticipantData;

  void RTPS_DiscoveredParticipantData_init(RTPS_SPDPdiscoveredParticipantData * dpd);
  void RTPS_DiscoveredParticipantData_clear(RTPS_SPDPdiscoveredParticipantData * dpd);
  void RTPS_DiscoveredParticipantData_copy(RTPS_SPDPdiscoveredParticipantData * to, RTPS_SPDPdiscoveredParticipantData * from); 

  /*********************************************************************
   */


  typedef struct RTPS_DiscoveredWriterData
  {
    /* DDS PublicationBuiltinTopicData: */
    /*  *NOTE*  keep this exactly like DDS_DCPSPublication ! */
    struct DDS_BuiltinTopicKey_t          participant_key;
    struct DDS_BuiltinTopicKey_t          key;
    char            *                     topic_name;
    char            *                     type_name;
    struct DDS_DurabilityQosPolicy        durability;
    struct DDS_DurabilityServiceQosPolicy durability_service;
    struct DDS_DeadlineQosPolicy          deadline;
    struct DDS_LatencyBudgetQosPolicy     latency_budget;
    struct DDS_LivelinessQosPolicy        liveliness;
    struct DDS_ReliabilityQosPolicy       reliability;
    struct DDS_LifespanQosPolicy          lifespan;
    struct DDS_UserDataQosPolicy          user_data;
    struct DDS_OwnershipQosPolicy         ownership;
    struct DDS_OwnershipStrengthQosPolicy ownership_strength;
    struct DDS_DestinationOrderQosPolicy  destination_order;
    struct DDS_PresentationQosPolicy      presentation;
    struct DDS_PartitionQosPolicy         partition;
    struct DDS_TopicDataQosPolicy         topic_data;
    struct DDS_GroupDataQosPolicy         group_data;
    struct DDS_DataRepresentationQosPolicy  representation;
  
    /* TOC Additions: */
    char                                 *entity_name;
    struct DDS_TypecodeQosPolicy          typecode;
    struct DDS_TypecodeQosPolicy          typeinfo; /* xtypes v2 TypeInformation */

    /* RPC Extensions: */
    /* TODO: decide: */
    /*   This adds 16 + 20 + sizeof(void*) + 256 bytes...! */
    /*     to each local and discovered writer... should we make this conditional?     */
    /*   We could hide it behind a pointer, and waste only 4/8 bytes if not present... */
    struct DDS_RpcQosPolicy               rpc;

    DDS_DataTagQosPolicy                  data_tags; /* @ID(0x1003) */


    /* FROM HERE DOWN, these things are not (yet) exposed by DDS_DCPSPublication
       at the DDS api level... 
       We are free to do what we want...
       Or, we can add things to DDS_DCPSPublication, etc ...
    */
    /* EXTENSIONS -- only available to RTPS -- not present in DDS types (via builtin reader): */
    /* RTPS WriterProxy: */
    struct list_head                      unicastLocatorList;
    struct list_head                      multicastLocatorList;
    unsigned char                         require_acks; /* from RTPSWriterQos */

#if defined(DDS_SECURITY) && (DDS_SECURITY)
#  if DDSSEC_V2  
    DDS_Security_EndpointSecurityInfo     endpoint_security_info; /* PID_ENDPOINT_SECURITY_INFO */
#  endif
#endif
  
  } RTPS_DiscoveredWriterData;

  void RTPS_DiscoveredWriterData_init ( RTPS_DiscoveredWriterData * d );
  void RTPS_DiscoveredWriterData_clear( RTPS_DiscoveredWriterData * d );
  void RTPS_DiscoveredWriterData_copy ( RTPS_DiscoveredWriterData * dst, RTPS_DiscoveredWriterData * src);



  /*********************************************************************
   */
  typedef struct RTPS_ContentFilterProperty_t
  {
    char         * contentFilteredTopicName; /* max 256 */
    char         * relatedTopicName;         /* max 256 */
    char         * filterClassName;          /* max 256 */
    char         * filterExpression;         /* unconstrained */
    DDS_StringSeq  expressionParameters; 
  } RTPS_ContentFilterProperty;


  typedef struct RTPS_DiscoveredReaderData
  {
    /* DDS SubscriptionBuiltinTopicData: */
    /*  *NOTE*  keep this exactly like DDS_DCPSSubscription ! */
    struct DDS_BuiltinTopicKey_t          participant_key;
    struct DDS_BuiltinTopicKey_t          key;
    char            *                     topic_name;
    char            *                     type_name;
    struct DDS_DurabilityQosPolicy        durability;
    struct DDS_DeadlineQosPolicy          deadline;
    struct DDS_LatencyBudgetQosPolicy     latency_budget;
    struct DDS_LivelinessQosPolicy        liveliness;
    struct DDS_ReliabilityQosPolicy       reliability;
    struct DDS_OwnershipQosPolicy         ownership;
    struct DDS_DestinationOrderQosPolicy  destination_order;
    struct DDS_UserDataQosPolicy          user_data;
    struct DDS_TimeBasedFilterQosPolicy   time_based_filter;
    struct DDS_PresentationQosPolicy      presentation;
    struct DDS_PartitionQosPolicy         partition;
    struct DDS_TopicDataQosPolicy         topic_data;
    struct DDS_GroupDataQosPolicy         group_data;
    struct DDS_DataRepresentationQosPolicy         representation;
    struct DDS_TypeConsistencyEnforcementQosPolicy type_consistency;
    
    /* TOC Additions: */
    char                                 *entity_name;
    struct DDS_TypecodeQosPolicy          typecode;
    struct DDS_TypecodeQosPolicy          typeinfo; /* xtypes v2 TypeInformation */
    /* RPC Extensions: */
    /* TODO: decide: */
    /*   This adds 16 + 20 + sizeof(void*) + 256 bytes... ! */
    /*     to each local and discovered writer... should we make this conditional?     */
    /*   We could hide it behind a pointer, and waste only 4/8 bytes if not present... */
    struct DDS_RpcQosPolicy               rpc;

    DDS_DataTagQosPolicy                  data_tags; /* @ID(0x1003) */


    /* FROM HERE DOWN, these things are not (yet) exposed by DDS_DCPSPublication
       at the DDS api level... 
       We are free to do what we want...
       Or, we can add things to DDS_DCPSPublication, etc ...
    */
  
    /* EXTENSTIONS -- only available to RTPS -- not present in DDS types (via builtin reader): */
    /* RTPS ReaderProxy: */
    unsigned char                         accept_batch_msg;
    unsigned char                         expectsInlineQos;
    struct list_head                      unicastLocatorList;
    struct list_head                      multicastLocatorList;
  
    /* ContentFilter */
    RTPS_ContentFilterProperty            content_filter;
  
#if defined(DDS_SECURITY) && (DDS_SECURITY)
#  if DDSSEC_V2  
    DDS_Security_EndpointSecurityInfo     endpoint_security_info; /* PID_ENDPOINT_SECURITY_INFO */
#  endif
#endif
  } RTPS_DiscoveredReaderData;



  void RTPS_DiscoveredReaderData_init ( RTPS_DiscoveredReaderData * d );
  void RTPS_DiscoveredReaderData_clear( RTPS_DiscoveredReaderData * d );
  void RTPS_DiscoveredReaderData_clear_qos( RTPS_DiscoveredReaderData * d );
  void RTPS_DiscoveredReaderData_clear_cf( RTPS_DiscoveredReaderData * d );
  void RTPS_DiscoveredReaderData_copy ( RTPS_DiscoveredReaderData * dst, RTPS_DiscoveredReaderData * src);


  
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
