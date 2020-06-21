#include "phloop.h"
#include <phoneme/phoneme_resource.h>
#include <memory.h>

void phloop_free_func(phloop_s *restrict r)
{
	if (r->aloop) refer_free(r->aloop);
	if (r->output) refer_free(r->output);
	if (r->mlog) refer_free(r->mlog);
	if (r->arg.core_path) refer_free((refer_t) r->arg.core_path);
	if (r->arg.package_path) refer_free((refer_t) r->arg.package_path);
}

phloop_s* phloop_alloc(phloop_arg_t *restrict arg, phoneme_output_s *restrict output, mlog_s *restrict mlog)
{
	register phloop_s *restrict r;
	if (arg && output)
	{
		r = (phloop_s *) refer_alloz(sizeof(phloop_s));
		if (r)
		{
			refer_set_free(r, (refer_free_f) phloop_free_func);
			memcpy(&r->arg, arg, sizeof(phloop_arg_t));
			if (arg->core_path) r->arg.core_path = phoneme_src_name_dump(arg->core_path);
			if (arg->package_path) r->arg.package_path = phoneme_src_name_dump(arg->package_path);
			if ((!arg->core_path || r->arg.core_path) && (!arg->package_path || r->arg.package_path))
			{
				r->output = (phoneme_output_s *) refer_save(output);
				r->mlog = (mlog_s *) refer_save(mlog);
				if ((r->aloop = aloop_alloc(r->arg.usleep_time)))
					return r;
			}
			refer_free(r);
		}
	}
	return NULL;
}

phloop_s* phloop_update(phloop_s *restrict phloop, const char *script_path)
{
	register phoneme_script_s *restrict ps;
	register phoneme_output_s *restrict po;
	phloop_s *r;
	r = NULL;
	ps = phoneme_script_create_default(phloop->arg.xmsize, phloop->mlog);
	if (ps)
	{
		ps->base_time = phloop->arg.base_time;
		ps->base_volume = phloop->arg.base_volume;
		ps->base_fre_line = phloop->arg.base_fre_line;
		ps->base_fre_step = phloop->arg.base_fre_step;
		ps->space_time = phloop->arg.space_time;
		ps->sdmax = phloop->arg.sdmax;
		ps->dmax = phloop->arg.dmax;
		ps->vstack_script_max = phloop->arg.vstack_script_max;
		ps->vstack_phoneme_max = phloop->arg.vstack_phoneme_max;
		po = phloop->output;
		phoneme_output_clear(po);
		phoneme_output_set_sampfre(po, phloop->arg.sampfre);
		if (phoneme_script_load(ps, script_path, po))
		{
			double *v;
			phoneme_output_join(po);
			v = po->output.buffer;
			if (aloop_update_double(phloop->aloop, &v, 1, po->sampfre, po->frames))
				r = phloop;
			else mlog_printf(phloop->mlog, "[phloop_update] update audio loop fail ...\n");
		}
		else mlog_printf(phloop->mlog, "[phloop_update] load script[%s] fail ...\n", script_path);
		refer_free(ps);
	}
	else mlog_printf(phloop->mlog, "[phloop_update] create default phoneme script fail ...\n");
	return r;
}

void phloop_uini(void)
{
	aloop_uini();
}
