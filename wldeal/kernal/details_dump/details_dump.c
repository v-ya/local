#define _DEFAULT_SOURCE
#include <math.h>
#include <wldeal.h>

typedef struct pri_s {
	const char *output;
	json_inode_t *array;
	size_t number;
	double basefre_last;
	double basefre_avg;
	double basefre_min;
	double basefre_max;
	double delta_bf_min;
	double delta_bf_max;
} pri_s;

static void pri_free_func(pri_s *restrict p)
{
	if (p->number)
	{
		printf(
			"[%lu] basefre: %.2f (%.2f, %.2f), delta: (%.3f, %.3f)\n",
			p->number, p->basefre_avg / p->number,
			p->basefre_min, p->basefre_max,
			p->delta_bf_min, p->delta_bf_max
		);
		if (p->output && p->array)
		{
			char *text;
			size_t n;
			if ((n = json_length(p->array)))
			{
				text = (char *) refer_alloc(n);
				if (text)
				{
					if (json_encode_intext(p->array, text))
					{
						FILE *fp;
						fp = fopen(p->output, "wb");
						if (fp)
						{
							fwrite(text, 1, n, fp);
							fclose(fp);
						}
					}
					refer_free(text);
				}
			}
		}
	}
	if (p->array) json_free(p->array);
}

pri_s* symbol_kernal_alloc(int argc, const char *argv[])
{
	pri_s *restrict p;
	p = (pri_s *) refer_alloz(sizeof(pri_s));
	if (p)
	{
		refer_set_free(p, (refer_free_f) pri_free_func);
		if (!argc) printf("... [output.json]\n");
		else p->output = argv[0];
		p->array = json_create_array();
		if (!p->array)
		{
			refer_free(p);
			p = NULL;
		}
	}
	return p;
}

void symbol_kernal_deal(pri_s *restrict p, uint32_t index, note_details_s *restrict nd, wavelike_kernal_arg_t *restrict arg)
{
	if (p)
	{
		register double dbf;
		register uint32_t i, n;
		json_inode_t *o, *v, *s;
		if (p->number)
		{
			dbf = arg->basefre;
			if (dbf < p->basefre_min) p->basefre_min = dbf;
			if (dbf > p->basefre_max) p->basefre_max = dbf;
			dbf -= p->basefre_last;
			if (dbf < p->delta_bf_min) p->delta_bf_min = dbf;
			if (dbf > p->delta_bf_max) p->delta_bf_max = dbf;
		}
		else p->basefre_last = p->basefre_min = p->basefre_max = arg->basefre;
		p->basefre_avg += arg->basefre;
		o = v = s = NULL;
		n = nd->used;
		dbf = arg->a;
		if (!(o = json_create_array())) goto label_skip;
		for (i = 0; i < n; ++i)
		{
			if (!json_array_set(o, i, v = json_create_floating(nd->saq[i].sa / dbf)))
				goto label_skip;
		}
		s = o;
		v = NULL;
		if (!(o = json_create_array())) goto label_skip;
		for (i = 0; i < n; ++i)
		{
			if (!json_array_set(o, i, v = json_create_floating(nd->saq[i].sq)))
				goto label_skip;
		}
		v = o;
		if (!(o = json_create_object())) goto label_skip;
		if (!json_object_set(o, "sa", s)) goto label_skip;
		s = NULL;
		if (!json_object_set(o, "sq", v)) goto label_skip;
		if (!json_object_set(o, "a", v = json_create_floating(arg->a))) goto label_skip;
		if (!json_object_set(o, "a", v = json_create_floating(arg->a))) goto label_skip;
		if (!json_object_set(o, "f", v = json_create_floating(arg->basefre))) goto label_skip;
		if (!json_object_set(o, "t", v = json_create_floating(arg->length))) goto label_skip;
		v = NULL;
		if (!json_array_set(p->array, p->number, o)) goto label_skip;
		++p->number;
		return ;
		label_skip:
		printf("skip ... %u\n", index);
		if (v) json_free(v);
		if (s) json_free(s);
		if (o) json_free(o);
	}
}
