#ifndef _queue_ring_h_
#define _queue_ring_h_

#include <refer.h>

typedef struct queue_ring_s queue_ring_s;

queue_ring_s* queue_ring_alloc(size_t size);
queue_ring_s* queue_ring_push(queue_ring_s *q, refer_t v);
refer_t queue_ring_pull(queue_ring_s *q);

#endif
