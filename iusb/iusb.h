#ifndef _iusb_h_
#define _iusb_h_

#include <refer.h>
#include <vattr.h>

typedef struct iusb_inst_s iusb_inst_s;
typedef struct iusb_device_s iusb_device_s;
typedef const vattr_s* iusb_pool_t;
typedef const vattr_s* iusb_attr_t;
typedef struct iusb_device_attr_device_s iusb_device_attr_device_s;
typedef struct iusb_device_attr_config_s iusb_device_attr_config_s;
typedef struct iusb_device_attr_interface_s iusb_device_attr_interface_s;
typedef struct iusb_device_attr_endpoint_s iusb_device_attr_endpoint_s;

typedef enum iusb_class_t {
	iusb_class_undefined           = 0x00, // device    由 interface 定义
	iusb_class_audio               = 0x01, // interface 音频
	iusb_class_cdc                 = 0x02, // both      通讯和 CDC 控制
	iusb_class_hid                 = 0x03, // interface 人机接口设备
	iusb_class_physical            = 0x05, // interface 物理设备
	iusb_class_image               = 0x06, // interface 静态图像
	iusb_class_printer             = 0x07, // interface 打印机
	iusb_class_msd                 = 0x08, // interface 大容量存储
	iusb_class_hub                 = 0x09, // device    集线器
	iusb_class_cdc_data            = 0x0a, // interface CDC 数据
	iusb_class_smart_card          = 0x0b, // interface 智能卡
	iusb_class_content_security    = 0x0d, // interface 内容安全
	iusb_class_video               = 0x0e, // interface 视频
	iusb_class_personal_healthcare = 0x0f, // interface 个人医疗保健
	iusb_class_audio_video         = 0x10, // interface 音视频设备
	iusb_class_billboard           = 0x11, // device    广告牌设备
	iusb_class_usb_type_c_bridge   = 0x12, // interface USB Type-C 桥接设备
	iusb_class_i3c                 = 0x3c, // interface I3C 设备
	iusb_class_diagnostic          = 0xdc, // both      诊断设备
	iusb_class_wireless_controller = 0xe0, // interface 无线控制器
	iusb_class_misc                = 0xef, // both      其他、杂项
	iusb_class_app_spec            = 0xfe, // interface 应用特定
	iusb_class_vendor_spec         = 0xff, // both      供应商特定
} iusb_class_t;

typedef enum iusb_config_flags_t {
	iusb_config_flags_one        = 0x80, // 必须被设置
	iusb_config_flags_self_power = 0x40, // 宿主供电
	iusb_config_flags_wakeup     = 0x20, // 允许唤醒
	iusb_config_flags_battery    = 0x10, // 电池供电
} iusb_config_flags_t;

typedef enum iusb_endpoint_address_t {
	iusb_endpoint_address_mask_number = 0x0f, // 地址号

	iusb_endpoint_address_mask_dir    = 0x80, // 数据方向
	iusb_endpoint_address_dir_out     = 0x00, // => device
	iusb_endpoint_address_dir_in      = 0x80, // => host
} iusb_endpoint_address_t;

typedef enum iusb_endpoint_xfer_t {
	iusb_endpoint_xfer_control      = 0x00,
	iusb_endpoint_xfer_isoc         = 0x01,
	iusb_endpoint_xfer_bulk         = 0x02,
	iusb_endpoint_xfer_interrupt    = 0x03,
} iusb_endpoint_xfer_t;

typedef enum iusb_endpoint_sync_t {
	iusb_endpoint_sync_none         = 0x00,
	iusb_endpoint_sync_async        = 0x01,
	iusb_endpoint_sync_adaptive     = 0x02,
	iusb_endpoint_sync_sync         = 0x03,
} iusb_endpoint_sync_t;

typedef enum iusb_endpoint_intr_t {
	iusb_endpoint_intr_periodic     = 0x00,
	iusb_endpoint_intr_notification = 0x01,
} iusb_endpoint_intr_t;

typedef struct iusb_attr_device_t {
	uint32_t usb_major;
	uint32_t usb_minor;
	iusb_class_t device_class;
	uint32_t device_sub_class;
	uint32_t device_protocol;
	uint32_t max_packet_size;
	uint32_t vendor;
	uint32_t product;
	uint32_t device_major;
	uint32_t device_minor;
	uint32_t desc_manufacturer;
	uint32_t desc_product;
	uint32_t desc_serial_number;
	uint32_t config_number;
} iusb_attr_device_t;

typedef struct iusb_attr_config_t {
	uint32_t interface_number;
	uint32_t config_value;
	uint32_t desc_config;
	iusb_config_flags_t flags;
	uint32_t max_power;
} iusb_attr_config_t;

typedef struct iusb_attr_interface_t {
	uint32_t interface_index;
	uint32_t alternate_setting;
	uint32_t endpoint_number;
	iusb_class_t interface_class;
	uint32_t interface_sub_class;
	uint32_t interface_protocol;
	uint32_t desc_interface;
} iusb_attr_interface_t;

typedef struct iusb_attr_endpoint_t {
	iusb_endpoint_address_t endpoint_address;
	iusb_endpoint_xfer_t xfer;
	iusb_endpoint_sync_t sync;
	iusb_endpoint_intr_t intr;
	uint32_t max_packet_size;
	uint32_t max_packet_size_mult;
	uint32_t interval;
	uint32_t refresh;
	uint32_t synch_address;
} iusb_attr_endpoint_t;

iusb_inst_s* iusb_inst_alloc(void);
iusb_pool_t iusb_inst_pool(iusb_inst_s *restrict inst);
uintptr_t iusb_inst_update(iusb_inst_s *restrict inst);
iusb_device_s* iusb_inst_alloc_device(iusb_inst_s *restrict inst, uint32_t bus_id, uint32_t device_id);

refer_string_t iusb_device_path(const iusb_device_s *restrict device);
refer_string_t iusb_device_id(const iusb_device_s *restrict device);
iusb_attr_t iusb_device_attr(const iusb_device_s *restrict device);

const vattr_vslot_t* iusb_attr_find_device(iusb_attr_t attr);
const iusb_attr_device_t* iusb_attr_device_data(const iusb_device_attr_device_s *restrict device);
iusb_attr_t iusb_attr_device_attr(const iusb_device_attr_device_s *restrict device);

const vattr_vslot_t* iusb_attr_find_config(iusb_attr_t attr);
const iusb_attr_config_t* iusb_attr_config_data(const iusb_device_attr_config_s *restrict config);
iusb_attr_t iusb_attr_config_attr(const iusb_device_attr_config_s *restrict config);

const vattr_vslot_t* iusb_attr_find_interface(iusb_attr_t attr);
const iusb_attr_interface_t* iusb_attr_interface_data(const iusb_device_attr_interface_s *restrict interface);
iusb_attr_t iusb_attr_interface_attr(const iusb_device_attr_interface_s *restrict interface);

const vattr_vslot_t* iusb_attr_find_endpoint(iusb_attr_t attr);
const iusb_attr_endpoint_t* iusb_attr_endpoint_data(const iusb_device_attr_endpoint_s *restrict endpoint);

#endif
