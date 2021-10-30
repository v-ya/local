#ifndef _tparse_tword_h_
#define _tparse_tword_h_

#include <refer.h>

typedef struct tparse_tword_edge_s tparse_tword_edge_s;

typedef enum tparse_tword_edge_t {
	tparse_tword_edge_head  = 0x01,
	tparse_tword_edge_inner = 0x02,
	tparse_tword_edge_tail  = 0x04
} tparse_tword_edge_t;

struct tparse_tword_edge_s {
	tparse_tword_edge_t edge[256];
};

tparse_tword_edge_s* tparse_tword_edge_alloc(void);

// [r]skip: return pos

uintptr_t tparse_tword_skip(const tparse_tword_edge_s *restrict edge, const void *restrict text, uintptr_t size, uintptr_t pos);
uintptr_t tparse_tword_rskip(const tparse_tword_edge_s *restrict edge, const void *restrict text, uintptr_t size, uintptr_t pos);

#endif
