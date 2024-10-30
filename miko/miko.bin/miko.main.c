#include "../header/miko.h"
#include "../header/miko.api.h"
#include "../header/miko.wink.h"
#include "../header/miko.std.h"
#include <stdio.h>
#include <yaw.h>

// test miko.wink.gomi ...

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
	uintptr_t i, n;
	n = (1 << 20);
	miko_wink_gomi_default_report(gomi, mlog, 1);
	miko_wink_gomi_call_cycle(gomi, 200);
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
}

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
			if ((gomi = miko_wink_gomi_alloc()))
			{
				gomi_test(gomi, mlog);
				refer_free(gomi);
			}
			refer_free(env);
		}
		mlog_printf(mlog, "miko.bin end ...\n");
		refer_free(mlog);
	}
	return 0;
}
