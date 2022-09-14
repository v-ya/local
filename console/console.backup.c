
#include "console.type.h"
#include <stdlib.h>

typedef enum console_backup_status_t {
	console_backup_status_none,
	console_backup_status_wait,
	console_backup_status_okay
} console_backup_status_t;

typedef struct console_backup_t {
	console_backup_status_t status;
	int stdin_fd;
	struct termios orginal;
} console_backup_t;

static console_backup_t backup;

int console_backup_initial(int stdin_fd)
{
	if (backup.status == console_backup_status_none)
	{
		if (~stdin_fd && !tcgetattr(stdin_fd, &backup.orginal))
		{
			backup.stdin_fd = stdin_fd;
			backup.status = console_backup_status_wait;
			return 0;
		}
	}
	return -1;
}

void console_backup_resume(void)
{
	if (backup.status == console_backup_status_wait)
	{
		tcsetattr(backup.stdin_fd, TCSAFLUSH, &backup.orginal);
		backup.status = console_backup_status_okay;
	}
}
