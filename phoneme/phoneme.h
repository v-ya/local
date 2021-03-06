#ifndef _phoneme_h_
#define _phoneme_h_

#include "phoneme_type.h"
#include "phoneme_pool.h"
#include <note.h>

#define phoneme_envelope_func(_symbol, _pri)  double _symbol(_pri pri, register note_dyarg_t *restrict arg, refer_t *restrict data)
#define phoneme_basefre_func(_symbol, _pri)   double _symbol(_pri pri, register note_dyarg_t *restrict arg, refer_t *restrict data)
#define phoneme_details_func(_symbol, _pri)   void _symbol(_pri pri, register note_dyarg_t *restrict arg, register note_details_s *restrict d, refer_t *restrict data)
#define phoneme_arg2pri_func(_symbol, _pri)   _pri _symbol(json_inode_t *restrict arg)

typedef struct phoneme_src_t {
	phoneme_src_name_s name;
	phoneme_arg_s *arg;
	void *func;
	refer_t pri;
} phoneme_src_t;

struct phoneme_s {
	note_s *note;
	uint32_t details_used;
	uint32_t details_max;
	phoneme_src_t envelope;
	phoneme_src_t basefre;
	phoneme_src_t details[];
};

void phoneme_src_unset(phoneme_src_t *restrict ps);
phoneme_src_t* phoneme_src_set(phoneme_src_t *restrict ps, phoneme_pool_s *restrict pp, hashmap_t *restrict symlist, const char *restrict type, const char *restrict name);
phoneme_s* phoneme_alloc(uint32_t sdmax, phoneme_pool_s *restrict pp, json_inode_t *restrict o);
phoneme_s* phoneme_set_arg(phoneme_s *restrict p, phoneme_pool_s *restrict pp, json_inode_t *restrict o);
note_s* phoneme_update(phoneme_s *restrict p, phoneme_pool_s *restrict pp, uint32_t dmax);
phoneme_s* phoneme_modify(phoneme_s *restrict p, phoneme_pool_s *restrict pp, const char *restrict *restrict modify, uint32_t sdmax);

#endif
