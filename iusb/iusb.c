#include "iusb.inner.h"

iusb_pool_s* iusb_pool_alloc(void)
{
	return iusb_inner_pool_alloc(16384);
}

vattr_s* iusb_pool_vattr(iusb_pool_s *restrict pool)
{
	return pool->device;
}

uintptr_t iusb_pool_update(iusb_pool_s *restrict pool)
{
	return iusb_inner_bus_scan(pool->device, &pool->usb_desc_cache);
}

refer_string_t iusb_device_path(const iusb_device_s *restrict device)
{
	return device->path;
}

refer_string_t iusb_device_id(const iusb_device_s *restrict device)
{
	return device->id;
}
