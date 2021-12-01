#ifndef _transport_inner_tpoll_h_
#define _transport_inner_tpoll_h_

#include "transport.h"
#include <sys/epoll.h>

typedef struct transport_inner_poll_s transport_inner_poll_s;

transport_inner_poll_s* transport_inner_poll_alloc(uintptr_t size);
transport_inner_poll_s* transport_inner_poll_insert(transport_inner_poll_s *restrict tpoll, transport_s *restrict tp, transport_poll_do_f func, refer_t pri, uint64_t timestamp_kill, uint32_t events);
uintptr_t transport_inner_poll_number(transport_inner_poll_s *restrict tpoll);
uintptr_t transport_inner_poll_get_okay(transport_inner_poll_s *restrict tpoll, uint64_t timeout_ms, refer_t *restrict pri_data, uintptr_t pri_size);
uintptr_t transport_inner_poll_get_timeout(transport_inner_poll_s *restrict tpoll, uint64_t timestamp_kill, refer_t *restrict pri_data, uintptr_t pri_size);

#endif
