#include "phoneme_type.h"
#include <string.h>

uint8_t phoneme_alpha_table_space[256] = {
	[' '] = 1,
	['\t'] = 1,
	['\r'] = 1,
	['\n'] = 1
};

void phoneme_hashmap_free_refer_func(register hashmap_vlist_t *restrict vl)
{
	if (vl->value) refer_free(vl->value);
}

phoneme_src_name_s phoneme_src_name_dump(const char *restrict s)
{
	phoneme_src_name_s r;
	size_t n;
	r = NULL;
	if (s)
	{
		n = strlen(s) + 1;
		r = (phoneme_src_name_s) refer_alloc(n);
		if (r) memcpy((void *) r, s, n);
	}
	return r;
}

static void phoneme_arg_free_func(register phoneme_arg_s *restrict arg)
{
	if (*arg) json_free(*arg);
}

phoneme_arg_s* phoneme_arg_alloc(json_inode_t *restrict j)
{
	register phoneme_arg_s *restrict r;
	r = (phoneme_arg_s *) refer_alloc(sizeof(phoneme_arg_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) phoneme_arg_free_func);
		*r = j;
	}
	return r;
}

phoneme_arg_s* phoneme_arg_dump(phoneme_arg_s *restrict a)
{
	register phoneme_arg_s *restrict r;
	r = (phoneme_arg_s *) refer_alloz(sizeof(phoneme_arg_s));
	if (r)
	{
		refer_set_free(r, (refer_free_f) phoneme_arg_free_func);
		if (*a && !(*r = json_copy(*a)))
		{
			refer_free(r);
			r = NULL;
		}
	}
	return r;
}

char* phoneme_read_string(char *restrict buffer, size_t n, register const char **restrict ps, const char *restrict endc)
{
	const char *restrict t;
	size_t nn;
	if ((t = strpbrk(*ps, endc))) nn = (size_t)(uintptr_t)(t - *ps);
	else nn = strlen(*ps);
	if (nn >= n) goto Err;
	if (nn) memcpy(buffer, *ps, nn);
	buffer[nn] = 0;
	*ps += nn;
	return buffer;
	Err:
	*ps += nn;
	return NULL;
}
