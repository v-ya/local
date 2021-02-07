#ifndef _queue_h_
#define _queue_h_

#include <refer.h>

typedef struct queue_s queue_s;

typedef queue_s* (*queue_push_f)(queue_s *q, refer_t v);
typedef refer_t (*queue_pull_f)(queue_s *q);

struct queue_s {
	queue_push_f push;
	queue_pull_f pull;
};

queue_s* queue_alloc_ring(size_t size);

#endif
