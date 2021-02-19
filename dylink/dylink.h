#ifndef _dylink_h_
#define _dylink_h_

#include <stdint.h>
#include <stddef.h>
#include <sys/types.h>

#ifdef __GNUC__
#  define dyl_used           __attribute__ ((__used__))
#  define dyl_global(_n)     __asm__ (".globl " #_n)
#  define dyl_alias(_r,_n)   __asm__ (".set " #_n ", " #_r)
#  define dyl_export(_r,_n)  dyl_global(_n);\
				dyl_alias(_r,_n)
#endif

void* xmem_alloc(size_t size);
int xmem_free(void *xmem, size_t size);

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

typedef void* (*dylink_plt_set_f)(void *restrict dst, void *func, void ***restrict plt);
typedef int (*dylink_set_f)(uint32_t type, void *restrict dst, int64_t addend, void *ptr, void **plt);
typedef void* (*dylink_import_f)(void *pri, const char *restrict symbol, void ***restrict plt);
typedef int (*dylink_export_f)(void *pri, const char *restrict symbol, void *ptr, void **plt);

size_t dylink_check(const uint8_t *restrict r, size_t size, const char *restrict machine);
int dylink_link(uint8_t *restrict d, const uint8_t *restrict r, dylink_set_f dylink_set, dylink_import_f import_func, dylink_export_f export_func, void *pri);

void* m_x86_64_dylink_plt_set(void *restrict dst, void *func, void ***restrict plt);
int m_x86_64_dylink_set(uint32_t type, void *restrict dst, int64_t addend, void *ptr, void **plt);

// dylink pool

typedef enum dylink_pool_report_type_t {
	dylink_pool_report_type_import_ok,
	dylink_pool_report_type_import_fail,
	dylink_pool_report_type_export_ok,
	dylink_pool_report_type_export_fail,
	dylink_pool_report_type_set_symbol,
	dylink_pool_report_type_delete_symbol
} dylink_pool_report_type_t;
typedef int (*dylink_pool_report_f)(void *pri, dylink_pool_report_type_t type, const char *restrict symbol, void *ptr, void **plt);
typedef struct dylink_pool_t dylink_pool_t;

dylink_pool_t* dylink_pool_alloc(const char *restrict mechine, dylink_set_f dylink_set, dylink_plt_set_f dylink_plt_set, size_t xmem_size);
void dylink_pool_free(dylink_pool_t *restrict dp);
void dylink_pool_set_report(dylink_pool_t *restrict dp, dylink_pool_report_f func, void *pri);
int dylink_pool_set_func(dylink_pool_t *restrict dp, const char *restrict symbol, void *func);
void* dylink_pool_get_symbol(const dylink_pool_t *restrict dp, const char *restrict symbol, void ***restrict plt);
void dylink_pool_delete_symbol(dylink_pool_t *restrict dp, const char *restrict symbol);
int dylink_pool_load(dylink_pool_t *restrict dp, const uint8_t *restrict dylink_data, size_t dylink_size);
int dylink_pool_load_file(dylink_pool_t *restrict dp, const char *restrict path);

// dylink pool layer

dylink_pool_t* dylink_pool_alloc_local(dylink_pool_t *restrict dp);
dylink_pool_t* dylink_pool_upper(dylink_pool_t *restrict dp);
int dylink_pool_sync_symbol(const dylink_pool_t *restrict dp, const char *restrict symbol, const char *restrict upper_symbol);

#endif
