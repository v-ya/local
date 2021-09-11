#include "udns_pri.h"

static void udns_free_func(udns_s *restrict r)
{
	if (r->inst)
		refer_free(r->inst);
	if (r->question)
		refer_free(r->question);
	if (r->answer)
		refer_free(r->answer);
	if (r->authority)
		refer_free(r->authority);
	if (r->additional)
		refer_free(r->additional);
}

udns_s* udns_alloc(udns_inst_s *restrict inst)
{
	udns_s *restrict r;
	if ((r = (udns_s *) refer_alloz(sizeof(udns_s))))
	{
		refer_set_free(r, (refer_free_f) udns_free_func);
		r->inst = inst?((udns_inst_s *) refer_save(inst)):udns_inst_alloc();
		if (r->inst)
			return r;
		refer_free(r);
	}
	return NULL;
}

void udns_clear(udns_s *restrict udns)
{
	if (udns->question)
		vattr_clear(udns->question);
	if (udns->answer)
		vattr_clear(udns->answer);
	if (udns->authority)
		vattr_clear(udns->authority);
	if (udns->additional)
		vattr_clear(udns->additional);
	udns->n_question = 0;
	udns->n_answer = 0;
	udns->n_authority = 0;
	udns->n_additional = 0;
	udns->id = 0;
	udns->flags = 0;
}

void udns_set_qr(udns_s *restrict udns, uintptr_t qr)
{
	if (qr) udns->flags |= (uint32_t) udns_header_flags_qr;
	else udns->flags &= ~(uint32_t) udns_header_flags_qr;
}

void udns_set_opcode(udns_s *restrict udns, udns_opcode_t opcode)
{
	udns->flags = (udns->flags & (uint32_t) udns_header_flags_opcode_mask) | (((uint32_t) opcode << 11) & (uint32_t) udns_header_flags_opcode_mask);
}

void udns_set_aa(udns_s *restrict udns, uintptr_t aa)
{
	if (aa) udns->flags |= (uint32_t) udns_header_flags_aa;
	else udns->flags &= ~(uint32_t) udns_header_flags_aa;
}

void udns_set_tc(udns_s *restrict udns, uintptr_t tc)
{
	if (tc) udns->flags |= (uint32_t) udns_header_flags_tc;
	else udns->flags &= ~(uint32_t) udns_header_flags_tc;
}

void udns_set_rd(udns_s *restrict udns, uintptr_t rd)
{
	if (rd) udns->flags |= (uint32_t) udns_header_flags_rd;
	else udns->flags &= ~(uint32_t) udns_header_flags_rd;
}

void udns_set_ra(udns_s *restrict udns, uintptr_t ra)
{
	if (ra) udns->flags |= (uint32_t) udns_header_flags_ra;
	else udns->flags &= ~(uint32_t) udns_header_flags_ra;
}

void udns_set_rcode(udns_s *restrict udns, udns_rcode_t rcode)
{
	udns->flags = (udns->flags & (uint32_t) udns_header_flags_rcode_mask) | ((uint32_t) rcode & (uint32_t) udns_header_flags_rcode_mask);
}

uintptr_t udns_get_qr(const udns_s *restrict udns)
{
	return !!(udns->flags & udns_header_flags_qr);
}

udns_opcode_t udns_get_opcode(const udns_s *restrict udns)
{
	return (udns_opcode_t) ((udns->flags & (uint32_t) udns_header_flags_opcode_mask) >> 11);
}

uintptr_t udns_get_aa(const udns_s *restrict udns)
{
	return !!(udns->flags & udns_header_flags_aa);
}

uintptr_t udns_get_tc(const udns_s *restrict udns)
{
	return !!(udns->flags & udns_header_flags_tc);
}

uintptr_t udns_get_rd(const udns_s *restrict udns)
{
	return !!(udns->flags & udns_header_flags_rd);
}

uintptr_t udns_get_ra(const udns_s *restrict udns)
{
	return !!(udns->flags & udns_header_flags_ra);
}

udns_rcode_t udns_get_rcode(const udns_s *restrict udns)
{
	return (udns_rcode_t) (udns->flags & (uint32_t) udns_header_flags_rcode_mask);
}

static udns_s* udns_add_qr(udns_s *restrict udns, vattr_s **restrict vattr, udns_type_t type, refer_t qr, uintptr_t *restrict n)
{
	const udns_type_func_t *restrict func;
	if (*n < 0x10000 && (func = udns_inst_inner_get_func(udns->inst, type)))
	{
		if (*vattr)
		{
			label_add:
			if (vattr_insert_tail(*vattr, func->type_name, qr))
			{
				*n += 1;
				return udns;
			}
		}
		else if ((*vattr = vattr_alloc()))
			goto label_add;
	}
	return NULL;
}

#define udns_add_qr_proxy(_u, _t, _qr)  udns_add_qr((_u), &(_u)->_t, (_qr)->type, _qr, &(_u)->n_##_t)

udns_s* udns_add_question(udns_s *restrict udns, udns_question_s *restrict question)
{
	return udns_add_qr_proxy(udns, question, question);
}

udns_s* udns_add_answer(udns_s *restrict udns, udns_resource_s *restrict answer)
{
	return udns_add_qr_proxy(udns, answer, answer);
}

udns_s* udns_add_authority(udns_s *restrict udns, udns_resource_s *restrict authority)
{
	return udns_add_qr_proxy(udns, authority, authority);
}

udns_s* udns_add_additional(udns_s *restrict udns, udns_resource_s *restrict additional)
{
	return udns_add_qr_proxy(udns, additional, additional);
}

udns_s* udns_add_question_info(udns_s *restrict udns, const char *restrict name, udns_type_t type, udns_class_t class)
{
	udns_s *restrict r;
	udns_question_s *restrict question;
	r = NULL;
	if ((question = udns_question_alloc(name, type, class)))
	{
		if (udns_add_qr_proxy(udns, question, question))
			r = udns;
		refer_free(question);
	}
	return r;
}
