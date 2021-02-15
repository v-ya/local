#include "script_ptag.h"

static inline const buffer_t* ptag_buffer_is_string(const buffer_t *restrict buffer)
{
	if (buffer && buffer->used && !buffer->data[buffer->used - 1])
		return buffer;
	return NULL;
}

static pocket_saver_s* ptag_null(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (buffer) return NULL;
	return pocket_saver_create_null(saver, index, path);
}

static pocket_saver_s* ptag_index(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (buffer) return NULL;
	return pocket_saver_create_index(saver, index, path);
}

static pocket_saver_s* ptag_string(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (!ptag_buffer_is_string(buffer) || align) return NULL;
	return pocket_saver_create_string(saver, index, path, (const char *) buffer->data);
}

static pocket_saver_s* ptag_text(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (!ptag_buffer_is_string(buffer)) return NULL;
	return pocket_saver_create_text(saver, index, path, (const char *) buffer->data, align);
}

static pocket_saver_s* ptag_u8(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (!buffer) return NULL;
	return pocket_saver_create_u8(saver, index, path, (const uint8_t *) buffer->data, buffer->used, align);
}

static pocket_saver_s* ptag_s8(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (!buffer) return NULL;
	return pocket_saver_create_s8(saver, index, path, (const int8_t *) buffer->data, buffer->used, align);
}

static pocket_saver_s* ptag_u16(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (!buffer) return NULL;
	return pocket_saver_create_u16(saver, index, path, (const uint16_t *) buffer->data, buffer->used >> 1, align);
}

static pocket_saver_s* ptag_s16(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (!buffer) return NULL;
	return pocket_saver_create_s16(saver, index, path, (const int16_t *) buffer->data, buffer->used >> 1, align);
}

static pocket_saver_s* ptag_u32(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (!buffer) return NULL;
	return pocket_saver_create_u32(saver, index, path, (const uint32_t *) buffer->data, buffer->used >> 2, align);
}

static pocket_saver_s* ptag_s32(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (!buffer) return NULL;
	return pocket_saver_create_s32(saver, index, path, (const int32_t *) buffer->data, buffer->used >> 2, align);
}

static pocket_saver_s* ptag_u64(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (!buffer) return NULL;
	return pocket_saver_create_u64(saver, index, path, (const uint64_t *) buffer->data, buffer->used >> 3, align);
}

static pocket_saver_s* ptag_s64(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (!buffer) return NULL;
	return pocket_saver_create_s64(saver, index, path, (const int64_t *) buffer->data, buffer->used >> 3, align);
}

static pocket_saver_s* ptag_f32(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (!buffer) return NULL;
	return pocket_saver_create_f32(saver, index, path, (const float *) buffer->data, buffer->used >> 2, align);
}

static pocket_saver_s* ptag_f64(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, const char *restrict *restrict path, const buffer_t *restrict buffer, uintptr_t align)
{
	if (!buffer) return NULL;
	return pocket_saver_create_f64(saver, index, path, (const double *) buffer->data, buffer->used >> 3, align);
}

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
