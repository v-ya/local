#ifndef _av1_env_h_
#define _av1_env_h_

#include <stdint.h>

struct av1_obu_sequence_header_t;
struct av1_ref_t;

typedef struct av1_env_t {
	struct av1_obu_sequence_header_t *restrict sequence_header;
	struct av1_ref_t *restrict ref;
	uint32_t SeenFrameHeader;
} av1_env_t;

av1_env_t* av1_env_alloc(void);
void av1_env_free(av1_env_t *restrict env);

#endif
