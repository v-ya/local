#ifndef _console_type_h_
#define _console_type_h_

#include "console.h"
#include <exbuffer.h>
#include <vattr.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

typedef struct console_inst_line_t {
	exbuffer_t raw;
	exbuffer_t args;
} console_inst_line_t;

struct console_inst_s {
	int stdin;
	int stdout;
	uintptr_t keeprows;
	uintptr_t row_curr;
	uintptr_t row_have;
	struct termios stdin_orginal;
	exbuffer_t line_raw_cache;
	console_inst_line_t lines[];
};

struct console_s {
	refer_nstring_t name;
	volatile uintptr_t running;
	vattr_s *cmds; // => (console_command_s *)
	vattr_s *hint; // => (refer_nstring_t)
};

typedef exbuffer_t* (*console_inst_hint_f)(exbuffer_t *restrict hint, const void *pri, uintptr_t argc, const char *const *argv, uintptr_t last_argv_is_finish);

const char *const * console_inst_get(console_inst_s *restrict inst, uintptr_t *restrict argc, const exbuffer_t *restrict cname, console_inst_hint_f hint_func, const void *hint_pri);

console_s* console_insert_command_exit(console_s *restrict console);
console_s* console_insert_command_help(console_s *restrict console);

#endif
