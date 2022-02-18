#include "iusb.inner.h"

static void iusb_inner_inst_free_func(iusb_inst_s *restrict r)
{
	if (r->device) refer_free(r->device);
	exbuffer_uini(&r->usb_desc_cache);
}

iusb_inst_s* iusb_inner_inst_alloc(uintptr_t usb_desc_cache_size)
{
	iusb_inst_s *restrict r;
	if ((r = (iusb_inst_s *) refer_alloz(sizeof(iusb_inst_s))))
	{
		refer_set_free(r, (refer_free_f) iusb_inner_inst_free_func);
		if (exbuffer_init(&r->usb_desc_cache, usb_desc_cache_size) &&
			(r->device = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

static exbuffer_t* iusb_inner_bus_scan_read_desc(exbuffer_t *restrict usb_descriptor, uintptr_t *restrict rsize, const char *restrict path)
{
	fsys_file_s *restrict fp;
	exbuffer_t *r;
	r = NULL;
	if ((fp = fsys_file_alloc(path, fsys_file_flag_read)))
	{
		if (fsys_file_read(fp, usb_descriptor->data, usb_descriptor->size, rsize) && *rsize)
			r = usb_descriptor;
		refer_free(fp);
	}
	return r;
}

uintptr_t iusb_inner_bus_scan(vattr_s *restrict pool, exbuffer_t *restrict usb_descriptor)
{
	fsys_dir_s *restrict dir;
	iusb_device_s *restrict dev;
	fsys_dir_item_t item;
	uintptr_t n, size;
	n = 0;
	vattr_clear(pool);
	if ((dir = fsys_dir_alloc(iusb_path_usbfs, fsys_type_cdev, fsys_dir_flag_looping | fsys_dir_flag_discard_hide)))
	{
		fsys_dir_set_max_depth(dir, 1);
		while (fsys_dir_read(dir, &item) && item.type)
		{
			if (iusb_inner_bus_scan_read_desc(usb_descriptor, &size, item.path) &&
				(dev = iusb_inner_device_alloc(usb_descriptor->data, size, item.path, item.rpath)))
			{
				if (vattr_insert_tail(pool, dev->id, dev))
					n += 1;
				refer_free(dev);
			}
		}
		refer_free(dir);
	}
	return n;
}

#include <stdio.h>

iusb_device_s* iusb_inner_bus_alloc_device(exbuffer_t *restrict usb_descriptor, uint32_t bus_id, uint32_t device_id)
{
	char path[64];
	uintptr_t size;
	sprintf(path, iusb_path_usbfs "%03u/%03u", bus_id, device_id);
	if (fsys_ctrl_exist(path, NULL) == fsys_type_cdev && iusb_inner_bus_scan_read_desc(usb_descriptor, &size, path))
		return iusb_inner_device_alloc(usb_descriptor->data, size, path, path + (sizeof(iusb_path_usbfs) - 1));
	return NULL;
}
