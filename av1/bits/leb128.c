#include "reader.h"
#include "writer.h"

av1_bits_reader_t* av1_bits_leb128_read(av1_bits_reader_t *restrict r, uint64_t *restrict value)
{
	register uint64_t v, c;
	uintptr_t i;
	v = 0;
	i = 8;
	do {
		if (r->saver.rbits < 8)
			goto label_fail;
		v = (v << 7) | ((c = av1_bits_reader_read(r, 8)) & 0x7f);
	} while ((c & 0x80) && --i);
	*value = v;
	return r;
	label_fail:
	return NULL;
}

av1_bits_writer_t* av1_bits_leb128_write(av1_bits_writer_t *restrict w, register uint64_t value)
{
	register uint64_t v;
	uint32_t bits;
	if ((value <<= 8))
	{
		while (!(value & 0xfe00000000000000ul))
			value <<= 7;
	}
	v = 0;
	bits = 0;
	goto label_entry;
	do {
		v |= 0x80;
		v <<= 7;
		label_entry:
		v |= (value >> 57);
		bits += 8;
		value <<= 7;
			
	} while (value);
	if (av1_bits_writer_write(w, v, bits))
		return w;
	return NULL;
}
