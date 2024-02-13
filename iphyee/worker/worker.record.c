#include "worker.inner.h"

typedef struct iphyee_worker_record_impl_s iphyee_worker_record_impl_s;

struct iphyee_worker_record_impl_s {
	iphyee_worker_record_s record;
	VkCommandBuffer *vaild_cmdbuf_array;
	VkSemaphore *wait_semaphore_array;
	VkSemaphore *signal_semaphore_array;
	VkPipelineStageFlags *wait_stage_array;
	uint32_t *cmdbuf_is_vaild_array;
	uintptr_t cmdbuf_vaild_count;
	uintptr_t cmdbuf_ready_okay;
};

#define impl  ((iphyee_worker_record_impl_s *) r)

static void iphyee_worker_record_free_func(iphyee_worker_record_s *restrict r)
{
	uintptr_t i, n;
	for (i = 0, n = r->signal_cur_count; i < n; ++i)
		refer_free(r->signal_array[i]);
	for (i = 0, n = r->wait_cur_count; i < n; ++i)
		refer_free(r->wait_array[i]);
	for (i = 0, n = r->cmd_count; i < n; ++i)
		refer_free(r->cmd_array[i]);
	if (r->mutex_record) refer_free(r->mutex_record);
	if (r->cmd_pool) refer_free(r->cmd_pool);
	if (r->queue) refer_free(r->queue);
	if (r->worker) refer_free(r->worker);
}

static iphyee_worker_record_impl_s* iphyee_worker_record_alloc_empty(uintptr_t cmd_count, uintptr_t wait_max_count, uintptr_t signal_max_count)
{
	iphyee_worker_record_impl_s *restrict r;
	uintptr_t size;
	size = sizeof(iphyee_worker_record_impl_s);
	size += cmd_count * (sizeof(iphyee_worker_command_buffer_s *) + sizeof(VkCommandBuffer) + sizeof(uint32_t));
	size += wait_max_count * (sizeof(iphyee_worker_semaphore_s *) + sizeof(VkSemaphore) + sizeof(VkPipelineStageFlags));
	size += signal_max_count * (sizeof(iphyee_worker_semaphore_s *) + sizeof(VkSemaphore));
	if (cmd_count && (r = (iphyee_worker_record_impl_s *) refer_alloz(size)))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_record_free_func);
		r->record.cmd_array = (iphyee_worker_command_buffer_s **) (r + 1);
		r->record.wait_array = (iphyee_worker_semaphore_s **) (r->record.cmd_array + cmd_count);
		r->record.signal_array = (iphyee_worker_semaphore_s **) (r->record.wait_array + wait_max_count);
		r->vaild_cmdbuf_array = (VkCommandBuffer *) (r->record.signal_array + signal_max_count);
		r->wait_semaphore_array = (VkSemaphore *) (r->vaild_cmdbuf_array + cmd_count);
		r->signal_semaphore_array = (VkSemaphore *) (r->wait_semaphore_array + wait_max_count);
		r->wait_stage_array = (VkPipelineStageFlags *) (r->signal_semaphore_array + signal_max_count);
		r->cmdbuf_is_vaild_array = (uint32_t *) (r->wait_stage_array + wait_max_count);
		r->record.cmd_count = cmd_count;
		r->record.wait_max_count = wait_max_count;
		r->record.signal_max_count = signal_max_count;
		return r;
	}
	return NULL;
}

iphyee_worker_record_s* iphyee_worker_record_alloc(iphyee_worker_submit_s *restrict submit, uintptr_t cmd_count, uintptr_t wait_max_count, uintptr_t signal_max_count)
{
	iphyee_worker_record_impl_s *restrict r;
	uintptr_t i;
	if ((r = iphyee_worker_record_alloc_empty(cmd_count, wait_max_count, signal_max_count)))
	{
		r->record.worker = (iphyee_worker_s *) refer_save(submit->worker);
		r->record.queue = (iphyee_worker_queue_s *) refer_save(submit->queue);
		if ((r->record.mutex_record = yaw_lock_alloc_mutex()) &&
			(r->record.cmd_pool = iphyee_worker_command_pool_alloc(submit->queue, 1)))
		{
			for (i = 0; i < cmd_count; ++i)
			{
				if (!(r->record.cmd_array[i] = iphyee_worker_command_buffer_create_primary(r->record.cmd_pool)))
					goto label_fail;
			}
			return &r->record;
		}
		label_fail:
		refer_free(r);
	}
	return NULL;
}

iphyee_worker_record_s* iphyee_worker_record_append_wait(iphyee_worker_record_s *restrict r, const char *restrict semaphore_name, uintptr_t semaphore_index, uint32_t stage_compute, uint32_t stage_transfer)
{
	iphyee_worker_semaphore_s **restrict wait_array;
	VkSemaphore *restrict wait_semaphore_array;
	VkPipelineStageFlags *restrict wait_stage_array;
	iphyee_worker_semaphore_s *restrict value;
	iphyee_worker_record_s *result;
	yaw_lock_s *restrict lock;
	uintptr_t i;
	result = NULL;
	lock = r->mutex_record;
	yaw_lock_lock(lock);
	if ((i = r->wait_cur_count) < r->wait_max_count &&
		(value = iphyee_worker_syncpool_save_semaphore(
			r->worker->syncpool, semaphore_name, semaphore_index)))
	{
		wait_array = r->wait_array;
		wait_semaphore_array = impl->wait_semaphore_array;
		wait_stage_array = impl->wait_stage_array;
		wait_array[i] = value;
		wait_semaphore_array[i] = value->semaphore;
		wait_stage_array[i] = (stage_compute?VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT:0) |
			(stage_transfer?VK_PIPELINE_STAGE_TRANSFER_BIT:0);
		r->wait_cur_count = i + 1;
		result = r;
	}
	yaw_lock_unlock(lock);
	return result;
}

iphyee_worker_record_s* iphyee_worker_record_append_signal(iphyee_worker_record_s *restrict r, const char *restrict semaphore_name, uintptr_t semaphore_index)
{
	iphyee_worker_semaphore_s **restrict signal_array;
	VkSemaphore *restrict signal_semaphore_array;
	iphyee_worker_semaphore_s *restrict value;
	iphyee_worker_record_s *result;
	yaw_lock_s *restrict lock;
	uintptr_t i;
	result = NULL;
	lock = r->mutex_record;
	yaw_lock_lock(lock);
	if ((i = r->signal_cur_count) < r->signal_max_count &&
		(value = iphyee_worker_syncpool_save_semaphore(
			r->worker->syncpool, semaphore_name, semaphore_index)))
	{
		signal_array = r->signal_array;
		signal_semaphore_array = impl->signal_semaphore_array;
		signal_array[i] = value;
		signal_semaphore_array[i] = value->semaphore;
		r->signal_cur_count = i + 1;
		result = r;
	}
	yaw_lock_unlock(lock);
	return result;
}

void iphyee_worker_record_begin(iphyee_worker_record_s *restrict r)
{
	yaw_lock_s *restrict lock;
	uint32_t *restrict cmdbuf_is_vaild_array;
	iphyee_worker_command_buffer_s **restrict cmd_array;
	uintptr_t i, n;
	lock = r->mutex_record;
	yaw_lock_lock(lock);
	cmd_array = r->cmd_array;
	cmdbuf_is_vaild_array = impl->cmdbuf_is_vaild_array;
	impl->cmdbuf_vaild_count = 0;
	impl->cmdbuf_ready_okay = 0;
	n = r->cmd_count;
	for (i = 0; i < n; ++i)
	{
		if (iphyee_worker_command_buffer_begin(cmd_array[i], 0))
			cmdbuf_is_vaild_array[i] = 1;
		else cmdbuf_is_vaild_array[i] = 0;
	}
	yaw_lock_unlock(lock);
}

void iphyee_worker_record_end(iphyee_worker_record_s *restrict r)
{
	yaw_lock_s *restrict lock;
	uint32_t *restrict cmdbuf_is_vaild_array;
	iphyee_worker_command_buffer_s **restrict cmd_array;
	VkCommandBuffer *restrict vaild_cmdbuf_array;
	iphyee_worker_command_buffer_s *restrict cb;
	uintptr_t cmdbuf_vaild_count;
	uintptr_t i, n;
	lock = r->mutex_record;
	yaw_lock_lock(lock);
	cmd_array = r->cmd_array;
	vaild_cmdbuf_array = impl->vaild_cmdbuf_array;
	cmdbuf_is_vaild_array = impl->cmdbuf_is_vaild_array;
	cmdbuf_vaild_count = 0;
	n = r->cmd_count;
	for (i = 0; i < n; ++i)
	{
		if (cmdbuf_is_vaild_array[i] && iphyee_worker_command_buffer_end(cb = cmd_array[i]))
		{
			vaild_cmdbuf_array[cmdbuf_vaild_count] = cb->command_buffer;
			cmdbuf_vaild_count += 1;
		}
		cmdbuf_is_vaild_array[i] = 0;
	}
	impl->cmdbuf_vaild_count = cmdbuf_vaild_count;
	impl->cmdbuf_ready_okay = 1;
	yaw_lock_unlock(lock);
}

iphyee_worker_command_buffer_s* iphyee_worker_record_index(iphyee_worker_record_s *restrict r, uintptr_t index)
{
	if (index < r->cmd_count && ((iphyee_worker_record_impl_s *) r)->cmdbuf_is_vaild_array[index])
		return r->cmd_array[index];
	return NULL;
}

iphyee_worker_record_s* iphyee_worker_record_inner_set_submit(iphyee_worker_record_s *restrict r, VkSubmitInfo *restrict submit_info)
{
	if (impl->cmdbuf_ready_okay)
	{
		submit_info->sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submit_info->pNext = NULL;
		submit_info->waitSemaphoreCount = r->wait_cur_count;
		submit_info->pWaitSemaphores = impl->wait_semaphore_array;
		submit_info->pWaitDstStageMask = impl->wait_stage_array;
		submit_info->commandBufferCount = impl->cmdbuf_vaild_count;
		submit_info->pCommandBuffers = impl->vaild_cmdbuf_array;
		submit_info->signalSemaphoreCount = r->signal_cur_count;
		submit_info->pSignalSemaphores = impl->signal_semaphore_array;
		impl->cmdbuf_ready_okay = 0;
		return r;
	}
	return NULL;
}
