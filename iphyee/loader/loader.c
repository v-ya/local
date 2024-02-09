#include "loader.h"

#define iphyee_loader_sname__  NULL
#define iphyee_loader_stype__  NULL
#define d_symbol(_m)      iphyee_loader_layer_custom__##_m
#define d_enum(_s, _v)    {.name = _s, .value = _v}
#define d_object(_m)      {.key = iphyee_loader_sname__##_m, .name = iphyee_loader_stype__##_m, .func = NULL}
#define d_objvar(_m, _t)  {.key = iphyee_loader_sname__##_m, .name = layer_model_stype__##_t, .func = NULL}

static const char *const d_symbol(x_fraginfo)[] = {
	iphyee_loader_stype__e_fraginfo,
	iphyee_loader_stype__o_fraginfo,
	NULL
};
static const char *const d_symbol(x_bonex_fixed)[] = {
	iphyee_loader_stype__o_bonex_fixed_coord,
	iphyee_loader_stype__o_bonex_fixed_inode,
	NULL
};
static const layer_model_custom_enum_t d_symbol(e_fbits)[] = {
	d_enum("float32", 32),
	d_enum("float64", 64),
	d_enum(NULL, 0)
};
static const layer_model_custom_enum_t d_symbol(e_ubits)[] = {
	d_enum("uint16", 16),
	d_enum("uint32", 32),
	d_enum("uint64", 64),
	d_enum(NULL, 0)
};
static const layer_model_custom_enum_t d_symbol(e_fraginfo)[] = {
	d_enum("tri-3", 3),
	d_enum("quad-4", 4),
	d_enum(NULL, 0)
};
static const layer_model_custom_object_t d_symbol(o_vertex)[] = {
	d_object(e_fbits),
	d_objvar(d_vertex, data),
	d_objvar(u_vertex_count, uint),
	d_object()
};
static const layer_model_custom_object_t d_symbol(o_normal)[] = {
	d_object(e_fbits),
	d_objvar(d_normal, data),
	d_objvar(u_normal_count, uint),
	d_object()
};
static const layer_model_custom_object_t d_symbol(o_texture)[] = {
	d_object(e_fbits),
	d_objvar(d_texture, data),
	d_objvar(u_texture_count, uint),
	d_objvar(s_texture_name, string),
	d_object()
};
static const layer_model_custom_object_t d_symbol(o_fraginfo)[] = {
	d_object(e_ubits),
	d_objvar(d_fraginfo, data),
	d_object()
};
static const layer_model_custom_object_t d_symbol(o_fragment)[] = {
	d_object(e_ubits),
	d_objvar(b_fragment_vertex, boolean),
	d_objvar(b_fragment_normal, boolean),
	d_objvar(b_fragment_texture, boolean),
	d_objvar(d_fragment, data),
	d_objvar(u_fragment_count, uint),
	d_object(x_fraginfo),
	d_object()
};
static const layer_model_custom_object_t d_symbol(o_vertex_joint)[] = {
	d_object(e_ubits),
	d_object(e_fbits),
	d_object(a_vertex_joint),
	d_objvar(d_vertex_weight, data),
	d_objvar(u_weight_count, uint),
	d_objvar(d_vertex_bind, data),
	d_objvar(u_bind_count, uint),
	d_object()
};
static const layer_model_custom_object_t d_symbol(o_mesh)[] = {
	d_object(o_vertex),
	d_object(o_normal),
	d_object(o_texture),
	d_object(o_fragment),
	d_object(o_vertex_joint),
	d_object()
};
static const layer_model_custom_object_t d_symbol(o_bonex_fixed_coord)[] = {
	d_objvar(s_coord_name, string),
	d_objvar(f_coord_value, float),
	d_object()
};
static const layer_model_custom_object_t d_symbol(o_inode_info)[] = {
	d_objvar(f_inode_minimum, float),
	d_objvar(f_inode_maximum, float),
	d_objvar(f_inode_multiplier, float),
	d_objvar(f_inode_addend, float),
	d_object()
};
static const layer_model_custom_object_t d_symbol(o_bonex_fixed_inode)[] = {
	d_objvar(s_inode_name, string),
	d_objvar(f_inode_value, float),
	d_object(o_inode_info),
	d_object()
};
static const layer_model_custom_object_t d_symbol(o_bonex)[] = {
	d_objvar(s_bonex_name, string),
	d_object(a_bonex_fixed),
	d_object()
};
static const layer_model_custom_object_t d_symbol(o_model)[] = {
	d_object(o_meshes),
	d_object(o_bonex),
	d_object()
};

#undef d_enum
#undef d_object
#undef d_objvar

static void iphyee_loader_free_func(iphyee_loader_s *restrict r)
{
	if (r->model) refer_free(r->model);
}

const iphyee_loader_s* iphyee_loader_alloc(void)
{
	iphyee_loader_s *restrict r;
	if ((r = (iphyee_loader_s *) refer_alloz(sizeof(iphyee_loader_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_loader_free_func);
		#define d_create_any(_m)          layer_model_create_any(r->model, iphyee_loader_stype__##_m, d_symbol(_m))
		#define d_create_enum(_m, _dv)    layer_model_create_enum(r->model, iphyee_loader_stype__##_m, d_symbol(_m), _dv) && layer_model_create_type__enum(r->model, iphyee_loader_stype__##_m, 0, iphyee_loader_stype__##_m)
		#define d_create_array(_m, _c)    layer_model_create_type__array(r->model, iphyee_loader_stype__##_m, 0, _c)
		#define d_create_object(_m)       layer_model_create_object(r->model, iphyee_loader_stype__##_m, d_symbol(_m)) && layer_model_create_type__object(r->model, iphyee_loader_stype__##_m, 0, layer_model_stype__null, iphyee_loader_stype__##_m)
		#define d_create_objpool(_m, _c)  layer_model_create_type__object(r->model, iphyee_loader_stype__##_m, 0, _c, NULL)
		if ((r->model = layer_model_alloc()) &&
			d_create_any(x_fraginfo) &&
			d_create_any(x_bonex_fixed) &&
			d_create_enum(e_fbits, 32) &&
			d_create_enum(e_ubits, 32) &&
			d_create_enum(e_fraginfo, 3) &&
			d_create_array(a_vertex_joint, layer_model_stype__string) &&
			d_create_array(a_bonex_fixed, iphyee_loader_stype__x_bonex_fixed) &&
			d_create_object(o_vertex) &&
			d_create_object(o_normal) &&
			d_create_object(o_texture) &&
			d_create_object(o_fraginfo) &&
			d_create_object(o_fragment) &&
			d_create_object(o_vertex_joint) &&
			d_create_object(o_mesh) &&
			d_create_objpool(o_meshes, iphyee_loader_stype__o_mesh) &&
			d_create_object(o_bonex_fixed_coord) &&
			d_create_object(o_inode_info) &&
			d_create_object(o_bonex_fixed_inode) &&
			d_create_object(o_bonex) &&
			d_create_object(o_model)
		) return r;
		#undef d_create_any
		#undef d_create_enum
		#undef d_create_array
		#undef d_create_object
		#undef d_create_objpool
		refer_free(r);
	}
	return NULL;
}
