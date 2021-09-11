#include "func.h"
#include "inner.h"
#include <json.h>
#include <arpa/inet.h>
#include <memory.h>

typedef struct udns_type_arg_stack_t {
	json_inode_t *soa;
	uintptr_t n_mname;
	uintptr_t n_rname;
	uint32_t net_serial;
	uint32_t net_refresh;
	uint32_t net_retry;
	uint32_t net_expire;
	uint32_t net_minimum;
	udns_inner_labels_t mname;
	udns_inner_labels_t rname;
} udns_type_arg_stack_t;

static const char *jkey_mname = "mname";
static const char *jkey_rname = "rname";
static const char *jkey_serial = "serial";
static const char *jkey_refresh = "refresh";
static const char *jkey_retry = "retry";
static const char *jkey_expire = "expire";
static const char *jkey_minimum = "minimum";

static udns_type_arg_stack_t* inner_soa_get_by_parse(udns_type_arg_stack_t *restrict r, const char *restrict json)
{
	if (json_decode(json, &r->soa))
	{
		const char *s;
		int64_t u;
		#define d(_name)  \
			if (!json_get_string(r->soa, jkey_##_name, &s))\
				s = "";\
			if (!udns_inner_labels4string(&r->_name, s, &r->n_##_name))\
				goto label_fail
		d(mname);
		d(rname);
		#undef d
		#define d(_name)  \
			u = 0;\
			json_get_integer(r->soa, jkey_##_name, &u);\
			r->net_##_name = (uint32_t) u
		d(serial);
		d(refresh);
		d(retry);
		d(expire);
		d(minimum);
		#undef d
		return r;
	}
	label_fail:
	return NULL;
}

static udns_type_arg_stack_t* inner_soa_get_by_data(udns_type_arg_stack_t *restrict r, const uint8_t *restrict data, uintptr_t size, uintptr_t pos)
{
	json_inode_t *v;
	uintptr_t n;
	udns_inner_labels_t labels;
	v = NULL;
	if (r->soa)
		json_free(r->soa);
	r->soa = json_create_object();
	if (r->soa)
	{
		if (udns_inner_labels2string(&labels, data, size, &pos, &n) &&
			json_object_set(r->soa, jkey_mname, v = json_create_string((char *) labels.data)) &&
			((v = NULL), udns_inner_labels4string(&r->mname, (char *) labels.data, &r->n_mname)) &&
			udns_inner_labels2string(&labels, data, size, &pos, &n) &&
			json_object_set(r->soa, jkey_rname, v = json_create_string((char *) labels.data)) &&
			((v = NULL), udns_inner_labels4string(&r->rname, (char *) labels.data, &r->n_rname)) &&
			size - pos >= 20)
		{
			#define d(_name, _p)  \
			memcpy(&r->net_##_name, data + pos + _p, 4);\
			if (!json_object_set(r->soa, jkey_##_name, v = json_create_integer((int64_t) ntohl(r->net_##_name))))\
				goto label_fail
			d(serial, 0);
			d(refresh, 4);
			d(retry, 8);
			d(expire, 12);
			d(minimum, 16);
			#undef d
			return r;
		}
	}
	label_fail:
	if (v) json_free(v);
	return NULL;
}

inner_type_initial(soa)
{
	arg->soa = NULL;
	arg->n_mname = 0;
	arg->n_rname = 0;
	arg->net_serial = 0;
	arg->net_refresh = 0;
	arg->net_retry = 0;
	arg->net_expire = 0;
	arg->net_minimum = 0;
}

inner_type_finaly(soa)
{
	if (arg->soa)
		json_free(arg->soa);
}

inner_type_parse_length(soa)
{
	uintptr_t n;
	if (!data)
	{
		if (arg->soa)
			goto label_okay;
	}
	else if (inner_soa_get_by_data(arg, data, size, pos))
	{
		label_okay:
		if ((n = (uintptr_t) json_length(arg->soa)))
			return n + 1;
	}
	return ~(uintptr_t) 0;
}

inner_type_build_length(soa)
{
	if (!parse)
	{
		if (arg->soa)
			goto label_okay;
	}
	else if (inner_soa_get_by_parse(arg, parse))
	{
		label_okay:
		return arg->n_mname + arg->n_rname + 20;
	}
	return ~(uintptr_t) 0;
}

inner_type_parse_write(soa)
{
	if (arg->soa)
	{
		if (json_encode(arg->soa, parse))
			return arg;
	}
	return NULL;
}

inner_type_build_write(soa)
{
	if (arg->soa)
	{
		memcpy(data, arg->mname.data, arg->n_mname);
		data += arg->n_mname;
		memcpy(data, arg->rname.data, arg->n_rname);
		data += arg->n_rname;
		memcpy(data, &arg->net_serial, 4);
		data += 4;
		memcpy(data, &arg->net_refresh, 4);
		data += 4;
		memcpy(data, &arg->net_retry, 4);
		data += 4;
		memcpy(data, &arg->net_expire, 4);
		data += 4;
		memcpy(data, &arg->net_minimum, 4);
		return arg;
	}
	return NULL;
}
