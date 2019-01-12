/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.8 at Thu Jan 03 17:46:20 2019. */

#ifndef PB_RBK_PROTOCOL_MESSAGE_NAVIGATION_PB_H_INCLUDED
#define PB_RBK_PROTOCOL_MESSAGE_NAVIGATION_PB_H_INCLUDED
#include <pb.h>

#include "message_header.pb.h"

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Enum definitions */
typedef enum _rbk_protocol_Message_NavTarget_MODE {
    rbk_protocol_Message_NavTarget_MODE_NULL_MODE = 0,
    rbk_protocol_Message_NavTarget_MODE_FORWARD_MODE = 1,
    rbk_protocol_Message_NavTarget_MODE_BACK_MODE = 2
} rbk_protocol_Message_NavTarget_MODE;
#define _rbk_protocol_Message_NavTarget_MODE_MIN rbk_protocol_Message_NavTarget_MODE_NULL_MODE
#define _rbk_protocol_Message_NavTarget_MODE_MAX rbk_protocol_Message_NavTarget_MODE_BACK_MODE
#define _rbk_protocol_Message_NavTarget_MODE_ARRAYSIZE ((rbk_protocol_Message_NavTarget_MODE)(rbk_protocol_Message_NavTarget_MODE_BACK_MODE+1))

typedef enum _rbk_protocol_Message_NavInfo_NavMode {
    rbk_protocol_Message_NavInfo_NavMode_NullNavMode = 0,
    rbk_protocol_Message_NavInfo_NavMode_TaskTargetReachMode = 1,
    rbk_protocol_Message_NavInfo_NavMode_SpeedControlMode = 2,
    rbk_protocol_Message_NavInfo_NavMode_TopoPosReachMode = 3
} rbk_protocol_Message_NavInfo_NavMode;
#define _rbk_protocol_Message_NavInfo_NavMode_MIN rbk_protocol_Message_NavInfo_NavMode_NullNavMode
#define _rbk_protocol_Message_NavInfo_NavMode_MAX rbk_protocol_Message_NavInfo_NavMode_TopoPosReachMode
#define _rbk_protocol_Message_NavInfo_NavMode_ARRAYSIZE ((rbk_protocol_Message_NavInfo_NavMode)(rbk_protocol_Message_NavInfo_NavMode_TopoPosReachMode+1))

typedef enum _rbk_protocol_Message_NavInfo_NavCmd {
    rbk_protocol_Message_NavInfo_NavCmd_NULLNavCmd = 0,
    rbk_protocol_Message_NavInfo_NavCmd_TaskCancel = 1,
    rbk_protocol_Message_NavInfo_NavCmd_TaskSuspend = 2,
    rbk_protocol_Message_NavInfo_NavCmd_TaskResume = 3,
    rbk_protocol_Message_NavInfo_NavCmd_TaskBegin = 4
} rbk_protocol_Message_NavInfo_NavCmd;
#define _rbk_protocol_Message_NavInfo_NavCmd_MIN rbk_protocol_Message_NavInfo_NavCmd_NULLNavCmd
#define _rbk_protocol_Message_NavInfo_NavCmd_MAX rbk_protocol_Message_NavInfo_NavCmd_TaskBegin
#define _rbk_protocol_Message_NavInfo_NavCmd_ARRAYSIZE ((rbk_protocol_Message_NavInfo_NavCmd)(rbk_protocol_Message_NavInfo_NavCmd_TaskBegin+1))

/* Struct definitions */
typedef struct _rbk_protocol_Message_ManualSpeed {
    double x;
    double y;
    double rotate;
    double steer_angle;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_ManualSpeed) */
} rbk_protocol_Message_ManualSpeed;

typedef struct _rbk_protocol_Message_NavInfo {
    int32_t nav_mode;
    int32_t nav_cmd;
    double nav_target_x;
    double nav_target_y;
    double nav_target_theta;
    double nav_target_mode;
    double nav_speed_x;
    double nav_speed_y;
    double nav_speed_w;
    int32_t topo_target_id;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_NavInfo) */
} rbk_protocol_Message_NavInfo;

typedef struct _rbk_protocol_Message_NavPath {
    pb_callback_t states;
    bool find_path;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_NavPath) */
} rbk_protocol_Message_NavPath;

typedef struct _rbk_protocol_Message_NavPose {
    double x;
    double y;
    double angle;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_NavPose) */
} rbk_protocol_Message_NavPose;

typedef struct _rbk_protocol_Message_NavSpeed {
    double x;
    double y;
    double rotate;
    rbk_protocol_Message_Header header;
    double steer_angle;
    double spin_speed;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_NavSpeed) */
} rbk_protocol_Message_NavSpeed;

typedef struct _rbk_protocol_Message_NavStatus {
    bool blocked;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_NavStatus) */
} rbk_protocol_Message_NavStatus;

typedef struct _rbk_protocol_Message_NavTarget {
    double x;
    double y;
    double angle;
    int32_t run_mode;
    int32_t topo_target_id;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_NavTarget) */
} rbk_protocol_Message_NavTarget;

typedef struct _rbk_protocol_Message_NavTopoPose {
    int32_t id;
    double angle;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_NavTopoPose) */
} rbk_protocol_Message_NavTopoPose;

typedef struct _rbk_protocol_Message_NavState {
    rbk_protocol_Message_NavPose pose;
    rbk_protocol_Message_NavSpeed speed;
    double radius;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_NavState) */
} rbk_protocol_Message_NavState;

/* Default values for struct fields */

/* Initializer values for message structs */
#define rbk_protocol_Message_NavStatus_init_default {0}
#define rbk_protocol_Message_NavSpeed_init_default {0, 0, 0, rbk_protocol_Message_Header_init_default, 0, 0}
#define rbk_protocol_Message_ManualSpeed_init_default {0, 0, 0, 0}
#define rbk_protocol_Message_NavPose_init_default {0, 0, 0}
#define rbk_protocol_Message_NavTopoPose_init_default {0, 0}
#define rbk_protocol_Message_NavState_init_default {rbk_protocol_Message_NavPose_init_default, rbk_protocol_Message_NavSpeed_init_default, 0}
#define rbk_protocol_Message_NavPath_init_default {{{NULL}, NULL}, 0}
#define rbk_protocol_Message_NavTarget_init_default {0, 0, 0, 0, 0}
#define rbk_protocol_Message_NavInfo_init_default {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
#define rbk_protocol_Message_NavStatus_init_zero {0}
#define rbk_protocol_Message_NavSpeed_init_zero  {0, 0, 0, rbk_protocol_Message_Header_init_zero, 0, 0}
#define rbk_protocol_Message_ManualSpeed_init_zero {0, 0, 0, 0}
#define rbk_protocol_Message_NavPose_init_zero   {0, 0, 0}
#define rbk_protocol_Message_NavTopoPose_init_zero {0, 0}
#define rbk_protocol_Message_NavState_init_zero  {rbk_protocol_Message_NavPose_init_zero, rbk_protocol_Message_NavSpeed_init_zero, 0}
#define rbk_protocol_Message_NavPath_init_zero   {{{NULL}, NULL}, 0}
#define rbk_protocol_Message_NavTarget_init_zero {0, 0, 0, 0, 0}
#define rbk_protocol_Message_NavInfo_init_zero   {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define rbk_protocol_Message_ManualSpeed_x_tag   1
#define rbk_protocol_Message_ManualSpeed_y_tag   2
#define rbk_protocol_Message_ManualSpeed_rotate_tag 3
#define rbk_protocol_Message_ManualSpeed_steer_angle_tag 4
#define rbk_protocol_Message_NavInfo_nav_mode_tag 1
#define rbk_protocol_Message_NavInfo_nav_cmd_tag 2
#define rbk_protocol_Message_NavInfo_nav_target_x_tag 3
#define rbk_protocol_Message_NavInfo_nav_target_y_tag 4
#define rbk_protocol_Message_NavInfo_nav_target_theta_tag 5
#define rbk_protocol_Message_NavInfo_nav_target_mode_tag 6
#define rbk_protocol_Message_NavInfo_nav_speed_x_tag 7
#define rbk_protocol_Message_NavInfo_nav_speed_y_tag 8
#define rbk_protocol_Message_NavInfo_nav_speed_w_tag 9
#define rbk_protocol_Message_NavInfo_topo_target_id_tag 10
#define rbk_protocol_Message_NavPath_states_tag  1
#define rbk_protocol_Message_NavPath_find_path_tag 2
#define rbk_protocol_Message_NavPose_x_tag       1
#define rbk_protocol_Message_NavPose_y_tag       2
#define rbk_protocol_Message_NavPose_angle_tag   3
#define rbk_protocol_Message_NavSpeed_x_tag      1
#define rbk_protocol_Message_NavSpeed_y_tag      2
#define rbk_protocol_Message_NavSpeed_rotate_tag 3
#define rbk_protocol_Message_NavSpeed_header_tag 4
#define rbk_protocol_Message_NavSpeed_steer_angle_tag 5
#define rbk_protocol_Message_NavSpeed_spin_speed_tag 6
#define rbk_protocol_Message_NavStatus_blocked_tag 1
#define rbk_protocol_Message_NavTarget_x_tag     1
#define rbk_protocol_Message_NavTarget_y_tag     2
#define rbk_protocol_Message_NavTarget_angle_tag 3
#define rbk_protocol_Message_NavTarget_run_mode_tag 4
#define rbk_protocol_Message_NavTarget_topo_target_id_tag 5
#define rbk_protocol_Message_NavTopoPose_id_tag  1
#define rbk_protocol_Message_NavTopoPose_angle_tag 3
#define rbk_protocol_Message_NavState_pose_tag   1
#define rbk_protocol_Message_NavState_speed_tag  2
#define rbk_protocol_Message_NavState_radius_tag 3

/* Struct field encoding specification for nanopb */
extern const pb_field_t rbk_protocol_Message_NavStatus_fields[2];
extern const pb_field_t rbk_protocol_Message_NavSpeed_fields[7];
extern const pb_field_t rbk_protocol_Message_ManualSpeed_fields[5];
extern const pb_field_t rbk_protocol_Message_NavPose_fields[4];
extern const pb_field_t rbk_protocol_Message_NavTopoPose_fields[3];
extern const pb_field_t rbk_protocol_Message_NavState_fields[4];
extern const pb_field_t rbk_protocol_Message_NavPath_fields[3];
extern const pb_field_t rbk_protocol_Message_NavTarget_fields[6];
extern const pb_field_t rbk_protocol_Message_NavInfo_fields[11];

/* Maximum encoded size of messages (where known) */
#define rbk_protocol_Message_NavStatus_size      2
#define rbk_protocol_Message_NavSpeed_size       69
#define rbk_protocol_Message_ManualSpeed_size    36
#define rbk_protocol_Message_NavPose_size        27
#define rbk_protocol_Message_NavTopoPose_size    20
#define rbk_protocol_Message_NavState_size       109
/* rbk_protocol_Message_NavPath_size depends on runtime parameters */
#define rbk_protocol_Message_NavTarget_size      49
#define rbk_protocol_Message_NavInfo_size        96

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define MESSAGE_NAVIGATION_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
