#include "rbtree.h"
#include <stdlib.h>
#include <string.h>

static rbtree_t* rbtree_balance_insert(rbtree_t *restrict vn, rbtree_t *restrict *restrict pr)
{
	register rbtree_t *v, *p, *pp, *pf;
	v = vn;
	while (v->u && (p = v->u)->color == rbtree_color_red)
	{
		pp = p->u;
		if (pp->l == p)
		{
			// left
			pf = pp->r;
			if (pf && pf->color == rbtree_color_red) goto label_clear_color;
			else if (p->r == v)
			{
				// rotate left
				//        p:R                         v:R       //
				// vf:B         v:R     =>      p:R         2:B //
				//          1:B     2:B    vf:B     1:B         //
				v->u = pp;
				pp->l = v;
				if ((p->r = pf = v->l)) pf->u = p;
				v->l = p;
				p->u = v;
				v = p;
				continue;
			}
			else
			{
				// rotate right
				//           pp:B                 p:B             //
				//     p:R          pf:B => v:R         pp:R      //
				// v:R     vf:B                    vf:B      pf:B //
				if ((p->u = pf = pp->u))
				{
					if (pf->l == pp) pf->l = p;
					else pf->r = p;
				}
				else *pr = p;
				if ((pp->l = pf = p->r)) pf->u = pp;
				p->r = pp;
				pp->u = p;
				p->color = rbtree_color_black;
				pp->color = rbtree_color_red;
				continue;
			}
		}
		else
		{
			// right
			pf = pp->l;
			if (pf && pf->color == rbtree_color_red) goto label_clear_color;
			else if (p->l == v)
			{
				// rotate right
				//           p:R                 v:R            //
				//     v:R         vf:B => 1:B         p:R      //
				// 1:B     2:B                     2:B     vf:B //
				v->u = pp;
				pp->r = v;
				if ((p->l = pf = v->r)) pf->u = p;
				v->r = p;
				p->u = v;
				v = p;
				continue;
			}
			else
			{
				// rotate left
				//        pp:B                          p:B       //
				// pf:B          p:R     =>      pp:R         v:R //
				//          vf:B     v:R    pf:B      vf:B        //
				if ((p->u = pf = pp->u))
				{
					if (pf->l == pp) pf->l = p;
					else pf->r = p;
				}
				else *pr = p;
				if ((pp->r = pf = p->l)) pf->u = pp;
				p->l = pp;
				pp->u = p;
				p->color = rbtree_color_black;
				pp->color = rbtree_color_red;
				continue;
			}
		}
		label_clear_color:
		// clear color: p, pf
		p->color = pf->color = rbtree_color_black;
		pp->color = rbtree_color_red;
		v = pp;
	}
	(*pr)->color = rbtree_color_black;
	return vn;
}

static void rbtree_balance_delete(register rbtree_t *restrict v, rbtree_t *restrict *restrict pr)
{
	register rbtree_t *p, *f, *l, *r;
	while (v->color == rbtree_color_black && (p = v->u))
	{
		// vf has black inode, so vf != NULL
		// and likely, v is black => black_number(vf) > 1 => vf->l != NULL && vf->r != NULL
		if (v == p->l)
		{
			f = p->r;
			l = f->l;
			r = f->r;
			if (f->color == rbtree_color_red)
			{
				//      pB                    fR                 fB      //
				// vB        fR    =>    pB        rB =>    pR        rB //
				//        lB    rB    vB    lB           vB    lB        //
				if ((f->u = r = p->u))
				{
					if (r->l == p) r->l = f;
					else r->r = f;
				}
				else *pr = f;
				p->r = l;
				l->u = p;
				f->l = p;
				p->u = f;
				f->color = rbtree_color_black;
				p->color = rbtree_color_red;
				continue;
			}
			else if (r->color == rbtree_color_red)
			{
				//      p#                    fB                 f#      //
				// vB        fB    =>    p#        rR =>    pB        rB //
				//        l*    rR    vB    l*           vB    l*        //
				if ((f->u = v = p->u))
				{
					if (v->l == p) v->l = f;
					else v->r = f;
				}
				else *pr = f;
				p->r = l;
				l->u = p;
				f->l = p;
				p->u = f;
				f->color = p->color;
				p->color = rbtree_color_black;
				r->color = rbtree_color_black;
				return ;
			}
			else if (l->color == rbtree_color_red)
			{
				//      p*                  p*                    p*            //
				// vB        fB     => vB        lR       => vB        lB       //
				//        lR    rB             1    fB               1    fR    //
				//      1    2                    2    rB               2    rB //
				p->r = l;
				l->u = p;
				f->l = p = l->r;
				p->u = f;
				l->r = f;
				f->u = l;
				l->color = rbtree_color_black;
				f->color = rbtree_color_red;
				continue;
			}
			else
			{
				//      p*                    pB         //
				// vB        fB    =>    vB        fR    //
				//        lB    rB              lB    rB //
				f->color = rbtree_color_red;
				if (p->color == rbtree_color_red)
				{
					p->color = rbtree_color_black;
					return ;
				}
				else
				{
					v = p;
					continue;
				}
			}
		}
		else
		{
			f = p->l;
			l = f->l;
			r = f->r;
			if (f->color == rbtree_color_red)
			{
				//         pB              fR                 fB         //
				//    fR        vB => lB        pB    => lB        pR    //
				// lB    rB                  rB    vB           rB    vB //
				if ((f->u = l = p->u))
				{
					if (l->l == p) l->l = f;
					else l->r = f;
				}
				else *pr = f;
				p->l = r;
				r->u = p;
				f->r = p;
				p->u = f;
				f->color = rbtree_color_black;
				p->color = rbtree_color_red;
				continue;
			}
			else if (l->color == rbtree_color_red)
			{
				//         p#              fR                 f#         //
				//    fB        vB => lR        p#    => lB        pB    //
				// lR    r*                  r*    vB           r*    vB //
				if ((f->u = v = p->u))
				{
					if (v->l == p) v->l = f;
					else v->r = f;
				}
				else *pr = f;
				p->l = r;
				r->u = p;
				f->r = p;
				p->u = f;
				f->color = p->color;
				p->color = rbtree_color_black;
				l->color = rbtree_color_black;
				return ;
			}
			else if (r->color == rbtree_color_red)
			{
				//         p*                    p*                    p*      //
				//    fB        vB =>       rR        vB =>       rB        vB //
				// lB    rR              fB    2               fR    2         //
				//     1    2         lB    1               lB    1            //
				p->l = r;
				r->u = p;
				f->r = p = r->l;
				p->u = f;
				r->l = f;
				f->u = r;
				r->color = rbtree_color_black;
				f->color = rbtree_color_red;
				continue;
			}
			else
			{
				//         p*                 pB      //
				//    fB        vB =>    fR        vB //
				// lB    rB           lB    rB        //
				f->color = rbtree_color_red;
				if (p->color == rbtree_color_black) v = p;
				else
				{
					p->color = rbtree_color_black;
					return ;
				}
			}
		}
	}
	if (v->color == rbtree_color_red) v->color = rbtree_color_black;
}

static rbtree_t* rbtree_balance_delete_void(register rbtree_t *restrict p, rbtree_t *restrict *restrict pr)
{
	register rbtree_t *f, *l, *r;
	if (!p) return *pr = NULL;
	else if (p->r)
	{
		label_left_re:
		f = p->r;
		if (f->color == rbtree_color_red)
		{
			// pB                 fR              fB      //
			//      fR    => pB        rB => pR        rB //
			//   lB    rB       lB              lB        //
			l = f->l;
			if ((f->u = r = p->u))
			{
				if (r->l == p) r->l = f;
				else r->r = f;
			}
			else *pr = f;
			p->r = l;
			l->u = p;
			f->l = p;
			p->u = f;
			f->color = rbtree_color_black;
			p->color = rbtree_color_red;
			goto label_left_re;
		}
		else if (f->r)
		{
			// p#                 fB              f#      //
			//      fB    => p#        rR => pB        rB //
			//   ?R    rR       ?R              ?R        //
			l = f->l;
			if ((f->u = r = p->u))
			{
				if (r->l == p) r->l = f;
				else r->r = f;
			}
			else *pr = f;
			p->r = l;
			if (l) l->u = p;
			f->l = p;
			p->u = f;
			f->color = p->color;
			p->color = rbtree_color_black;
			f->r->color = rbtree_color_black;
			return NULL;
		}
		else if (f->l)
		{
			// p#            lR          l#    //
			//      fB => p#    fB => pB    fB //
			//   lR                            //
			l = f->l;
			if ((l->u = r = p->u))
			{
				if (r->l == p) r->l = l;
				else r->r = l;
			}
			else *pr = l;
			l->l = p;
			l->r = f;
			p->u = l;
			f->u = l;
			p->r = NULL;
			f->l = NULL;
			l->color = p->color;
			p->color = rbtree_color_black;
			return NULL;
		}
		else
		{
			// p#       pB    //
			//    fB =>    fR //
			f->color = rbtree_color_red;
			if (p->color == rbtree_color_black) return p;
			else p->color = rbtree_color_black;
			return NULL;
		}
	}
	else if (p->l)
	{
		label_right_re:
		f = p->l;
		if (f->color == rbtree_color_red)
		{
			//         pB         fR              fB      //
			//    fR      => lB        pB => lB        pR //
			// lB    rB             rB              rB    //
			r = f->r;
			if ((f->u = l = p->u))
			{
				if (l->l == p) l->l = f;
				else l->r = f;
			}
			else *pr = f;
			p->l = r;
			r->u = p;
			f->r = p;
			p->u = f;
			f->color = rbtree_color_black;
			p->color = rbtree_color_red;
			goto label_right_re;
		}
		else if (f->l)
		{
			//         p#         fB              f#      //
			//    fB      => lR        p# => lB        pB //
			// lR    *R             *R              *B    //
			r = f->r;
			if ((f->u = l = p->u))
			{
				if (l->l == p) l->l = f;
				else l->r = f;
			}
			else *pr = f;
			p->l = r;
			if (r) r->u = p;
			f->r = p;
			p->u = f;
			f->color = p->color;
			p->color = rbtree_color_black;
			f->l->color = rbtree_color_black;
			return NULL;
		}
		else if (f->r)
		{
			//      p#       rR          r#    //
			// fB      => fB    p# => fB    pB //
			//    rR                           //
			r = f->r;
			if ((r->u = l = p->u))
			{
				if (l->l == p) l->l = r;
				else l->r = r;
			}
			else *pr = r;
			r->l = f;
			r->r = p;
			f->u = r;
			p->u = r;
			f->r = NULL;
			p->l = NULL;
			r->color = p->color;
			p->color = rbtree_color_black;
			return NULL;
		}
		else
		{
			//    p#       pB //
			// fB    => fR    //
			f->color = rbtree_color_red;
			if (p->color == rbtree_color_black) return p;
			else p->color = rbtree_color_black;
			return NULL;
		}
	}
	else return p;
}

rbtree_t* rbtree_insert(rbtree_t *restrict *restrict pr, const char *restrict name, uint64_t index, const void *value, rbtree_func_free_f freeFunc)
{
	register rbtree_t *v, *p, *restrict *restrict pv;
	int s_cmp;
	pv = pr;
	p = NULL;
	s_cmp = 0;
	if (name)
	{
		while ((v = *pv))
		{
			p = v;
			if (v->key_name)
			{
				s_cmp = strcmp(name, v->key_name);
				if (s_cmp < 0) pv = &v->l;
				else if (s_cmp > 0) pv = &v->r;
				else return NULL;
			}
			else pv = &v->l;
		}
		s_cmp = strlen(name) + 1;
	}
	else
	{
		while ((v = *pv))
		{
			p = v;
			if (v->key_name) pv = &v->r;
			else
			{
				if (index < v->key_index) pv = &v->l;
				else if (index > v->key_index) pv = &v->r;
				else return NULL;
			}
		}
	}
	v = (rbtree_t*) calloc(1, sizeof(rbtree_t));
	if (!v) return NULL;
	v->u = p;
	if (name)
	{
		v->key_name = (char *) malloc(s_cmp);
		if (!v->key_name)
		{
			free(v);
			return NULL;
		}
		memcpy((void *) v->key_name, name, s_cmp);
	}
	v->key_index = index;
	v->value = (void *) value;
	v->freeFunc = freeFunc;
	*pv = v;
	return rbtree_balance_insert(v, pr);
}

void rbtree_delete_by_pointer(rbtree_t *restrict *restrict pr, register rbtree_t *restrict p)
{
	register rbtree_t *v, *pp;
	uint32_t color;
	if ((v = p->r))
	{
		do
		{
			pp = v;
		} while ((v = v->l));
		if (p->freeFunc) p->freeFunc(p);
		if (p->key_name) free((void *) p->key_name);
		p->key_name = pp->key_name;
		p->key_index = pp->key_index;
		p->value = pp->value;
		p->freeFunc = pp->freeFunc;
		v = pp->r;
		if (p == pp->u)
		{
			p->r = v;
			if (v) v->u = p;
		}
		else
		{
			p = pp->u;
			p->l = v;
			if (v) v->u = p;
		}
		color = pp->color;
		free(pp);
	}
	else
	{
		v = p->l;
		pp = p->u;
		if (v) v->u = pp;
		if (pp)
		{
			if (p == pp->l) pp->l = v;
			else pp->r = v;
		}
		else *pr = v;
		if (p->freeFunc) p->freeFunc(p);
		color = p->color;
		free(p);
		p = pp;
	}
	if (color == rbtree_color_black)
	{
		if (!v) v = rbtree_balance_delete_void(p, pr);
		if (v) rbtree_balance_delete(v, pr);
	}
}

void rbtree_delete(register rbtree_t *restrict *restrict pr, const char *restrict name, uint64_t index)
{
	register rbtree_t *p;
	int s_cmp;
	p = *pr;
	if (name)
	{
		while (p)
		{
			if (p->key_name)
			{
				s_cmp = strcmp(name, p->key_name);
				if (s_cmp < 0) p = p->l;
				else if (s_cmp > 0) p = p->r;
				else break;
			}
			else p = p->l;
		}
	}
	else
	{
		while (p)
		{
			if (p->key_name) p = p->r;
			else
			{
				if (index < p->key_index) p = p->l;
				else if (index > p->key_index) p = p->r;
				else break;
			}
		}
	}
	if (p) rbtree_delete_by_pointer(pr, p);
}

rbtree_t* rbtree_find(rbtree_t *const restrict *restrict pr, const char *restrict name, uint64_t index)
{
	register rbtree_t *p;
	int s_cmp;
	p = *pr;
	if (name)
	{
		while (p)
		{
			if (p->key_name)
			{
				s_cmp = strcmp(name, p->key_name);
				if (s_cmp < 0) p = p->l;
				else if (s_cmp > 0) p = p->r;
				else return p;
			}
			else p = p->l;
		}
	}
	else
	{
		while (p)
		{
			if (p->key_name) p = p->r;
			else
			{
				if (index < p->key_index) p = p->l;
				else if (index > p->key_index) p = p->r;
				else return p;
			}
		}
	}
	return NULL;
}

void rbtree_clear(rbtree_t *restrict *restrict pr)
{
	register rbtree_t *v, *c;
	v = *pr;
	*pr = NULL;
	while (v)
	{
		if ((c = v->l))
		{
			v->l = NULL;
			v = c;
			continue;
		}
		else if ((c = v->r))
		{
			v->r = NULL;
			v = c;
			continue;
		}
		else
		{
			c = v->u;
			if (v->freeFunc) v->freeFunc(v);
			if (v->key_name) free((void *) v->key_name);
			free(v);
			v = c;
			continue;
		}
	}
}

void rbtree_call(rbtree_t *const restrict *restrict pr, rbtree_func_call_f callFunc, void *data)
{
	register rbtree_t *v;
	v = *pr;
	if (callFunc && v)
	{
		// find minimum
		while (v->l) v = v->l;
		while (v)
		{
			callFunc(v, data);
			// find next
			if (v->r)
			{
				v = v->r;
				while (v->l) v = v->l;
			}
			else
			{
				while (v->u && v->u->r == v) v = v->u;
				v = v->u;
			}
		}
	}
}

void rbtree_isfree(rbtree_t *restrict *restrict pr, rbtree_func_isfree_f callFunc, void *data)
{
	register rbtree_t *v, *n;
	v = *pr;
	if (callFunc && v)
	{
		// find minimum
		while (v->l) v = v->l;
		while (v)
		{
			if (callFunc(v, data))
			{
				n = v;
				if (!v->r)
				{
					n = v;
					while (n->u && n->u->r == n) n = n->u;
					n = n->u;
				}
				rbtree_delete_by_pointer(pr, v);
			}
			else
			{
				// find next
				if (!(n = v->r))
				{
					n = v;
					while (n->u && n->u->r == n) n = n->u;
					n = n->u;
				}
				else while (n->l) n = n->l;
			}
			v = n;
		}
	}
}
