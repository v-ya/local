#define _DEFAULT_SOURCE
#include "xmem.h"
#ifdef __unix__
#include <sys/mman.h>

void* xmem_alloc(size_t size)
{
	void *r;
	r = mmap(NULL, size,
		PROT_READ | PROT_WRITE | PROT_EXEC, 
		MAP_PRIVATE | MAP_ANONYMOUS,
		-1, 0
	);
	if (r != MAP_FAILED) return r;
	return NULL;
}

int xmem_free(void *xmem, size_t size)
{
	return munmap(xmem, size);
}

#else
#include <Windows.h>

void* xmem_alloc(size_t size)
{
	return VirtualAlloc(NULL, size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
}

int xmem_free(void *xmem, size_t size)
{
	return VirtualFree(xmem, size, MEM_RELEASE);
}

#endif
