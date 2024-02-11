#ifndef _iphyee_h_
#define _iphyee_h_

#include <refer.h>

struct mlog_s;

typedef struct iphyee_s iphyee_s;
typedef struct iphyee_bonex_s iphyee_bonex_s;
typedef struct iphyee_model_s iphyee_model_s;

typedef struct iphyee_param_bonex_t iphyee_param_bonex_t;

// iphyee

iphyee_s* iphyee_alloc(void);
const iphyee_bonex_s* iphyee_create_bonex(iphyee_s *restrict r, const iphyee_param_bonex_t *restrict param);

// bonex

// model

// worker

typedef struct iphyee_worker_s iphyee_worker_s;
typedef enum iphyee_worker_debug_t iphyee_worker_debug_t;

enum iphyee_worker_debug_t {
	iphyee_worker_debug__none,
	iphyee_worker_debug__error,
	iphyee_worker_debug__warning,
	iphyee_worker_debug__info,
	iphyee_worker_debug__version,
};

#endif
