#ifndef _iphyee_worker_pri_h_
#define _iphyee_worker_pri_h_

#include "../iphyee.pri.h"
#include <mlog.h>

typedef struct iphyee_worker_enumerate_s iphyee_worker_enumerate_s;
typedef struct iphyee_worker_instance_s iphyee_worker_instance_s;
typedef struct iphyee_worker_physical_device_s iphyee_worker_physical_device_s;
typedef struct iphyee_worker_device_s iphyee_worker_device_s;
typedef struct iphyee_worker_queue_s iphyee_worker_queue_s;
typedef struct iphyee_worker_fence_s iphyee_worker_fence_s;
typedef struct iphyee_worker_semaphore_s iphyee_worker_semaphore_s;
typedef struct iphyee_worker_memory_heap_s iphyee_worker_memory_heap_s;
typedef struct iphyee_worker_buffer_s iphyee_worker_buffer_s;
typedef struct iphyee_worker_setlayout_s iphyee_worker_setlayout_s;
typedef struct iphyee_worker_desc_pool_s iphyee_worker_desc_pool_s;
typedef struct iphyee_worker_shader_s iphyee_worker_shader_s;

// instance

iphyee_worker_instance_s* iphyee_worker_instance_alloc(const char *restrict application_name, uint32_t app_version, const char *restrict engine_name, uint32_t engine_version, struct mlog_s *restrict mlog, iphyee_worker_debug_t level);
iphyee_worker_instance_s* iphyee_worker_instance_enumerate_device(iphyee_worker_instance_s *restrict r);
uintptr_t iphyee_worker_instance_count_device(const iphyee_worker_instance_s *restrict r);
iphyee_worker_physical_device_s* iphyee_worker_instance_index_device(const iphyee_worker_instance_s *restrict r, uintptr_t index);
iphyee_worker_device_s* iphyee_worker_instance_create_device(iphyee_worker_instance_s *restrict r, uintptr_t index);

// device

iphyee_worker_device_s* iphyee_worker_device_alloc(const iphyee_worker_instance_s *restrict instance, const iphyee_worker_physical_device_s *restrict physical_device);
iphyee_worker_device_s* iphyee_worker_device_wait_idle(iphyee_worker_device_s *restrict r);

// queue

iphyee_worker_queue_s* iphyee_worker_queue_create_compute(iphyee_worker_device_s *restrict device);
iphyee_worker_queue_s* iphyee_worker_queue_create_transfer(iphyee_worker_device_s *restrict device);
iphyee_worker_queue_s* iphyee_worker_queue_wait_idle(iphyee_worker_queue_s *restrict r);

// fence

iphyee_worker_fence_s* iphyee_worker_fence_alloc(iphyee_worker_device_s *restrict device, uint32_t signaled);
iphyee_worker_fence_s* iphyee_worker_fence_reset(iphyee_worker_fence_s *restrict fence);
iphyee_worker_fence_s* iphyee_worker_fence_is_signaled(iphyee_worker_fence_s *restrict fence);
iphyee_worker_fence_s* iphyee_worker_fence_wait(iphyee_worker_fence_s *restrict fence, uint64_t timeout);

// semaphore

iphyee_worker_semaphore_s* iphyee_worker_semaphore_alloc(iphyee_worker_device_s *restrict device);
iphyee_worker_semaphore_s* iphyee_worker_semaphore_signal(iphyee_worker_semaphore_s *restrict semaphore, uint64_t value);
iphyee_worker_semaphore_s* iphyee_worker_semaphore_wait(iphyee_worker_semaphore_s *restrict semaphore, uint64_t timeout, uint64_t value);

// buffer

void* iphyee_worker_buffer_map(iphyee_worker_buffer_s *restrict buffer, uint64_t offset, uint64_t size);
void iphyee_worker_buffer_unmap(iphyee_worker_buffer_s *restrict buffer);
iphyee_worker_buffer_s* iphyee_worker_buffer_flush(iphyee_worker_buffer_s *restrict buffer, uint64_t offset, uint64_t size);
iphyee_worker_buffer_s* iphyee_worker_buffer_invalidate(iphyee_worker_buffer_s *restrict buffer, uint64_t offset, uint64_t size);

// shader

iphyee_worker_shader_s* iphyee_worker_shader_alloc(iphyee_worker_device_s *restrict device, const void *restrict code, uintptr_t size, const char *restrict entry_name, iphyee_worker_setlayout_s *restrict setlayout, uint32_t push_constants_size);
iphyee_worker_shader_s* iphyee_worker_shader_binary(iphyee_worker_shader_s *restrict r, uintptr_t *restrict binary_size, void *restrict binary_data);

#endif
