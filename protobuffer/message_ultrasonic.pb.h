/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.8 at Fri Dec 15 22:47:54 2017. */

#ifndef PB_RBK_PROTOCOL_MESSAGE_ULTRASONIC_PB_H_INCLUDED
#define PB_RBK_PROTOCOL_MESSAGE_ULTRASONIC_PB_H_INCLUDED
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
typedef struct _rbk_protocol_Message_UltraNode {
    int32_t id;
    double dist;
    bool valid;
    double pos_x;
    double pos_y;
    double pos_angle;
    double max_dist;
    double min_dist;
    double aperture;
    bool forbidden;
    rbk_protocol_Message_Header header;
    int32_t offset_time_ms;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_UltraNode) */
} rbk_protocol_Message_UltraNode;

typedef struct _rbk_protocol_Message_Ultrasonic {
    uint32_t cycle;
    pb_callback_t nodes;
    uint64_t base_time_ns;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_Ultrasonic) */
} rbk_protocol_Message_Ultrasonic;

/* Default values for struct fields */

/* Initializer values for message structs */
#define rbk_protocol_Message_UltraNode_init_default {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, rbk_protocol_Message_Header_init_default, 0}
#define rbk_protocol_Message_Ultrasonic_init_default {0, {{NULL}, NULL}, 0}
#define rbk_protocol_Message_UltraNode_init_zero {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, rbk_protocol_Message_Header_init_zero, 0}
#define rbk_protocol_Message_Ultrasonic_init_zero {0, {{NULL}, NULL}, 0}

/* Field tags (for use in manual encoding/decoding) */
#define rbk_protocol_Message_UltraNode_id_tag    1
#define rbk_protocol_Message_UltraNode_dist_tag  2
#define rbk_protocol_Message_UltraNode_valid_tag 3
#define rbk_protocol_Message_UltraNode_pos_x_tag 4
#define rbk_protocol_Message_UltraNode_pos_y_tag 5
#define rbk_protocol_Message_UltraNode_pos_angle_tag 6
#define rbk_protocol_Message_UltraNode_max_dist_tag 7
#define rbk_protocol_Message_UltraNode_min_dist_tag 8
#define rbk_protocol_Message_UltraNode_aperture_tag 9
#define rbk_protocol_Message_UltraNode_forbidden_tag 10
#define rbk_protocol_Message_UltraNode_header_tag 11
#define rbk_protocol_Message_UltraNode_offset_time_ms_tag 12
#define rbk_protocol_Message_Ultrasonic_cycle_tag 1
#define rbk_protocol_Message_Ultrasonic_nodes_tag 2
#define rbk_protocol_Message_Ultrasonic_base_time_ns_tag 3

/* Struct field encoding specification for nanopb */
extern const pb_field_t rbk_protocol_Message_UltraNode_fields[13];
extern const pb_field_t rbk_protocol_Message_Ultrasonic_fields[4];

/* Maximum encoded size of messages (where known) */
#define rbk_protocol_Message_UltraNode_size      113
/* rbk_protocol_Message_Ultrasonic_size depends on runtime parameters */

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define MESSAGE_ULTRASONIC_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif