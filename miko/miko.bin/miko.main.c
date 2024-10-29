#include "../miko.h"
#include "../miko.dev.h"
#include "../miko.wink.h"
#include "../miko.std.h"
#include <stdio.h>
#include <yaw.h>

void gomi_test(miko_wink_gomi_s *gomi, mlog_s *restrict mlog)
{
	miko_wink_t p, c;
	uintptr_t i, n;
	n = (1 << 20);
	miko_wink_gomi_default_report(gomi, mlog, 1);
	miko_wink_gomi_call_cycle(gomi, 200);
	p = c = NULL;
	for (i = 0; i < n; ++i)
	{
		if (p || (p = miko_wink_alloz(gomi, 0)))
		{
			if ((c = miko_wink_alloz(gomi, 0)))
			{
				miko_wink_link(p, c);
				miko_wink_lost(c);
			}
		}
	}
	if (p) miko_wink_lost(p);
	miko_wink_gomi_call_gomi(gomi, NULL);
	p = c = NULL;
	for (i = 0; i < n; ++i)
	{
		if (c || (c = miko_wink_alloz(gomi, 0)))
		{
			if ((p = miko_wink_alloz(gomi, 0)))
			{
				miko_wink_link(p, c);
				miko_wink_lost(c);
				c = p;
			}
		}
	}
	if (p) miko_wink_lost(p);
	miko_wink_gomi_call_gomi(gomi, NULL);
	{
		miko_wink_report_t data;
		while (miko_wink_gomi_call_gomi(gomi, &data) && (data.latter_root_inode + data.latter_lost_inode))
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
				// gomi_test(gomi, mlog);
				refer_free(gomi);
			}
			refer_free(env);
		}
		mlog_printf(mlog, "miko.bin end ...\n");
		refer_free(mlog);
	}
	return 0;
}
