#include "bits.h"

static const uint8_t* media_bits_reader_next_bytes_default(struct media_bits_reader_t *restrict reader, uintptr_t *restrict nbyte)
{
	const uint8_t *restrict u8;
	uintptr_t nres;
	if ((nres = reader->size - reader->upos))
	{
		if (*nbyte > nres) *nbyte = nres;
		u8 = reader->u8 + reader->upos;
		reader->upos += *nbyte;
		return u8;
	}
	*nbyte = 0;
	return NULL;
}

// be

static struct media_bits_reader_t* media_bits_reader_try_cache_be(struct media_bits_reader_t *restrict reader)
{
	const uint8_t *restrict bytes;
	uintptr_t i, nbyte;
	media_bits_t v;
	if (!(nbyte = sizeof(media_bits_t) - ((reader->cache_bits + 7) >> 3)))
		goto label_okay;
	if ((bytes = reader->func.next_bytes(reader, &nbyte)) && nbyte)
	{
		v = reader->cache_value;
		for (i = 0; i < nbyte; ++i)
			v = (v << 8) | bytes[i];
		reader->cache_value = v;
		reader->cache_bits += (uint32_t) nbyte << 3;
		label_okay:
		return reader;
	}
	return NULL;
}

static struct media_bits_reader_t* media_bits_reader_read_bits_be(struct media_bits_reader_t *restrict reader, uint32_t nbits, media_bits_t *restrict vbits)
{
	media_bits_t v;
	uint32_t nb, sh;
	v = 0;
	do {
		if ((nb = (nbits <= reader->cache_bits)?nbits:reader->cache_bits))
		{
			sh = (reader->cache_bits -= nb);
			v = (v << nb) | (reader->cache_value >> sh);
			reader->cache_value &= ((media_bits_t) 1 << sh) - 1;
			nbits -= nb;
		}
	} while (nbits && reader->func.try_cache(reader));
	if (vbits) *vbits = v;
	if (!nbits) return reader;
	return NULL;
}

static struct media_bits_reader_t* media_bits_reader_push_back_be(struct media_bits_reader_t *restrict reader, uint32_t nbits, media_bits_t vbits)
{
	if (nbits + reader->cache_bits <= (sizeof(media_bits_t) << 3))
	{
		if (nbits < (sizeof(media_bits_t) << 3))
			vbits &= ((media_bits_t) 1 << nbits) - 1;
		reader->cache_value |= vbits << reader->cache_bits;
		reader->cache_bits += nbits;
		return reader;
	}
	return NULL;
}

// extern api

struct media_bits_reader_t* media_bits_reader_initial_be(struct media_bits_reader_t *restrict reader, const uint8_t *restrict d, uintptr_t n, media_bits_reader_next_bytes_f next_bytes)
{
	if (!next_bytes) next_bytes = media_bits_reader_next_bytes_default;
	reader->func.next_bytes = next_bytes;
	reader->func.try_cache = media_bits_reader_try_cache_be;
	reader->func.read_bits = media_bits_reader_read_bits_be;
	reader->func.push_back = media_bits_reader_push_back_be;
	reader->u8 = d;
	reader->size = n;
	reader->upos = 0;
	reader->cache_value = 0;
	reader->cache_bits = 0;
	return reader;
}

uintptr_t media_bits_reader_bits_pos(struct media_bits_reader_t *restrict reader)
{
	return reader->upos * 8 - reader->cache_bits;
}

uintptr_t media_bits_reader_bits_res(struct media_bits_reader_t *restrict reader)
{
	return (reader->size - reader->upos) * 8 + reader->cache_bits;
}

struct media_bits_reader_t* media_bits_reader_read_cache(struct media_bits_reader_t *restrict reader, uint32_t *restrict nbits, media_bits_t *restrict vbits)
{
	if (reader->cache_bits || reader->func.try_cache(reader))
	{
		if (nbits) *nbits = reader->cache_bits;
		if (vbits) *vbits = reader->cache_value;
		reader->cache_bits = 0;
		reader->cache_value = 0;
		return reader;
	}
	return NULL;
}
