#ifndef _iphyee_glsl_function_rasterize_
#define _iphyee_glsl_function_rasterize_

#include "refer.rasterize.glsl"
#include "function.tri3.box.glsl"
#include "function.tri3.inside.glsl"

void iphyee_function_rasterize_push(const in iphyee_refer_rasterize rasterize, const in iphyee_rasterize_block block, const in vec3 tri[3], const in iphyee_refer_model model, const in uint fusion_index)
{
	iphyee_tri3_box box;
	if (iphyee_function_tri3_box(box, tri, rasterize.rasterize.image))
	{
		uint tri3_index;
		tri3_index = atomicAdd(rasterize.rasterize.tri3_cur_count, 1);
		if (tri3_index < rasterize.rasterize.tri3_max_count)
		{
			uint v, h;
			// set tri3
			rasterize.rasterize.tri3.tri3[tri3_index].model = model;
			iphyee_function_tri3_inside(rasterize.rasterize.tri3.tri3[tri3_index].inside, tri);
			rasterize.rasterize.tri3.tri3[tri3_index].depth.depth_0 = tri[0].z;
			rasterize.rasterize.tri3.tri3[tri3_index].depth.depth_1 = tri[1].z;
			rasterize.rasterize.tri3.tri3[tri3_index].depth.depth_2 = tri[2].z;
			rasterize.rasterize.tri3.tri3[tri3_index].fusion_index = fusion_index;
			// set list
			box.xmin /= rasterize.rasterize.block_width;
			box.xmax /= rasterize.rasterize.block_width;
			box.ymin /= rasterize.rasterize.block_height;
			box.ymax /= rasterize.rasterize.block_height;
			for (v = box.ymin; v <= box.ymax; v += 1)
			{
				for (h = box.xmin; h <= box.xmax; h += 1)
				{
					uint block_index, list_index;
					list_index = atomicAdd(rasterize.rasterize.list_cur_count, 1);
					if (list_index < rasterize.rasterize.list_max_count)
					{
						block_index = v * rasterize.rasterize.block_h_count + h;
						rasterize.rasterize.list.list[list_index].next = atomicExchange(
							block.list_next[block_index], list_index);
						rasterize.rasterize.list.list[list_index].index = tri3_index;
					}
				}
			}
		}
	}
}

void iphyee_function_rasterize_push_normal(const in iphyee_refer_rasterize rasterize, const in vec3 tri[3], const in iphyee_refer_model model, const in uint fusion_index)
{
	iphyee_function_rasterize_push(rasterize, rasterize.rasterize.block_normal, tri, model, fusion_index);
}

void iphyee_function_rasterize_push_alpha(const in iphyee_refer_rasterize rasterize, const in vec3 tri[3], const in iphyee_refer_model model, const in uint fusion_index)
{
	iphyee_function_rasterize_push(rasterize, rasterize.rasterize.block_alpha, tri, model, fusion_index);
}

#endif
