#ifndef _iphyee_h_
#define _iphyee_h_

#include <refer.h>
#include <graph/graph.h>
#include <graph/device.h>

typedef struct iphyee_s iphyee_s;
typedef struct iphyee_bonex_s iphyee_bonex_s;
typedef struct iphyee_model_s iphyee_model_s;

typedef struct iphyee_param_bonex_t iphyee_param_bonex_t;

// iphyee

iphyee_s* iphyee_alloc(void);
const iphyee_bonex_s* iphyee_create_bonex(iphyee_s *restrict r, const iphyee_param_bonex_t *restrict param);

// bonex

// model

#endif
