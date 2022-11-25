#include "parser.h"

static void jpeg_parser_free_func(jpeg_parser_s *restrict r)
{
	if (r->m) refer_free(r->m);
	if (r->td) refer_free(r->td);
	hashmap_uini(&r->type2parser);
}

jpeg_parser_s* jpeg_parser_alloc(mlog_s *restrict m, tmlog_data_s *restrict td)
{
	jpeg_parser_s *restrict r;
	if ((r = (jpeg_parser_s *) refer_alloz(sizeof(jpeg_parser_s))))
	{
		refer_set_free(r, (refer_free_f) jpeg_parser_free_func);
		r->m = (mlog_s *) refer_save(m);
		r->td = (tmlog_data_s *) refer_save(td);
		if (
			hashmap_init(&r->type2parser) &&
			#define d_sp(_name, _desc)  jpeg_parser_add_segment(r, jpeg_segment_type__##_name, #_name, _desc, jpeg_parser_segment__##_name)
			d_sp(dht,   "define huffman table(s)") &&
			d_sp(sof0,  "start of frame (non-differential, huffman coding) (baseline DCT)") &&
			d_sp(sof1,  "start of frame (non-differential, huffman coding) (extended sequential DCT)") &&
			d_sp(sof2,  "start of frame (non-differential, huffman coding) (progressive DCT)") &&
			d_sp(sof3,  "start of frame (non-differential, huffman coding) (lossless (sequential))") &&
			d_sp(sof9,  "start of frame (non-differential, arithmetic coding) (extended sequential DCT)") &&
			d_sp(sof10, "start of frame (non-differential, arithmetic coding) (progressive DCT)") &&
			d_sp(sof11, "start of frame (non-differential, arithmetic coding) (lossless (sequential))") &&
			d_sp(rst0,  "* restart marker 0") &&
			d_sp(rst1,  "* restart marker 1") &&
			d_sp(rst2,  "* restart marker 2") &&
			d_sp(rst3,  "* restart marker 3") &&
			d_sp(rst4,  "* restart marker 4") &&
			d_sp(rst5,  "* restart marker 5") &&
			d_sp(rst6,  "* restart marker 6") &&
			d_sp(rst7,  "* restart marker 7") &&
			d_sp(soi,   "* start of image") &&
			d_sp(eoi,   "* end of image") &&
			d_sp(sos,   "start of scan") &&
			d_sp(dqt,   "define quantization table(s)") &&
			d_sp(dhp,   "define hierarchical progression") &&
			#undef d_sp
		1) return r;
		refer_free(r);
	}
	return NULL;
}

void jpeg_parser_done(jpeg_parser_s *restrict p, jpeg_parser_target_t *restrict t)
{
	const jpeg_parser_segment_s *restrict seg;
	const char *restrict name, *restrict desc;
	uintptr_t last_pos, this_pos, data_pos, length;
	enum jpeg_segment_type_t type;
	mlog_printf(p->m, "jpeg size:[%zu] pos@%zu\n", t->size, last_pos = t->pos);
	while (jpeg_parser_scan_segment(t, &type))
	{
		this_pos = t->pos - 2;
		// print skip ...
		if (this_pos > last_pos)
		{
			mlog_printf(p->m, "@(%8zu)+%zu skip ...\n", last_pos, length = this_pos - last_pos);
			if (length > 32) length = 32;
			tmlog_add(p->td, 1);
			jpeg_parser_print_rawdata(p->m, "unknow", t->data + last_pos, length);
			tmlog_sub(p->td, 1);
		}
		// get segment parser
		name = desc = NULL;
		if ((seg = jpeg_parser_get_segment(p, type)))
		{
			name = seg->name;
			desc = seg->desc;
		}
		if (!name) name = "?";
		if (!desc) desc = "unknow";
		// get segment length
		if (jpeg_parser_get_segment_length(t, type, &length))
		{
			data_pos = t->pos;
			mlog_printf(p->m, "@(%8zu)+%zu [%5zu] %02x (%s): %s\n", this_pos, data_pos - this_pos, length, type, name, desc);
			if (length)
			{
				tmlog_add(p->td, 1);
				if (seg && seg->parse && !seg->parse(p, t, length))
					mlog_printf(p->m, "!! (%s) parse fail !!\n", name);
				if (t->pos < data_pos + length)
				{
					length = data_pos + length - t->pos;
					mlog_printf(p->m, "@(%8zu)+%zu ignore ...\n", t->pos, length);
					jpeg_parser_print_rawdata(p->m, NULL, t->data + t->pos, length);
					t->pos += length;
				}
				tmlog_sub(p->td, 1);
			}
		}
		else mlog_printf(p->m, "@(%8zu)+? [error] %02x (%s): %s\n", this_pos, type, name, desc);
		last_pos = t->pos;
	}
	mlog_printf(p->m, "jpeg tail: %zu/%zu\n", t->pos, t->size);
}
