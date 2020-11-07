#include "at_imple.h"
#include "../at_cache.h"
#include "at_op_imple.h"

at_layer_t* at_layer_op_set_imple(at_layer_t *restrict layer)
{
	if (!at_layer_op_set$none(layer)) goto label_fail;
	return layer;
	label_fail:
	return NULL;
}
