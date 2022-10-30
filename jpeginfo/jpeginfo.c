#include "jpeg.h"
#include "parser.h"
#include <fsys.h>

refer_nstring_t jpeginfo_load(const char *restrict path)
{
	fsys_file_s *restrict fp;
	refer_nstring_t r;
	r = NULL;
	if ((fp = fsys_file_alloc(path, fsys_file_flag_read)))
	{
		r = fsys_file_aread_all(fp);
		refer_free(fp);
	}
	return r;
}

int main(int argc, const char *argv[])
{
	mlog_s *restrict m;
	tmlog_data_s *restrict td;
	jpeg_parser_s *restrict p;
	refer_nstring_t d;
	jpeg_parser_target_t target;
	if ((m = mlog_alloc(0)))
	{
		td = tmlog_initial_report(m, 4);
		if (argc == 2)
		{
			if ((p = jpeg_parser_alloc(m, td)))
			{
				if ((d = jpeginfo_load(argv[1])))
				{
					target.data = (const uint8_t *) d->string;
					target.size = d->length;
					target.pos = 0;
					jpeg_parser_done(p, &target);
					refer_free(d);
				}
				else mlog_printf(m, "load [%s] fail\n", argv[1]);
				refer_free(p);
			}
			else mlog_printf(m, "create jpeg-paser fail\n");
		}
		else mlog_printf(m, "%s <jpeg-path>\n", argv[0]);
		refer_free(m);
	}
	return 0;
}
