#include "udns_pri.h"
#include <arpa/inet.h>
#include <memory.h>

typedef udns_s* (*udns_parse_add_question_f)(udns_s *restrict udns, udns_question_s *restrict question);
typedef udns_s* (*udns_parse_add_resource_f)(udns_s *restrict udns, udns_resource_s *restrict resource);

typedef struct udns_parse_context_t {
	udns_s *restrict udns;
	udns_inst_s *restrict inst;
	const uint8_t *restrict data;
	uintptr_t size;
	udns_parse_flags_t flags;
} udns_parse_context_t;

static udns_header_t* udns_parse_header(udns_header_t *restrict header, const uint8_t *restrict data, uintptr_t size, uintptr_t *restrict pos)
{
	if (*pos + sizeof(udns_header_t) <= size)
	{
		memcpy(header, data + *pos, sizeof(udns_header_t));
		*pos += sizeof(udns_header_t);
		header->id = ntohs(header->id);
		header->flags = ntohs(header->flags);
		header->n_question = ntohs(header->n_question);
		header->n_answer = ntohs(header->n_answer);
		header->n_authority = ntohs(header->n_authority);
		header->n_additional = ntohs(header->n_additional);
		return header;
	}
	return NULL;
}

static udns_parse_context_t* udns_parse_question(udns_parse_context_t *restrict context, uintptr_t number, uintptr_t *restrict pos, udns_parse_add_question_f func)
{
	udns_question_s *restrict q;
	udns_s *status;
	uintptr_t i, p;
	udns_type_t type;
	if (*pos >= context->size)
		goto label_fail_format;
	for (i = 0; i < number; ++i)
	{
		q = udns_question_inner_parse(context->data, context->size, pos);
		if (q)
		{
			type = q->type;
			status = func(context->udns, q);
			refer_free(q);
			if (!status)
			{
				p = *pos;
				goto label_check;
			}
		}
		else
		{
			p = *pos;
			if (!udns_question_inner_skip(context->data, context->size, &p, &type))
				goto label_fail_format;
			label_check:
			if (!(context->flags & udns_parse_flags_ignore_qrfail))
			{
				if (!(context->flags & udns_parse_flags_ignore_unknow))
					goto label_fail;
				if (udns_inst_inner_get_func(context->inst, type))
					goto label_fail;
			}
			*pos = p;
		}
	}
	label_okay:
	return context;
	label_fail_format:
	if (context->flags & udns_parse_flags_ignore_format)
	{
		*pos = context->size;
		goto label_okay;
	}
	label_fail:
	return NULL;
}

static udns_parse_context_t* udns_parse_resource(udns_parse_context_t *restrict context, uintptr_t number, uintptr_t *restrict pos, udns_parse_add_resource_f func)
{
	udns_resource_s *restrict r;
	udns_s *status;
	uintptr_t i, p;
	udns_type_t type;
	if (*pos >= context->size)
		goto label_fail_format;
	for (i = 0; i < number; ++i)
	{
		r = udns_resource_inner_parse(context->inst, context->data, context->size, pos);
		if (r)
		{
			type = r->type;
			status = func(context->udns, r);
			refer_free(r);
			if (!status)
			{
				p = *pos;
				goto label_check;
			}
		}
		else
		{
			p = *pos;
			if (!udns_resource_inner_skip(context->data, context->size, &p, &type))
				goto label_fail_format;
			label_check:
			if (!(context->flags & udns_parse_flags_ignore_qrfail))
			{
				if (!(context->flags & udns_parse_flags_ignore_unknow))
					goto label_fail;
				if (udns_inst_inner_get_func(context->inst, type))
					goto label_fail;
			}
			*pos = p;
		}
	}
	label_okay:
	return context;
	label_fail_format:
	if (context->flags & udns_parse_flags_ignore_format)
	{
		*pos = context->size;
		goto label_okay;
	}
	label_fail:
	return NULL;
}

udns_s* udns_parse(udns_s *restrict udns, const uint8_t *restrict data, uintptr_t size, uintptr_t *restrict rpos, udns_parse_flags_t flags)
{
	uintptr_t p, n;
	udns_parse_context_t context;
	udns_header_t header;
	p = 0;
	if (!(flags & udns_parse_flags_dont_clear))
		udns_clear(udns);
	if (udns_parse_header(&header, data, size, &p))
	{
		udns->id = (uint32_t) header.id;
		udns->flags = (uint32_t) header.flags;
		context.udns = udns;
		context.inst = udns->inst;
		context.data = data;
		context.size = size;
		context.flags = flags;
		if ((n = (uintptr_t) header.n_question))
		{
			if (!udns_parse_question(&context, n, &p, udns_add_question))
				goto label_fail;
		}
		if ((n = (uintptr_t) header.n_answer))
		{
			if (!udns_parse_resource(&context, n, &p, udns_add_answer))
				goto label_fail;
		}
		if ((n = (uintptr_t) header.n_authority))
		{
			if (!udns_parse_resource(&context, n, &p, udns_add_authority))
				goto label_fail;
		}
		if ((n = (uintptr_t) header.n_additional))
		{
			if (!udns_parse_resource(&context, n, &p, udns_add_additional))
				goto label_fail;
		}
		if (rpos) *rpos = p;
		return udns;
	}
	label_fail:
	if (rpos) *rpos = p;
	return NULL;
}
