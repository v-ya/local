#ifndef _iusb_inner_h_
#define _iusb_inner_h_

#include "iusb.h"
#include <exbuffer.h>
#include <fsys.h>
#include <linux/usbdevice_fs.h>

typedef struct iusb_device_attr_container_s iusb_device_attr_container_s;

struct iusb_inst_s {
	vattr_s *device;
	exbuffer_t usb_desc_cache;
};

struct iusb_device_s {
	refer_string_t path;
	refer_string_t id;
	vattr_s *attr;
	const uint8_t *usb_descriptor_data;
	uintptr_t usb_descriptor_size;
};

struct iusb_device_attr_container_s {
	vattr_s *attr;
};

struct iusb_device_attr_device_s {
	iusb_device_attr_container_s container;
	iusb_attr_device_t device;
};

struct iusb_device_attr_config_s {
	iusb_device_attr_container_s container;
	iusb_attr_config_t config;
};

struct iusb_device_attr_interface_s {
	iusb_device_attr_container_s container;
	iusb_attr_interface_t interface;
};

struct iusb_device_attr_endpoint_s {
	iusb_attr_endpoint_t endpoint;
};

struct iusb_dev_s {
	int fd;
	uintptr_t wait_msec_gap;
};

struct iusb_urb_s {
	iusb_dev_s *dev;
	struct usbdevfs_urb *volatile urb;
	exbuffer_t data;
	struct {
		struct usbdevfs_urb urb;
		struct usbdevfs_iso_packet_desc iso_frame_desc;
	} urb_header;
};

#define iusb_path_usbfs    "/dev/bus/usb/"

#define iusb_key_device    "device"
#define iusb_key_config    "config"
#define iusb_key_interface "interface"
#define iusb_key_endpoint  "endpoint"

#define iusb_type_to_u8(_v)         ((uint8_t) (_v))
#define iusb_type_from_u8(_v)       ((uint32_t) (_v))
#define iusb_enum_from_u8(_v, _t)   ((_t) (_v))

#define iusb_type_to_le16(_v)       ((uint16_t) (_v))
#define iusb_type_from_le16(_v)     ((uint32_t) (_v))
#define iusb_enum_from_le16(_v, _t) ((_t) (_v))

#define iusb_major_from_bcd(_v)     iusb_number_from_bcd2(iusb_type_from_le16(_v) >> 8)
#define iusb_minor_from_bcd(_v)     iusb_number_from_bcd2(iusb_type_from_le16(_v) & 0xff)

// inline

static inline uint32_t iusb_number_from_bcd2(uint32_t bcd2)
{
	return (bcd2 >> 4) * 10 + (bcd2 & 15);
}

// bus

iusb_inst_s* iusb_inner_inst_alloc(uintptr_t usb_desc_cache_size);
uintptr_t iusb_inner_bus_scan(vattr_s *restrict pool, exbuffer_t *restrict usb_descriptor);
iusb_device_s* iusb_inner_bus_alloc_device(exbuffer_t *restrict usb_descriptor, uint32_t bus_id, uint32_t device_id);

// device

iusb_device_s* iusb_inner_device_alloc(const uint8_t *restrict desc, uintptr_t size, const char *restrict path, const char *restrict id);

// dev

iusb_dev_s* iusb_inner_dev_alloc(const char *restrict dev_path);
iusb_dev_speed_t iusb_inner_dev_get_speed(iusb_dev_s *restrict dev);
iusb_dev_s* iusb_inner_dev_submit_urb(iusb_dev_s *restrict dev, struct usbdevfs_urb *restrict urb);
iusb_dev_s* iusb_inner_dev_discard_urb(iusb_dev_s *restrict dev, struct usbdevfs_urb *restrict urb);
iusb_urb_s* iusb_inner_dev_reap_urb(iusb_dev_s *restrict dev);

// urb

iusb_urb_s* iusb_inner_urb_alloc(iusb_dev_s *restrict dev, uintptr_t urb_size);
iusb_urb_s* iusb_inner_urb_need_wait(iusb_urb_s *restrict urb);
void iusb_inner_urb_reap(iusb_urb_s *restrict urb);
iusb_urb_s* iusb_inner_urb_set_param(iusb_urb_s *restrict urb, iusb_endpoint_xfer_t xfer, uint32_t endpoint);
iusb_urb_s* iusb_inner_urb_fill_data_control(iusb_urb_s *restrict urb, uint32_t request_type, uint32_t request, uint32_t value, uint32_t index, const void *data, uintptr_t size);
const void* iusb_inner_urb_get_data_control(iusb_urb_s *restrict urb, uintptr_t *restrict rsize);
iusb_urb_s* iusb_inner_urb_submit(iusb_urb_s *restrict urb);
iusb_urb_s* iusb_inner_urb_discard(iusb_urb_s *restrict urb);

#endif
