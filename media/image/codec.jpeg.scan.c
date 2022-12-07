
#include "codec.jpeg.h"
#include <memory.h>

struct mi_jpeg_scan_data_t {
	uint64_t offset;
	enum mi_jpeg_segment_type_t type;
	uint32_t mark;
};

static void* media_codec_jpeg_scan_func(struct mi_jpeg_scan_data_t *restrict scan, uint64_t offset, const uint8_t *restrict data, uintptr_t size)
{
	uintptr_t i;
	for (i = 0; i < size; ++i)
	{
		if (!scan->mark)
		{
			if (data[i] == mi_jpeg_segment_type__mark)
				scan->mark = 1;
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
					scan->mark = 0;
					// fall through
				case mi_jpeg_segment_type__mark:
					break;
				default:
					scan->offset = offset + i + 1;
					scan->type = data[i];
					scan->mark = 0;
					return scan;
			}
		}
	}
	return NULL;
}

void media_codec_jpeg_scan_initial(struct mi_jpeg_scan_t *restrict scan)
{
	scan->offset_segment_start = 0;
	scan->offset_segment_data = 0;
	scan->offset_segment_next = 0;
	scan->segment_data_length = 0;
	scan->type = mi_jpeg_segment_type__nul;
}

struct mi_jpeg_scan_t* media_codec_jpeg_scan_fetch(struct mi_jpeg_scan_t *restrict scan, struct media_io_s *restrict io)
{
	struct mi_jpeg_scan_data_t data;
	uintptr_t size;
	uint16_t length;
	data.offset = scan->offset_segment_next;
	data.type = mi_jpeg_segment_type__nul;
	data.mark = 0;
	if (media_io_offset(io, &data.offset) == data.offset &&
		media_io_scan(io, (media_io_user_scan_f) media_codec_jpeg_scan_func, &data) &&
		media_io_offset(io, &data.offset) == data.offset)
	{
		switch (data.type)
		{
			case mi_jpeg_segment_type__tem:
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
				if (media_io_read(io, &length, sizeof(length)) != sizeof(length))
					goto label_fail;
				size = (uintptr_t) media_n2be_16(length);
				if (size < sizeof(length))
					goto label_fail;
				break;
		}
		scan->offset_segment_start = data.offset - 2;
		scan->offset_segment_data = data.offset;
		if (size)
		{
			scan->offset_segment_data += sizeof(length);
			size -= sizeof(length);
		}
		scan->offset_segment_next = scan->offset_segment_data + size;
		scan->segment_data_length = size;
		scan->type = data.type;
		return scan;
	}
	label_fail:
	return NULL;
}
