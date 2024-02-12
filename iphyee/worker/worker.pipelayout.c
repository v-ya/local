#include "worker.inner.h"

static void iphyee_worker_pipelayout_free_func(iphyee_worker_pipelayout_s *restrict r)
{
	if (r->pipelayout) vkDestroyPipelineLayout(r->device, r->pipelayout, NULL);
	if (r->shader) refer_free(r->shader);
}

iphyee_worker_pipelayout_s* iphyee_worker_pipelayout_alloc(const iphyee_worker_shader_s *restrict shader)
{
	iphyee_worker_pipelayout_s *restrict r;
	VkPipelineLayoutCreateInfo info;
	VkPushConstantRange range;
	info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	info.setLayoutCount = 1;
	info.pSetLayouts = &shader->setlayout->setlayout;
	info.pushConstantRangeCount = 0;
	info.pPushConstantRanges = NULL;
	if (shader->push_constants_length)
	{
		info.pushConstantRangeCount = 1;
		info.pPushConstantRanges = &range;
		range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		range.offset = 0;
		range.size = shader->push_constants_length;
	}
	if ((r = (iphyee_worker_pipelayout_s *) refer_alloz(sizeof(iphyee_worker_pipelayout_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_pipelayout_free_func);
		r->shader = (const iphyee_worker_shader_s *) refer_save(shader);
		r->device = shader->device;
		if (!vkCreatePipelineLayout(r->device, &info, NULL, &r->pipelayout))
			return r;
		refer_free(r);
	}
	return NULL;
}
