#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json.h>
#include <wav.h>
#include <note_details.h>

static uint32_t qzero;

static uint32_t get_frames(json_inode_t *restrict array, uint32_t sampfre, double *a_max)
{
	json_inode_t *o, *v;
	uint32_t i, n, r;
	n = array->value.array.number;
	for (i = r = 0; i < n; ++i)
	{
		o = json_array_find(array, i);
		if (o && o->type == json_inode_object)
		{
			v = json_object_find(o, "t");
			if (v && v->type == json_inode_floating)
				r += (uint32_t) (v->value.floating * sampfre + 0.5);
			if (a_max && (v = json_object_find(o, "a")) && v && v->type == json_inode_floating &&
				v->value.floating > *a_max) *a_max = v->value.floating;
		}
	}
	return r;
}

static void set_frames(double *data, uint32_t frames, json_inode_t *restrict array, uint32_t sampfre, double ak)
{
	json_inode_t *o, *v, *sa, *sq;
	note_details_s *nd;
	double loudness;
	uint32_t i, n, j, m, df;
	nd = note_details_alloc(128);
	if (nd)
	{
		n = array->value.array.number;
		for (i = 0; i < n; ++i)
		{
			o = json_array_find(array, i);
			if (!o || o->type != json_inode_object)
				continue;
			v = json_object_find(o, "t");
			if (!v || v->type != json_inode_floating)
				continue;
			df = (uint32_t) (v->value.floating * sampfre + 0.5);
			if (df > frames) break;
			v = json_object_find(o, "a");
			if (!v || v->type != json_inode_floating)
				continue;
			loudness = v->value.floating * ak;
			sa = json_object_find(o, "sa");
			sq = json_object_find(o, "sq");
			if (sa && sq && sa->type == json_inode_array && sq->type == json_inode_array)
			{
				m = sa->value.array.number;
				if (m > sq->value.array.number) m = sq->value.array.number;
				if (m > nd->max) m = nd->max;
				nd->used = m;
				if (qzero) for (j = 0; j < m; ++j)
				{
					v = json_array_find(sa, j);
					if (v && v->type == json_inode_floating)
						nd->saq[j].sa = v->value.floating;
					else nd->saq[j].sa = 0;
				}
				else for (j = 0; j < m; ++j)
				{
					v = json_array_find(sa, j);
					if (v && v->type == json_inode_floating)
						nd->saq[j].sa = v->value.floating;
					else nd->saq[j].sa = 0;
					v = json_array_find(sq, j);
					if (v && v->type == json_inode_floating)
						nd->saq[j].sq = v->value.floating;
					else nd->saq[j].sq = 0;
				}
				note_details_gen(data, frames, nd, df, loudness, 0);
				data += df;
				frames -= df;
			}
		}
		refer_free(nd);
	}
}

int main(int argc, const char *argv[])
{
	if (argc == 3 || (argc == 4 && !strcmp(argv[3], "-qzero")))
	{
		json_inode_t *restrict array;
		double *v, amax;
		uint32_t sampfre, frames;
		qzero = (argc == 4);
		array = json_load(argv[1]);
		if (array && array->type == json_inode_array)
		{
			sampfre = 96000;
			amax = 0;
			frames = get_frames(array, sampfre, &amax);
			if (frames)
			{
				v = (double *) calloc(sizeof(double), frames);
				if (v)
				{
					set_frames(v, frames, array, sampfre, 0.8 / amax);
					if (wav_save_double(argv[2], &v, frames, 1, sampfre))
						printf("save [%s] fail\n", argv[2]);
					free(v);
				}
			}
			else printf("[%s] frames = 0\n", argv[1]);
			json_free(array);
		}
		else printf("[%s] don't load or not array json\n", argv[1]);
	}
	else printf("%s <details.json> <output.wav> [-qzero]\n", argv[0]);
	return 0;
}
