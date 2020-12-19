#include "iyii/iyii.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

void usleep(int);

int main(void)
{
	mlog_s *mlog;
	iyii_s *iyii;
	mlog = mlog_alloc(0);
	if (mlog)
	{
		mlog_set_report(mlog, mlog_report_stdout_func, NULL);
		iyii = iyii_alloc(mlog, 1);
		if (iyii)
		{
			mlog_printf(mlog, "iyii alloc ok\n");
			iyii_wait_exit(iyii);
			refer_free(iyii);
		}
		mlog_printf(mlog, "mlog number: %lu\n", refer_save_number(mlog));
		refer_free(mlog);
	}
	return 0;
}
