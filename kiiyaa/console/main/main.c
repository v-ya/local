#define _GNU_SOURCE
#include "console_command.h"
#include <mlog.h>
#include <vattr.h>
#include <exbuffer.h>
#include <signal.h>
#include <unistd.h>
#include <poll.h>

typedef struct console_args_cache_t {
	exbuffer_t buffer;
	exbuffer_t cache;
	exbuffer_t argv;
	uintptr_t cache_pos;
	uintptr_t have_error;
} console_args_cache_t;

typedef struct console_args_t {
	const char *const *argv;
	uintptr_t argc;
} console_args_t;

hashmap_vlist_t* console_parse_initial(void);
void console_parse_finally(void);
console_command_s* console_command_alloc_exit(volatile uintptr_t *running);
console_command_s* console_command_alloc_help(void);

extern mlog_s *$mlog;

vattr_s *ccpool;
static console_args_cache_t args_cache;
static volatile uintptr_t running;

static void console_args_cache_finally(console_args_cache_t *restrict ca)
{
	exbuffer_uini(&ca->buffer);
	exbuffer_uini(&ca->cache);
	exbuffer_uini(&ca->argv);
}

static console_args_cache_t* console_args_cache_initial(console_args_cache_t *restrict ca)
{
	ca->cache_pos = 0;
	ca->have_error = 0;
	if (exbuffer_init(&ca->buffer, 0) && exbuffer_init(&ca->cache, 0) && exbuffer_init(&ca->argv, 0))
		return ca;
	console_args_cache_finally(ca);
	return NULL;
}

static console_args_cache_t* console_args_cache_check_line(console_args_cache_t *restrict ca)
{
	char *restrict p;
	uintptr_t i, n;
	if ((i = ca->cache_pos) < (n = ca->cache.used))
	{
		p = (char *) ca->cache.data;
		do {
			if (p[i] == '\n')
			{
				++i;
				if (!exbuffer_append(&ca->buffer, p + ca->cache_pos, i - ca->cache_pos))
					ca->have_error = 1;
				ca->cache_pos = i;
				return ca;
			}
		} while (++i < n);
		if (!exbuffer_append(&ca->buffer, p + ca->cache_pos, ca->cache.used - ca->cache_pos))
			ca->have_error = 1;
	}
	ca->cache_pos = ca->cache.used = 0;
	return NULL;
}

static console_args_cache_t* console_args_cache_tryget(console_args_cache_t *restrict ca)
{
	static struct pollfd fds = {
		.fd = STDIN_FILENO,
		.events = POLLIN,
		.revents = 0
	};
	ssize_t n;
	if (poll(&fds, 1, 50) > 0)
	{
		n = read(fds.fd, ca->cache.data + ca->cache.used, ca->cache.size - ca->cache.used);
		if (n > 0)
		{
			ca->cache.used += (uintptr_t) n;
			return ca;
		}
	}
	return NULL;
}

static char* console_args_cache_get_line(console_args_cache_t *restrict ca, uintptr_t *restrict length)
{
	if (console_args_cache_check_line(ca))
		goto label_okay;
	if (console_args_cache_tryget(ca) && console_args_cache_check_line(ca))
	{
		label_okay:
		*length = ca->buffer.used;
		return (char *) ca->buffer.data;
	}
	return NULL;
}

static const char** console_args_cache_push(console_args_cache_t *restrict ca, const char *value, uintptr_t *restrict argc)
{
	if (!exbuffer_append(&ca->argv, &value, sizeof(value)))
		ca->have_error = 1;
	*argc = ca->argv.used / sizeof(value);
	return (const char **) ca->argv.data;
}

static void console_args_cache_clear(console_args_cache_t *restrict ca)
{
	ca->buffer.used = 0;
	ca->argv.used = 0;
	ca->have_error = 0;
}

static console_args_t* get_command_args(console_args_t *restrict args, console_args_cache_t *restrict cache)
{
	uintptr_t i, n, isspace;
	char *restrict p;
	if ((p = console_args_cache_get_line(cache, &n)))
	{
		args->argv = NULL;
		args->argc = 0;
		isspace = 1;
		for (i = 0; i < n; ++i)
		{
			switch (p[i])
			{
				case ' ':
				case '\t':
				case '\r':
				case '\n':
					// space
					p[i] = 0;
					isspace = 1;
					break;
				default:
					// no space
					if (isspace)
					{
						isspace = 0;
						args->argv = console_args_cache_push(cache, p + i, &args->argc);
					}
					break;
			}
		}
		return args;
	}
	return NULL;
}

static vattr_vlist_t* initial_cc_exit(void)
{
	vattr_vlist_t *vl;
	console_command_s *cc;
	vl = NULL;
	if ((cc = console_command_alloc_exit(&running)))
	{
		vl = vattr_set(ccpool, "exit", cc);
		refer_free(cc);
	}
	return vl;
}

static vattr_vlist_t* initial_cc_help(void)
{
	vattr_vlist_t *vl;
	console_command_s *cc;
	vl = NULL;
	if ((cc = console_command_alloc_help()))
	{
		vl = vattr_set(ccpool, "help", cc);
		refer_free(cc);
	}
	return vl;
}

const char* initial(uintptr_t argc, const char *const argv[])
{
	if (console_args_cache_initial(&args_cache))
	{
		if ((ccpool = vattr_alloc()))
		{
			if (console_parse_initial())
			{
				if (initial_cc_exit() &&
					initial_cc_help())
					return NULL;
				console_parse_finally();
			}
			refer_free(ccpool);
		}
		console_args_cache_finally(&args_cache);
	}
	return "console.main.initial";
}

void finally(void)
{
	console_parse_finally();
	refer_free(ccpool);
	console_args_cache_finally(&args_cache);
}

static void console_main_signal_int(int s)
{
	running = 0;
}

int console_main(uintptr_t argc, const char *const argv[])
{
	console_args_t args;
	running = 1;
	signal(SIGINT, console_main_signal_int);
	goto label_entry;
	while (running)
	{
		if (get_command_args(&args, &args_cache))
		{
			if (args.argc && !args_cache.have_error)
			{
				console_command_s *restrict cc;
				cc = (console_command_s *) vattr_get_first(ccpool, args.argv[0]);
				if (cc && cc->main)
					cc->main(cc, args.argc, args.argv);
				else mlog_printf($mlog, "don't find (%s)\n", args.argv[0]);
			}
			console_args_cache_clear(&args_cache);
			label_entry:
			mlog_printf($mlog, "[console.main] ");
		}
	}
	mlog_printf($mlog, "console exited\n");
	return 0;
}
