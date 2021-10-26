#ifndef _fsys_h_
#define _fsys_h_

#include <refer.h>

typedef struct fsys_dir_s fsys_dir_s;

typedef enum fsys_type_t {
	fsys_type_null   = 0x00,
	fsys_type_dirent = 0x01,
	fsys_type_file   = 0x02,
	fsys_type_link   = 0x04,
	fsys_type_unknow = 0x80,
	fsys_type_all    = 0xff
} fsys_type_t;

typedef enum fsys_dir_flag_t {
	fsys_dir_flag_normal                     = 0x00,
	fsys_dir_flag_looping                    = 0x01,
	fsys_dir_flag_discard_same_iparent       = 0x02,
	fsys_dir_flag_discard_hide               = 0x04,
	fsys_dir_flag_link_type_parse            = 0x08,
	fsys_dir_flag_interrupt_beyond_max_depth = 0x10,
	fsys_dir_flag_interrupt_sub_dirent_fail  = 0x20
} fsys_dir_flag_t;

typedef struct fsys_dir_item_t {
	fsys_type_t type;
	const char *path;
	const char *rpath;
	const char *name;
} fsys_dir_item_t;

fsys_dir_s* fsys_dir_alloc(const char *restrict path, fsys_type_t type, fsys_dir_flag_t flags);
void fsys_dir_set_max_depth(fsys_dir_s *restrict dir, uintptr_t max_depth);
fsys_dir_s* fsys_dir_read(fsys_dir_s *restrict dir, fsys_dir_item_t *restrict item);

#endif
