/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.8 at Fri Dec 22 13:55:00 2017. */

#include "message_rfid.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t rbk_protocol_Message_RFIDNode_fields[5] = {
    PB_FIELD(  1, UINT32  , SINGULAR, STATIC  , FIRST, rbk_protocol_Message_RFIDNode, id, id, 0),
    PB_FIELD(  2, UINT32  , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_RFIDNode, count, id, 0),
    PB_FIELD(  3, MESSAGE , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_RFIDNode, header, count, &rbk_protocol_Message_Header_fields),
    PB_FIELD(  4, UINT32  , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_RFIDNode, strength, header, 0),
    PB_LAST_FIELD
};

const pb_field_t rbk_protocol_Message_RFID_fields[2] = {
    PB_FIELD(  1, MESSAGE , REPEATED, STATIC  , FIRST, rbk_protocol_Message_RFID, rfid_nodes, rfid_nodes, &rbk_protocol_Message_RFIDNode_fields),
    PB_LAST_FIELD
};


/* Check that field information fits in pb_field_t */
#if !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_32BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in 8 or 16 bit
 * field descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(rbk_protocol_Message_RFIDNode, header) < 65536 && pb_membersize(rbk_protocol_Message_RFID, rfid_nodes[0]) < 65536), YOU_MUST_DEFINE_PB_FIELD_32BIT_FOR_MESSAGES_rbk_protocol_Message_RFIDNode_rbk_protocol_Message_RFID)
#endif

#if !defined(PB_FIELD_16BIT) && !defined(PB_FIELD_32BIT)
/* If you get an error here, it means that you need to define PB_FIELD_16BIT
 * compile-time option. You can do that in pb.h or on compiler command line.
 * 
 * The reason you need to do this is that some of your messages contain tag
 * numbers or field sizes that are larger than what can fit in the default
 * 8 bit descriptors.
 */
PB_STATIC_ASSERT((pb_membersize(rbk_protocol_Message_RFIDNode, header) < 256 && pb_membersize(rbk_protocol_Message_RFID, rfid_nodes[0]) < 256), YOU_MUST_DEFINE_PB_FIELD_16BIT_FOR_MESSAGES_rbk_protocol_Message_RFIDNode_rbk_protocol_Message_RFID)
#endif


/* @@protoc_insertion_point(eof) */
