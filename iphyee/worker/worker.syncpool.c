#include "worker.inner.h"

static void iphyee_worker_syncpool_free_func(iphyee_worker_syncpool_s *restrict r)
{
	if (r->read) refer_free(r->read);
	if (r->write) refer_free(r->write);
	if (r->semaphore) refer_free(r->semaphore);
	if (r->fence) refer_free(r->fence);
}

iphyee_worker_syncpool_s* iphyee_worker_syncpool_alloc(void)
{
	iphyee_worker_syncpool_s *restrict r;
	if ((r = (iphyee_worker_syncpool_s *) refer_alloz(sizeof(iphyee_worker_syncpool_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_syncpool_free_func);
		if (!yaw_lock_alloc_rwlock(&r->read, &r->write) &&
			(r->semaphore = vattr_alloc()) &&
			(r->fence = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

iphyee_worker_syncpool_s* iphyee_worker_syncpool_insert_semaphore(iphyee_worker_syncpool_s *restrict r, iphyee_worker_device_s *restrict device, const char *restrict name, uintptr_t count)
{
	iphyee_worker_syncpool_s *result;
	iphyee_worker_semaphore_s *restrict v;
	yaw_lock_s *restrict lock;
	vattr_vlist_t *vl;
	uintptr_t i;
	result = NULL;
	if (name)
	{
		lock = r->write;
		yaw_lock_lock(lock);
		for (i = 0; i < count; ++i)
		{
			vl = NULL;
			if ((v = iphyee_worker_semaphore_alloc(device, 0, 0)))
			{
				vl = vattr_insert_tail(r->semaphore, name, v);
				refer_free(v);
			}
			if (!vl) goto label_fail;
		}
		result = r;
		label_fail:
		yaw_lock_unlock(lock);
		return r;
	}
	return result;
}

iphyee_worker_syncpool_s* iphyee_worker_syncpool_insert_fence(iphyee_worker_syncpool_s *restrict r, iphyee_worker_device_s *restrict device, const char *restrict name, uintptr_t count)
{
	iphyee_worker_syncpool_s *result;
	iphyee_worker_fence_s *restrict v;
	yaw_lock_s *restrict lock;
	vattr_vlist_t *vl;
	uintptr_t i;
	result = NULL;
	if (name)
	{
		lock = r->write;
		yaw_lock_lock(lock);
		for (i = 0; i < count; ++i)
		{
			vl = NULL;
			if ((v = iphyee_worker_fence_alloc(device, 0)))
			{
				vl = vattr_insert_tail(r->fence, name, v);
				refer_free(v);
			}
			if (!vl) goto label_fail;
		}
		result = r;
		label_fail:
		yaw_lock_unlock(lock);
		return r;
	}
	return result;
}

iphyee_worker_semaphore_s* iphyee_worker_syncpool_save_semaphore(iphyee_worker_syncpool_s *restrict r, const char *restrict name, uintptr_t index)
{
	iphyee_worker_semaphore_s *restrict v;
	yaw_lock_s *restrict lock;
	if (name)
	{
		lock = r->read;
		yaw_lock_lock(lock);
		v = (iphyee_worker_semaphore_s *) refer_save(vattr_get_index(r->semaphore, name, index));
		yaw_lock_unlock(lock);
		return v;
	}
	return NULL;
}

iphyee_worker_fence_s* iphyee_worker_syncpool_save_fence(iphyee_worker_syncpool_s *restrict r, const char *restrict name, uintptr_t index)
{
	iphyee_worker_fence_s *restrict v;
	yaw_lock_s *restrict lock;
	if (name)
	{
		lock = r->read;
		yaw_lock_lock(lock);
		v = (iphyee_worker_fence_s *) refer_save(vattr_get_index(r->fence, name, index));
		yaw_lock_unlock(lock);
		return v;
	}
	return NULL;
}
