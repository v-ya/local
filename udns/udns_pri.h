#ifndef _udns_pri_h_
#define _udns_pri_h_

#include "udns.h"
#include <vattr.h>

#define udns_type_max       128
#define udns_type_name_max  16

struct udns_type_arg_stack_t {
	uintptr_t used[256];
};

typedef void (*udns_type_arg_do_f)(struct udns_type_arg_stack_t *restrict arg);
typedef uintptr_t (*udns_type_parse_length_f)(const uint8_t *restrict data, uintptr_t length, struct udns_type_arg_stack_t *restrict arg);
typedef uintptr_t (*udns_type_build_length_f)(const char *parse, struct udns_type_arg_stack_t *restrict arg);
typedef struct udns_type_arg_stack_t* (*udns_type_parse_write_f)(char *restrict parse, struct udns_type_arg_stack_t *restrict arg);
typedef struct udns_type_arg_stack_t* (*udns_type_build_write_f)(uint8_t *restrict data, struct udns_type_arg_stack_t *restrict arg);

typedef enum udns_header_flags_t {
	udns_header_flags_qr          = 0x8000,
	udns_header_flags_opcode_mask = 0x7800,
	udns_header_flags_aa          = 0x0400,
	udns_header_flags_tc          = 0x0200,
	udns_header_flags_rd          = 0x0100,
	udns_header_flags_ra          = 0x0080,
	udns_header_flags_z_mask      = 0x0070,
	udns_header_flags_rcode_mask  = 0x000f,
} udns_header_flags_t;

typedef struct udns_type_func_t {
	char type_name[udns_type_name_max];
	udns_type_arg_do_f initial;
	udns_type_arg_do_f finaly;
	udns_type_parse_length_f parse_length;
	udns_type_build_length_f build_length;
	udns_type_parse_write_f parse_write;
	udns_type_build_write_f build_write;
} udns_type_func_t;

struct udns_inst_s {
	udns_type_func_t func[udns_type_max];
};

struct udns_s {
	udns_inst_s *inst;
	vattr_s *question;
	vattr_s *answer;
	vattr_s *authority;
	vattr_s *additional;
	uintptr_t n_question;
	uintptr_t n_answer;
	uintptr_t n_authority;
	uintptr_t n_additional;
	uint32_t id;
	uint32_t flags;
};

typedef struct udns_header_t {
	uint16_t id;
	uint16_t flags;
	uint16_t n_question;
	uint16_t n_answer;
	uint16_t n_authority;
	uint16_t n_additional;
} __attribute__((packed)) udns_header_t;

const udns_type_func_t* udns_inst_inner_get_func(udns_inst_s *restrict inst, udns_type_t type);

udns_question_s* udns_question_inner_parse(const uint8_t *restrict data, uintptr_t size, uintptr_t *restrict pos);
uintptr_t udns_question_inner_skip(const uint8_t *restrict data, uintptr_t size, uintptr_t *restrict pos, udns_type_t *restrict type);

udns_resource_s* udns_resource_inner_parse(udns_inst_s *restrict inst, const uint8_t *restrict data, uintptr_t size, uintptr_t *restrict pos);
uintptr_t udns_resource_inner_skip(const uint8_t *restrict data, uintptr_t size, uintptr_t *restrict pos, udns_type_t *restrict type);

#endif
