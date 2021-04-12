#ifndef _mpeg4_box_include_h_
#define _mpeg4_box_include_h_

#include "../mpeg4.box.h"
#include "../mpeg4.inst.h"

struct mpeg4_dump_data_t {
	uint32_t timescale;
};

mpeg4$define$dump(box, unknow);

mpeg4$define$dump(box, container);

mpeg4$define$dump(box, ftyp);

mpeg4$define$dump(box, free_skip);

mpeg4$define$dump(box, mdat);

mpeg4$define$dump(box, mvhd);

mpeg4$define$dump(box, tkhd);

mpeg4$define$dump(box, mdhd);

mpeg4$define$dump(box, hdlr);

mpeg4$define$dump(box, nmhd);

mpeg4$define$dump(box, elng);

mpeg4$define$dump(box, stsd);

mpeg4$define$dump(box, stts);

mpeg4$define$dump(box, ctts);

#endif
