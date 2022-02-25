#include "iusb.inner.h"
#include <memory.h>

#define iusb_pipe_package_size_min  8
#define iusb_pipe_package_size_max  (16 << 10)

static void iusb_inner_pipe_free_func(iusb_pipe_s *restrict r)
{
	uintptr_t i, n;
	n = r->urb_number;
	for (i = 0; i < n; ++i)
	{
		if (r->urb_array[i])
			refer_free(r->urb_array[i]);
	}
	if (r->dev) refer_free(r->dev);
}

static inline void iusb_inner_pipe_clear(iusb_pipe_s *restrict r)
{
	uintptr_t i, n;
	n = r->urb_number;
	for (i = 0; i < n; ++i)
		iusb_urb_discard(r->urb_array[i]);
}

iusb_pipe_s* iusb_pipe_alloc(iusb_dev_s *restrict dev, uint32_t endpoint, uintptr_t package_size, uintptr_t want_buffer_size)
{
	iusb_pipe_s *restrict r;
	uintptr_t urb_half_number, urb_number;
	if (package_size < iusb_pipe_package_size_min) package_size = iusb_pipe_package_size_min;
	if (package_size > iusb_pipe_package_size_max) package_size = iusb_pipe_package_size_max;
	urb_half_number = ((want_buffer_size >> 1) + (package_size - 1)) / package_size;
	if (!urb_half_number) urb_half_number += 1;
	urb_number = urb_half_number << 1;
	if ((r = (iusb_pipe_s *) refer_alloz(sizeof(iusb_pipe_s) + sizeof(iusb_urb_s *) * urb_number)))
	{
		refer_set_free(r, (refer_free_f) iusb_inner_pipe_free_func);
		r->dev = (iusb_dev_s *) refer_save(dev);
		r->package_size = package_size;
		r->iusb_endpoint_dir = (uintptr_t) (endpoint & iusb_endpoint_address_mask_dir);
		r->urb_half_number = urb_half_number;
		r->urb_number = urb_number;
		for (urb_half_number = 0; urb_half_number < urb_number; ++urb_half_number)
		{
			if (!(r->urb_array[urb_half_number] = iusb_urb_alloc(dev, package_size)))
				goto label_fail;
			if (!iusb_urb_set_param(r->urb_array[urb_half_number], iusb_endpoint_xfer_bulk, endpoint))
				goto label_fail;
		}
		return r;
		label_fail:
		refer_free(r);
	}
	return NULL;
}

void iusb_pipe_set_calc_size(iusb_pipe_s *restrict pipe, iusb_pipe_calc_package_size_f calc_size)
{
	pipe->calc_size = calc_size;
}

typedef struct iusb_pipe_context_t {
	iusb_urb_s **urb_submit;
	iusb_urb_s **urb_wait;
	uintptr_t submit_size;
	uintptr_t wait_size;
	uintptr_t submit_number;
	uintptr_t wait_number;
	const uint8_t *send_data;
	uint8_t *recv_data;
	uintptr_t size;
	uintptr_t timeout_msec;
} iusb_pipe_context_t;

static inline void iusb_inner_pipe_context_initial(iusb_pipe_context_t *restrict c, const void *send_data, void *recv_data, uintptr_t size, uintptr_t timeout_msec)
{
	c->urb_submit = NULL;
	c->urb_wait = NULL;
	c->submit_size = 0;
	c->wait_size = 0;
	c->submit_number = 0;
	c->wait_number = 0;
	c->send_data = send_data;
	c->recv_data = recv_data;
	c->size = size;
	c->timeout_msec = timeout_msec;
}

static inline const iusb_pipe_context_t* iusb_inner_pipe_context_need_continue(const iusb_pipe_context_t *restrict c)
{
	return (c->size || c->urb_wait)?c:NULL;
}

static iusb_pipe_s* iusb_inner_pipe_submit_new_and_wait_last(iusb_pipe_s *restrict pipe, uint32_t stream_id, iusb_pipe_context_t *restrict c)
{
	iusb_urb_s **p;
	const void *d;
	uintptr_t i, n;
	if ((p = c->urb_submit) || (c->urb_submit = p = pipe->urb_array))
	{
		c->submit_size = 0;
		for (i = 0; c->size && i < pipe->urb_half_number; ++i)
		{
			n = pipe->package_size;
			if (n > c->size) n = c->size;
			if (!iusb_urb_fill_data_bulk(p[i], stream_id, c->send_data, n))
				goto label_fail;
			if (!iusb_urb_submit(p[i]))
				goto label_fail;
			if (c->send_data) c->send_data += n;
			c->submit_size += n;
			c->size -= n;
		}
		c->submit_number = i;
	}
	if ((p = c->urb_wait))
	{
		iusb_dev_wait_complete(pipe->dev, p, c->wait_number, c->timeout_msec);
		for (i = 0; c->wait_size && i < c->wait_number; ++i)
		{
			if (!(d = iusb_urb_get_data_bulk(p[i], &n)))
				goto label_fail;
			if (c->recv_data)
			{
				memcpy(c->recv_data, d, n);
				c->recv_data += n;
			}
			c->wait_size -= n;
		}
		if (c->wait_size) goto label_fail;
		c->urb_wait = NULL;
		c->wait_number = 0;
	}
	if (c->submit_size)
	{
		c->urb_wait = c->urb_submit;
		c->wait_size = c->submit_size;
		c->wait_number = c->submit_number;
		if (c->urb_submit == pipe->urb_array)
			c->urb_submit = pipe->urb_array + pipe->urb_half_number;
		else c->urb_submit = pipe->urb_array;
		c->submit_size = 0;
		c->submit_number = 0;
	}
	return pipe;
	label_fail:
	return NULL;
}

iusb_pipe_s* iusb_pipe_send_full(iusb_pipe_s *restrict pipe, uint32_t stream_id, const void *data, uintptr_t size, uintptr_t timeout_msec)
{
	iusb_pipe_s *r;
	r = NULL;
	if (pipe->iusb_endpoint_dir == iusb_endpoint_address_dir_out)
	{
		iusb_pipe_context_t c;
		iusb_inner_pipe_context_initial(&c, data, NULL, size, timeout_msec);
		while (iusb_inner_pipe_context_need_continue(&c))
		{
			if (!iusb_inner_pipe_submit_new_and_wait_last(pipe, stream_id, &c))
				goto label_fail;
		}
		r = pipe;
		label_fail:
		iusb_inner_pipe_clear(pipe);
	}
	return r;
}

iusb_pipe_s* iusb_pipe_recv_full(iusb_pipe_s *restrict pipe, uint32_t stream_id, void *data, uintptr_t size, uintptr_t timeout_msec)
{
	iusb_pipe_s *r;
	r = NULL;
	if (pipe->iusb_endpoint_dir == iusb_endpoint_address_dir_in)
	{
		iusb_pipe_context_t c;
		iusb_inner_pipe_context_initial(&c, NULL, data, size, timeout_msec);
		while (iusb_inner_pipe_context_need_continue(&c))
		{
			if (!iusb_inner_pipe_submit_new_and_wait_last(pipe, stream_id, &c))
				goto label_fail;
		}
		r = pipe;
		label_fail:
		iusb_inner_pipe_clear(pipe);
	}
	return r;
}

iusb_pipe_s* iusb_pipe_recv_package(iusb_pipe_s *restrict pipe, uint32_t stream_id, exbuffer_t *restrict package, uintptr_t timeout_msec)
{
	exbuffer_clear(package);
	if (pipe->calc_size && pipe->iusb_endpoint_dir == iusb_endpoint_address_dir_in)
	{
		iusb_urb_s *urb;
		const void *d;
		uintptr_t n, size;
		urb = *pipe->urb_array;
		if (!iusb_urb_fill_data_bulk(urb, stream_id, NULL, pipe->package_size))
			goto label_fail;
		if (!iusb_urb_submit(urb))
			goto label_fail;
		iusb_dev_wait_complete(pipe->dev, &urb, 1, timeout_msec);
		if (!(d = iusb_urb_get_data_bulk(urb, &n)))
			goto label_fail;
		if (!(size = pipe->calc_size(d, n)))
			goto label_fail;
		if (!exbuffer_need(package, size))
			goto label_fail;
		package->used = size;
		if (n > size) n = size;
		if (n) memcpy(package->data, d, n);
		size -= n;
		if (size) return iusb_pipe_recv_full(pipe, stream_id, package->data + n, size, timeout_msec);
		return pipe;
		label_fail:
		iusb_urb_discard(urb);
	}
	return NULL;
}
