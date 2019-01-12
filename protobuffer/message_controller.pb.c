/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.8 at Mon Dec 17 13:37:12 2018. */

#include "message_controller.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t rbk_protocol_Message_Controller_fields[10] = {
    PB_FIELD(  1, DOUBLE  , SINGULAR, STATIC  , FIRST, rbk_protocol_Message_Controller, temp, temp, 0),
    PB_FIELD(  2, DOUBLE  , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Controller, humi, temp, 0),
    PB_FIELD(  3, DOUBLE  , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Controller, voltage, humi, 0),
    PB_FIELD(  4, BOOL    , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Controller, emc, voltage, 0),
    PB_FIELD(  5, BOOL    , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Controller, brake, emc, 0),
    PB_FIELD(  6, BOOL    , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Controller, driverEmc, brake, 0),
    PB_FIELD(  7, BOOL    , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Controller, manualCharge, driverEmc, 0),
    PB_FIELD(  8, BOOL    , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Controller, autoCharge, manualCharge, 0),
    PB_FIELD(  9, BOOL    , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_Controller, electric, autoCharge, 0),
    PB_LAST_FIELD
};


/* On some platforms (such as AVR), double is really float.
 * These are not directly supported by nanopb, but see example_avr_double.
 * To get rid of this error, remove any double fields from your .proto.
 */
PB_STATIC_ASSERT(sizeof(double) == 8, DOUBLE_MUST_BE_8_BYTES)

/* @@protoc_insertion_point(eof) */
