#ifndef _fsys_dirent_h_
#define _fsys_dirent_h_

#include "fsys.h"
#include "fsys-type.h"
#include <exbuffer.h>
#include <dirent.h>

#define fsys_dir_stack_default_size  16
#define fsys_dir_stack_max_depth     64

typedef struct fsys_dir_stack_t {
	DIR *dirent;
	struct dirent *curr;
	uintptr_t pp_last_name;
	uintptr_t pp_last_end;
} fsys_dir_stack_t;

struct fsys_dir_s {
	DIR *dirent;
	struct dirent *curr;
	// dir stack
	fsys_dir_stack_t *dstack;
	uintptr_t ds_size;
	uintptr_t ds_pos;
	uintptr_t ds_max_depth;
	// path
	exbuffer_t path_buffer;
	uintptr_t pp_rpath;
	uintptr_t pp_name;
	uintptr_t pp_end;
	// control
	fsys_type_t allow_type;
	fsys_dir_flag_t flags;
	// item cache
	fsys_dir_item_t item;
};

#endif
