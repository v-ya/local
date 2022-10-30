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
			d_sp(soi, "start of image") &&
			d_sp(eoi, "end of image") &&
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
			if (length > 512) length = 512;
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
				if (seg && seg->parse)
				{
					seg->parse(p, t, length);
					length = 0;
				}
				else jpeg_parser_print_rawdata(p->m, name, t->data + data_pos, length);
				tmlog_sub(p->td, 1);
			}
			t->pos += length;
		}
		else mlog_printf(p->m, "@(%8zu)+? [error] %02x (%s): %s\n", this_pos, type, name, desc);
		last_pos = t->pos;
	}
	mlog_printf(p->m, "jpeg tail: %zu/%zu\n", t->pos, t->size);
}
