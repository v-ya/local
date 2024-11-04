#include "miko.std.syntaxor.h"
#include <memory.h>

typedef struct miko_std_syntaxor_item_keyword_s miko_std_syntaxor_item_keyword_s;

struct miko_std_syntaxor_item_keyword_s {
	miko_std_syntaxor_item_s item;
	miko_count_t count;
	refer_nstring_t keyword[];
};

static uintptr_t miko_std_syntaxor_item_keyword_test_func(miko_std_syntaxor_item_keyword_s *restrict r, const char *restrict p, uintptr_t n, uintptr_t pos)
{
	refer_nstring_t v;
	uintptr_t i, count, z;
	p += pos;
	n -= pos;
	count = r->count;
	for (i = 0; i < count; ++i)
	{
		v = r->keyword[i];
		if ((z = v->length) && z <= n && !memcmp(p, v->string, z))
			return z;
	}
	return 0;
}

static void miko_std_syntaxor_item_keyword_free_func(miko_std_syntaxor_item_keyword_s *restrict r)
{
	uintptr_t i, n;
	for (i = 0, n = r->count; i < n; ++i)
		{ refer_ck_free(r->keyword[i]); }
	refer_ck_free(r->item.id.name);
}

miko_std_syntaxor_item_s* miko_std_syntaxor_item_create_keyword(const char *const restrict keyword[])
{
	miko_std_syntaxor_item_keyword_s *restrict r;
	uintptr_t i, n;
	for (n = 0; keyword[n]; ++n) ;
	if ((r = (miko_std_syntaxor_item_keyword_s *) refer_alloz(
		sizeof(miko_std_syntaxor_item_keyword_s) + n * sizeof(refer_nstring_t))))
	{
		refer_hook_free(r, syntaxor_item_keyword);
		r->item.test = (miko_std_syntaxor_test_f) miko_std_syntaxor_item_keyword_test_func;
		for (i = 0; i < n; ++i)
		{
			if (!(r->keyword[i] = refer_dump_nstring(keyword[i])))
				goto label_fail;
			r->count += 1;
		}
		return &r->item;
		label_fail:
		refer_free(r);
	}
	return NULL;
}
