#ifndef _mpeg4_inst_h_
#define _mpeg4_inst_h_

#include <rbtree.h>
#include <mlog.h>

#define mlog_level_width            4
#define mlog_level_inital           1
#define mlog_level_dump(_fmt, ...)  mlog_printf(mlog, "%*c" _fmt, level, 0, ##__VA_ARGS__)

#define mpeg4$define(_major, _minor, _func)  mpeg4$##_major##$##_minor##$##_func

typedef struct mpeg4_s mpeg4_t;

struct mpeg4_atom_s;

typedef const struct mpeg4_atom_s* (mpeg4_atom_alloc_f)(struct mpeg4_s *restrict inst);

// => mpeg4_atom_s
struct mpeg4_s {
	rbtree_t *pool;
	const struct mpeg4_atom_s *root;
	const struct mpeg4_atom_s *unknow;
};

const struct mpeg4_atom_s* mpeg4_find_atom(mpeg4_t *restrict inst, mpeg4_atom_alloc_f func, uint32_t type, uint32_t extra);

#endif
