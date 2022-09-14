#include <console.h>
#include <mlog.h>
#include <hashmap.h>

hashmap_vlist_t* console_parse_initial(void);
void console_parse_finally(void);

extern mlog_s *$mlog;

console_s *console;

const char* initial(uintptr_t argc, const char *const argv[])
{
	if ((console = console_alloc("\e[34m" "console" "\e[0m")))
	{
		if (console_parse_initial())
			return NULL;
		refer_free(console);
	}
	return "console.main.initial";
}

void finally(void)
{
	console_parse_finally();
	refer_free(console);
}

int console_main(uintptr_t argc, const char *const argv[])
{
	console_inst_s *restrict inst;
	console_backup_initial(0);
	if ((inst = console_inst_alloc(0, 1, 16)))
	{
		console_do(console, inst, $mlog);
		mlog_printf($mlog, "console exited\n");
		refer_free(inst);
	}
	console_backup_resume();
	return 0;
}
