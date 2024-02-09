#ifndef _gvcx_model_gguf_h_
#define _gvcx_model_gguf_h_

#include <stdint.h>

// default little-endian

typedef enum gguf_tensor_type_t gguf_tensor_type_t;
typedef enum gguf_metadata_type_t gguf_metadata_type_t;
typedef struct gguf_string_t gguf_string_t;
typedef struct gguf_array_t gguf_array_t;
typedef union gguf_metadata_t gguf_metadata_t;
typedef struct gguf_metadata_kv_t gguf_metadata_kv_t;
typedef struct gguf_header_t gguf_header_t;
typedef struct gguf_tensor_info_t gguf_tensor_info_t;
typedef struct gguf_file_t gguf_file_t;

enum gguf_tensor_type_t {
	gguf_tensor_type__f32  = 0,
	gguf_tensor_type__f16  = 1,
	gguf_tensor_type__q4_0 = 2,
	gguf_tensor_type__q4_1 = 3,
	gguf_tensor_type__q4_2 = 4, // support has been removed
	gguf_tensor_type__q4_3 = 5, // support has been removed
	gguf_tensor_type__q5_0 = 6,
	gguf_tensor_type__q5_1 = 7,
	gguf_tensor_type__q8_0 = 8,
	gguf_tensor_type__q8_1 = 9,
	gguf_tensor_type__q2_K = 10,
	gguf_tensor_type__q3_K = 11,
	gguf_tensor_type__q4_K = 12,
	gguf_tensor_type__q5_K = 13,
	gguf_tensor_type__q6_K = 14,
	gguf_tensor_type__q8_K = 15,
	gguf_tensor_type__i8   = 16,
	gguf_tensor_type__i16  = 17,
	gguf_tensor_type__i32  = 18,
	gguf_tensor_type_count
};

enum gguf_metadata_type_t {
	gguf_metadata_type__uint8   = 0,  // 8-bit unsigned integer
	gguf_metadata_type__int8    = 1,  // 8-bit signed integer
	gguf_metadata_type__uint16  = 2,  // 16-bit unsigned integer
	gguf_metadata_type__int16   = 3,  // 16-bit signed integer
	gguf_metadata_type__uint32  = 4,  // 32-bit unsigned integer
	gguf_metadata_type__int32   = 5,  // 32-bit signed integer
	gguf_metadata_type__float32 = 6,  // 32-bit ieee754 floating point number
	gguf_metadata_type__bool    = 7,  // 1-byte boolean (0:false|1:true)
	gguf_metadata_type__string  = 8,  // utf-8 non-null-terminated string, with length prepended
	gguf_metadata_type__array   = 9,  // array of other(allow nested) values, with length(elements) and type prepended
	gguf_metadata_type__uint64  = 10, // 64-bit unsigned integer
	gguf_metadata_type__int64   = 11, // 64-bit signed integer
	gguf_metadata_type__float64 = 12, // 64-bit ieee754 floating point number
	gguf_metadata_type_count
};

struct gguf_string_t {
	uint64_t length; // length of string, bytes
	char string[0];  // utf-8 non-null-terminated string, length bytes
};

struct gguf_array_t {
	uint32_t type;    // gguf_metadata_type_t
	uint64_t length;  // length of elements
	uint8_t array[0]; // metadata array, length elements
};

union gguf_metadata_t {
	uint8_t uint8;
	int8_t int8;
	uint16_t uint16;
	int16_t int16;
	uint32_t uint32;
	int32_t int32;
	float float32;
	uint64_t uint64;
	int64_t int64;
	double float64;
	uint8_t bool;
	gguf_string_t string;
	gguf_array_t array;
};

struct gguf_metadata_kv_t {
	gguf_string_t key;     // ascii, lower_snake_case and separated by a '.', length < 65536
	uint32_t type;         // gguf_metadata_type_t
	gguf_metadata_t value; // metadata value
};

struct gguf_header_t {
	uint32_t magic;                    // "GGUF", little-endian is 0x46554747
	uint32_t version;                  // version = 2, 3
	uint64_t n_tensor;                 // the number of tensors in the file
	uint64_t n_metadata_kv;            // the number of metadata key-value pairs
	gguf_metadata_kv_t metadata_kv[0]; // length is n_metadata_kv
};

struct gguf_tensor_info_t {
    gguf_string_t name;
    uint32_t n_dimensions;   // currently at most 4
    uint64_t dimensions[0];  // length is n_dimensions
    gguf_tensor_type_t type; // tensor's type
    uint64_t offset;         // offset MUST alignment
};

struct gguf_file_t {
	gguf_header_t header;              // header
	gguf_tensor_info_t tensor_info[0]; // length is header.n_tensor
	uint8_t padding[0];                // tensor alignment
	uint8_t tensor_data[0];            // segmentation by tensor_info
};

#endif
