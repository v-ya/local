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
	return mi_jpeg_codec_load_sof(codec, io, scan->segment_data_length);
}

static struct mi_jpeg_codec_s* media_container_jpeg_parse__sos(struct mi_jpeg_codec_s *restrict codec, struct mi_jpeg_scan_t *restrict scan, media_io_s *restrict io, struct media_container_s *restrict c)
{
	const struct mi_jpeg_sof_s *restrict sof;
	const struct mi_jpeg_sos_s *restrict sos;
	const char *restrict frame_name;
	struct media_stream_s *restrict st;
	struct mi_jpeg_scan_t scan_skip_frame;
	struct media_stack__ozp_t ozp;
	if (mi_jpeg_codec_load_sos(codec, io, scan->segment_data_length) &&
		(frame_name = mi_jpeg_test_frame_name(sof = codec->sof, sos = codec->sos)) &&
		(st = media_container_new_stream(c, media_st_image, frame_name)) &&
		media_attr_set_int(st->attr, media_nasi_width, sof->width) &&
		media_attr_set_int(st->attr, media_nasi_height, sof->height) &&
		media_attr_set_int(st->attr, media_nasi_bpc, sof->ch_depth) &&
		media_attr_set_int(st->attr, media_nasi_jpeg_channel, sos->ch_number))
	{
		scan_skip_frame = *scan;
		if (media_codec_jpeg_scan_fetch(&scan_skip_frame, io))
		{
			struct mi_jpeg_frame_info_s *fi;
			ozp.offset = scan->offset_segment_next;
			ozp.size = (uintptr_t) (scan_skip_frame.offset_segment_start - ozp.offset);
			scan->offset_segment_next = scan_skip_frame.offset_segment_start;
			if ((ozp.pri = fi = mi_jpeg_codec_create_frame_info(codec)))
			{
				// media_mlog_print_rawdata(c->inner->media->mlog_verbose, "frame", fi->frame, sizeof(struct mi_jpeg_frame_t));
				// media_mlog_print_rawdata(c->inner->media->mlog_verbose, "ch[]", fi->ch, sizeof(struct mi_jpeg_frame_ch_t) * fi->frame->channel);
				// media_mlog_print_rawdata(c->inner->media->mlog_verbose, "q", fi->q, fi->q_size);
				// media_mlog_print_rawdata(c->inner->media->mlog_verbose, "h", fi->h, fi->h_size);
				if (!media_stack_push(st->stack, &ozp))
					codec = NULL;
				refer_free(ozp.pri);
				return codec;
			}
		}
	}
	return NULL;
}

d_media_container__parse_head(jpeg)
{
	struct media_container_inner_s *restrict ci;
	struct mi_jpeg_codec_s *restrict pri, *restrict codec;
	struct mi_jpeg_scan_t scan;
	ci = c->inner;
	pri = (struct mi_jpeg_codec_s *) ci->pri_data;
	codec = NULL;
	media_codec_jpeg_scan_initial(&scan);
	while (pri && media_codec_jpeg_scan_fetch(&scan, ci->io))
	{
		switch (scan.type)
		{
			case mi_jpeg_segment_type__soi:
				codec = pri;
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
