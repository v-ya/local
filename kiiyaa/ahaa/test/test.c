#include <mlog.h>

extern mlog_s *restrict $mlog;

int main_do(uintptr_t argc, const char *const argv[])
{
	uintptr_t i;
	for (i = 0; i < argc; ++i)
		mlog_printf($mlog, "[%zu] %s\n", i, argv[i]);
	return 0;
}
