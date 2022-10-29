#include "container.jpeg.h"

struct media_container_jpeg_parse_scan_data_t {
	uint64_t offset;
	enum mi_jpeg_segment_type_t type;
	uint32_t mark;
};

static void* media_container_jpeg_parse_scan_func(struct media_container_jpeg_parse_scan_data_t *restrict pri, uint64_t offset, const uint8_t *restrict data, uintptr_t size)
{
	uintptr_t i;
	for (i = 0; i < size; ++i)
	{
		if (!pri->mark)
		{
			if (data[i] == mi_jpeg_segment_type__mark)
				pri->mark = 1;
		}
		else
		{
			switch (data[i])
			{
				case mi_jpeg_segment_type__nul:
				case mi_jpeg_segment_type__rst0:
				case mi_jpeg_segment_type__rst1:
				case mi_jpeg_segment_type__rst2:
				case mi_jpeg_segment_type__rst3:
				case mi_jpeg_segment_type__rst4:
				case mi_jpeg_segment_type__rst5:
				case mi_jpeg_segment_type__rst6:
				case mi_jpeg_segment_type__rst7:
					pri->mark = 0;
					// fall through
				case mi_jpeg_segment_type__mark:
					break;
				default:
					pri->offset = offset + i + 1;
					pri->type = data[i];
					pri->mark = 0;
					return pri;
			}
		}
	}
	return NULL;
}

static struct media_container_inner_s* media_container_jpeg_parse_dump(struct media_container_inner_s *restrict ci)
{
	struct media_container_jpeg_parse_scan_data_t data;
	uint8_t buffer[256];
	uintptr_t size;
	uint16_t length;
	data.offset = 0;
	data.type = mi_jpeg_segment_type__nul;
	data.mark = 0;
	while (media_io_scan(ci->io, (media_io_user_scan_f) media_container_jpeg_parse_scan_func, &data))
	{
		if (media_io_offset(ci->io, &data.offset) != data.offset)
			goto label_fail;
		switch (data.type)
		{
			case mi_jpeg_segment_type__rst0:
			case mi_jpeg_segment_type__rst1:
			case mi_jpeg_segment_type__rst2:
			case mi_jpeg_segment_type__rst3:
			case mi_jpeg_segment_type__rst4:
			case mi_jpeg_segment_type__rst5:
			case mi_jpeg_segment_type__rst6:
			case mi_jpeg_segment_type__rst7:
			case mi_jpeg_segment_type__soi:
			case mi_jpeg_segment_type__eoi:
				size = 0;
				break;
			default:
				if (!media_io_read(ci->io, &length, sizeof(length)))
					goto label_fail;
				size = (uintptr_t) media_n2be_16(length);
				if (size < 2) goto label_fail;
				size -= 2;
				break;
		}
		media_info(ci->media, "%02x ... @%zu [%zu]", data.type, (uintptr_t) data.offset - 2, size);
		data.offset = media_io_offset(ci->io, NULL) + size;
		if (size > sizeof(buffer)) size = sizeof(buffer);
		if (size)
		{
			if (media_io_read(ci->io, buffer, size))
				media_mlog_print_rawdata(ci->media->mlog_verbose, NULL, buffer, size);
		}
		if (media_io_offset(ci->io, &data.offset) != data.offset)
			goto label_fail;
	}
	media_info(ci->media, "scan end ... %zu/%zu", (uintptr_t) media_io_offset(ci->io, NULL), (uintptr_t) media_io_size(ci->io));
	return ci;
	label_fail:
	return NULL;
}

/*
	d8 mi_jpeg_segment_type__soi   = 0xd8, // start of image (NO-PARAMS)
	e0 mi_jpeg_segment_type__app0  = 0xe0, // application segment 0  (JFIF (len >=14) / JFXX (len >= 6) / AVI MJPEG)
	db mi_jpeg_segment_type__dqt   = 0xdb, // define quantization table(s)
	db mi_jpeg_segment_type__dqt   = 0xdb, // define quantization table(s)
	c0 mi_jpeg_segment_type__sof0  = 0xc0, // start of frame (baseline jpeg)
	c4 mi_jpeg_segment_type__dht   = 0xc4, // define huffman tables
	c4 mi_jpeg_segment_type__dht   = 0xc4, // define huffman tables
	c4 mi_jpeg_segment_type__dht   = 0xc4, // define huffman tables
	c4 mi_jpeg_segment_type__dht   = 0xc4, // define huffman tables
	da mi_jpeg_segment_type__sos   = 0xda, // start of scan
	d9 mi_jpeg_segment_type__eoi   = 0xd9, // end of image (NO-PARAMS)
*/

d_media_container__parse_head(jpeg)
{
	struct media_container_inner_s *restrict ci;
	ci = c->inner;
	media_container_jpeg_parse_dump(ci);
	media_warning(ci->media, "image/jpeg parse head ... fail");
	return NULL;
}
