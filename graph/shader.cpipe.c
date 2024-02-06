#include "shader_pri.h"
#include "layout_pri.h"
#include "type_pri.h"
#include "system.h"

graph_pipe_s* graph_pipe_alloc_compute(const graph_cpipe_param_s *restrict param, const graph_pipe_cache_s *restrict cache)
{
	graph_pipe_s *restrict r;
	VkResult ret;
	if (param->pi)
	{
		if ((r = (graph_pipe_s *) refer_alloz(sizeof(graph_pipe_s))))
		{
			refer_set_free(r, (refer_free_f) graph_pipe_free_func);
			r->ml = (mlog_s *) refer_save(param->ml);
			r->dev = (graph_dev_s *) refer_save(param->dev);
			r->cache = (graph_pipe_cache_s *) refer_save(cache);
			r->ga = (graph_allocator_s *) refer_save(param->dev->ga);
			ret = vkCreateComputePipelines(r->dev->dev, cache?cache->pipe_cache:NULL, 1, param->pi, &r->ga->alloc, &r->pipe);
			if (!ret) return r;
			mlog_printf(r->ml, "[graph_pipe_alloc_compute] vkCreateComputePipelines = %d\n", ret);
			refer_free(r);
		}
	}
	else mlog_printf(param->ml, "[graph_pipe_alloc_compute] make sure call 'graph_cpipe_param_ok' ok\n");
	return NULL;
}

static void graph_cpipe_param_free_func(graph_cpipe_param_s *restrict r)
{
	void *v;
	if ((v = r->pipe_layout)) refer_free(v);
	graph_pipe_shader_stage_finally(&r->shader_stage);
	if ((v = r->ml)) refer_free(v);
	if ((v = r->dev)) refer_free(v);
}

graph_cpipe_param_s* graph_cpipe_param_alloc(struct graph_dev_s *restrict dev)
{
	graph_cpipe_param_s *restrict r;
	if ((r = (graph_cpipe_param_s *) refer_alloz(sizeof(graph_cpipe_param_s))))
	{
		refer_set_free(r, (refer_free_f) graph_cpipe_param_free_func);
		r->ml = (mlog_s *) refer_save(dev->ml);
		r->dev = (graph_dev_s *) refer_save(dev);
		r->info.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
		r->info.stage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		return r;
	}
	return NULL;
}

graph_cpipe_param_s* graph_cpipe_param_set_shader(graph_cpipe_param_s *restrict r, const graph_shader_s *restrict shader, graph_shader_type_t type, const char *restrict entry, const graph_shader_spec_s *restrict spec)
{
	r->pi = NULL;
	if (!shader || !entry) goto label_arg;
	graph_pipe_shader_stage_finally(&r->shader_stage);
	if (graph_pipe_shader_stage_initial(&r->shader_stage, shader, spec, entry))
	{
		r->info.stage.flags = 0;
		r->info.stage.stage = (VkShaderStageFlagBits) type;
		r->info.stage.module = shader->shader;
		r->info.stage.pName = r->shader_stage.name;
		r->info.stage.pSpecializationInfo = spec?&spec->spec:NULL;
		return r;
	}
	else
	{
		r->info.stage.flags = 0;
		r->info.stage.stage = 0;
		r->info.stage.module = NULL;
		r->info.stage.pName = NULL;
		r->info.stage.pSpecializationInfo = NULL;
	}
	mlog_printf(r->ml, "[graph_cpipe_param_set_shader] malloc fail\n");
	label_return_null:
	return NULL;
	label_arg:
	mlog_printf(r->ml, "[graph_cpipe_param_set_shader] !shader(%p) || !entry(%s)\n", shader, entry);
	goto label_return_null;
}

void graph_cpipe_param_set_layout(graph_cpipe_param_s *restrict r, const struct graph_pipe_layout_s *restrict layout)
{
	r->pi = NULL;
	r->info.layout = NULL;
	if (r->pipe_layout)
	{
		refer_free(r->pipe_layout);
		r->pipe_layout = NULL;
	}
	if (layout)
	{
		r->pipe_layout = (graph_pipe_layout_s *) refer_save(layout);
		r->info.layout = layout->layout;
	}
}

graph_cpipe_param_s* graph_cpipe_param_ok(graph_cpipe_param_s *restrict r)
{
	static char empty[] = {0};
	r->pi = NULL;
	if (r->shader_stage.name && r->shader_stage.gs && r->info.layout)
	{
		r->pi = &r->info;
		return r;
	}
	mlog_printf(r->ml, "[graph_cpipe_param_ok] (%s%s%s%s%s%s%s%s ) not set\n",
		(r->shader_stage.name && r->shader_stage.gs)?empty:" shader",
		r->info.layout?empty:" layout"
	);
	return NULL;
}
