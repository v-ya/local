#ifndef _iphyee_glsl_model_type_
#define _iphyee_glsl_model_type_

#include "../standard/pointer.glsl"

// model base

struct iphyee_model_vec3xyz {
	float x;
	float y;
	float z;
};

struct iphyee_model_vec2uv {
	float u;
	float v;
};

struct iphyee_model_vtx2nml { // vertex => normal
	uint normal_offset;   // normal[] 中法线偏移
	uint normal_number;   // normal[] 中法线数量
};

struct iphyee_model_wjoint { // 关节关联权重
	uint joint_index;    // 关节索引 (mat4x4 joint[])
	float joint_weight;  // 关节权重 sum(wjoint[ejoint].weight) = 1
};

struct iphyee_model_ejoint { // 关节影响
	uint wjoint_offset;  // wjoint[] 中 wjoint 偏移
	uint wjoint_number;  // wjoint[] 中 wjoint 数量
};

struct iphyee_model_material { // 材质
	float diffuse;         // 漫反射强度
	float specular;        // 镜面反射强度
	float refraction;      // 折射强度 (全反射下将累加于<镜面反射强度>)
	float refridx;         // 折射率 (入射折射率，出射为此倒数)
};

struct iphyee_model_fusion { // 面元融合节点
	uint vertex;         // index of iphyee_model_vertex_array
	uint normal;         // index of iphyee_model_normal_array
	uint texture;        // index of iphyee_model_texture_array
	uint material;       // index of iphyee_model_material_array
};

// model array

_def_pointer_ iphyee_model_vertex_array;
_def_array_(4) iphyee_model_vertex_array {
	iphyee_model_vec3xyz vertex[];
};

_def_pointer_ iphyee_model_normal_array;
_def_array_(4) iphyee_model_normal_array {
	iphyee_model_vec3xyz normal[];
};

_def_pointer_ iphyee_model_vtx2nml_array;
_def_array_(4) iphyee_model_vtx2nml_array {
	iphyee_model_vtx2nml vtx2nml[];
};

_def_pointer_ iphyee_model_texture_array;
_def_array_(4) iphyee_model_texture_array {
	iphyee_model_vec2uv texture[];
};

_def_pointer_ iphyee_model_wjoint_array;
_def_array_(4) iphyee_model_wjoint_array {
	iphyee_model_wjoint wjoint[];
};

_def_pointer_ iphyee_model_ejoint_array;
_def_array_(4) iphyee_model_ejoint_array {
	iphyee_model_ejoint ejoint[];
};

_def_pointer_ iphyee_model_material_array;
_def_array_(4) iphyee_model_material_array {
	iphyee_model_material material[];
};

_def_pointer_ iphyee_model_fusion_array;
_def_array_(4) iphyee_model_fusion_array {
	iphyee_model_fusion fusion[];
};

// model data

struct iphyee_model_static_data {
	iphyee_model_vertex_array vertex;
	iphyee_model_normal_array normal;
	iphyee_model_vtx2nml_array vtx2nml;
	iphyee_model_texture_array texture;
	iphyee_model_wjoint_array wjoint;
	iphyee_model_ejoint_array ejoint;
	iphyee_model_material_array material;
	iphyee_model_fusion_array fusion;
};

struct iphyee_model_static_count {
	uint vertex;   // vertex count
	uint normal;   // normal count
	uint vtx2nml;  // vtx2nml count, must (vtx2nml == vertex) or (vtx2nml == 0)
	uint texture;  // texture count
	uint wjoint;   // wjoint count
	uint ejoint;   // ejoint count, must (ejoint == vertex) or (ejoint == 0)
	uint material; // material count
	uint fusion;   // fusion count, must (fusion % 3 == 0)
};

struct iphyee_model_dynamic_data {
	iphyee_model_vertex_array vertex;
	iphyee_model_normal_array normal;
};

struct iphyee_model_dynamic_count {
	uint vertex; // vertex count
	uint normal; // normal count
};

// model

struct iphyee_model_static {
	iphyee_model_static_data data;
	iphyee_model_static_count count;
};

struct iphyee_model_dynamic {
	iphyee_model_static static;
	iphyee_model_dynamic_data data;
	iphyee_model_dynamic_count count;
};

// dynamic.data.vertex[i] = dynamic.static.ejoint[i](dynamic.static.data.vertex[i])
// dynamic.data.normal[vtx2nml[i]] = dynamic.static.ejoint[i](dynamic.static.data.normal[vtx2nml[i]])

// model refer

_def_pointer_ iphyee_model_static_refer;
_def_array_(8) iphyee_model_static_refer {
	iphyee_model_static model;
};

_def_pointer_ iphyee_model_dynamic_refer;
_def_array_(8) iphyee_model_dynamic_refer {
	iphyee_model_dynamic model;
};

// model render

struct iphyee_model_render {
	iphyee_model_dynamic_refer model;
	uint render_enable;
	uint fusion_offset;
	uint fusion_number;
	uint texture_index;
};

_def_pointer_ iphyee_model_render_refer;
_def_array_(8) iphyee_model_render_refer {
	iphyee_model_render render;
};

#endif
