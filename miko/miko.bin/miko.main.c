#include "../miko.h"
#include "../miko.dev.h"
#include <stdio.h>

#include "../miko.base/miko.env.h"

int main(int argc, const char *argv[])
{
	mlog_s *mlog;
	miko_env_s *env;
	miko_iset_s *iset;
	if ((mlog = mlog_alloc(0)))
	{
		mlog_set_report(mlog, mlog_report_stdout_func, NULL);
		mlog_printf(mlog, "miko.bin start ...\n");
		if ((env = miko_env_alloc(mlog)))
		{
			if ((iset = miko_iset_alloc("test")))
				refer_free(iset);
			refer_free(env);
		}
		mlog_printf(mlog, "miko.bin end ...\n");
		refer_free(mlog);
	}
	return 0;
}
