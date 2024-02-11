#ifndef _iphyee_worker_pri_h_
#define _iphyee_worker_pri_h_

#include "../iphyee.pri.h"
#include <mlog.h>

typedef struct iphyee_worker_enumerate_s iphyee_worker_enumerate_s;
typedef struct iphyee_worker_instance_s iphyee_worker_instance_s;
typedef struct iphyee_worker_physical_device_s iphyee_worker_physical_device_s;
typedef struct iphyee_worker_device_s iphyee_worker_device_s;
typedef struct iphyee_worker_memory_heap_s iphyee_worker_memory_heap_s;
typedef struct iphyee_worker_buffer_s iphyee_worker_buffer_s;

// instance

iphyee_worker_instance_s* iphyee_worker_instance_alloc(const char *restrict application_name, uint32_t app_version, const char *restrict engine_name, uint32_t engine_version, struct mlog_s *restrict mlog, iphyee_worker_debug_t level);
iphyee_worker_instance_s* iphyee_worker_instance_enumerate_device(iphyee_worker_instance_s *restrict r);
uintptr_t iphyee_worker_instance_count_device(const iphyee_worker_instance_s *restrict r);
iphyee_worker_physical_device_s* iphyee_worker_instance_index_device(const iphyee_worker_instance_s *restrict r, uintptr_t index);
iphyee_worker_device_s* iphyee_worker_instance_create_device(iphyee_worker_instance_s *restrict r, uintptr_t index);

// device

iphyee_worker_device_s* iphyee_worker_device_wait_idle(iphyee_worker_device_s *restrict r);

#endif
