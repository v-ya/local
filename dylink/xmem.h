#ifndef _xmem_h_
#define _xmem_h_

#include <stddef.h>
#include <stdint.h>

void* xmem_alloc(size_t size);
int xmem_free(void *xmem, size_t size);

#endif
