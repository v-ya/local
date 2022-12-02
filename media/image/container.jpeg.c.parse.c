#include "container.jpeg.h"

static struct media_container_s* media_container_jpeg_parse(struct media_container_s *restrict c, struct mi_jpeg_scan_t *restrict scan)
{
	// c->inner->pri_data;
	media_verbose(c->inner->media, "[jpeg](%02x) sp-start: %lu, sp-data: %lu, data-length: %zu", scan->type, scan->offset_segment_start, scan->offset_segment_data, scan->segment_data_length);
	return c;
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
	struct mi_jpeg_scan_t scan;
	ci = c->inner;
	media_codec_jpeg_scan_initial(&scan);
	while (media_codec_jpeg_scan_fetch(&scan, ci->io))
	{
		if (!media_container_jpeg_parse(c, &scan))
			goto label_fail;
	}
	return c;
	label_fail:
	return NULL;
}
