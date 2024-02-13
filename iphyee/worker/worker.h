#ifndef _iphyee_worker_pri_h_
#define _iphyee_worker_pri_h_

#include "../iphyee.h"
#include "../iphyee.worker.h"
#include <exbuffer.h>
#include <vattr.h>
#include <yaw.h>

// worker

typedef struct iphyee_worker_syncpool_s iphyee_worker_syncpool_s;

struct iphyee_worker_syncpool_s {
	yaw_lock_s *read;
	yaw_lock_s *write;
	vattr_s *semaphore;
	vattr_s *fence;
};

struct iphyee_worker_s {
	iphyee_worker_device_s *device;
	iphyee_worker_memory_heap_s *mheap_host;
	iphyee_worker_memory_heap_s *mheap_device;
	iphyee_worker_syncpool_s *syncpool;
};

struct iphyee_worker_submit_s {
	iphyee_worker_s *worker;
	iphyee_worker_queue_s *queue;
	yaw_lock_s *mutex_submit;
	vattr_s *record_pool;
	exbuffer_t submit_info_buffer;
};

struct iphyee_worker_record_s {
	iphyee_worker_s *worker;
	iphyee_worker_queue_s *queue;
	iphyee_worker_command_pool_s *cmd_pool;
	yaw_lock_s *mutex_record;
	iphyee_worker_command_buffer_s **cmd_array;
	iphyee_worker_semaphore_s **wait_array;
	iphyee_worker_semaphore_s **signal_array;
	uintptr_t cmd_count;
	uintptr_t wait_max_count;
	uintptr_t wait_cur_count;
	uintptr_t signal_max_count;
	uintptr_t signal_cur_count;
};

iphyee_worker_syncpool_s* iphyee_worker_syncpool_alloc(void);
iphyee_worker_syncpool_s* iphyee_worker_syncpool_insert_semaphore(iphyee_worker_syncpool_s *restrict r, iphyee_worker_device_s *restrict device, const char *restrict name, uintptr_t count);
iphyee_worker_syncpool_s* iphyee_worker_syncpool_insert_fence(iphyee_worker_syncpool_s *restrict r, iphyee_worker_device_s *restrict device, const char *restrict name, uintptr_t count);
iphyee_worker_semaphore_s* iphyee_worker_syncpool_save_semaphore(iphyee_worker_syncpool_s *restrict r, const char *restrict name, uintptr_t index);
iphyee_worker_fence_s* iphyee_worker_syncpool_save_fence(iphyee_worker_syncpool_s *restrict r, const char *restrict name, uintptr_t index);

iphyee_worker_s* iphyee_worker_alloc(iphyee_worker_instance_s *restrict instance, uintptr_t index, uint32_t compute_submit_count, uint32_t transfer_submit_count);

iphyee_worker_record_s* iphyee_worker_record_alloc(iphyee_worker_submit_s *restrict submit, uintptr_t cmd_count, uintptr_t wait_max_count, uintptr_t signal_max_count);

#endif
