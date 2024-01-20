#include "gvcx.model.h"

const gvcx_model_type_s* gvcx_model_type_create__any(void)
{
	return gvcx_model_type_alloc(gvcx_model_stype__any, sizeof(gvcx_model_type_s), NULL, NULL);
}
