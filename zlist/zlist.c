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

static void zlist_list_free_list(zlist_list_t *p, zlist_list_free_f func, void *pri)
{
	zlist_list_t *q;
	while (p->a) p = p->a;
	while ((q = p))
	{
		p = p->b;
		q->a = q->b = NULL;
		q->u = NULL;
		func(q, pri);
	}
}

zlist_t* zlist_init(zlist_t *restrict r, uint32_t nl_max)
{
	if (nl_max < 4)
		nl_max = 4;
	r->layer = NULL;
	r->layer_cache = NULL;
	r->nl_max = nl_max;
	return r;
}

void zlist_uini(zlist_t *restrict r, zlist_list_free_f func, void *pri)
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
			zlist_list_free_list((zlist_list_t *) x, func, pri);
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

void zlist_free(zlist_t *restrict r, zlist_list_free_f func, void *pri)
{
	zlist_uini(r, func, pri);
	free(r);
}

static void zlist_layer_fix_range_a(zlist_layer_t *restrict layer, int64_t n)
{
	zlist_layer_t *restrict u;
	layer->na = n;
	while ((u = layer->u) && layer == u->xa)
		(layer = u)->na = n;
}

static void zlist_layer_fix_range_b(zlist_layer_t *restrict layer, int64_t n)
{
	zlist_layer_t *restrict u;
	layer->nb = n;
	while ((u = layer->u) && layer == u->xb)
		(layer = u)->nb = n;
}

static zlist_layer_t* zlist_layer_copy_ab(zlist_t *restrict r, zlist_layer_t *restrict a)
{
	zlist_layer_t *restrict b;
	if ((b = zlist_layer_cache_alloc(&r->layer_cache)))
	{
		b->a = a;
		if ((b->b = a->b))
			b->b->a = b;
		a->b = b;
		b->u = a->u;
		b->xa = a->xa;
		b->xb = a->xb;
		b->level = a->level;
		b->nl = a->nl;
		b->na = a->na;
		b->nb = a->nb;
	}
	return b;
}

static zlist_layer_t* zlist_layer_upper_ab(zlist_t *restrict r, zlist_layer_t *restrict a)
{
	zlist_layer_t *restrict u, *restrict b;
	if ((u = zlist_layer_cache_alloc(&r->layer_cache)))
	{
		b = a->b;
		u->a = u->b = u->u = NULL;
		u->xa = a;
		u->xb = b;
		u->level = a->level + 1;
		u->nl = 2;
		u->na = a->na;
		u->nb = b->nb;
		a->u = u;
		b->u = u;
		r->layer = u;
	}
	return u;
}

static void zlist_layer0_copy_reset_x(zlist_layer_t *restrict a, zlist_layer_t *restrict b, uint32_t n)
{
	zlist_list_t *restrict p;
	a->nl = n;
	b->nl -= n;
	p = (zlist_list_t *) a->xa;
	for (--n; n; --n)
		p = p->b;
	a->xb = (zlist_layer_t *) p;
	a->nb = p->n;
	p = p->b;
	b->xa = (zlist_layer_t *) p;
	b->na = p->n;
	for (n = b->nl; n; p = p->b, --n)
		p->u = b;
}

static void zlist_layer_copy_reset_x(zlist_layer_t *restrict a, zlist_layer_t *restrict b, uint32_t n)
{
	zlist_layer_t *restrict p;
	a->nl = n;
	b->nl -= n;
	p = a->xa;
	for (--n; n; --n)
		p = p->b;
	a->xb = p;
	a->nb = p->nb;
	p = p->b;
	b->xa = p;
	b->na = p->na;
	for (n = b->nl; n; p = p->b, --n)
		p->u = b;
}

static void zlist_layer_fusion_reset_u_b2a(zlist_layer_t *restrict ub, zlist_layer_t *restrict ua)
{
	if (ub->level)
	{
		zlist_layer_t *restrict a, *restrict b;
		a = ub->xa;
		b = ub->xb;
		goto label_entry_layer;
		do {
			a = a->b;
			label_entry_layer:
			a->u = ua;
		} while (a != b);
	}
	else
	{
		zlist_list_t *restrict a, *restrict b;
		a = (zlist_list_t *) ub->xa;
		b = (zlist_list_t *) ub->xb;
		goto label_entry_list;
		do {
			a = a->b;
			label_entry_list:
			a->u = ua;
		} while (a != b);
	}
}

static inline zlist_layer_t* zlist_layer_fusion_ab_rbu(zlist_t *restrict r, zlist_layer_t *restrict a)
{
	zlist_layer_t *restrict b, *restrict u;
	b = a->b;
	if ((a->b = b->b))
		a->b->a = a;
	a->xb = b->xb;
	a->nb = b->nb;
	a->nl += b->nl;
	zlist_layer_fusion_reset_u_b2a(b, a);
	if ((u = b->u) != a->u)
	{
		zlist_layer_fix_range_b(a->u, a->nb);
		zlist_layer_fix_range_a(u, (u->xa = b->b)->na);
		--u->nl;
	}
	else if (u)
	{
		if (u->xb == b)
			u->xb = a;
		--u->nl;
	}
	zlist_layer_cache_free(&r->layer_cache, b);
	return u;
}

static inline void zlist_layer_set_root(zlist_t *restrict r, zlist_layer_t *restrict root)
{
	zlist_layer_t *restrict u;
	r->layer = root;
	u = root->u;
	root->u = NULL;
	while ((root = u))
	{
		u = root->u;
		zlist_layer_cache_free(&r->layer_cache, root);
	}
}

static void zlist_layer_fusion(zlist_t *restrict r, zlist_layer_t *restrict a);

static zlist_layer_t* zlist_layer_division(zlist_t *restrict r, zlist_layer_t *restrict layer0)
{
	zlist_layer_t *restrict layer1;
	for (;;)
	{
		if ((layer1 = zlist_layer_copy_ab(r, layer0)))
		{
			if (layer0->level) zlist_layer_copy_reset_x(layer0, layer1, layer0->nl >> 1);
			else zlist_layer0_copy_reset_x(layer0, layer1, layer0->nl >> 1);
			if ((layer1 = layer0->u))
			{
				if (layer1->xb == layer0)
					layer1->xb = layer0->b;
				if (++layer1->nl <= r->nl_max)
				{
					label_okay:
					return layer0;
				}
				layer0 = layer1;
				continue;
			}
			else
			{
				if (zlist_layer_upper_ab(r, layer0))
					goto label_okay;
				zlist_layer_fusion(r, layer0);
			}
		}
		return NULL;
	}
}

static void zlist_layer_fusion(zlist_t *restrict r, zlist_layer_t *restrict a)
{
	zlist_layer_t *restrict u;
	for (;;)
	{
		u = zlist_layer_fusion_ab_rbu(r, a);
		if (a->nl > r->nl_max)
			zlist_layer_division(r, a);
		if (u->nl <= 1)
		{
			if ((a = u->a))
			{
				if (a->nl >= r->nl_max && u->b)
					goto label_fusion_b;
				continue;
			}
			else if (u->b)
			{
				label_fusion_b:
				a = u;
				continue;
			}
			else zlist_layer_set_root(r, u);
		}
		return ;
	}
}

static inline zlist_list_t* zlist_list_x_layer0(zlist_t *restrict r, zlist_list_t *restrict v, zlist_layer_t *restrict ua, zlist_layer_t *restrict ub)
{
	if (!ub)
	{
		// a
		ua->xb = (zlist_layer_t *) v;
		zlist_layer_fix_range_b(ua, v->n);
	}
	else if (!ua)
	{
		// b
		ub->xa = (zlist_layer_t *) v;
		zlist_layer_fix_range_a(ua = ub, v->n);
	}
	// else inner
	v->u = ua;
	if (++ua->nl <= r->nl_max || zlist_layer_division(r, ua))
		return v;
	return NULL;
}

static zlist_list_t* zlist_list_insert_a(zlist_t *restrict r, zlist_list_t *restrict v, zlist_list_t *restrict a)
{
	zlist_list_t *restrict b;
	zlist_layer_t *restrict ua, *restrict ub;
	ua = a->u;
	ub = NULL;
	v->a = a;
	if ((v->b = b = a->b))
	{
		b->a = v;
		if ((ub = b->u)->nl < ua->nl)
			ua = NULL;
		else if (ua != ub)
			ub = NULL;
	}
	a->b = v;
	return zlist_list_x_layer0(r, v, ua, ub);
}

static zlist_list_t* zlist_list_insert_b(zlist_t *restrict r, zlist_list_t *restrict v, zlist_list_t *restrict b)
{
	zlist_list_t *restrict a;
	zlist_layer_t *restrict ua, *restrict ub;
	ua = NULL;
	ub = b->u;
	v->b = b;
	if ((v->a = a = b->a))
	{
		a->b = v;
		if ((ua = a->u)->nl < ub->nl)
			ub = NULL;
		else if (ua != ub)
			ua = NULL;
	}
	b->a = v;
	return zlist_list_x_layer0(r, v, ua, ub);
}

static zlist_list_t* zlist_list_insert_only(zlist_t *restrict r, zlist_list_t *restrict v)
{
	zlist_layer_t *restrict u;
	v->a = v->b = NULL;
	v->u = NULL;
	if ((u = zlist_layer_cache_alloc(&r->layer_cache)))
	{
		u->a = u->b = u->u = NULL;
		u->xa = (zlist_layer_t *) v;
		u->xb = (zlist_layer_t *) v;
		u->level = 0;
		u->nl = 1;
		u->na = v->n;
		u->nb = v->n;
		v->u = u;
		r->layer = u;
		return v;
	}
	return NULL;
}

static void zlist_layer0_remove(zlist_t *restrict r, zlist_layer_t *restrict layer0, zlist_list_t *restrict v)
{
	if (layer0->nl > 1)
	{
		if (layer0->xa == (zlist_layer_t *) v)
		{
			layer0->xa = (zlist_layer_t *) v->b;
			zlist_layer_fix_range_a(layer0, v->b->n);
		}
		if (layer0->xb == (zlist_layer_t *) v)
		{
			layer0->xb = (zlist_layer_t *) v->a;
			zlist_layer_fix_range_b(layer0, v->a->n);
		}
		if (--layer0->nl <= 1)
		{
			if (layer0->a)
				zlist_layer_fusion(r, layer0->a);
			else if (layer0->b)
				zlist_layer_fusion(r, layer0);
			else zlist_layer_set_root(r, layer0);
		}
	}
	else
	{
		r->layer = NULL;
		zlist_layer_cache_free(&r->layer_cache, layer0);
	}
}

zlist_list_t* zlist_insert(zlist_t *restrict r, zlist_list_t *restrict v, zlist_list_t *restrict near)
{
	if (near) near = zlist_list_find_near(near, v->n);
	else near = zlist_find_near(r, v->n);
	if (near)
	{
		if (v->n >= near->n)
		{
			if (!zlist_list_insert_a(r, v, near))
				goto label_fail;
		}
		else
		{
			if (!zlist_list_insert_b(r, v, near))
				goto label_fail;
		}
	}
	else
	{
		if (!zlist_list_insert_only(r, v))
			goto label_fail_not_remove;
	}
	return v;
	label_fail:
	zlist_remove(r, v);
	label_fail_not_remove:
	return NULL;
}

zlist_list_t* zlist_remove(zlist_t *restrict r, zlist_list_t *restrict v)
{
	zlist_list_t *restrict a, *restrict b;
	if (v->u)
	{
		a = v->a;
		b = v->b;
		if (a) a->b = b;
		if (b) b->a = a;
		zlist_layer0_remove(r, v->u, v);
		v->a = v->b = NULL;
		v->u = NULL;
	}
	return v;
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
