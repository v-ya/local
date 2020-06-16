#define _DEFAULT_SOURCE
#include <phoneme/phoneme.h>
#include "get_float.inc"

static dyl_used double seqence(register size_t n, register double *restrict s, register double *restrict v, register double t)
{
	if (t < *s) goto other;
	while (--n && *++s < t) ++v;
	if (t >= *s) goto other;
	--s;
	if (s[0] == s[1]) goto other;
	t = (t - *s) / (s[1] - s[0]);
	return v[0] * (1 - t) + v[1] * t;
	other:
	return *v;
}
dyl_export(seqence, vya.common.seqence);

static dyl_used void seqence_arg(register size_t n, register double *restrict s, register double *restrict v, register json_inode_t *arg)
{
	register size_t i;
	json_inode_t *a;
	double ts, tv, k;
	ts = tv = k = 0;
	if (n > 1) k = 1.0 / (n - 1);
	for (i = 0; i < n; ++i)
	{
		a = json_array_find(arg, i);
		if (a->type == json_inode_object)
		{
			*s = ts;
			*v = tv;
			get_float(s, a, ".s");
			get_float(v, a, ".v");
		}
		else
		{
			*s = k * i;
			*v = tv;
			if (a->type == json_inode_floating) *v = a->value.floating;
			else if (a->type == json_inode_integer) *v = a->value.integer;
		}
		if (*s < ts) *s = ts;
		ts = *s++;
		tv = *v++;
	}
}
dyl_export(seqence_arg, vya.common.seqence_arg);
