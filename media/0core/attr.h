#ifndef _media_core_attr_h_
#define _media_core_attr_h_

#include <refer.h>
#include <hashmap.h>

enum media_attr_type_t {
	media_attr_type__int,
	media_attr_type__float,
	media_attr_type__string,
	media_attr_type__data,
	media_attr_type__ptr,
};

enum media_attr_copy_t {
	media_attr_copy__replace,
	media_attr_copy__ignore,
};

union media_attr_value_t {
	int64_t av_int;
	double av_float;
	refer_string_t av_string;
	refer_nstring_t av_data;
	refer_t av_ptr;
};

struct media_attr_item_s {
	enum media_attr_type_t type;
	union media_attr_value_t value;
};

typedef const struct media_attr_s* (*media_attr_judge_f)(const struct media_attr_s *restrict attr, const struct media_attr_item_s *restrict value);

struct media_attr_judge_s {
	hashmap_t judge;
	media_attr_judge_f clear;
};

struct media_attr_s {
	hashmap_t attr;
	const struct media_attr_judge_s *judge;
	refer_t judge_inst;
};

struct media_attr_s* media_attr_alloc(const struct media_attr_s *restrict src);
struct media_attr_s* media_attr_copy(struct media_attr_s *restrict dst, const struct media_attr_s *restrict src, enum media_attr_copy_t ctype);
void media_attr_set_judge(struct media_attr_s *restrict attr, const struct media_attr_judge_s *restrict judge, refer_t judge_inst);
void media_attr_clear(struct media_attr_s *restrict attr);
void media_attr_unset(struct media_attr_s *restrict attr, const char *restrict name);
struct media_attr_s* media_attr_set(struct media_attr_s *restrict attr, const char *restrict name, enum media_attr_type_t type, union media_attr_value_t value);
const struct media_attr_s* media_attr_get(const struct media_attr_s *restrict attr, const char *restrict name, enum media_attr_type_t type, union media_attr_value_t *restrict value);

struct media_attr_judge_s* media_attr_judge_alloc(void);
struct media_attr_judge_s* media_attr_judge_set(struct media_attr_judge_s *restrict judge, const char *restrict name, media_attr_judge_f jf);

#endif
