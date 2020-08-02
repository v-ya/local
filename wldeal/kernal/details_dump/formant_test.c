#include <json.h>
#include <scatterplot.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#define marco_dmax  128
#define marco_fmax  20000

typedef struct statistics_p_t {
	uint32_t n;
	uint32_t ok;
	double x;
	double y;
	double y2;
} statistics_p_t;

typedef struct statistics_t {
	statistics_p_t s[marco_dmax];
} statistics_t;

typedef struct tone_avg_s {
	double l;
	size_t n;
	statistics_p_t p[];
} tone_avg_s;

#define statistics_init(_x)  ((_x = alloca(sizeof(statistics_t)))?memset(_x, 0, sizeof(statistics_t)):_x)

static void statistics_pos(register statistics_t *restrict s, register uint32_t i, register double x, register double y)
{
	if (i < marco_dmax)
	{
		register statistics_p_t *p;
		p = s->s + i;
		++p->n;
		p->x += x;
		p->y += y;
		p->y2 += y * y;
	}
}

static void statistics_ok(statistics_t *restrict s)
{
	register statistics_p_t *p;
	register double y, y2;
	register uint32_t i, n;
	p = s->s;
	i = marco_dmax;
	do {
		--i;
		if (!p->ok && (n = p->n))
		{
			p->x /= n;
			p->y = y = p->y / n;
			y2 = p->y2 / n;
			y2 -= y * y;
			if (y2 > 0) y2 = sqrt(y2);
			else y2 = 0;
			p->y2 = y2;
			p->ok = 1;
		}
		++p;
	} while (i);
}

static void statistics_sp(statistics_t *restrict s, scatterplot_s *restrict sp)
{
	register statistics_p_t *p;
	register uint32_t i;
	p = s->s;
	i = marco_dmax;
	do {
		--i;
		if (p->ok)
		{
			scatterplot_pos(sp, p->x, p->y, 0xff0000);
			scatterplot_pos(sp, p->x, p->y2, 0x00ff00);
		}
		++p;
	} while (i);
}

static void statistics_sp_d(statistics_t *restrict s, scatterplot_s *restrict sp)
{
	register statistics_p_t *p;
	register double xlast, ylast;
	register uint32_t i, mask;
	p = s->s;
	i = marco_dmax;
	mask = 0;
	do {
		if (p->ok)
		{
			if (mask)
				scatterplot_pos(sp, (xlast + p->x) / 2, (p->y - ylast) / (p->x - xlast), 0xff0000);
			xlast = p->x;
			ylast = p->y;
			mask = 1;
			scatterplot_pos(sp, p->x, p->y2, 0x00ff00);
		}
		--i;
		++p;
	} while (i);
}

static tone_avg_s* tone_avg_alloc(size_t n, double L)
{
	register tone_avg_s *restrict r;
	if (n && L > 0)
	{
		r = refer_alloz(sizeof(tone_avg_s) + sizeof(statistics_p_t) * n);
		if (r)
		{
			r->l = L / n;
			r->n = n;
			return r;
		}
	}
	return NULL;
}

static void tone_avg_send_statistics(register tone_avg_s *restrict t, statistics_t *restrict s)
{
	register statistics_p_t *restrict p, *restrict q;
	register uint32_t n, i;
	p = s->s;
	n = marco_dmax;
	do {
		if (p->ok)
		{
			i = (uint32_t) p->x / t->l;
			if (i < t->n)
			{
				q = t->p + i;
				++q->n;
				q->x += p->x;
				q->y += p->y;
				q->y2 += p->y2;
			}
		}
		++p;
	} while (--n);
}

static void tone_avg_ok(tone_avg_s *restrict t)
{
	register statistics_p_t *restrict p;
	register size_t i;
	register uint32_t n;
	p = t->p;
	i = t->n;
	do {
		if (!p->ok && (n = p->n))
		{
			p->x /= n;
			p->y /= n;
			p->y2 /= n;
			p->ok = 1;
		}
		++p;
	} while (--i);
}

static void tone_avg_sp(tone_avg_s *restrict t, scatterplot_s *restrict sp)
{
	register statistics_p_t *restrict p;
	register size_t i;
	p = t->p;
	i = t->n;
	do {
		if (p->ok)
		{
			scatterplot_pos(sp, p->x, p->y, 0xff0000);
			scatterplot_pos(sp, p->x, p->y2, 0x00ff00);
		}
		++p;
	} while (--i);
}

static json_inode_t* tone_avg_save_json_create_array_inode(double s, double v)
{
	json_inode_t *o, *c;
	if ((o = json_create_object()))
	{
		if (json_object_set(o, "s", c = json_create_floating(s)) &&
			json_object_set(o, "v", c = json_create_floating(v)))
			return o;
		if (c) json_free(c);
		json_free(o);
	}
	return NULL;
}

static void tone_avg_save_json(tone_avg_s *restrict t, const char *restrict path)
{
	json_inode_t *array, *v;
	register statistics_p_t *restrict p;
	register size_t i;
	p = t->p;
	i = t->n;
	array = json_create_array();
	v = NULL;
	if (array)
	{
		if (!json_array_set(array, array->value.array.number, v = tone_avg_save_json_create_array_inode(0, 0)))
			goto Err;
		do {
			if (p->ok)
			{
				if (!json_array_set(array, array->value.array.number, v = tone_avg_save_json_create_array_inode(p->x, p->y)))
					goto Err;
			}
			++p;
		} while (--i);
		if (!json_array_set(array, array->value.array.number, v = tone_avg_save_json_create_array_inode(t->l * t->n, 0)))
			goto Err;
		v = NULL;
		i = json_length(array);
		if (i)
		{
			char *s;
			s = (char *) malloc(i);
			if (s)
			{
				if (json_encode_intext(array, s))
				{
					FILE *fp;
					fp = fopen(path, "wb");
					if (fp)
					{
						fwrite(s, 1, i, fp);
						fclose(fp);
					}
				}
				free(s);
			}
		}
		Err:
		if (v) json_free(v);
		json_free(array);
	}
}

static json_inode_t* get_floating(register json_inode_t *restrict value, register double *restrict v)
{
	if (value)
	{
		if (value->type == json_inode_floating)
			*v = value->value.floating;
		else if (value->type == json_inode_integer)
			*v = (double) value->value.integer;
		else value = NULL;
	}
	return value;
}

static inline uint32_t get_time_color(register uint32_t i, register uint32_t n, register double *offset)
{
	register double p;
	p = (double) i / (n - 1);
	*offset = p * 0.5;
	return ((uint32_t) (255 * p) << 8) | ((uint32_t) (255 * (1 - p)) << 16);
}

static void sp_axis_n(scatterplot_s *restrict sp, double limit)
{
	double f, offset;
	uint32_t i, m;
	scatterplot_set_view(sp, 0, 0, marco_dmax + 1, limit);
	offset = limit * 0.001;
	for (i = 0; i < marco_dmax; i += 5)
	{
		m = (i & 1)?5:10;
		for (f = 0; m; --m)
			scatterplot_pos(sp, i, f -= offset, 0x0000ff);
	}
	for (f = 0; f < limit; f += 1)
	{
		for (i = 0; i < (marco_dmax * 16); ++i)
		scatterplot_pos(sp, i / 16.0, f, 0x0000ff);
	}
}

static void sp_axis_f(scatterplot_s *restrict sp, double limit)
{
	double f, offset;
	uint32_t i, m;
	scatterplot_set_view(sp, 0, 0, marco_fmax, limit);
	offset = limit * 0.001;
	for (i = 0; i <= (marco_fmax / 100); ++i)
	{
		m = (i % 5)?3:((i & 1)?5:10);
		for (f = 0; m; --m)
			scatterplot_pos(sp, i * 100, f -= offset, 0x0000ff);
	}
	for (f = 0; f < limit; f += 1)
	{
		for (i = 0; i < marco_fmax; ++i)
		scatterplot_pos(sp, i, f, 0x0000ff);
	}
}

static void sp_axis_nl(scatterplot_s *restrict sp, double limit)
{
	double f, offset;
	uint32_t i, j, m, c;
	scatterplot_set_view(sp, 0, -limit, marco_dmax + 1, limit);
	offset = limit * 0.001;
	for (i = 0; i < marco_dmax; i += 5)
	{
		m = (i & 1)?5:10;
		for (f = limit; m; --m)
		{
			f += offset;
			scatterplot_pos(sp, i, f, 0x0000ff);
			scatterplot_pos(sp, i, -f, 0x0000ff);
		}
	}
	j = 0;
	m = 1;
	c = 0x0000ff;
	for (f = 0; f < limit; f += 1)
	{
		for (i = 0; i < (marco_dmax * 16); ++i)
		{
			scatterplot_pos(sp, i / 16.0, f, c);
			scatterplot_pos(sp, i / 16.0, -f, c);
		}
		if (j++ >= m)
		{
			m <<= 1;
			c >>= 1;
		}
	}
}

static void sp_axis_fl(scatterplot_s *restrict sp, double limit)
{
	double f, offset;
	uint32_t i, j, m, c;
	scatterplot_set_view(sp, 0, -limit, marco_fmax, limit);
	offset = limit * 0.001;
	for (i = 0; i <= (marco_fmax / 100); ++i)
	{
		m = (i % 5)?3:((i & 1)?5:10);
		for (f = limit; m; --m)
		{
			f += offset;
			scatterplot_pos(sp, i * 100, f, 0x0000ff);
			scatterplot_pos(sp, i * 100, -f, 0x0000ff);
		}
	}
	j = 0;
	m = 1;
	c = 0x0000ff;
	for (f = 0; f < limit; f += 1)
	{
		for (i = 0; i < marco_fmax; ++i)
		{
			scatterplot_pos(sp, i, f, c);
			scatterplot_pos(sp, i, -f, c);
		}
		if (j++ >= m)
		{
			m <<= 1;
			c >>= 1;
		}
	}
}

static void sp_formant_n(scatterplot_s *restrict sp, json_inode_t *array, double limit)
{
	json_inode_t *a, *o, *v;
	double f, offset;
	uint32_t i, n, j, m, k, c;
	n = array->value.array.number;
	for (i = 0; i < n; ++i)
	{
		o = json_array_find(array, i);
		if (o && o->type == json_inode_object)
		{
			c = get_time_color(i, n, &offset);
			a = json_object_find(o, "sa");
			if (a && a->type == json_inode_array)
			{
				m = a->value.array.number;
				for (j = 0; j < m; j = k)
				{
					k = j + 1;
					v = json_array_find(a, j);
					if (get_floating(v, &f))
						scatterplot_pos(sp, j + offset, f * k * k, c);
				}
			}
		}
	}
	sp_axis_n(sp, limit);
}

static void sp_formant_f(scatterplot_s *restrict sp, json_inode_t *array, double limit)
{
	json_inode_t *a, *o, *v;
	double f, fre, offset;
	uint32_t i, n, j, m, k, c;
	n = array->value.array.number;
	for (i = 0; i < n; ++i)
	{
		o = json_array_find(array, i);
		if (o && o->type == json_inode_object)
		{
			v = json_object_find(o, "f");
			if (get_floating(v, &fre))
			{
				c = get_time_color(i, n, &offset);
				a = json_object_find(o, "sa");
				if (a && a->type == json_inode_array)
				{
					m = a->value.array.number;
					for (j = 0; j < m; j = k)
					{
						k = j + 1;
						v = json_array_find(a, j);
						if (k * fre < marco_fmax && get_floating(v, &f))
							scatterplot_pos(sp, k * fre, f * k * k, c);
					}
				}
			}
		}
	}
	sp_axis_f(sp, limit);
}

static void sp_formant_nl(scatterplot_s *restrict sp, json_inode_t *array, double limit, double yp)
{
	json_inode_t *a, *o, *v;
	double f, offset;
	uint32_t i, n, j, m, k, c;
	n = array->value.array.number;
	for (i = 0; i < n; ++i)
	{
		o = json_array_find(array, i);
		if (o && o->type == json_inode_object)
		{
			c = get_time_color(i, n, &offset);
			a = json_object_find(o, "sa");
			if (a && a->type == json_inode_array)
			{
				m = a->value.array.number;
				for (j = 0; j < m; j = k)
				{
					k = j + 1;
					v = json_array_find(a, j);
					if (get_floating(v, &f))
						scatterplot_pos(sp, j + offset, log(f * k * k) + yp, c);
				}
			}
		}
	}
	sp_axis_nl(sp, limit);
}

static void sp_formant_fl(scatterplot_s *restrict sp, json_inode_t *array, double limit, double yp)
{
	json_inode_t *a, *o, *v;
	double f, fre, offset;
	uint32_t i, n, j, m, k, c;
	n = array->value.array.number;
	for (i = 0; i < n; ++i)
	{
		o = json_array_find(array, i);
		if (o && o->type == json_inode_object)
		{
			v = json_object_find(o, "f");
			if (get_floating(v, &fre))
			{
				c = get_time_color(i, n, &offset);
				a = json_object_find(o, "sa");
				if (a && a->type == json_inode_array)
				{
					m = a->value.array.number;
					for (j = 0; j < m; j = k)
					{
						k = j + 1;
						v = json_array_find(a, j);
						if (k * fre < marco_fmax && get_floating(v, &f))
							scatterplot_pos(sp, k * fre, log(f * k * k) + yp, c);
					}
				}
			}
		}
	}
	sp_axis_fl(sp, limit);
}

static void sp_formant_nld(scatterplot_s *restrict sp, json_inode_t *array, double limit)
{
	json_inode_t *a, *o, *v;
	double f, offset, last;
	uint32_t i, n, j, m, k, c;
	n = array->value.array.number;
	for (i = 0; i < n; ++i)
	{
		o = json_array_find(array, i);
		if (o && o->type == json_inode_object)
		{
			c = get_time_color(i, n, &offset);
			a = json_object_find(o, "sa");
			if (a && a->type == json_inode_array)
			{
				m = a->value.array.number;
				for (j = 0; j < m; j = k)
				{
					k = j + 1;
					v = json_array_find(a, j);
					if (get_floating(v, &f))
					{
						f = log(f * k * k);
						if (j) scatterplot_pos(sp, j + offset - 0.5, f - last, c);
						last = f;
					}
				}
			}
		}
	}
	sp_axis_nl(sp, limit);
}

static void sp_formant_fld(scatterplot_s *restrict sp, json_inode_t *array, double limit)
{
	json_inode_t *a, *o, *v;
	double f, fre, offset, last;
	uint32_t i, n, j, m, k, c;
	n = array->value.array.number;
	for (i = 0; i < n; ++i)
	{
		o = json_array_find(array, i);
		if (o && o->type == json_inode_object)
		{
			v = json_object_find(o, "f");
			if (get_floating(v, &fre))
			{
				c = get_time_color(i, n, &offset);
				a = json_object_find(o, "sa");
				if (a && a->type == json_inode_array)
				{
					m = a->value.array.number;
					for (j = 0; j < m; j = k)
					{
						k = j + 1;
						v = json_array_find(a, j);
						if (j * fre < marco_fmax && get_floating(v, &f))
						{
							f = log(f * k * k);
							if (j) scatterplot_pos(sp, (k - 0.5) * fre, (f - last) / fre, c);
							last = f;
						}
					}
				}
			}
		}
	}
	sp_axis_fl(sp, limit);
}

static void sp_formant_fls(scatterplot_s *restrict sp, json_inode_t *array, double limit)
{
	statistics_t *s;
	json_inode_t *a, *o, *v;
	double f, fre;
	uint32_t i, n, j, m, k;
	if (!statistics_init(s)) return ;
	n = array->value.array.number;
	for (i = 0; i < n; ++i)
	{
		o = json_array_find(array, i);
		if (o && o->type == json_inode_object)
		{
			v = json_object_find(o, "f");
			if (get_floating(v, &fre))
			{
				a = json_object_find(o, "sa");
				if (a && a->type == json_inode_array)
				{
					m = a->value.array.number;
					for (j = 0; j < m; j = k)
					{
						k = j + 1;
						v = json_array_find(a, j);
						if (get_floating(v, &f))
							statistics_pos(s, j, k * fre, log(f * k * k));
					}
				}
			}
		}
	}
	statistics_ok(s);
	statistics_sp(s, sp);
}

static void sp_formant_flds(scatterplot_s *restrict sp, json_inode_t *array, double limit)
{
	statistics_t *s;
	json_inode_t *a, *o, *v;
	double f, fre, last;
	uint32_t i, n, j, m, k;
	if (!statistics_init(s)) return ;
	n = array->value.array.number;
	for (i = 0; i < n; ++i)
	{
		o = json_array_find(array, i);
		if (o && o->type == json_inode_object)
		{
			v = json_object_find(o, "f");
			if (get_floating(v, &fre))
			{
				a = json_object_find(o, "sa");
				if (a && a->type == json_inode_array)
				{
					m = a->value.array.number;
					for (j = 0; j < m; j = k)
					{
						k = j + 1;
						v = json_array_find(a, j);
						if (j * fre < marco_fmax && get_floating(v, &f))
						{
							f = log(f * k * k);
							if (j) statistics_pos(s, j, (k - 0.5) * fre, (f - last) / fre);
							last = f;
						}
					}
				}
			}
		}
	}
	statistics_ok(s);
	statistics_sp(s, sp);
}

static void sp_formant_flsd(scatterplot_s *restrict sp, json_inode_t *array, double limit)
{
	statistics_t *s;
	json_inode_t *a, *o, *v;
	double f, fre;
	uint32_t i, n, j, m, k;
	if (!statistics_init(s)) return ;
	n = array->value.array.number;
	for (i = 0; i < n; ++i)
	{
		o = json_array_find(array, i);
		if (o && o->type == json_inode_object)
		{
			v = json_object_find(o, "f");
			if (get_floating(v, &fre))
			{
				a = json_object_find(o, "sa");
				if (a && a->type == json_inode_array)
				{
					m = a->value.array.number;
					for (j = 0; j < m; j = k)
					{
						k = j + 1;
						v = json_array_find(a, j);
						if (get_floating(v, &f))
							statistics_pos(s, j, k * fre, log(f * k * k));
					}
				}
			}
		}
	}
	statistics_ok(s);
	statistics_sp_d(s, sp);
}

static void sp_formant_flds_avg(scatterplot_s *restrict sp, json_inode_t *array, double limit, tone_avg_s *restrict ta)
{
	statistics_t *s;
	json_inode_t *a, *o, *v;
	double f, fre, last;
	uint32_t i, n, j, m, k;
	if (!statistics_init(s)) return ;
	n = array->value.array.number;
	for (i = 0; i < n; ++i)
	{
		o = json_array_find(array, i);
		if (o && o->type == json_inode_object)
		{
			v = json_object_find(o, "f");
			if (get_floating(v, &fre))
			{
				a = json_object_find(o, "sa");
				if (a && a->type == json_inode_array)
				{
					m = a->value.array.number;
					for (j = 0; j < m; j = k)
					{
						k = j + 1;
						v = json_array_find(a, j);
						if (j * fre < marco_fmax && get_floating(v, &f))
						{
							f = log(f * k * k);
							if (j) statistics_pos(s, j, (k - 0.5) * fre, (f - last) / fre);
							last = f;
						}
					}
				}
			}
		}
	}
	statistics_ok(s);
	tone_avg_send_statistics(ta, s);
}

static json_inode_t* load_dump_json(const char *restrict path)
{
	register json_inode_t *restrict r;
	if ((r = json_load(path)))
	{
		if (r->type != json_inode_array)
		{
			json_free(r);
			r = NULL;
			printf("json [%s] not array\n", path);
		}
	}
	else printf("load json [%s] fail\n", path);
	return r;
}

enum exec_type_e {
	exec_type_null,
	exec_type_formant_n,
	exec_type_formant_f,
	exec_type_formant_nl,
	exec_type_formant_fl,
	exec_type_formant_nld,
	exec_type_formant_fld,
	exec_type_formant_fls,
	exec_type_formant_flds,
	exec_type_formant_flsd,
	exec_type_formant_flds_avg
};

static uint32_t get_exec_type(const char *restrict type)
{
	uint32_t r;
	r = 0;
	if (!type) goto label_miss;
	else if (!strcmp(type, "formant_n")) r = exec_type_formant_n;
	else if (!strcmp(type, "formant_f")) r = exec_type_formant_f;
	else if (!strcmp(type, "formant_nl")) r = exec_type_formant_nl;
	else if (!strcmp(type, "formant_fl")) r = exec_type_formant_fl;
	else if (!strcmp(type, "formant_nld")) r = exec_type_formant_nld;
	else if (!strcmp(type, "formant_fld")) r = exec_type_formant_fld;
	else if (!strcmp(type, "formant_fls")) r = exec_type_formant_fls;
	else if (!strcmp(type, "formant_flds")) r = exec_type_formant_flds;
	else if (!strcmp(type, "formant_flsd")) r = exec_type_formant_flsd;
	else if (!strcmp(type, "formant_flds_avg")) r = exec_type_formant_flds_avg;
	label_miss:
	return r;
}

static inline void help(const char *restrict exec)
{
	printf(
		"%s <type> [ ... ]\n"
		"\t" "formant_n        <dump.json> <output.bmp> <y-limit>\n"
		"\t" "formant_f        <dump.json> <output.bmp> <y-limit>\n"
		"\t" "formant_nl       <dump.json> <output.bmp> <y-limit> <y-offset>\n"
		"\t" "formant_fl       <dump.json> <output.bmp> <y-limit> <y-offset>\n"
		"\t" "formant_nld      <dump.json> <output.bmp> <y-limit>\n"
		"\t" "formant_fld      <dump.json> <output.bmp> <y-limit>\n"
		"\t" "formant_fls      <output.bmp> <y-limit> <dump.json> [ ... ]\n"
		"\t" "formant_flds     <output.bmp> <y-limit> <dump.json> [ ... ]\n"
		"\t" "formant_flsd     <output.bmp> <y-limit> <dump.json> [ ... ]\n"
		"\t" "formant_flds_avg <output.bmp> <y-limit> <n-split> <tone.json> <dump.json> [ ... ]\n"
		, exec
	);
}

int main(int argc, const char *argv[])
{
	scatterplot_s *sp;
	json_inode_t *dump;
	const char *output;
	const char *exec;
	const char *tone;
	double limit, offset;
	size_t split_n;
	uint32_t type;
	int r;
	r = -1;
	exec = argv[0];
	if (argc > 1 && (type = get_exec_type(argv[1])))
	{
		argc -= 2;
		argv += 2;
		sp = scatterplot_alloc();
		if (sp)
		{
			scatterplot_set_image(sp, 2048, 1024, 0, 0);
			dump = NULL;
			output = NULL;
			limit = offset = 0;
			switch (type)
			{
				case exec_type_formant_n:
				case exec_type_formant_f:
				case exec_type_formant_nld:
				case exec_type_formant_fld:
					if (argc != 3) goto Help;
					label_formant_base:
					dump = load_dump_json(argv[0]);
					if (!dump) goto Err;
					output = argv[1];
					limit = atof(argv[2]);
					if (limit <= 0) limit = 1;
					break;
				case exec_type_formant_nl:
				case exec_type_formant_fl:
					if (argc != 4) goto Help;
					offset = atof(argv[3]);
					goto label_formant_base;
				case exec_type_formant_fls:
				case exec_type_formant_flds:
				case exec_type_formant_flsd:
					if (argc < 3) goto Help;
					output = argv[0];
					limit = atof(argv[1]);
					if (limit <= 0) limit = 1;
					argc -= 2;
					argv += 2;
					break;
				case exec_type_formant_flds_avg:
					if (argc < 5) goto Help;
					output = argv[0];
					limit = atof(argv[1]);
					if (limit <= 0) limit = 1;
					split_n = atol(argv[2]);
					tone = argv[3];
					argc -= 4;
					argv += 4;
					break;
				default:
					goto Help;
			}
			switch (type)
			{
				case exec_type_formant_n:
					sp_formant_n(sp, dump, limit);
					break;
				case exec_type_formant_f:
					sp_formant_f(sp, dump, limit);
					break;
				case exec_type_formant_nl:
					sp_formant_nl(sp, dump, limit, offset);
					break;
				case exec_type_formant_fl:
					sp_formant_fl(sp, dump, limit, offset);
					break;
				case exec_type_formant_nld:
					sp_formant_nld(sp, dump, limit);
					break;
				case exec_type_formant_fld:
					sp_formant_fld(sp, dump, limit);
					break;
				case exec_type_formant_fls:
				case exec_type_formant_flds:
				case exec_type_formant_flsd:
					while (argc)
					{
						dump = load_dump_json(argv[0]);
						if (!dump) goto Err;
						switch (type)
						{
							case exec_type_formant_fls:
								sp_formant_fls(sp, dump, limit);
								break;
							case exec_type_formant_flds:
								sp_formant_flds(sp, dump, limit);
								break;
							case exec_type_formant_flsd:
								sp_formant_flsd(sp, dump, limit);
								break;
						}
						json_free(dump);
						dump = NULL;
						--argc;
						++argv;
					}
					sp_axis_fl(sp, limit);
					break;
				case exec_type_formant_flds_avg:
					if (split_n && split_n < marco_fmax)
					{
						tone_avg_s *ta;
						ta = tone_avg_alloc(split_n, marco_fmax);
						if (!ta) goto Err;
						while (argc)
						{
							dump = load_dump_json(argv[0]);
							if (!dump)
							{
								refer_free(ta);
								goto Err;
							}
							sp_formant_flds_avg(sp, dump, limit, ta);
							json_free(dump);
							dump = NULL;
							--argc;
							++argv;
						}
						tone_avg_ok(ta);
						tone_avg_sp(ta, sp);
						sp_axis_fl(sp, limit);
						tone_avg_save_json(ta, tone);
						refer_free(ta);
					}
					else goto Err;
					break;
			}
			if (output) r = scatterplot_save(sp, output);
			Err:
			if (dump) json_free(dump);
			refer_free(sp);
		}
	}
	else
	{
		Help:
		help(exec);
	}
	return r;
}

