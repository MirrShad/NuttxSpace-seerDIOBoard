/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.8 at Fri Nov 30 14:31:39 2018. */

#include "message_jack.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t rbk_protocol_Message_Jack_fields[7] = {
    PB_FIELD(  1, INT32   , SINGULAR, STATIC  , FIRST, rbk_protocol_Message_Jack, mode, mode, 0),
    PB_FIELD(  2, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Jack, state, mode, 0),
    PB_FIELD(  3, BOOL    , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Jack, enable, state, 0),
    PB_FIELD(  4, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Jack, errorCode, enable, 0),
    PB_FIELD(  5, BOOL    , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Jack, isFull, errorCode, 0),
    PB_FIELD(  6, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Jack, speed_level, isFull, 0),
    PB_LAST_FIELD
};


/* @@protoc_insertion_point(eof) */
