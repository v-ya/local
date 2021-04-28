#include "inner.fullbox.h"
#include <string.h>

inner_fullbox_t* mpeg4$define(inner, fullbox, get)(inner_fullbox_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	if (*size >= sizeof(mpeg4_full_box_suffix_t))
	{
		mpeg4_full_box_suffix_t suffix;
		memcpy(&suffix, *data, sizeof(suffix));
		r->version = suffix.version;
		r->flags = ((uint32_t) suffix.flags[0] << 2) |
			((uint32_t) suffix.flags[1] << 1) |
			(uint32_t) suffix.flags[2];
		*data += sizeof(mpeg4_full_box_suffix_t);
		*size -= sizeof(mpeg4_full_box_suffix_t);
		return r;
	}
	return NULL;
}

uint8_t* mpeg4$define(inner, fullbox, set)(uint8_t *restrict data, const inner_fullbox_t *restrict r)
{
	data[0] = (uint8_t) r->version;
	data[1] = (uint8_t) (r->flags >> 16);
	data[2] = (uint8_t) (r->flags >> 8);
	data[3] = (uint8_t) (r->flags);
	return data + sizeof(mpeg4_full_box_suffix_t);
}

const char* mpeg4$define(inner, flags, string)(char *restrict buffer, uint32_t flags, const char *flag_name[], uintptr_t flag_number)
{
	const char *r;
	uintptr_t i;
	size_t n;
	*buffer = 0;
	r = buffer;
	for (i = 0; i < flag_number; ++i)
	{
		if (flag_name[i] && (flags & 1))
		{
			if (*buffer) ++buffer;
			n = strlen(flag_name[i]);
			memcpy(buffer, flag_name[i], n);
			buffer += n;
			*buffer = '|';
		}
		flags >>= 1;
	}
	*buffer = 0;
	return r;
}

void mpeg4$define(inner, fullbox, dump)(mlog_s *restrict mlog, const inner_fullbox_t *restrict r, const char *restrict flags, uint32_t level)
{
	if (!flags) mlog_level_dump("version = %u, flags = %06x\n", r->version, r->flags);
	else mlog_level_dump("version = %u, flags = %06x (%s)\n", r->version, r->flags, flags);
}
