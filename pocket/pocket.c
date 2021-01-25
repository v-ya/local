#include "pocket.h"
#include <rbtree.h>
#include <string.h>
#include <stdlib.h>

typedef struct pocket_hash_slot_t pocket_hash_slot_t;
struct pocket_hash_slot_t {
	pocket_hash_slot_t *slot;
	pocket_hash_slot_t *next;
};

typedef struct pocket_edge_t {
	uint64_t string$begin;
	uint64_t string$end;
	uint64_t index$begin;
	uint64_t index$end;
	uint64_t data$begin;
	uint64_t data$end;
} pocket_edge_t;

struct pocket_s {
	pocket_header_t *pocket;
	pocket_hash_slot_t *slots;
	rbtree_t *preset_tag;
	pocket_edge_t edge;
	const char *preset_tag_index;
	const char *preset_tag_string;
};

static pocket_header_t* pocket_check_size(uint8_t *restrict pocket, uint64_t size)
{
	pocket_header_t *h;
	uint64_t n;
	h = (pocket_header_t *) pocket;
	n = sizeof(pocket_header_t);
	if (size < n) goto label_fail;
	if (h->magic != pocket_magic) goto label_fail;
	// check string
	if (h->string$offset < n) goto label_fail;
	if (h->string$offset > size) goto label_fail;
	if (h->string$size > size - h->string$offset)
		goto label_fail;
	n = h->string$offset + h->string$size;
	if (h->string$size && pocket[n - 1])
		goto label_fail;
	// check index
	if (h->index$offset < n) goto label_fail;
	if (h->index$offset > size) goto label_fail;
	if (h->index$offset & 0xf) goto label_fail;
	if (h->index$size > size - h->index$offset)
		goto label_fail;
	if (h->index$size & 0x1f) goto label_fail;
	n = h->index$offset + h->index$size;
	// check data
	if (h->data$offset < n) goto label_fail;
	if (h->data$offset > size) goto label_fail;
	if (h->data$size > size - h->data$offset)
		goto label_fail;
	return h;
	label_fail:
	return NULL;
}

static inline const pocket_edge_t* pocket_check_string(const pocket_edge_t *restrict edge, const pocket_string_t string)
{
	return (!string.offset || (string.offset >= edge->string$begin && string.offset < edge->string$end))?edge:NULL;
}

static pocket_s* pocket_build_preset_tag(pocket_s *restrict r, const char *restrict string_pool, const char *restrict edge)
{
	typedef struct pocket_preset_tag_t {
		struct pocket_preset_tag_t *next;
		const char *tag;
		uintptr_t id;
	} pocket_preset_tag_t;
	pocket_preset_tag_t ptag[] = {
		#define pocket_preset_tag_def(_t)  {.next = NULL, .tag = pocket_tag_string$##_t, .id = pocket_tag$##_t}
		pocket_preset_tag_def(index),
		pocket_preset_tag_def(string),
		pocket_preset_tag_def(text),
		pocket_preset_tag_def(u8),
		pocket_preset_tag_def(s8),
		pocket_preset_tag_def(u16),
		pocket_preset_tag_def(s16),
		pocket_preset_tag_def(u32),
		pocket_preset_tag_def(s32),
		pocket_preset_tag_def(u64),
		pocket_preset_tag_def(s64),
		pocket_preset_tag_def(f16),
		pocket_preset_tag_def(f32),
		pocket_preset_tag_def(f64),
		pocket_preset_tag_def(f128),
		pocket_preset_tag_def(largeN)
		#undef pocket_preset_tag_def
	};
	pocket_preset_tag_t *ph, *p, **pp;
	uintptr_t i, n;
	n = (sizeof(ptag) / sizeof(pocket_preset_tag_t));
	for (i = 1; i < n; ++i)
		ptag[i - 1].next = ptag + i;
	ph = ptag;
	label_skip:
	if (string_pool < edge)
	{
		for (pp = &ph; (p = *pp); pp = &p->next)
		{
			if (!strcmp(string_pool, p->tag))
			{
				*pp = p->next;
				if (!rbtree_insert(&r->preset_tag, NULL, (uintptr_t) string_pool, (void *) p->id, NULL))
					goto label_fail;
				if (p->id == pocket_tag$index) r->preset_tag_index = string_pool;
				else if (p->id == pocket_tag$string) r->preset_tag_string = string_pool;
				string_pool += strlen(string_pool) + 1;
				goto label_skip;
			}
		}
	}
	return r;
	label_fail:
	return NULL;
}

static pocket_edge_t* pocket_check_header(pocket_edge_t *restrict edge, pocket_header_t *restrict header)
{
	uint64_t offset;
	if (!pocket_check_string(edge, header->verify)) goto label_fail;
	if (!pocket_check_string(edge, header->tag)) goto label_fail;
	if (!pocket_check_string(edge, header->version)) goto label_fail;
	if (!pocket_check_string(edge, header->author)) goto label_fail;
	if (!pocket_check_string(edge, header->time)) goto label_fail;
	if (!pocket_check_string(edge, header->description)) goto label_fail;
	if (!pocket_check_string(edge, header->flag)) goto label_fail;
	if ((offset = header->system))
	{
		if (offset < edge->index$begin) goto label_fail;
		if (offset >= edge->index$end) goto label_fail;
		if ((offset - edge->index$begin) & 0x1f) goto label_fail;
	}
	if ((offset = header->user))
	{
		if (offset < edge->index$begin) goto label_fail;
		if (offset >= edge->index$end) goto label_fail;
		if ((offset - edge->index$begin) & 0x1f) goto label_fail;
	}
	return edge;
	label_fail:
	return NULL;
}

static pocket_s* pocket_check_attr(pocket_s *restrict r)
{
	const pocket_attr_t *restrict attr, *restrict end;
	const char *restrict p, *restrict tag;
	uint64_t n;
	p = (const char *) r->pocket;
	for (attr = (pocket_attr_t *) (p + r->edge.index$begin),
		end = (pocket_attr_t *) (p + r->edge.index$end);
		attr < end;
		++attr)
	{
		if (!pocket_check_string(&r->edge, attr->name)) goto label_fail;
		if (!pocket_check_string(&r->edge, attr->tag)) goto label_fail;
		n = attr->data.offset;
		if (!attr->tag.offset)
		{
			// null
			if (n || attr->size)
				goto label_fail;
		}
		else if ((tag = p + attr->tag.offset) == r->preset_tag_index)
		{
			// index
			if (n)
			{
				if (!attr->size) goto label_fail;
				if (n < r->edge.index$begin) goto label_fail;
				if (n >= r->edge.index$end) goto label_fail;
				if ((n - r->edge.index$begin) & 0x1f) goto label_fail;
				if ((const char *) attr >= p + n) goto label_fail;
			}
			else if (attr->size) goto label_fail;
		}
		else if (tag == r->preset_tag_string)
		{
			// string
			if ((n && (n < r->edge.string$begin || n >= r->edge.string$end)) || attr->size)
				goto label_fail;
		}
		else
		{
			// data
			if (n)
			{
				if (!attr->size) goto label_fail;
				if (n < r->edge.data$begin) goto label_fail;
				if (n >= r->edge.data$end) goto label_fail;
				if (attr->size > r->edge.data$end - n) goto label_fail;
			}
			else if (attr->size) goto label_fail;
		}
	}
	return r;
	label_fail:
	return NULL;
}

static void pocket_attr_magic(pocket_attr_t *restrict p, pocket_attr_t *restrict e, uint64_t start)
{
	while (p < e)
	{
		if (p->name.offset) p->name.offset += start;
		if (p->tag.offset) p->tag.offset += start;
		if (p->data.offset) p->data.offset += start;
		++p;
	}
}

static void pocket_free_func(pocket_s *restrict r)
{
	if (r->slots) free(r->slots);
	rbtree_clear(&r->preset_tag);
}

static void pocket_free_magic_func(pocket_s *restrict r)
{
	pocket_attr_magic((pocket_attr_t *) ((uint8_t *) r->pocket + r->edge.index$begin),
			(pocket_attr_t *) ((uint8_t *) r->pocket + r->edge.index$end),
			-(uint64_t) (uintptr_t) r->pocket);
	pocket_free_func(r);
}

static void pocket_free_ntr_func(pocket_s *restrict r)
{
	free(r->pocket);
	pocket_free_func(r);
}

pocket_s* pocket_alloc(uint8_t *restrict pocket, uint64_t size)
{
	pocket_s *restrict r;
	pocket_header_t *h;
	r = (pocket_s *) refer_alloz(sizeof(pocket_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) pocket_free_func);
		if (!(r->pocket = h = pocket_check_size(pocket, size)))
			goto label_fail;
		r->edge.string$begin = h->string$offset;
		r->edge.string$end = h->string$offset + h->string$size;
		r->edge.index$begin = h->index$offset;
		r->edge.index$end = h->index$offset + h->index$size;
		r->edge.data$begin = h->data$offset;
		r->edge.data$end = h->data$offset + h->data$size;
		if (!pocket_build_preset_tag(r, (const char *) pocket + r->edge.string$begin, (const char *) pocket + r->edge.string$end))
			goto label_fail;
		if (!pocket_check_header(&r->edge, r->pocket))
			goto label_fail;
		if (!pocket_check_attr(r))
			goto label_fail;
		// check verify
		// magic attr
		r->slots = (pocket_hash_slot_t *) calloc(r->pocket->index$size >> 5, sizeof(pocket_hash_slot_t));
		if (!r->slots) goto label_fail;
		pocket_attr_magic((pocket_attr_t *) ((uint8_t *) r->pocket + r->edge.index$begin),
				(pocket_attr_t *) ((uint8_t *) r->pocket + r->edge.index$end),
				(uint64_t) (uintptr_t) r->pocket);
		refer_set_free(r, (refer_free_f) pocket_free_magic_func);
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}
