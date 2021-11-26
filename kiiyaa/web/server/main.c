#include "server.h"
#include <mlog.h>

inst_web_server_s *inst;

web_server_s* web$server$get_save(void)
{
	return (web_server_s *) refer_save(inst->server);
}

static uintptr_t* initial_args_compare_okay_get(const char *restrict name, const char *restrict string, uintptr_t *restrict value)
{
	while (*name && *string)
	{
		if (*name != *string)
			goto label_compare_fail;
		++name;
		++string;
	}
	if (!*name && *string == '=')
	{
		uintptr_t n, c;
		++string;
		n = 0;
		if (*string == 'x')
		{
			// base 16
			++string;
			while (*string)
			{
				if (*string >= '0' && *string <= '9')
					c = (uintptr_t) (*string - '0');
				else if (*string >= 'a' && *string <= 'f')
					c = (uintptr_t) (*string - ('a' - 10));
				else if (*string >= 'A' && *string <= 'F')
					c = (uintptr_t) (*string - ('A' - 10));
				else break;
				n = (n << 4) | c;
				++string;
			}
		}
		else
		{
			// base 10
			while (*string)
			{
				if (*string >= '0' && *string <= '9')
					c = (uintptr_t) (*string - '0');
				else break;
				n = n * 10 + c;
				++string;
			}
		}
		*value = n;
		return value;
	}
	label_compare_fail:
	return NULL;
}

static void initial_args(web_server_limit_t *restrict limit, uintptr_t argc, const char *const argv[])
{
	extern mlog_s *$mlog;
	uintptr_t i, v;
	for (i = 1; i < argc; ++i)
	{
		#define compare_and_set(_m) (initial_args_compare_okay_get(#_m, argv[i], &v)) limit->_m = v
		if compare_and_set(http_max_length);
		else if compare_and_set(body_max_length);
		else if compare_and_set(transport_recv_timeout_ms);
		else if compare_and_set(transport_send_timeout_ms);
		else if compare_and_set(working_number);
		else if compare_and_set(detach_number);
		else if compare_and_set(queue_depth_size);
		else if compare_and_set(wait_req_max_number);
		else mlog_printf($mlog, "[%s] unknow arg[%zu](%s)\n", argv[0], i, argv[i]);
		#undef compare_and_set
	}
}

const char* initial(uintptr_t argc, const char *const argv[])
{
	inst_web_server_s* inst_web_server_alloc(const web_server_limit_t *restrict limit);
	web_server_limit_t limit = {
		.http_max_length = 0,
		.body_max_length = 0,
		.transport_recv_timeout_ms = 0,
		.transport_send_timeout_ms = 0,
		.working_number = 0,
		.detach_number = 0,
		.queue_depth_size = 0,
		.wait_req_max_number = 0
	};
	initial_args(&limit, argc, argv);
	if ((inst = inst_web_server_alloc(&limit)))
		return NULL;
	return "web.server.initial";
}

void finally(void)
{
	inst_web_server_s *r;
	if ((r = inst))
	{
		refer_free(r);
		inst = NULL;
	}
}
