#ifndef _media_core_attr_h_
#define _media_core_attr_h_

#include <refer.h>
#include <hashmap.h>
#include <vattr.h>

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

enum media_attr_judge_need_t {
	media_attr_judge_need__any,
	media_attr_judge_need__int,
	media_attr_judge_need__float,
	media_attr_judge_need__string,
	media_attr_judge_need__data,
	media_attr_judge_need__ptr,
	media_attr_judge_need__uint32,
	media_attr_judge_need__sint32,
	media_attr_judge_need__uint16,
	media_attr_judge_need__sint16,
	media_attr_judge_need__uint8,
	media_attr_judge_need__sint8,
	media_attr_judge_need__exist_string,
	media_attr_judge_need__exist_data,
	media_attr_judge_need__exist_ptr,
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

typedef const struct media_attr_s* (*media_attr_set_f)(const struct media_attr_s *restrict attr, refer_t pri, const struct media_attr_item_s *restrict value);
typedef void (*media_attr_unset_f)(const struct media_attr_s *restrict attr, refer_t pri);

struct media_attr_judge_item_s {
	media_attr_set_f set;
	media_attr_unset_f unset;
	enum media_attr_judge_need_t need;
};

struct media_attr_judge_s {
	hashmap_t judge;
	media_attr_unset_f clear;
};

struct media_attr_s {
	vattr_s *attr;
	const struct media_attr_judge_s *judge;
	refer_t pri_data;
};

struct media_attr_s* media_attr_alloc(const struct media_attr_s *restrict src);
struct media_attr_s* media_attr_copy(struct media_attr_s *restrict dst, const struct media_attr_s *restrict src, enum media_attr_copy_t ctype);
void media_attr_set_judge(struct media_attr_s *restrict attr, const struct media_attr_judge_s *restrict judge, refer_t pri_data);
void media_attr_clear(struct media_attr_s *restrict attr);
void media_attr_unset(struct media_attr_s *restrict attr, const char *restrict name);
struct media_attr_s* media_attr_set(struct media_attr_s *restrict attr, const char *restrict name, enum media_attr_type_t type, union media_attr_value_t value);
const struct media_attr_s* media_attr_get(const struct media_attr_s *restrict attr, const char *restrict name, enum media_attr_type_t type, union media_attr_value_t *restrict value);

struct media_attr_judge_s* media_attr_judge_alloc(void);
struct media_attr_judge_s* media_attr_judge_set(struct media_attr_judge_s *restrict judge, const char *restrict name, media_attr_set_f sf, media_attr_unset_f uf, enum media_attr_judge_need_t need);
void media_attr_judge_set_extra_clear(struct media_attr_judge_s *restrict judge, media_attr_unset_f uf);

#define media_attr_symbol(_tf, _type, _name, _func)   media_attr__##_tf##__##_type##__##_name##__##_func
#define d_media_attr_set(_type, _name, _func, _pt)    const struct media_attr_s* media_attr_symbol(set, _type, _name, _func)(const struct media_attr_s *restrict attr, _pt *restrict pri, const struct media_attr_item_s *restrict value)
#define d_media_attr_unset(_type, _name, _func, _pt)  void media_attr_symbol(unset, _type, _name, _func)(const struct media_attr_s *restrict attr, _pt *restrict pri)

#define d_media_attr_judge_add(_judge, _type, _name, _func, _qn, _need) media_attr_judge_set(_judge, _qn, (media_attr_set_f) media_attr_symbol(set, _type, _name, _func), (media_attr_unset_f) media_attr_symbol(unset, _type, _name, _func), media_attr_judge_need__##_need)

#endif
