#include "iusb.inner.h"
#include <linux/usb/ch9.h>
#include <memory.h>

static void iusb_inner_urb_free_func(iusb_urb_s *restrict r)
{
	if (r->dev)
	{
		if (r->urb)
			iusb_inner_dev_discard_urb(r->dev, r->urb);
		refer_free(r->dev);
	}
	exbuffer_uini(&r->data);
}

iusb_urb_s* iusb_urb_alloc(iusb_dev_s *restrict dev, uintptr_t urb_size)
{
	iusb_urb_s *restrict r;
	if ((r = (iusb_urb_s *) refer_alloz(sizeof(iusb_urb_s))))
	{
		refer_set_free(r, (refer_free_f) iusb_inner_urb_free_func);
		r->dev = (iusb_dev_s *) refer_save(dev);
		if (exbuffer_init(&r->data, urb_size))
		{
			r->data.used = urb_size;
			r->urb_header.urb.buffer = r->data.data;
			r->urb_header.urb.usercontext = r;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

iusb_urb_s* iusb_urb_need_wait(iusb_urb_s *restrict urb)
{
	return (urb->urb)?urb:NULL;
}

void iusb_inner_urb_reap(iusb_urb_s *restrict urb)
{
	urb->urb = NULL;
}

iusb_urb_s* iusb_urb_set_param(iusb_urb_s *restrict urb, iusb_endpoint_xfer_t xfer, uint32_t endpoint)
{
	if (!urb->urb)
	{
		struct usbdevfs_urb *restrict p;
		p = &urb->urb_header.urb;
		switch (xfer)
		{
			case iusb_endpoint_xfer_control:
				p->type = USBDEVFS_URB_TYPE_CONTROL;
				break;
			case iusb_endpoint_xfer_isoc:
				p->type = USBDEVFS_URB_TYPE_ISO;
				break;
			case iusb_endpoint_xfer_bulk:
				p->type = USBDEVFS_URB_TYPE_BULK;
				break;
			case iusb_endpoint_xfer_interrupt:
				p->type = USBDEVFS_URB_TYPE_INTERRUPT;
				break;
			default:
				goto label_fail;
		}
		p->endpoint = (uint8_t) endpoint;
		return urb;
	}
	label_fail:
	return NULL;
}

iusb_urb_s* iusb_urb_fill_data_control(iusb_urb_s *restrict urb, uint32_t request_type, uint32_t request, uint32_t value, uint32_t index, const void *data, uintptr_t size)
{
	struct usb_ctrlrequest *restrict rq;
	if (!urb->urb && size + sizeof(*rq) <= urb->data.used)
	{
		rq = (struct usb_ctrlrequest *) urb->data.data;
		rq->bRequestType = iusb_type_to_u8(request_type);
		rq->bRequest = iusb_type_to_u8(request);
		rq->wValue = iusb_type_to_le16(value);
		rq->wIndex = iusb_type_to_le16(index);
		rq->wLength = iusb_type_to_le16(size);
		urb->urb_header.urb.buffer_length = (int) (sizeof(struct usb_ctrlrequest) + size);
		if (data) memcpy(rq + 1, data, size);
		return urb;
	}
	return NULL;
}

const void* iusb_urb_get_data_control(iusb_urb_s *restrict urb, uintptr_t *restrict rsize)
{
	struct usbdevfs_urb *restrict p;
	if (!urb->urb && !(p = &urb->urb_header.urb)->status)
	{
		if (p->actual_length >= 0 && (int) (p->actual_length + sizeof(struct usb_ctrlrequest)) <= p->buffer_length)
		{
			if (rsize) *rsize = (uintptr_t) p->actual_length;
			return (uint8_t *) p->buffer + sizeof(struct usb_ctrlrequest);
		}
	}
	if (rsize) *rsize = 0;
	return NULL;
}

iusb_urb_s* iusb_urb_fill_data_bulk(iusb_urb_s *restrict urb, uint32_t stream_id, const void *data, uintptr_t size)
{
	if (!urb->urb && size <= urb->data.used)
	{
		urb->urb_header.urb.stream_id = stream_id;
		urb->urb_header.urb.buffer_length = (int) size;
		if (data) memcpy(urb->data.data, data, size);
		return urb;
	}
	return NULL;
}

const void* iusb_urb_get_data_bulk(iusb_urb_s *restrict urb, uintptr_t *restrict rsize)
{
	struct usbdevfs_urb *restrict p;
	if (!urb->urb && !(p = &urb->urb_header.urb)->status)
	{
		if (p->actual_length >= 0 && (int) p->actual_length <= p->buffer_length)
		{
			if (rsize) *rsize = (uintptr_t) p->actual_length;
			return (uint8_t *) p->buffer;
		}
	}
	if (rsize) *rsize = 0;
	return NULL;
}

iusb_urb_s* iusb_urb_submit(iusb_urb_s *restrict urb)
{
	if (!urb->urb)
	{
		struct usbdevfs_urb *restrict p;
		p = &urb->urb_header.urb;
		p->status = 0;
		p->actual_length = 0;
		p->error_count = 0;
		urb->urb = p;
		urb->last_submit_timestamp = yaw_timestamp_msec();
		if (iusb_inner_dev_submit_urb(urb->dev, p))
			return urb;
		urb->urb = NULL;
	}
	return NULL;
}

iusb_urb_s* iusb_urb_discard(iusb_urb_s *restrict urb)
{
	struct usbdevfs_urb *restrict u;
	if ((u = urb->urb))
	{
		urb->urb = NULL;
		if (iusb_inner_dev_discard_urb(urb->dev, u))
			return urb;
	}
	return NULL;
}
