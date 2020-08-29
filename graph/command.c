#include "command_pri.h"

static void graph_command_pool_free_func(register graph_command_pool_s *restrict r)
{
	register void *v;
	if ((v = r->cpool)) vkDestroyCommandPool(r->dev->dev, (VkCommandPool) v, &r->ga->alloc);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
	if ((v = r->ga)) refer_free(v);
}

graph_command_pool_s* graph_command_pool_alloc(register struct graph_dev_s *restrict dev, register const struct graph_device_queue_t *restrict queue, graph_command_pool_flags_t flags)
{
	register graph_command_pool_s *restrict r;
	VkCommandPoolCreateInfo info;
	VkResult ret;
	info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	info.pNext = NULL;
	info.flags = (VkCommandPoolCreateFlags) flags;
	info.queueFamilyIndex = (uint32_t) queue->index;
	r = (graph_command_pool_s *) refer_alloz(sizeof(graph_command_pool_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) graph_command_pool_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->ga = (graph_allocator_s *) refer_save(dev->ga);
		ret = vkCreateCommandPool(dev->dev, &info, &r->ga->alloc, &r->cpool);
		if (!ret) return r;
		mlog_printf(r->ml, "[graph_command_pool_alloc] vkCreateCommandPool = %d\n", ret);
		refer_free(r);
	}
	return NULL;
}
