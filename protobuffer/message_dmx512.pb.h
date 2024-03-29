/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.9 at Tue Nov 13 14:00:50 2018. */

#ifndef PB_RBK_PROTOCOL_MESSAGE_DMX512_PB_H_INCLUDED
#define PB_RBK_PROTOCOL_MESSAGE_DMX512_PB_H_INCLUDED
#include <pb.h>

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Struct definitions */
typedef struct _rbk_protocol_Message_Dmx512 {
    uint32_t type;
    uint32_t battery;
    uint32_t color_r;
    uint32_t color_g;
    uint32_t color_b;
    uint32_t color_w;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_Dmx512) */
} rbk_protocol_Message_Dmx512;

/* Default values for struct fields */

/* Initializer values for message structs */
#define rbk_protocol_Message_Dmx512_init_default {0, 0, 0, 0, 0, 0}
#define rbk_protocol_Message_Dmx512_init_zero    {0, 0, 0, 0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define rbk_protocol_Message_Dmx512_type_tag     1
#define rbk_protocol_Message_Dmx512_battery_tag  2
#define rbk_protocol_Message_Dmx512_color_r_tag  3
#define rbk_protocol_Message_Dmx512_color_g_tag  4
#define rbk_protocol_Message_Dmx512_color_b_tag  5
#define rbk_protocol_Message_Dmx512_color_w_tag  6

/* Struct field encoding specification for nanopb */
extern const pb_field_t rbk_protocol_Message_Dmx512_fields[7];

/* Maximum encoded size of messages (where known) */
#define rbk_protocol_Message_Dmx512_size         36

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define MESSAGE_DMX512_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
