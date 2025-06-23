#ifndef STUDIO_RTPS_SUBMSGS_H
#define STUDIO_RTPS_SUBMSGS_H

#include "list.h"
#include "rtps_base.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */


  typedef struct RTPS_MessageHeader_t
  {
    char                 tag[4];
    RTPS_ProtocolVersion version;
    RTPS_VendorId        vendorId;
    RTPS_GuidPrefix      guidPrefix;
  } RTPS_MessageHeader;

  
  typedef struct RTPS_SubmessageHeader_t
  {
    RTPS_Octet     submessageId;
    RTPS_Octet     flags;
    unsigned short octetsToNextHeader;
  } RTPS_SubmessageHeader;

  typedef enum RTPS_SubmessageKind_t {
    RTPS_HEADER_EXT      = 0x00, /* Header Extension */
    RTPS_PAD             = 0x01, /* Pad */
    RTPS_DATA_v1         = 0x02, /* Data      (old: RTPS 1) */
    RTPS_NOKEY_DATA_v1   = 0x03, /* NoKeyData (old: RTPS 1) */


    RTPS_ACKNACK         = 0x06, /* AckNack   */
    RTPS_HEARTBEAT       = 0x07, /* Heartbeat */
    RTPS_GAP             = 0x08, /* Gap       */
    RTPS_INFO_TS         = 0x09, /* InfoTimestamp */


    RTPS_INFO_SRC        = 0x0c, /* InfoSource */
    RTPS_INFO_REPLY_IP4  = 0x0d, /* InfoReplyIp4 */
    RTPS_INFO_DST        = 0x0e, /* InfoDestination */
    RTPS_INFO_REPLY      = 0x0f, /* InfoReply */
    RTPS_DATA_FRAG_v1       = 0x10, /* DataFrag (old: RTPS 1) */
    RTPS_NOKEY_DATA_FRAG_v1 = 0x11, /* NoKeyDataFrag */
    RTPS_NACK_FRAG       = 0x12, /* NackFrag */
    RTPS_HEARTBEAT_FRAG  = 0x13, /* HeartbeatFrag */

    RTPS_DATA            = 0x15, /* Data */
    RTPS_DATA_FRAG       = 0x16, /* DataFrag */
    RTPS_ACKNACK_BATCH   = 0x17,
    RTPS_DATA_BATCH      = 0x18,
    RTPS_HEARTBEAT_BATCH = 0x19,

    RTPS_SECURE          = 0x30, /* SecureBodySubMsg        */
    RTPS_SEC_PREFIX      = 0x31, /* SecurePrefixSubMsg      */
    RTPS_SEC_POSTFIX     = 0x32, /* SecurePostfixSubMsg     */
    RTPS_SRTPS_PREFIX    = 0x33, /* SecureRTPSPrefixSubMsg  */
    RTPS_SRTPS_POSTFIX   = 0x34, /* SecureRTPSPostfixSubMsg */

    /* CoreDX DDS Extension -- try to get into next revision of spec */
    RTPS_MSG_LEN         = 0x80,  /* MsgLen (if present, must immediately follow RTPS MSGHEADER) specifies entire length of msg */
    RTPS_LOC_LIST        = 0x81,
    RTPS_MSG_CHKSUM      = 0x82   /* crc32 checksum (for now) */
  } RTPS_SubmessageKind;

  typedef struct RTPS_HeaderExt_t
  {
    uint8_t              ext_flags;
    uint32_t             ext_msg_len;
    struct timespec      ext_tx_timestamp;
    uint32_t             ext_4;
    uint64_t             ext_8;
    uint64_t             ext_checksum;
    // ParameterSeq      ext_parameters;
  } RTPS_HeaderExt;

  typedef struct RTPS_Data_t
  {
    RTPS_EntityId       readerId;
    RTPS_EntityId       writerId;
    RTPS_SequenceNumber writerSN;

    /* inlineQos */
    unsigned char *     inlineQosData; /* marshalled/raw */
    uint32_t            inlineQosDataLen; 

    /* serialized data */
    unsigned char       data_is_key;
    unsigned char *     data;
    uint32_t            dataLen;
  } RTPS_Data;

  typedef struct RTPS_DataFrag_t
  {
    RTPS_EntityId       readerId;
    RTPS_EntityId       writerId;
    RTPS_SequenceNumber writerSN; 

    /* inlineQos */
    unsigned char *     inlineQosData; /* marshalled/raw */
    uint32_t            inlineQosDataLen; 

    /* serialized data */
    uint32_t            sampleSize;  /* total sample size (all frags) */
    uint32_t            fragmentStartingNum;
    unsigned short      fragmentsInSubmessage;
    unsigned short      fragmentSize; /* each frag must be same size */
  
    unsigned char     * data;
    uint32_t            dataLen; /* this message's fragment[s] */
    unsigned char       data_is_key;
  } RTPS_DataFrag;

  typedef struct RTPS_AckNack_t
  {
    RTPS_EntityId          readerId;
    RTPS_EntityId          writerId;
    RTPS_SequenceNumberSet readerSNState;
    RTPS_Count             count;
    unsigned char          finalFlag;
    unsigned char          pauseFlag;
#define ACKNACK_FINAL_FLAG   0x02
#define ACKNACK_PAUSE_FLAG   0x04
  } RTPS_AckNack;

  typedef struct RTPS_Heartbeat_t
  {
    RTPS_EntityId       readerId;
    RTPS_EntityId       writerId;
    RTPS_SequenceNumber firstSN;
    RTPS_SequenceNumber lastSN;
    RTPS_Count          count;
    unsigned char       finalFlag;
    unsigned char       livelinessFlag;
#define HB_FINAL_FLAG      0x02
#define HB_LIVELINESS_FLAG 0x04
  } RTPS_Heartbeat;

  typedef struct RTPS_Gap_t
  {
    RTPS_EntityId          readerId;
    RTPS_EntityId          writerId;
    RTPS_SequenceNumber    gapStart;
    RTPS_SequenceNumberSet gapList;  
  } RTPS_Gap;

  typedef struct RTPS_HeartbeatBatch_t
  {
    RTPS_EntityId       readerId;
    RTPS_EntityId       writerId;
    RTPS_SequenceNumber firstBatchSN;
    RTPS_SequenceNumber lastBatchSN;
    RTPS_SequenceNumber firstSN;
    RTPS_SequenceNumber lastSN;
    RTPS_Count          count;
    unsigned char       livelinessFlag; 
    unsigned char       finalFlag; 
  } RTPS_HeartbeatBatch;

  typedef struct RTPS_AckNackBatch_t
  {
    RTPS_EntityId          readerId;
    RTPS_EntityId          writerId;
    RTPS_SequenceNumberSet readerSNState;
    RTPS_Count             count;
    unsigned char          finalFlag;
  } RTPS_AckNackBatch;

  typedef struct RTPS_DataBatch_t
  {
    /* byte: DATA_BATCH */
    /* byte: flags xxxxxxQE */
    unsigned char       flags;
    RTPS_EntityId       readerId;
    RTPS_EntityId       writerId;

    RTPS_SequenceNumber batchSN;
    RTPS_SequenceNumber firstSN;
    uint32_t            snDelta;
    uint32_t            sampleCount;

    unsigned char     * sampleInfos;
    unsigned char       encoding;
    unsigned char     * samples;

    /* SampleInfo[s] */
    /*    uint16_t  FLAGS xxxxxxxxxxKIDOQT */
    /*    uint16_t  octetsToInlineQos */
    /*    uint32_t  dataLen */
    /*    Timestamp (2*int32_t) if 'T' */
    /*    uint32_t  offsetSN if 'O' */
    /*    InlineQos if 'Q' */

    /* uint16_t sampleEncoding */

    /* uint16_t pad */
    /* Sample[s] */
    /*   DATA if 'D' */
    /*   KEY if 'K' */

  } RTPS_DataBatch;


  typedef struct RTPS_DataBatchSampleInfo_t
  {
#define RTPS_DATABATCH_FLAGS_T 0x0001  
#define RTPS_DATABATCH_FLAGS_Q 0x0002
#define RTPS_DATABATCH_FLAGS_O 0x0004
#define RTPS_DATABATCH_FLAGS_D 0x0008
#define RTPS_DATABATCH_FLAGS_K 0x0020

    uint16_t        flags; /*  xxxxxxxxxxKIDOQT  */
    uint16_t        octetsToInlineQos; 
    uint32_t        dataLen;
    struct timespec timestamp;    /* if flags[T] */
    uint32_t        sn_offset;    /* if flags[O] */
    unsigned char * inline_qos;   /* if flags[Q] */
    uint32_t        inline_qos_len; 
  } RTPS_DataBatchSampleInfo;

  typedef struct RTPS_InfoTimestamp_t
  {
    RTPS_Timestamp value;
  } RTPS_InfoTimestamp;

  typedef struct RTPS_InfoSource_t
  {
    RTPS_ProtocolVersion protocolVersion;
    RTPS_VendorId        vendorId;
    RTPS_GuidPrefix      guidPrefix;
  } RTPS_InfoSource;

  typedef struct RTPS_InfoReply_t
  {
    struct list_head  multicastLocatorList;
    struct list_head  unicastLocatorList;
  } RTPS_InfoReply;

  typedef struct RTPS_InfoDestination_t
  { 
    RTPS_GuidPrefix guidPrefix;
  } RTPS_InfoDestination;


  typedef struct RTPS_NackFrag_t
  {
    RTPS_EntityId          readerId;
    RTPS_EntityId          writerId;
    RTPS_SequenceNumberSet fragmentNumberState;
    RTPS_Count             count;
    RTPS_SequenceNumber    writerSN;
  } RTPS_NackFrag;

  typedef struct RTPS_HeartbeatFrag_t
  {
    RTPS_EntityId         readerId;
    RTPS_EntityId         writerId;
    RTPS_SequenceNumber   writerSN;
    RTPS_FragmentNumber_t lastFragmentNum;
    RTPS_Count            count;
  } RTPS_HeartbeatFrag;

  typedef struct RTPS_MsgLen_t
  {
    uint64_t       msgLen; 
  } RTPS_MsgLen;


  typedef struct RTPS_SecPrefix_t
  {
    uint8_t dummy;
  } RTPS_SecPrefix;
  
#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
