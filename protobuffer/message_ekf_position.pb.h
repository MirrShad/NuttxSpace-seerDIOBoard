/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.8 at Thu Oct 26 10:10:37 2017. */

#ifndef PB_RBK_PROTOCOL_MESSAGE_EKF_POSITION_PB_H_INCLUDED
#define PB_RBK_PROTOCOL_MESSAGE_EKF_POSITION_PB_H_INCLUDED
#include <pb.h>

#include "message_header.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Struct definitions */
typedef struct _rbk_protocol_Message_Ekf_Position {
    rbk_protocol_Message_Header header;
    float x;
    float y;
    float angle;
    float steer_angle;
    float vel_x;
    float vel_y;
    float vel_rotate;
    float odom_x;
    float odom_y;
    float odom_angle;
    float imu_yaw;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_Ekf_Position) */
} rbk_protocol_Message_Ekf_Position;

/* Default values for struct fields */

/* Initializer values for message structs */
#define rbk_protocol_Message_Ekf_Position_init_default {rbk_protocol_Message_Header_init_default, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define rbk_protocol_Message_Ekf_Position_init_zero {rbk_protocol_Message_Header_init_zero, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define rbk_protocol_Message_Ekf_Position_header_tag 1
#define rbk_protocol_Message_Ekf_Position_x_tag  2
#define rbk_protocol_Message_Ekf_Position_y_tag  3
#define rbk_protocol_Message_Ekf_Position_angle_tag 4
#define rbk_protocol_Message_Ekf_Position_steer_angle_tag 5
#define rbk_protocol_Message_Ekf_Position_vel_x_tag 6
#define rbk_protocol_Message_Ekf_Position_vel_y_tag 7
#define rbk_protocol_Message_Ekf_Position_vel_rotate_tag 8
#define rbk_protocol_Message_Ekf_Position_odom_x_tag 9
#define rbk_protocol_Message_Ekf_Position_odom_y_tag 10
#define rbk_protocol_Message_Ekf_Position_odom_angle_tag 11
#define rbk_protocol_Message_Ekf_Position_imu_yaw_tag 12

/* Struct field encoding specification for nanopb */
extern const pb_field_t rbk_protocol_Message_Ekf_Position_fields[13];

/* Maximum encoded size of messages (where known) */
#define rbk_protocol_Message_Ekf_Position_size   79

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define MESSAGE_EKF_POSITION_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
