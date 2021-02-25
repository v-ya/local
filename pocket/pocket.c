#include "pocket.h"
#include "pocket-verify.h"
#include <rbtree.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define pocket_attr_bits  5

typedef struct pocket_hash_slot_t pocket_hash_slot_t;
struct pocket_hash_slot_t {
	pocket_hash_slot_t *slot;
	pocket_hash_slot_t *next;
	const pocket_attr_t *attr;
	uintptr_t hash;
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
	uint64_t pocket_size;
	pocket_edge_t edge;
	const char *preset_tag_index;
	const char *preset_tag_string;
	pocket_attr_t *attr$begin;
	pocket_attr_t *attr$end;
	refer_t depend;
};

static pocket_header_t* pocket_check_size(const uint8_t *restrict pocket, uint64_t size)
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
	if ((offset = header->system.offset))
	{
		if (offset < edge->index$begin) goto label_fail;
		if (offset >= edge->index$end) goto label_fail;
		if ((offset - edge->index$begin) & 0x1f) goto label_fail;
	}
	if ((offset = header->user.offset))
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

static void pocket_header_magic(pocket_header_t *restrict h, uint64_t start)
{
	#define magic(_)  if (h->_.offset) h->_.offset += start
	magic(verify);
	magic(system);
	magic(user);
	magic(tag);
	magic(version);
	magic(author);
	magic(time);
	magic(description);
	magic(flag);
	#undef magic
}

static void pocket_attr_magic(pocket_attr_t *restrict p, pocket_attr_t *restrict e, uint64_t start)
{
	while (p < e)
	{
		#define magic(_)  if (p->_.offset) p->_.offset += start
		magic(name);
		magic(tag);
		magic(data);
		#undef magic
		++p;
	}
}

static inline uintptr_t pocket_string_hash(register const char *restrict s)
{
	register uintptr_t mix = 0;
	do {
		// mix = mix*11 + mix/4 + *name
		mix += (mix << 3) + (mix >> 2) + (mix << 1) + *(uint8_t *) s;
	} while (*++s);
	return mix;
}

static pocket_hash_slot_t* pocket_attr_hash_block(pocket_hash_slot_t *restrict p, const pocket_attr_t *restrict index, uint64_t n)
{
	pocket_hash_slot_t *restrict v, *restrict s;
	uint64_t m;
	v = p;
	m = n;
	do {
		if (v->attr) goto label_fail;
		v->attr = index;
		if (index->name.string)
		{
			s = p + (v->hash = pocket_string_hash(index->name.string)) % m;
			v->next = s->slot;
			s->slot = v;
		}
		++v;
		++index;
	} while (--n);
	return p;
	label_fail:
	return NULL;
}

static pocket_s* pocket_attr_hash(pocket_s *restrict r, const pocket_attr_t *restrict index)
{
	uint64_t n;
	if ((n = index->size))
	{
		index = (const pocket_attr_t *) index->data.ptr;
		if (!pocket_attr_hash_block(
			r->slots + (((uintptr_t) index - (uintptr_t) r->attr$begin) >> pocket_attr_bits),
			index,
			n)) goto label_fail;
		do {
			if (index->tag.string == r->preset_tag_index)
				pocket_attr_hash(r, index);
			++index;
		} while (--n);
	}
	return r;
	label_fail:
	return NULL;
}

static void pocket_free_func(pocket_s *restrict r)
{
	if (r->slots) free(r->slots);
	rbtree_clear(&r->preset_tag);
	if (r->depend) refer_free(r->depend);
}

static void pocket_free_magic_func(pocket_s *restrict r)
{
	uint64_t magic = -(uint64_t) (uintptr_t) r->pocket;
	pocket_header_magic(r->pocket, magic);
	pocket_attr_magic(r->attr$begin, r->attr$end, magic);
	pocket_free_func(r);
}

static void pocket_free_ntr_func(pocket_s *restrict r)
{
	free(r->pocket);
	pocket_free_func(r);
}

pocket_s* pocket_alloc(uint8_t *restrict pocket, uint64_t size, const struct pocket_verify_s *restrict verify)
{
	pocket_s *restrict r;
	pocket_header_t *h;
	uint64_t magic;
	r = (pocket_s *) refer_alloz(sizeof(pocket_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) pocket_free_func);
		if (!(r->pocket = h = pocket_check_size(pocket, size)))
			goto label_fail;
		r->pocket_size = size;
		r->edge.string$begin = h->string$offset;
		r->edge.string$end = h->string$offset + h->string$size;
		r->edge.index$begin = h->index$offset;
		r->edge.index$end = h->index$offset + h->index$size;
		r->edge.data$begin = h->data$offset;
		r->edge.data$end = h->data$offset + h->data$size;
		if (!pocket_build_preset_tag(r, (const char *) pocket + r->edge.string$begin, (const char *) pocket + r->edge.string$end))
			goto label_fail;
		if (!pocket_check_header(&r->edge, h))
			goto label_fail;
		if (!pocket_check_attr(r))
			goto label_fail;
		r->attr$begin = (pocket_attr_t *) ((uint8_t *) h + r->edge.index$begin);
		r->attr$end = (pocket_attr_t *) ((uint8_t *) h + r->edge.index$end);
		// check verify
		if (verify && h->verify.offset && !pocket_check_verify(verify, pocket, size))
			goto label_fail;
		// magic attr
		r->slots = (pocket_hash_slot_t *) calloc(h->index$size >> pocket_attr_bits, sizeof(pocket_hash_slot_t));
		if (!r->slots) goto label_fail;
		magic = (uint64_t) (uintptr_t) h;
		pocket_header_magic(h, magic);
		pocket_attr_magic(r->attr$begin, r->attr$end, magic);
		refer_set_free(r, (refer_free_f) pocket_free_magic_func);
		if ((!h->system.ptr || pocket_attr_hash(r, (pocket_attr_t *) h->system.ptr)) &&
			(!h->user.ptr || pocket_attr_hash(r, (pocket_attr_t *) h->user.ptr)))
			return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

pocket_s* pocket_load(const char *restrict path, const struct pocket_verify_s *restrict verify)
{
	pocket_s *r;
	FILE *fp;
	r = NULL;
	fp = fopen(path, "rb");
	if (fp)
	{
		size_t n;
		fseek(fp, 0, SEEK_END);
		if ((n = ftell(fp)) >= sizeof(pocket_header_t))
		{
			uint8_t *data;
			data = (uint8_t *) malloc(n);
			if (data)
			{
				fseek(fp, 0, SEEK_SET);
				if (fread(data, 1, n, fp) == n && (r = pocket_alloc(data, n, verify)))
					refer_set_free(r, (refer_free_f) pocket_free_ntr_func);
				else free(data);
			}
		}
		fclose(fp);
	}
	return r;
}

void pocket_set_depend(pocket_s *restrict pocket, refer_t depend)
{
	if (pocket->depend)
		refer_free(pocket->depend);
	pocket->depend = refer_save(depend);
}

uint8_t* pocket_pull(const pocket_s *restrict p, uint64_t *restrict size)
{
	if (refer_get_free(p) == (refer_free_f) pocket_free_ntr_func)
	{
		refer_set_free(p, (refer_free_f) pocket_free_magic_func);
		if (size) *size = p->pocket_size;
		return (uint8_t *) p->pocket;
	}
	return NULL;
}

void pocket_pull_free(uint8_t *restrict pocket)
{
	free(pocket);
}

const pocket_header_t* pocket_header(const pocket_s *restrict p)
{
	return p->pocket;
}

const pocket_attr_t* pocket_system(const pocket_s *restrict p)
{
	return (pocket_attr_t *) p->pocket->system.ptr;
}

const pocket_attr_t* pocket_user(const pocket_s *restrict p)
{
	return (pocket_attr_t *) p->pocket->user.ptr;
}

pocket_tag_t pocket_preset_tag(const pocket_s *restrict p, const pocket_attr_t *restrict attr)
{
	if (attr->tag.string)
	{
		rbtree_t *restrict v;
		if ((v = rbtree_find(&p->preset_tag, NULL, attr->tag.offset)))
			return (pocket_tag_t) (uintptr_t) v->value;
		return pocket_tag$custom;
	}
	return pocket_tag$null;
}

const pocket_attr_t* pocket_is_tag(const pocket_s *restrict p, const pocket_attr_t *restrict attr, pocket_tag_t tag, const char *restrict custom)
{
	pocket_tag_t t;
	if (!attr) goto label_null;
	if ((t = pocket_preset_tag(p, attr)) != tag)
		goto label_null;
	if (tag == pocket_tag$custom && (!custom || strcmp(attr->tag.string, custom)))
		goto label_null;
	return attr;
	label_null:
	return NULL;
}

const pocket_attr_t* pocket_find(const pocket_s *restrict p, const pocket_attr_t *restrict index, const char *restrict name)
{
	if (index->tag.string == p->preset_tag_index && index->size)
	{
		register pocket_hash_slot_t *restrict v;
		uintptr_t hash;
		hash = pocket_string_hash(name);
		v = p->slots + (((uintptr_t) index->data.ptr - (uintptr_t) p->attr$begin) >> pocket_attr_bits) + (hash % index->size);
		v = v->slot;
		while (v)
		{
			if (v->hash == hash && !strcmp((index = v->attr)->name.string, name))
				return index;
			v = v->next;
		}
	}
	return NULL;
}

const pocket_attr_t* pocket_find_path(const pocket_s *restrict p, const pocket_attr_t *restrict index, const char *const restrict *restrict path)
{
	register const char *restrict name;
	while (index && (name = *path++))
		index = pocket_find(p, index, name);
	return index;
}

const pocket_attr_t* pocket_find_tag(const pocket_s *restrict p, const pocket_attr_t *restrict index, const char *restrict name, pocket_tag_t tag, const char *restrict custom)
{
	return pocket_is_tag(p, pocket_find(p, index, name), tag, custom);
}

const pocket_attr_t* pocket_find_path_tag(const pocket_s *restrict p, const pocket_attr_t *restrict index, const char *const restrict *restrict path, pocket_tag_t tag, const char *restrict custom)
{
	return pocket_is_tag(p, pocket_find_path(p, index, path), tag, custom);
}

// orginal pocket data

static const char* pocket_tag2string(pocket_tag_t tag)
{
	static const char *tag2string[pocket_tag$custom] = {
		#define pocket_tag_def(_t)  [pocket_tag$##_t] = pocket_tag_string$##_t
		pocket_tag_def(null),
		pocket_tag_def(index),
		pocket_tag_def(string),
		pocket_tag_def(text),
		pocket_tag_def(u8),
		pocket_tag_def(s8),
		pocket_tag_def(u16),
		pocket_tag_def(s16),
		pocket_tag_def(u32),
		pocket_tag_def(s32),
		pocket_tag_def(u64),
		pocket_tag_def(s64),
		pocket_tag_def(f16),
		pocket_tag_def(f32),
		pocket_tag_def(f64),
		pocket_tag_def(f128),
		pocket_tag_def(largeN),
		#undef pocket_tag_def
	};
	if ((uint32_t) tag < pocket_tag$custom)
		return tag2string[tag];
	return NULL;
}

static void* pocket_find_verify(const uint8_t *restrict pocket, uint64_t size, pocket_tag_t verify_tag, uint64_t verify_size)
{
	const char *restrict tag_string;
	const pocket_header_t *restrict header;
	const pocket_attr_t *restrict attr;
	uint64_t offset;
	if ((tag_string = pocket_tag2string(verify_tag)) && size)
	{
		if ((header = pocket_check_size(pocket, size)) &&
			(offset = header->system.offset) &&
			offset >= header->index$offset &&
			header->index$size >= sizeof(pocket_attr_t) &&
			(offset -= header->index$offset) <= (header->index$size - sizeof(pocket_attr_t)) &&
			!(offset % sizeof(pocket_attr_t)) &&
			(attr = (const pocket_attr_t *) (pocket + header->system.offset))->tag.offset &&
			attr->tag.offset >= header->string$offset &&
			(attr->tag.offset - header->string$offset) < header->string$size &&
			!strcmp((const char *) (pocket + attr->tag.offset), pocket_tag2string(pocket_tag$index)) &&
			(offset = attr->size * sizeof(pocket_attr_t)) &&
			offset < header->index$size &&
			attr->data.offset >= header->index$offset &&
			(attr->data.offset - header->index$offset) <= (header->index$size - offset) &&
			!((attr->data.offset - header->index$offset) % sizeof(pocket_attr_t)))
		{
			offset = attr->size;
			attr = (const pocket_attr_t *) (pocket + attr->data.offset);
			do {
				if (attr->name.offset >= header->string$offset &&
					(attr->name.offset - header->string$offset) < header->string$size &&
					!strcmp((const char *) (pocket + attr->name.offset), pocket_verify_attr_name))
				{
					if (attr->size == verify_size && attr->tag.offset >= header->string$offset &&
						(attr->tag.offset - header->string$offset) < header->string$size &&
						!strcmp((const char *) (pocket + attr->tag.offset), tag_string) &&
						attr->data.offset >= header->data$offset &&
						(attr->data.offset - header->data$offset) <= header->data$size)
						return (void *) (pocket + attr->data.offset);
					break;
				}
				++attr;
			} while (--offset);
		}
	}
	return NULL;
}

const struct pocket_verify_s* pocket_check_verify(const struct pocket_verify_s *restrict verify, const uint8_t *restrict pocket, uint64_t size)
{
	const pocket_header_t *restrict header;
	const pocket_verify_entry_s *restrict entry;
	void *restrict vdata;
	if ((header = pocket_check_size(pocket, size)))
	{
		if (!header->verify.offset) goto label_ok;
		if (header->verify.offset >= header->string$offset &&
			(header->verify.offset - header->string$offset) < header->string$size &&
			(entry = verify->method(verify, (const char *) (pocket + header->verify.offset))) &&
			entry->check && (vdata = pocket_find_verify(pocket, size, entry->tag, entry->size)))
		{
			if (entry->check(entry, vdata, pocket, size))
			{
				label_ok:
				return verify;
			}
		}
	}
	return NULL;
}

const struct pocket_verify_s* pocket_build_verify(const struct pocket_verify_s *restrict verify, uint8_t *restrict pocket, uint64_t size)
{
	const pocket_header_t *restrict header;
	const pocket_verify_entry_s *restrict entry;
	void *restrict vdata;
	if ((header = pocket_check_size(pocket, size)))
	{
		if (!header->verify.offset) goto label_ok;
		if (header->verify.offset >= header->string$offset &&
			(header->verify.offset - header->string$offset) < header->string$size &&
			(entry = verify->method(verify, (const char *) (pocket + header->verify.offset))) &&
			entry->build && (vdata = pocket_find_verify(pocket, size, entry->tag, entry->size)))
		{
			if (entry->build(entry, vdata, pocket, size))
			{
				label_ok:
				return verify;
			}
		}
	}
	return NULL;
}
