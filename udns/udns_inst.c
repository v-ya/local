#include "udns_pri.h"
#include "type/func.h"
#include <memory.h>

static inline udns_inst_s* udns_inst_set_func(
	udns_inst_s *restrict r, udns_type_t type,
	const char *restrict name, uintptr_t length,
	udns_type_arg_do_f initial, udns_type_arg_do_f finaly,
	udns_type_parse_length_f parse_length, udns_type_build_length_f build_length,
	udns_type_parse_write_f parse_write, udns_type_build_write_f build_write)
{
	if ((uint32_t) type < udns_type_max && length && length < udns_type_name_max)
	{
		udns_type_func_t *restrict func;
		func = r->func + type;
		memcpy(func->type_name, name, length);
		func->initial = initial;
		func->finaly = finaly;
		func->parse_length = parse_length;
		func->build_length = build_length;
		func->parse_write = parse_write;
		func->build_write = build_write;
		return r;
	}
	return NULL;
}

udns_inst_s* udns_inst_alloc(void)
{
	udns_inst_s *restrict r;
	r = (udns_inst_s *) refer_alloz(sizeof(udns_inst_s));
	if (r)
	{
		if (
			#define d_type(_name) udns_inst_set_func(\
				r, udns_type_##_name,\
				#_name, sizeof(#_name) - 1,\
				udns_func_type_##_name##_initial,\
				udns_func_type_##_name##_finaly,\
				udns_func_type_##_name##_pl,\
				udns_func_type_##_name##_bl,\
				udns_func_type_##_name##_pw,\
				udns_func_type_##_name##_bw\
				)
			d_type(a) &&
			#undef d_type
		1) return r;
		refer_free(r);
	}
	return NULL;
}
