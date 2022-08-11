#include "xwindow.private.h"
#include <sys/shm.h>

static void xwindow_memory_free_func(xwindow_memory_s *restrict r)
{
	if (r->addr)
		free(r->addr);
}

xwindow_memory_s* xwindow_memory_alloc(uintptr_t size)
{
	xwindow_memory_s *restrict r;
	if (size)
	{
		r = (xwindow_memory_s *) refer_alloz(sizeof(xwindow_memory_s));
		if (r)
		{
			refer_set_free(r, (refer_free_f) xwindow_memory_free_func);
			if ((r->addr = malloc(r->size = size)))
				return r;
			refer_free(r);
		}
	}
	return NULL;
}

static void xwindow_shm_free_func(xwindow_shm_s *restrict r)
{
	if (r->attached)
		shmdt(r->addr);
	if (~r->shmid)
		shmctl(r->shmid, IPC_RMID, NULL);
}

xwindow_shm_s* xwindow_shm_alloc(uintptr_t size)
{
	xwindow_shm_s *restrict r;
	uint32_t shmid;
	if ((size = (size + 4095) & ~4095))
	{
		r = (xwindow_shm_s *) refer_alloz(sizeof(xwindow_shm_s));
		if (r)
		{
			refer_set_free(r, (refer_free_f) xwindow_shm_free_func);
			r->addr = (void *) ~(intptr_t) 0;
			r->size = size;
			shmid = (uint32_t) shmget(IPC_PRIVATE, size, IPC_CREAT | 0600);
			if (~shmid)
			{
				r->shmid = shmid;
				if (~(uintptr_t) (r->addr = shmat(shmid, NULL, 0)))
				{
					r->attached = 1;
					return r;
				}
			}
			return r;
			refer_free(r);
		}
	}
	return NULL;
}

void xwindow_shm_remove(xwindow_shm_s *restrict r)
{
	if (~r->shmid)
	{
		shmctl(r->shmid, IPC_RMID, NULL);
		r->shmid = ~0;
	}
}
