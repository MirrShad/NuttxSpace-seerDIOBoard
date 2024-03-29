/* Automatically generated nanopb header */
/* Generated by nanopb-0.3.8 at Thu Jan 03 17:44:28 2019. */

#ifndef PB_RBK_PROTOCOL_MESSAGE_LIFTRET_PB_H_INCLUDED
#define PB_RBK_PROTOCOL_MESSAGE_LIFTRET_PB_H_INCLUDED
#include <pb.h>

/* @@protoc_insertion_point(includes) */
#if PB_PROTO_HEADER_VERSION != 30
#error Regenerate this file with the current version of nanopb generator.
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Struct definitions */
typedef struct _rbk_protocol_Message_Lift_Ret {
    int32_t retAction;
    bool toLimitFini;
    bool spinDegreeFini;
/* @@protoc_insertion_point(struct:rbk_protocol_Message_Lift_Ret) */
} rbk_protocol_Message_Lift_Ret;

/* Default values for struct fields */

/* Initializer values for message structs */
#define rbk_protocol_Message_Lift_Ret_init_default {0, 0, 0}
#define rbk_protocol_Message_Lift_Ret_init_zero  {0, 0, 0}

/* Field tags (for use in manual encoding/decoding) */
#define rbk_protocol_Message_Lift_Ret_retAction_tag 1
#define rbk_protocol_Message_Lift_Ret_toLimitFini_tag 2
#define rbk_protocol_Message_Lift_Ret_spinDegreeFini_tag 3

/* Struct field encoding specification for nanopb */
extern const pb_field_t rbk_protocol_Message_Lift_Ret_fields[4];

/* Maximum encoded size of messages (where known) */
#define rbk_protocol_Message_Lift_Ret_size       15

/* Message IDs (where set with "msgid" option) */
#ifdef PB_MSGID

#define MESSAGE_LIFTRET_MESSAGES \


#endif

#ifdef __cplusplus
} /* extern "C" */
#endif
/* @@protoc_insertion_point(eof) */

#endif
