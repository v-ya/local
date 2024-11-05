#include "miko.std.syntaxor.h"

typedef struct miko_std_syntaxor_item_tstring_s miko_std_syntaxor_item_tstring_s;

struct miko_std_syntaxor_item_tstring_s {
	miko_std_syntaxor_item_s item;
	tparse_tstring_s *tstring;
};

static uintptr_t miko_std_syntaxor_item_tstring_test_func(miko_std_syntaxor_item_tstring_s *restrict r, const char *restrict p, uintptr_t n, uintptr_t pos)
{
	uintptr_t npos;
	tparse_tstring_clear(r->tstring);
	if (tparse_tstring_transport(r->tstring, p, n, pos, &npos))
		return npos - pos;
	return 0;
}

static const char* miko_std_syntaxor_item_tstring_data_func(miko_std_syntaxor_item_tstring_s *restrict r, uintptr_t *restrict length)
{
	return tparse_tstring_get_data(r->tstring, length);
}

static void miko_std_syntaxor_item_tstring_free_func(miko_std_syntaxor_item_tstring_s *restrict r)
{
	refer_ck_free(r->tstring);
	refer_ck_free(r->item.id.name);
}

miko_std_syntaxor_item_s* miko_std_syntaxor_item_create_tstring(tparse_tstring_s *restrict tstring)
{
	miko_std_syntaxor_item_tstring_s *restrict r;
	if (tstring && (r = (miko_std_syntaxor_item_tstring_s *) refer_alloz(sizeof(miko_std_syntaxor_item_tstring_s))))
	{
		refer_hook_free(r, syntaxor_item_tstring);
		r->item.test = (miko_std_syntaxor_test_f) miko_std_syntaxor_item_tstring_test_func;
		r->item.data = (miko_std_syntaxor_data_f) miko_std_syntaxor_item_tstring_data_func;
		r->tstring = (tparse_tstring_s *) refer_save(tstring);
		return &r->item;
	}
	return NULL;
}
