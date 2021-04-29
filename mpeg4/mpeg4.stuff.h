#ifndef _mpeg4_stuff_h_
#define _mpeg4_stuff_h_

#include "mpeg4.inst.h"
#include "mpeg4.box.h"

typedef struct mpeg4_stuff_s mpeg4_stuff_t;

typedef mpeg4_stuff_t* (*mpeg4_stuff_init_f)(mpeg4_stuff_t *restrict stuff);

typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$major_brand)(mpeg4_stuff_t *restrict r, mpeg4_box_type_t major_brand);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$minor_version)(mpeg4_stuff_t *restrict r, uint32_t minor_version);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$add$compatible_brands)(mpeg4_stuff_t *restrict r, const mpeg4_box_type_t *restrict compatible_brands, uint32_t n);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$data)(mpeg4_stuff_t *restrict r, const void *data, uintptr_t size);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$version_and_flags)(mpeg4_stuff_t *restrict r, uint32_t version, uint32_t flags);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$creation_time)(mpeg4_stuff_t *restrict r, uint64_t creation_time);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$modification_time)(mpeg4_stuff_t *restrict r, uint64_t modification_time);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$timescale)(mpeg4_stuff_t *restrict r, uint32_t timescale);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$duration)(mpeg4_stuff_t *restrict r, uint64_t duration);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$rate)(mpeg4_stuff_t *restrict r, double rate);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$volume)(mpeg4_stuff_t *restrict r, double volume);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$matrix)(mpeg4_stuff_t *restrict r, double matrix[9]);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$next_track_id)(mpeg4_stuff_t *restrict r, uint32_t next_track_id);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$track_id)(mpeg4_stuff_t *restrict r, uint32_t track_id);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$track_layer)(mpeg4_stuff_t *restrict r, int16_t track_layer);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$alternate_group)(mpeg4_stuff_t *restrict r, int16_t alternate_group);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$resolution)(mpeg4_stuff_t *restrict r, double width, double height);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$language)(mpeg4_stuff_t *restrict r, const char language[3]);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$name)(mpeg4_stuff_t *restrict r, const char *restrict name);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$graphicsmode)(mpeg4_stuff_t *restrict r, uint16_t graphicsmode);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$opcolor)(mpeg4_stuff_t *restrict r, uint16_t red, uint16_t green, uint16_t blue);
typedef const mpeg4_stuff_t* (*mpeg4_stuff_func$set$balance)(mpeg4_stuff_t *restrict r, double balance);

typedef enum mpeg4_stuff_method_t {
	mpeg4_stuff_method$set$major_brand,
	mpeg4_stuff_method$set$minor_version,
	mpeg4_stuff_method$add$compatible_brands,
	mpeg4_stuff_method$set$data,
	mpeg4_stuff_method$set$version_and_flags,
	mpeg4_stuff_method$set$creation_time,
	mpeg4_stuff_method$set$modification_time,
	mpeg4_stuff_method$set$timescale,
	mpeg4_stuff_method$set$duration,
	mpeg4_stuff_method$set$rate,
	mpeg4_stuff_method$set$volume,
	mpeg4_stuff_method$set$matrix,
	mpeg4_stuff_method$set$next_track_id,
	mpeg4_stuff_method$set$track_id,
	mpeg4_stuff_method$set$track_layer,
	mpeg4_stuff_method$set$alternate_group,
	mpeg4_stuff_method$set$resolution,
	mpeg4_stuff_method$set$language,
	mpeg4_stuff_method$set$name,
	mpeg4_stuff_method$set$graphicsmode,
	mpeg4_stuff_method$set$opcolor,
	mpeg4_stuff_method$set$balance,
	mpeg4_stuff_method_max
} mpeg4_stuff_method_t;

typedef struct mpeg4_stuff_container_type_finder_t {
	mpeg4_stuff_t *same_list;
	mpeg4_stuff_t **p_same_tail;
} mpeg4_stuff_container_type_finder_t;

typedef struct mpeg4_stuff_container_t {
	rbtree_t *finder;
	mpeg4_stuff_t *list;
	mpeg4_stuff_t **p_tail;
} mpeg4_stuff_container_t;

typedef struct mpeg4_stuff_link_t {
	mpeg4_stuff_t *next;          // = *this->p_next
	mpeg4_stuff_t *same_next;     // = *this->p_same_next
	mpeg4_stuff_t **p_next;       // = &last->next
	mpeg4_stuff_t **p_same_next;  // = &same_last->same_next
	mpeg4_stuff_t *container;     // = parent
} mpeg4_stuff_link_t;

typedef struct mpeg4_stuff_box_t {
	mpeg4_box_type_t type;  // box type
	uint32_t calc_okay;     // (box size && all size) is vaild
	uint64_t inner_size;    // box inner size
	uint64_t all_size;      // box inner size + box header size
	uint64_t link_number;   // number of child box
} mpeg4_stuff_box_t;

struct mpeg4_stuff_s {
	const mpeg4_t *inst;                // refer save
	const struct mpeg4_atom_s *atom;    // only link
	mpeg4_stuff_container_t container;  // list => refer save
	mpeg4_stuff_link_t link;            // only link
	mpeg4_stuff_box_t info;
};

#define mpeg4$stuff$method$set(_atom, _name, _mid)  rbtree_insert(&(_atom)->method, NULL, mpeg4_stuff_method$##_mid, mpeg4$define(stuff, _name, _mid), NULL)
#define mpeg4$stuff$method$call(_stuff, _mid, ...)  ({register rbtree_t *restrict _rbv = rbtree_find(&(_stuff)->atom->method, NULL, mpeg4_stuff_method$##_mid);\
							(_rbv && _rbv->value)?((mpeg4_stuff_func$##_mid) _rbv->value)(_stuff, ##__VA_ARGS__):NULL;})

void mpeg4_stuff_free_default_func(mpeg4_stuff_t *restrict r);
mpeg4_stuff_t* mpeg4_stuff_alloc(const struct mpeg4_atom_s *restrict atom, const mpeg4_t *restrict inst, mpeg4_box_type_t type, size_t size, mpeg4_stuff_init_f init_func, refer_free_f free_func);
void mpeg4_stuff_calc_okay(mpeg4_stuff_t *restrict stuff, uint64_t box_inner_size);
void mpeg4_stuff_calc_invalid(mpeg4_stuff_t *restrict stuff);
mpeg4_stuff_t* mpeg4_stuff_container_link(mpeg4_stuff_t *restrict container, mpeg4_stuff_t *restrict stuff);
mpeg4_stuff_t* mpeg4_stuff_container_append(mpeg4_stuff_t *restrict container, mpeg4_box_type_t type);
mpeg4_stuff_t* mpeg4_stuff_container_find(mpeg4_stuff_t *restrict container, const mpeg4_box_type_t *restrict type);
void mpeg4_stuff_unlink(mpeg4_stuff_t *restrict stuff);

#endif
