#include "phoneme_type.h"
#include <string.h>

void phoneme_hashmap_free_refer_func(register hashmap_vlist_t *restrict vl)
{
	if (vl) refer_free(vl->value);
}

phoneme_src_name_s* phoneme_src_name_dump(const char *restrict s)
{
	phoneme_src_name_s *r;
	size_t n;
	r = NULL;
	if (s)
	{
		n = strlen(s) + 1;
		r = (phoneme_src_name_s *) refer_alloc(n);
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

