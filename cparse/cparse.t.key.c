#include "cparse.type.h"

tparse_tword_edge_s* cparse_inner_alloc_tword_key(void)
{
	tparse_tword_edge_s *restrict r;
	r = tparse_tword_edge_alloc();
	if (r)
	{
		uintptr_t i;
		r->edge['_'] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
		r->edge['$'] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
		for (i = 'A'; i <= 'Z'; ++i)
			r->edge[i] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
		for (i = 'a'; i <= 'z'; ++i)
			r->edge[i] = tparse_tword_edge_head | tparse_tword_edge_inner | tparse_tword_edge_tail;
		for (i = '0'; i <= '9'; ++i)
			r->edge[i] = tparse_tword_edge_inner | tparse_tword_edge_tail;
	}
	return r;
}
