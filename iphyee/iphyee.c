#include "iphyee.pri.h"
#include "worker/worker.h"
#include "bonex/bonex.h"

static void iphyee_free_func(iphyee_s *restrict r)
{
	if (r->bonex_preset) refer_free(r->bonex_preset);
	if (r->worker_instance) refer_free(r->worker_instance);
}

iphyee_s* iphyee_alloc(void)
{
	iphyee_s *restrict r;
	if ((r = (iphyee_s *) refer_alloz(sizeof(iphyee_s))))
	{
		refer_set_free(r, (refer_free_f) iphyee_free_func);
		if ((r->bonex_preset = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

// self keep

iphyee_s* iphyee_initial_worker_instance(iphyee_s *restrict r, const char *restrict application_name, uint32_t application_version, struct mlog_s *restrict mlog, iphyee_worker_debug_t level)
{
	if (!r->worker_instance && (r->worker_instance = iphyee_worker_instance_alloc(
		application_name, application_version, "iphyee", 0, mlog, level)))
		return r;
	return NULL;
}

iphyee_s* iphyee_enumerate_worker_device(iphyee_s *restrict r)
{
	if (r->worker_instance && iphyee_worker_instance_enumerate_device(r->worker_instance))
		return r;
	return NULL;
}

iphyee_s* iphyee_insert_bonex_preset(iphyee_s *restrict r, const iphyee_param_bonex_t *restrict param)
{
	if (iphyee_bonex_create(r->bonex_preset, param))
		return r;
	return NULL;
}

// need caller refer free

iphyee_worker_s* iphyee_create_worker(iphyee_s *restrict r, uintptr_t physical_device_index, uint32_t compute_submit_count, uint32_t transfer_submit_count)
{
	if (r->worker_instance)
		return iphyee_worker_alloc(r->worker_instance, physical_device_index, compute_submit_count, transfer_submit_count);
	return NULL;
}
