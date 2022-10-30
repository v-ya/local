#include "cell.h"
#include <memory.h>

struct media_cell_info_t* media_cell_info_initial(struct media_cell_info_t *restrict info, const struct media_cell_info_t *restrict param)
{
	if (param->cellsize_divisor && param->dimension_divisor)
		return (struct media_cell_info_t *) memcpy(info, param, sizeof(*info));
	return NULL;
}

uintptr_t media_cell_set(struct media_cell_t *restrict cell, const struct media_cell_info_t *restrict info, uintptr_t cellsize, uintptr_t dimension)
{
	#define d_calc(_field, _info)  ((_field + (_info)->_field##_addend) / (_info)->_field##_divisor * (_info)->_field##_multiplier + (_info)->_field##_plus)
	cell->cell_size = cellsize = d_calc(cellsize, info);
	cell->cell_number = dimension = d_calc(dimension, info);
	#undef d_calc
	return cellsize * dimension;
}
