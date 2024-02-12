#include "worker.inner.h"

static void iphyee_worker_shader_free_func(iphyee_worker_shader_s *restrict r)
{
	if (r->shader) r->depend->vkDestroyShaderEXT(r->device, r->shader, NULL);
	if (r->setlayout) refer_free(r->setlayout);
	if (r->depend) refer_free(r->depend);
}

iphyee_worker_shader_s* iphyee_worker_shader_alloc(iphyee_worker_device_s *restrict device, const void *restrict code, uintptr_t size, const char *restrict entry_name, iphyee_worker_setlayout_s *restrict setlayout, uint32_t push_constants_size)
{
	iphyee_worker_shader_s *restrict r;
	VkShaderCreateInfoEXT info;
	VkPushConstantRange range;
	if (device->vkCreateShadersEXT && device->vkDestroyShaderEXT && setlayout)
	{
		info.sType = VK_STRUCTURE_TYPE_SHADER_CREATE_INFO_EXT;
		info.pNext = NULL;
		info.flags = 0;
		info.stage = VK_SHADER_STAGE_COMPUTE_BIT;
		info.nextStage = 0;
		info.codeType = VK_SHADER_CODE_TYPE_SPIRV_EXT;
		info.codeSize = size;
		info.pCode = code;
		info.pName = entry_name;
		info.setLayoutCount = 0;
		info.pSetLayouts = NULL;
		info.pushConstantRangeCount = 0;
		info.pPushConstantRanges = NULL;
		info.pSpecializationInfo = NULL;
		info.setLayoutCount = 1;
		info.pSetLayouts = &setlayout->setlayout;
		if (push_constants_size)
		{
			info.pushConstantRangeCount = 1;
			info.pPushConstantRanges = &range;
			range.stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
			range.offset = 0;
			range.size = push_constants_size;
		}
		if ((r = (iphyee_worker_shader_s *) refer_alloz(sizeof(iphyee_worker_shader_s))))
		{
			refer_set_free(r, (refer_free_f) iphyee_worker_shader_free_func);
			r->depend = (iphyee_worker_device_s *) refer_save(device);
			r->setlayout = (iphyee_worker_setlayout_s *) refer_save(setlayout);
			r->device = device->device;
			r->push_constants_offset = 0;
			r->push_constants_length = push_constants_size;
			if (!device->vkCreateShadersEXT(r->device, 1, &info, NULL, &r->shader))
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

void iphyee_worker_shader_bind(iphyee_worker_command_buffer_s *restrict command_buffer, const iphyee_worker_shader_s *restrict shader)
{
	VkShaderStageFlagBits stage;
	stage = VK_SHADER_STAGE_COMPUTE_BIT;
	if (shader->depend->vkCmdBindShadersEXT)
		shader->depend->vkCmdBindShadersEXT(command_buffer->command_buffer, 1, &stage, &shader->shader);
}
