#include "xwindow.private.h"
#include <sys/shm.h>

static void xwindow_shm_free_func(xwindow_shm_s *restrict r)
{
	if (~(uintptr_t) r->addr)
		shmdt(r->addr);
	if (r->remove)
		shmctl(r->shmid, IPC_RMID, NULL);
}

xwindow_shm_s* xwindow_shm_alloc(uintptr_t size)
{
	xwindow_shm_s *restrict r;
	int shmid;
	if ((size = (size + 4095) & ~4095))
	{
		r = (xwindow_shm_s *) refer_alloz(sizeof(xwindow_shm_s));
		if (r)
		{
			refer_set_free(r, (refer_free_f) xwindow_shm_free_func);
			r->addr = (void *) ~(intptr_t) 0;
			r->size = size;
			shmid = shmget(IPC_PRIVATE, size, IPC_CREAT | 0600);
			if (shmid >= 0)
			{
				r->shmid = (uint32_t) shmid;
				r->remove = 1;
				if (~(uintptr_t) (r->addr = shmat(shmid, NULL, 0)))
				{
					if (!shmctl(shmid, IPC_RMID, NULL))
						r->remove = 0;
					return r;
				}
			}
			return r;
			refer_free(r);
		}
	}
	return NULL;
}
