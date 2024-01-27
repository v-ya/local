#include "gvcx.model.h"

const gvcx_model_type_s* gvcx_model_type_create__object(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor, const char *restrict cname)
{
	return gvcx_model_type_alloc(name, gvcx_model_type__object, 0, sizeof(gvcx_model_type_s), NULL, NULL);
}
