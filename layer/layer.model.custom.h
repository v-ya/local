#ifndef _layer_model_custom_h_
#define _layer_model_custom_h_

#include "layer.model.h"

typedef struct layer_model_custom_enum_t layer_model_custom_enum_t;
typedef struct layer_model_custom_object_t layer_model_custom_object_t;

typedef layer_model_item_s* (*layer_model_custom_object_default_f)(const layer_model_s *restrict m);

struct layer_model_custom_enum_t {
	const char *restrict name;
	uint64_t value;
};

struct layer_model_custom_object_t {
	const char *restrict key;
	const char *restrict name;
	layer_model_custom_object_default_f func;
};

// create type

layer_model_s* layer_model_create_type__null(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
layer_model_s* layer_model_create_type__uint(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
layer_model_s* layer_model_create_type__int(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
layer_model_s* layer_model_create_type__float(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
layer_model_s* layer_model_create_type__boolean(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
layer_model_s* layer_model_create_type__string(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
layer_model_s* layer_model_create_type__data(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor);
layer_model_s* layer_model_create_type__enum(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict ename);
layer_model_s* layer_model_create_type__array(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname);
layer_model_s* layer_model_create_type__object(layer_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname, const char *restrict oname);

// create any

layer_model_s* layer_model_create_any(layer_model_s *restrict m, const char *restrict name, const char *const restrict *restrict list);

// create enum

layer_model_s* layer_model_create_enum(layer_model_s *restrict m, const char *restrict name, const layer_model_custom_enum_t *restrict list, uint64_t dv);

// create object

layer_model_s* layer_model_create_object(layer_model_s *restrict m, const char *restrict name, const layer_model_custom_object_t *restrict list);

#endif
