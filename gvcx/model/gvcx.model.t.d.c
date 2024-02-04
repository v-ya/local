#include "gvcx.model.h"
#include <inttypes.h>

typedef struct gvcx_model_item_data_s gvcx_model_item_data_s;

struct gvcx_model_item_data_s {
	gvcx_model_item_s item;
	gvcx_file_s *file;
	uint64_t offset;
	uint64_t size;
};

static void gvcx_model_item_data_free_func(gvcx_model_item_data_s *restrict r)
{
	if (r->file) refer_free(r->file);
	gvcx_model_item_free_func(&r->item);
}

static d_type_create(data, gvcx_model_type_s)
{
	gvcx_model_item_s *restrict r;
	if ((r = gvcx_model_item_alloc(sizeof(gvcx_model_item_data_s), t)))
		refer_set_free(r, (refer_free_f) gvcx_model_item_data_free_func);
	return r;
}
static d_type_copyto(data, gvcx_model_type_s, gvcx_model_item_data_s)
{
	if ((dst->file)) refer_free(dst->file);
	dst->file = (gvcx_file_s *) refer_save(src->file);
	dst->offset = src->offset;
	dst->size = src->size;
	return &dst->item;
}
static d_type_iprint(data, gvcx_model_type_s, gvcx_model_item_data_s)
{
	if (gvcx_model_item_iprint(&item->item, log))
	{
		if (item->file)
			mlog_printf(log->input, "\n+(file:%p)[%" PRIu64 "] @%" PRIu64 "+%" PRIu64 "\n",
				item->file, gvcx_file_size(item->file),
				item->offset, item->size);
		else mlog_printf(log->input, "\n");
	}
}
static d_type_initial(data, void)
{
	t->create = d_type_function(data, create);
	t->copyto = d_type_function(data, copyto);
	t->iprint = d_type_function(data, iprint);
	return t;
}

const gvcx_model_type_s* gvcx_model_type_create__data(const gvcx_model_s *restrict m, const char *restrict name, uint32_t type_minor)
{
	return gvcx_model_type_alloc(name, gvcx_model_type__data, type_minor, sizeof(gvcx_model_type_s), d_type_function(data, initial), NULL);
}

// api

const gvcx_model_item_s* gvcx_model_get_d(const gvcx_model_item_s *restrict i, struct gvcx_file_s *restrict *restrict file, uint64_t *restrict offset, uint64_t *restrict size)
{
	if (i->type_major == gvcx_model_type__data)
	{
		if (file) *file = ((const gvcx_model_item_data_s *) i)->file;
		if (offset) *offset = ((const gvcx_model_item_data_s *) i)->offset;
		if (size) *size = ((const gvcx_model_item_data_s *) i)->size;
		return i;
	}
	return NULL;
}
gvcx_model_item_s* gvcx_model_set_d(gvcx_model_item_s *restrict i, struct gvcx_file_s *restrict file, uint64_t offset, uint64_t size)
{
	uint64_t fsize;
	if (i->type_major == gvcx_model_type__data && (i->item_flag & gvcx_model_flag__write))
	{
		if (file)
		{
			fsize = gvcx_file_size(file);
			if (offset < fsize && size <= fsize - offset)
			{
				refer_save(file);
				label_set:
				if (((gvcx_model_item_data_s *) i)->file)
					refer_free(((gvcx_model_item_data_s *) i)->file);
				((gvcx_model_item_data_s *) i)->file = file;
				((gvcx_model_item_data_s *) i)->offset = offset;
				((gvcx_model_item_data_s *) i)->size = size;
				return i;
			}
		}
		else
		{
			offset = size = 0;
			goto label_set;
		}
	}
	return NULL;
}
