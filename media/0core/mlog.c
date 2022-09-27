#include "mlog.h"
#include <yaw.h>

static void media_mlog_data_free(struct media_mlog_data_s *restrict r)
{
	if (r->mlog) refer_free(r->mlog);
	if (r->loglevel) refer_free(r->loglevel);
}

static struct media_mlog_data_s* media_mlog_data_alloc(mlog_s *restrict mlog, const char *restrict loglevel, uint64_t tsms_start)
{
	struct media_mlog_data_s *restrict r;
	if ((r = (struct media_mlog_data_s *) refer_alloz(sizeof(struct media_mlog_data_s))))
	{
		refer_set_free(r, (refer_free_f) media_mlog_data_free);
		r->mlog = (mlog_s *) refer_save(mlog);
		r->loglevel = refer_dump_string(loglevel);
		r->tsms_start = tsms_start;
		if (r->mlog && r->loglevel)
			return r;
		refer_free(r);
	}
	return NULL;
}

static int media_mlog_report_func(const char *restrict msg, size_t length, struct media_mlog_data_s *restrict data)
{
	uint64_t duration_msec;
	duration_msec = yaw_timestamp_msec() - data->tsms_start;
	mlog_printf(data->mlog,
		"[%zu.%03u %s] %s\n",
		(uintptr_t) (duration_msec / 1000),
		(uint32_t) (duration_msec % 1000),
		data->loglevel, msg);
	return 1;
}

mlog_s* media_mlog_alloc(mlog_s *restrict mlog, const char *restrict loglevel, uint64_t tsms_start, struct yaw_lock_s *restrict lock)
{
	struct media_mlog_data_s *restrict data;
	mlog_s *restrict r;
	r = NULL;
	if ((data = media_mlog_data_alloc(mlog, loglevel, tsms_start)))
	{
		if ((r = mlog_alloc(0)))
		{
			mlog_set_report(r, (mlog_report_f) media_mlog_report_func, data);
			if (lock) mlog_set_locker(r, lock);
		}
		refer_free(data);
	}
	return r;
}

static void media_mlog_print_rawdata_line_16(char *restrict hex, char *restrict ascii, const uint8_t *restrict d, uintptr_t n)
{
	static const char hex2c[16] = "0123456789abcdef";
	uintptr_t i;
	for (i = 0; i < n; ++i)
	{
		*hex++ = hex2c[d[i] >> 4];
		*hex++ = hex2c[d[i] & 15];
		*hex++ = ' ';
		*ascii++ = (d[i] >= ' ' && d[i] <= '~')?d[i]:'.';
	}
	while (i < 16)
	{
		*hex++ = ' ';
		*hex++ = ' ';
		*hex++ = ' ';
		++i;
	}
	*hex = 0;
	*ascii = 0;
}

void media_mlog_print_rawdata(mlog_s *restrict mlog, const char *restrict name, const void *restrict data, uintptr_t size)
{
	const uint8_t *restrict p;
	uintptr_t n;
	char hex[64], ascii[32];
	if (name) mlog_printf(mlog, "%s [%zu]:", name, size);
	p = (const uint8_t *) data;
	while (size)
	{
		n = 16;
		if (size < n) n = size;
		media_mlog_print_rawdata_line_16(hex, ascii, p, n);
		mlog_printf(mlog, "%s| %s", hex, ascii);
		p += n;
		size -= n;
	}
}
