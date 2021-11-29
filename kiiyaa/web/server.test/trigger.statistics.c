#include "statistics.h"
#include "../server/server.h"

typedef struct trigger_statistics_s {
	inst_web_server_value_s v;
	statistics_s *statistics;
} trigger_statistics_s;

static void trigger_statistics_free_func(trigger_statistics_s *restrict r)
{
	if (r->statistics)
		refer_free(r->statistics);
}

static uhttp_header_s* trigger_statistics(trigger_statistics_s *restrict r, refer_nstring_t name, web_server_request_t *restrict request)
{
	statistics_touch(r->statistics, uhttp_uri_get_path(request->request_uri));
	return NULL;
}

inst_web_server_value_s* trigger_alloc_statistics(const web_server_s *server)
{
	trigger_statistics_s *restrict r;
	r = NULL;
	if (!statistics) statistics = statistics_alloc();
	if (statistics && (r = (trigger_statistics_s *) refer_alloz(sizeof(trigger_statistics_s))))
	{
		refer_set_free(r, (refer_free_f) trigger_statistics_free_func);
		r->statistics = (statistics_s *) refer_save(statistics);
		r->v.value = (inst_web_server_value_f) trigger_statistics;
	}
	return &r->v;
}
