#ifndef _miko_base_miko_info_h_
#define _miko_base_miko_info_h_

#include "miko.h"

typedef enum miko_log_type_t miko_log_type_t;
typedef struct miko_log_data_item_t miko_log_data_item_t;
typedef struct miko_log_data_inode_t miko_log_data_inode_t;
typedef union miko_log_data_t miko_log_data_t;
typedef struct miko_log_item_t miko_log_item_t;

enum miko_log_type_t {
	miko_log_type__item,
	miko_log_type__inode,
	miko_log_type_max
};

struct miko_log_data_item_t {
	refer_string_t key;
	refer_string_t value;
};

struct miko_log_data_inode_t {
	refer_string_t name;
	miko_count_t count;
};

union miko_log_data_t {
	miko_log_data_item_t item;
	miko_log_data_inode_t inode;
};

struct miko_log_item_t {
	miko_log_type_t type;
	miko_log_level_t level;
	miko_log_data_t data;
};

struct miko_log_s {
	miko_vector_s_t log_array;
	miko_strpool_s_t log_string;
	miko_log_level_t limit;
};

#endif
