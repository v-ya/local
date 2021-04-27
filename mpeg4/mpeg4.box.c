#include "mpeg4.box.h"

#define _size_base    sizeof(mpeg4_box_t)
#define _size_extend  (sizeof(mpeg4_box_t) + sizeof(mpeg4_box_extend_size_t))

uint64_t mpeg4_box_border_parse(const uint8_t *restrict data, uint64_t size, mpeg4_box_extend_size_t *restrict rsize, mpeg4_box_type_t *restrict rtype)
{
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
}

uint64_t mpeg4_box_border_build(uint8_t *restrict data, uint64_t box_size, mpeg4_box_type_t type)
{
	((mpeg4_box_t *) data)->type.v = type.v;
	box_size += _size_base;
	if (box_size <= (uint64_t) 0xffffffff)
	{
		((mpeg4_box_t *) data)->size = mpeg4_n32((uint32_t) box_size);
		return _size_base;
	}
	else
	{
		((mpeg4_box_t *) data)->size = mpeg4_n32(1);
		*(mpeg4_box_extend_size_t *) (data + _size_extend) = mpeg4_n64(box_size + sizeof(mpeg4_box_extend_size_t));
		return _size_extend;
	}
}
