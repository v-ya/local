#ifndef _phoneme_pool_h_
#define _phoneme_pool_h_

#include "phoneme_type.h"
#include "phoneme_arg.h"
#include <dylink.h>

typedef struct phoneme_pool_s {
	dylink_pool_t *dypool;  // 动态链接池
	// author.package => path
	hashmap_t depend;       // 已加载依赖表
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

phoneme_pool_s* phoneme_pool_alloc(dylink_pool_t *dypool);
int phoneme_pool_test_package(phoneme_pool_s *restrict pp, const char *restrict package);
int phoneme_pool_load_package(phoneme_pool_s *restrict pp, const char *restrict package, const char *restrict path);
phoneme_src_name_s phoneme_pool_get_name(phoneme_pool_s *restrict pp, const char *restrict type);
json_inode_t* phoneme_pool_set_var(phoneme_pool_s *restrict pp, const char *restrict jpath, json_inode_t *restrict value);
json_inode_t* phoneme_pool_get_var(phoneme_pool_s *restrict pp, const char *restrict jpath);
phoneme_arg_pool_s* phoneme_pool_get_arg_pool(phoneme_pool_s *restrict pp, const char *restrict type);
phoneme_arg_pool_s* phoneme_pool_set_arg_pool(phoneme_pool_s *restrict pp, const char *restrict type, const char *restrict arg2pri, const char *restrict refer);
phoneme_arg_s* phoneme_pool_set_arg(phoneme_pool_s *restrict pp, const char *restrict type, const char *restrict name, phoneme_arg_s *arg);
phoneme_arg_s* phoneme_pool_set_arg_json(phoneme_pool_s *restrict pp, const char *restrict type, const char *restrict name, json_inode_t *restrict value);
phoneme_arg_s* phoneme_pool_get_arg(phoneme_pool_s *restrict pp, const char *restrict type, const char *restrict name);
refer_t phoneme_pool_get_pri(phoneme_pool_s *restrict pp, const char *restrict type, const char *restrict name);
phoneme_s* phoneme_pool_set_phoneme(phoneme_pool_s *restrict pp, const char *restrict phname, phoneme_s *restrict p);
phoneme_s* phoneme_pool_set_phoneme_json(phoneme_pool_s *restrict pp, const char *restrict phname, json_inode_t *restrict value, uint32_t sdmax, uint32_t dmax);

#endif
