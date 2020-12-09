#include "iyii/iyii.h"
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

volatile uint32_t running = 1;

static void signal_int(int sig)
{
	if (running) running = 0;
	else exit(-1);
}

int main(void)
{
	mlog_s *mlog;
	iyii_s *iyii;
	signal(SIGINT, signal_int);
	mlog = mlog_alloc(0);
	if (mlog)
	{
		mlog_set_report(mlog, mlog_report_stdout_func, NULL);
		iyii = iyii_alloc(mlog);
		if (iyii)
		{
			mlog_printf(mlog, "iyii alloc ok\n");
			while (running)
			{
				void usleep(int);
				if (iyii_do_events(iyii))
					break;
				usleep(10000);
			}
			refer_free(iyii);
		}
		refer_free(mlog);
	}
	return 0;
}
