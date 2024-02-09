#ifndef _layer_model_pri_h_
#define _layer_model_pri_h_

#include "../layer.model.h"
#include "../layer.model.custom.h"
#include "../layer.model.string.h"
#include "../layer.log.h"
#include "../layer.file.h"
#include <vattr.h>

typedef enum layer_model_type_t layer_model_type_t;
typedef enum layer_model_flag_t layer_model_flag_t;
typedef struct layer_model_type_s layer_model_type_s;
typedef struct layer_model_any_s layer_model_any_s;
typedef struct layer_model_enum_s layer_model_enum_s;
typedef struct layer_model_object_s layer_model_object_s;

typedef layer_model_type_s* (*layer_model_type_initial_f)(layer_model_type_s *restrict t, const void *restrict pri);
typedef layer_model_item_s* (*layer_model_type_create_f)(const layer_model_type_s *restrict t, const layer_model_s *restrict m);
typedef layer_model_item_s* (*layer_model_type_copyto_f)(const layer_model_type_s *restrict t, layer_model_item_s *restrict dst, const layer_model_item_s *restrict src);
typedef void (*layer_model_type_iprint_f)(const layer_model_type_s *restrict t, const layer_model_item_s *restrict item, layer_log_s *restrict log);

#define d_type_symbol(_name, _method)       layer_model_type__##_name##__##_method
#define d_type_initial(_name, _type)        layer_model_type_s* d_type_symbol(_name, initial)(layer_model_type_s *restrict t, const _type *restrict pri)
#define d_type_create(_name, _type)         layer_model_item_s* d_type_symbol(_name, create)(const _type *restrict t, const layer_model_s *restrict m)
#define d_type_copyto(_name, _type, _item)  layer_model_item_s* d_type_symbol(_name, copyto)(const _type *restrict t, _item *restrict dst, const _item *restrict src)
#define d_type_iprint(_name, _type, _item)  void d_type_symbol(_name, iprint)(const _type *restrict t, const _item *restrict item, layer_log_s *restrict log)
#define d_type_function(_name, _method)     (layer_model_type_##_method##_f) d_type_symbol(_name, _method)

enum layer_model_type_t {
	layer_model_type__unknow,  //?unknow
	layer_model_type__any,     // any
	layer_model_type__uint,    // uint64
	layer_model_type__int,     // int64
	layer_model_type__float,   // double
	layer_model_type__boolean, // boolean
	layer_model_type__string,  // string
	layer_model_type__data,    // data-block
	layer_model_type__enum,    //+enum
	layer_model_type__array,   //+array
	layer_model_type__object,  //+object
	layer_model_type__preset,  //+preset
	layer_model_type_max,
};

enum layer_model_flag_t {
	layer_model_flag__write  = 0x1,
	layer_model_flag__child  = 0x2,
};

struct layer_model_type_s {
	refer_string_t name;
	uint32_t type_major;
	uint32_t type_minor;
	layer_model_type_create_f create;
	layer_model_type_copyto_f copyto;
	layer_model_type_iprint_f iprint;
};

struct layer_model_item_s {
	const layer_model_type_s *type;
	uint32_t type_major;
	uint32_t item_flag;
};

struct layer_model_s {
	vattr_s *pool_type;
	vattr_s *pool_any;
	vattr_s *pool_enum;
	vattr_s *pool_object;
};

// layer.model.c

const layer_model_type_s* layer_model_find_type(const layer_model_s *restrict m, const char *restrict name);
const layer_model_any_s* layer_model_find_any(const layer_model_s *restrict m, const char *restrict name);
const layer_model_enum_s* layer_model_find_enum(const layer_model_s *restrict m, const char *restrict name);
const layer_model_object_s* layer_model_find_object(const layer_model_s *restrict m, const char *restrict name);
refer_string_t layer_model_find_cname(const layer_model_s *restrict m, const char *restrict cname, const layer_model_type_s *restrict *restrict type, const layer_model_any_s *restrict *restrict any);
layer_model_s* layer_model_add_type(layer_model_s *restrict m, const layer_model_type_s *restrict type);
layer_model_s* layer_model_add_any(layer_model_s *restrict m, const layer_model_any_s *restrict any);
layer_model_s* layer_model_add_enum(layer_model_s *restrict m, const char *restrict name, const layer_model_enum_s *restrict e);
layer_model_s* layer_model_add_object(layer_model_s *restrict m, const char *restrict name, const layer_model_object_s *restrict object);

// layer.model.t.c

void layer_model_type_free_func(layer_model_type_s *restrict r);
const layer_model_type_s* layer_model_type_alloc(const char *restrict name, uint32_t type_major, uint32_t type_minor, uintptr_t tsize, layer_model_type_initial_f initial, const void *restrict pri);
layer_model_type_t layer_model_type_name_enum(const char *restrict name);
const char* layer_model_type_name_inherit(const char *restrict parent, const char *restrict child);
const char* layer_model_type_inherit_name(refer_string_t cname, const layer_model_any_s *restrict cany, const char *restrict name);
const layer_model_type_s* layer_model_type_inherit_type(refer_string_t cname, const layer_model_any_s *restrict cany, const layer_model_type_s *restrict type);

// layer.model.any.c

layer_model_any_s* layer_model_any_alloc(const char *restrict name);
refer_string_t layer_model_any_name(const layer_model_any_s *restrict any);
layer_model_any_s* layer_model_any_insert(layer_model_any_s *restrict any, const char *restrict name);
const char* layer_model_any_inherit(const layer_model_any_s *restrict any, const char *restrict name);

// layer.model.enum.c

layer_model_enum_s* layer_model_enum_alloc(uint64_t dv);
layer_model_enum_s* layer_model_enum_insert(layer_model_enum_s *restrict e, const char *restrict name, uint64_t value);
uint64_t layer_model_enum_dv(const layer_model_enum_s *restrict e);
const char* layer_model_enum_name(const layer_model_enum_s *restrict e, uint64_t value);
const layer_model_enum_s* layer_model_enum_value(const layer_model_enum_s *restrict e, const char *restrict name, uint64_t *restrict value);

// layer.model.object.c

layer_model_object_s* layer_model_object_alloc(void);
const vattr_s* layer_model_object_vattr(const layer_model_object_s *restrict object);
layer_model_object_s* layer_model_object_insert(layer_model_object_s *restrict object, const char *restrict key, refer_string_t name, const layer_model_any_s *restrict any, layer_model_item_s *restrict dv);
const layer_model_object_s* layer_model_object_find(const layer_model_object_s *restrict object, const char *restrict key, refer_string_t *restrict name, const layer_model_any_s *restrict *restrict any, layer_model_item_s *restrict *restrict dv);

// layer.model.i.c

void layer_model_item_free_func(layer_model_item_s *restrict r);
layer_model_item_s* layer_model_item_alloc(uintptr_t isize, const layer_model_type_s *restrict type);
mlog_s* layer_model_item_iprint(const layer_model_item_s *restrict item, layer_log_s *restrict log);

// layer.model.t.*.c

const layer_model_type_s* layer_model_type_create__null(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const layer_model_type_s* layer_model_type_create__uint(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const layer_model_type_s* layer_model_type_create__int(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const layer_model_type_s* layer_model_type_create__float(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const layer_model_type_s* layer_model_type_create__boolean(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const layer_model_type_s* layer_model_type_create__string(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const layer_model_type_s* layer_model_type_create__data(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
const layer_model_type_s* layer_model_type_create__enum(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict ename);
const layer_model_type_s* layer_model_type_create__array(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname);
const layer_model_type_s* layer_model_type_create__object(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname, const char *restrict oname);

#endif
