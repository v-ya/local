#include "../miko.h"
#include "../miko.dev.h"
#include "../miko.wink.h"
#include <stdio.h>
#include <yaw.h>

#include "../miko.base/miko.env.h"

void wink_free_func(void *restrict r)
{
	printf("free [%p]\n", r);
}

void gomi_test(miko_wink_gomi_s *gomi, mlog_s *restrict mlog)
{
	miko_wink_t a, b, c, d, e;
	miko_wink_gomi_default_report(gomi, mlog, 1);
	miko_wink_gomi_call_cycle(gomi, 200);
	if ((a = miko_wink_alloz(gomi, 0)) &&
		(b = miko_wink_alloz(gomi, 0)) &&
		(c = miko_wink_alloz(gomi, 0)) &&
		(d = miko_wink_alloz(gomi, 0)) &&
		(e = miko_wink_alloz(gomi, 0)))
	{
		miko_wink_set_free(a, wink_free_func);
		miko_wink_set_free(b, wink_free_func);
		miko_wink_set_free(c, wink_free_func);
		miko_wink_set_free(d, wink_free_func);
		miko_wink_set_free(e, wink_free_func);
		printf("%p, %p, %p, %p, %p\n", a, b, c, d, e);
		miko_wink_link(a, a);
		miko_wink_link(b, c);
		miko_wink_link(c, b);
		miko_wink_link(d, e);
		miko_wink_link(e, d);
		// miko_wink_link(a, b);
		miko_wink_link(a, e);
		(void) getchar();
		miko_wink_link(c, a);
		miko_wink_link(d, a);
		miko_wink_lost(a);
		(void) getchar();
		miko_wink_lost(b);
		miko_wink_lost(c);
		(void) getchar();
		miko_wink_lost(d);
		miko_wink_lost(e);
		(void) getchar();
		miko_wink_gomi_call_gomi(gomi, NULL);
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
		if ((env = miko_env_alloc(mlog)))
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
