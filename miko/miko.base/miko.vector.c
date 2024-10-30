#include "miko.vector.h"
#include <memory.h>

static void miko_vector_free_func(miko_vector_s *restrict r)
{
	miko_vector_pop(r, r->item_count);
	exbuffer_uini(&r->vector);
}

miko_vector_s_t miko_vector_alloc(uintptr_t item_size, miko_vector_initial_f initial, miko_vector_finally_f finally)
{
	miko_vector_s *restrict r;
	if (item_size && (r = (miko_vector_s *) refer_alloz(sizeof(miko_vector_s))))
	{
		refer_hook_free(r, vector);
		if (exbuffer_init(&r->vector, 0))
		{
			r->item_size = item_size;
			r->initial = initial;
			r->finally = finally;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

uintptr_t miko_vector_push(miko_vector_s_t r, const void *item_array, uintptr_t item_count)
{
	miko_vector_initial_f initial;
	uint8_t *restrict p;
	uintptr_t n, z;
	if ((n = (z = r->item_size) * item_count) &&
		(p = (uint8_t *) exbuffer_need(&r->vector, r->vector.used + n)))
	{
		p += r->vector.used;
		memcpy(p, item_array, n);
		if ((initial = r->initial))
		{
			for (n = 0; n < item_count; ++n, p += z)
			{
				if (!initial(p)) break;
				r->vector.used += z;
			}
			r->item_count += n;
			return n;
		}
		r->vector.used += n;
		r->item_count += item_count;
		return item_count;
	}
	return 0;
}

uintptr_t miko_vector_pop(miko_vector_s_t r, uintptr_t item_count)
{
	miko_vector_finally_f finally;
	uint8_t *restrict p;
	uintptr_t i, n, z;
	if (item_count > (n = r->item_count))
		item_count = n;
	p = r->vector.data + (r->vector.used = (r->item_count = n - item_count) * (z = r->item_size));
	if ((finally = r->finally))
	{
		for (i = 0; i < item_count; ++i, p += z)
			finally(p);
	}
	return item_count;
}

uintptr_t miko_vector_count(const miko_vector_s_t r)
{
	return r->item_count;
}

void* miko_vector_data(const miko_vector_s_t r)
{
	return r->vector.data;
}

void* miko_vector_index(const miko_vector_s_t r, uintptr_t item_index)
{
	if (item_index < r->item_count)
		return r->vector.data + item_index * r->item_size;
	return NULL;
}

void* miko_vector_map(const miko_vector_s_t r, uintptr_t item_offset, uintptr_t item_count, uintptr_t *restrict count)
{
	uintptr_t n;
	n = r->item_count;
	if (!~item_offset && item_count <= n) item_offset = n - item_count;
	if (!~item_count && item_offset <= n) item_count = n - item_offset;
	if (item_offset < n && item_count && item_count <= (n - item_offset))
	{
		if (count) *count = item_count;
		return r->vector.data + item_offset * r->item_size;
	}
	if (count) *count = 0;
	return NULL;
}
