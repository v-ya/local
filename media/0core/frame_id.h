#ifndef _media_core_frame_id_h_
#define _media_core_frame_id_h_

#include <refer.h>
#include "cell.h"
#include "channel.h"

struct media_frame_id_s {
	refer_string_t name;
	refer_string_t compat;
	uintptr_t channel;
	uintptr_t dimension;
	struct media_cell_info_t cell[];
};

struct media_frame_id_s* media_frame_id_alloc(const char *restrict name, uintptr_t channel, uintptr_t dimension);
struct media_frame_id_s* media_frame_id_initial(struct media_frame_id_s *restrict frame_id, uintptr_t channel_index, uintptr_t dimension, const struct media_cell_info_t *restrict params);

const struct media_cell_info_t* media_frame_id_get_cells(const struct media_frame_id_s *restrict frame_id, uintptr_t channel_index, uintptr_t dimension);

#define d_media_ci__cell(_a, _d, _m, _p)  \
	.cellsize_addend      = _a,\
	.cellsize_divisor     = _d,\
	.cellsize_multiplier  = _m,\
	.cellsize_plus        = _p
#define d_media_ci__dimension(_a, _d, _m, _p)  \
	.dimension_addend     = _a,\
	.dimension_divisor    = _d,\
	.dimension_multiplier = _m,\
	.dimension_plus       = _p
#define d_media_ci__cv(_a)  d_media_ci__cell(_a, 1, 1, 0)
#define d_media_ci__dv()    d_media_ci__dimension(0, 1, 1, 0)
#define d_media_ci__d1()    d_media_ci__dimension(0, 1, 0, 1)
#define d_media_ci__dd(_d)  d_media_ci__dimension((_d - 1), _d, 1, 0)

#define d_media_ci__inext()    { d_media_ci__cv(0), d_media_ci__dv() }
#define d_media_ci__ikeep()    { d_media_ci__cv(0), d_media_ci__d1() }
#define d_media_ci__ibase(_a)  { d_media_ci__cv(_a), d_media_ci__dv() }
#define d_media_ci__iplus(_a)  { d_media_ci__cv(_a), d_media_ci__d1() }

#define d_media_ci__inext_with_align(_align)   { d_media_ci__cell((_align - 1), _align, _align, 0), d_media_ci__dv() }
#define d_media_ci__inext_with_sink(_dd)       { d_media_ci__cv(0), d_media_ci__dd(_dd) }
#define d_media_ci__ibase_with_sink(_a, _dd)   { d_media_ci__cv(_a), d_media_ci__dd(_dd) }

#endif
