#include "refer.h"
#include <stdlib.h>

typedef struct refer_s {
	uint64_t inode;
	refer_free_f free_func;
} refer_s;

refer_t refer_alloc(size_t size)
{
	refer_s *r;
	r = (refer_s *) malloc(size + sizeof(refer_s));
	if (r)
	{
		r->inode = 1;
		r->free_func = NULL;
	}
	return (refer_t) (r + 1);
}

refer_t refer_alloz(size_t size)
{
	refer_s *r;
	r = (refer_s *) calloc(1, size + sizeof(refer_s));
	if (r) r->inode = 1;
	return (refer_t) (r + 1);
}

refer_t refer_free(refer_t v)
{
	if (!__sync_sub_and_fetch(&((refer_s *) v - 1)->inode, 1))
	{
		refer_s *s;
		s = (refer_s *) v - 1;
		if (s->free_func) s->free_func((void *) v);
		free(s);
		v = NULL;
	}
	return v;
}

void refer_set_free(refer_t v, refer_free_f free_func)
{
	((refer_s *) v - 1)->free_func = free_func;
}

refer_free_f refer_get_free(refer_t v)
{
	return ((refer_s *) v - 1)->free_func;
}

refer_t refer_save(refer_t v)
{
	if (v) __sync_add_and_fetch(&((refer_s *) v - 1)->inode, 1);
	return v;
}

uint64_t refer_save_number(refer_t v)
{
	return ((refer_s *) v - 1)->inode;
}

// data dump

#include <string.h>

refer_string_t refer_dump_string_with_length(const char *restrict string, size_t length)
{
	char *restrict r;
	r = (char *) refer_alloc(length + 1);
	if (r)
	{
		memcpy(r, string, length);
		r[length] = 0;
	}
	return r;
}

refer_string_t refer_dump_string(const char *restrict string)
{
	return refer_dump_string_with_length(string, strlen(string));
}

refer_nstring_t refer_dump_nstring_with_length(const char *restrict string, size_t length)
{
	struct refer_nstring_s *restrict r;
	r = (struct refer_nstring_s *) refer_alloc((sizeof(struct refer_nstring_s) + 1) + length);
	if (r)
	{
		r->length = length;
		memcpy(r->string, string, length);
		r->string[length] = 0;
	}
	return r;
}

refer_nstring_t refer_dump_nstring(const char *restrict string)
{
	return refer_dump_nstring_with_length(string, strlen(string));
}

refer_t refer_dump_data(const void *restrict data, size_t size)
{
	if (size)
	{
		void *restrict r;
		r = (char *) refer_alloc(size);
		if (r) return memcpy(r, data, size);
	}
	return NULL;
}
