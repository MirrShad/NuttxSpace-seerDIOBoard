/* Automatically generated nanopb constant definitions */
/* Generated by nanopb-0.3.9.1 at Mon Jul 16 09:54:50 2018. */

#include "message_chassisconfig.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif



const pb_field_t rbk_protocol_Message_ChassisConfig_fields[8] = {
    PB_FIELD(  1, FLOAT   , SINGULAR, STATIC  , FIRST, rbk_protocol_Message_ChassisConfig, E, E, 0),
    PB_FIELD(  2, FLOAT   , REPEATED, STATIC  , OTHER, rbk_protocol_Message_ChassisConfig, R, E, 0),
    PB_FIELD(  3, INT32   , REPEATED, STATIC  , OTHER, rbk_protocol_Message_ChassisConfig, driveID, R, 0),
    PB_FIELD(  4, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_ChassisConfig, encoderLines, driveID, 0),
    PB_FIELD(  5, FLOAT   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_ChassisConfig, reductionRatio, encoderLines, 0),
    PB_FIELD(  6, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_ChassisConfig, motorMaxSpeedRpm, reductionRatio, 0),
    PB_FIELD(  7, BOOL    , REPEATED, STATIC  , OTHER, rbk_protocol_Message_ChassisConfig, Inverse, motorMaxSpeedRpm, 0),
    PB_LAST_FIELD
};

const pb_field_t rbk_protocol_Message_ChassisDemandConfig_fields[11] = {
    PB_FIELD(  1, INT32   , SINGULAR, STATIC  , FIRST, rbk_protocol_Message_ChassisDemandConfig, bChassisExist, bChassisExist, 0),
    PB_FIELD(  2, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_ChassisDemandConfig, bBatteryExist, bChassisExist, 0),
    PB_FIELD(  3, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_ChassisDemandConfig, bGyroExist, bBatteryExist, 0),
    PB_FIELD(  4, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_ChassisDemandConfig, bSubsystemExist, bGyroExist, 0),
    PB_FIELD(  5, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_ChassisDemandConfig, bSonicExist, bSubsystemExist, 0),
    PB_FIELD(  6, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_ChassisDemandConfig, bRFIDExist, bSonicExist, 0),
    PB_FIELD(  7, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_ChassisDemandConfig, bMagneticSenserExist, bRFIDExist, 0),
    PB_FIELD(  8, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_ChassisDemandConfig, bIntBrdExist, bMagneticSenserExist, 0),
    PB_FIELD(  9, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_ChassisDemandConfig, chassisType, bIntBrdExist, 0),
    PB_FIELD( 10, INT32   , SINGULAR, STATIC  , OTHER, rbk_protocol_Message_ChassisDemandConfig, driverBrand, chassisType, 0),
    PB_LAST_FIELD
};


/* @@protoc_insertion_point(eof) */
