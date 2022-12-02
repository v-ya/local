#include "container.jpeg.h"
#include <inttypes.h>

static struct mi_jpeg_codec_s* media_container_jpeg_parse__dqt(struct mi_jpeg_codec_s *restrict codec, struct mi_jpeg_scan_t *restrict scan, media_io_s *restrict io)
{
	return mi_jpeg_codec_load_q(codec, io, scan->segment_data_length);
}

static struct mi_jpeg_codec_s* media_container_jpeg_parse__dht(struct mi_jpeg_codec_s *restrict codec, struct mi_jpeg_scan_t *restrict scan, media_io_s *restrict io)
{
	return mi_jpeg_codec_load_h(codec, io, scan->segment_data_length);
}

static struct mi_jpeg_codec_s* media_container_jpeg_parse__sof(struct mi_jpeg_codec_s *restrict codec, struct mi_jpeg_scan_t *restrict scan, media_io_s *restrict io, struct media_container_s *restrict c)
{
	const struct mi_jpeg_sof_s *restrict sof;
	struct media_attr_s *restrict attr;
	if (mi_jpeg_codec_load_sof(codec, io, scan->segment_data_length) && (sof = codec->sof))
	{
		attr = c->inner->attr;
		if (media_attr_set_int(attr, media_naci_width, sof->width) &&
			media_attr_set_int(attr, media_naci_height, sof->height) &&
			media_attr_set_int(attr, media_naci_bpc, sof->ch_depth) &&
			media_attr_set_int(attr, media_naci_jpeg_channel, sof->ch_number)
			) return codec;
	}
	return NULL;
}

static struct mi_jpeg_codec_s* media_container_jpeg_parse__sos(struct mi_jpeg_codec_s *restrict codec, struct mi_jpeg_scan_t *restrict scan, media_io_s *restrict io, struct media_container_s *restrict c)
{
	const struct mi_jpeg_sos_s *restrict sos;
	struct mi_jpeg_scan_t scan_skip_frame;
	if (mi_jpeg_codec_load_sos(codec, io, scan->segment_data_length) && (sos = codec->sos))
	{
		scan_skip_frame = *scan;
		if (media_codec_jpeg_scan_fetch(&scan_skip_frame, io))
		{
			media_verbose(c->inner->media, "image/jpeg frame at: %" PRIu64 ", size: %" PRIu64, scan->offset_segment_next, scan_skip_frame.offset_segment_start - scan->offset_segment_next);
			scan->offset_segment_next = scan_skip_frame.offset_segment_start;
			return codec;
		}
	}
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
	struct media_container_pri_jpeg_s *restrict pri;
	struct mi_jpeg_codec_s *restrict codec;
	struct mi_jpeg_scan_t scan;
	ci = c->inner;
	pri = (struct media_container_pri_jpeg_s *) ci->pri_data;
	codec = NULL;
	media_codec_jpeg_scan_initial(&scan);
	while (pri && media_codec_jpeg_scan_fetch(&scan, ci->io))
	{
		switch (scan.type)
		{
			case mi_jpeg_segment_type__soi:
				codec = pri->codec;
				break;
			case mi_jpeg_segment_type__eoi:
				pri = NULL;
				break;
			case mi_jpeg_segment_type__dqt:
				if (codec) codec = media_container_jpeg_parse__dqt(codec, &scan, ci->io);
				break;
			case mi_jpeg_segment_type__dht:
				if (codec) codec = media_container_jpeg_parse__dht(codec, &scan, ci->io);
				break;
			case mi_jpeg_segment_type__sof0:
			case mi_jpeg_segment_type__sof1:
				if (codec) codec = media_container_jpeg_parse__sof(codec, &scan, ci->io, c);
				break;
			case mi_jpeg_segment_type__sos:
				if (codec) codec = media_container_jpeg_parse__sos(codec, &scan, ci->io, c);
				break;
			default: break;
		}
		if (!codec)
		{
			media_warning(ci->media, "image/jpeg parse type(%02x) at offset(%" PRIu64 ") fail", scan.type, scan.offset_segment_start);
			goto label_fail;
		}
	}
	if (!pri) return c;
	label_fail:
	return NULL;
}
