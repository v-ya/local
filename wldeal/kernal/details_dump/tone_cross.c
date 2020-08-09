#include <json.h>
#include <scatterplot.h>
#include <memory.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

typedef struct tone_vec_t {
	size_t number;
	double value[];
} tone_vec_t;

static tone_vec_t* tone_vec_alloc(json_inode_t *restrict array)
{
	register size_t i, n;
	if (array && array->type == json_inode_array && (n = array->value.array.number))
	{
		register tone_vec_t *restrict r;
		register json_inode_t *restrict v;
		r = (tone_vec_t *) malloc(sizeof(tone_vec_t) + n * sizeof(double));
		r->number = n;
		for (i = 0; i < n; ++i)
		{
			v = json_array_find(array, i);
			switch (v?v->type:json_inode_null)
			{
				case json_inode_integer:
					r->value[i] = v->value.integer;
					break;
				case json_inode_floating:
					r->value[i] = v->value.floating;
					break;
				default:
					r->value[i] = 0;
					break;
			}
		}
		return r;
	}
	return NULL;
}

static tone_vec_t* tone_vec_alloc_by_number(size_t number)
{
	register tone_vec_t *restrict r;
	r = NULL;
	if (number)
	{
		r = (tone_vec_t *) calloc(1, sizeof(tone_vec_t) + number * sizeof(double));
		if (r) r->number = number;
	}
	return r;
}

static inline double tone_vec_get(register tone_vec_t *restrict v, register double x)
{
	if (x > 0)
	{
		register size_t i;
		i = (size_t) (x *= v->number);
		if (i + 1 < v->number)
		{
			x -= i;
			return v->value[i] * (1 - x) + v->value[i + 1] * x;
		}
	}
	return 0;
}

static tone_vec_t* tone_vec_load(const char *restrict path)
{
	json_inode_t *restrict array;
	tone_vec_t *restrict r;
	r = NULL;
	array = json_load(path);
	if (array)
	{
		r = tone_vec_alloc(array);
		json_free(array);
	}
	return r;
}

static void tone_vec_store(register tone_vec_t *restrict tv, const char *restrict path)
{
	json_inode_t *array, *v;
	register size_t i, n;
	n = tv->number;
	array = json_create_array();
	v = NULL;
	if (array)
	{
		for (i = 0; i < n; ++i)
		{
			if (!json_array_set(array, i, v = json_create_floating(tv->value[i])))
				goto Err;
		}
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

static void tv_sp(register tone_vec_t *restrict v, const char *restrict path, double limit, register double g)
{
	register scatterplot_s *restrict sp;
	register double x;
	sp = scatterplot_alloc();
	if (sp)
	{
		g /= 20000;
		for (x = 0; x < 1.0; x += g)
		{
			scatterplot_pos(sp, x, tone_vec_get(v, x), 0xff0000);
		}
		for (x = 0; x < 1.0; x += g)
		{
			scatterplot_pos(sp, x, 0, 0x0000ff);
		}
		scatterplot_set_image(sp, 2048, 1024, 0, 0);
		scatterplot_set_view(sp, 0, -limit, 1, limit);
		scatterplot_save(sp, path);
		refer_free(sp);
	}
}

static void tv_isp(register tone_vec_t *restrict v, const char *restrict path, double limit, register double g)
{
	register scatterplot_s *restrict sp;
	register double x, l, integral;
	sp = scatterplot_alloc();
	if (sp)
	{
		l = g;
		g /= 20000;
		integral = 0;
		for (x = 0; x < 1.0; x += g)
		{
			integral += tone_vec_get(v, x) * l;
			scatterplot_pos(sp, x, integral, 0xff0000);
		}
		for (x = 0; x < 1.0; x += g)
		{
			scatterplot_pos(sp, x, 0, 0x0000ff);
		}
		scatterplot_set_image(sp, 2048, 1024, 0, 0);
		scatterplot_set_view(sp, 0, -limit, 1, limit);
		scatterplot_save(sp, path);
		refer_free(sp);
	}
}

static const char *exec_name;
static void help(void);

static int exec_sp(int argc, const char *argv[])
{
	if (argc == 4)
	{
		double limit;
		double g;
		limit = atof(argv[2]);
		g = atof(argv[3]);
		if (limit > 0 && g > 0)
		{
			tone_vec_t *tv;
			tv = tone_vec_load(argv[1]);
			if (tv)
			{
				tv_sp(tv, argv[0], limit, g);
				free(tv);
				return 0;
			}
		}
	}
	else help();
	return -1;
}

static int exec_isp(int argc, const char *argv[])
{
	if (argc == 4)
	{
		double limit;
		double g;
		limit = atof(argv[2]);
		g = atof(argv[3]);
		if (limit > 0 && g > 0)
		{
			tone_vec_t *tv;
			tv = tone_vec_load(argv[1]);
			if (tv)
			{
				tv_isp(tv, argv[0], limit, g);
				free(tv);
				return 0;
			}
		}
	}
	else help();
	return -1;
}

static int exec_tr(int argc, const char *argv[])
{
	const char *restrict tr, *restrict tone;
	intptr_t n;
	if (argc == 3)
	{
		tr = argv[0];
		tone = argv[1];
		n = atol(argv[2]);
		if (n > 0)
		{
			register tone_vec_t *restrict v;
			v = tone_vec_load(tone);
			if (v)
			{
				register tone_vec_t *restrict r;
				r = tone_vec_alloc_by_number(n);
				if (r)
				{
					register size_t i, n;
					for (i = 0, n = r->number; i < n; ++i)
						r->value[i] = tone_vec_get(v, (double) i / n);
					tone_vec_store(r, tr);
					free(r);
					free(v);
					return 0;
				}
				free(v);
			}
		}
	}
	else help();
	return -1;
}

static int exec_sub(int argc, const char *argv[])
{
	const char *restrict sub, *restrict t1, *restrict t2;
	register size_t i, n;
	register tone_vec_t *restrict r, *restrict v1, *restrict v2;
	int ret;
	ret = -1;
	if (argc == 3)
	{
		sub = argv[0];
		t1 = argv[1];
		t2 = argv[2];
		v1 = tone_vec_load(t1);
		v2 = tone_vec_load(t2);
		if (v1 && v2 && (n = v1->number) == v2->number)
		{
			r = tone_vec_alloc_by_number(n);
			if (r)
			{
				ret = 0;
				for (i = 0; i < n; ++i)
					r->value[i] = v1->value[i] - v2->value[i];
				tone_vec_store(r, sub);
				free(r);
			}
		}
		if (v1) free(v1);
		if (v2) free(v2);
	}
	else help();
	return ret;
}

static int exec_avg(int argc, const char *argv[])
{
	const char *restrict avg;
	register size_t i, n;
	register tone_vec_t *restrict r, *restrict v;
	unsigned int number;
	int ret;
	ret = -1;
	if (argc > 1)
	{
		avg = argv[0];
		--argc;
		++argv;
		r = v = NULL;
		n = 0;
		number = 0;
		while (argc)
		{
			v = tone_vec_load(argv[0]);
			if (!v) goto Err;
			if (!r)
			{
				r = tone_vec_alloc_by_number(n = v->number);
				if (!r) goto Err;
			}
			else if (r->number != n) goto Err;
			for (i = 0; i < n; ++i)
				r->value[i] += v->value[i];
			free(v);
			v = NULL;
			++number;
			--argc;
			++argv;
		}
		for (i = 0; i < n; ++i)
			r->value[i] /= number;
		if (r && avg) tone_vec_store(r, avg);
		Err:
		if (r) free(r);
		if (v) free(v);
	}
	else help();
	return ret;
}

static int (*get_exec_func(const char *restrict type))(int argc, const char *argv[])
{
	int (*r)(int argc, const char *argv[]);
	r = NULL;
	if (!type) goto label_miss;
	else if (!strcmp(type, "sp")) r = exec_sp;
	else if (!strcmp(type, "isp")) r = exec_isp;
	else if (!strcmp(type, "tr")) r = exec_tr;
	else if (!strcmp(type, "sub")) r = exec_sub;
	else if (!strcmp(type, "avg")) r = exec_avg;
	label_miss:
	return r;
}

static void help(void)
{
	printf(
		"%s <type> [ ... ]\n"
		"\t" "sp  <output.bmp> <tone.json> <y-limit> <x-g>\n"
		"\t" "isp <output.bmp> <tone.json> <y-limit> <x-g>\n"
		"\t" "tr  <tr-tone.json> <tone.json> <n-split>\n"
		"\t" "sub <sub-tone.json> <t1.json> <t2.json>\n"
		"\t" "avg <avg-tone.json> <tone.json> ...\n"
		, exec_name
	);
}

int main(int argc, const char *argv[])
{
	int (*exec)(int argc, const char *argv[]);
	exec_name = argv[0];
	if (argc > 1)
	{
		exec = get_exec_func(argv[1]);
		if (exec) return exec(argc - 2, argv + 2);
	}
	help();
	return -1;
}
