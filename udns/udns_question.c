#include "udns_pri.h"
#include "type/inner.h"
#include <string.h>
#include <arpa/inet.h>

udns_question_s* udns_question_alloc(const char *restrict name, udns_type_t type, udns_class_t class)
{
	struct udns_question_s *restrict r;
	uintptr_t n, p_name, n_name, p_labels, n_labels;
	udns_inner_labels_t labels;
	n = udns_inner_align(sizeof(udns_question_s));
	p_name = n;
	n_name = strlen(name) + 1;
	n += n_name;
	n = udns_inner_align(n);
	p_labels = n;
	if (udns_inner_labels4string(&labels, name, &n_labels))
	{
		n += n_labels;
		n = udns_inner_align(n);
		r = (struct udns_question_s *) refer_alloc(n);
		if (r)
		{
			r->name_string = memcpy((uint8_t *) r + p_name, name, n_name);
			r->labels_data = memcpy((uint8_t *) r + p_labels, labels.data, n_labels);
			r->labels_length = n_labels;
			r->type = type;
			r->class = class;
			return r;
		}
	}
	return NULL;
}

udns_question_s* udns_question_parse(const uint8_t *restrict data, uintptr_t size, uintptr_t *restrict pos)
{
	struct udns_question_s *restrict r;
	uintptr_t p, n, p_name, p_labels, n_name, n_labels;
	udns_inner_labels_t labels_string;
	udns_inner_labels_t labels_data;
	udns_type_t type;
	udns_class_t class;
	p = *pos;
	if (udns_inner_labels2string(&labels_string, data, size, &p, &n_name) &&
		udns_inner_labels4string(&labels_data, (char *) labels_string.data, &n_labels) &&
		p + 4 <= size)
	{
		type = (udns_type_t) ntohs(*(const uint16_t *) (data + p));
		class = (udns_class_t) ntohs(*(const uint16_t *) (data + p + 2));
		p += 4;
		n = udns_inner_align(sizeof(udns_question_s));
		p_name = n;
		n += n_name;
		n = udns_inner_align(n);
		p_labels = n;
		n += n_labels;
		n = udns_inner_align(n);
		r = (struct udns_question_s *) refer_alloc(n);
		if (r)
		{
			r->name_string = memcpy((uint8_t *) r + p_name, labels_string.data, n_name);
			r->labels_data = memcpy((uint8_t *) r + p_labels, labels_data.data, n_labels);
			r->labels_length = n_labels;
			r->type = type;
			r->class = class;
			*pos = p;
			return r;
		}
	}
	return NULL;
}
