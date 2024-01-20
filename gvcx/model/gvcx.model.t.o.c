#include "gvcx.model.h"

const gvcx_model_type_s* gvcx_model_type_create__o(void)
{
	return gvcx_model_type_alloc(gvcx_model_stype__object, sizeof(gvcx_model_type_s), NULL, NULL);
}
