#include "box.include.h"
#include "inner.bindata.h"
#include "inner.data.h"
#include <stdlib.h>
#include <memory.h>

#define media_data_chip_level  24
#define media_data_chip_size   (1ul << media_data_chip_level)
#define media_data_chip_mask   (media_data_chip_size - 1)

typedef struct mpeg4_stuff__media_data_t {
	inner_array_t data;
	uint64_t size;
	uint64_t offset;
	const void *parse_data;
	uint64_t parse_size;
} mpeg4_stuff__media_data_t;

typedef struct mpeg4_stuff__media_data_s {
	mpeg4_stuff_t stuff;
	mpeg4_stuff__media_data_t pri;
} mpeg4_stuff__media_data_s;

static mpeg4_stuff__media_data_s* mpeg4$define(inner, media_data, calc_offset)(mpeg4_stuff__media_data_s *r)
{
	mpeg4_stuff_t *p;
	uint64_t offset;
	r->pri.offset = offset = 0;
	if ((p = r->stuff.link.container))
	{
		p = p->container.list;
		while (p && p != &r->stuff)
		{
			if (!p->info.calc_okay)
			{
				if (!p->atom->interface.calc)
					goto label_fail;
				if (!p->atom->interface.calc(p))
					goto label_fail;
			}
			offset += p->info.all_size;
			p = p->link.next;
		}
	}
	r->pri.offset = offset;
	return r;
	label_fail:
	return NULL;
}

static uint64_t mpeg4$define(inner, media_data, get_data)(const mpeg4_stuff__media_data_t *restrict r, uint8_t *restrict data, uint64_t offset, uint64_t size)
{
	uint64_t n, ret;
	if (offset < (n = r->size))
	{
		if (size > n - offset)
			size = n - offset;
		if ((ret = size))
		{
			if ((n = offset & media_data_chip_mask))
			{
				n = media_data_chip_size - n;
				if (n > size) n = size;
				memcpy(data, ((uint8_t **) r->data.array)[offset >> media_data_chip_level] + (offset & media_data_chip_mask), n);
				if (!(size -= n))
					goto label_ok;
				data += n;
				offset += n;
			}
			n = size & media_data_chip_mask;
			offset >>= media_data_chip_level;
			size >>= media_data_chip_level;
			while (offset < size)
			{
				memcpy(data, ((uint8_t **) r->data.array)[offset], media_data_chip_size);
				data += media_data_chip_size;
				++offset;
			}
			if (n) memcpy(data, ((uint8_t **) r->data.array)[offset], n);
			label_ok:
			return ret;
		}
	}
	return 0;
}

static mpeg4_stuff__media_data_t* mpeg4$define(inner, media_data, need_data)(mpeg4_stuff__media_data_t *restrict r, uint64_t size)
{
	uint8_t **p;
	uintptr_t i, n;
	i = (r->size + media_data_chip_mask) >> media_data_chip_level;
	n = (size + media_data_chip_mask) >> media_data_chip_level;
	if (n > r->data.number && !mpeg4$define(inner, array, append_point)(&r->data, n - r->data.number))
		goto label_fail;
	p = (uint8_t **) r->data.array + i;
	while (i < n)
	{
		if (!p[i])
		{
			if (!(p[i] = (uint8_t *) malloc(media_data_chip_size)))
				goto label_fail;
		}
		++i;
	}
	return r;
	label_fail:
	return NULL;
}

static mpeg4_stuff__media_data_t* mpeg4$define(inner, media_data, add_data)(mpeg4_stuff__media_data_t *restrict r, const uint8_t *restrict data, uint64_t size)
{
	uint64_t n, i;
	if (!mpeg4$define(inner, media_data, need_data)(r, r->size + size))
		goto label_fail;
	i = r->size >> media_data_chip_level;
	if ((n = r->size & media_data_chip_mask))
	{
		n = media_data_chip_size - n;
		if (n > size) n = size;
		memcpy(((uint8_t **) r->data.array)[i++] + (r->size & media_data_chip_mask), data, n);
		data += n;
	}
	n = size - n;
	while (n)
	{
		if (n >= media_data_chip_size)
		{
			memcpy(((uint8_t **) r->data.array)[i], data, media_data_chip_size);
			data += media_data_chip_size;
			n -= media_data_chip_size;
		}
		else
		{
			memcpy(((uint8_t **) r->data.array)[i], data, n);
			break;
		}
		++i;
	}
	r->size += size;
	return r;
	label_fail:
	return NULL;
}

static mpeg4$define$dump(mdat)
{
	#define mlevel(_n)  (1.0 / (1u << _n))
	uint32_t level = unidata->dump_level;
	mlog_level_dump("media data size: %lu B = %.1f KiB = %.2f MiB = %.2f GiB\n", size, size * mlevel(10), size * mlevel(20), size * mlevel(30));
	return atom;
	#undef mlevel
}

static mpeg4$define$stuff$init(mdat, mpeg4_stuff__media_data_s)
{
	mpeg4$define(inner, array, init)(&r->pri.data, sizeof(uint8_t *));
	return &r->stuff;
}

static mpeg4$define$stuff$free(mdat, mpeg4_stuff__media_data_s)
{
	uint8_t **p;
	uintptr_t i, n;
	if ((p = (uint8_t **) r->pri.data.array))
	{
		for (i = 0, n = r->pri.data.number; i < n; ++i)
		{
			if (p[i]) free(p[i]);
		}
		mpeg4$define(inner, array, clear)(&r->pri.data);
	}
	mpeg4_stuff_free_default_func(&r->stuff);
}

static mpeg4$define$create(mdat)
{
	return mpeg4_stuff_alloc(atom, inst, type,
		sizeof(mpeg4_stuff__media_data_s),
		(mpeg4_stuff_init_f) mpeg4$define(stuff, mdat, init),
		(refer_free_f) mpeg4$define(stuff, mdat, free));
}

static mpeg4$define$parse(mdat)
{
	mpeg4_stuff_t *restrict root;
	if (!mpeg4$stuff$method$call(stuff, add$data, data, size, NULL))
		goto label_fail;
	if (!mpeg4$stuff$method$call(stuff, inner$set_parse, data, size))
		goto label_fail;
	if (!(root = stuff->link.container))
		goto label_fail;
	while (root->link.container)
		root = root->link.container;
	if (!mpeg4$stuff$method$call(root, inner$push_mdat, stuff))
		goto label_fail;
	return stuff;
	label_fail:
	return NULL;
}

static mpeg4$define$calc(mdat)
{
	return mpeg4_stuff_calc_okay(stuff, ((mpeg4_stuff__media_data_s *) stuff)->pri.size);
}

static mpeg4$define$build(mdat)
{
	mpeg4_stuff__media_data_t *restrict r = &((mpeg4_stuff__media_data_s *) stuff)->pri;
	if (mpeg4$define(inner, media_data, get_data)(r, data, 0, r->size) == r->size)
		return stuff;
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, mdat, add$data)(mpeg4_stuff__media_data_s *restrict r, const void *data, uint64_t size, uint64_t *restrict offset)
{
	if (offset) *offset = r->pri.size;
	if (!size || mpeg4$define(inner, media_data, add_data)(&r->pri, (const uint8_t *) data, size))
		return &r->stuff;
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, mdat, get$data)(mpeg4_stuff__media_data_s *restrict r, uint64_t offset, void *data, uint64_t size, uint64_t *restrict rsize)
{
	uint64_t n;
	n = 0;
	if (size)
		n = mpeg4$define(inner, media_data, get_data)(&r->pri, data, offset, size);
	else if (r->pri.size > offset)
		n = r->pri.size - offset;
	if (rsize) *rsize = n;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, mdat, calc$offset)(mpeg4_stuff__media_data_s *restrict r, uint64_t *restrict offset)
{
	if (!r->stuff.info.calc_okay)
	{
		if (!r->stuff.atom->interface.calc)
			goto label_fail;
		if (!r->stuff.atom->interface.calc(&r->stuff))
			goto label_fail;
	}
	if (!mpeg4$define(inner, media_data, calc_offset)(r))
		goto label_fail;
	if (offset)
		*offset = r->pri.offset + (r->stuff.info.all_size - r->stuff.info.inner_size);
	return &r->stuff;
	label_fail:
	return NULL;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, mdat, inner$set_parse)(mpeg4_stuff__media_data_s *restrict r, const void *data, uint64_t size)
{
	r->pri.parse_data = data;
	r->pri.parse_size = size;
	return &r->stuff;
}

static const mpeg4_stuff_t* mpeg4$define(stuff, mdat, inner$do_parse_mdat)(mpeg4_stuff__media_data_s *restrict r, const void **restrict p_data, uint64_t *restrict p_size)
{
	if (p_data) *p_data = r->pri.parse_data;
	if (p_size) *p_size = r->pri.parse_size;
	r->pri.parse_data = NULL;
	r->pri.parse_size = 0;
	return &r->stuff;
}

static mpeg4$define$link_update(mdat)
{
	if (!last_container)
		last_container = stuff->link.container;
	if (last_container)
	{
		while (last_container->link.container)
			last_container = last_container->link.container;
		mpeg4_stuff_calc_invalid_tree(last_container);
	}
}

static const mpeg4$define$alloc(mdat)
{
	mpeg4_atom_s *restrict r;
	r = mpeg4_atom_alloc_empty();
	if (r)
	{
		r->interface.dump = mpeg4$define(atom, mdat, dump);
		r->interface.create = mpeg4$define(atom, mdat, create);
		r->interface.parse = mpeg4$define(atom, mdat, parse);
		r->interface.calc = mpeg4$define(atom, mdat, calc);
		r->interface.build = mpeg4$define(atom, mdat, build);
		r->interface.link_update = mpeg4$define(atom, mdat, link_update);
		if (
			mpeg4$stuff$method$set(r, mdat, add$data) &&
			mpeg4$stuff$method$set(r, mdat, get$data) &&
			mpeg4$stuff$method$set(r, mdat, calc$offset) &&
			mpeg4$stuff$method$set(r, mdat, inner$set_parse) &&
			mpeg4$stuff$method$set(r, mdat, inner$do_parse_mdat)
		) return r;
		refer_free(r);
	}
	return NULL;
}

mpeg4$define$find(mdat)
{
	static const mpeg4_box_type_t type = { .c = "mdat" };
	return mpeg4_find_atom(inst, mpeg4$define(atom, mdat, alloc), type.v, 0);
}
