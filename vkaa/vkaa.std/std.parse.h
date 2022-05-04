#ifndef _vkaa_std_std_parse_h_
#define _vkaa_std_std_parse_h_

#include "std.h"

// keyword

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_var(vkaa_parse_s *restrict parse);

// operator

// type2var

vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_scope(vkaa_parse_s *restrict parse, const vkaa_std_typeid_t *restrict typeid);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_brackets(vkaa_parse_s *restrict parse);
vkaa_parse_type2var_s* vkaa_std_parse_set_type2var_square(vkaa_parse_s *restrict parse);

#endif
