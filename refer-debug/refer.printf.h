#ifndef _refer_printf_h_
#define _refer_printf_h_

void refer_debug_printf(int fd, const char *restrict fmt, ...) __attribute__ ((__format__ (__printf__, 2, 0)));

#endif
