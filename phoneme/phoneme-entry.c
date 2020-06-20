#include "phoneme_script.h"
#include "phoneme-entry-arg.h"
#include "phoneme_resource.h"
#include <wav.h>
#include <stdio.h>

static int mlog_report_func(const char *restrict msg, size_t length, refer_t pri)
{
	if (length) fwrite(msg, 1, length, stdout);
	return 1;
}

int main(int argc, const char *argv[])
{
	mlog_s *mlog;
	phoneme_script_s *ps;
	phoneme_output_s *po;
	args_t args;
	int r;
	r = 0;
	if (argc == 1) args_help(argv[0], NULL);
	else
	{
		mlog = mlog_alloc(0);
		if (mlog)
		{
			r = args_init(&args, argc, argv);
			if (r > 0) r = 0;
			else if (!r && !(r = args_check(&args)))
			{
				r = -1;
				mlog_set_report(mlog, (mlog_report_f) mlog_report_func, NULL);
				ps = phoneme_script_create_default(args.xmsize, mlog);
				if (ps)
				{
					if ((!args.core_path || phoneme_script_set_core_path(ps, args.core_path)) &&
						(!args.package_path || phoneme_script_set_package_path(ps, args.package_path)))
					{
						ps->base_time = args.base_time;
						ps->base_volume = args.base_volume;
						ps->base_fre_line = args.base_fre_line;
						ps->base_fre_step = args.base_fre_step;
						ps->space_time = args.space_time;
						ps->sdmax = args.sdmax;
						ps->dmax = args.dmax;
						ps->vstack_script_max = args.vstack_script_max;
						ps->vstack_phoneme_max = args.vstack_phoneme_max;
						po = phoneme_output_alloc(args.sampfre, 0, args.thread, 5000);
						if (po)
						{
							if (args.thread && args.nice)
							{
								if (!phoneme_output_set_priority(po, args.nice))
									mlog_printf(mlog, "set phoneme output thread priority fail\n");
							}
							if (phoneme_script_load(ps, args.input, po))
							{
								phoneme_output_join(po);
								if (!args.output || !wav_save_double(args.output, &po->output.buffer, po->frames, 1, po->sampfre))
									r = 0;
								else mlog_printf(mlog, "save output[%s] fail ...\n", args.output);
							}
							else mlog_printf(mlog, "load script[%s] fail ...\n", args.input);
							refer_free(po);
						}
						else mlog_printf(mlog, "alloc phoneme output fail ...\n");
					}
					else mlog_printf(mlog, "init phoneme script environment fail ...\n");
					refer_free(ps);
				}
				else mlog_printf(mlog, "alloc phoneme script environment fail ...\n");
			}
			else mlog_printf(mlog, "args check fail ...\n");
			if (mlog->length) fwrite(mlog->mlog, 1, mlog->length, stdout);
			refer_free(mlog);
		}
	}
	return r;
}

