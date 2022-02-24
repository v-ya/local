#include "iusb.inner.h"
#include <sys/ioctl.h>
#include <linux/ioctl.h>
#include <fcntl.h>
#include <unistd.h>

static void iusb_inner_dev_free_func(iusb_dev_s *restrict r)
{
	if (~r->fd) close(r->fd);
}

iusb_dev_s* iusb_inner_dev_alloc(const char *restrict dev_path)
{
	iusb_dev_s *restrict r;
	if ((r = (iusb_dev_s *) refer_alloz(sizeof(iusb_dev_s))))
	{
		r->fd = ~0;
		refer_set_free(r, (refer_free_f) iusb_inner_dev_free_func);
		r->wait_msec_gap = 5;
		if (~(r->fd = open(dev_path, O_RDWR, 0644)))
			return r;
		refer_free(r);
	}
	return NULL;
}

iusb_dev_speed_t iusb_dev_get_speed(iusb_dev_s *restrict dev)
{
	int r;
	switch ((r = ioctl(dev->fd, USBDEVFS_GET_SPEED, NULL)))
	{
		case iusb_dev_speed_unknow:
		case iusb_dev_speed_low:
		case iusb_dev_speed_full:
		case iusb_dev_speed_high:
		case iusb_dev_speed_wireless:
		case iusb_dev_speed_super:
		case iusb_dev_speed_super_plus:
			return (iusb_dev_speed_t) r;
		default:
			return iusb_dev_speed_unknow;
	}
}

iusb_dev_s* iusb_dev_interface_claim(iusb_dev_s *restrict dev, uint32_t interface)
{
	
	if (!ioctl(dev->fd, USBDEVFS_CLAIMINTERFACE, &interface))
		return dev;
	return NULL;
}

iusb_dev_s* iusb_dev_interface_release(iusb_dev_s *restrict dev, uint32_t interface)
{
	
	if (!ioctl(dev->fd, USBDEVFS_RELEASEINTERFACE, &interface))
		return dev;
	return NULL;
}

iusb_dev_s* iusb_dev_clear_halt(iusb_dev_s *restrict dev, uint32_t endpoint)
{
	
	if (!ioctl(dev->fd, USBDEVFS_CLEAR_HALT, &endpoint))
		return dev;
	return NULL;
}

iusb_dev_s* iusb_inner_dev_submit_urb(iusb_dev_s *restrict dev, struct usbdevfs_urb *restrict urb)
{
	if (!ioctl(dev->fd, USBDEVFS_SUBMITURB, urb))
		return dev;
	return NULL;
}

iusb_dev_s* iusb_inner_dev_discard_urb(iusb_dev_s *restrict dev, struct usbdevfs_urb *restrict urb)
{
	if (!ioctl(dev->fd, USBDEVFS_DISCARDURB, urb))
		return dev;
	return NULL;
}

iusb_urb_s* iusb_inner_dev_reap_urb(iusb_dev_s *restrict dev)
{
	struct usbdevfs_urb *urb;
	urb = NULL;
	if (ioctl(dev->fd, USBDEVFS_REAPURBNDELAY, &urb) >= 0 && urb)
		return (iusb_urb_s *) urb->usercontext;
	return NULL;
}
