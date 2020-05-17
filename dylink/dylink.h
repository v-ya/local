#ifndef _dylink_h_
#define _dylink_h_

#include <stdint.h>
#include <sys/types.h>

#define dylink_mechine_x86_64  "dyl.X86_64"

typedef struct dylink_header_t {
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
} dylink_header_t;

typedef struct dylink_isym_t {
	uint32_t name_offset;
	uint32_t type;
	uint64_t offset;
	int64_t addend;
} dylink_isym_t;

typedef struct dylink_esym_t {
	uint32_t name_offset;
	uint32_t offset;
} dylink_esym_t;

typedef void* (*dylink_plt_set_f)(void *dst, void *func, void ***plt);
typedef int (*dylink_set_f)(uint32_t type, void *dst, int64_t addend, void *ptr, void **plt);
typedef void* (*dylink_import_f)(void *pri, const char *symbol, void ***plt);
typedef int (*dylink_export_f)(void *pri, const char *symbol, void *ptr, void **plt);

size_t dylink_check(uint8_t *r, size_t size, const char *machine);
int dylink_link(uint8_t *d, uint8_t *r, dylink_set_f dylink_set, dylink_import_f import_func, dylink_export_f export_func, void *pri);

void* m_x86_64_dylink_plt_set(void *dst, void *func, void ***plt);
int m_x86_64_dylink_set(uint32_t type, void *dst, int64_t addend, void *ptr, void **plt);

typedef enum dylink_pool_report_type_t {
	dylink_pool_report_type_import_ok,
	dylink_pool_report_type_import_fail,
	dylink_pool_report_type_export_ok,
	dylink_pool_report_type_export_fail,
	dylink_pool_report_type_set_symbol,
	dylink_pool_report_type_delete_symbol
} dylink_pool_report_type_t;
typedef int (*dylink_pool_report_f)(void *pri, dylink_pool_report_type_t type, const char *symbol, void *ptr, void **plt);
typedef struct dylink_pool_t dylink_pool_t;

dylink_pool_t* dylink_pool_alloc(const char *mechine, dylink_set_f dylink_set, dylink_plt_set_f dylink_plt_set, size_t xmem_size);
void dylink_pool_free(dylink_pool_t *dp);
void dylink_pool_set_report(dylink_pool_t *dp, dylink_pool_report_f func, void *pri);
int dylink_pool_set_func(dylink_pool_t *dp, const char *symbol, void *func);
void* dylink_pool_get_symbol(dylink_pool_t *dp, const char *symbol, void ***plt);
void dylink_pool_delete_symbol(dylink_pool_t *dp, const char *symbol);
int dylink_pool_load(dylink_pool_t *dp, uint8_t *dylink_data, size_t dylink_size);
int dylink_pool_load_file(dylink_pool_t *dp, const char *path);

#endif
