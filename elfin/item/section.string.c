#include "section.h"
#include <exbuffer.h>
#include <hashmap.h>
#include <rbtree.h>
#include <string.h>

struct elfin_item__section_string_s {
	struct elfin_item_section_s section;
	exbuffer_t string_map;
	hashmap_t string2index;   // =string> ((uintptr_t) string_index)
	rbtree_t *string_builder; // =(~length)> ((hashmap_t *) =string> ((uintptr_t) length))
};

static void inner_string_builder_free_func(rbtree_t *restrict rbv)
{
	if (rbv->value)
		hashmap_free((hashmap_t *) rbv->value);
}

static struct elfin_item__section_string_s* inner_push_string_builder(struct elfin_item__section_string_s *restrict r, const char *restrict string, uintptr_t length)
{
	rbtree_t *restrict inode;
	hashmap_t *restrict hm;
	if ((inode = rbtree_find(&r->string_builder, NULL, ~(uint64_t) length)))
	{
		hm = (hashmap_t *) inode->value;
		label_put:
		if (hashmap_put_name(hm, string, (const void *) (uintptr_t) length, NULL))
			return r;
	}
	else if ((hm = hashmap_alloc()))
	{
		if (rbtree_insert(&r->string_builder, NULL, ~(uint64_t) length, hm, inner_string_builder_free_func))
			goto label_put;
		hashmap_free(hm);
	}
	return NULL;
}

static struct elfin_item__section_string_s* inner_insert_string_index(struct elfin_item__section_string_s *restrict r, const char *restrict string, uintptr_t length)
{
	uintptr_t start;
	start = (uintptr_t) r->string_map.data;
	goto label_entry;
	do {
		++string;
		--length;
		label_entry:
		if (!hashmap_put_name(&r->string2index, string, (const void *) ((uintptr_t) string - start), NULL))
			return NULL;
	} while (length);
	return r;
}

static struct elfin_item__section_string_s* inner_insert_string_by_block(struct elfin_item__section_string_s *restrict r, const char *restrict block, uintptr_t size)
{
	char *restrict p;
	uintptr_t length;
	while (size)
	{
		p = memchr(block, 0, size);
		length = (uintptr_t) p - (uintptr_t) block;
		if (!inner_push_string_builder(r, block, length))
			goto label_fail;
		if (!inner_insert_string_index(r, block, length))
			goto label_fail;
		length += 1;
		block += length;
		size -= length;
		while (size && !*block) ++block, --size;
	}
	return r;
	label_fail:
	return NULL;
}

static void inner_build_string_map_func_func(hashmap_vlist_t *restrict vl, struct elfin_item__section_string_s *restrict *restrict pr)
{
	struct elfin_item__section_string_s *restrict r;
	uintptr_t pos;
	if ((r = *pr) && !hashmap_find_name(&r->string2index, vl->name))
	{
		pos = r->string_map.used;
		if (!exbuffer_append(&r->string_map, vl->name, (uintptr_t) vl->value + 1) ||
			!inner_insert_string_index(r, (const char *) r->string_map.data + pos, (uintptr_t) vl->value))
			*pr = NULL;
	}
}

static void inner_build_string_map_func(rbtree_t *restrict inode, struct elfin_item__section_string_s *restrict *restrict pr)
{
	if (*pr) hashmap_call((hashmap_t *) inode->value, (hashmap_func_call_f) inner_build_string_map_func_func, (void *) pr);
}

static struct elfin_item__section_string_s* inner_build_string_map(struct elfin_item__section_string_s *restrict r)
{
	hashmap_clear(&r->string2index);
	rbtree_call(&r->string_builder, (rbtree_func_call_f) inner_build_string_map_func, (void *) &r);
	return r;
}

#define r  ((struct elfin_item__section_string_s *) item)

static struct elfin_item_s* get_section_size(struct elfin_item_s *restrict item, uint64_t *restrict size)
{
	*size = r->section.size;
	return item;
}

static struct elfin_item_s* clear_section_data(struct elfin_item_s *restrict item)
{
	r->section.size = 0;
	r->string_map.used = 0;
	hashmap_clear(&r->string2index);
	rbtree_clear(&r->string_builder);
	return item;
}

static struct elfin_item_s* append_section_data(struct elfin_item_s *restrict item, const void *data, uintptr_t size, uintptr_t align, uint64_t *restrict offset)
{
	uintptr_t pos;
	pos = r->string_map.used;
	if (offset) *offset = pos;
	if (size)
	{
		if (!exbuffer_append(&r->string_map, data, size))
			goto label_fail;
		if (r->string_map.data[r->string_map.used - 1])
		{
			if (!exbuffer_need(&r->string_map, r->string_map.used + 1))
				goto label_fail;
			r->string_map.data[r->string_map.used++] = 0;
		}
		if (!inner_insert_string_by_block(r, (const char *) r->string_map.data + pos, r->string_map.used - pos))
			goto label_fail;
		r->section.size = r->string_map.used;
	}
	return item;
	label_fail:
	r->section.size = r->string_map.used;
	return NULL;
}

static struct elfin_item_s* get_section_data(struct elfin_item_s *restrict item, void **restrict p_data, uintptr_t *restrict size)
{
	*p_data = (void *) r->string_map.data;
	*size = r->string_map.used;
	return item;
}

static struct elfin_item_s* get_string_by_index(struct elfin_item_s *restrict item, uintptr_t string_index, const char **restrict p_string)
{
	if (string_index < r->string_map.used)
	{
		*p_string = (const char *) r->string_map.data + string_index;
		return item;
	}
	return NULL;
}

static struct elfin_item_s* put_string_with_length(struct elfin_item_s *restrict item, const char *restrict string, uintptr_t length)
{
	if (inner_push_string_builder(r, string, length))
		return item;
	return NULL;
}

static struct elfin_item_s* put_string(struct elfin_item_s *restrict item, const char *restrict string)
{
	if (inner_push_string_builder(r, string, strlen(string)))
		return item;
	return NULL;
}

static struct elfin_item_s* clear_string(struct elfin_item_s *restrict item)
{
	rbtree_clear(&r->string_builder);
	return item;
}

static struct elfin_item_s* build_string_map(struct elfin_item_s *restrict item)
{
	r->section.size = 0;
	r->string_map.used = 0;
	if (inner_build_string_map(r))
	{
		r->section.size = r->string_map.used;
		return item;
	}
	r->section.size = r->string_map.used;
	return NULL;
}

static struct elfin_item_s* get_string_index(struct elfin_item_s *restrict item, const char *restrict string, uintptr_t *restrict string_index)
{
	hashmap_vlist_t *restrict vl;
	if ((vl = hashmap_find_name(&r->string2index, string)))
	{
		*string_index = (uintptr_t) vl->value;
		return item;
	}
	return NULL;
}

#undef r

static void elfin_item__section_string_free_func(struct elfin_item__section_string_s *restrict r)
{
	exbuffer_uini(&r->string_map);
	hashmap_uini(&r->string2index);
	rbtree_clear(&r->string_builder);
	elfin_inner_clear_item_section(&r->section);
}

static struct elfin_item_s* elfin_item__section_string_alloc(const struct elfin_item_inst_s *restrict inst)
{
	struct elfin_item__section_string_s *restrict r;
	if ((r = (struct elfin_item__section_string_s *) refer_alloz(sizeof(struct elfin_item__section_string_s))))
	{
		refer_set_free(r, (refer_free_f) elfin_item__section_string_free_func);
		if (exbuffer_init(&r->string_map, 0) && hashmap_init(&r->string2index))
		{
			r->section.type = elfin_section_type__string_table;
			r->section.flags = 0;
			r->section.alignment = 1;
			r->section.entry_size = 0;
			return elfin_inner_link_item_section(&r->section, inst);
		}
		refer_free(r);
	}
	return NULL;
}

#include "../elfin.h"

struct elfin_item_inst_s* elfin_inner_alloc_item_inst__section_string(void)
{
	struct elfin_item_inst_s *restrict r;
	if ((r = (struct elfin_item_inst_s *) refer_alloz(sizeof(struct elfin_item_inst_s))))
	{
		r->name = elfin_name_section_string;
		r->alloc = elfin_item__section_string_alloc;
		elfin_inner_init_item_section_inst(r);
		#define d_func(_id) r->func._id = _id
		d_func(get_section_size);
		d_func(clear_section_data);
		d_func(append_section_data);
		d_func(get_section_data);
		d_func(get_string_by_index);
		d_func(put_string);
		d_func(put_string_with_length);
		d_func(clear_string);
		d_func(build_string_map);
		d_func(get_string_index);
		#undef d_func
	}
	return r;
}
