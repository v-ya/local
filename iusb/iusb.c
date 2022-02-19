#include "iusb.inner.h"

// inst

iusb_inst_s* iusb_inst_alloc(void)
{
	return iusb_inner_inst_alloc(16384);
}

iusb_pool_t iusb_inst_pool(iusb_inst_s *restrict inst)
{
	return inst->device;
}

uintptr_t iusb_inst_update(iusb_inst_s *restrict inst)
{
	return iusb_inner_bus_scan(inst->device, &inst->usb_desc_cache);
}

iusb_device_s* iusb_inst_alloc_device(iusb_inst_s *restrict inst, uint32_t bus_id, uint32_t device_id)
{
	return iusb_inner_bus_alloc_device(&inst->usb_desc_cache, bus_id, device_id);
}

// device

refer_string_t iusb_device_path(const iusb_device_s *restrict device)
{
	return device->path;
}

refer_string_t iusb_device_id(const iusb_device_s *restrict device)
{
	return device->id;
}

iusb_attr_t iusb_device_attr(const iusb_device_s *restrict device)
{
	return device->attr;
}

// attr device

const vattr_vslot_t* iusb_attr_find_device(iusb_attr_t attr)
{
	return vattr_get_vslot(attr, iusb_key_device);
}

const iusb_attr_device_t* iusb_attr_device_data(const iusb_device_attr_device_s *restrict device)
{
	return &device->device;
}

iusb_attr_t iusb_attr_device_attr(const iusb_device_attr_device_s *restrict device)
{
	return device->container.attr;
}

// attr config

const vattr_vslot_t* iusb_attr_find_config(iusb_attr_t attr)
{
	return vattr_get_vslot(attr, iusb_key_config);
}

const iusb_attr_config_t* iusb_attr_config_data(const iusb_device_attr_config_s *restrict config)
{
	return &config->config;
}

iusb_attr_t iusb_attr_config_attr(const iusb_device_attr_config_s *restrict config)
{
	return config->container.attr;
}

// attr interface

const vattr_vslot_t* iusb_attr_find_interface(iusb_attr_t attr)
{
	return vattr_get_vslot(attr, iusb_key_interface);
}

const iusb_attr_interface_t* iusb_attr_interface_data(const iusb_device_attr_interface_s *restrict interface)
{
	return &interface->interface;
}

iusb_attr_t iusb_attr_interface_attr(const iusb_device_attr_interface_s *restrict interface)
{
	return interface->container.attr;
}

// attr endpoint

const vattr_vslot_t* iusb_attr_find_endpoint(iusb_attr_t attr)
{
	return vattr_get_vslot(attr, iusb_key_endpoint);
}

const iusb_attr_endpoint_t* iusb_attr_endpoint_data(const iusb_device_attr_endpoint_s *restrict endpoint)
{
	return &endpoint->endpoint;
}
