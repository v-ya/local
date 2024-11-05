#include "../header/miko.h"
#include "../header/miko.type.h"
#include "../header/miko.api.h"
#include "../header/miko.wink.h"
#include "../header/miko.std.h"
#include "../header/miko.std.type.h"
#include "../header/miko.std.api.h"
#include "../header/miko.std.marco.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <yaw.h>

void test_syntax_print(mlog_s *restrict mlog, miko_std_syntax_s *restrict syntax, uint32_t indent);

void test_syntax_print_scope(mlog_s *restrict mlog, miko_std_syntax_s *restrict syntax, uint32_t indent, uint32_t isblock)
{
	miko_std_syntax_s *restrict value;
	vattr_vlist_t *restrict vlist;
	refer_nstring_t nstr;
	uintptr_t pos;
	nstr = syntax->data.syntax;
	if (nstr && nstr->length >= 2)
	{
		if (!isblock) mlog_printf(mlog, "%c", nstr->string[0]);
		else mlog_printf(mlog, "\n%*s%c\n", indent, "", nstr->string[0]);
		indent += 4;
	}
	pos = 0;
	for (vlist = syntax->data.scope->vattr; vlist; vlist = vlist->vattr_next)
	{
		value = (miko_std_syntax_s *) vlist->value;
		if (isblock && !pos)
			mlog_printf(mlog, "%*s", indent, "");
		if (pos) mlog_printf(mlog, " ");
		test_syntax_print(mlog, value, indent);
		pos += 1;
		if (isblock && !strcmp(value->id.name, miko_std_syntax__semicolon))
		{
			mlog_printf(mlog, "\n");
			pos = 0;
		}
		if (!value->id.name || !strcmp(value->id.name, miko_std_syntax__scope))
			pos = 0;
	}
	if (nstr && nstr->length >= 2)
	{
		indent -= 4;
		if (!isblock) mlog_printf(mlog, "%c", nstr->string[1]);
		else mlog_printf(mlog, "%s%*s%c\n", pos?"\n":"", indent, "", nstr->string[1]);
	}
}

void test_syntax_print(mlog_s *restrict mlog, miko_std_syntax_s *restrict syntax, uint32_t indent)
{
	if (syntax->data.scope)
	{
		if (!syntax->id.name || !strcmp(syntax->id.name, miko_std_syntax__scope))
			test_syntax_print_scope(mlog, syntax, indent, 1);
		else test_syntax_print_scope(mlog, syntax, indent, 0);
	}
	else if (syntax->data.syntax)
		mlog_printf(mlog, "%s", syntax->data.syntax->string);
}

#define _str_(...)  #__VA_ARGS__

static const char *const code = _str_(
	for (i = 0, b = 0; i < length; ++i)
	{
		if (a) a[i] = 0;
		if (b)
		{
			c = (uint32_t) ("0123456789"[i] - '0');
			b += a;
			b *= b;
		}
	}
);

#undef _str_

void test_std(mlog_s *restrict mlog)
{
	refer_nstring_t nstr;
	miko_source_s *restrict source;
	miko_std_syntaxor_s *restrict syntaxor;
	miko_std_syntax_s *restrict syntax;
	if ((nstr = refer_dump_nstring(code)))
	{
		if ((source = miko_source_alloc("test", nstr)))
		{
			if ((syntaxor = miko_std_syntaxor_create_spec(mlog)))
			{
				mlog_printf(mlog, "syntaxor %p okay\n", syntaxor);
				if ((syntax = miko_std_syntaxor_create(syntaxor, source)))
				{
					mlog_printf(mlog, "syntax %p\n", syntax);
					test_syntax_print(mlog, syntax, 0);
					refer_free(syntax);
				}
				refer_free(syntaxor);
			}
			refer_free(source);
		}
		refer_free(nstr);
	}
}

void gomi_test_form(miko_wink_gomi_s *gomi, mlog_s *restrict mlog);
void gomi_test(miko_wink_gomi_s *gomi, mlog_s *restrict mlog);

int main(int argc, const char *argv[])
{
	mlog_s *mlog;
	miko_env_s *env;
	miko_wink_gomi_s *gomi;
	if ((mlog = mlog_alloc(0)))
	{
		mlog_set_report(mlog, mlog_report_stdout_func, NULL);
		mlog_printf(mlog, "miko.bin start ...\n");
		if ((env = miko_env_create(mlog, (miko_env_register_f []) {miko_std_env_register, NULL})))
		{
			mlog_printf(mlog, "miko.env = %p ...\n", env);
			if ((gomi = miko_wink_gomi_alloc()))
			{
				mlog_printf(mlog, "miko.wink.gomi = %p ...\n", gomi);
				miko_wink_gomi_default_report(gomi, mlog, 1);
				miko_wink_gomi_call_cycle(gomi, 200);
				test_std(mlog);
				// gomi_test(gomi, mlog);
				// gomi_test_form(gomi, mlog);
				refer_free(gomi);
			}
			refer_free(env);
		}
		mlog_printf(mlog, "miko.bin end ...\n");
		refer_free(mlog);
	}
	return 0;
}

// test miko.wink.gomi ...

void gomi_test_form(miko_wink_gomi_s *gomi, mlog_s *restrict mlog)
{
	uint64_t start_msec, stop_msec;
	uintptr_t range_min, range_max;
	uintptr_t i, n;
	n = (1 << 20);
	range_min = (1 << 12);
	range_max = range_min * 2;
	start_msec = yaw_timestamp_msec();
	{
		uintptr_t real_count, refer_count, wink_count;
		miko_form_w *restrict form;
		miko_form_w *restrict copy;
		miko_form_t *restrict p;
		miko_wink_t wink;
		refer_t refer;
		real_count = refer_count = wink_count = n;
		range_max -= range_min;
		if (range_max && (form = miko_form_alloc(gomi, real_count + refer_count + wink_count)))
		{
			while (real_count || refer_count || wink_count)
			{
				#define _rand_count_(_type_, _n_)  \
					_n_ = ((uintptr_t) rand() % range_max) + range_min;\
					if (_n_ > _type_##_count) _n_ = _type_##_count;\
					_type_##_count -= _n_
				#define _push_count_(_type_, _n_)  \
					if (_n_ && !miko_form_push_n_zero(form, miko_major_vtype__##_type_, _n_))\
						goto label_fail
				_rand_count_(real, n);
				_push_count_(real, n);
				_rand_count_(refer, n);
				_push_count_(refer, n);
				_rand_count_(wink, n);
				_push_count_(wink, n);
				#undef _rand_count_
				#undef _push_count_
			}
			mlog_printf(mlog, "form [%zu]...\n", form->count);
			p = form->form;
			n = form->count;
			for (i = 0; i < n; ++i)
			{
				switch (p[i].vtype)
				{
					case miko_major_vtype__refer:
						if ((refer = refer_alloz(0)))
						{
							p[i].var.refer = refer_save(refer);
							refer_free(refer);
						}
						break;
					case miko_major_vtype__wink:
						if ((wink = miko_wink_alloz(gomi, 0, NULL, NULL, NULL)))
						{
							miko_wink_used(p[i].var.wink = wink);
							miko_wink_unref(wink);
						}
						break;
					default:
						break;
				}
			}
			mlog_printf(mlog, "form [%zu]...\n", form->count);
			if ((copy = miko_form_alloc(gomi, form->count)))
			{
				miko_form_push_n_copy(copy, form->form, form->count);
				miko_wink_unref(form);
				form = copy;
				copy = NULL;
			}
			mlog_printf(mlog, "form => copy [%zu]...\n", form->count);
			miko_form_keep_count(form, 0);
			label_fail:
			miko_wink_unref(form);
		}
		miko_wink_gomi_call_gomi(gomi, NULL);
	}
	{
		miko_wink_report_t data;
		while (miko_wink_gomi_call_gomi(gomi, &data) && (data.former_root_inode + data.latter_lost_inode))
			;
	}
	stop_msec = yaw_timestamp_msec();
	mlog_printf(mlog, "gomi_test cost = %.3f msec\n", (double) (stop_msec - start_msec) / 1000);
}

#include <exbuffer.h>

typedef struct gomi_test_list_w gomi_test_list_w;
typedef struct gomi_test_array_w gomi_test_array_w;

struct gomi_test_list_w {
	gomi_test_list_w *next;
};

static miko_wink_view_s* gomi_test_list_view_func(miko_wink_view_s *restrict view, gomi_test_list_w *restrict list)
{
	return miko_wink_view_add(view, list->next);
}

static gomi_test_list_w* gomi_test_list_w_create(miko_wink_gomi_s *restrict gomi)
{
	return (gomi_test_list_w *) miko_wink_alloz(gomi, sizeof(gomi_test_list_w),
		NULL, (miko_wink_view_f) gomi_test_list_view_func, NULL);
}

struct gomi_test_array_w {
	exbuffer_t array;
};

static miko_wink_view_s* gomi_test_array_view_func(miko_wink_view_s *restrict view, gomi_test_array_w *restrict array)
{
	return miko_wink_view_add_array(view, (miko_wink_t *) array->array.data, array->array.used / sizeof(miko_wink_t));
}

static void gomi_test_array_free_func(gomi_test_array_w *restrict array)
{
	exbuffer_uini(&array->array);
}

static gomi_test_array_w* gomi_test_array_w_create(miko_wink_gomi_s *restrict gomi)
{
	gomi_test_array_w *restrict r;
	if ((r = ((gomi_test_array_w *) miko_wink_alloz(gomi, sizeof(gomi_test_array_w), NULL,
		(miko_wink_view_f) gomi_test_array_view_func,
		(miko_wink_free_f) gomi_test_array_free_func))))
	{
		if (exbuffer_init(&r->array, 0))
			return r;
		miko_wink_unref(r);
	}
	return NULL;
}

static void gomi_test_array_w_append(gomi_test_array_w *restrict r, miko_wink_t item)
{
	miko_wink_wlock_lock(r);
	exbuffer_append(&r->array, &item, sizeof(item));
	miko_wink_wlock_unlock(r);
	miko_wink_used(item);
}

void gomi_test(miko_wink_gomi_s *gomi, mlog_s *restrict mlog)
{
	uint64_t start_msec, stop_msec;
	uintptr_t i, n;
	n = (1 << 20);
	start_msec = yaw_timestamp_msec();
	{
		gomi_test_array_w *restrict pa;
		miko_wink_t item;
		pa = NULL;
		item = NULL;
		if ((pa = gomi_test_array_w_create(gomi)))
		{
			for (i = 0; i < n; ++i)
			{
				if ((item = miko_wink_alloc(gomi, 0, NULL, NULL, NULL)))
				{
					gomi_test_array_w_append(pa, item);
					miko_wink_unref(item);
					item = NULL;
				}
			}
			miko_wink_unref(pa);
		}
		miko_wink_gomi_call_gomi(gomi, NULL);
	}
	{
		gomi_test_list_w *restrict pl;
		gomi_test_list_w *restrict cl;
		pl = cl = NULL;
		if ((pl = gomi_test_list_w_create(gomi)))
		{
			for (i = 0; i < n; ++i)
			{
				if ((cl = gomi_test_list_w_create(gomi)))
				{
					miko_wink_used(cl->next = pl);
					miko_wink_unref(pl);
					pl = cl;
					cl = NULL;
				}
			}
			miko_wink_unref(pl);
		}
		miko_wink_gomi_call_gomi(gomi, NULL);
	}
	{
		miko_wink_report_t data;
		while (miko_wink_gomi_call_gomi(gomi, &data) && (data.former_root_inode + data.latter_lost_inode))
			;
	}
	stop_msec = yaw_timestamp_msec();
	mlog_printf(mlog, "gomi_test cost = %.3f msec", (double) (stop_msec - start_msec) / 1000);
}
