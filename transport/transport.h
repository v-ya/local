#ifndef _transport_h_
#define _transport_h_

#include <refer.h>

typedef struct transport_s transport_s;
typedef struct transport_poll_s transport_poll_s;
typedef struct transport_attr_t transport_attr_t;
typedef enum transport_attr_flag_t transport_attr_flag_t;
typedef enum transport_poll_status_t transport_poll_status_t;
typedef enum transport_poll_flag_t transport_poll_flag_t;

enum transport_attr_flag_t {
	transport_attr_flag_do_some        = 0x01,
	transport_attr_flag_do_full        = 0x02,
	transport_attr_flag_modify_timeout = 0x10,
};

enum transport_poll_status_t {
	transport_poll_status_continue,
	transport_poll_status_okay
};

enum transport_poll_flag_t {
	transport_poll_flag_read  = 0x01,
	transport_poll_flag_write = 0x02,
	transport_poll_flag_edge  = 0x10,
};

struct transport_attr_t {
	const volatile uintptr_t *running;
	uint64_t timeout_ms;
	transport_attr_flag_t flags;
};

typedef transport_s* (*transport_send_f)(transport_s *restrict r, const void *data, uintptr_t n, uintptr_t *restrict rn);
typedef transport_s* (*transport_recv_f)(transport_s *restrict r, void *data, uintptr_t n, uintptr_t *restrict rn);
typedef transport_poll_status_t (*transport_poll_do_f)(transport_s *restrict tp, refer_t pri);

void transport_initial(transport_s *restrict r, const char *type, transport_s *layer, transport_send_f send, transport_recv_f recv);
void transport_final(transport_s *restrict r);

transport_s* transport_send(transport_s *restrict r, const void *data, uintptr_t n, uintptr_t *restrict rn, transport_attr_t *restrict attr);
transport_s* transport_recv(transport_s *restrict r, void *data, uintptr_t n, uintptr_t *restrict rn, transport_attr_t *restrict attr);

transport_s* transport_push_recv(transport_s *restrict r, const void *data, uintptr_t n);
void transport_discard_cache(transport_s *restrict r);

// timestamp

uint64_t transport_timestamp_sec(void);
uint64_t transport_timestamp_ms(void);
uint64_t transport_timestamp_us(void);

// tpoll

transport_poll_s* transport_poll_alloc(uintptr_t size);
void transport_poll_clear_get(transport_poll_s *restrict tpoll);
transport_poll_s* transport_poll_insert(transport_poll_s *restrict tpoll, transport_s *restrict tp, transport_poll_do_f func, refer_t pri, uint64_t timestamp_kill, transport_poll_flag_t flags);
refer_t* transport_poll_get(transport_poll_s *restrict tpoll, uintptr_t *restrict number, uint64_t timeout_ms);
uintptr_t transport_poll_number(transport_poll_s *restrict tpoll);

#endif
