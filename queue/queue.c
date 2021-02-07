#include "queue.h"
#include "queue_ring.h"

queue_s* queue_alloc_ring(size_t size)
{
	return (queue_s *) queue_ring_alloc(size);
}
