#ifndef _dylink_h_
#define _dylink_h_

#include <stdint.h>
#include <sys/types.h>

#define dylink_mechine_x86_64  "dyl.X86_64"

typedef struct dylink_header_s {
	char machine[16];
	uint32_t version;
	uint32_t header_size;
	uint64_t img_takeup;
	uint32_t img_offset;
	uint32_t img_size;
	uint32_t strpool_offset;
	uint32_t strpool_size;
	uint32_t isym_offset;
	uint32_t isym_number;
	uint32_t esym_offset;
	uint32_t esym_number;
} dylink_header_s;

typedef struct dylink_isym_s {
	uint32_t name_offset;
	uint32_t type;
	uint64_t offset;
	int64_t addend;
} dylink_isym_s;

typedef struct dylink_esym_s {
	uint32_t name_offset;
	uint32_t offset;
} dylink_esym_s;

typedef int (*dylink_set_f)(uint32_t type, void *dst, int64_t addend, void *ptr, void *plt);
typedef void* (*dylink_import_f)(void *pri, const char *symbol, void **plt);
typedef int (*dylink_export_f)(void *pri, const char *symbol, void *ptr, void **plt);

size_t dylink_check(uint8_t *r, size_t size, const char *machine);
int dylink_link(uint8_t *d, uint8_t *r, dylink_set_f dylink_set, dylink_import_f import_func, dylink_export_f export_func, void *pri);

void* m_x86_64_plt_set(void *dst, void *func);
int m_x86_64_dylink_set(uint32_t type, void *dst, int64_t addend, void *ptr, void *plt);

#endif
