#include "reader.h"
#include "writer.h"

av1_bits_reader_t* av1_bits_flag_read(av1_bits_reader_t *restrict r, uint8_t *restrict flag)
{
	if (r->saver.rbits)
	{
		*flag = (uint8_t) av1_bits_reader_read(r, 1);
		return r;
	}
	return NULL;
}

av1_bits_writer_t* av1_bits_flag_write(av1_bits_writer_t *restrict w, uint8_t flag)
{
	return av1_bits_writer_write(w, !!flag, 1);
}
