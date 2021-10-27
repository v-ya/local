#ifndef _fsys_h_
#define _fsys_h_

#include <refer.h>

typedef struct fsys_rpath_s fsys_rpath_s;
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

// rpath

fsys_rpath_s* fsys_rpath_alloc(const char *restrict root_path, uintptr_t stack_size);
void fsys_rpath_set_delimiter(fsys_rpath_s *restrict rp, char delimiter);
fsys_rpath_s* fsys_rpath_set_root_with_length(fsys_rpath_s *restrict rp, const char *restrict root_path, uintptr_t length);
fsys_rpath_s* fsys_rpath_set_root(fsys_rpath_s *restrict rp, const char *restrict root_path);
const char* fsys_rpath_get_full_path_with_length(fsys_rpath_s *restrict rp, const char *restrict rpath, uintptr_t length);
const char* fsys_rpath_get_full_path(fsys_rpath_s *restrict rp, const char *restrict rpath);
const char* fsys_rpath_push_with_length(fsys_rpath_s *restrict rp, const char *restrict rpath, uintptr_t length);
const char* fsys_rpath_push(fsys_rpath_s *restrict rp, const char *restrict rpath);
const char* fsys_rpath_pop(fsys_rpath_s *restrict rp);

// dir

fsys_dir_s* fsys_dir_alloc(const char *restrict path, fsys_type_t type, fsys_dir_flag_t flags);
void fsys_dir_set_max_depth(fsys_dir_s *restrict dir, uintptr_t max_depth);
fsys_dir_s* fsys_dir_read(fsys_dir_s *restrict dir, fsys_dir_item_t *restrict item);

// ctrl

fsys_type_t fsys_ctrl_exist(const char *restrict path, uint64_t *restrict rsize);
int fsys_ctrl_remove(const char *restrict path);
int fsys_ctrl_rmdir(const char *restrict path);
int fsys_ctrl_mkdir(const char *restrict path);
int fsys_ctrl_copy(const char *restrict dst, const char *restrict src);
int fsys_ctrl_make_parent_path(fsys_rpath_s *restrict rp, const char *restrict rpath);
int fsys_ctrl_mkpath_copy(fsys_rpath_s *restrict dst_rp, const char *restrict dst_rpath, const char *restrict src);

#endif
