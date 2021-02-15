#include "buffer.h"
#include <stdlib.h>
#include <memory.h>

#define buffer_default_size  4096

buffer_t* buffer_alloc(void)
{
	buffer_t *restrict r;
	r = (buffer_t *) malloc(sizeof(buffer_t));
	if (r)
	{
		r->data = (uint8_t *) calloc(1, buffer_default_size);
		if (r->data)
		{
			r->size = buffer_default_size;
			r->used = 0;
			return r;
		}
		free(r);
	}
	return NULL;
}

void buffer_free(buffer_t *restrict buffer)
{
	free(buffer->data);
	free(buffer);
}

void buffer_clear(buffer_t *restrict buffer)
{
	if (buffer->used)
	{
		memset(buffer->data, 0, buffer->used);
		buffer->used = 0;
	}
}

buffer_t* buffer_need(buffer_t *restrict buffer, uintptr_t size)
{
	uint8_t *data;
	uintptr_t n;
	if (size <= buffer->size)
	{
		label_ok:
		if (size > buffer->used)
			buffer->used = size;
		return buffer;
	}
	n = size;
	--size;
	size |= size >> 32;
	size |= size >> 16;
	size |= size >> 8;
	size |= size >> 4;
	size |= size >> 2;
	size |= size >> 1;
	if (~size)
	{
		data = (uint8_t *) realloc(buffer->data, ++size);
		if (data)
		{
			memset(data + buffer->size, 0, size - buffer->size);
			buffer->data = data;
			buffer->size = size;
			size = n;
			goto label_ok;
		}
	}
	return NULL;
}
