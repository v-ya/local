#include "gvcx.model.h"

const gvcx_model_type_s* gvcx_model_type_create__null(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor)
{
	return gvcx_model_type_alloc(name, gvcx_model_type__unknow, type_minor, sizeof(gvcx_model_type_s), NULL, NULL);
}
