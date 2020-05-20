#ifndef _phoneme_h_
#define _phoneme_h_

#include <refer.h>
#include <hashmap.h>
#include <json.h>
#include <dylink.h>
#include <note.h>

typedef json_inode_t *phoneme_arg_s;
typedef const char *phoneme_src_name_s;
typedef refer_t (*phoneme_arg2pri_f)(json_inode_t *arg);

typedef struct phoneme_src_t {
	phoneme_arg_s *arg;
	phoneme_src_name_s name;
} phoneme_src_t;

typedef struct phoneme_arg_pool_s {
	// 根据 json 转换成 pri
	phoneme_arg2pri_f arg2pri;
	// arg_name => refer_t json_inode_t **arg
	hashmap_t json;
	// arg_name => refer_t *pri
	hashmap_t pri;
} phoneme_arg_pool_s;

typedef struct phoneme_s {
	note_s *note;
	uint32_t details_used;
	uint32_t details_max;
	phoneme_src_t envelope;
	phoneme_src_t basefre;
	phoneme_src_t details[];
} phoneme_s;

typedef struct phoneme_pool_s {
	dylink_pool_t *dypool;  // 动态链接池
	// author.name => phoneme_src_name_s "author.name"
	// author.name 必须互不相同
	hashmap_t name;         // 名称引用池
	// sym -> path: author$type$name -> author.name => func
	// type: envelope,        basefre,               details
	// func: note_envelope_f, note_base_frequency_f, note_details_f
	hashmap_t envelope;     // 包络构造函数符号表
	hashmap_t basefre;      // 基频构造函数符号表
	hashmap_t details;      // 细节构造函数符号表
	// sym -> path: author$arg2pri$name -> author.name => phoneme_arg2pri_f
	hashmap_t arg2pri;      // 参数转换函数符号表
	// author.name => refer_t phoneme_arg_pool_s *
	hashmap_t arg;          // 构造函数参数类型二级映射表
	// phoneme_name => phoneme_s *
	hashmap_t phoneme;      // 音素表
	json_inode_t *var;      // 自定义变量树
} phoneme_pool_s;

#endif
