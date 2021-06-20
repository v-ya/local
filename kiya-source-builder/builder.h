#ifndef _kiya_source_builder_builder_h_
#define _kiya_source_builder_builder_h_

#include <refer.h>

typedef struct source_builder_s source_builder_s;

typedef source_builder_s* (*source_builder_set_attr_f)(source_builder_s *restrict sb, const char *restrict attr);
typedef source_builder_s* (*source_builder_set_source_f)(source_builder_s *restrict sb, const char *restrict name, const char *restrict path);
typedef source_builder_s* (*source_builder_save_f)(source_builder_s *restrict sb, const char *restrict path);

struct source_builder_s {
	source_builder_set_attr_f verify;
	source_builder_set_attr_f version;
	source_builder_set_attr_f author;
	source_builder_set_attr_f time;
	source_builder_set_attr_f description;
	source_builder_set_attr_f flag;
	source_builder_set_source_f source;
	source_builder_save_f save;
};

struct pocket_verify_s;

source_builder_s* source_builder_alloc_script(const char *restrict output);
source_builder_s* source_builder_alloc_pocket(struct pocket_verify_s *restrict verify);

#endif
