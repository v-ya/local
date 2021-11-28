#include "script_ptag.h"

static inline const exbuffer_t* ptag_buffer_is_string(const exbuffer_t *restrict buffer)
{
	if (buffer && buffer->used && !buffer->data[buffer->used - 1])
		return buffer;
	return NULL;
}

static inline const void* ptag_buffer_to_ptr(const exbuffer_t *restrict buffer, uintptr_t shr, uint64_t *restrict n)
{
	if (buffer)
	{
		*n = buffer->used >> shr;
		return buffer->data;
	}
	else
	{
		*n = 0;
		return NULL;
	}
}

static pocket_saver_s* ptag_null(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const exbuffer_t *restrict buffer, uintptr_t align)
{
	if (buffer) return NULL;
	return pocket_saver_create_null(saver, index, path);
}

static pocket_saver_s* ptag_index(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const exbuffer_t *restrict buffer, uintptr_t align)
{
	if (buffer) return NULL;
	return pocket_saver_create_index(saver, index, path);
}

static pocket_saver_s* ptag_string(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const exbuffer_t *restrict buffer, uintptr_t align)
{
	if (!ptag_buffer_is_string(buffer) || align) return NULL;
	return pocket_saver_create_string(saver, index, path, (const char *) buffer->data);
}

static pocket_saver_s* ptag_text(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const exbuffer_t *restrict buffer, uintptr_t align)
{
	if (!ptag_buffer_is_string(buffer)) return NULL;
	return pocket_saver_create_text(saver, index, path, (const char *) buffer->data, align);
}

#define d_ptag_array(_t, _shr)  \
	static pocket_saver_s* ptag_##_t(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const exbuffer_t *restrict buffer, uintptr_t align)\
	{\
		const void *d;\
		uint64_t n;\
		d = ptag_buffer_to_ptr(buffer, _shr, &n);\
		return pocket_saver_create_##_t(saver, index, path, d, n, align);\
	}\

d_ptag_array(u8, 0)
d_ptag_array(s8, 0)
d_ptag_array(u16, 1)
d_ptag_array(s16, 1)
d_ptag_array(u32, 2)
d_ptag_array(s32, 2)
d_ptag_array(u64, 3)
d_ptag_array(s64, 3)
d_ptag_array(f32, 2)
d_ptag_array(f64, 3)

#undef d_ptag_array

hashmap_t* script_ptag_init(hashmap_t *restrict p)
{
	#define d(name) hashmap_set_name(p, #name, ptag_##name, NULL)
	if (
		d(null) &&
		d(index) &&
		d(string) &&
		d(text) &&
		d(u8) &&
		d(s8) &&
		d(u16) &&
		d(s16) &&
		d(u32) &&
		d(s32) &&
		d(u64) &&
		d(s64) &&
		d(f32) &&
		d(f64)
	) return p;
	return NULL;
	#undef d
}
