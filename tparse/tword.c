#include "tword.h"

tparse_tword_edge_s* tparse_tword_edge_alloc(void)
{
	return (tparse_tword_edge_s *) refer_alloz(sizeof(tparse_tword_edge_s));
}

#define d_t2u(_t)  ((const uint8_t *) _t)

uintptr_t tparse_tword_skip(const tparse_tword_edge_s *restrict edge, const void *restrict text, uintptr_t size, uintptr_t pos)
{
	uintptr_t p;
	tparse_tword_edge_t e;
	p = pos;
	if (p < size && (e = edge->edge[d_t2u(text)[p]]) & tparse_tword_edge_head)
	{
		do {
			++p;
		} while (p < size && ((e = edge->edge[d_t2u(text)[p]]) & tparse_tword_edge_inner));
		if (e & tparse_tword_edge_tail)
			goto label_next;
		while (p > pos && !(edge->edge[d_t2u(text)[p - 1]] & tparse_tword_edge_tail))
			--p;
	}
	return p;
	label_next:
	if (p < size) ++p;
	return p;
}

uintptr_t tparse_tword_rskip(const tparse_tword_edge_s *restrict edge, const void *restrict text, uintptr_t size, uintptr_t pos)
{
	uintptr_t p;
	tparse_tword_edge_t e;
	p = pos;
	if (p && (e = edge->edge[d_t2u(text)[p]]) & tparse_tword_edge_tail)
	{
		do {
			--p;
		} while (p && ((e = edge->edge[d_t2u(text)[p]]) & tparse_tword_edge_inner));
		if (e & tparse_tword_edge_head)
			goto label_next;
		while (p < pos && !(edge->edge[d_t2u(text)[p + 1]] & tparse_tword_edge_head))
			++p;
	}
	return p;
	label_next:
	if (p) --p;
	return p;
}
