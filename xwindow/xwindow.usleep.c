#define _DEFAULT_SOURCE
#include "xwindow.private.h"
#include <unistd.h>

void xwindow_usleep(uint32_t us)
{
	usleep(us);
}
