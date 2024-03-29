#ifndef _zlist_h_
#define _zlist_h_

#include <stdint.h>

typedef struct zlist_t zlist_t;
typedef struct zlist_layer_t zlist_layer_t;
typedef struct zlist_list_t zlist_list_t;

typedef void (*zlist_list_free_f)(zlist_list_t *restrict list, void *pri);

struct zlist_t {
	zlist_layer_t *layer;
	void *layer_cache;
	uint32_t nl_max;
};

struct zlist_layer_t {
	zlist_layer_t *a;
	zlist_layer_t *b;
	zlist_layer_t *u;
	zlist_layer_t *xa;
	zlist_layer_t *xb;
	uint32_t level;
	uint32_t nl;
	int64_t na;
	int64_t nb;
};

struct zlist_list_t {
	zlist_list_t *a;
	zlist_list_t *b;
	zlist_layer_t *u;
	int64_t n;
};

zlist_t* zlist_init(zlist_t *restrict r, uint32_t nl_max);
void zlist_uini(zlist_t *restrict r, zlist_list_free_f func, void *pri);

zlist_t* zlist_alloc(uint32_t nl_max);
void zlist_free(zlist_t *restrict r, zlist_list_free_f func, void *pri);

zlist_list_t* zlist_insert(zlist_t *restrict r, zlist_list_t *restrict v, zlist_list_t *restrict near);
zlist_list_t* zlist_remove(zlist_t *restrict r, zlist_list_t *restrict v);

zlist_list_t* zlist_find_a(zlist_t *restrict r);
zlist_list_t* zlist_find_b(zlist_t *restrict r);
zlist_list_t* zlist_find_must(zlist_t *restrict r, int64_t a, int64_t b);
zlist_list_t* zlist_find_near(zlist_t *restrict r, int64_t v);
zlist_list_t* zlist_list_find_near(zlist_list_t *restrict list, int64_t v);

#endif
