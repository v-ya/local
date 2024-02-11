#include "worker.inner.h"

static void iphyee_worker_setlayout_free_func(iphyee_worker_setlayout_s *restrict r)
{
	if (r->setlayout) vkDestroyDescriptorSetLayout(r->device, r->setlayout, NULL);
	if (r->depend) refer_free(r->depend);
}

iphyee_worker_setlayout_s* iphyee_worker_setlayout_alloc(iphyee_worker_device_s *restrict device, uintptr_t binding_count, const iphyee_worker_setlayout_binding_t *restrict binding_array)
{
	iphyee_worker_setlayout_s *restrict r;
	VkDescriptorSetLayoutCreateInfo info;
	VkDescriptorSetLayoutBinding binding[binding_count];
	uintptr_t i;
	info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	info.pNext = NULL;
	info.flags = 0;
	info.bindingCount = binding_count;
	info.pBindings = binding;
	for (i = 0; i < binding_count; ++i)
	{
		binding[i].binding = binding_array[i].binding;
		binding[i].descriptorType = binding_array[i].desc_type;
		binding[i].descriptorCount = 1;
		binding[i].stageFlags = VK_SHADER_STAGE_COMPUTE_BIT;
		binding[i].pImmutableSamplers = NULL;
	}
	if ((r = (iphyee_worker_setlayout_s *) refer_alloz(sizeof(iphyee_worker_setlayout_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_worker_setlayout_free_func);
		r->depend = (iphyee_worker_device_s *) refer_save(device);
		r->device = device->device;
		if (!vkCreateDescriptorSetLayout(r->device, &info, NULL, &r->setlayout))
			return r;
		refer_free(r);
	}
	return NULL;
}
