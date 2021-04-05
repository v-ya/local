#define __USE_POSIX
#include "mpeg4.box.h"
#include <memory.h>
#include <stdio.h>
#include <time.h>

uint64_t mpeg4_box_border_parse(const uint8_t *restrict data, uint64_t size, mpeg4_box_extend_size_t *restrict rsize, mpeg4_box_type_t *restrict rtype)
{
	#define _size_base    sizeof(mpeg4_box_t)
	#define _size_extend  (sizeof(mpeg4_box_t) + sizeof(mpeg4_box_extend_size_t))
	if (size < _size_base)
		goto label_fail;
	rtype->v = ((const mpeg4_box_t *) data)->type.v;
	switch ((*rsize = (uint64_t) mpeg4_n32(((const mpeg4_box_t *) data)->size)))
	{
		case 1:
			if (size < _size_extend)
				goto label_fail;
			if ((*rsize = mpeg4_n64(*((const mpeg4_box_extend_size_t *) (data + _size_base)))) < _size_extend || *rsize > size)
				goto label_fail;
			return _size_extend;
		case 0:
			*rsize = size;
			// fall through
		default:
			if (*rsize < _size_base || *rsize > size)
				goto label_fail;
			return _size_base;
	}
	label_fail:
	return 0;
	#undef _size_base
	#undef _size_extend
}

uint32_t mpeg4_full_box_suffix_parse(mpeg4_full_box_suffix_t suffix, uint32_t *restrict flags)
{
	if (flags) *flags = ((uint32_t) suffix.flags[0] << 2) | ((uint32_t) suffix.flags[1] << 1) | (uint32_t) suffix.flags[2];
	return suffix.version;
}

const char* mpeg4_time1904_string(char *restrict buffer, uint64_t t1904)
{
	struct tm tm;
	time_t t = (time_t) t1904 - 2082844800;
	localtime_r(&t, &tm);
	buffer[0] = 0;
	sprintf(buffer, "%04d-%02u-%02u %02u:%02u:%02u", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	return buffer;
}

const char* mpeg4_duration_string(char *restrict buffer, double duration)
{
	uint64_t hour;
	uint64_t min;
	uint64_t sec;
	char sign;
	sign = '+';
	if (duration < 0)
	{
		duration = -duration;
		sign = '-';
	}
	sec = duration;
	duration -= sec;
	min = sec / 60;
	sec %= 60;
	hour = min / 60;
	min %= 60;
	buffer[0] = 0;
	sprintf(buffer, "%c%lu:%02lu:%05.2f", sign, hour, min, duration + sec);
	return buffer;
}
