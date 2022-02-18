#ifndef _iusb_h_
#define _iusb_h_

#include <refer.h>
#include <vattr.h>

typedef struct iusb_pool_s iusb_pool_s;
typedef struct iusb_device_s iusb_device_s;

iusb_pool_s* iusb_pool_alloc(void);
vattr_s* iusb_pool_vattr(iusb_pool_s *restrict pool);
uintptr_t iusb_pool_update(iusb_pool_s *restrict pool);

refer_string_t iusb_device_path(const iusb_device_s *restrict device);
refer_string_t iusb_device_id(const iusb_device_s *restrict device);

#endif
