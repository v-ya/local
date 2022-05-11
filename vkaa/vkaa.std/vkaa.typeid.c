#include "std.h"

vkaa_std_typeid_s* vkaa_std_typeid_alloc(vkaa_tpool_s *restrict tpool)
{
	struct vkaa_std_typeid_s *restrict r;
	if ((r = (struct vkaa_std_typeid_s *) refer_alloz(sizeof(struct vkaa_std_typeid_s))))
	{
		r->id_void = vkaa_tpool_genid(tpool);
		r->id_null = vkaa_tpool_genid(tpool);
		r->id_scope = vkaa_tpool_genid(tpool);
		r->id_syntax = vkaa_tpool_genid(tpool);
		r->id_marco = vkaa_tpool_genid(tpool);
		r->id_function = vkaa_tpool_genid(tpool);
		r->id_string = vkaa_tpool_genid(tpool);
		r->id_bool = vkaa_tpool_genid(tpool);
		r->id_uint = vkaa_tpool_genid(tpool);
		r->id_int = vkaa_tpool_genid(tpool);
		r->id_float = vkaa_tpool_genid(tpool);
	}
	return r;
}
