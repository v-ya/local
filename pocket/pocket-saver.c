#include "pocket-saver.h"
#include "pocket-verify.h"
#include <hashmap.h>
#include <rbtree.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct pocket_saver_attr_t pocket_saver_attr_t;
typedef struct pocket_saver_attr_index_t pocket_saver_attr_index_t;
typedef struct pocket_saver_attr_data_t pocket_saver_attr_data_t;

struct pocket_saver_attr_t {
	pocket_attr_t attr;
	pocket_tag_t tag;
	rbtree_t *restrict *restrict string_pool_list;
	pocket_saver_attr_t **pr;
	pocket_saver_attr_t *next;
	uint64_t attr_offset;
};

struct pocket_saver_attr_index_t {
	pocket_saver_attr_t attr;
	hashmap_t pool;
	pocket_saver_attr_t *head;
	pocket_saver_attr_t **ptail;
};

struct pocket_saver_attr_data_t {
	pocket_saver_attr_t attr;
	void *data;
	uintptr_t align;
	pocket_saver_attr_data_t **pr;
	pocket_saver_attr_data_t *next;
};

struct pocket_saver_s {
	rbtree_t *string_pool_list;
	rbtree_t *data_align_list;
	pocket_saver_attr_index_t *system;
	pocket_saver_attr_index_t *user;
	const char *verify;
	const char *tag;
	const char *version;
	const char *author;
	const char *time;
	const char *description;
	const char *flag;
};

static void string_pool_list_free_func(rbtree_t *restrict v)
{
	if (v->value)
		hashmap_free((hashmap_t *) v->value);
}

static const char* string_pool_list_link(rbtree_t *restrict *restrict string_pool_list, const char *string, uint64_t n)
{
	rbtree_t *v;
	hashmap_t *value;
	hashmap_vlist_t *vl;
	n = ~n;
	if ((v = rbtree_find(string_pool_list, NULL, n)))
	{
		value = (hashmap_t *) v->value;
		label_link:
		if ((vl = hashmap_put_name(value, string, NULL, NULL)))
		{
			vl->value = (void *) ((uintptr_t) vl->value + 1);
			return vl->name;
		}
	}
	else if ((value = hashmap_alloc()))
	{
		if ((v = rbtree_insert(string_pool_list, NULL, n, value, string_pool_list_free_func)))
			goto label_link;
		hashmap_free(value);
	}
	return NULL;
}

static void string_pool_list_unlink(rbtree_t *restrict *restrict string_pool_list, const char *string)
{
	rbtree_t *v;
	hashmap_vlist_t *vl;
	uint64_t n;
	if (*string == '@') n = 0;
	else n = ~(uint64_t) strlen(string);
	if ((v = rbtree_find(string_pool_list, NULL, n)) && (vl = hashmap_find_name((hashmap_t *) v->value, string)))
	{
		if (!(vl->value = (void *) ((uintptr_t) vl->value - 1)))
			hashmap_delete_name((hashmap_t *) v->value, string);
	}
}

static void pocket_saver_attr_index_free(pocket_saver_attr_index_t *restrict r)
{
	if (r->attr.pr)
	{
		if ((*r->attr.pr = r->attr.next))
			r->attr.next->pr = r->attr.pr;
	}
	if (r->attr.attr.name.string)
		string_pool_list_unlink(r->attr.string_pool_list, r->attr.attr.name.string);
	string_pool_list_unlink(r->attr.string_pool_list, r->attr.attr.tag.string);
	hashmap_uini(&r->pool);
	free(r);
}

static pocket_saver_attr_index_t* pocket_saver_attr_index_alloc(const char *restrict name, rbtree_t *restrict *restrict string_pool_list)
{
	pocket_saver_attr_index_t *r;
	r = (pocket_saver_attr_index_t *) calloc(1, sizeof(pocket_saver_attr_index_t));
	if (r)
	{
		if (hashmap_init(&r->pool))
		{
			if (!name || (r->attr.attr.name.string = string_pool_list_link(string_pool_list, name, strlen(name))))
			{
				if ((r->attr.attr.tag.string = string_pool_list_link(string_pool_list, pocket_tag_string$index, ~(uint64_t) 0)))
				{
					r->attr.tag = pocket_tag$index;
					r->attr.string_pool_list = string_pool_list;
					r->ptail = &r->head;
					return r;
				}
				if (name) string_pool_list_unlink(string_pool_list, name);
			}
			hashmap_uini(&r->pool);
		}
		free(r);
	}
	return NULL;
}

static void pocket_saver_attr_data_free(pocket_saver_attr_data_t *restrict r)
{
	if (r->attr.pr)
	{
		if ((*r->attr.pr = r->attr.next))
			r->attr.next->pr = r->attr.pr;
	}
	if (r->pr)
	{
		if ((*r->pr = r->next))
			r->next->pr = r->pr;
	}
	string_pool_list_unlink(r->attr.string_pool_list, r->attr.attr.name.string);
	if (r->attr.attr.tag.string)
		string_pool_list_unlink(r->attr.string_pool_list, r->attr.attr.tag.string);
	if (r->attr.tag == pocket_tag$string)
		string_pool_list_unlink(r->attr.string_pool_list, (const char *) r->data);
	else if (r->data) free(r->data);
	free(r);
}

static pocket_saver_attr_data_t* pocket_saver_attr_data_alloc(const char *restrict name, rbtree_t *restrict *restrict string_pool_list, pocket_tag_t tag, const char *tag_string, const void *data, uint64_t size, uintptr_t align)
{
	static const char *const tag_string_array[pocket_tag$custom] = {
		[pocket_tag$null] = NULL,
		[pocket_tag$index] = NULL,
		[pocket_tag$string] = pocket_tag_string$string,
		[pocket_tag$text] = pocket_tag_string$text,
		[pocket_tag$u8] = pocket_tag_string$u8,
		[pocket_tag$s8] = pocket_tag_string$s8,
		[pocket_tag$u16] = pocket_tag_string$u16,
		[pocket_tag$s16] = pocket_tag_string$s16,
		[pocket_tag$u32] = pocket_tag_string$u32,
		[pocket_tag$s32] = pocket_tag_string$s32,
		[pocket_tag$u64] = pocket_tag_string$u64,
		[pocket_tag$s64] = pocket_tag_string$s64,
		[pocket_tag$f16] = pocket_tag_string$f16,
		[pocket_tag$f32] = pocket_tag_string$f32,
		[pocket_tag$f64] = pocket_tag_string$f64,
		[pocket_tag$f128] = pocket_tag_string$f128,
		[pocket_tag$largeN] = pocket_tag_string$largeN
	};
	static const uintptr_t align_array[pocket_tag$custom + 1] = {
		[pocket_tag$null] = 0,
		[pocket_tag$index] = 0,
		[pocket_tag$string] = 0,
		[pocket_tag$text] = 1,
		[pocket_tag$u8] = 1,
		[pocket_tag$s8] = 1,
		[pocket_tag$u16] = 2,
		[pocket_tag$s16] = 2,
		[pocket_tag$u32] = 4,
		[pocket_tag$s32] = 4,
		[pocket_tag$u64] = 8,
		[pocket_tag$s64] = 8,
		[pocket_tag$f16] = 2,
		[pocket_tag$f32] = 4,
		[pocket_tag$f64] = 8,
		[pocket_tag$f128] = 16,
		[pocket_tag$largeN] = 16,
		[pocket_tag$custom] = 16
	};
	pocket_saver_attr_data_t *r;
	uint64_t n = ~(uint64_t) 0;
	if ((uint32_t) tag > pocket_tag$custom) goto label_fail;
	if (!align) align = align_array[tag];
	if (tag == pocket_tag$null) size = 0;
	else if (tag == pocket_tag$index) goto label_fail;
	else if (tag == pocket_tag$string)
	{
		if (!data) goto label_fail;
		tag_string = tag_string_array[pocket_tag$string];
		size = 0;
		align = 0;
	}
	else if (tag == pocket_tag$custom)
	{
		if (!tag_string || *tag_string == '@')
			goto label_fail;
		n = strlen(tag_string);
	}
	else tag_string = tag_string_array[tag];
	r = (pocket_saver_attr_data_t *) calloc(1, sizeof(pocket_saver_attr_data_t));
	if (r)
	{
		if (!size || (r->data = malloc(size)))
		{
			if (size)
			{
				if (data) memcpy(r->data, data, size);
				else memset(r->data, 0, size);
			}
			if ((r->attr.attr.name.string = string_pool_list_link(string_pool_list, name, strlen(name))))
			{
				if (!tag_string || (r->attr.attr.tag.string = string_pool_list_link(string_pool_list, tag_string, n)))
				{
					if (tag != pocket_tag$string || (r->data = (void *) string_pool_list_link(string_pool_list, (const char *) data, strlen((const char *) data))))
					{
						r->attr.attr.size = size;
						r->attr.tag = tag;
						r->attr.string_pool_list = string_pool_list;
						r->align = align;
						return r;
					}
					if (tag_string) string_pool_list_unlink(string_pool_list, tag_string);
				}
				string_pool_list_unlink(string_pool_list, name);
			}
			if (size) free(r->data);
		}
		free(r);
	}
	label_fail:
	return NULL;
}

static void pocket_saver_attr_index_delete(pocket_saver_attr_index_t *restrict index, const char *restrict name)
{
	pocket_saver_attr_t *attr;
	if ((attr = (pocket_saver_attr_t *) hashmap_get_name(&index->pool, name)))
	{
		if (index->ptail == &attr->next)
			index->ptail = attr->pr;
		hashmap_delete_name(&index->pool, name);
	}
}

static inline void pocket_saver_attr_index_link_attr(pocket_saver_attr_index_t *restrict index, pocket_saver_attr_t *restrict attr)
{
	attr->pr = index->ptail;
	*index->ptail = attr;
	index->ptail = &attr->next;
}

static void pocket_saver_attr_index_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		pocket_saver_attr_index_free((pocket_saver_attr_index_t *) vl->value);
}

static pocket_saver_attr_index_t* pocket_saver_attr_index_find_index(pocket_saver_attr_index_t *restrict index, const char *restrict name)
{
	pocket_saver_attr_index_t *v;
	if ((v = (pocket_saver_attr_index_t *) hashmap_get_name(&index->pool, name)))
	{
		if (v->attr.tag == pocket_tag$index)
			return v;
	}
	return NULL;
}

static pocket_saver_attr_index_t* pocket_saver_attr_index_insert_index(pocket_saver_attr_index_t *restrict index, const char *restrict name)
{
	pocket_saver_attr_index_t *v;
	if ((v = (pocket_saver_attr_index_t *) hashmap_get_name(&index->pool, name)))
	{
		if (v->attr.tag == pocket_tag$index)
			goto label_ok;
	}
	else if ((v = pocket_saver_attr_index_alloc(name, index->attr.string_pool_list)))
	{
		if (hashmap_set_name(&index->pool, name, v, pocket_saver_attr_index_free_func))
		{
			pocket_saver_attr_index_link_attr(index, &v->attr);
			label_ok:
			return v;
		}
		pocket_saver_attr_index_free(v);
	}
	return NULL;
}

static void pocket_saver_attr_data_free_func(hashmap_vlist_t *restrict vl)
{
	if (vl->value)
		pocket_saver_attr_data_free((pocket_saver_attr_data_t *) vl->value);
}

static pocket_saver_attr_data_t* pocket_saver_attr_index_insert_data(pocket_saver_attr_index_t *restrict index, const char *restrict name, pocket_tag_t tag, const char *tag_string, const void *data, uint64_t size, uintptr_t align)
{
	pocket_saver_attr_data_t *v;
	if (!hashmap_find_name(&index->pool, name))
	{
		if ((v = pocket_saver_attr_data_alloc(name, index->attr.string_pool_list, tag, tag_string, data, size, align)))
		{
			if (hashmap_set_name(&index->pool, name, v, pocket_saver_attr_data_free_func))
			{
				pocket_saver_attr_index_link_attr(index, &v->attr);
				return v;
			}
			pocket_saver_attr_data_free(v);
		}
	}
	return NULL;
}

static pocket_saver_attr_index_t* pocket_saver_attr_insert_path(pocket_saver_attr_index_t *restrict index, const char *const restrict *restrict *restrict pp)
{
	const char *const restrict *restrict p;
	if (*(p = *pp))
	{
		while (index && p[1])
		{
			index = pocket_saver_attr_index_insert_index(index, *p);
			++p;
		}
		*pp = p;
		return index;
	}
	return NULL;
}

static void pocket_saver_attr_delete_path(pocket_saver_attr_index_t *restrict index, const char *const restrict *restrict path)
{
	if (*path)
	{
		while (index && path[1])
		{
			index = pocket_saver_attr_index_find_index(index, *path);
			++path;
		}
		if (index) pocket_saver_attr_index_delete(index, *path);
	}
}

static pocket_saver_s* pocket_saver_link_data(pocket_saver_s *restrict saver, pocket_saver_attr_data_t *restrict data)
{
	rbtree_t *v;
	pocket_saver_attr_data_t *next;
	if (!data->attr.attr.size) goto label_ok;
	v = rbtree_find(&saver->data_align_list, NULL, data->align);
	if (v)
	{
		label_link:
		if ((next = (pocket_saver_attr_data_t *) v->value))
			next->pr = &data->next;
		data->next = next;
		data->pr = (pocket_saver_attr_data_t **) &v->value;
		v->value = data;
		label_ok:
		return saver;
	}
	else if ((v = rbtree_insert(&saver->data_align_list, NULL, data->align, NULL, NULL)))
		goto label_link;
	return NULL;
}

static void pocket_saver_free_func(pocket_saver_s *restrict r)
{
	if (r->verify) string_pool_list_unlink(&r->string_pool_list, r->verify);
	if (r->tag) string_pool_list_unlink(&r->string_pool_list, r->tag);
	if (r->version) string_pool_list_unlink(&r->string_pool_list, r->version);
	if (r->author) string_pool_list_unlink(&r->string_pool_list, r->author);
	if (r->time) string_pool_list_unlink(&r->string_pool_list, r->time);
	if (r->description) string_pool_list_unlink(&r->string_pool_list, r->description);
	if (r->flag) string_pool_list_unlink(&r->string_pool_list, r->flag);
	if (r->system) pocket_saver_attr_index_free(r->system);
	if (r->user) pocket_saver_attr_index_free(r->user);
	rbtree_clear(&r->data_align_list);
	rbtree_clear(&r->string_pool_list);
}

pocket_saver_s* pocket_saver_alloc(void)
{
	pocket_saver_s *restrict r;
	r = (pocket_saver_s *) refer_alloz(sizeof(pocket_saver_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) pocket_saver_free_func);
		r->system = pocket_saver_attr_index_alloc(NULL, &r->string_pool_list);
		r->user = pocket_saver_attr_index_alloc(NULL, &r->string_pool_list);
		if (r->system && r->user)
			return r;
		refer_free(r);
	}
	return NULL;
}

static inline pocket_saver_s* pocket_saver_set_string(pocket_saver_s *restrict saver, const char *restrict *restrict p, const char *restrict s)
{
	if (*p) string_pool_list_unlink(&saver->string_pool_list, *p);
	*p = NULL;
	if (!s || (*p = string_pool_list_link(&saver->string_pool_list, s, strlen(s))))
		return saver;
	return NULL;
}

pocket_saver_s* pocket_saver_set_verify(pocket_saver_s *restrict saver, const char *restrict verify)
{
	return pocket_saver_set_string(saver, &saver->verify, verify);
}

pocket_saver_s* pocket_saver_set_tag(pocket_saver_s *restrict saver, const char *restrict tag)
{
	return pocket_saver_set_string(saver, &saver->tag, tag);
}

pocket_saver_s* pocket_saver_set_version(pocket_saver_s *restrict saver, const char *restrict version)
{
	return pocket_saver_set_string(saver, &saver->version, version);
}

pocket_saver_s* pocket_saver_set_author(pocket_saver_s *restrict saver, const char *restrict author)
{
	return pocket_saver_set_string(saver, &saver->author, author);
}

pocket_saver_s* pocket_saver_set_time(pocket_saver_s *restrict saver, const char *restrict time)
{
	return pocket_saver_set_string(saver, &saver->time, time);
}

pocket_saver_s* pocket_saver_set_description(pocket_saver_s *restrict saver, const char *restrict description)
{
	if (description)
	{
		pocket_saver_attr_t *attr;
		attr = (pocket_saver_attr_t *) hashmap_get_name(&saver->system->pool, description);
		if (!attr || attr->tag != pocket_tag$text)
			return NULL;
	}
	return pocket_saver_set_string(saver, &saver->description, description);
}

pocket_saver_s* pocket_saver_set_flag(pocket_saver_s *restrict saver, const char *restrict flag)
{
	return pocket_saver_set_string(saver, &saver->flag, flag);
}

pocket_saver_index_t* pocket_saver_root_system(pocket_saver_s *restrict saver)
{
	return saver->system;
}

pocket_saver_index_t* pocket_saver_root_user(pocket_saver_s *restrict saver)
{
	return saver->user;
}

pocket_saver_index_t* pocket_saver_cd(pocket_saver_index_t *restrict index, const char *const restrict *restrict path)
{
	while (index && *path)
	{
		index = (pocket_saver_attr_index_t *) hashmap_get_name(&index->pool, *path);
		if (index && index->attr.tag != pocket_tag$index)
			return NULL;
		++path;
	}
	return index;
}

void pocket_saver_delete(pocket_saver_index_t *restrict index, const char *const restrict *restrict path)
{
	pocket_saver_attr_delete_path(index, path);
}

pocket_saver_s* pocket_saver_create_null(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path)
{
	if ((index = pocket_saver_attr_insert_path(index, &path)))
	{
		if (pocket_saver_attr_index_insert_data(index, *path, pocket_tag$null, NULL, NULL, 0, 0))
			return saver;
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_index(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path)
{
	if ((index = pocket_saver_attr_insert_path(index, &path)))
	{
		if (pocket_saver_attr_index_insert_index(index, *path))
			return saver;
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_string(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const char *restrict string)
{
	if (string && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$string, NULL, string, 0, 0)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_text(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const char *restrict data, uintptr_t align)
{
	if (data && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$text, NULL, data, strlen(data) + 1, align)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_u8(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const uint8_t *restrict data, uint64_t n, uintptr_t align)
{
	if (n && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$u8, NULL, data, n, align)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_s8(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const int8_t *restrict data, uint64_t n, uintptr_t align)
{
	if (n && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$s8, NULL, data, n, align)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_u16(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const uint16_t *restrict data, uint64_t n, uintptr_t align)
{
	if (n && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$u16, NULL, data, n << 1, align)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_s16(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const int16_t *restrict data, uint64_t n, uintptr_t align)
{
	if (n && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$s16, NULL, data, n << 1, align)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_u32(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const uint32_t *restrict data, uint64_t n, uintptr_t align)
{
	if (n && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$u32, NULL, data, n << 2, align)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_s32(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const int32_t *restrict data, uint64_t n, uintptr_t align)
{
	if (n && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$s32, NULL, data, n << 2, align)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_u64(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const uint64_t *restrict data, uint64_t n, uintptr_t align)
{
	if (n && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$u64, NULL, data, n << 3, align)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_s64(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const int64_t *restrict data, uint64_t n, uintptr_t align)
{
	if (n && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$s64, NULL, data, n << 3, align)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_f32(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const float *restrict data, uint64_t n, uintptr_t align)
{
	if (n && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$f32, NULL, data, n << 2, align)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_f64(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const double *restrict data, uint64_t n, uintptr_t align)
{
	if (n && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$f64, NULL, data, n << 3, align)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

pocket_saver_s* pocket_saver_create_custom(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *const restrict *restrict path, const char *tag, const void *restrict data, uint64_t size, uintptr_t align)
{
	if (tag && (index = pocket_saver_attr_insert_path(index, &path)))
	{
		pocket_saver_attr_data_t *v;
		if ((v = pocket_saver_attr_index_insert_data(index, *path, pocket_tag$custom, tag, data, size, align)))
		{
			if (pocket_saver_link_data(saver, v))
				return saver;
			pocket_saver_attr_index_delete(index, *path);
		}
	}
	return NULL;
}

static hashmap_t* pocket_saver_appoint_string_pool_insert(hashmap_t *restrict string_pool, const char *restrict string, uint64_t *restrict offset)
{
	hashmap_vlist_t *vl;
	void *v;
	uintptr_t n;
	if (!hashmap_find_name(string_pool, string))
	{
		n = (uintptr_t) *offset;
		*offset += strlen(string) + 1;
		do {
			vl = hashmap_put_name(string_pool, string, v = (void *) n++, NULL);
			if (!vl) goto label_fail;
			if (vl->value != v) break;
		} while (*string++);
	}
	return string_pool;
	label_fail:
	return NULL;
}

static void pocket_saver_appoint_string_pool_do_func(hashmap_vlist_t *restrict vl, void *argv[2])
{
	if (argv[0] && !pocket_saver_appoint_string_pool_insert((hashmap_t *) argv[0], vl->name, (uint64_t *) argv[1]))
		argv[0] = NULL;
}

static void pocket_saver_appoint_string_pool_layer_func(rbtree_t *restrict v, void *argv[2])
{
	if (v->value)
		hashmap_call((hashmap_t *) v->value, (hashmap_func_call_f) pocket_saver_appoint_string_pool_do_func, argv);
}

static void pocket_saver_appoint_attr_appoint(pocket_saver_attr_t *restrict attr, uint64_t *restrict offset, uint64_t *restrict n)
{
	pocket_saver_attr_t *restrict p;
	*n = 0;
	for (p = attr; p; p = p->next)
	{
		p->attr_offset = *offset;
		*offset += sizeof(pocket_attr_t);
		++*n;
	}
	for (p = attr; p; p = p->next)
	{
		if (p->tag == pocket_tag$index && (attr = ((pocket_saver_attr_index_t *) p)->head))
		{
			p->attr.data.offset = *offset;
			pocket_saver_appoint_attr_appoint(attr, offset, &p->attr.size);
			if (!p->attr.size) p->attr.data.offset = 0;
		}
	}
}

static uint64_t pocket_saver_appoint_root_index_appoint(pocket_saver_attr_index_t *restrict index, uint64_t *restrict offset)
{
	uint64_t n;
	if (index->head)
	{
		pocket_saver_appoint_attr_appoint(&index->attr, offset, &n);
		return index->attr.attr_offset;
	}
	return 0;
}

static inline void pocket_saver_appoint_align(register uint64_t *restrict offset, register uintptr_t align)
{
	if (align > 1)
	{
		register uint64_t n = *offset + align - 1;
		*offset = n - n % align;
	}
}

static void pocket_saver_appoint_data_align_list_func(rbtree_t *restrict v, void *argv[2])
{
	pocket_saver_attr_data_t *restrict p;
	hashmap_t *restrict string_pool;
	uint64_t *restrict offset;
	if (argv[0])
	{
		p = (pocket_saver_attr_data_t *) v->value;
		string_pool = (hashmap_t *) argv[0];
		offset = (uint64_t *) argv[1];
		while (p)
		{
			if (p->attr.tag == pocket_tag$string)
			{
				if (!(p->attr.attr.data.offset = (uintptr_t) hashmap_get_name(string_pool, (const char *) p->data)))
					goto label_fail;
			}
			else if (p->attr.attr.size)
			{
				pocket_saver_appoint_align(offset, p->align);
				p->attr.attr.data.offset = *offset;
				*offset += p->attr.attr.size;
				pocket_saver_appoint_align(offset, p->align);
			}
			p = p->next;
		}
	}
	return ;
	label_fail:
	argv[0] = NULL;
}

static uint64_t pocket_saver_appoint(pocket_saver_s *restrict saver, hashmap_t *restrict string_pool, pocket_header_t *restrict header)
{
	void *argv[2];
	uint64_t offset;
	memset(header, 0, sizeof(pocket_header_t));
	header->magic = pocket_magic;
	offset = sizeof(pocket_header_t);
	header->string$offset = offset;
	argv[0] = string_pool;
	argv[1] = &offset;
	rbtree_call(&saver->string_pool_list, (rbtree_func_call_f) pocket_saver_appoint_string_pool_layer_func, argv);
	if (!argv[0]) goto label_fail;
	header->string$size = offset - header->string$offset;
	pocket_saver_appoint_align(&offset, 16);
	header->index$offset = offset;
	header->system.offset = pocket_saver_appoint_root_index_appoint(saver->system, &offset);
	header->user.offset = pocket_saver_appoint_root_index_appoint(saver->user, &offset);
	header->index$size = offset - header->index$offset;
	header->data$offset = offset;
	rbtree_call(&saver->data_align_list, (rbtree_func_call_f) pocket_saver_appoint_data_align_list_func, argv);
	if (!argv[0]) goto label_fail;
	header->data$size = offset - header->data$offset;
	#define set_string(_n)  if (saver->_n) header->_n.offset = (uintptr_t) hashmap_get_name(string_pool, saver->_n)
	set_string(verify);
	set_string(tag);
	set_string(version);
	set_string(author);
	set_string(time);
	set_string(description);
	set_string(flag);
	#undef set_string
	return offset;
	label_fail:
	return 0;
}

static void pocket_saver_write_string_pool_func(hashmap_vlist_t *restrict vl, uint8_t *restrict pocket)
{
	strcpy((char *) (pocket + (uintptr_t) vl->value), vl->name);
}

static void pocket_saver_write_attr(pocket_saver_attr_t *restrict attr, uint8_t *restrict pocket, hashmap_t *restrict string_pool)
{
	while (attr)
	{
		register pocket_attr_t *p;
		p = (pocket_attr_t *) (pocket + attr->attr_offset);
		p->name.offset = attr->attr.name.string?(uintptr_t) hashmap_get_name(string_pool, attr->attr.name.string):0;
		p->tag.offset = attr->attr.tag.string?(uintptr_t) hashmap_get_name(string_pool, attr->attr.tag.string):0;
		p->data.offset = attr->attr.data.offset;
		p->size = attr->attr.size;
		if (attr->tag == pocket_tag$index)
			pocket_saver_write_attr(((pocket_saver_attr_index_t *) attr)->head, pocket, string_pool);
		attr = attr->next;
	}
}

static void pocket_saver_write_data_func(rbtree_t *restrict v, uint8_t *restrict pocket)
{
	pocket_saver_attr_data_t *restrict p;
	p = (pocket_saver_attr_data_t *) v->value;
	while (p)
	{
		if (p->attr.attr.size)
			memcpy(pocket + p->attr.attr.data.offset, p->data, p->attr.attr.size);
		p = p->next;
	}
}

static void pocket_saver_write(pocket_saver_s *restrict saver, hashmap_t *restrict string_pool, pocket_header_t *restrict header, uint8_t *pocket)
{
	memcpy(pocket, header, sizeof(pocket_header_t));
	hashmap_call(string_pool, (hashmap_func_call_f) pocket_saver_write_string_pool_func, pocket);
	if (header->system.offset) pocket_saver_write_attr(&saver->system->attr, pocket, string_pool);
	if (header->user.offset) pocket_saver_write_attr(&saver->user->attr, pocket, string_pool);
	rbtree_call(&saver->data_align_list, (rbtree_func_call_f) pocket_saver_write_data_func, pocket);
}

uint8_t* pocket_saver_build(pocket_saver_s *restrict saver, uint64_t *restrict size, const struct pocket_verify_s *restrict verify)
{
	uint8_t *pocket;
	const pocket_verify_entry_s *restrict entry;
	pocket_saver_attr_data_t *restrict data;
	hashmap_t string_pool;
	pocket_header_t header;
	pocket = NULL;
	entry = NULL;
	data = NULL;
	if (saver->verify)
	{
		if (!verify) goto label_return;
		if (!(entry = verify->method(verify, saver->verify)))
			goto label_return;
		if (!entry->build)
			goto label_return;
		if (!(data = pocket_saver_attr_index_insert_data(
			saver->system,
			pocket_verify_attr_name,
			entry->tag,
			NULL,
			NULL,
			entry->size,
			entry->align)))
			goto label_return;
		if (!pocket_saver_link_data(saver, data))
			goto label_delete;
	}
	if (hashmap_init(&string_pool))
	{
		if ((*size = pocket_saver_appoint(saver, &string_pool, &header)))
		{
			pocket = (uint8_t *) calloc(1, (size_t) *size);
			if (pocket)
			{
				pocket_saver_write(saver, &string_pool, &header, pocket);
				if (saver->verify && !entry->build(entry, pocket + data->attr.attr.data.offset, pocket, *size))
				{
					free(pocket);
					pocket = NULL;
				}
			}
		}
		hashmap_uini(&string_pool);
	}
	label_delete:
	if (saver->verify) pocket_saver_attr_index_delete(saver->system, pocket_verify_attr_name);
	label_return:
	return pocket;
}

void pocket_saver_build_free(uint8_t *restrict pocket)
{
	free(pocket);
}

pocket_saver_s* pocket_saver_save(pocket_saver_s *restrict saver, const char *restrict path, const struct pocket_verify_s *restrict verify)
{
	pocket_saver_s *r;
	uint8_t *data;
	uint64_t size;
	r = NULL;
	data = pocket_saver_build(saver, &size, verify);
	if (data)
	{
		FILE *fp;
		fp = fopen(path, "wb");
		if (fp)
		{
			if (fwrite(data, 1, (size_t) size, fp) == (size_t) size)
				r = saver;
			fclose(fp);
		}
		free(data);
	}
	return r;
}
