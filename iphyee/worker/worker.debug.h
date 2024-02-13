#ifndef _iphyee_worker_debug_h_
#define _iphyee_worker_debug_h_

#include "worker.inner.h"
#include <mlog.h>

typedef struct iphyee_worker_debug_utils_messenger_s iphyee_worker_debug_utils_messenger_s;

iphyee_worker_debug_utils_messenger_s* iphyee_worker_debug_utils_messenger_alloc(VkInstance instance, mlog_s *ml, VkDebugUtilsMessageSeverityFlagsEXT level, VkDebugUtilsMessageTypeFlagsEXT type);

#endif
