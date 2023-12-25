#include "iusb.inner.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

static void iusb_inner_inst_free_func(iusb_inst_s *restrict r)
{
	if (r->device) refer_free(r->device);
	if (r->rpath) refer_free(r->rpath);
	exbuffer_uini(&r->cache);
}

iusb_inst_s* iusb_inner_inst_alloc(uintptr_t cache_size)
{
	iusb_inst_s *restrict r;
	if ((r = (iusb_inst_s *) refer_alloz(sizeof(iusb_inst_s))))
	{
		refer_set_free(r, (refer_free_f) iusb_inner_inst_free_func);
		if (exbuffer_init(&r->cache, cache_size) &&
			(r->rpath = fsys_rpath_alloc(iusb_path_usbsys, 1)) &&
			(r->device = vattr_alloc()))
			return r;
		refer_free(r);
	}
	return NULL;
}

static uint32_t iusb_inner_bus_scan_get_number(exbuffer_t *restrict cache, const char *restrict path)
{
	fsys_file_s *restrict fp;
	uintptr_t n;
	uint32_t r;
	r = 0;
	if (path && (fp = fsys_file_alloc(path, fsys_file_flag_read)))
	{
		if (fsys_file_read(fp, cache->data, cache->size - 1, &n) && n)
		{
			cache->data[n] = 0;
			r = (uint32_t) strtoul((const char *) cache->data, NULL, 10);
		}
		refer_free(fp);
	}
	return r;
}

static refer_string_t iusb_inner_bus_scan_get_string(exbuffer_t *restrict cache, const char *restrict path)
{
	fsys_file_s *restrict fp;
	const char *restrict p;
	uintptr_t n;
	p = NULL;
	if (path && (fp = fsys_file_alloc(path, fsys_file_flag_read)))
	{
		if (fsys_file_read(fp, cache->data, cache->size, &n) && n)
		{
			p = (const char *) cache->data;
			while (n && p[n - 1] == '\n') n -= 1;
		}
		refer_free(fp);
	}
	if (p) return refer_dump_string_with_length(p, n);
	return NULL;
}

static exbuffer_t* iusb_inner_bus_scan_read_data(exbuffer_t *restrict cache, uintptr_t *restrict rsize, const char *restrict path)
{
	fsys_file_s *restrict fp;
	exbuffer_t *r;
	r = NULL;
	if (path && (fp = fsys_file_alloc(path, fsys_file_flag_read)))
	{
		if (fsys_file_read(fp, cache->data, cache->size, rsize) && *rsize)
			r = cache;
		refer_free(fp);
	}
	return r;
}

static iusb_device_s* iusb_inner_bus_alloc_device_inner(const uint8_t *descriptors, uintptr_t size, uint32_t bus_id, uint32_t dev_id, exbuffer_t *restrict cache)
{
	char path[64];
	sprintf(path, iusb_path_usbfs "%03u/%03u", bus_id, dev_id);
	if (fsys_ctrl_exist(path, NULL) == fsys_type_cdev)
	{
		if (descriptors && size)
			return iusb_inner_device_alloc(descriptors, size, path, path + (sizeof(iusb_path_usbfs) - 1), bus_id, dev_id);
		else if (cache && iusb_inner_bus_scan_read_data(cache, &size, path))
			return iusb_inner_device_alloc(cache->data, size, path, path + (sizeof(iusb_path_usbfs) - 1), bus_id, dev_id);
	}
	return NULL;
}

static iusb_device_s* iusb_inner_bus_scan_alloc_device(exbuffer_t *restrict cache, fsys_rpath_s *restrict rpath, uintptr_t parse_string)
{
	iusb_device_s *restrict device;
	uintptr_t size;
	uint32_t bus_id, dev_id;
	bus_id = iusb_inner_bus_scan_get_number(cache, fsys_rpath_get_full_path(rpath, "busnum"));
	dev_id = iusb_inner_bus_scan_get_number(cache, fsys_rpath_get_full_path(rpath, "devnum"));
	if (bus_id && dev_id && iusb_inner_bus_scan_read_data(
		cache, &size, fsys_rpath_get_full_path(rpath, "descriptors")))
	{
		device = iusb_inner_bus_alloc_device_inner(cache->data, size, bus_id, dev_id, NULL);
		if (parse_string && device)
		{
			device->manufacturer = iusb_inner_bus_scan_get_string(cache, fsys_rpath_get_full_path(rpath, "manufacturer"));
			device->product = iusb_inner_bus_scan_get_string(cache, fsys_rpath_get_full_path(rpath, "product"));
			device->serial = iusb_inner_bus_scan_get_string(cache, fsys_rpath_get_full_path(rpath, "serial"));
		}
		return device;
	}
	return NULL;
}

uintptr_t iusb_inner_bus_scan(vattr_s *restrict pool, fsys_rpath_s *restrict rpath, exbuffer_t *restrict cache, uintptr_t parse_string)
{
	fsys_dir_s *restrict dir;
	iusb_device_s *restrict dev;
	fsys_dir_item_t item;
	uintptr_t n;
	n = 0;
	vattr_clear(pool);
	if ((dir = fsys_dir_alloc(fsys_rpath_get_full_path_with_length(rpath, NULL, 0),
		fsys_type_dirent, fsys_dir_flag_link_type_parse | fsys_dir_flag_discard_hide)))
	{
		while (fsys_dir_read(dir, &item) && item.type)
		{
			if (!strchr(item.name, ':'))
			{
				if (fsys_rpath_push(rpath, item.name))
				{
					if ((dev = iusb_inner_bus_scan_alloc_device(cache, rpath, parse_string)))
					{
						if (vattr_insert_tail(pool, dev->id, dev))
							n += 1;
						refer_free(dev);
					}
					fsys_rpath_pop(rpath);
				}
			}
		}
		refer_free(dir);
	}
	return n;
}

iusb_device_s* iusb_inner_bus_alloc_device(exbuffer_t *restrict cache, uint32_t bus_id, uint32_t dev_id)
{
	return iusb_inner_bus_alloc_device_inner(NULL, 0, bus_id, dev_id, cache);
}
