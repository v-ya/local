#include "layer.model.h"

const layer_model_type_s* layer_model_type_create__null(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor)
{
	return layer_model_type_alloc(name, layer_model_type__unknow, type_minor, sizeof(layer_model_type_s), NULL, NULL);
}
