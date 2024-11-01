#ifndef _miko_base_miko_major_h_
#define _miko_base_miko_major_h_

#include "miko.h"
#include <vattr.h>
#include <yaw.h>

struct miko_major_s {
	refer_string_t name;       // miko.major.name
	refer_string_t iset;       // miko.iset.name
	yaw_lock_s *read;          // miko.major's read lock
	yaw_lock_s *write;         // miko.major's write lock
	vattr_s *pool;             // miko.minor.name => (miko_minor_s *) miko.minor
	miko_vector_s_t table;     // miko_minor_s *table[]
	miko_major_vtype_t vtype;  // create var's vtype (<real>, refer_t, miko_wink_t)
};

miko_major_s* miko_major_alloc(const char *restrict name, refer_string_t iset, miko_major_vtype_t vtype);

#endif
