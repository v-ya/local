#include "zlist.h"
#include <stdlib.h>

static void zlist_layer_cache_clear(void **restrict pc)
{
	register void *restrict p, *q;
	q = *pc;
	*pc = NULL;
	while ((p = q))
	{
		q = *(void **) p;
		free(p);
	}
}

static inline zlist_layer_t* zlist_layer_cache_alloc(void **restrict pc)
{
	register void *r;
	if ((r = *pc))
		*pc = *(void **) r;
	else r = malloc(sizeof(zlist_layer_t));
	return r;
}

static inline void zlist_layer_cache_free(void **restrict pc, zlist_layer_t *restrict v)
{
	*(void **) v = *pc;
	*pc = (void *) v;
}

static void zlist_layer_free_list(void **restrict pc, zlist_layer_t *p)
{
	zlist_layer_t *q;
	while (p->a) p = p->a;
	while ((q = p))
	{
		p = p->b;
		zlist_layer_cache_free(pc, q);
	}
}

static void zlist_list_free_list(zlist_list_t *p, zlist_list_free_f func)
{
	zlist_list_t *q;
	while (p->a) p = p->a;
	while ((q = p))
	{
		p = p->b;
		q->a = q->b = q->u = NULL;
		func(q);
	}
}

zlist_t* zlist_init(zlist_t *restrict r, uint32_t nl_max)
{
	r->layer = NULL;
	r->layer_cache = NULL;
	r->nl_max = nl_max;
	return r;
}

void zlist_uini(zlist_t *restrict r, zlist_list_free_f func)
{
	zlist_layer_t *layer, *x;
	uint32_t level;
	if ((layer = r->layer))
	{
		do {
			level = layer->level;
			x = layer->xa;
			zlist_layer_free_list(&r->layer_cache, layer);
			layer = x;
		} while (level);
		if (func)
			zlist_list_free_list((zlist_list_t *) x, func);
	}
	if (r->layer_cache)
		zlist_layer_cache_clear(&r->layer_cache);
}

zlist_t* zlist_alloc(uint32_t nl_max)
{
	zlist_t *restrict r;
	if ((r = (zlist_t *) malloc(sizeof(zlist_t))))
		return zlist_init(r, nl_max);
	return NULL;
}

void zlist_free(zlist_t *restrict r, zlist_list_free_f func)
{
	zlist_uini(r, func);
	free(r);
}

zlist_list_t* zlist_find_must(zlist_t *restrict r, int64_t a, int64_t b)
{
	zlist_layer_t *layer, *e;
	layer = r->layer;
	e = NULL;
	while (layer != e)
	{
		if (layer->na <= b && layer->nb >= a)
		{
			if (layer->level)
			{
				e = layer->xb->b;
				layer = layer->xa;
			}
			else
			{
				zlist_list_t *list, *el;
				el = ((zlist_list_t *) layer->xb)->b;
				list = (zlist_list_t *) layer->xa;
				while (list != el)
				{
					if (list->n >= a && list->n <= b)
						return list;
					list = list->b;
				}
				break;
			}
		}
		else layer = layer->b;
	}
	return NULL;
}

zlist_list_t* zlist_find_near(zlist_t *restrict r, int64_t v)
{
	zlist_layer_t *layer;
	zlist_list_t *list;
	layer = r->layer;
	while (layer)
	{
		if (layer->na > v)
		{
			do {
				if (!layer->a)
					goto label_xa;
				layer = layer->a;
			} while (layer->na > v);
			label_xb:
			if (layer->level)
				layer = layer->xb;
			else return zlist_list_find_near((zlist_list_t *) layer->xb, v);
		}
		else if (layer->nb < v)
		{
			do {
				if (!layer->b)
					goto label_xb;
				layer = layer->b;
			} while (layer->nb < v);
			label_xa:
			if (layer->level)
				layer = layer->xa;
			else return zlist_list_find_near((zlist_list_t *) layer->xa, v);
		}
		else goto label_xa;
	}
	return NULL;
}

zlist_list_t* zlist_list_find_near(zlist_list_t *restrict list, int64_t v)
{
	if (list->n > v)
	{
		do {
			if (!list->a)
				goto label_return;
			list = list->a;
		} while (list->n > v);
	}
	else if (list->n < v)
	{
		do {
			if (!list->b)
				goto label_return;
			list = list->b;
		} while (list->n < v);
	}
	label_return:
	return list;
}
