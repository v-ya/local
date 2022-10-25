#ifndef _media_core_cell_h_
#define _media_core_cell_h_

#include <refer.h>

// cell_size = (cell_size + cell_addend + cell_align - 1) / cell_align;
// dimension = (dimension + dimension_addend) * dimension_multiplier / dimension_divisor;
// size = (size + size_align - 1) / size_align;

struct media_cell_info_t {
	uintptr_t cell_addend;
	uintptr_t cell_align;
	uintptr_t dimension_addend;
	uintptr_t dimension_multiplier;
	uintptr_t dimension_divisor;
	uintptr_t size_align;
};

struct media_cell_t {
	uintptr_t cell_size;
	uintptr_t cell_number;
	uintptr_t skip_size;
	uintptr_t used_size;
};

struct media_cell_info_t* media_cell_info_initial(struct media_cell_info_t *restrict info, uintptr_t cell_addend, uintptr_t cell_align, uintptr_t dimension_addend, uintptr_t dimension_multiplier, uintptr_t dimension_divisor, uintptr_t size_align);
uintptr_t media_cell_set(struct media_cell_t *restrict cell, const struct media_cell_info_t *restrict info, uintptr_t cell_size, uintptr_t dimension);

#endif
