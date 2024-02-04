#ifndef _gvcx_model_pri_h_
#define _gvcx_model_pri_h_

#include "../gvcx.model.h"
#include "../gvcx.model.custom.h"
#include "../gvcx.model.string.h"
#include "../gvcx.log.h"
#include "../gvcx.file.h"
#include <vattr.h>

typedef enum gvcx_model_type_t gvcx_model_type_t;
typedef enum gvcx_model_flag_t gvcx_model_flag_t;
typedef struct gvcx_model_type_s gvcx_model_type_s;
typedef struct gvcx_model_any_s gvcx_model_any_s;
typedef struct gvcx_model_enum_s gvcx_model_enum_s;
typedef struct gvcx_model_object_s gvcx_model_object_s;

typedef gvcx_model_type_s* (*gvcx_model_type_initial_f)(gvcx_model_type_s *restrict t, const void *restrict pri);
typedef gvcx_model_item_s* (*gvcx_model_type_create_f)(const gvcx_model_type_s *restrict t, const gvcx_model_s *restrict m);
typedef gvcx_model_item_s* (*gvcx_model_type_copyto_f)(const gvcx_model_type_s *restrict t, gvcx_model_item_s *restrict dst, const gvcx_model_item_s *restrict src);
typedef void (*gvcx_model_type_iprint_f)(const gvcx_model_type_s *restrict t, const gvcx_model_item_s *restrict item, gvcx_log_s *restrict log);

#define d_type_symbol(_name, _method)       gvcx_model_type__##_name##__##_method
#define d_type_initial(_name, _type)        gvcx_model_type_s* d_type_symbol(_name, initial)(gvcx_model_type_s *restrict t, const _type *restrict pri)
#define d_type_create(_name, _type)         gvcx_model_item_s* d_type_symbol(_name, create)(const _type *restrict t, const gvcx_model_s *restrict m)
#define d_type_copyto(_name, _type, _item)  gvcx_model_item_s* d_type_symbol(_name, copyto)(const _type *restrict t, _item *restrict dst, const _item *restrict src)
#define d_type_iprint(_name, _type, _item)  void d_type_symbol(_name, iprint)(const _type *restrict t, const _item *restrict item, gvcx_log_s *restrict log)
#define d_type_function(_name, _method)     (gvcx_model_type_##_method##_f) d_type_symbol(_name, _method)

enum gvcx_model_type_t {
	gvcx_model_type__unknow,  //?unknow
	gvcx_model_type__any,     // any
	gvcx_model_type__uint,    // uint64
	gvcx_model_type__int,     // int64
	gvcx_model_type__float,   // double
	gvcx_model_type__boolean, // boolean
	gvcx_model_type__string,  // string
	gvcx_model_type__data,    // data-block
	gvcx_model_type__enum,    //+enum
	gvcx_model_type__array,   //+array
	gvcx_model_type__object,  //+object
	gvcx_model_type__preset,  //+preset
	gvcx_model_type_max,
};

enum gvcx_model_flag_t {
	gvcx_model_flag__write  = 0x1,
	gvcx_model_flag__child  = 0x2,
};

struct gvcx_model_type_s {
	refer_string_t name;
	uint32_t type_major;
	uint32_t type_minor;
	gvcx_model_type_create_f create;
	gvcx_model_type_copyto_f copyto;
	gvcx_model_type_iprint_f iprint;
};

struct gvcx_model_item_s {
	const gvcx_model_type_s *type;
	uint32_t type_major;
	uint32_t item_flag;
};

struct gvcx_model_s {
	vattr_s *pool_type;
	vattr_s *pool_any;
	vattr_s *pool_enum;
	vattr_s *pool_object;
};

// gvcx.model.c

const gvcx_model_type_s* gvcx_model_find_type(const gvcx_model_s *restrict m, const char *restrict name);
const gvcx_model_any_s* gvcx_model_find_any(const gvcx_model_s *restrict m, const char *restrict name);
const gvcx_model_enum_s* gvcx_model_find_enum(const gvcx_model_s *restrict m, const char *restrict name);
const gvcx_model_object_s* gvcx_model_find_object(const gvcx_model_s *restrict m, const char *restrict name);
refer_string_t gvcx_model_find_cname(const gvcx_model_s *restrict m, const char *restrict cname, const gvcx_model_type_s *restrict *restrict type, const gvcx_model_any_s *restrict *restrict any);
gvcx_model_s* gvcx_model_add_type(gvcx_model_s *restrict m, const gvcx_model_type_s *restrict type);
gvcx_model_s* gvcx_model_add_any(gvcx_model_s *restrict m, const gvcx_model_any_s *restrict any);
gvcx_model_s* gvcx_model_add_enum(gvcx_model_s *restrict m, const char *restrict name, const gvcx_model_enum_s *restrict e);
gvcx_model_s* gvcx_model_add_object(gvcx_model_s *restrict m, const char *restrict name, const gvcx_model_object_s *restrict object);

// gvcx.model.t.c

void gvcx_model_type_free_func(gvcx_model_type_s *restrict r);
const gvcx_model_type_s* gvcx_model_type_alloc(const char *restrict name, uint32_t type_major, uint32_t type_minor, uintptr_t tsize, gvcx_model_type_initial_f initial, const void *restrict pri);
gvcx_model_type_t gvcx_model_type_name_enum(const char *restrict name);
const char* gvcx_model_type_name_inherit(const char *restrict parent, const char *restrict child);
const char* gvcx_model_type_inherit_name(refer_string_t cname, const gvcx_model_any_s *restrict cany, const char *restrict name);
const gvcx_model_type_s* gvcx_model_type_inherit_type(refer_string_t cname, const gvcx_model_any_s *restrict cany, const gvcx_model_type_s *restrict type);

// gvcx.model.any.c

gvcx_model_any_s* gvcx_model_any_alloc(const char *restrict name);
refer_string_t gvcx_model_any_name(const gvcx_model_any_s *restrict any);
gvcx_model_any_s* gvcx_model_any_insert(gvcx_model_any_s *restrict any, const char *restrict name);
const char* gvcx_model_any_inherit(const gvcx_model_any_s *restrict any, const char *restrict name);

// gvcx.model.enum.c

gvcx_model_enum_s* gvcx_model_enum_alloc(uint64_t dv);
gvcx_model_enum_s* gvcx_model_enum_insert(gvcx_model_enum_s *restrict e, const char *restrict name, uint64_t value);
uint64_t gvcx_model_enum_dv(const gvcx_model_enum_s *restrict e);
const char* gvcx_model_enum_name(const gvcx_model_enum_s *restrict e, uint64_t value);
const gvcx_model_enum_s* gvcx_model_enum_value(const gvcx_model_enum_s *restrict e, const char *restrict name, uint64_t *restrict value);

// gvcx.model.object.c

gvcx_model_object_s* gvcx_model_object_alloc(void);
const vattr_s* gvcx_model_object_vattr(const gvcx_model_object_s *restrict object);
gvcx_model_object_s* gvcx_model_object_insert(gvcx_model_object_s *restrict object, const char *restrict key, refer_string_t name, const gvcx_model_any_s *restrict any, gvcx_model_item_s *restrict dv);
const gvcx_model_object_s* gvcx_model_object_find(const gvcx_model_object_s *restrict object, const char *restrict key, refer_string_t *restrict name, const gvcx_model_any_s *restrict *restrict any, gvcx_model_item_s *restrict *restrict dv);

// gvcx.model.i.c

void gvcx_model_item_free_func(gvcx_model_item_s *restrict r);
gvcx_model_item_s* gvcx_model_item_alloc(uintptr_t isize, const gvcx_model_type_s *restrict type);
mlog_s* gvcx_model_item_iprint(const gvcx_model_item_s *restrict item, gvcx_log_s *restrict log);

// gvcx.model.t.*.c

const gvcx_model_type_s* gvcx_model_type_create__null(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const gvcx_model_type_s* gvcx_model_type_create__uint(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const gvcx_model_type_s* gvcx_model_type_create__int(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const gvcx_model_type_s* gvcx_model_type_create__float(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const gvcx_model_type_s* gvcx_model_type_create__boolean(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const gvcx_model_type_s* gvcx_model_type_create__string(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const gvcx_model_type_s* gvcx_model_type_create__data(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const gvcx_model_type_s* gvcx_model_type_create__enum(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict ename);
const gvcx_model_type_s* gvcx_model_type_create__array(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname);
const gvcx_model_type_s* gvcx_model_type_create__object(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname, const char *restrict oname);

#endif
