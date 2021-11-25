#ifndef _kiiyaa_console_main__console_command_h_
#define _kiiyaa_console_main__console_command_h_

#include <refer.h>

typedef struct console_command_s console_command_s;
typedef void (*console_command_do_f)(console_command_s *restrict cc, uintptr_t argc, const char *const argv[]);
typedef const char* (*console_command_info_f)(console_command_s *restrict cc);

struct console_command_s {
	console_command_do_f main;
	console_command_do_f help;
	console_command_info_f name;
	console_command_info_f desc;
};

#endif
