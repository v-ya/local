#ifndef _iphyee_glsl_function_rasterize_
#define _iphyee_glsl_function_rasterize_

#include "refer.rasterize.glsl"
#include "function.tri3.box.glsl"
#include "function.tri3.inside.glsl"

void iphyee_function_rasterize_set_tri3(const in iphyee_array_tri3_render tri3_array, const uint tri3_index, const in vec3 tri[3], const in iphyee_refer_model model, const in uint fusion_index)
{
	tri3_array.tri3[tri3_index].model = model;
	iphyee_function_tri3_inside(tri3_array.tri3[tri3_index].inside, tri);
	tri3_array.tri3[tri3_index].depth.depth_0 = tri[0].z;
	tri3_array.tri3[tri3_index].depth.depth_1 = tri[1].z;
	tri3_array.tri3[tri3_index].depth.depth_2 = tri[2].z;
	tri3_array.tri3[tri3_index].fusion_index = fusion_index;
}

void iphyee_function_rasterize_set_block(const in iphyee_array_rasterize_block block_array, const in iphyee_rasterize rasterize, const in iphyee_tri3_box box, const uint tri3_index)
{
	uint ha, hb, va, vb, h, v;
	ha = box.xmin / rasterize.block_width;
	hb = box.xmax / rasterize.block_width;
	va = box.ymin / rasterize.block_height;
	vb = box.ymax / rasterize.block_height;
	for (v = va; v <= vb; v += 1)
	{
		for (h = ha; h <= hb; h += 1)
		{
			uint block_index, link_index;
			block_index = v * rasterize.block_h_count + h;
			link_index = atomicAdd(block_array.block[block_index].index_cur_count, 1);
			if (link_index < block_array.block[block_index].index_max_count)
				block_array.block[block_index].tri3_index_array.index[link_index] = tri3_index;
		}
	}
}

void iphyee_function_rasterize_push_normal(const in iphyee_refer_rasterize rasterize, const in vec3 tri[3], const in iphyee_refer_model model, const in uint fusion_index)
{
	iphyee_tri3_box box;
	if (iphyee_function_tri3_box(box, tri, rasterize.rasterize.image))
	{
		uint tri3_index;
		tri3_index = atomicAdd(rasterize.rasterize.tri3_normal.tri3_cur_count, 1);
		if (tri3_index < rasterize.rasterize.tri3_normal.tri3_max_count)
		{
			iphyee_function_rasterize_set_tri3(rasterize.rasterize.tri3_normal.tri3_array, tri3_index, tri, model, fusion_index);
			iphyee_function_rasterize_set_block(rasterize.rasterize.block_normal, rasterize.rasterize, box, tri3_index);
		}
	}
}

void iphyee_function_rasterize_push_alpha(const in iphyee_refer_rasterize rasterize, const in vec3 tri[3], const in iphyee_refer_model model, const in uint fusion_index)
{
	iphyee_tri3_box box;
	if (iphyee_function_tri3_box(box, tri, rasterize.rasterize.image))
	{
		uint tri3_index;
		tri3_index = atomicAdd(rasterize.rasterize.tri3_alpha.tri3_cur_count, 1);
		if (tri3_index < rasterize.rasterize.tri3_alpha.tri3_max_count)
		{
			iphyee_function_rasterize_set_tri3(rasterize.rasterize.tri3_alpha.tri3_array, tri3_index, tri, model, fusion_index);
			iphyee_function_rasterize_set_block(rasterize.rasterize.block_alpha, rasterize.rasterize, box, tri3_index);
		}
	}
}

#endif
