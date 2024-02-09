#ifndef _iphyee_loader_h_
#define _iphyee_loader_h_

#include <refer.h>
#include <layer/layer.file.h>
#include <layer/layer.model.h>
#include <layer/layer.model.string.h>

typedef struct iphyee_loader_s iphyee_loader_s;

// o:iphyee.model
// 	o:iphyee.meshes (name => o:iphyee.mesh)
// 		o:iphyee.vertex
// 			e:iphyee.fbits
// 			d:iphyee.vertex (vec3 vertex)
// 			u:iphyee.vertex.count
// 		o:iphyee.normal
// 			e:iphyee.fbits
// 			d:iphyee.normal (vec3 normal)
// 			u:iphyee.normal.count
// 		o:iphyee.texture
// 			e:iphyee.fbits
// 			d:iphyee.texture (vec2 texture)
// 			u:iphyee.texture.count
// 			s:iphyee.texture.name
// 		o:iphyee.fragment
// 			e:iphyee.ubits
// 			b:iphyee.fragment.vertex
// 			b:iphyee.fragment.normal
// 			b:iphyee.fragment.texture
// 			d:iphyee.fragment (uint vertex_index?, uint normal_index?, uint texture_index?)
// 			u:iphyee.fragment.count
// 			*:iphyee.fraginfo
// 				e:iphyee.fraginfo
// 				o:iphyee.fraginfo
// 					e:iphyee.ubits
// 					d:iphyee.fraginfo (uint fragment_offset, uint fragment_number)
// 		o:iphyee.vertex.joint
// 			e:iphyee.ubits
// 			e:iphyee.fbits
// 			a:iphyee.vertex.joint (=> s joint_name)
// 			d:iphyee.vertex.weight (uint joint_index, float weight)
// 			u:iphyee.weight.count
// 			d:iphyee.vertex.bind (uint weight_offset, uint weight_number)
// 			u:iphyee.bind.count
// 	o:iphyee.bonex
// 		s:iphyee.bonex.name
// 		a:iphyee.bonex.fixed (=> *:iphyee.bonex.fixed)
// 			o:iphyee.bonex.fixed.coord
// 				s:iphyee.coord.name
// 				f:iphyee.coord.value
// 			o:iphyee.bonex.fixed.inode
// 				s:iphyee.inode.name
// 				f:iphyee.inode.value
// 				o:iphyee.inode.info
// 					f:iphyee.inode.minimum
// 					f:iphyee.inode.maximum
// 					f:iphyee.inode.multiplier
// 					f:iphyee.inode.addend

#define iphyee_loader_stype__x_fraginfo           "*:iphyee.fraginfo"
#define iphyee_loader_stype__x_bonex_fixed        "*:iphyee.bonex.fixed"
#define iphyee_loader_stype__e_fbits              "e:iphyee.fbits"
#define iphyee_loader_stype__e_ubits              "e:iphyee.ubits"
#define iphyee_loader_stype__e_fraginfo           "e:iphyee.fraginfo"
#define iphyee_loader_stype__a_vertex_joint       "a:iphyee.vertex.joint"
#define iphyee_loader_stype__a_bonex_fixed        "a:iphyee.bonex.fixed"
#define iphyee_loader_stype__o_model              "o:iphyee.model"
#define iphyee_loader_stype__o_meshes             "o:iphyee.meshes"
#define iphyee_loader_stype__o_mesh               "o:iphyee.mesh"
#define iphyee_loader_stype__o_vertex             "o:iphyee.vertex"
#define iphyee_loader_stype__o_normal             "o:iphyee.normal"
#define iphyee_loader_stype__o_texture            "o:iphyee.texture"
#define iphyee_loader_stype__o_fragment           "o:iphyee.fragment"
#define iphyee_loader_stype__o_fraginfo           "o:iphyee.fraginfo"
#define iphyee_loader_stype__o_vertex_joint       "o:iphyee.vertex.joint"
#define iphyee_loader_stype__o_bonex              "o:iphyee.bonex"
#define iphyee_loader_stype__o_bonex_fixed_coord  "o:iphyee.bonex.fixed.coord"
#define iphyee_loader_stype__o_bonex_fixed_inode  "o:iphyee.bonex.fixed.inode"
#define iphyee_loader_stype__o_inode_info         "o:iphyee.inode.info"

#define iphyee_loader_sname__x_fraginfo           iphyee_loader_stype__x_fraginfo
#define iphyee_loader_sname__u_vertex_count       "u:iphyee.vertex.count"
#define iphyee_loader_sname__u_normal_count       "u:iphyee.normal.count"
#define iphyee_loader_sname__u_texture_count      "u:iphyee.texture.count"
#define iphyee_loader_sname__u_fragment_count     "u:iphyee.fragment.count"
#define iphyee_loader_sname__u_weight_count       "u:iphyee.weight.count"
#define iphyee_loader_sname__u_bind_count         "u:iphyee.bind.count"
#define iphyee_loader_sname__f_coord_value        "f:iphyee.coord.value"
#define iphyee_loader_sname__f_inode_value        "f:iphyee.inode.value"
#define iphyee_loader_sname__f_inode_minimum      "f:iphyee.inode.minimum"
#define iphyee_loader_sname__f_inode_maximum      "f:iphyee.inode.maximum"
#define iphyee_loader_sname__f_inode_multiplier   "f:iphyee.inode.multiplier"
#define iphyee_loader_sname__f_inode_addend       "f:iphyee.inode.addend"
#define iphyee_loader_sname__b_fragment_vertex    "b:iphyee.fragment.vertex"
#define iphyee_loader_sname__b_fragment_normal    "b:iphyee.fragment.normal"
#define iphyee_loader_sname__b_fragment_texture   "b:iphyee.fragment.texture"
#define iphyee_loader_sname__s_texture_name       "s:iphyee.texture.name"
#define iphyee_loader_sname__s_bonex_name         "s:iphyee.bonex.name"
#define iphyee_loader_sname__s_coord_name         "s:iphyee.coord.name"
#define iphyee_loader_sname__s_inode_name         "s:iphyee.inode.name"
#define iphyee_loader_sname__d_vertex             "d:iphyee.vertex"
#define iphyee_loader_sname__d_normal             "d:iphyee.normal"
#define iphyee_loader_sname__d_texture            "d:iphyee.texture"
#define iphyee_loader_sname__d_fragment           "d:iphyee.fragment"
#define iphyee_loader_sname__d_fraginfo           "d:iphyee.fraginfo"
#define iphyee_loader_sname__d_vertex_weight      "d:iphyee.vertex.weight"
#define iphyee_loader_sname__d_vertex_bind        "d:iphyee.vertex.bind"
#define iphyee_loader_sname__e_fbits              iphyee_loader_stype__e_fbits
#define iphyee_loader_sname__e_ubits              iphyee_loader_stype__e_ubits
#define iphyee_loader_sname__e_fraginfo           iphyee_loader_stype__e_fraginfo
#define iphyee_loader_sname__a_vertex_joint       iphyee_loader_stype__a_vertex_joint
#define iphyee_loader_sname__a_bonex_fixed        iphyee_loader_stype__a_bonex_fixed
#define iphyee_loader_sname__o_model              iphyee_loader_stype__o_model
#define iphyee_loader_sname__o_meshes             iphyee_loader_stype__o_meshes
#define iphyee_loader_sname__o_mesh               iphyee_loader_stype__o_mesh
#define iphyee_loader_sname__o_vertex             iphyee_loader_stype__o_vertex
#define iphyee_loader_sname__o_normal             iphyee_loader_stype__o_normal
#define iphyee_loader_sname__o_texture            iphyee_loader_stype__o_texture
#define iphyee_loader_sname__o_fragment           iphyee_loader_stype__o_fragment
#define iphyee_loader_sname__o_fraginfo           iphyee_loader_stype__o_fraginfo
#define iphyee_loader_sname__o_vertex_joint       iphyee_loader_stype__o_vertex_joint
#define iphyee_loader_sname__o_bonex              iphyee_loader_stype__o_bonex
#define iphyee_loader_sname__o_bonex_fixed_coord  iphyee_loader_stype__o_bonex_fixed_coord
#define iphyee_loader_sname__o_bonex_fixed_inode  iphyee_loader_stype__o_bonex_fixed_inode
#define iphyee_loader_sname__o_inode_info         iphyee_loader_stype__o_inode_info

const iphyee_loader_s* iphyee_loader_alloc(void);

layer_model_item_s* iphyee_loader_parse_obj_by_memory(const iphyee_loader_s *restrict loader, const char *restrict data, uintptr_t size);
layer_model_item_s* iphyee_loader_parse_obj_by_file(const iphyee_loader_s *restrict loader, layer_file_s *restrict file, uint64_t offset, uint64_t size);
layer_model_item_s* iphyee_loader_parse_obj_by_path(const iphyee_loader_s *restrict loader, const char *restrict path);
layer_file_s* iphyee_loader_build_obj_to_file(const layer_model_item_s *restrict model);
const char* iphyee_loader_build_obj_to_path(const layer_model_item_s *restrict model, const char *restrict path);

#endif
