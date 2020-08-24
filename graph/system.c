#include "system.h"
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

char* graph_system_dump_string(const char *restrict s)
{
	register char *restrict r;
	register size_t n;
	n = strlen(s) + 1;
	r = (char *) malloc(n);
	if (r) return (char *) memcpy(r, s, n);
	return NULL;
}

uint8_t* graph_system_load_file(const char *restrict path, size_t *restrict size)
{
	uint8_t *restrict r;
	size_t n;
	int fd;
	r = NULL;
	fd = open(path, O_RDONLY);
	if (~fd)
	{
		struct stat stat;
		if (!fstat(fd, &stat))
		{
			if ((*size = n = stat.st_size))
			{
				r = (uint8_t *) malloc(n);
				if (r)
				{
					if (read(fd, r, n) < 0)
					{
						free(r);
						r = NULL;
					}
				}
			}
		}
		close(fd);
	}
	return r;
}

