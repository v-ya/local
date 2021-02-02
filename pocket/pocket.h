#ifndef _pocket_h_
#define _pocket_h_

#include <stdint.h>
#include <refer.h>

// 文件分区
//    文件头
//    字符串池(utf-8)，保证相同的字符串永远被指向在同一字符串池位置，所有使用到的预设 attr.tag 字段必须被提前
//    索引区，保证 data 永远指向自身之后
//    数据区，尽量从小对齐要求的数据开始

// offset  储存模式，从字符串池中索引，从 pocket 起始计算，0 => NULL
// string  加载模式，地址指引
typedef union pocket_string_t {
	uint64_t offset;
	const char *string;
} pocket_string_t;

// offset  储存模式，从数据区/索引区(index)/字符串池(string)中索引，从 pocket 起始计算，0 => NULL
// string  加载模式，地址指引
typedef union pocket_ptr_t {
	uint64_t offset;
	void *ptr;
} pocket_ptr_t;

// 预设类型
typedef enum pocket_tag_t {
	pocket_tag$null,    // (NULL)       无类型，data = 0，size = 0
	pocket_tag$index,   // ("@index")   索引类型
	pocket_tag$string,  // ("@string")  字符串类型，utf-8，data => string pool，size = 0
	pocket_tag$text,    // ("@text")    字符串类型，utf-8，以 0 结尾
	pocket_tag$u8,      // ("@u8")      uint8_t[]
	pocket_tag$s8,      // ("@s8")      int8_t[]
	pocket_tag$u16,     // ("@u16")     uint16_t[]
	pocket_tag$s16,     // ("@s16")     int16_t[]
	pocket_tag$u32,     // ("@u32")     uint32_t[]
	pocket_tag$s32,     // ("@s32")     int32_t[]
	pocket_tag$u64,     // ("@u64")     uint64_t[]
	pocket_tag$s64,     // ("@s64")     int64_t[]
	pocket_tag$f16,     // ("@f16")     __float16[]
	pocket_tag$f32,     // ("@f32")     float[]
	pocket_tag$f64,     // ("@f64")     double[]
	pocket_tag$f128,    // ("@f128")    __float128[]
	pocket_tag$largeN,  // ("@largeN")  无符号大整数(小端序)
	pocket_tag$custom   // (?)          自定义类型，非空字符串并且不以 '@' 开头
} pocket_tag_t;

#define pocket_magic              0x0074656b636f7040ul
#define pocket_tag_string$null    NULL
#define pocket_tag_string$index   "@index"
#define pocket_tag_string$string  "@string"
#define pocket_tag_string$text    "@text"
#define pocket_tag_string$u8      "@u8"
#define pocket_tag_string$s8      "@s8"
#define pocket_tag_string$u16     "@u16"
#define pocket_tag_string$s16     "@s16"
#define pocket_tag_string$u32     "@u32"
#define pocket_tag_string$s32     "@s32"
#define pocket_tag_string$u64     "@u64"
#define pocket_tag_string$s64     "@s64"
#define pocket_tag_string$f16     "@f16"
#define pocket_tag_string$f32     "@f32"
#define pocket_tag_string$f64     "@f64"
#define pocket_tag_string$f128    "@f128"
#define pocket_tag_string$largeN  "@largeN"

// 文件头
typedef struct pocket_header_t {
	uint64_t magic;               // 文件标识 "@pocket"
	pocket_string_t verify;       // 文件校验方式 (NULL 为无校验)，校验参数和校验结果将会从系统区查找
	uint64_t string$offset;       // 字符串池-起始位置，16 字节对齐
	uint64_t string$size;         // 字符串池-大小
	uint64_t index$offset;        // 索引区-起始位置，16 字节对齐
	uint64_t index$size;          // 索引区-大小
	uint64_t data$offset;         // 数据区-起始位置，16 字节对齐
	uint64_t data$size;           // 数据区-大小
	pocket_ptr_t system;          // 系统根索引位置
	pocket_ptr_t user;            // 用户根索引位置
	pocket_string_t tag;          // 该 pocket 的类型
	pocket_string_t version;      // 该 pocket 的版本
	pocket_string_t author;       // 该 pocket 的作者
	pocket_string_t time;         // 该 pocket 的创建时间 "yyyy-MM-dd hh:mm:ss"
	pocket_string_t description;  // 该 pocket 的描述，系统跟索引的子节点名称，索引 tag 必须为 text
	pocket_string_t flag;         // 随便贴点啥都行，只要不太长......
} pocket_header_t;

// 被索引数据的属性
typedef struct pocket_attr_t {
	pocket_string_t name;  // 名称
	pocket_string_t tag;   // 类型
	pocket_ptr_t data;     // 实体
	uint64_t size;         // 大小
} pocket_attr_t;

typedef struct pocket_s pocket_s;

struct pocket_verify_s;

pocket_s* pocket_alloc(uint8_t *restrict pocket, uint64_t size, const struct pocket_verify_s *restrict verify);
pocket_s* pocket_load(const char *restrict path, const struct pocket_verify_s *restrict verify);
uint8_t* pocket_pull(const pocket_s *restrict p, uint64_t *restrict size);
void pocket_pull_free(uint8_t *restrict pocket);

const pocket_header_t* pocket_header(const pocket_s *restrict p);
const pocket_attr_t* pocket_system(const pocket_s *restrict p);
const pocket_attr_t* pocket_user(const pocket_s *restrict p);
pocket_tag_t pocket_preset_tag(const pocket_s *restrict p, const pocket_attr_t *restrict attr);
const pocket_attr_t* pocket_is_tag(const pocket_s *restrict p, const pocket_attr_t *restrict attr, pocket_tag_t tag, const char *restrict custom);
const pocket_attr_t* pocket_find(const pocket_s *restrict p, const pocket_attr_t *restrict index, const char *restrict name);
const pocket_attr_t* pocket_find_path(const pocket_s *restrict p, const pocket_attr_t *restrict index, const char *const restrict *restrict path);
const pocket_attr_t* pocket_find_tag(const pocket_s *restrict p, const pocket_attr_t *restrict index, const char *restrict name, pocket_tag_t tag, const char *restrict custom);
const pocket_attr_t* pocket_find_path_tag(const pocket_s *restrict p, const pocket_attr_t *restrict index, const char *const restrict *restrict path, pocket_tag_t tag, const char *restrict custom);

const struct pocket_verify_s* pocket_check_verify(const struct pocket_verify_s *restrict verify, const uint8_t *restrict pocket, uint64_t size);
const struct pocket_verify_s* pocket_build_verify(const struct pocket_verify_s *restrict verify, uint8_t *restrict pocket, uint64_t size);

#endif
