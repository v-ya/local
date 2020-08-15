#ifndef _graph_type_h_
#define _graph_type_h_

typedef enum graph_layer_e {
	graph_layer_null,
	graph_layer_validation,
	graph_layer_api_dump,
	graph_layer_number
} graph_layer_t;

typedef enum graph_extension_e {
	graph_extension_null,
	graph_extension_ext_debug_utils,
	graph_extension_number
} graph_extension_t;

typedef enum graph_debug_message_level_e {
	graph_debug_message_level_version = 0x0001,
	graph_debug_message_level_info    = 0x0010,
	graph_debug_message_level_warning = 0x0100,
	graph_debug_message_level_error   = 0x1000
} graph_debug_message_level_t;

typedef enum graph_debug_message_type_e {
	graph_debug_message_type_general     = 0x0001,
	graph_debug_message_type_validation  = 0x0002,
	graph_debug_message_type_performance = 0x0004
} graph_debug_message_type_t;

typedef enum graph_physical_device_type_e {
    graph_physical_device_type_other          = 0,
    graph_physical_device_type_integrated_gpu = 1,
    graph_physical_device_type_discrete_gpu   = 2,
    graph_physical_device_type_virtual_gpu    = 3,
    graph_physical_device_type_cpu            = 4
} graph_physical_device_type_t;

#endif
