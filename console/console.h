#ifndef _console_h_
#define _console_h_

#include <refer.h>
#include <mlog.h>

typedef struct console_inst_s console_inst_s;
typedef struct console_s console_s;
typedef struct console_command_s console_command_s;

typedef console_s* (*console_command_main_f)(console_command_s *restrict cc, mlog_s *mlog, uintptr_t argc, const char *const argv[]);
typedef void (*console_command_help_f)(console_command_s *restrict cc, mlog_s *mlog, uintptr_t argc, const char *const argv[]);

struct console_command_s {
	console_command_main_f main;
	console_command_help_f help;
	const char *name;
	const char *desc;
	console_s *console;
};

console_inst_s* console_inst_alloc(int stdin_fd, int stdout_fd, uintptr_t keeprows);

console_s* console_alloc(const char *restrict name);
console_s* console_insert_command(console_s *restrict console, const char *restrict name, console_command_s *restrict command);
void console_delete_command(console_s *restrict console, const char *restrict name);

void console_do(console_s *restrict console, console_inst_s *restrict inst, mlog_s *mlog);

#endif
