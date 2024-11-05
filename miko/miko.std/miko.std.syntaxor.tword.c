#include "miko.std.syntaxor.h"

typedef struct miko_std_syntaxor_item_tword_s miko_std_syntaxor_item_tword_s;

struct miko_std_syntaxor_item_tword_s {
	miko_std_syntaxor_item_s item;
	tparse_tword_edge_s *edge;
};

static uintptr_t miko_std_syntaxor_item_tword_test_func(miko_std_syntaxor_item_tword_s *restrict r, const char *restrict p, uintptr_t n, uintptr_t pos)
{
	return tparse_tword_skip(r->edge, p, n, pos) - pos;
}

static void miko_std_syntaxor_item_tword_init_edge(tparse_tword_edge_s *restrict edge, miko_std_syntaxor_chars_t cs, tparse_tword_edge_t flag)
{
	uintptr_t i;
	for (i = 0; cs[i]; ++i)
		edge->edge[((const uint8_t *) cs)[i]] |= flag;
}

static void miko_std_syntaxor_item_tword_free_func(miko_std_syntaxor_item_tword_s *restrict r)
{
	refer_ck_free(r->edge);
	refer_ck_free(r->item.id.name);
}

miko_std_syntaxor_item_s* miko_std_syntaxor_item_create_tword(miko_std_syntaxor_chars_t head, miko_std_syntaxor_chars_t inner, miko_std_syntaxor_chars_t tail)
{
	miko_std_syntaxor_item_tword_s *restrict r;
	if (head && inner && tail && (r = (miko_std_syntaxor_item_tword_s *) refer_alloz(sizeof(miko_std_syntaxor_item_tword_s))))
	{
		refer_hook_free(r, syntaxor_item_tword);
		r->item.test = (miko_std_syntaxor_test_f) miko_std_syntaxor_item_tword_test_func;
		if ((r->edge = tparse_tword_edge_alloc()))
		{
			miko_std_syntaxor_item_tword_init_edge(r->edge, head, tparse_tword_edge_head);
			miko_std_syntaxor_item_tword_init_edge(r->edge, inner, tparse_tword_edge_inner);
			miko_std_syntaxor_item_tword_init_edge(r->edge, tail, tparse_tword_edge_tail);
			return &r->item;
		}
		refer_free(r);
	}
	return NULL;
}
