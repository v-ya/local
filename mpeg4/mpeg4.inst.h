#ifndef _mpeg4_inst_h_
#define _mpeg4_inst_h_

#include <rbtree.h>
#include <mlog.h>

#define mlog_level_width            4
#define mlog_level_dump(_fmt, ...)  mlog_printf(mlog, "%*c" _fmt, level * mlog_level_width + 1, 0, ##__VA_ARGS__)

typedef struct mpeg4_s mpeg4_t;

typedef struct mpeg4_dump_data_t mpeg4_dump_data_t;

typedef const mpeg4_t* (*mpeg4_dump_f)(const mpeg4_t *restrict inst, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, mpeg4_dump_data_t *restrict unidata, uint32_t level);

#define mpeg4$define(_major, _minor, _func)  mpeg4$##_major##$##_minor##$##_func
#define mpeg4$define$dump(_major, _minor)    const mpeg4_t* mpeg4$define(_major, _minor, dump)(const mpeg4_t *restrict inst, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, mpeg4_dump_data_t *restrict unidata, uint32_t level)

typedef struct mpeg4_func_box_t {
	mpeg4_dump_f dump;
} mpeg4_func_box_t;

// => mpeg4_func_box_t
struct mpeg4_s {
	rbtree_t *type;
	rbtree_t *uuid;
};

mpeg4_t* mpeg4_set_type(mpeg4_t *restrict inst, uint32_t type, const mpeg4_func_box_t *restrict func);
const mpeg4_t* mpeg4_dump_level(const mpeg4_t *restrict inst, mlog_s *restrict mlog, const uint8_t *restrict data, uint64_t size, mpeg4_dump_data_t *restrict unidata, uint32_t level);

#endif
