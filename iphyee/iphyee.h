#ifndef _iphyee_h_
#define _iphyee_h_

#include <refer.h>

struct mlog_s;

typedef struct iphyee_s iphyee_s;
typedef struct iphyee_worker_s iphyee_worker_s;
typedef struct iphyee_bonex_s iphyee_bonex_s;
typedef struct iphyee_bonex_s iphyee_bonex_s;
typedef struct iphyee_model_s iphyee_model_s;

typedef enum iphyee_worker_debug_t iphyee_worker_debug_t;
typedef struct iphyee_param_bonex_t iphyee_param_bonex_t;

enum iphyee_worker_debug_t {
	iphyee_worker_debug__none,
	iphyee_worker_debug__error,
	iphyee_worker_debug__warning,
	iphyee_worker_debug__info,
	iphyee_worker_debug__version,
};

// iphyee

iphyee_s* iphyee_alloc(void);
iphyee_s* iphyee_initial_worker_instance(iphyee_s *restrict r, const char *restrict application_name, uint32_t application_version, struct mlog_s *restrict mlog, iphyee_worker_debug_t level);
iphyee_s* iphyee_enumerate_worker_device(iphyee_s *restrict r);
iphyee_s* iphyee_insert_bonex_preset(iphyee_s *restrict r, const iphyee_param_bonex_t *restrict param);

iphyee_worker_s* iphyee_create_worker(iphyee_s *restrict r, uintptr_t physical_device_index, uint32_t compute_submit_count, uint32_t transfer_submit_count);

// bonex

// model

#endif
