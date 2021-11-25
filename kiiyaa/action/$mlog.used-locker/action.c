#include <mlog.h>
#include <yaw.h>

extern mlog_s *$mlog;

const char* action(uintptr_t argc, const char *argv[])
{
	const char *restrict error;
	yaw_lock_s *restrict locker;
	error = "action.$mlog.used-locker";
	if ((locker = yaw_lock_alloc_mutex()))
	{
		if (mlog_set_locker($mlog, locker))
			error = NULL;
		refer_free(locker);
	}
	return error;
}
