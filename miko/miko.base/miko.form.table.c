#include "miko.form.h"

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
