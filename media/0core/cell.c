#include "cell.h"

struct media_cell_info_t* media_cell_info_initial(struct media_cell_info_t *restrict info, uintptr_t cell_addend, uintptr_t cell_align, uintptr_t dimension_addend, uintptr_t dimension_multiplier, uintptr_t dimension_divisor, uintptr_t size_align)
{
	if (cell_align && size_align && dimension_divisor)
	{
		info->cell_addend = cell_addend;
		info->cell_align = cell_align;
		info->dimension_addend = dimension_addend;
		info->dimension_multiplier = dimension_multiplier;
		info->dimension_divisor = dimension_divisor;
		info->size_align = size_align;
		return info;
	}
	return NULL;
}

uintptr_t media_cell_set(struct media_cell_t *restrict cell, const struct media_cell_info_t *restrict info, uintptr_t cell_size, uintptr_t dimension)
{
	uintptr_t size;
	cell_size = (cell_size + info->cell_addend + info->cell_align - 1) / info->cell_align;
	dimension = (dimension + info->dimension_addend) * info->dimension_multiplier / info->dimension_divisor;
	size = cell_size * dimension;
	cell->cell_number = dimension;
	cell->cell_size = cell_size;
	cell->used_size = (size + info->size_align - 1) / info->size_align;
	cell->skip_size = cell->used_size - size;
	return cell->used_size;
}
