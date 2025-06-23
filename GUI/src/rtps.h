#ifndef STUDIO_RTPS_H
#define STUDIO_RTPS_H

#include "list.h"
#include "rtps_base.h"
#include "rtps_submsgs.h"
#include <dds/dds.h>


#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  
  extern RTPS_EntityId RTPS_ENTITYID_UNKNOWN;
  extern RTPS_EntityId RTPS_ENTITYID_PARTICIPANT;
  extern RTPS_EntityId RTPS_ENTITYID_SPDP_BUILTIN_PARTICIPANT_WRITER;
  extern RTPS_EntityId RTPS_ENTITYID_SPDP_BUILTIN_PARTICIPANT_READER;
  extern RTPS_EntityId RTPS_ENTITYID_SEDP_BUILTIN_TOPIC_WRITER;
  extern RTPS_EntityId RTPS_ENTITYID_SEDP_BUILTIN_TOPIC_READER;
  extern RTPS_EntityId RTPS_ENTITYID_SEDP_BUILTIN_PUBLICATIONS_WRITER;
  extern RTPS_EntityId RTPS_ENTITYID_SEDP_BUILTIN_PUBLICATIONS_READER;
  extern RTPS_EntityId RTPS_ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_WRITER;
  extern RTPS_EntityId RTPS_ENTITYID_SEDP_BUILTIN_SUBSCRIPTIONS_READER;
  extern RTPS_EntityId RTPS_ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_WRITER;
  extern RTPS_EntityId RTPS_ENTITYID_P2P_BUILTIN_PARTICIPANT_MESSAGE_READER;


  extern void                 RTPS_guidInit( RTPS_GUID       *g, 
                                             RTPS_GuidPrefix *p,
                                             RTPS_EntityId   *e);
  extern char *               RTPS_guidToString( RTPS_GUID *g,
                                                 char *buf,
                                                 int buflen);
  extern void                 RTPS_SequenceNumber_add_int(RTPS_SequenceNumber * a, int b);
  extern RTPS_SequenceNumber *RTPS_SequenceNumber_inc(RTPS_SequenceNumber * sn);

  
  struct RTPS_MessageReceiver_t;
  
  typedef struct RTPS_MessageHandler_t 
  {
    int (*handle_header_ext)(struct RTPS_MessageHandler_t      * h,
                             struct RTPS_HeaderExt_t           * msg, 
                             struct RTPS_MessageReceiver_t     * mr,
                             const char                        * raw,
                             int32_t                             raw_len );
    
    int (*handle_acknack)(struct RTPS_MessageHandler_t      * h,
                          struct RTPS_AckNack_t             * msg, 
                          struct RTPS_MessageReceiver_t     * mr,
			  const unsigned char               * raw,
                          int32_t                             raw_len );
    
    int (*handle_heartbeat)(struct RTPS_MessageHandler_t    * h,
                            struct RTPS_Heartbeat_t         * msg, 
                            struct RTPS_MessageReceiver_t   * mrw,
			    const unsigned char             * raw,
                            int32_t                           raw_len );
    
    int (*handle_gap)(struct RTPS_MessageHandler_t          * h,
                      struct RTPS_Gap_t                     * msg, 
                      struct RTPS_MessageReceiver_t         * mrw,
		      const unsigned char                   * raw,
                      int32_t                                 raw_len );
    
    int (*handle_nackfrag)(struct RTPS_MessageHandler_t     * h,
                           struct RTPS_NackFrag_t           * msg, 
                           struct RTPS_MessageReceiver_t    * mrw,
			   const unsigned char              * raw,
                           int32_t                            raw_len );
    
    int (*handle_heartbeatfrag)(struct RTPS_MessageHandler_t  * h,
                                struct RTPS_HeartbeatFrag_t   * msg, 
                                struct RTPS_MessageReceiver_t * mrw,
				const unsigned char           * raw,
                                int32_t                         raw_len );
    
    int (*handle_data)(struct RTPS_MessageHandler_t         * h,
                       struct RTPS_Data_t                   * msg, 
                       struct RTPS_MessageReceiver_t        * mr, 
                       RTPS_InstanceHandle                    writerHandle,
		       const unsigned char                  * raw,
                       int32_t                                raw_len );
    
    int (*handle_datafrag)(struct RTPS_MessageHandler_t     * h,
                           struct RTPS_DataFrag_t           * msg, 
                           struct RTPS_MessageReceiver_t    * mr, 
                           RTPS_InstanceHandle                writerHandle,
			   const unsigned char              * raw,
                           int32_t                            raw_len );
    
    int (*handle_databatch)(struct RTPS_MessageHandler_t    * h,
                            struct RTPS_DataBatch_t         * msg, 
                            struct RTPS_MessageReceiver_t   * mrw,
			    const unsigned char             * raw,
                            int32_t                           raw_len );
    
    int (*handle_secure)(struct RTPS_MessageHandler_t       * h,
                         struct RTPS_MessageReceiver_t      * mr, 
                         DDS_OctetSeq                       * enc_data,
                         DDS_OctetSeq                       * dec_data, 
                         int32_t                            * bytes_consumed,
			 const unsigned char                * raw,
                         int32_t                              raw_len );
    void * closure;
  } RTPS_MessageHandler;
  
  typedef struct RTPS_MessageReceiver_t
  {
    void     (*reset)        ( struct RTPS_MessageReceiver_t * mr,
                               RTPS_GuidPrefix * destPrefix  );
    int      (*parse_submsg) ( struct RTPS_MessageReceiver_t * mr,
                               unsigned char * submsg,
                               int32_t submsgLen );
    int      (*parse_msg)    ( struct RTPS_MessageReceiver_t * mr,
                               unsigned char * msg,
                               int32_t msgLen );
    void     (*set_source)   ( struct RTPS_MessageReceiver_t * mr,
                               struct RTPS_Locator_t * src  );

    RTPS_ProtocolVersion sourceVersion;
    RTPS_VendorId        sourceVendorId;
    RTPS_GuidPrefix      sourceGuidPrefix;
    RTPS_GuidPrefix      defaultGuidPrefix;
    RTPS_GuidPrefix      destGuidPrefix;

#if 0    
    struct list_head     unicastReplyLocatorList; /* RTPS_Locator(s) */
    struct list_head     multicastReplyLocatorList; /* RTPS_Locator(s) */
#endif
    
    unsigned char        haveTimestamp;
    struct timespec      timestamp;

    struct RTPS_MessageHandler_t * handler;
    struct RTPS_Locator_t        * src_locator;
  
    unsigned char        le; /* little endian flag for current sub-msg */
    int32_t              _debug_flags;
  } RTPS_MessageReceiver;

  
  extern void                   STUDIO_RTPS_MessageReceiver__init(RTPS_MessageReceiver * mr, 
                                                           RTPS_GuidPrefix      * destPrefix,
                                                           struct RTPS_MessageHandler_t * h, 
                                                           uint32_t               debug_flags);

#if 0  
  extern void                   STUDIO_RTPS_MessageReceiver_reset(RTPS_MessageReceiver * mr, 
                                                           RTPS_GuidPrefix      * destPrefix );
  
  extern int                    STUDIO_RTPS_MessageReceiver_parse_msg(RTPS_MessageReceiver * mr, 
                                                               unsigned char * msg, int32_t msgLen);

  extern int                    STUDIO_RTPS_MR_parse_databatch_si(RTPS_MessageReceiver     *mr, 
                                                           RTPS_DataBatchSampleInfo *si,
                                                           unsigned char     flags, 
                                                           unsigned char   * msg);
#endif  
  extern const char           * STUDIO_rmr_lookup_kind(unsigned char k);



  /* *********************************************************** */
  /* INLINE QOS SUPPORT: */
  /* *********************************************************** */

  struct RTPS_KeyHashQosPolicy
  {
    unsigned char value[16];
  };

  typedef struct RTPS_InlineQos_t
  {
    /* char                              * topic_name;   i don't think this is ever used */
    RTPS_StatusInfo                     status_info; /* this is used for lifecycle alive/unreg/disp */
    struct RTPS_KeyHashQosPolicy        key_hash;   
    DDS_SampleIdentity_t                related_sample_id; /* adds 24 bytes */

#define IQOS_STATUS_INFO_VALID       0x01
#define IQOS_KEY_HASH_VALID          0x02
#define IQOS_RELATED_SAMPLE_ID_VALID 0x04
    unsigned char                       flags;
  
  } RTPS_InlineQos;

  /*extern struct _TypeSupport RTPS_InlineQosTypeSupport; */

  extern uint32_t   RTPS_InlineQos_init( struct _TypeSupport   * ts, 
                                         RTPS_InlineQos        * iqos,
                                         unsigned char           instance_state,
                                         unsigned char         * keyhash, 
                                         DDS_SampleIdentity_t  * related_sid );

  extern uint32_t   RTPS_InlineQos_marshal_fixed_size( struct _TypeSupport * ts );
  extern uint32_t   RTPS_InlineQos_marshal( struct _TypeSupport * ts, 
                                            RTPS_InlineQos * iqos,
                                            unsigned char  * stream,
                                            unsigned char    le );
  extern uint32_t   RTPS_InlineQos_unmarshal( struct _TypeSupport * ts, 
                                              void          * data,
                                              unsigned char * stream,			       
                                              uint32_t        streamLen, 
                                              unsigned char   le );
  extern uint32_t   RTPS_InlineQos_unmarshal_get_length( unsigned char * stream, unsigned char le );
  extern void       RTPS_InlineQos_clear    ( struct _TypeSupport * ts, 
                                              void * instance );

#define PERROR( LD, FLAGS, NM, ... ) do { fprintf( stderr, __VA_ARGS__ ); } while( 0 )
#define PTRACE( LD, FLAGS, NM, FMT, ... ) do { } while( 0 )
#define PDATA( LD, FLAGS, NM, FMT, ... ) do { } while( 0 )

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
