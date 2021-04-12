#include "mpeg4.box.h"

uint64_t mpeg4_box_border_parse(const uint8_t *restrict data, uint64_t size, mpeg4_box_extend_size_t *restrict rsize, mpeg4_box_type_t *restrict rtype)
{
	#define _size_base    sizeof(mpeg4_box_t)
	#define _size_extend  (sizeof(mpeg4_box_t) + sizeof(mpeg4_box_extend_size_t))
	if (size < _size_base)
		goto label_fail;
	rtype->v = ((const mpeg4_box_t *) data)->type.v;
	switch ((*rsize = (uint64_t) mpeg4_n32(((const mpeg4_box_t *) data)->size)))
	{
		case 1:
			if (size < _size_extend)
				goto label_fail;
			if ((*rsize = mpeg4_n64(*((const mpeg4_box_extend_size_t *) (data + _size_base)))) < _size_extend || *rsize > size)
				goto label_fail;
			return _size_extend;
		case 0:
			*rsize = size;
			// fall through
		default:
			if (*rsize < _size_base || *rsize > size)
				goto label_fail;
			return _size_base;
	}
	label_fail:
	return 0;
	#undef _size_base
	#undef _size_extend
}
