#ifndef _gvcx_model_pri_h_
#define _gvcx_model_pri_h_

#include "../gvcx.model.h"
#include "../gvcx.model.string.h"
#include <vattr.h>

typedef enum gvcx_model_type_t gvcx_model_type_t;
typedef enum gvcx_model_flag_t gvcx_model_flag_t;
typedef struct gvcx_model_type_s gvcx_model_type_s;

typedef gvcx_model_type_s* (*gvcx_model_type_initial_f)(gvcx_model_type_s *restrict t, const void *restrict pri);
typedef gvcx_model_item_s* (*gvcx_model_type_create_f)(const gvcx_model_type_s *restrict t, const gvcx_model_s *restrict m, gvcx_model_type_t type, gvcx_model_flag_t flag, const char *restrict tname);
typedef gvcx_model_item_s* (*gvcx_model_type_copyto_f)(const gvcx_model_type_s *restrict t, gvcx_model_item_s *restrict dst, const gvcx_model_item_s *restrict src);

enum gvcx_model_type_t {
	gvcx_model_type__any,    // any
	gvcx_model_type__u,      // uint64
	gvcx_model_type__i,      // int64
	gvcx_model_type__f,      // double
	gvcx_model_type__b,      // boolean
	gvcx_model_type__s,      // string
	gvcx_model_type__d,      //+data-block
	gvcx_model_type__e,      //+enum
	gvcx_model_type__a,      //+array
	gvcx_model_type__o,      //+object
	gvcx_model_type__preset, //+preset
	gvcx_model_type_max,
};

enum gvcx_model_flag_t {
	gvcx_model_flag__write  = 0x1,
};

struct gvcx_model_type_s {
	refer_string_t tname;
	gvcx_model_type_create_f create;
	gvcx_model_type_copyto_f copyto;
};

struct gvcx_model_s {
	vattr_s *pool_enum;
	vattr_s *pool_object;
	vattr_s *pool_preset;
	const gvcx_model_type_s *type[gvcx_model_type_max];
};

struct gvcx_model_item_s {
	uint32_t type;
	uint32_t flag;
	refer_string_t tname;
};

// gvcx.model.t.c

void gvcx_model_type_free_func(gvcx_model_type_s *restrict r);
const gvcx_model_type_s* gvcx_model_type_alloc(const char *restrict tname, uintptr_t tsize, gvcx_model_type_initial_f initial, const void *restrict pri);
gvcx_model_type_t gvcx_model_type_enum(const char *restrict tname);
const char* gvcx_model_type_inherit(const char *restrict parent, const char *restrict child);

// gvcx.model.i.c

void gvcx_model_item_free_func(gvcx_model_item_s *restrict r);
gvcx_model_item_s* gvcx_model_item_alloc(uintptr_t isize, gvcx_model_type_t type, gvcx_model_flag_t flag, refer_string_t tname);

// gvcx.model.t.*.c

const gvcx_model_type_s* gvcx_model_type_create__any(void);
const gvcx_model_type_s* gvcx_model_type_create__u(void);
const gvcx_model_type_s* gvcx_model_type_create__i(void);
const gvcx_model_type_s* gvcx_model_type_create__f(void);
const gvcx_model_type_s* gvcx_model_type_create__b(void);
const gvcx_model_type_s* gvcx_model_type_create__s(void);
const gvcx_model_type_s* gvcx_model_type_create__d(void);
const gvcx_model_type_s* gvcx_model_type_create__e(void);
const gvcx_model_type_s* gvcx_model_type_create__a(void);
const gvcx_model_type_s* gvcx_model_type_create__o(void);
const gvcx_model_type_s* gvcx_model_type_create__preset(void);

#endif
