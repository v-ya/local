#include "miko.form.h"
#include <memory.h>

miko_form_table_s* miko_form_table_alloc(miko_count_t count)
{
	miko_form_table_s *restrict r;
	if ((r = (miko_form_table_s *) refer_alloz(sizeof(miko_form_table_s) + count * sizeof(miko_form_t *))))
	{
		r->table_array = (miko_form_table_t) (r + 1);
		r->table_count = count;
		return r;
	}
	return NULL;
}

void miko_form_table_clear(miko_form_table_s *restrict r)
{
	memset(r->table_array, 0, r->table_count * sizeof(miko_form_t *));
}

miko_form_table_s* miko_form_table_copy(miko_form_table_s *restrict dst, const miko_form_table_s *restrict src)
{
	uintptr_t n;
	if ((n = dst->table_count) == src->table_count)
	{
		memcpy(dst->table_array, src->table_array, n * sizeof(miko_form_t *));
		return dst;
	}
	return NULL;
}
