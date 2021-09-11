#include "udns_pri.h"
#include <arpa/inet.h>
#include <memory.h>

static uintptr_t udns_build_length_question(const vattr_s *restrict vattr)
{
	const vattr_vlist_t *restrict v;
	udns_question_s *restrict q;
	uintptr_t n;
	n = 0;
	for (v = vattr->vattr; v; v = v->vattr_next)
	{
		q = (udns_question_s *) v->value;
		n += q->labels_length + 4;
	}
	return n;
}

static uintptr_t udns_build_length_resource(const vattr_s *restrict vattr)
{
	const vattr_vlist_t *restrict v;
	udns_resource_s *restrict r;
	uintptr_t n;
	n = 0;
	for (v = vattr->vattr; v; v = v->vattr_next)
	{
		r = (udns_resource_s *) v->value;
		n += r->labels_length + r->length + 10;
	}
	return n;
}

uintptr_t udns_build_length(const udns_s *restrict udns)
{
	uintptr_t n;
	n = 12;
	if (udns->question && udns->n_question)
		n += udns_build_length_question(udns->question);
	if (udns->answer && udns->n_answer)
		n += udns_build_length_resource(udns->answer);
	if (udns->authority && udns->n_authority)
		n += udns_build_length_resource(udns->authority);
	if (udns->additional && udns->n_additional)
		n += udns_build_length_resource(udns->additional);
	return n;
}

static inline uint8_t* udns_build_write_header(const udns_s *restrict udns, uint8_t *restrict data)
{
	udns_header_t header;
	header.id = htons((uint16_t) udns->id);
	header.flags = htons((uint16_t) udns->flags);
	header.n_question = htons((uint16_t) udns->n_question);
	header.n_answer = htons((uint16_t) udns->n_answer);
	header.n_authority = htons((uint16_t) udns->n_authority);
	header.n_additional = htons((uint16_t) udns->n_additional);
	memcpy(data, &header, sizeof(header));
	return data + sizeof(header);
}

static uint8_t* udns_build_write_question(const vattr_s *restrict vattr, uint8_t *restrict data)
{
	const vattr_vlist_t *restrict v;
	udns_question_s *restrict q;
	uint16_t u16;
	for (v = vattr->vattr; v; v = v->vattr_next)
	{
		q = (udns_question_s *) v->value;
		// name   -- bits
		memcpy(data, q->labels_data, q->labels_length);
		data += q->labels_length;
		// type   16 bits
		u16 = htons((uint16_t) q->type);
		memcpy(data, &u16, sizeof(u16));
		data += sizeof(u16);
		// class  16 bits
		u16 = htons((uint16_t) q->class);
		memcpy(data, &u16, sizeof(u16));
		data += sizeof(u16);
	}
	return data;
}

static uint8_t* udns_build_write_resource(const vattr_s *restrict vattr, uint8_t *restrict data)
{
	const vattr_vlist_t *restrict v;
	udns_resource_s *restrict r;
	uint32_t u32;
	uint16_t u16;
	for (v = vattr->vattr; v; v = v->vattr_next)
	{
		r = (udns_resource_s *) v->value;
		// name         -- bits
		memcpy(data, r->labels_data, r->labels_length);
		data += r->labels_length;
		// type         16 bits
		u16 = htons((uint16_t) r->type);
		memcpy(data, &u16, sizeof(u16));
		data += sizeof(u16);
		// class        16 bits
		u16 = htons((uint16_t) r->class);
		memcpy(data, &u16, sizeof(u16));
		data += sizeof(u16);
		// ttl          32 bits
		u32 = htonl((uint32_t) r->ttl);
		memcpy(data, &u32, sizeof(u32));
		data += sizeof(u32);
		// data length  16 bits
		u16 = htons((uint16_t) r->length);
		memcpy(data, &u16, sizeof(u16));
		data += sizeof(u16);
		// data         -- bits
		memcpy(data, r->data, r->length);
		data += r->length;
	}
	return data;
}

uint8_t* udns_build_write(const udns_s *restrict udns, uint8_t *restrict data)
{
	data = udns_build_write_header(udns, data);
	if (udns->question && udns->n_question)
		data = udns_build_write_question(udns->question, data);
	if (udns->answer && udns->n_answer)
		data = udns_build_write_resource(udns->answer, data);
	if (udns->authority && udns->n_authority)
		data = udns_build_write_resource(udns->authority, data);
	if (udns->additional && udns->n_additional)
		data = udns_build_write_resource(udns->additional, data);
	return data;
}

uint8_t* udns_build(const udns_s *restrict udns, exbuffer_t *restrict data, uintptr_t *restrict length)
{
	uint8_t *restrict p;
	uintptr_t n;
	n = udns_build_length(udns);
	if ((p = (uint8_t *) exbuffer_need(data, n)))
		udns_build_write(udns, p);
	else n = 0;
	if (length)
		*length = n;
	return p;
}
