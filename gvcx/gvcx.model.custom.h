#ifndef _gvcx_model_custom_h_
#define _gvcx_model_custom_h_

#include "gvcx.model.h"

typedef struct gvcx_model_custom_enum_t gvcx_model_custom_enum_t;
typedef struct gvcx_model_custom_object_t gvcx_model_custom_object_t;

typedef gvcx_model_item_s* (*gvcx_model_custom_object_default_f)(const gvcx_model_s *restrict m);

struct gvcx_model_custom_enum_t {
	const char *restrict name;
	uint64_t value;
};

struct gvcx_model_custom_object_t {
	const char *restrict key;
	const char *restrict name;
	gvcx_model_custom_object_default_f func;
};

// create type

gvcx_model_s* gvcx_model_create_type__uint(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
gvcx_model_s* gvcx_model_create_type__int(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
gvcx_model_s* gvcx_model_create_type__float(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
gvcx_model_s* gvcx_model_create_type__boolean(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
gvcx_model_s* gvcx_model_create_type__string(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
gvcx_model_s* gvcx_model_create_type__data(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor);
gvcx_model_s* gvcx_model_create_type__enum(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict ename);
gvcx_model_s* gvcx_model_create_type__array(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname);
gvcx_model_s* gvcx_model_create_type__object(gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname);

// create any

gvcx_model_s* gvcx_model_create_any(gvcx_model_s *restrict m, const char *restrict name, const char *const restrict *restrict list);

// create enum

gvcx_model_s* gvcx_model_create_enum(gvcx_model_s *restrict m, const char *restrict name, const gvcx_model_custom_enum_t *restrict list, uint64_t dv);

// create object

gvcx_model_s* gvcx_model_create_object(gvcx_model_s *restrict m, const char *restrict name, const gvcx_model_custom_object_t *restrict list);

#endif
