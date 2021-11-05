#include "tmapping.h"
#include <stdlib.h>
#include <string.h>

// type

typedef struct tparse_tmapping_single_s tparse_tmapping_single_s;
typedef struct tparse_tmapping_multi_s tparse_tmapping_multi_s;
typedef struct tparse_tmapping_mixing_s tparse_tmapping_mixing_s;

typedef struct tparse_tmapping_single_t tparse_tmapping_single_t;
typedef struct tparse_tmapping_multi_t tparse_tmapping_multi_t;

typedef struct tparse_tmapping_multi_route_t {
	uint8_t route[256];
} tparse_tmapping_multi_route_t;

struct tparse_tmapping_single_t {
	refer_t mapping[256];
};

struct tparse_tmapping_multi_t {
	tparse_tmapping_multi_t *next;
	tparse_tmapping_multi_route_t *route;
	uintptr_t length;
	uintptr_t pos;
	refer_t value;
};

struct tparse_tmapping_single_s {
	tparse_tmapping_s tmapping;
	tparse_tmapping_single_t single;
};

struct tparse_tmapping_multi_s {
	tparse_tmapping_s tmapping;
	tparse_tmapping_multi_t *multi;
	tparse_tmapping_multi_t **p_multi_tail;
};

struct tparse_tmapping_mixing_s {
	tparse_tmapping_s tmapping;
	tparse_tmapping_single_t single;
	tparse_tmapping_multi_t *multi;
	tparse_tmapping_multi_t **p_multi_tail;
};

// single

static void tparse_tmapping_inner_single_set(tparse_tmapping_single_t *restrict r, uint8_t name, refer_t value)
{
	if (r->mapping[name])
		refer_free(r->mapping[name]);
	r->mapping[name] = refer_save(value);
}

static void tparse_tmapping_inner_single_clear(tparse_tmapping_single_t *restrict r)
{
	for (uintptr_t i = 0; i < 256; ++i)
	{
		if (r->mapping[i])
		{
			refer_free(r->mapping[i]);
			r->mapping[i] = NULL;
		}
	}
}

static inline refer_t tparse_tmapping_inner_single_test(const tparse_tmapping_single_t *restrict r, char c)
{
	return r->mapping[(uint8_t) c];
}

// multi

static void tparse_tmapping_inner_multi_build_route(tparse_tmapping_multi_route_t *restrict route, const uint8_t *restrict name, uintptr_t n)
{
	uintptr_t i, pl;
	memset(route, 0, sizeof(tparse_tmapping_multi_route_t) * n);
	for (i = 0; i < n; ++i)
	{
		route[i].route[*name] = 1;
		for (pl = 1; pl < i; ++pl)
		{
			if (!memcmp(name, name + i - pl, pl))
				route[i].route[name[pl]] = pl + 1;
		}
		route[i].route[name[i]] = i + 1;
	}
}

static tparse_tmapping_multi_t* tparse_tmapping_inner_multi_alloc(const char *restrict name, refer_t value)
{
	tparse_tmapping_multi_t *restrict r;
	uintptr_t n;
	if (name && (n = strlen(name)) && n < 256)
	{
		r = (tparse_tmapping_multi_t *) malloc(sizeof(tparse_tmapping_multi_t) + n * sizeof(tparse_tmapping_multi_route_t));
		if (r)
		{
			r->next = NULL;
			r->length = n;
			r->route = (tparse_tmapping_multi_route_t *) (r + 1);
			r->pos = 0;
			r->value = refer_save(value);
			tparse_tmapping_inner_multi_build_route(r->route, (const uint8_t *) name, n);
			return r;
		}
	}
	return NULL;
}

static void tparse_tmapping_inner_multi_free(tparse_tmapping_multi_t *restrict r)
{
	if (r->value)
		refer_free(r->value);
	free(r);
}

static inline void tparse_tmapping_inner_multi_list_append(tparse_tmapping_multi_t ***restrict pp_list_tail, tparse_tmapping_multi_t *item)
{
	**pp_list_tail = item;
	*pp_list_tail = &item->next;
}

static void tparse_tmapping_inner_multi_list_free(tparse_tmapping_multi_t *restrict list)
{
	tparse_tmapping_multi_t *p;
	while ((p = list))
	{
		list = list->next;
		tparse_tmapping_inner_multi_free(p);
	}
}

static void tparse_tmapping_inner_multi_list_clear(tparse_tmapping_multi_t *restrict list)
{
	while (list)
	{
		list->pos = 0;
		list = list->next;
	}
}

static inline refer_t tparse_tmapping_inner_multi_test(tparse_tmapping_multi_t *restrict r, char c)
{
	if ((r->pos = (uintptr_t) r->route[r->pos].route[(uint8_t) c]) == r->length)
	{
		r->pos = 0;
		return r->value;
	}
	return NULL;
}

static refer_t tparse_tmapping_inner_multi_list_test(tparse_tmapping_multi_t *restrict list, char c)
{
	refer_t rv, v;
	rv = NULL;
	while (list)
	{
		if ((v = tparse_tmapping_inner_multi_test(list, c)) && !rv)
			rv = v;
		list = list->next;
	}
	return rv;
}

// api

static void tparse_tmapping_single_free_func(tparse_tmapping_single_s *restrict r)
{
	tparse_tmapping_inner_single_clear(&r->single);
}

static void tparse_tmapping_multi_free_func(tparse_tmapping_multi_s *restrict r)
{
	tparse_tmapping_inner_multi_list_free(r->multi);
}

static void tparse_tmapping_free_func(tparse_tmapping_mixing_s *restrict r)
{
	tparse_tmapping_inner_single_clear(&r->single);
	tparse_tmapping_inner_multi_list_free(r->multi);
}

static tparse_tmapping_single_s* tparse_tmapping_add_single(tparse_tmapping_single_s *restrict mp, const char *restrict name, refer_t value)
{
	if (name && (!name[0] || !name[1]))
	{
		tparse_tmapping_inner_single_set(&mp->single, *(const uint8_t *) name, value);
		return mp;
	}
	return NULL;
}

static tparse_tmapping_multi_s* tparse_tmapping_add_multi(tparse_tmapping_multi_s *restrict mp, const char *restrict name, refer_t value)
{
	tparse_tmapping_multi_t *restrict item;
	if ((item = tparse_tmapping_inner_multi_alloc(name, value)))
	{
		tparse_tmapping_inner_multi_list_append(&mp->p_multi_tail, item);
		return mp;
	}
	return NULL;
}

static tparse_tmapping_mixing_s* tparse_tmapping_add_mixing(tparse_tmapping_mixing_s *restrict mp, const char *restrict name, refer_t value)
{
	tparse_tmapping_multi_t *restrict item;
	if (name)
	{
		if (!name[0] || !name[1])
			tparse_tmapping_inner_single_set(&mp->single, (uint8_t) *name, value);
		else if ((item = tparse_tmapping_inner_multi_alloc(name, value)))
			tparse_tmapping_inner_multi_list_append(&mp->p_multi_tail, item);
		else goto label_fail;
		return mp;
	}
	label_fail:
	return NULL;
}

static refer_t tparse_tmapping_test_single(tparse_tmapping_single_s *restrict mp, char c)
{
	return tparse_tmapping_inner_single_test(&mp->single, c);
}

static refer_t tparse_tmapping_test_multi(tparse_tmapping_multi_s *restrict mp, char c)
{
	return tparse_tmapping_inner_multi_list_test(mp->multi, c);
}

static refer_t tparse_tmapping_test_mixing(tparse_tmapping_mixing_s *restrict mp, char c)
{
	refer_t s, m;
	s = tparse_tmapping_inner_single_test(&mp->single, c);
	m = mp->multi?tparse_tmapping_inner_multi_list_test(mp->multi, c):NULL;
	return s?s:m;
}

static void tparse_tmapping_clear_single(tparse_tmapping_single_s *restrict mp)
{
	return ;
}

static void tparse_tmapping_clear_multi(tparse_tmapping_multi_s *restrict mp)
{
	tparse_tmapping_inner_multi_list_clear(mp->multi);
}

static void tparse_tmapping_clear_mixing(tparse_tmapping_mixing_s *restrict mp)
{
	tparse_tmapping_inner_multi_list_clear(mp->multi);
}

#define d_tmapping(_t)  r->tmapping.add = (tparse_tmapping_add_f) tparse_tmapping_add_##_t;\
			r->tmapping.test = (tparse_tmapping_test_f) tparse_tmapping_test_##_t;\
			r->tmapping.clear = (tparse_tmapping_clear_f) tparse_tmapping_clear_##_t

tparse_tmapping_s* tparse_tmapping_alloc_single(void)
{
	tparse_tmapping_single_s *restrict r;
	r = (tparse_tmapping_single_s *) refer_alloz(sizeof(tparse_tmapping_single_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) tparse_tmapping_single_free_func);
		d_tmapping(single);
	}
	return &r->tmapping;
}

tparse_tmapping_s* tparse_tmapping_alloc_multi(void)
{
	tparse_tmapping_multi_s *restrict r;
	r = (tparse_tmapping_multi_s *) refer_alloc(sizeof(tparse_tmapping_multi_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) tparse_tmapping_multi_free_func);
		r->multi = NULL;
		r->p_multi_tail = &r->multi;
		d_tmapping(multi);
	}
	return &r->tmapping;
}

tparse_tmapping_s* tparse_tmapping_alloc_mixing(void)
{
	tparse_tmapping_mixing_s *restrict r;
	r = (tparse_tmapping_mixing_s *) refer_alloz(sizeof(tparse_tmapping_mixing_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) tparse_tmapping_free_func);
		r->multi = NULL;
		r->p_multi_tail = &r->multi;
		d_tmapping(mixing);
	}
	return &r->tmapping;
}

#undef d_tmapping
