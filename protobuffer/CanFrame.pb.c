/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.8 at Tue Jun 06 21:34:09 2017. */

#include "CanFrame.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t CanFrame_fields[9] = {
    PB_FIELD(  1, UINT32  , SINGULAR, STATIC  , FIRST, CanFrame, ID, ID, 0),
    PB_FIELD(  2, BOOL    , SINGULAR, STATIC  , OTHER, CanFrame, Extended, ID, 0),
    PB_FIELD(  3, BOOL    , SINGULAR, STATIC  , OTHER, CanFrame, Remote, Extended, 0),
    PB_FIELD(  4, UINT32  , SINGULAR, STATIC  , OTHER, CanFrame, DLC, Remote, 0),
    PB_FIELD(  5, BYTES   , SINGULAR, STATIC  , OTHER, CanFrame, Data, DLC, 0),
    PB_FIELD(  6, UINT32  , SINGULAR, STATIC  , OTHER, CanFrame, Channel, Data, 0),
    PB_FIELD(  7, UINT32  , SINGULAR, STATIC  , OTHER, CanFrame, Timestamp, Channel, 0),
    PB_FIELD(  8, BOOL    , SINGULAR, STATIC  , OTHER, CanFrame, Direction, Timestamp, 0),
    PB_LAST_FIELD
};



/* @@protoc_insertion_point(eof) */
