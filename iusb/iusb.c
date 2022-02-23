#include "iusb.inner.h"
#include <yaw.h>
#include <alloca.h>

// inst

iusb_inst_s* iusb_inst_alloc(void)
{
	return iusb_inner_inst_alloc(64 << 10);
}

iusb_pool_t iusb_inst_pool(iusb_inst_s *restrict inst)
{
	return inst->device;
}

uintptr_t iusb_inst_update(iusb_inst_s *restrict inst)
{
	return iusb_inner_bus_scan(inst->device, inst->rpath, &inst->cache);
}

iusb_device_s* iusb_inst_alloc_device(iusb_inst_s *restrict inst, uint32_t bus_id, uint32_t device_id)
{
	return iusb_inner_bus_alloc_device(&inst->cache, bus_id, device_id);
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

// dev

iusb_dev_s* iusb_dev_alloc(const iusb_device_s *restrict device)
{
	if (device->path)
		return iusb_inner_dev_alloc(device->path);
	return NULL;
}

uintptr_t iusb_dev_do_events(iusb_dev_s *restrict dev)
{
	uintptr_t n;
	iusb_urb_s *restrict urb;
	n = 0;
	while ((urb = iusb_inner_dev_reap_urb(dev)))
	{
		iusb_inner_urb_reap(urb);
		n += 1;
	}
	return n;
}

static uintptr_t iusb_dev_wait_complete_check(iusb_urb_s *urb_array[], uintptr_t *restrict p, uintptr_t urb_number, uintptr_t *restrict pi)
{
	uintptr_t i, n;
	n = 0;
	while ((i = *pi) < urb_number)
	{
		if (iusb_inner_urb_need_wait(urb_array[i]))
		{
			pi = p + i;
			n += 1;
		}
		else *pi = p[i];
	}
	return n;
}

uintptr_t iusb_dev_wait_complete(iusb_dev_s *restrict dev, iusb_urb_s *urb_array[], uintptr_t urb_number, uintptr_t timeout_msec)
{
	uint64_t kill_msec;
	uintptr_t *restrict p;
	uintptr_t n, i;
	n = urb_number;
	if ((p = (uintptr_t *) alloca(sizeof(uintptr_t) * n)))
	{
		kill_msec = 0;
		for (i = 0; i < n; ++i)
		{
			p[i] = i + 1;
			if (urb_array[i]->last_submit_timestamp > kill_msec)
				kill_msec = urb_array[i]->last_submit_timestamp;
		}
		i = 0;
		kill_msec += timeout_msec;
		goto label_try;
		while (n && yaw_timestamp_msec() < kill_msec)
		{
			yaw_msleep(dev->wait_msec_gap);
			label_try:
			iusb_dev_do_events(dev);
			n = iusb_dev_wait_complete_check(urb_array, p, urb_number, &i);
		}
	}
	return n;
}

// urb

iusb_urb_s* iusb_urb_alloc(iusb_dev_s *restrict dev, uintptr_t urb_size)
{
	return iusb_inner_urb_alloc(dev, urb_size);
}

iusb_urb_s* iusb_urb_need_wait(iusb_urb_s *restrict urb)
{
	return iusb_inner_urb_need_wait(urb);
}

iusb_urb_s* iusb_urb_set_param(iusb_urb_s *restrict urb, iusb_endpoint_xfer_t xfer, uint32_t endpoint)
{
	return iusb_inner_urb_set_param(urb, xfer, endpoint);
}

iusb_urb_s* iusb_urb_fill_data_control(iusb_urb_s *restrict urb, uint32_t request_type, uint32_t request, uint32_t value, uint32_t index, const void *data, uintptr_t size)
{
	return iusb_inner_urb_fill_data_control(urb, request_type, request, value, index, data, size);
}

const void* iusb_urb_get_data_control(iusb_urb_s *restrict urb, uintptr_t *restrict rsize)
{
	return iusb_inner_urb_get_data_control(urb, rsize);
}

iusb_urb_s* iusb_urb_fill_data_bulk(iusb_urb_s *restrict urb, uint32_t stream_id, const void *data, uintptr_t size)
{
	return iusb_inner_urb_fill_data_bulk(urb, stream_id, data, size);
}

const void* iusb_urb_get_data_bulk(iusb_urb_s *restrict urb, uintptr_t *restrict rsize)
{
	return iusb_inner_urb_get_data_bulk(urb, rsize);
}

iusb_urb_s* iusb_urb_submit(iusb_urb_s *restrict urb)
{
	urb->last_submit_timestamp = yaw_timestamp_msec();
	return iusb_inner_urb_submit(urb);
}

iusb_urb_s* iusb_urb_discard(iusb_urb_s *restrict urb)
{
	return iusb_inner_urb_discard(urb);
}

// desc_string

iusb_desc_string_s* iusb_desc_string_alloc(iusb_dev_s *restrict dev, uintptr_t urb_number, uintptr_t timeout_msec)
{
	return iusb_inner_desc_string_alloc(dev, urb_number, timeout_msec);
}

iusb_desc_string_s* iusb_desc_string_submit_langid(iusb_desc_string_s *restrict ds)
{
	return iusb_inner_desc_string_submit_langid(ds);
}

const uint32_t* iusb_desc_string_get_langid(iusb_desc_string_s *restrict ds, uintptr_t *restrict number)
{
	return iusb_inner_desc_string_submit_and_get_langid(ds, number);
}

uint32_t iusb_desc_string_get_default_langid(iusb_desc_string_s *restrict ds)
{
	const uint32_t *restrict p;
	uintptr_t n;
	if ((p = iusb_inner_desc_string_submit_and_get_langid(ds, &n)) && n)
		return *p;
	return 0;
}

void iusb_desc_string_set_langid(iusb_desc_string_s *restrict ds, uint32_t langid)
{
	iusb_inner_desc_string_set_langid(ds, langid);
}

iusb_desc_string_s* iusb_desc_string_submit(iusb_desc_string_s *restrict ds, uint32_t desc_index)
{
	return iusb_inner_desc_string_submit(ds, desc_index);
}

refer_string_t iusb_desc_string_get(iusb_desc_string_s *restrict ds, uint32_t desc_index)
{
	return iusb_inner_desc_string_get(ds, desc_index);
}

refer_string_t iusb_desc_string_submit_get(iusb_desc_string_s *restrict ds, uint32_t desc_index)
{
	if (iusb_inner_desc_string_submit(ds, desc_index))
		return iusb_inner_desc_string_get(ds, desc_index);
	return NULL;
}

// preset

iusb_dev_speed_t iusb_dev_get_speed(iusb_dev_s *restrict dev)
{
	return iusb_inner_dev_get_speed(dev);
}

