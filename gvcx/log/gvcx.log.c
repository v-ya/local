#include "gvcx.log.h"
#include <memory.h>

typedef struct gvcx_log_wdone_t gvcx_log_wdone_t;

struct gvcx_log_wdone_t {
	const char *data;
	uintptr_t size;
	uintptr_t nspace;
};

static void gvcx_log_data_free_func(gvcx_log_data_s *restrict r)
{
	if (r->output) refer_free(r->output);
}

static gvcx_log_data_s* gvcx_log_data_alloc(mlog_s *restrict input, mlog_s *restrict output)
{
	gvcx_log_data_s *restrict r;
	if ((r = (gvcx_log_data_s *) refer_alloz(sizeof(gvcx_log_data_s))))
	{
		refer_set_free(r, (refer_free_f) gvcx_log_data_free_func);
		r->input = input;
		r->output = (mlog_s *) refer_save(output);
		r->nindex = 0;
		r->nspace = 4;
		r->mindex = 64;
		r->mspace = 16;
		return r;
	}
	return NULL;
}

static mlog_s* gvcx_log_wdone(mlog_s *restrict mlog, const gvcx_log_wdone_t *restrict pri, uintptr_t *restrict rlength)
{
	char *restrict p;
	uintptr_t nspace;
	uintptr_t size;
	uintptr_t n;
	if ((p = mlog_wneed(mlog, n = (nspace = pri->nspace) + (size = pri->size))))
	{
		if (nspace) memset(p, ' ', nspace);
		if (size) memcpy(p + nspace, pri->data, size);
		*rlength = n;
		return mlog;
	}
	return NULL;
}

static void gvcx_log_report_initial_wdoen(gvcx_log_wdone_t *restrict param, const char *restrict p, uintptr_t n, uint32_t nindex, uint32_t nspace)
{
	while (n && *p == '+')
		nindex += 1, p += 1, n -= 1;
	param->data = p;
	param->size = n;
	param->nspace = (uintptr_t) nindex * nspace;
}

static int gvcx_log_report(const char *restrict msg, uintptr_t length, const gvcx_log_data_s *restrict pri)
{
	gvcx_log_wdone_t param;
	mlog_s *restrict input;
	const char *restrict p;
	const char *restrict q;
	p = (input = pri->input)->mlog;
	while (length && (q = memchr(msg, '\n', length)))
	{
		q += 1;
		gvcx_log_report_initial_wdoen(&param, p, (uintptr_t) q - (uintptr_t) p, pri->nindex, pri->nspace);
		mlog_wdone(pri->output, (mlog_wdone_f) gvcx_log_wdone, &param);
		length = (uintptr_t) msg + length - (uintptr_t) q;
		p = msg = q;
	}
	if ((uintptr_t) msg == (uintptr_t) p)
	{
		if (length) memmove(input->mlog, msg, length);
		input->length = length;
		return 1;
	}
	return 0;
}

static void gvcx_log_inst_free_func(gvcx_log_inst_s *restrict r)
{
	if (r->data) refer_free(r->data);
	if (r->log.input) refer_free(r->log.input);
}

gvcx_log_s* gvcx_log_alloc(mlog_s *restrict output, struct yaw_lock_s *restrict locker)
{
	gvcx_log_inst_s *restrict r;
	if ((r = (gvcx_log_inst_s *) refer_alloz(sizeof(gvcx_log_inst_s))))
	{
		refer_set_free(r, (refer_free_f) gvcx_log_inst_free_func);
		if ((r->log.input = mlog_alloc(0)))
		{
			if ((r->data = gvcx_log_data_alloc(r->log.input, output)))
			{
				mlog_set_report(r->log.input, (mlog_report_f) gvcx_log_report, r->data);
				mlog_set_locker(r->log.input, locker);
				return &r->log;
			}
		}
		refer_free(r);
	}
	return NULL;
}

gvcx_log_s* gvcx_log_nspace(gvcx_log_s *restrict log, uint32_t nspace)
{
	gvcx_log_data_s *restrict data;
	if (log)
	{
		data = ((gvcx_log_inst_s *) log)->data;
		if (nspace <= data->mspace)
		{
			data->nspace = nspace;
			return log;
		}
	}
	return NULL;
}

gvcx_log_s* gvcx_log_push(gvcx_log_s *restrict log, uint32_t nindex)
{
	gvcx_log_data_s *restrict data;
	if (log)
	{
		data = ((gvcx_log_inst_s *) log)->data;
		if (nindex <= data->mindex - data->nindex)
		{
			data->nindex += nindex;
			return log;
		}
	}
	return NULL;
}

gvcx_log_s* gvcx_log_pop(gvcx_log_s *restrict log, uint32_t nindex)
{
	gvcx_log_data_s *restrict data;
	if (log)
	{
		data = ((gvcx_log_inst_s *) log)->data;
		if (nindex <= data->nindex)
		{
			data->nindex -= nindex;
			return log;
		}
	}
	return NULL;
}

void gvcx_log_reset(gvcx_log_s *restrict log)
{
	gvcx_log_data_s *restrict data;
	if (log)
	{
		data = ((gvcx_log_inst_s *) log)->data;
		data->nindex = 0;
	}
}
