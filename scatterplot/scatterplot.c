#include "scatterplot.h"
#include <sys/types.h>
#include <stdlib.h>
#include <bmp.h>

#define sp_page_size  1024

typedef struct sp_point_page_t {
	struct sp_point_page_t *next;
	size_t used;
	double x[sp_page_size];
	double y[sp_page_size];
	uint32_t color[sp_page_size];
} sp_point_page_t;

struct scatterplot_s {
	uint32_t w;
	uint32_t h;
	uint32_t k;
	uint32_t color;
	double x1;
	double y1;
	double x2;
	double y2;
	sp_point_page_t *page_list;
};

static void scatterplot_free_func(register scatterplot_s *restrict sp)
{
	register sp_point_page_t *restrict p, *restrict q;
	for (p = sp->page_list; p; p = q)
	{
		q = p->next;
		free(p);
	}
}

scatterplot_s* scatterplot_alloc(void)
{
	register scatterplot_s *restrict sp;
	sp = refer_alloz(sizeof(scatterplot_s));
	if (sp) refer_set_free(sp, (refer_free_f) scatterplot_free_func);
	return sp;
}

void scatterplot_set_image(register scatterplot_s *restrict sp, uint32_t w, uint32_t h, uint32_t k, uint32_t color)
{
	sp->w = w;
	sp->h = h;
	sp->k = k;
	sp->color = color;
}

void scatterplot_set_view(register scatterplot_s *restrict sp, double x1, double y1, double x2, double y2)
{
	sp->x1 = x1;
	sp->x2 = x2;
	sp->y1 = y1;
	sp->y2 = y2;
}

void scatterplot_clear(register scatterplot_s *restrict sp)
{
	register sp_point_page_t *restrict p, *restrict q;
	for (p = sp->page_list; p; p = q)
	{
		q = p->next;
		free(p);
	}
	sp->page_list = NULL;
}

scatterplot_s* scatterplot_pos(register scatterplot_s *restrict sp, double x, double y, uint32_t color)
{
	register sp_point_page_t *restrict p;
	register size_t i;
	if (!(p = sp->page_list) || (i = p->used) >= sp_page_size)
	{
		p = (sp_point_page_t *) calloc(1, sizeof(sp_point_page_t));
		if (!p) return NULL;
		p->next = sp->page_list;
		sp->page_list = p;
		i = 0;
	}
	p->x[i] = x;
	p->y[i] = y;
	p->color[i] = color;
	++p->used;
	return sp;
}

static void scatterplot_calc_view(register scatterplot_s *restrict sp, double *x1, double *y1, double *x2, double *y2)
{
	register sp_point_page_t *restrict p;
	register size_t i, n;
	register double xmin, ymin, xmax, ymax, c;
	if ((p = sp->page_list))
	{
		xmin = xmax = *p->x;
		ymin = ymax = *p->y;
		while (p)
		{
			for (i = 0, n = p->used; i < n; ++i)
			{
				if ((c = p->x[i]) < xmin) xmin = c;
				if (c > xmax) xmax = c;
				if ((c = p->y[i]) < ymin) ymin = c;
				if (c > ymax) ymax = c;
			}
			p = p->next;
		}
		*x1 = xmin;
		*x2 = xmax;
		*y1 = ymin;
		*y2 = ymax;
	}
	else *x1 = *y1 = *x2 = *y2 = 0;
}

static uint32_t* scatterplot_gen_image(register sp_point_page_t *restrict p, uint32_t w, uint32_t h, uint32_t color, double xp, double yp, double xk, double yk)
{
	register uint32_t *restrict data;
	register uint32_t i, n, x, y;
	data = (uint32_t *) malloc((n = w * h) * sizeof(uint32_t));
	if (data)
	{
		for (i = 0; i < n; ++i) data[i] = color;
		while (p)
		{
			for (i = 0, n = p->used; i < n; ++i)
			{
				x = (uint32_t) ((p->x[i] - xp) * xk);
				y = (uint32_t) ((p->y[i] - yp) * yk);
				if (x < w && y < h) data[y * w + x] = p->color[i];
			}
			p = p->next;
		}
	}
	return data;
}

int scatterplot_save(register scatterplot_s *restrict sp, const char *restrict path)
{
	uint32_t *data;
	uint32_t w, h;
	double xp, yp, xk, yk;
	int r;
	r = -1;
	if (sp->x2 > sp->x1 && sp->y2 > sp->y1)
	{
		xp = sp->x1;
		yp = sp->y1;
		xk = sp->x2 - sp->x1;
		yk = sp->y2 - sp->y1;
	}
	else
	{
		scatterplot_calc_view(sp, &xp, &yp, &xk, &yk);
		xk -= xp;
		yk -= yp;
	}
	if (xk <= 0) xk = 1;
	if (yk <= 0) yk = 1;
	xp -= xk * 0.1;
	yp -= yk * 0.1;
	xk *= 1.2;
	yk *= 1.2;
	if (sp->w && sp->h)
	{
		w = sp->w;
		h = sp->h;
		xk = w / xk;
		yk = h / yk;
	}
	else if (sp->k)
	{
		if (xk >= yk)
		{
			w = sp->k;
			xk = w / xk;
			h = xk * yk;
			yk = xk;
		}
		else
		{
			h = sp->k;
			yk = h / yk;
			w = xk * yk;
			xk = yk;
		}
	}
	else
	{
		if (xk >= yk)
		{
			w = 1024;
			xk = w / xk;
			h = xk * yk;
			yk = xk;
		}
		else
		{
			h = 1024;
			yk = h / yk;
			w = xk * yk;
			xk = yk;
		}
	}
	data = scatterplot_gen_image(sp->page_list, w, h, sp->color, xp, yp, xk, yk);
	if (data)
	{
		r = bmp_save_bgra(path, w, h, data);
		free(data);
	}
	return r;
}
