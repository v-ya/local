#define _GNU_SOURCE
#include "refer.printf.h"
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

void refer_debug_printf(int fd, const char *restrict fmt, ...)
{
	char buffer[16384];
	va_list list;
	int n;
	va_start(list, fmt);
	n = vsnprintf(buffer, sizeof(buffer), fmt, list);
	if (n > 0) write(fd, buffer, n);
	va_end(list);
}
