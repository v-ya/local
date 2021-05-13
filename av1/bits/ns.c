#include "reader.h"
#include "writer.h"

av1_bits_reader_t* av1_bits_ns_read(av1_bits_reader_t *restrict r, uint64_t *restrict value, uint64_t n)
{
	uint64_t v;
	uint32_t L;
	if (!n) goto label_fail;
	L = 63 - __builtin_clz(n);
	n = ((uint64_t) 1 << (L + 1)) - n;
	if (L > r->saver.rbits)
		goto label_fail;
	if ((v = av1_bits_reader_read(r, L)) < n)
		goto label_ok;
	if (!r->saver.rbits)
		goto label_fail;
	v = (v << 1) - n + av1_bits_reader_read(r, 1);
	label_ok:
	*value = v;
	return r;
	label_fail:
	return NULL;
}

av1_bits_writer_t* av1_bits_ns_write(av1_bits_writer_t *restrict w, uint64_t value, uint64_t n)
{
	uint32_t L;
	if (!n) goto label_fail;
	L = 64 - __builtin_clz(n);
	n = ((uint64_t) 1 << L) - n;
	if (value < n) --L;
	else value += n;
	return av1_bits_writer_write(w, value, L);
	label_fail:
	return NULL;
}

uint64_t av1_bits_ns_bits(uint64_t value, uint64_t n)
{
	uint32_t L;
	if (!n) goto label_fail;
	L = 64 - __builtin_clz(n);
	return L - (value < (((uint64_t) 1 << L) - n));
	label_fail:
	return 0;
}
