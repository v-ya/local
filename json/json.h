#ifndef _json_h_
#define _json_h_

#include <stdint.h>
#include <hashmap.h>

typedef uintptr_t json_inode_type_t;
enum json_inode_type_e {
	json_inode_null,
	json_inode_boolean,
	json_inode_integer,
	json_inode_floating,
	json_inode_string,
	json_inode_array,
	json_inode_object
};

typedef union json_inode_value_t {
	uint64_t boolean;
	int64_t integer;
	double floating;
	char string[0];
	hashmap_t array;
	hashmap_t object;
} json_inode_value_t;

typedef struct json_inode_t {
	json_inode_type_t type;
	json_inode_value_t value;
} json_inode_t;

// array 类型的索引号必须从零开始并且连续增加

void json_free(json_inode_t *restrict json);
const char* json_decode(const char *restrict s, json_inode_t **restrict pji);
uint64_t json_length(json_inode_t *restrict ji);
char* json_encode_intext(json_inode_t *restrict ji, char *restrict s);
char* json_encode(json_inode_t *restrict ji, char *restrict s);
json_inode_t* json_create_null(void);
json_inode_t* json_create_boolean(uint64_t boolean);
json_inode_t* json_create_integer(int64_t integer);
json_inode_t* json_create_floating(double floating);
json_inode_t* json_create_string(const char *restrict string);
json_inode_t* json_create_array(void);
json_inode_t* json_create_object(void);
json_inode_t* json_array_find(json_inode_t *restrict ji, uint64_t index);
json_inode_t* json_object_find(json_inode_t *restrict ji, const char *restrict key);
json_inode_t* json_array_get(json_inode_t *restrict ji, uint64_t index);
json_inode_t* json_object_get(json_inode_t *restrict ji, const char *restrict key);
json_inode_t* json_array_set(json_inode_t *restrict ji, uint64_t index, json_inode_t *restrict value);
json_inode_t* json_object_set(json_inode_t *restrict ji, const char *restrict key, json_inode_t *restrict value);
void json_array_delete(json_inode_t *restrict ji, uint64_t index);
void json_object_delete(json_inode_t *restrict ji, const char *restrict key);
json_inode_t* json_load(const char *restrict path);
json_inode_t* json_find(json_inode_t *restrict ji, const char *restrict jpath);
json_inode_t* json_set(json_inode_t *restrict *restrict pji, const char *restrict jpath, json_inode_t *restrict value);
json_inode_t* json_get_null(json_inode_t *restrict ji, const char *restrict jpath);
json_inode_t* json_get_boolean(json_inode_t *restrict ji, const char *restrict jpath, uint64_t *restrict v);
json_inode_t* json_get_integer(json_inode_t *restrict ji, const char *restrict jpath, int64_t *restrict v);
json_inode_t* json_get_floating(json_inode_t *restrict ji, const char *restrict jpath, double *restrict v);
json_inode_t* json_get_string(json_inode_t *restrict ji, const char *restrict jpath, const char **restrict v);
json_inode_t* json_get_array(json_inode_t *restrict ji, const char *restrict jpath, hashmap_t **restrict v);
json_inode_t* json_get_object(json_inode_t *restrict ji, const char *restrict jpath, hashmap_t **restrict v);
json_inode_t* json_copy(json_inode_t *restrict ji);

#endif
