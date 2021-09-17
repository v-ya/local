#include "udns_pri.h"
#include "type/func.h"
#include <memory.h>

static inline struct udns_inst_s* udns_inst_set_func(
	struct udns_inst_s *restrict r, udns_type_t type,
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

static udns_inst_s* udns_inst_alloc_by_types(const udns_type_t *restrict types)
{
	struct udns_inst_s *restrict r;
	r = (struct udns_inst_s *) refer_alloz(sizeof(struct udns_inst_s));
	if (r)
	{
		udns_type_t t;
		while ((t = *types))
		{
			switch (t)
			{
				#define d_type(_name)  \
					case udns_type_##_name:\
						if (!udns_inst_set_func(\
							r, udns_type_##_name,\
							#_name, sizeof(#_name) - 1,\
							udns_func_type_##_name##_initial,\
							udns_func_type_##_name##_finaly,\
							udns_func_type_##_name##_pl,\
							udns_func_type_##_name##_bl,\
							udns_func_type_##_name##_pw,\
							udns_func_type_##_name##_bw\
						)) goto label_fail;\
						break
				d_type(a);
				d_type(ns);
				d_type(cname);
				d_type(soa);
				d_type(aaaa);
				#undef d_type
				default: goto label_fail;
			}
			++types;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

udns_inst_s* udns_inst_alloc_mini(void)
{
	static const udns_type_t types[] = {
		udns_type_a,
		udns_type_cname,
		udns_type_aaaa,
		udns_type_reserved
	};
	return udns_inst_alloc_by_types(types);
}

udns_inst_s* udns_inst_alloc(void)
{
	static const udns_type_t types[] = {
		udns_type_a,
		udns_type_ns,
		udns_type_cname,
		udns_type_soa,
		udns_type_aaaa,
		udns_type_reserved
	};
	return udns_inst_alloc_by_types(types);
}

const udns_type_func_t* udns_inst_inner_get_func(udns_inst_s *restrict inst, udns_type_t type)
{
	const udns_type_func_t *restrict func;
	if ((uint32_t) type < udns_type_max && (func = inst->func + type)->type_name[0])
		return func;
	return NULL;
}

#include "type/inner.h"

int udns_check_ipv4(const char *restrict ipv4, uint8_t array[4])
{
	uint8_t a[4];
	if (!array) array = a;
	return udns_inner_get_ipv4(ipv4, array)?0:-1;
}

int udns_check_ipv6(const char *restrict ipv6, uint8_t array[16])
{
	uint8_t a[16];
	if (!array) array = a;
	return udns_inner_get_ipv6(ipv6, array)?0:-1;
}
