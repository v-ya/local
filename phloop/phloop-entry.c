#define _DEFAULT_SOURCE
#include "phloop.h"
#include "phloop-entry-arg.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>

static volatile uint32_t running;

static void signal_int_func(int sig)
{
	printf("wait stop ...\n");
	running = 0;
}

static int mlog_report_func(const char *restrict msg, size_t length, refer_t pri)
{
	if (length) fwrite(msg, 1, length, stdout);
	return 1;
}

static phloop_s* update(phloop_s *pl, const char *restrict path, struct timespec *restrict last)
{
	register struct timespec *restrict this;
	phloop_s *r;
	struct stat s;
	r = NULL;
	if (!stat(path, &s))
	{
		this = &s.st_mtim;
		r = pl;
		if (this->tv_sec != last->tv_sec || this->tv_nsec != last->tv_nsec)
		{
			last->tv_sec = this->tv_sec;
			last->tv_nsec = this->tv_nsec;
			r = phloop_update(pl, path);
		}
	}
	return r;
}

int main(int argc, const char *argv[])
{
	phloop_s *pl;
	phoneme_output_s *po;
	mlog_s *mlog;
	args_t args;
	struct timespec save;
	uint32_t tg, n;
	int r;
	r = 0;
	running = 1;
	signal(SIGINT, signal_int_func);
	if (argc == 1) args_help(argv[0], NULL);
	else
	{
		mlog = mlog_alloc(0);
		if (mlog)
		{
			mlog_set_report(mlog, (mlog_report_f) mlog_report_func, NULL);
			r = args_init(&args, argc, argv);
			if (r > 0) r = 0;
			else if (!r && !(r = args_check(&args)))
			{
				r = -1;
				po = phoneme_output_alloc(args.phloop.sampfre, 0, args.thread, tg = args.phloop.usleep_time);
				if (po)
				{
					if (args.thread && args.nice)
					{
						if (!phoneme_output_set_priority(po, args.nice))
							mlog_printf(mlog, "set phoneme output thread priority fail\n");
					}
					pl = phloop_alloc(&args.phloop, po, mlog);
					if (pl)
					{
						n = 0;
						save.tv_sec = save.tv_nsec = 0;
						while (running)
						{
							if (n > tg)
							{
								n -= tg;
								usleep(tg);
								continue;
							}
							n = args.update_time;
							if (!update(pl, args.input, &save))
								mlog_printf(mlog, "update script[%s] fail\n", args.input);
						}
						refer_free(pl);
						phloop_uini();
						r = 0;
					}
					else mlog_printf(mlog, "alloc phloop fail\n");
					refer_free(po);
				}
				else mlog_printf(mlog, "alloc phoneme output fail\n");
			}
			else mlog_printf(mlog, "args check fail ...\n");
			if (mlog->length) fwrite(mlog->mlog, 1, mlog->length, stdout);
			refer_free(mlog);
		}
	}
	return r;
}

