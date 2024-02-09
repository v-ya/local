#include "layer.model.h"
#include <inttypes.h>

typedef struct layer_model_item_data_s layer_model_item_data_s;

struct layer_model_item_data_s {
	layer_model_item_s item;
	layer_file_s *file;
	uint64_t offset;
	uint64_t size;
};

static void layer_model_item_data_free_func(layer_model_item_data_s *restrict r)
{
	if (r->file) refer_free(r->file);
	layer_model_item_free_func(&r->item);
}

static d_type_create(data, layer_model_type_s)
{
	layer_model_item_s *restrict r;
	if ((r = layer_model_item_alloc(sizeof(layer_model_item_data_s), t)))
		refer_set_free(r, (refer_free_f) layer_model_item_data_free_func);
	return r;
}
static d_type_copyto(data, layer_model_type_s, layer_model_item_data_s)
{
	if ((dst->file)) refer_free(dst->file);
	dst->file = (layer_file_s *) refer_save(src->file);
	dst->offset = src->offset;
	dst->size = src->size;
	return &dst->item;
}
static d_type_iprint(data, layer_model_type_s, layer_model_item_data_s)
{
	if (layer_model_item_iprint(&item->item, log))
	{
		if (item->file)
			mlog_printf(log->input, "\n+(file:%p)[%" PRIu64 "] @%" PRIu64 "+%" PRIu64 "\n",
				item->file, layer_file_size(item->file),
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

const layer_model_type_s* layer_model_type_create__data(const layer_model_s *restrict m, const char *restrict name, uint32_t type_minor)
{
	return layer_model_type_alloc(name, layer_model_type__data, type_minor, sizeof(layer_model_type_s), d_type_function(data, initial), NULL);
}

// api

const layer_model_item_s* layer_model_get_d(const layer_model_item_s *restrict i, struct layer_file_s *restrict *restrict file, uint64_t *restrict offset, uint64_t *restrict size)
{
	if (i->type_major == layer_model_type__data)
	{
		if (file) *file = ((const layer_model_item_data_s *) i)->file;
		if (offset) *offset = ((const layer_model_item_data_s *) i)->offset;
		if (size) *size = ((const layer_model_item_data_s *) i)->size;
		return i;
	}
	return NULL;
}
layer_model_item_s* layer_model_set_d(layer_model_item_s *restrict i, struct layer_file_s *restrict file, uint64_t offset, uint64_t size)
{
	uint64_t fsize;
	if (i->type_major == layer_model_type__data && (i->item_flag & layer_model_flag__write))
	{
		if (file)
		{
			fsize = layer_file_size(file);
			if (offset < fsize && size <= fsize - offset)
			{
				refer_save(file);
				label_set:
				if (((layer_model_item_data_s *) i)->file)
					refer_free(((layer_model_item_data_s *) i)->file);
				((layer_model_item_data_s *) i)->file = file;
				((layer_model_item_data_s *) i)->offset = offset;
				((layer_model_item_data_s *) i)->size = size;
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
