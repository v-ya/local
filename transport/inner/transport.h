#ifndef _transport_inner_transport_h_
#define _transport_inner_transport_h_

#include "../transport.h"

typedef struct transport_cache_t transport_cache_t;

struct transport_cache_t {
	uint8_t *data;
	uint8_t *pos;
	uintptr_t size;
	uintptr_t used;
};

struct transport_s {
	const char *type;
	struct transport_s *layer;
	transport_send_f send;
	transport_recv_f recv;
	struct transport_cache_t cache;
	void *p_socket;
};

#endif
