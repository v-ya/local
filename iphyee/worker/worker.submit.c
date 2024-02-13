#include "worker.inner.h"

static void iphyee_worker_submit_free_func(iphyee_worker_submit_s *restrict r)
{
	if (r->record_pool) refer_free(r->record_pool);
	if (r->mutex_submit) refer_free(r->mutex_submit);
	if (r->queue) refer_free(r->queue);
	if (r->worker) refer_free(r->worker);
	exbuffer_uini(&r->submit_info_buffer);
}

static iphyee_worker_submit_s* iphyee_worker_submit_alloc(iphyee_worker_s *restrict worker, iphyee_worker_queue_s* (*qfunc)(iphyee_worker_device_s *restrict device))
{
	iphyee_worker_submit_s *restrict r;
	if ((r = (iphyee_worker_submit_s *) refer_alloz(sizeof(iphyee_worker_submit_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_submit_free_func);
		r->worker = (iphyee_worker_s *) refer_save(worker);
		if (exbuffer_init(&r->submit_info_buffer, 0) &&
			(r->mutex_submit = yaw_lock_alloc_mutex()) &&
			(r->record_pool = vattr_alloc()) &&
			(r->queue = qfunc(worker->device)))
			return r;
		refer_free(r);
	}
	return NULL;
}

iphyee_worker_submit_s* iphyee_worker_submit_alloc_compute(iphyee_worker_s *restrict r)
{
	return iphyee_worker_submit_alloc(r, iphyee_worker_queue_create_compute);
}

iphyee_worker_submit_s* iphyee_worker_submit_alloc_transfer(iphyee_worker_s *restrict r)
{
	return iphyee_worker_submit_alloc(r, iphyee_worker_queue_create_transfer);
}

iphyee_worker_submit_s* iphyee_worker_submit_wait_idle(iphyee_worker_submit_s *restrict r)
{
	if (iphyee_worker_queue_wait_idle(r->queue))
		return r;
	return NULL;
}

iphyee_worker_record_s* iphyee_worker_submit_create_record(iphyee_worker_submit_s *restrict r, const char *restrict group_name, uintptr_t cmd_count, uintptr_t wait_max_count, uintptr_t signal_max_count)
{
	iphyee_worker_record_s *restrict record;
	iphyee_worker_record_s *result;
	yaw_lock_s *restrict lock;
	result = NULL;
	if (group_name && cmd_count)
	{
		if ((record = iphyee_worker_record_alloc(r, cmd_count, wait_max_count, signal_max_count)))
		{
			lock = r->mutex_submit;
			yaw_lock_lock(lock);
			if (vattr_insert_tail(r->record_pool, group_name, record))
				result = record, record = NULL;
			yaw_lock_unlock(lock);
			if (record) refer_free(record);
		}
	}
	return result;
}

iphyee_worker_submit_s* iphyee_worker_submit_group(iphyee_worker_submit_s *restrict r, const char *restrict group_name, const char *restrict fence_name, uintptr_t fence_index)
{
	iphyee_worker_submit_s *result;
	iphyee_worker_fence_s *restrict fence;
	iphyee_worker_record_s *restrict p;
	vattr_vslot_t *restrict vslot;
	vattr_vlist_t *restrict vl;
	yaw_lock_s *restrict lock;
	VkSubmitInfo *si;
	uintptr_t count;
	result = NULL;
	if (group_name)
	{
		if ((fence = iphyee_worker_syncpool_save_fence(r->worker->syncpool, fence_name, fence_index)))
			iphyee_worker_fence_reset(fence);
		lock = r->mutex_submit;
		yaw_lock_lock(lock);
		if ((vslot = vattr_get_vslot(r->record_pool, group_name)) &&
			(si = (VkSubmitInfo *) exbuffer_need(&r->submit_info_buffer,
				sizeof(VkSubmitInfo) * vslot->number)))
		{
			count = 0;
			for (vl = vslot->vslot; vl; vl = vl->vslot_next)
			{
				p = (iphyee_worker_record_s *) vl->value;
				yaw_lock_lock(p->mutex_record);
			}
			for (vl = vslot->vslot; vl; vl = vl->vslot_next)
			{
				p = (iphyee_worker_record_s *) vl->value;
				if (iphyee_worker_record_inner_set_submit(p, si + count))
					count += 1;
			}
			if (!vkQueueSubmit(r->queue->queue, count, si, fence?fence->fence:NULL))
				result = r;
			for (vl = vslot->vslot; vl; vl = vl->vslot_next)
			{
				p = (iphyee_worker_record_s *) vl->value;
				yaw_lock_unlock(p->mutex_record);
			}
		}
		yaw_lock_unlock(lock);
		if (fence) refer_free(fence);
	}
	return result;
}
