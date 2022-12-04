#ifndef _media_image_container_jpeg_h_
#define _media_image_container_jpeg_h_

#include "container.h"
#include "codec.jpeg.h"

struct media_stream_pri_jpeg_s {
	struct mi_jpeg_codec_s *codec;
	// info ...
	uint32_t width;
	uint32_t height;
	uint32_t bpc;
	uint32_t channel;
};

// MUST: sof0(0xc0) sof1(0xc1) dht(0xc4) soi(0xd8) eoi(0xd9) sos(0xda) dqt(0xdb) dri(0xdd) app0(0xe0) com(0xfe)
// native data is big-order

// container

#define media_container__initial_judge__jpeg NULL
d_media_container__create_pri(jpeg);
d_media_container__parse_head(jpeg);
#define media_container__parse_tail__jpeg NULL
#define media_container__build_head__jpeg NULL
#define media_container__build_tail__jpeg NULL

// stream

d_media_stream__initial_judge(jpeg);
d_media_stream__create_pri(jpeg);
d_media_stream__spec_append(jpeg);

#endif
