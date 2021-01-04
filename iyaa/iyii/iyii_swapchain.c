#include "iyii_swapchain.h"
#include "iyii_render.h"
#include <stdlib.h>

static void iyii_swapchain_free_func(iyii_swapchain_s *restrict r)
{
	refer_t *restrict array;
	uint32_t i, n;
	if ((array = (refer_t *) r->semaphore))
	{
		for (i = 0, n = r->image_number * 2; i < n; ++i)
			if (array[i]) refer_free(array[i]);
		free((void *) array);
	}
	if ((array = (refer_t *) r->fence))
	{
		for (i = 0, n = r->image_number; i < n; ++i)
			if (array[i]) refer_free(array[i]);
		free((void *) array);
	}
	if ((array = (refer_t *) r->image_view))
	{
		for (i = 0, n = r->image_number; i < n; ++i)
			if (array[i]) refer_free(array[i]);
		free((void *) array);
	}
	if (r->image_view_param) refer_free(r->image_view_param);
	if (r->swapchain) refer_free(r->swapchain);
}

iyii_swapchain_s* iyii_swapchain_alloc(graph_dev_s *restrict dev, const graph_device_t *restrict device, graph_surface_s *restrict surface)
{
	iyii_swapchain_s *restrict r;
	r = (iyii_swapchain_s *) refer_alloz(sizeof(iyii_swapchain_s));
	if (r)
	{
		graph_image_view_s **iv;
		graph_fence_s **fc;
		graph_semaphore_s **sp;
		uint32_t i, n;
		refer_set_free(r, (refer_free_f) iyii_swapchain_free_func);
		r->swapchain = iyii_graph_create_swapchain(dev, device, surface);
		if (!r->swapchain) goto label_fail;
		graph_swapchain_info(r->swapchain, &r->image_number, &r->format, &r->width, &r->height);
		if (!(n = r->image_number) || !r->format || !r->width || !r->height) goto label_fail;
		r->image_view_param = graph_image_view_param_alloc(graph_image_view_type_2D);
		if (!r->image_view_param) goto label_fail;
		r->image_view = iv = (graph_image_view_s **) calloc(n, sizeof(graph_image_view_s *));
		if (!iv) goto label_fail;
		for (i = 0; i < n; ++i)
		{
			if (!(iv[i] = graph_image_view_alloc_by_swapchain(r->image_view_param, r->swapchain, i)))
				goto label_fail;
		}
		r->offset = n;
		r->fence = fc = (graph_fence_s **) calloc(n, sizeof(graph_fence_s *));
		if (!fc) goto label_fail;
		for (i = 0; i < n; ++i)
		{
			if (!(fc[i] = graph_fence_alloc(dev, graph_fence_flags_signaled)))
				goto label_fail;
		}
		r->semaphore = sp = (graph_semaphore_s **) calloc(n *= 2, sizeof(graph_semaphore_s *));
		if (!sp) goto label_fail;
		for (i = 0; i < n; ++i)
		{
			if (!(sp[i] = graph_semaphore_alloc(dev)))
				goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

iyii_swapchain_s* iyii_swapchain_acquire(iyii_swapchain_s *restrict swapchain, uint32_t *restrict index, uint32_t *restrict i)
{
	register uint32_t ii;
	graph_fence_wait(swapchain->fence[*i = ii = swapchain->index], ~0ul);
	if (~(*index = graph_swapchain_acquire(
		swapchain->swapchain,
		~0ul,
		swapchain->semaphore[ii],
		NULL)))
	{
		graph_fence_reset(swapchain->fence[ii]);
		swapchain->index = (ii + 1) % swapchain->image_number;
		return swapchain;
	}
	return NULL;
}

iyii_swapchain_s* iyii_swapchain_render(iyii_swapchain_s *restrict swapchain, graph_queue_t *restrict graphics, graph_command_pool_s *restrict cpool_draw, uint32_t index, uint32_t i)
{
	uint32_t j;
	j = i + swapchain->offset;
	if (graph_queue_submit(
		graphics,
		cpool_draw,
		index,
		swapchain->semaphore[i],
		swapchain->semaphore[j],
		swapchain->fence[i],
		graph_pipeline_stage_flags_top_of_pipe))
	{
		if (graph_queue_present(
			graphics,
			swapchain->swapchain,
			index,
			swapchain->semaphore[j]))
			return swapchain;
	}
	return NULL;
}
