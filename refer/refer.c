#include "refer.h"
#include <stdlib.h>

typedef struct refer_s {
	uint64_t inode;
	refer_free_f free_func;
} refer_s;

refer_t refer_alloc(size_t size)
{
	refer_s *r;
	r = (refer_s *) malloc(size + sizeof(refer_s));
	if (r)
	{
		r->inode = 1;
		r->free_func = NULL;
	}
	return (refer_t) (r + 1);
}

refer_t refer_alloz(size_t size)
{
	refer_s *r;
	r = (refer_s *) calloc(1, size + sizeof(refer_s));
	if (r) r->inode = 1;
	return (refer_t) (r + 1);
}

refer_t refer_free(refer_t v)
{
	if (!__sync_sub_and_fetch(&((refer_s *) v - 1)->inode, 1))
	{
		refer_s *s;
		s = (refer_s *) v - 1;
		if (s->free_func) s->free_func(v);
		free(s);
		v = NULL;
	}
	return v;
}

void refer_set_free(refer_t v, refer_free_f free_func)
{
	((refer_s *) v - 1)->free_func = free_func;
}

refer_t refer_save(refer_t v)
{
	__sync_add_and_fetch(&((refer_s *) v - 1)->inode, 1);
	return v;
}

uint64_t refer_save_number(refer_t v)
{
	return ((refer_s *) v - 1)->inode;
}
