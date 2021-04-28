#include "inner.timespec.h"
#include <memory.h>
#include <stdio.h>
#include <time.h>

#define _utc_to_t1904 2082844800ul

inner_timespec_t* mpeg4$define(inner, timespec32, get)(inner_timespec_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	inner_timespec_32_t t;
	if (*size >= sizeof(t))
	{
		memcpy(&t, *data, sizeof(t));
		*data += sizeof(t);
		*size -= sizeof(t);
		r->creation_time = mpeg4_n32(t.creation_time);
		r->modification_time = mpeg4_n32(t.modification_time);
		r->timescale = mpeg4_n32(t.timescale);
		r->duration = mpeg4_n32(t.duration);
		return r;
	}
	return NULL;
}

uint8_t* mpeg4$define(inner, timespec32, set)(uint8_t *restrict data, const inner_timespec_t *restrict r)
{
	inner_timespec_32_t t;
	t.creation_time = mpeg4_n32((uint32_t) r->creation_time);
	t.modification_time = mpeg4_n32((uint32_t) r->modification_time);
	t.timescale = mpeg4_n32(r->timescale);
	t.duration = mpeg4_n32((uint32_t) r->duration);
	memcpy(data, &t, sizeof(t));
	return data + sizeof(t);
}

inner_timespec_t* mpeg4$define(inner, timespec64, get)(inner_timespec_t *restrict r, const uint8_t *restrict *restrict data, uint64_t *restrict size)
{
	inner_timespec_64_t t;
	if (*size >= sizeof(t))
	{
		memcpy(&t, *data, sizeof(t));
		*data += sizeof(t);
		*size -= sizeof(t);
		r->creation_time = mpeg4_n64(t.creation_time);
		r->modification_time = mpeg4_n64(t.modification_time);
		r->timescale = mpeg4_n32(t.timescale);
		r->duration = mpeg4_n64(t.duration);
		return r;
	}
	return NULL;
}

uint8_t* mpeg4$define(inner, timespec64, set)(uint8_t *restrict data, const inner_timespec_t *restrict r)
{
	inner_timespec_64_t t;
	t.creation_time = mpeg4_n64(r->creation_time);
	t.modification_time = mpeg4_n64(r->modification_time);
	t.timescale = mpeg4_n32(r->timescale);
	t.duration = mpeg4_n64(r->duration);
	memcpy(data, &t, sizeof(t));
	return data + sizeof(t);
}

uint64_t mpeg4$define(inner, time1904, utc)(uint64_t t1904)
{
	return t1904 - _utc_to_t1904;
}

uint64_t mpeg4$define(inner, utc, time1904)(uint64_t utc)
{
	return utc + _utc_to_t1904;
}

const char* mpeg4$define(inner, time1904, string)(char *restrict buffer, uint64_t t1904)
{
	struct tm tm;
	time_t t = (time_t) t1904 - _utc_to_t1904;
	localtime_r(&t, &tm);
	buffer[0] = 0;
	sprintf(buffer, "%04d-%02u-%02u %02u:%02u:%02u", tm.tm_year + 1900, tm.tm_mon + 1, tm.tm_mday, tm.tm_hour, tm.tm_min, tm.tm_sec);
	return buffer;
}

const char* mpeg4$define(inner, duration, string)(char *restrict buffer, double duration)
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

