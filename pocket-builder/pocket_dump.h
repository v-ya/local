#ifndef _pocket_dump_h_
#define _pocket_dump_h_

#include <pocket/pocket.h>

typedef enum dump_bits_t {
	dump_bits_header = 0x01,
	dump_bits_desc   = 0x02,
	dump_bits_system = 0x04,
	dump_bits_user   = 0x08
} dump_bits_t;

void pocket_dump(const pocket_s *restrict pocket, dump_bits_t bits);

#endif
