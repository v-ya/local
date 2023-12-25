#include "iusb.inner.h"
#include <linux/usb/ch9.h>
#include <memory.h>

static void iusb_inner_device_attr_container_free_func(iusb_device_attr_container_s *restrict r)
{
	if (r->attr) refer_free(r->attr);
}

static iusb_device_attr_device_s* iusb_inner_device_attr_alloc_device(const struct usb_device_descriptor *restrict d)
{
	iusb_device_attr_device_s *restrict r;
	if (d->bLength == USB_DT_DEVICE_SIZE && (r = (iusb_device_attr_device_s *) refer_alloz(sizeof(iusb_device_attr_device_s))))
	{
		refer_set_free(r, (refer_free_f) iusb_inner_device_attr_container_free_func);
		if ((r->container.attr = vattr_alloc()))
		{
			r->device.usb_major          = iusb_major_from_bcd(d->bcdUSB);
			r->device.usb_minor          = iusb_minor_from_bcd(d->bcdUSB);
			r->device.device_class       = iusb_enum_from_u8(d->bDeviceClass, iusb_class_t);
			r->device.device_sub_class   = iusb_type_from_u8(d->bDeviceSubClass);
			r->device.device_protocol    = iusb_type_from_u8(d->bDeviceProtocol);
			r->device.max_packet_size    = iusb_type_from_u8(d->bMaxPacketSize0);
			r->device.vendor             = iusb_type_from_le16(d->idVendor);
			r->device.product            = iusb_type_from_le16(d->idProduct);
			r->device.device_major       = iusb_major_from_bcd(d->bcdDevice);
			r->device.device_minor       = iusb_minor_from_bcd(d->bcdDevice);
			r->device.desc_manufacturer  = iusb_type_from_u8(d->iManufacturer);
			r->device.desc_product       = iusb_type_from_u8(d->iProduct);
			r->device.desc_serial_number = iusb_type_from_u8(d->iSerialNumber);
			r->device.config_number      = iusb_type_from_u8(d->bNumConfigurations);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static iusb_device_attr_config_s* iusb_inner_device_attr_alloc_config(const struct usb_config_descriptor *restrict d)
{
	iusb_device_attr_config_s *restrict r;
	if (d->bLength == USB_DT_CONFIG_SIZE && (r = (iusb_device_attr_config_s *) refer_alloz(sizeof(iusb_device_attr_config_s))))
	{
		refer_set_free(r, (refer_free_f) iusb_inner_device_attr_container_free_func);
		if ((r->container.attr = vattr_alloc()))
		{
			r->config.interface_number = iusb_type_from_u8(d->bNumInterfaces);
			r->config.config_value     = iusb_type_from_u8(d->bConfigurationValue);
			r->config.desc_config      = iusb_type_from_u8(d->iConfiguration);
			r->config.flags            = iusb_enum_from_u8(d->bmAttributes, iusb_config_flags_t);
			r->config.max_power        = iusb_type_from_u8(d->bMaxPower);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static iusb_device_attr_interface_s* iusb_inner_device_attr_alloc_interface(const struct usb_interface_descriptor *restrict d)
{
	iusb_device_attr_interface_s *restrict r;
	if (d->bLength == USB_DT_INTERFACE_SIZE && (r = (iusb_device_attr_interface_s *) refer_alloz(sizeof(iusb_device_attr_interface_s))))
	{
		refer_set_free(r, (refer_free_f) iusb_inner_device_attr_container_free_func);
		if ((r->container.attr = vattr_alloc()))
		{
			r->interface.interface_index     = iusb_type_from_u8(d->bInterfaceNumber);
			r->interface.alternate_setting   = iusb_type_from_u8(d->bAlternateSetting);
			r->interface.endpoint_number     = iusb_type_from_u8(d->bNumEndpoints);
			r->interface.interface_class     = iusb_enum_from_u8(d->bInterfaceClass, iusb_class_t);
			r->interface.interface_sub_class = iusb_type_from_u8(d->bInterfaceSubClass);
			r->interface.interface_protocol  = iusb_type_from_u8(d->bInterfaceProtocol);
			r->interface.desc_interface      = iusb_type_from_u8(d->iInterface);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static iusb_device_attr_endpoint_s* iusb_inner_device_attr_alloc_endpoint(const struct usb_endpoint_descriptor *restrict d)
{
	iusb_device_attr_endpoint_s *restrict r;
	if ((d->bLength == USB_DT_ENDPOINT_SIZE || d->bLength == USB_DT_ENDPOINT_AUDIO_SIZE) &&
		(r = (iusb_device_attr_endpoint_s *) refer_alloz(sizeof(iusb_device_attr_endpoint_s))))
	{
		r->endpoint.endpoint_address     = iusb_enum_from_u8(d->bEndpointAddress, iusb_endpoint_address_t);
		r->endpoint.xfer                 = (iusb_endpoint_xfer_t) (iusb_type_from_u8(d->bmAttributes) & USB_ENDPOINT_XFERTYPE_MASK);
		r->endpoint.sync                 = (iusb_endpoint_sync_t) ((iusb_type_from_u8(d->bmAttributes) & USB_ENDPOINT_SYNCTYPE) >> 2);
		r->endpoint.intr                 = (iusb_endpoint_intr_t) ((iusb_type_from_u8(d->bmAttributes) & USB_ENDPOINT_INTRTYPE) >> 4);
		r->endpoint.max_packet_size      = iusb_type_from_le16(d->wMaxPacketSize) & USB_ENDPOINT_MAXP_MASK;
		r->endpoint.max_packet_size_mult = USB_EP_MAXP_MULT(iusb_type_from_le16(d->wMaxPacketSize)) + 1;
		r->endpoint.interval             = iusb_type_from_u8(d->bInterval);
		if (d->bLength == USB_DT_ENDPOINT_AUDIO_SIZE)
		{
			r->endpoint.refresh       = iusb_type_from_u8(d->bRefresh);
			r->endpoint.synch_address = iusb_type_from_u8(d->bSynchAddress);
		}
		return r;
	}
	return NULL;
}

static iusb_device_s* iusb_inner_device_alloc_parse(iusb_device_s *restrict r, const uint8_t *restrict desc, uintptr_t size)
{
	const struct usb_descriptor_header *restrict h;
	iusb_device_attr_device_s *restrict device;
	iusb_device_attr_config_s *restrict config;
	iusb_device_attr_interface_s *restrict interface;
	iusb_device_attr_endpoint_s *restrict endpoint;
	refer_t attr;
	uintptr_t n;
	device = NULL;
	config = NULL;
	interface = NULL;
	endpoint = NULL;
	attr = NULL;
	while (size)
	{
		if (size < sizeof(*h))
			goto label_fail;
		h = (const struct usb_descriptor_header *) desc;
		if ((n = h->bLength) > size)
			goto label_fail;
		switch (h->bDescriptorType)
		{
			case USB_DT_DEVICE:
				if (!(attr = device = iusb_inner_device_attr_alloc_device((const struct usb_device_descriptor *) h)))
					goto label_fail;
				if (!vattr_insert_tail(r->attr, iusb_key_device, attr))
					goto label_fail;
				refer_free(attr);
				attr = NULL;
				break;
			case USB_DT_CONFIG:
				if (!(attr = config = iusb_inner_device_attr_alloc_config((const struct usb_config_descriptor *) h)))
					goto label_fail;
				if (!vattr_insert_tail(r->attr, iusb_key_config, attr))
					goto label_fail;
				if (device && !vattr_insert_tail(device->container.attr, iusb_key_config, attr))
					goto label_fail;
				refer_free(attr);
				attr = NULL;
				break;
			case USB_DT_INTERFACE:
				if (!(attr = interface = iusb_inner_device_attr_alloc_interface((const struct usb_interface_descriptor *) h)))
					goto label_fail;
				if (!vattr_insert_tail(r->attr, iusb_key_interface, attr))
					goto label_fail;
				if (device && !vattr_insert_tail(device->container.attr, iusb_key_interface, attr))
					goto label_fail;
				if (config && !vattr_insert_tail(config->container.attr, iusb_key_interface, attr))
					goto label_fail;
				refer_free(attr);
				attr = NULL;
				break;
			case USB_DT_ENDPOINT:
				if (!(attr = endpoint = iusb_inner_device_attr_alloc_endpoint((const struct usb_endpoint_descriptor *) h)))
					goto label_fail;
				if (!vattr_insert_tail(r->attr, iusb_key_endpoint, attr))
					goto label_fail;
				if (device && !vattr_insert_tail(device->container.attr, iusb_key_endpoint, attr))
					goto label_fail;
				if (config && !vattr_insert_tail(config->container.attr, iusb_key_endpoint, attr))
					goto label_fail;
				if (interface && !vattr_insert_tail(interface->container.attr, iusb_key_endpoint, attr))
					goto label_fail;
				refer_free(attr);
				attr = NULL;
				break;
			default:
				break;
		}
		desc += n;
		size -= n;
	}
	return r;
	label_fail:
	if (attr) refer_free(attr);
	return NULL;
}

static void iusb_inner_device_free_func(iusb_device_s *restrict r)
{
	if (r->path) refer_free(r->path);
	if (r->id) refer_free(r->id);
	if (r->manufacturer) refer_free(r->manufacturer);
	if (r->product) refer_free(r->product);
	if (r->serial) refer_free(r->serial);
	if (r->attr) refer_free(r->attr);
}

iusb_device_s* iusb_inner_device_alloc(const uint8_t *restrict desc, uintptr_t size, const char *restrict path, const char *restrict id, uint32_t bus_id, uint32_t dev_id)
{
	iusb_device_s *restrict r;
	if ((r = (iusb_device_s *) refer_alloc(sizeof(iusb_device_s) + size)))
	{
		memset(r, 0, sizeof(iusb_device_s));
		refer_set_free(r, (refer_free_f) iusb_inner_device_free_func);
		r->path = refer_dump_string(path);
		r->id = refer_dump_string(id);
		r->attr = vattr_alloc();
		r->usb_descriptor_data = (const uint8_t *) memcpy(r + 1, desc, size);
		r->usb_descriptor_size = size;
		r->bus_id = bus_id;
		r->dev_id = dev_id;
		if (r->path && r->id && r->attr && iusb_inner_device_alloc_parse(r, desc, size))
			return r;
		refer_free(r);
	}
	return NULL;
}
