#include "reader.h"
#include "writer.h"

av1_bits_reader_t* av1_bits_uvlc_read(av1_bits_reader_t *restrict r, uint32_t *restrict value)
{
	register uint32_t n;
	n = ~(uint32_t) 0;
	do {
		++n;
		if (!r->saver.rbits)
			goto label_fail;
	} while (!av1_bits_reader_read(r, 1));
	if (n < 32)
	{
		if (n > r->saver.rbits)
			goto label_fail;
		if (n) n = (av1_bits_reader_read(r, n) | (1u << n)) - 1;
		*value = n;
	}
	else *value = 0xffffffff;
	return r;
	label_fail:
	return NULL;
}

av1_bits_writer_t* av1_bits_uvlc_write(av1_bits_writer_t *restrict w, register uint32_t value)
{
	if (++value)
	{
		if (!av1_bits_writer_write(w, value, ((uint32_t) (31 - __builtin_clz(value)) << 1) | 1))
			goto label_fail;
		label_ok:
		return w;
	}
	else
	{
		if (av1_bits_writer_write(w, 1, 33))
			goto label_ok;
	}
	label_fail:
	return NULL;
}

uint64_t av1_bits_uvlc_bits(register uint32_t value)
{
	if (++value)
		return ((uint64_t) (31 - __builtin_clz(value)) << 1) | 1;
	return 33;
}
