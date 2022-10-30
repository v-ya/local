#include "tmlog.h"
#include <unistd.h>
#include <string.h>

static int tmlog_report_stdout_func(const char *restrict msg, size_t length, tmlog_data_s *restrict pri)
{
	const char *restrict endptr;
	uintptr_t n;
	uint32_t need_tab;
	while (length)
	{
		need_tab = pri->next_need_tab;
		if ((endptr = memchr(msg, '\n', length)))
		{
			n = (uintptr_t) endptr - (uintptr_t) msg + 1;
			pri->next_need_tab = 1;
		}
		else
		{
			n = length;
			pri->next_need_tab = 0;
		}
		if (pri->next_color)
		{
			write(STDOUT_FILENO, pri->next_color, strlen(pri->next_color));
			pri->next_color = NULL;
		}
		if (need_tab && (need_tab = pri->space_count))
		{
			while (need_tab > pri->space_array_length)
			{
				write(STDOUT_FILENO, pri->space_array_data, pri->space_array_length);
				need_tab -= pri->space_array_length;
			}
			write(STDOUT_FILENO, pri->space_array_data, need_tab);
		}
		if (length)
			write(STDOUT_FILENO, msg, n);
		msg += n;
		length -= n;
	}
	return 1;
}

static tmlog_data_s* tmlog_data_alloc(uint32_t space_array_length, uint32_t tab2space)
{
	tmlog_data_s *restrict r;
	if ((r = (tmlog_data_s *) refer_alloc(sizeof(tmlog_data_s) + (uintptr_t) space_array_length)))
	{
		r->next_need_tab = 0;
		r->space_count = 0;
		r->tab2space = tab2space;
		r->space_array_length = space_array_length;
		r->next_color = NULL;
		memset(r->space_array_data, ' ', (uintptr_t) space_array_length);
	}
	return r;
}

tmlog_data_s* tmlog_initial_report(mlog_s *restrict mlog, uint32_t tab2space)
{
	tmlog_data_s *restrict td;
	if ((td = tmlog_data_alloc(64, tab2space)))
	{
		mlog_set_report(mlog, (mlog_report_f) tmlog_report_stdout_func, td);
		refer_free(td);
	}
	return td;
}

uint32_t tmlog_get(tmlog_data_s *restrict td)
{
	if (td && td->tab2space)
		return td->space_count / td->tab2space;
	return 0;
}

void tmlog_set(tmlog_data_s *restrict td, uint32_t ntab)
{
	if (td) td->space_count = ntab * td->tab2space;
}

void tmlog_add(tmlog_data_s *restrict td, uint32_t ntab)
{
	if (td) td->space_count += ntab * td->tab2space;
}

void tmlog_sub(tmlog_data_s *restrict td, uint32_t ntab)
{
	if (td)
	{
		ntab *= td->tab2space;
		if (td->space_count >= ntab)
			td->space_count -= ntab;
		else td->space_count = 0;
	}
}

void tmlog_color(tmlog_data_s *restrict td, tmlog_color_t color)
{
	static const char *const e2c[tmlog_color_max] = {
		"\e[0m",
		"\e[31m",
		"\e[32m",
		"\e[33m",
		"\e[34m",
		"\e[35m",
		"\e[36m",
		"\e[37m",
	};
	if ((uint32_t) color < tmlog_color_max)
		td->next_color = e2c[color];
}
