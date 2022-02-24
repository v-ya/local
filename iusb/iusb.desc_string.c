#include "iusb.inner.h"
#include <linux/usb/ch9.h>

#define iusb_inner_desc_string_max_size  256

typedef struct iusb_desc_string_urb_t iusb_desc_string_urb_t;

struct iusb_desc_string_urb_t {
	iusb_desc_string_urb_t *next;
	iusb_urb_s *urb;
	uint64_t key;
};

struct iusb_desc_string_s {
	iusb_dev_s *dev;
	rbtree_t *string_cache;
	uintptr_t langid;
	uintptr_t timeout_msec;
	uintptr_t urb_number;
	iusb_desc_string_urb_t *used;
	iusb_desc_string_urb_t *free;
	iusb_desc_string_urb_t urb[];
};

static void iusb_inner_desc_string_free_func(iusb_desc_string_s *restrict r)
{
	for (uintptr_t i = 0; i < r->urb_number; ++i)
		if (r->urb[i].urb) refer_free(r->urb[i].urb);
	if (r->string_cache) rbtree_clear(&r->string_cache);
	if (r->dev) refer_free(r->dev);
}

static void iusb_inner_desc_string_rbtree_free_func(rbtree_t *restrict rbv)
{
	if (rbv->value) refer_free(rbv->value);
}

static uintptr_t iusb_inner_desc_string_unicode16le_to_utf8(char *restrict utf8, const uint16_t *restrict unicode16le, uintptr_t n)
{
	// 0xxxxxxx                   :  7 bits 00000000 0xxxxxxx
	// 110xxxxx 10xxxxxx          : 11 bits 00000xxx xxxxxxxx
	// 1110xxxx 10xxxxxx 10xxxxxx : 16 bits xxxxxxxx xxxxxxxx
	uintptr_t i;
	int16_t u;
	i = 0;
	while (n)
	{
		u = *unicode16le++;
		if (!(u & 0xff80))
			utf8[i++] = (char) u;
		else if (!(u & 0xf800))
		{
			utf8[i++] = (char) ((u >> 6) | 0xc0);
			utf8[i++] = (char) ((u & 0x003f) | 0x80);
		}
		else
		{
			utf8[i++] = (char) ((u >> 12) | 0xe0);
			utf8[i++] = (char) (((u >> 6) & 0x003f) | 0x80);
			utf8[i++] = (char) ((u & 0x003f) | 0x80);
		}
		--n;
	}
	return i;
}

static void* iusb_inner_desc_string_get_by_urb(iusb_desc_string_urb_t *restrict urb)
{
	const struct usb_string_descriptor *restrict usd;
	uintptr_t n;
	if ((usd = iusb_urb_get_data_control(urb->urb, &n)))
	{
		const uint16_t *restrict p;
		if (iusb_enum_from_u8(usd->bLength, uintptr_t) < n)
			n = iusb_enum_from_u8(usd->bLength, uintptr_t);
		if (n >= offsetof(struct usb_string_descriptor, wData))
		{
			p = (const uint16_t *) ((const uint8_t *) usd + offsetof(struct usb_string_descriptor, wData));
			n = (n - offsetof(struct usb_string_descriptor, wData)) / sizeof(uint16_t);
			if (urb->key)
			{
				// data
				char buffer[iusb_inner_desc_string_max_size * 2];
				return (void *) refer_dump_string_with_length(buffer, iusb_inner_desc_string_unicode16le_to_utf8(buffer, p, n));
			}
			else
			{
				// langid
				uint32_t *restrict langid_array;
				uintptr_t i;
				langid_array = (uint32_t *) refer_alloc(sizeof(uint32_t) * (n + 1));
				if (langid_array)
				{
					langid_array[0] = (uint32_t) n;
					for (i = 0; i < n; ++i)
						langid_array[i + 1] = (uint32_t) p[i];
					return langid_array;
				}
			}
		}
	}
	return NULL;
}

static iusb_desc_string_s* iusb_inner_desc_string_reap_urb(iusb_desc_string_s *restrict r, iusb_desc_string_urb_t *restrict urb)
{
	rbtree_t *restrict v;
	if ((v = rbtree_find(&r->string_cache, NULL, urb->key)))
	{
		if (v->free)
			v->free(v);
		v->value = iusb_inner_desc_string_get_by_urb(urb);
		v->free = iusb_inner_desc_string_rbtree_free_func;
		if (v->value)
			return r;
	}
	return NULL;
}

static uintptr_t iusb_inner_desc_string_check_used(iusb_desc_string_s *restrict r)
{
	iusb_desc_string_urb_t **restrict p;
	iusb_desc_string_urb_t *restrict urb;
	uintptr_t n;
	n = 0;
	p = &r->used;
	while ((urb = *p))
	{
		if (iusb_urb_need_wait(urb->urb))
			p = &urb->next;
		else
		{
			*p = urb->next;
			iusb_inner_desc_string_reap_urb(r, urb);
			urb->next = r->free;
			r->free = urb;
			n += 1;
		}
	}
	return n;
}

static iusb_desc_string_urb_t* iusb_inner_desc_string_find_free_urb(iusb_desc_string_s *restrict r)
{
	iusb_desc_string_urb_t *restrict urb;
	if ((urb = r->free))
		goto label_okay;
	iusb_dev_do_events(r->dev);
	if (iusb_inner_desc_string_check_used(r) && (urb = r->free))
		goto label_okay;
	return NULL;
	label_okay:
	r->free = urb->next;
	urb->next = NULL;
	return urb;
}

static inline iusb_desc_string_s* iusb_inner_desc_string_is_urb(iusb_desc_string_s *restrict r, void *value)
{
	if ((uintptr_t) value >= (uintptr_t) r->urb && (uintptr_t) value < (uintptr_t) (r->urb + r->urb_number))
		return r;
	return NULL;
}

static inline uint64_t iusb_inner_desc_string_key(uint32_t langid, uint32_t desc_index)
{
	return ((uint64_t) langid << 32) | desc_index;
}

static iusb_desc_string_s* iusb_inner_desc_string_submit_inner(iusb_desc_string_s *restrict r, uint32_t langid, uint32_t desc_index)
{
	iusb_desc_string_urb_t *restrict urb;
	rbtree_t *restrict v;
	uint64_t key;
	key = iusb_inner_desc_string_key(langid, desc_index);
	if (rbtree_find(&r->string_cache, NULL, key))
		goto label_okay;
	if ((urb = iusb_inner_desc_string_find_free_urb(r)))
	{
		urb->key = key;
		if (iusb_urb_fill_data_control(
			urb->urb,
			iusb_endpoint_address_dir_in | 0, USB_REQ_GET_DESCRIPTOR,
			(USB_DT_STRING << 8) | (desc_index & 0xff),
			langid,
			NULL,
			iusb_inner_desc_string_max_size) &&
			iusb_urb_submit(urb->urb))
		{
			urb->next = r->used;
			r->used = urb;
			if ((v = rbtree_insert(&r->string_cache, NULL, key, urb, NULL)))
			{
				label_okay:
				return r;
			}
			iusb_urb_discard(urb->urb);
		}
		else
		{
			urb->next = r->free;
			r->free = urb;
		}
	}
	return NULL;
}

static refer_t iusb_inner_desc_string_get_inner(iusb_desc_string_s *restrict r, uint32_t langid, uint32_t desc_index)
{
	iusb_desc_string_urb_t *restrict urb;
	rbtree_t *restrict v;
	uint64_t key;
	key = iusb_inner_desc_string_key(langid, desc_index);
	if ((v = rbtree_find(&r->string_cache, NULL, key)))
	{
		if (iusb_inner_desc_string_is_urb(r, urb = (iusb_desc_string_urb_t *) v->value))
		{
			iusb_dev_wait_complete(r->dev, &urb->urb, 1, r->timeout_msec);
			iusb_inner_desc_string_check_used(r);
			if (((volatile rbtree_t *) v)->value != urb)
				return ((volatile rbtree_t *) v)->value;
			iusb_urb_discard(urb->urb);
			if (v->free) v->free(v);
			v->value = NULL;
		}
		return v->value;
	}
	return NULL;
}

iusb_desc_string_s* iusb_desc_string_alloc(iusb_dev_s *restrict dev, uintptr_t urb_number, uintptr_t timeout_msec)
{
	iusb_desc_string_s *restrict r;
	uintptr_t i;
	if (urb_number)
	{
		if ((r = (iusb_desc_string_s *) refer_alloz(sizeof(iusb_desc_string_s) + sizeof(iusb_desc_string_urb_t) * urb_number)))
		{
			refer_set_free(r, (refer_free_f) iusb_inner_desc_string_free_func);
			r->dev = (iusb_dev_s *) refer_save(dev);
			r->timeout_msec = timeout_msec;
			r->urb_number = urb_number;
			for (i = 0; i < urb_number; ++i)
			{
				if (!(r->urb[i].urb = iusb_urb_alloc(dev, sizeof(struct usb_ctrlrequest) + iusb_inner_desc_string_max_size)))
					goto label_fail;
				if (!iusb_urb_set_param(r->urb[i].urb, iusb_endpoint_xfer_control, 0))
					goto label_fail;
				r->urb[i].next = r->free;
				r->free = r->urb + i;
			}
			return r;
			label_fail:
			refer_free(r);
		}
	}
	return NULL;
}

iusb_desc_string_s* iusb_desc_string_submit_langid(iusb_desc_string_s *restrict ds)
{
	return iusb_inner_desc_string_submit_inner(ds, 0, 0);
}

const uint32_t* iusb_desc_string_submit_and_get_langid(iusb_desc_string_s *restrict ds, uintptr_t *restrict number)
{
	const uint32_t *restrict p;
	if (iusb_inner_desc_string_submit_inner(ds, 0, 0) &&
		(p = iusb_inner_desc_string_get_inner(ds, 0, 0)))
	{
		if (number) *number = (uintptr_t) *p;
		return p + 1;
	}
	if (number) *number = 0;
	return NULL;
}

uint32_t iusb_desc_string_get_default_langid(iusb_desc_string_s *restrict ds)
{
	const uint32_t *restrict p;
	uintptr_t n;
	if ((p = iusb_desc_string_submit_and_get_langid(ds, &n)) && n)
		return *p;
	return 0;
}

void iusb_desc_string_set_langid(iusb_desc_string_s *restrict ds, uint32_t langid)
{
	ds->langid = langid;
}

iusb_desc_string_s* iusb_desc_string_submit(iusb_desc_string_s *restrict ds, uint32_t desc_index)
{
	if (ds->langid && desc_index)
		return iusb_inner_desc_string_submit_inner(ds, ds->langid, desc_index);
	return NULL;
}

refer_string_t iusb_desc_string_get(iusb_desc_string_s *restrict ds, uint32_t desc_index)
{
	if (ds->langid && desc_index)
		return iusb_inner_desc_string_get_inner(ds, ds->langid, desc_index);
	return NULL;
}

refer_string_t iusb_desc_string_submit_get(iusb_desc_string_s *restrict ds, uint32_t desc_index)
{
	if (ds->langid && desc_index && iusb_inner_desc_string_submit_inner(ds, ds->langid, desc_index))
		return iusb_inner_desc_string_get_inner(ds, ds->langid, desc_index);
	return NULL;
}
