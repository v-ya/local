#ifndef _iphyee_worker_h_
#define _iphyee_worker_h_

#include "iphyee.h"

typedef struct iphyee_worker_submit_s iphyee_worker_submit_s;
typedef struct iphyee_worker_record_s iphyee_worker_record_s;
typedef struct iphyee_worker_instance_s iphyee_worker_instance_s;
typedef struct iphyee_worker_physical_device_s iphyee_worker_physical_device_s;
typedef struct iphyee_worker_device_s iphyee_worker_device_s;
typedef struct iphyee_worker_queue_s iphyee_worker_queue_s;
typedef struct iphyee_worker_fence_s iphyee_worker_fence_s;
typedef struct iphyee_worker_semaphore_s iphyee_worker_semaphore_s;
typedef struct iphyee_worker_memory_heap_s iphyee_worker_memory_heap_s;
typedef struct iphyee_worker_buffer_s iphyee_worker_buffer_s;
typedef struct iphyee_worker_shader_s iphyee_worker_shader_s;
typedef struct iphyee_worker_command_pool_s iphyee_worker_command_pool_s;
typedef struct iphyee_worker_command_buffer_s iphyee_worker_command_buffer_s;

typedef enum iphyee_worker_buffer_usage_t iphyee_worker_buffer_usage_t;

enum iphyee_worker_buffer_usage_t {
	iphyee_worker_buffer_usage__src = 0x01,
	iphyee_worker_buffer_usage__dst = 0x02,
	iphyee_worker_buffer_usage__fix = 0x03
};

// worker

iphyee_worker_s* iphyee_worker_insert_semaphore(iphyee_worker_s *restrict r, const char *restrict semaphore_name, uintptr_t semaphore_count);
iphyee_worker_s* iphyee_worker_insert_fence(iphyee_worker_s *restrict r, const char *restrict fence_name, uintptr_t fence_count);
iphyee_worker_shader_s* iphyee_worker_create_shader(iphyee_worker_s *restrict r, const void *restrict code, uintptr_t size, const char *restrict entry_name, uint32_t push_constants_size);
iphyee_worker_buffer_s* iphyee_worker_create_buffer_host(iphyee_worker_s *restrict r, uint64_t size, iphyee_worker_buffer_usage_t usage);
iphyee_worker_buffer_s* iphyee_worker_create_buffer_device(iphyee_worker_s *restrict r, uint64_t size, iphyee_worker_buffer_usage_t usage);
iphyee_worker_s* iphyee_worker_wait_fence(iphyee_worker_s *restrict r, const char *restrict fence_name, uintptr_t fence_index, uint64_t timeout_nsec);
iphyee_worker_s* iphyee_worker_wait_idle(iphyee_worker_s *restrict r);

// submit

iphyee_worker_submit_s* iphyee_worker_submit_alloc_compute(iphyee_worker_s *restrict r);
iphyee_worker_submit_s* iphyee_worker_submit_alloc_transfer(iphyee_worker_s *restrict r);
iphyee_worker_submit_s* iphyee_worker_submit_wait_idle(iphyee_worker_submit_s *restrict r);
iphyee_worker_submit_s* iphyee_worker_submit_group(iphyee_worker_submit_s *restrict r, const char *restrict group_name, const char *restrict fence_name, uintptr_t fence_index);
iphyee_worker_record_s* iphyee_worker_submit_create_record(iphyee_worker_submit_s *restrict r, const char *restrict group_name, uintptr_t cmd_count, uintptr_t wait_max_count, uintptr_t signal_max_count);

// record

iphyee_worker_record_s* iphyee_worker_record_append_wait(iphyee_worker_record_s *restrict r, const char *restrict semaphore_name, uintptr_t semaphore_index, uint32_t stage_compute, uint32_t stage_transfer);
iphyee_worker_record_s* iphyee_worker_record_append_signal(iphyee_worker_record_s *restrict r, const char *restrict semaphore_name, uintptr_t semaphore_index);
void iphyee_worker_record_begin(iphyee_worker_record_s *restrict r);
void iphyee_worker_record_end(iphyee_worker_record_s *restrict r);
iphyee_worker_command_buffer_s* iphyee_worker_record_index(iphyee_worker_record_s *restrict r, uintptr_t index);

// instance

iphyee_worker_instance_s* iphyee_worker_instance_alloc(const char *restrict application_name, uint32_t application_version, const char *restrict engine_name, uint32_t engine_version, struct mlog_s *restrict mlog, iphyee_worker_debug_t level);
iphyee_worker_instance_s* iphyee_worker_instance_enumerate_device(iphyee_worker_instance_s *restrict r);
uintptr_t iphyee_worker_instance_count_device(const iphyee_worker_instance_s *restrict r);
iphyee_worker_physical_device_s* iphyee_worker_instance_index_device(const iphyee_worker_instance_s *restrict r, uintptr_t index);
iphyee_worker_device_s* iphyee_worker_instance_create_device(iphyee_worker_instance_s *restrict r, uintptr_t index, uint32_t compute_queue_count, uint32_t transfer_queue_count);

// device

void iphyee_worker_device_get_queue_limit(const iphyee_worker_physical_device_s *restrict physical_device, uint32_t *restrict compute_max_count, uint32_t *restrict transfer_max_count, uint32_t *restrict sum_count_limit);
iphyee_worker_device_s* iphyee_worker_device_alloc(const iphyee_worker_instance_s *restrict instance, const iphyee_worker_physical_device_s *restrict physical_device, uint32_t compute_queue_count, uint32_t transfer_queue_count);
iphyee_worker_device_s* iphyee_worker_device_wait_idle(iphyee_worker_device_s *restrict r);

// queue

iphyee_worker_queue_s* iphyee_worker_queue_create_compute(iphyee_worker_device_s *restrict device);
iphyee_worker_queue_s* iphyee_worker_queue_create_transfer(iphyee_worker_device_s *restrict device);
iphyee_worker_queue_s* iphyee_worker_queue_wait_idle(iphyee_worker_queue_s *restrict r);

// fence

iphyee_worker_fence_s* iphyee_worker_fence_alloc(iphyee_worker_device_s *restrict device, uint32_t signaled);
iphyee_worker_fence_s* iphyee_worker_fence_reset(iphyee_worker_fence_s *restrict fence);
iphyee_worker_fence_s* iphyee_worker_fence_is_signaled(iphyee_worker_fence_s *restrict fence);
iphyee_worker_fence_s* iphyee_worker_fence_wait(iphyee_worker_fence_s *restrict fence, uint64_t timeout_nesc);

// semaphore

iphyee_worker_semaphore_s* iphyee_worker_semaphore_alloc(iphyee_worker_device_s *restrict device, uint32_t stage_compute, uint32_t stage_transfer);

// buffer

iphyee_worker_buffer_s* iphyee_worker_buffer_create_storage(iphyee_worker_memory_heap_s *restrict memory_heap, uint64_t size, iphyee_worker_buffer_usage_t usage);
uint64_t iphyee_worker_buffer_device_address(iphyee_worker_buffer_s *restrict buffer);
void* iphyee_worker_buffer_map(iphyee_worker_buffer_s *restrict buffer, uint64_t offset, uint64_t size);
void iphyee_worker_buffer_unmap(iphyee_worker_buffer_s *restrict buffer);
iphyee_worker_buffer_s* iphyee_worker_buffer_flush(iphyee_worker_buffer_s *restrict buffer, uint64_t offset, uint64_t size);
iphyee_worker_buffer_s* iphyee_worker_buffer_invalidate(iphyee_worker_buffer_s *restrict buffer, uint64_t offset, uint64_t size);

// shader

iphyee_worker_shader_s* iphyee_worker_shader_alloc(iphyee_worker_device_s *restrict device, const void *restrict code, uintptr_t size, const char *restrict entry_name, uint32_t push_constants_size);
const iphyee_worker_shader_s* iphyee_worker_shader_binary(const iphyee_worker_shader_s *restrict r, uintptr_t *restrict binary_size, void *restrict binary_data);

// command_pool

iphyee_worker_command_pool_s* iphyee_worker_command_pool_alloc(iphyee_worker_queue_s *restrict queue, uint32_t short_live);

// command_buffer

iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_create_primary(iphyee_worker_command_pool_s *restrict command_pool);
iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_create_secondary(iphyee_worker_command_pool_s *restrict command_pool);
iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_reset(iphyee_worker_command_buffer_s *restrict r);
iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_begin(iphyee_worker_command_buffer_s *restrict r, uint32_t keep);
iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_end(iphyee_worker_command_buffer_s *restrict r);
iphyee_worker_command_buffer_s* iphyee_worker_command_buffer_submit(iphyee_worker_command_buffer_s *restrict r, iphyee_worker_fence_s *restrict fence, uintptr_t wait_count, iphyee_worker_semaphore_s **restrict wait_array, uintptr_t signal_count, iphyee_worker_semaphore_s **restrict signal_array);

// cmd

void iphyee_worker_cmd_copy_buffer(iphyee_worker_command_buffer_s *restrict command_buffer, const iphyee_worker_buffer_s *restrict src, iphyee_worker_buffer_s *restrict dst, uint64_t src_offset, uint64_t dst_offset, uint64_t copy_size);
void iphyee_worker_cmd_bind_shader(iphyee_worker_command_buffer_s *restrict command_buffer, const iphyee_worker_shader_s *restrict shader);
void iphyee_worker_cmd_push_constants(iphyee_worker_command_buffer_s *restrict command_buffer, const iphyee_worker_shader_s *restrict shader, uint32_t offset, uint32_t size, const void *restrict value);
void iphyee_worker_cmd_dispatch_base(iphyee_worker_command_buffer_s *restrict command_buffer, uint32_t group_base_x, uint32_t group_base_y, uint32_t group_base_z, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);
void iphyee_worker_cmd_dispatch(iphyee_worker_command_buffer_s *restrict command_buffer, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z);

#endif
