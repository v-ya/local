#include "xwindow.private.h"

xwindow_s* xwindow_enable_shm(xwindow_s *restrict r, uintptr_t shm_size)
{
	xcb_generic_error_t *restrict error;
	xwindow_shm_s *restrict shm;
	xcb_shm_seg_t shmseg;
	shm = NULL;
	if (r->shm)
		goto label_fail;
	if (!(shm = xwindow_shm_alloc(shm_size)))
		goto label_fail;
	shmseg = xcb_generate_id(r->connection);
	if (!(error = xcb_request_check(r->connection, xcb_shm_attach_checked(
			r->connection, shmseg, shm->shmid, 1))))
	{
		r->shm = shm;
		r->shmseg = shmseg;
		return r;
	}
	free(error);
	label_fail:
	if (shm) refer_free(shm);
	return NULL;
}

xwindow_s* xwindow_disable_shm(xwindow_s *restrict r)
{
	if (r->shm)
	{
		if (r->shmseg)
		{
			xcb_shm_detach(r->connection, r->shmseg);
			r->shmseg = 0;
		}
		refer_free(r->shm);
		r->shm = NULL;
		return r;
	}
	return NULL;
}
