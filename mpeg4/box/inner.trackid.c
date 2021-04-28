#include "inner.trackid.h"
#include <memory.h>

inner_trackid_t* mpeg4$define(inner, trackid32, get)(inner_trackid_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	inner_trackid_32_t t;
	if (*size >= sizeof(t))
	{
		memcpy(&t, *data, sizeof(t));
		*data += sizeof(t);
		*size -= sizeof(t);
		r->creation_time = mpeg4_n32(t.creation_time);
		r->modification_time = mpeg4_n32(t.modification_time);
		r->track_id = mpeg4_n32(t.track_id);
		r->duration = mpeg4_n32(t.duration);
		return r;
	}
	return NULL;
}

uint8_t* mpeg4$define(inner, trackid32, set)(uint8_t *restrict data, const inner_trackid_t *restrict r)
{
	inner_trackid_32_t t;
	t.creation_time = mpeg4_n32((uint32_t) r->creation_time);
	t.modification_time = mpeg4_n32((uint32_t) r->modification_time);
	t.track_id = mpeg4_n32(r->track_id);
	t.reserved = 0;
	t.duration = mpeg4_n32((uint32_t) r->duration);
	memcpy(data, &t, sizeof(t));
	return data + sizeof(t);
}

inner_trackid_t* mpeg4$define(inner, trackid64, get)(inner_trackid_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	inner_trackid_64_t t;
	if (*size >= sizeof(t))
	{
		memcpy(&t, *data, sizeof(t));
		*data += sizeof(t);
		*size -= sizeof(t);
		r->creation_time = mpeg4_n64(t.creation_time);
		r->modification_time = mpeg4_n64(t.modification_time);
		r->track_id = mpeg4_n32(t.track_id);
		r->duration = mpeg4_n64(t.duration);
		return r;
	}
	return NULL;
}

uint8_t* mpeg4$define(inner, trackid64, set)(uint8_t *restrict data, const inner_trackid_t *restrict r)
{
	inner_trackid_64_t t;
	t.creation_time = mpeg4_n64(r->creation_time);
	t.modification_time = mpeg4_n64(r->modification_time);
	t.track_id = mpeg4_n32(r->track_id);
	t.reserved = 0;
	t.duration = mpeg4_n64(r->duration);
	memcpy(data, &t, sizeof(t));
	return data + sizeof(t);
}
