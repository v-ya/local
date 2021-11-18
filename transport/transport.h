#ifndef _transport_h_
#define _transport_h_

#include <refer.h>

typedef struct transport_s transport_s;
typedef struct transport_cache_t transport_cache_t;
typedef struct transport_attr_t transport_attr_t;
typedef enum transport_attr_flag_t transport_attr_flag_t;

typedef transport_s* (*transport_send_f)(transport_s *restrict r, const void *data, uintptr_t n, uintptr_t *restrict rn);
typedef transport_s* (*transport_recv_f)(transport_s *restrict r, void *data, uintptr_t n, uintptr_t *restrict rn);

struct transport_cache_t {
	uint8_t *data;
	uint8_t *pos;
	uintptr_t size;
	uintptr_t used;
};

struct transport_s {
	const char *type;
	transport_s *layer;
	transport_send_f send;
	transport_recv_f recv;
	transport_cache_t cache;
};

enum transport_attr_flag_t {
	transport_attr_flag_do_some        = 0x01,
	transport_attr_flag_do_full        = 0x02,
	transport_attr_flag_modify_timeout = 0x10,
};

struct transport_attr_t {
	const volatile uintptr_t *running;
	uint64_t timeout_ms;
	transport_attr_flag_t flags;
};

void transport_initial(transport_s *restrict r, const char *type, transport_s *layer, transport_send_f send, transport_recv_f recv);
void transport_final(transport_s *restrict r);

transport_s* transport_send(transport_s *restrict r, const void *data, uintptr_t n, uintptr_t *restrict rn, transport_attr_t *restrict attr);
transport_s* transport_recv(transport_s *restrict r, void *data, uintptr_t n, uintptr_t *restrict rn, transport_attr_t *restrict attr);

transport_s* transport_push_recv(transport_s *restrict r, const void *data, uintptr_t n);
void transport_discard_cache(transport_s *restrict r);

// other

uint64_t transport_timestamp_sec(void);
uint64_t transport_timestamp_ms(void);
uint64_t transport_timestamp_us(void);

#endif
