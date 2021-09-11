#include "udns_pri.h"
#include "type/inner.h"
#include <string.h>
#include <arpa/inet.h>

// udns_question_s* udns_question_alloc(const char *restrict name, udns_type_t type, udns_class_t class)
// {
// 	struct udns_question_s *restrict r;
// 	uintptr_t n, p_name, n_name, p_labels, n_labels;
// 	udns_inner_labels_t labels;
// 	n = udns_inner_align(sizeof(udns_question_s));
// 	p_name = n;
// 	n_name = strlen(name) + 1;
// 	n += n_name;
// 	n = udns_inner_align(n);
// 	p_labels = n;
// 	if (udns_inner_labels4string(&labels, name, &n_labels))
// 	{
// 		n += n_labels;
// 		n = udns_inner_align(n);
// 		r = (struct udns_question_s *) refer_alloc(n);
// 		if (r)
// 		{
// 			r->name_string = memcpy((uint8_t *) r + p_name, name, n_name);
// 			r->labels_data = memcpy((uint8_t *) r + p_labels, labels.data, n_labels);
// 			r->labels_length = n_labels;
// 			r->type = type;
// 			r->class = class;
// 			return r;
// 		}
// 	}
// 	return NULL;
// }

udns_resource_s* udns_resource_inner_parse(udns_inst_s *restrict inst, const uint8_t *restrict data, uintptr_t size, uintptr_t *restrict pos)
{
	struct udns_resource_s *restrict r;
	const udns_type_func_t *restrict func;
	uintptr_t p, n, l, p_name, p_labels, p_parse, p_data, n_name, n_labels, n_data, n_parse;
	struct udns_type_arg_stack_t arg;
	udns_inner_labels_t labels_string;
	udns_inner_labels_t labels_data;
	udns_type_t t;
	uint32_t ttl;
	uint16_t type;
	uint16_t class;
	uint16_t length;
	p = *pos;
	if (udns_inner_labels2string(&labels_string, data, size, &p, &n_name) &&
		udns_inner_labels4string(&labels_data, (char *) labels_string.data, &n_labels) &&
		p + 10 <= size)
	{
		++n_name;
		memcpy(&type, data + p, sizeof(type));
		memcpy(&class, data + p + 2, sizeof(class));
		memcpy(&ttl, data + p + 4, sizeof(ttl));
		memcpy(&length, data + p + 8, sizeof(length));
		p += 10;
		t = (udns_type_t) ntohs(type);
		l = (uintptr_t) ntohs(length);
		if (l <= size - p)
		{
			if ((func = udns_inst_inner_get_func(inst, t)))
			{
				if (func->initial)
					func->initial(&arg);
				n_parse = func->parse_length(&arg, data, p + l, p);
				n_data = func->build_length(&arg, NULL);
				if (~n_parse && ~n_data)
				{
					n = udns_inner_align(sizeof(udns_resource_s));
					p_name = n;
					n += n_name;
					n = udns_inner_align(n);
					p_labels = n;
					n += n_labels;
					n = udns_inner_align(n);
					p_parse = n;
					n += n_parse;
					n = udns_inner_align(n);
					p_data = n;
					n += n_data;
					n = udns_inner_align(n);
					r = (struct udns_resource_s *) refer_alloc(n);
					if (r)
					{
						r->name_string = memcpy((uint8_t *) r + p_name, labels_string.data, n_name);
						r->labels_data = memcpy((uint8_t *) r + p_labels, labels_data.data, n_labels);
						r->labels_length = n_labels;
						if (func->parse_write(&arg, (char *) r + p_parse) &&
							func->build_write(&arg, (uint8_t *) r + p_data))
						{
							r->data_parse = (char *) r + p_parse;
							r->data = (uint8_t *) r + p_data;
							r->length = n_data;
							r->type = t;
							r->class = (udns_class_t) ntohs(class);
							r->ttl = ntohl(ttl);
							p += l;
							if (func->finaly)
								func->finaly(&arg);
							*pos = p;
							return r;
						}
						refer_free(r);
					}
				}
				if (func->finaly)
					func->finaly(&arg);
			}
		}
	}
	return NULL;
}

uintptr_t udns_resource_inner_skip(const uint8_t *restrict data, uintptr_t size, uintptr_t *restrict pos, udns_type_t *restrict type)
{
	uintptr_t p, n;
	uint16_t u16;
	p = *pos;
	// name && type && class && ttl
	if (udns_inner_labels_skip(data, size, &p) && (p += 8) + 2 <= size)
	{
		if (type)
		{
			memcpy(&u16, data + p - 8, sizeof(u16));
			*type = (udns_type_t) ntohs(u16);
		}
		// data length
		memcpy(&u16, data + p, sizeof(u16));
		n = (uintptr_t) ntohs(u16);
		p += 2;
		// data
		if (n <= size - p)
		{
			p += n;
			n = p - *pos;
			*pos = p;
			return n;
		}
	}
	return 0;
}
