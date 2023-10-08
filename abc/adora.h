#ifndef _abc_adora_h_
#define _abc_adora_h_

#include <stdint.h>
#include <refer.h>

typedef enum abc_adora_vflag_t abc_adora_vflag_t;
typedef union abc_adora_value_t abc_adora_value_t;
typedef struct abc_adora_type_t abc_adora_type_t;

typedef struct abc_adora_mcode_s abc_adora_mcode_s;
typedef abc_adora_mcode_s* (*abc_adora_mcode_append_f)(abc_adora_mcode_s *restrict m, const void *restrict data, uintptr_t size);
typedef abc_adora_mcode_s* (*abc_adora_mcode_fillch_f)(abc_adora_mcode_s *restrict m, int32_t ch, uintptr_t count);
typedef uintptr_t (*abc_adora_mcode_offset_f)(const abc_adora_mcode_s *restrict m);
typedef abc_adora_mcode_s* (*abc_adora_mcode_restore_f)(abc_adora_mcode_s *restrict m, uintptr_t offset);
typedef uint8_t* (*abc_adora_mcode_mapping_f)(abc_adora_mcode_s *restrict m, uintptr_t *restrict size);
typedef void (*abc_adora_mcode_clear_f)(abc_adora_mcode_s *restrict m);

enum abc_adora_vflag_t {
	abc_adora_vflag__read  = 0x01,
	abc_adora_vflag__write = 0x02,
	abc_adora_vflag__xcall = 0x04,
};

union abc_adora_value_t {
	uint64_t u64;
	int64_t s64;
	double f64;
	uint32_t u32;
	int32_t s32;
	float f32;
};

struct abc_adora_type_t {
	uint32_t type;
	uint32_t vflag;
};

struct abc_adora_mcode_s {
	abc_adora_mcode_append_f append;
	abc_adora_mcode_fillch_f fillch;
	abc_adora_mcode_offset_f offset;
	abc_adora_mcode_restore_f restore;
	abc_adora_mcode_mapping_f mapping;
	abc_adora_mcode_clear_f clear;
};

#endif
