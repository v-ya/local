#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include <memory.h>

typedef struct seqarray_s {
	double k;
	double start;
	size_t number;
	double value[];
} seqarray_s;

static dyl_used double seqarray(register seqarray_s *restrict sa, register double x)
{
	register double r;
	register size_t index, n;
	if ((x -= sa->start) > 0 && (n = sa->number) > 1)
	{
		index = (size_t) (x *= sa->k);
		if (index + 1 < n)
		{
			x -= index;
			r = sa->value[index] * (1 - x) + sa->value[index + 1] * x;
		}
		else r = sa->value[n - 1];
	}
	else r = sa->value[0];
	return r;
}
dyl_export(seqarray, vya.common.seqarray);

static dyl_used seqarray_s* seqarray_alloc(register double *restrict value, size_t number, double start, double length)
{
	register seqarray_s *restrict r;
	r = NULL;
	if (number && length)
	{
		r = (seqarray_s *) refer_alloc(sizeof(seqarray_s) + sizeof(double) * number);
		if (r)
		{
			r->k = number / length;
			r->start = start;
			r->number = number;
			memcpy(r->value, value, sizeof(double) * number);
		}
	}
	return r;
}
dyl_export(seqarray_alloc, vya.common.seqarray_alloc);

static dyl_used seqarray_s* seqarray_arg(register json_inode_t *arg, double start, double length)
{
	register json_inode_t *v;
	register seqarray_s *restrict r;
	register size_t i, n;
	r = NULL;
	if (arg && arg->type == json_inode_array && (n = arg->value.array.number) && length)
	{
		r = (seqarray_s *) refer_alloc(sizeof(seqarray_s) + sizeof(double) * n);
		if (r)
		{
			r->k = n / length;
			r->start = start;
			r->number = n;
			for (i = 0; i < n; ++i)
			{
				v = json_array_find(arg, i);
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
		}
	}
	return r;
}
dyl_export(seqarray_arg, vya.common.seqarray_arg);
