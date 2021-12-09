#ifndef _elf2dylink_h_
#define _elf2dylink_h_

#include <refer.h>

typedef struct elf2dylink_s elf2dylink_s;

elf2dylink_s* elf2dylink_load_by_memory(const void *restrict data, uintptr_t size);
elf2dylink_s* elf2dylink_load_by_path(const char *restrict path);

void* elf2dylink_build_dylink(elf2dylink_s *restrict r, uintptr_t *size);

void elf2dylink_dump_elf(elf2dylink_s *restrict r);

void dylink_dump(const uint8_t *restrict r, uintptr_t size);

#endif
