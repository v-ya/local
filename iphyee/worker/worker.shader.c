#include "worker.inner.h"

static void iphyee_worker_shader_free_func(iphyee_worker_shader_s *restrict r)
{
	if (r->layout) vkDestroyPipelineLayout(r->device, r->layout, NULL);
	if (r->shader) r->depend->vkDestroyShaderEXT(r->device, r->shader, NULL);
	if (r->depend) refer_free(r->depend);
}

iphyee_worker_shader_s* iphyee_worker_shader_alloc(iphyee_worker_device_s *restrict device, const void *restrict code, uintptr_t size, const char *restrict entry_name, uint32_t push_constants_size)
{
	iphyee_worker_shader_s *restrict r;
	VkShaderCreateInfoEXT info_shader;
	VkPipelineLayoutCreateInfo info_layout;
	VkPushConstantRange push_constant_range;
	if (device->vkCreateShadersEXT && device->vkDestroyShaderEXT && entry_name)
	{
		info_shader.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
		info_shader.pNext = NULL;
		info_shader.flags = 0;
		info_shader.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		info_shader.nextStage = 0;
		info_shader.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
		info_shader.codeSize = size;
		info_shader.pCode = code;
		info_shader.pName = entry_name;
		info_shader.setLayoutCount = 0;
		info_shader.pSetLayouts = NULL;
		info_shader.pushConstantRangeCount = 0;
		info_shader.pPushConstantRanges = NULL;
		info_shader.pSpecializationInfo = NULL;
		info_shader.setLayoutCount = 0;
		info_shader.pSetLayouts = NULL;
		info_layout.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		info_layout.pNext = NULL;
		info_layout.flags = 0;
		info_layout.setLayoutCount = 0;
		info_layout.pSetLayouts = NULL;
		info_layout.pushConstantRangeCount = 0;
		info_layout.pPushConstantRanges = NULL;
		if (push_constants_size)
		{
			push_constant_range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
			push_constant_range.offset = 0;
			push_constant_range.size = push_constants_size;
			info_shader.pushConstantRangeCount = 1;
			info_shader.pPushConstantRanges = &push_constant_range;
			info_layout.pushConstantRangeCount = 1;
			info_layout.pPushConstantRanges = &push_constant_range;
		}
		if ((r = (iphyee_worker_shader_s *) refer_alloz(sizeof(iphyee_worker_shader_s))))
		{
			refer_set_free(r, (refer_free_f) iphyee_worker_shader_free_func);
			r->depend = (iphyee_worker_device_s *) refer_save(device);
			r->device = device->device;
			r->push_constants_offset = 0;
			r->push_constants_length = push_constants_size;
			if (!device->vkCreateShadersEXT(r->device, 1, &info_shader, NULL, &r->shader) &&
				!vkCreatePipelineLayout(r->device, &info_layout, NULL, &r->layout))
				return r;
			refer_free(r);
		}
	}
	return NULL;
}

const iphyee_worker_shader_s* iphyee_worker_shader_binary(const iphyee_worker_shader_s *restrict r, uintptr_t *restrict binary_size, void *restrict binary_data)
{
	if (r->depend->vkGetShaderBinaryDataEXT && !r->depend->vkGetShaderBinaryDataEXT(
		r->device, r->shader, binary_size, binary_data))
		return r;
	return NULL;
}
