#include "elfin.h"
#include "arch/arch.h"
#include <stdlib.h>
#include <fsys.h>

elfin_item_s* elfin_load_elfin_by_memory(const elfin_inst_s *restrict inst, const uint8_t *restrict data, uintptr_t size)
{
	const struct elfin_indent_t *restrict indent;
	if (size >= sizeof(struct elfin_indent_t))
	{
		indent = (const struct elfin_indent_t *) data;
		if (indent->version == elfin_version__current)
		{
			if (indent->class == elfin_indent_class__64)
			{
				if (indent->data == elfin_indent_data__2c_le)
					return elfin_inner_load_2c_le_64(inst, data, size);
			}
		}
	}
	return NULL;
}

elfin_item_s* elfin_load_elfin_by_path(const elfin_inst_s *restrict inst, const char *restrict path)
{
	elfin_item_s *restrict r;
	fsys_file_s *restrict fp;
	uint8_t *restrict data;
	uintptr_t n;
	uint64_t size;
	r = NULL;
	if ((fp = fsys_file_alloc(path, fsys_file_flag_read)))
	{
		if (fsys_file_attr_size(fp, &size) && (n = (uintptr_t) size) && (data = (uint8_t *) malloc(n)))
		{
			if (fsys_file_read(fp, data, n, &n) && n == (uintptr_t) size)
				r = elfin_load_elfin_by_memory(inst, data, n);
			free(data);
		}
		refer_free(fp);
	}
	return r;
}
