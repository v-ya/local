#ifndef _mpeg4_box_include_h_
#define _mpeg4_box_include_h_

#include "../mpeg4.box.h"
#include "../mpeg4.atom.h"
#include "../mpeg4.stuff.method.h"

struct mpeg4_atom_dump_t {
	uint32_t dump_level;
	uint32_t dump_samples;
	uint32_t timescale;
};

mpeg4$define$alloc(container);
mpeg4$define$alloc(container_full);
mpeg4$define$alloc(container_count);

mpeg4$define$find(unknow);
mpeg4$define$find(free_skip);
mpeg4$define$find(file);
mpeg4$define$find(ftyp);
mpeg4$define$find(mdat);
mpeg4$define$find(moov);
mpeg4$define$find(mvhd);
mpeg4$define$find(iods);
mpeg4$define$find(trak);
mpeg4$define$find(tkhd);
mpeg4$define$find(edts);
mpeg4$define$find(elst);
mpeg4$define$find(mdia);
mpeg4$define$find(mdhd);
mpeg4$define$find(hdlr);
mpeg4$define$find(elng);
mpeg4$define$find(minf);
mpeg4$define$find(nmhd);
mpeg4$define$find(vmhd);
mpeg4$define$find(smhd);
mpeg4$define$find(dinf);
mpeg4$define$find(dref);
mpeg4$define$find(url_);
mpeg4$define$find(urn_);
mpeg4$define$find(stbl);
mpeg4$define$find(stsd);
mpeg4$define$find(stts);
mpeg4$define$find(ctts);
mpeg4$define$find(stss);
mpeg4$define$find(stsz);
mpeg4$define$find(stsc);
mpeg4$define$find(stco);
mpeg4$define$find(co64);
mpeg4$define$find(sbgp);
mpeg4$define$find(udta);
mpeg4$define$find(meta);
mpeg4$define$find(ilst);
mpeg4$define$find(ilst_copyright);
mpeg4$define$find(ilst_copyright_data);

#endif
