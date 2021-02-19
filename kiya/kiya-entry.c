#include "kiya.h"
#include <pocket/pocket-verify.h>

int main(int argc, const char *argv[])
{
	mlog_s *restrict mlog;
	kiya_t *restrict kiya;
	pocket_verify_s *restrict verify;
	pocket_s *restrict pocket;
	int i, ret = 0;
	if (argc > 1)
	{
		mlog = mlog_alloc(0);
		if (mlog)
		{
			mlog_set_report(mlog, mlog_report_stdout_func, NULL);
			verify = pocket_verify_default();
			if (verify)
			{
				kiya = kiya_alloc(mlog, 1 << 20);
				if (kiya)
				{
					for (i = 1; !ret && i < argc; ++i)
					{
						if (!(pocket = pocket_load(argv[i], verify)))
							break;
						if (!kiya_load(kiya, pocket))
							ret = -1;
						refer_free(pocket);
					}
					if (!ret) kiya_do(kiya, NULL, "main", &ret);
					kiya_free(kiya);
				}
				refer_free(verify);
			}
			refer_free(mlog);
		}
	}
	return ret;
}
