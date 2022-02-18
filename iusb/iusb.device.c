#include "iusb.inner.h"
#include <memory.h>

static iusb_device_s* iusb_inner_device_alloc_parse(iusb_device_s *restrict r, const uint8_t *restrict desc, uintptr_t size)
{
	const struct usb_descriptor_header *restrict h;
	uintptr_t n;
	while (size)
	{
		if (size < sizeof(*h))
			goto label_fail;
		h = (const struct usb_descriptor_header *) desc;
		if ((n = h->bLength) > size)
			goto label_fail;
		desc += n;
		size -= n;
	}
	return r;
	label_fail:
	return NULL;
}

static void iusb_inner_device_free_func(iusb_device_s *restrict r)
{
	if (r->path) refer_free(r->path);
	if (r->id) refer_free(r->id);
}

iusb_device_s* iusb_inner_device_alloc(const uint8_t *restrict desc, uintptr_t size, const char *restrict path, const char *restrict id)
{
	iusb_device_s *restrict r;
	if ((r = (iusb_device_s *) refer_alloc(sizeof(iusb_device_s) + size)))
	{
		memset(r, 0, sizeof(iusb_device_s));
		refer_set_free(r, (refer_free_f) iusb_inner_device_free_func);
		r->path = refer_dump_string(path);
		r->id = refer_dump_string(id);
		r->usb_descriptor_data = (const uint8_t *) memcpy(r + 1, desc, size);
		r->usb_descriptor_size = size;
		if (r->path && r->id && iusb_inner_device_alloc_parse(r, desc, size))
			return r;
		refer_free(r);
	}
	return NULL;
}
