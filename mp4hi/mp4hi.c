#include "args.h"
#include <mpeg4.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

static uint8_t* load_file(const char *restrict path, uint64_t *restrict rsize)
{
	uint8_t *restrict rdata;
	struct stat st;
	int fd;
	if (!stat(path, &st))
	{
		if ((rdata = malloc((size_t) (*rsize = (uint64_t) st.st_size))))
		{
			if ((fd = open(path, O_RDONLY)) >= 0)
			{
				if (read(fd, rdata, st.st_size) == (ssize_t) *rsize)
				{
					close(fd);
					return rdata;
				}
				close(fd);
			}
			free(rdata);
		}
	}
	return NULL;
}

int main(int argc, const char *argv[])
{
	args_t args;
	const mpeg4_s *restrict inst;
	mlog_s *restrict mlog;
	if (args_get(&args, argc, argv))
	{
		if ((inst = mpeg4_alloc()))
		{
			if ((mlog = mlog_alloc(0)))
			{
				mlog_set_report(mlog, mlog_report_stdout_func, NULL);
				if (args.input)
				{
					uint8_t *restrict data;
					uint64_t size;
					if ((data = load_file(args.input, &size)))
					{
						if (!mpeg4_dump(inst, mlog, data, size, !!args.dump_samples))
							mlog_printf(mlog, "** unexpected termination **\n");
						free(data);
					}
				}
				else if (args.libmpeg4_verbose)
					mpeg4_verbose_dump(inst, mlog);
				refer_free(mlog);
			}
			refer_free(inst);
		}
	}
	return 0;
}
