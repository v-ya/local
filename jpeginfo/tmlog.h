#ifndef _tmlog_h_
#define _tmlog_h_

#include <mlog.h>

typedef struct tmlog_data_s {
	uint32_t next_need_tab;
	uint32_t space_count;
	uint32_t tab2space;
	uint32_t space_array_length;
	const char *restrict next_color;
	char space_array_data[];
} tmlog_data_s;

typedef enum tmlog_color_t {
	tmlog_color__back,
	tmlog_color__red,
	tmlog_color__green,
	tmlog_color__yellow,
	tmlog_color__blue,
	tmlog_color__purple,
	tmlog_color__aqua,
	tmlog_color__white,
	tmlog_color_max
} tmlog_color_t;

tmlog_data_s* tmlog_initial_report(mlog_s *restrict mlog, uint32_t tab2space);
uint32_t tmlog_get(tmlog_data_s *restrict td);
void tmlog_set(tmlog_data_s *restrict td, uint32_t ntab);
void tmlog_add(tmlog_data_s *restrict td, uint32_t ntab);
void tmlog_sub(tmlog_data_s *restrict td, uint32_t ntab);
void tmlog_color(tmlog_data_s *restrict td, tmlog_color_t color);

#endif
