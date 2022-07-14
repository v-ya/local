#include "mtask.inner.h"

queue_s* mtask_inner_queue_must_push(queue_s *restrict q, refer_t v, yaw_signal_s *restrict s, const volatile uintptr_t *running)
{
	uint32_t status;
	if (q->push(q, v))
		return q;
	for (;;)
	{
		status = yaw_signal_get(s);
		if (!*running)
			break;
		if (q->push(q, v))
			return q;
		yaw_signal_wait(s, status);
	}
	return NULL;
}
