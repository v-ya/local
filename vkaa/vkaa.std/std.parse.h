#ifndef _vkaa_std_std_parse_h_
#define _vkaa_std_std_parse_h_

#include "std.h"
#include "../vkaa.parse.h"
#include "../vkaa.syntax.h"
#include "../vkaa.oplevel.h"
#include "../vkaa.tpool.h"
#include "../vkaa.scope.h"
#include "../vkaa.selector.h"
#include "../vkaa.function.h"
#include "../vkaa.execute.h"

// keyword

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword(vkaa_parse_s *restrict p, const char *restrict keyword, vkaa_parse_keyword_f parse, vkaa_parse_keytype_t keytype);

vkaa_parse_keyword_s* vkaa_std_parse_set_keyword_var(vkaa_parse_s *restrict parse);

// operator

// type2var

#endif
