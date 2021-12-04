#define _DEFAULT_SOURCE
#include "console.type.h"

static void console_inst_free_func(console_inst_s *restrict r)
{
	uintptr_t i;
	if (~r->stdin)
		tcsetattr(r->stdin, TCSANOW, &r->stdin_orginal);
	exbuffer_uini(&r->line_raw_cache);
	for (i = 0; i < r->keeprows; ++i)
	{
		exbuffer_uini(&r->lines[i].raw);
		exbuffer_uini(&r->lines[i].args);
	}
}

console_inst_s* console_inst_alloc(int stdin_fd, int stdout_fd, uintptr_t keeprows)
{
	console_inst_s *restrict r;
	struct termios attr;
	uintptr_t i;
	if (!keeprows) keeprows = 1;
	if ((r = (console_inst_s *) refer_alloz(sizeof(console_inst_s) + sizeof(console_inst_line_t) * keeprows)))
	{
		r->stdout = r->stdin = ~0;
		r->keeprows = keeprows;
		refer_set_free(r, (refer_free_f) console_inst_free_func);
		if (tcgetattr(stdout_fd, &attr))
			goto label_fail;
		if (tcgetattr(stdin_fd, &attr))
			goto label_fail;
		r->stdin_orginal = attr;
		r->stdin = stdin_fd;
		r->stdout = stdout_fd;
		attr.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | IXON);
		attr.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
		attr.c_cflag &= ~(CSIZE | PARENB);
		attr.c_cflag |= CS8;
		if (tcsetattr(stdin_fd, TCSANOW, &attr))
			goto label_fail;
		if (!exbuffer_init(&r->line_raw_cache, 0))
			goto label_fail;
		for (i = 0; i < keeprows; ++i)
		{
			if (!exbuffer_init(&r->lines[i].raw, 0))
				goto label_fail;
			if (!exbuffer_init(&r->lines[i].args, 0))
				goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

static const uint8_t console_inst_char_case[256] = {
	// other ctrl
	// space
	[0] = 1,
	['\t'] = 1,
	['\n'] = 1,
	['\r'] = 1,
	[' '] = 1,
	// visible
	['!' ... '~'] = 2,
	[128 ... 255] = 2
};

typedef enum console_ctrl_t {
	console_ctrl_null,
	console_ctrl_up,
	console_ctrl_down,
	console_ctrl_right,
	console_ctrl_left,
} console_ctrl_t;

static console_ctrl_t console_inst_get_ctrl(const uint8_t *restrict d, uintptr_t n)
{
	if (n)
	{
		if (*d == '[' && n >= 2)
		{
			switch (d[1])
			{
				case 'A': return console_ctrl_up;
				case 'B': return console_ctrl_down;
				case 'C': return console_ctrl_right;
				case 'D': return console_ctrl_left;
				default: goto label_not_match;
			}
		}
	}
	label_not_match:
	return console_ctrl_null;
}

static inline uintptr_t console_inst_get_ctrl_length(console_ctrl_t ctrl)
{
	static const uintptr_t map[] = {
		[console_ctrl_null] = 0,
		[console_ctrl_up] = 2,
		[console_ctrl_down] = 2,
		[console_ctrl_right] = 2,
		[console_ctrl_left] = 2,
	};
	return map[ctrl];
}

static const char *const * console_inst_get_args(console_inst_line_t *restrict line, uintptr_t *restrict argc)
{
	uint8_t *restrict d;
	uintptr_t n;
	int k, c;
	d = line->raw.data;
	n = line->raw.used;
	line->args.used = 0;
	k = 0;
	while (n)
	{
		if (!(c = (console_inst_char_case[*d] == 2)))
			*d = 0;
		if (k != c)
		{
			if (c) exbuffer_append(&line->args, (const void *) &d, sizeof(d));
			k = c;
		}
		++d;
		--n;
	}
	*argc = line->args.used / sizeof(d);
	return (const char *const *) line->args.data;
}

static exbuffer_t* console_inst_get_back_raw(exbuffer_t *restrict braw, const exbuffer_t *restrict raw)
{
	uint8_t *restrict d;
	uintptr_t n;
	braw->used = 0;
	if (!raw->used)
		goto label_okay;
	if ((d = exbuffer_append(braw, raw->data, n = raw->used)))
	{
		do {
			if (console_inst_char_case[*d] < 2)
				*d = ' ';
			++d;
		} while (--n);
		label_okay:
		return braw;
	}
	return NULL;
}

static console_inst_line_t* console_inst_get_new_line(console_inst_s *restrict inst)
{
	if (inst->lines[inst->row_curr].args.used)
	{
		if (inst->row_have < inst->keeprows)
			inst->row_have += 1;
		inst->row_curr = (inst->row_curr + 1) % inst->keeprows;
	}
	return &inst->lines[inst->row_curr];
}

static void console_inst_get_reecho(console_inst_s *restrict inst, const exbuffer_t *restrict cname, console_inst_line_t *line)
{
	write(inst->stdout, (const uint8_t [8]) { 0x1b, '[', '2', 'K', 0x1b, '[', '0', 'G' }, 8);
	if (cname && cname->used)
		write(inst->stdout, cname->data, cname->used);
	if (console_inst_get_back_raw(&inst->line_raw_cache, &line->raw))
		write(inst->stdout, inst->line_raw_cache.data, inst->line_raw_cache.used);
}

static inline void console_inst_get_copy_line(console_inst_line_t *dst, const console_inst_line_t *src)
{
	dst->raw.used = 0;
	dst->args.used = 0;
	if (dst != src)
	{
		const uint8_t *restrict d;
		uintptr_t n;
		d = src->raw.data;
		n = src->raw.used;
		while (n && console_inst_char_case[d[n - 1]] < 2)
			--n;
		exbuffer_append(&dst->raw, d, n);
	}
}

static console_inst_line_t* console_inst_get_select_prev(console_inst_s *restrict inst, console_inst_line_t *select)
{
	uintptr_t index, edge;
	index = ((uintptr_t) select - (uintptr_t) inst->lines) / sizeof(*select);
	index = (index + inst->keeprows - 1) % inst->keeprows;
	edge = (inst->row_curr + (inst->keeprows << 1) - inst->row_have - 1) % inst->keeprows;
	if (index != edge)
		return &inst->lines[index];
	return select;
}

static console_inst_line_t* console_inst_get_select_next(console_inst_s *restrict inst, console_inst_line_t *select)
{
	uintptr_t index, edge;
	index = ((uintptr_t) select - (uintptr_t) inst->lines) / sizeof(*select);
	index = (index + 1) % inst->keeprows;
	edge = (inst->row_curr + 1) % inst->keeprows;
	if (index != edge)
		return &inst->lines[index];
	return select;
}

const char *const * console_inst_get(console_inst_s *restrict inst, uintptr_t *restrict argc, const exbuffer_t *restrict cname, console_inst_hint_f hint_func, const void *hint_pri)
{
	console_inst_line_t *restrict line, *select;
	uint8_t *data;
	ssize_t length;
	uintptr_t n;
	console_ctrl_t ctrl;
	select = line = console_inst_get_new_line(inst);
	line->raw.used = 0;
	line->args.used = 0;
	if (cname && cname->used)
		write(inst->stdout, cname->data, cname->used);
	for (;;)
	{
		length = read(inst->stdin, data = inst->line_raw_cache.data, inst->line_raw_cache.size);
		if (length < 0)
			goto label_exit;
		while (length > 0)
		{
			switch (*data)
			{
				case 0x00:
				case 0x03:
				case 0x15:
				case 0x1c:
					write(inst->stdout, (const uint8_t [1]) { 0x0a }, 1);
					goto label_exit;
				case 0x09:
					// <tab>
					if (hint_func && exbuffer_need(&line->raw, line->raw.used + 1))
					{
						const char *const *hint_argv;
						uintptr_t hint_argc;
						line->raw.data[line->raw.used] = 0;
						hint_argv = console_inst_get_args(line, &hint_argc);
						inst->line_raw_cache.used = 0;
						if (hint_func(&inst->line_raw_cache, hint_pri, hint_argc, hint_argv))
						{
							if (inst->line_raw_cache.used)
							{
								if (exbuffer_need(&line->raw, line->raw.used + inst->line_raw_cache.used))
								{
									memcpy(line->raw.data + line->raw.used, inst->line_raw_cache.data, inst->line_raw_cache.used);
									line->raw.used += inst->line_raw_cache.used;
									write(inst->stdout, inst->line_raw_cache.data, inst->line_raw_cache.used);
								}
							}
							else console_inst_get_reecho(inst, cname, line);
						}
					}
					break;
				case 0x1b:
					switch ((ctrl = console_inst_get_ctrl(data + 1, length - 1)))
					{
						case console_ctrl_up:
							select = console_inst_get_select_prev(inst, select);
							goto label_copy_reecho;
						case console_ctrl_down:
							select = console_inst_get_select_next(inst, select);
							goto label_copy_reecho;
						default:
							goto label_skip_esc;
					}
					label_copy_reecho:
					console_inst_get_copy_line(line, select);
					console_inst_get_reecho(inst, cname, line);
					label_skip_esc:
					n = console_inst_get_ctrl_length(ctrl);
					data += n;
					length -= n;
					break;
				case 0x7f:
					if (line->raw.used)
					{
						line->raw.used -= 1;
						write(inst->stdout, (const uint8_t [3]) { 0x08, 0x20, 0x08 }, 3);
					}
					break;
				default:
					if (console_inst_char_case[*data])
					{
						if (line->raw.used < line->raw.size)
						{
							label_push:
							line->raw.data[line->raw.used++] = *data;
							write(inst->stdout, data, 1);
							if (*data == '\n')
								goto label_okay_get_args;
						}
						else if (exbuffer_need(&line->raw, line->raw.size << 1))
							goto label_push;
					}
					break;
			}
			++data;
			--length;
		}
	}
	label_exit:
	return NULL;
	label_okay_get_args:
	return console_inst_get_args(line, argc);
}
