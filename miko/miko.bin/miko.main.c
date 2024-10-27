#include "../miko.h"
#include "../miko.dev.h"
#include <stdio.h>
#include <yaw.h>

#include "../miko.base/miko.env.h"
#include "../miko.base/miko.wink.h"

void wink_free_func(void *restrict r)
{
	printf("free [%p]\n", r);
}

void gomi_test(miko_wink_gomi_s *gomi)
{
	miko_wink_s *a, *b, *c, *d, *e;
	if ((a = miko_wink_alloc(gomi, sizeof(miko_wink_s), wink_free_func)) &&
		(b = miko_wink_alloc(gomi, sizeof(miko_wink_s), wink_free_func)) &&
		(c = miko_wink_alloc(gomi, sizeof(miko_wink_s), wink_free_func)) &&
		(d = miko_wink_alloc(gomi, sizeof(miko_wink_s), wink_free_func)) &&
		(e = miko_wink_alloc(gomi, sizeof(miko_wink_s), wink_free_func)))
	{
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
			if ((gomi = miko_wink_gomi_alloc(1000)))
			{
				gomi_test(gomi);
				refer_free(gomi);
			}
			refer_free(env);
		}
		mlog_printf(mlog, "miko.bin end ...\n");
		refer_free(mlog);
	}
	return 0;
}
