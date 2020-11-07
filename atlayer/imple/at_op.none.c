#include "at_op.none.h"

at_layer_t* at_layer_op_set$none(at_layer_t *restrict layer)
{
	return at_layer_op_set(layer, at_op$none, 0, NULL, NULL, NULL);
}
