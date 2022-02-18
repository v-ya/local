#ifndef _iusb_inner_h_
#define _iusb_inner_h_

#include "iusb.h"
#include <exbuffer.h>
#include <fsys.h>
#include <sys/ioctl.h>
#include <linux/usb/ch9.h>
#include <linux/usbdevice_fs.h>

struct iusb_pool_s {
	vattr_s *device;
	exbuffer_t usb_desc_cache;
};

struct iusb_device_s {
	refer_string_t path;
	refer_string_t id;
	const uint8_t *usb_descriptor_data;
	uintptr_t usb_descriptor_size;
};

// bus

iusb_pool_s* iusb_inner_pool_alloc(uintptr_t usb_desc_cache_size);
uintptr_t iusb_inner_bus_scan(vattr_s *restrict pool, exbuffer_t *restrict usb_descriptor);

// device

iusb_device_s* iusb_inner_device_alloc(const uint8_t *restrict desc, uintptr_t size, const char *restrict path, const char *restrict id);

#endif
