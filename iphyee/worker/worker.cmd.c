#include "worker.inner.h"

void iphyee_worker_cmd_copy_buffer(iphyee_worker_command_buffer_s *restrict command_buffer, const iphyee_worker_buffer_s *restrict src, iphyee_worker_buffer_s *restrict dst, uint64_t src_offset, uint64_t dst_offset, uint64_t copy_size)
{
	VkBufferCopy copy;
	copy.srcOffset = src_offset;
	copy.dstOffset = dst_offset;
	copy.size = copy_size;
	vkCmdCopyBuffer(command_buffer->command_buffer, src->buffer, dst->buffer, 1, &copy);
}

void iphyee_worker_cmd_fill_buffer(iphyee_worker_command_buffer_s *restrict command_buffer, iphyee_worker_buffer_s *restrict dst, uint64_t dst_offset, uint64_t fill_size, uint32_t fill_value)
{
	vkCmdFillBuffer(command_buffer->command_buffer, dst->buffer, dst_offset, fill_size, fill_value);
}

void iphyee_worker_cmd_bind_shader(iphyee_worker_command_buffer_s *restrict command_buffer, const iphyee_worker_shader_s *restrict shader)
{
	VkShaderStageFlagBits stage;
	stage = VK_SHADER_STAGE_COMPUTE_BIT;
	if (shader->depend->vkCmdBindShadersEXT)
		shader->depend->vkCmdBindShadersEXT(command_buffer->command_buffer, 1, &stage, &shader->shader);
}

void iphyee_worker_cmd_push_constants(iphyee_worker_command_buffer_s *restrict command_buffer, const iphyee_worker_shader_s *restrict shader, uint32_t offset, uint32_t size, const void *restrict value)
{
	vkCmdPushConstants(command_buffer->command_buffer, shader->layout,
		VK_SHADER_STAGE_COMPUTE_BIT, offset, size, value);
}

void iphyee_worker_cmd_dispatch_base(iphyee_worker_command_buffer_s *restrict command_buffer, uint32_t group_base_x, uint32_t group_base_y, uint32_t group_base_z, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
{
	vkCmdDispatchBase(command_buffer->command_buffer, group_base_x, group_base_y, group_base_z, group_count_x, group_count_y, group_count_z);
}

void iphyee_worker_cmd_dispatch(iphyee_worker_command_buffer_s *restrict command_buffer, uint32_t group_count_x, uint32_t group_count_y, uint32_t group_count_z)
{
	vkCmdDispatch(command_buffer->command_buffer, group_count_x, group_count_y, group_count_z);
}

void iphyee_worker_cmd_bind_barrier_transfer2shader(iphyee_worker_command_buffer_s *restrict command_buffer)
{
	static const VkMemoryBarrier memory_barrier = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
	};
	vkCmdPipelineBarrier(
		command_buffer->command_buffer,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0, 1, &memory_barrier, 0, NULL, 0, NULL);
}

void iphyee_worker_cmd_bind_barrier_shader2transfer(iphyee_worker_command_buffer_s *restrict command_buffer)
{
	static const VkMemoryBarrier memory_barrier = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT,
	};
	vkCmdPipelineBarrier(
		command_buffer->command_buffer,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		0, 1, &memory_barrier, 0, NULL, 0, NULL);
}

void iphyee_worker_cmd_bind_barrier_shader2shader(iphyee_worker_command_buffer_s *restrict command_buffer)
{
	static const VkMemoryBarrier memory_barrier = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
		.pNext = NULL,
		.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT,
		.dstAccessMask = VK_ACCESS_SHADER_READ_BIT,
	};
	vkCmdPipelineBarrier(
		command_buffer->command_buffer,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT,
		0, 1, &memory_barrier, 0, NULL, 0, NULL);
}
