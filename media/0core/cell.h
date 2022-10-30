#ifndef _media_core_cell_h_
#define _media_core_cell_h_

#include <refer.h>

// v' = (v + addend) / divisor * multiplier + plus;
// size = cellsize' * dimension';

struct media_cell_info_t {
	uintptr_t cellsize_addend;
	uintptr_t cellsize_divisor;
	uintptr_t cellsize_multiplier;
	uintptr_t cellsize_plus;
	uintptr_t dimension_addend;
	uintptr_t dimension_divisor;
	uintptr_t dimension_multiplier;
	uintptr_t dimension_plus;
};

struct media_cell_t {
	uintptr_t cell_size;
	uintptr_t cell_number;
};

struct media_cell_info_t* media_cell_info_initial(struct media_cell_info_t *restrict info, const struct media_cell_info_t *restrict param);
uintptr_t media_cell_set(struct media_cell_t *restrict cell, const struct media_cell_info_t *restrict info, uintptr_t cellsize, uintptr_t dimension);

#endif
