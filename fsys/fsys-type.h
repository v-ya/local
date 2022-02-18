#ifndef _fsys_type_h_
#define _fsys_type_h_

#ifndef _DEFAULT_SOURCE
#define _DEFAULT_SOURCE
#endif
#include "fsys.h"
#include <dirent.h>

#define fsys_default_open_mode  0644

typedef typeof(((struct dirent *) 0)->d_type) fsys_dt_type_t;

static inline fsys_type_t fsys_dt2type(fsys_dt_type_t type)
{
	switch (type)
	{
		case DT_CHR: return fsys_type_cdev;
		case DT_BLK: return fsys_type_bdev;
		case DT_DIR: return fsys_type_dirent;
		case DT_REG: return fsys_type_file;
		case DT_LNK: return fsys_type_link;
		default: return fsys_type_unknow;
	}
}

#endif
