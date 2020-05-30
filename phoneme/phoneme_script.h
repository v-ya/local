#ifndef _phoneme_script_h_
#define _phoneme_script_h_

#include "phoneme_type.h"
#include "phoneme_buffer.h"
#include "phoneme_pool.h"
#include "phoneme.h"

typedef int (*phoneme_script_sysfunc_f)(dylink_pool_t *restrict dyp, void *pri);

typedef struct phoneme_script_s {
	phoneme_pool_s *phoneme_pool;
	hashmap_t package;
	phoneme_src_name_s core_path;
	phoneme_src_name_s package_path;
	double base_time;
	double base_volume;
	double base_fre_line;
	double base_fre_step;
	double space_time;
	double curr_pos;
	double last_pos;
	uint32_t sdmax;
	uint32_t dmax;
} phoneme_script_s;

phoneme_script_s* phoneme_script_alloc(size_t xmsize, phoneme_script_sysfunc_f sysfunc, void *pri);
phoneme_src_name_s phoneme_script_set_core_path(phoneme_script_s *restrict ps, const char *restrict path);
phoneme_src_name_s phoneme_script_set_package_path(phoneme_script_s *restrict ps, const char *restrict path);
phoneme_script_s* phoneme_script_load_core(phoneme_script_s *restrict ps, const char *restrict core);
phoneme_script_s* phoneme_script_load_package(phoneme_script_s *restrict ps, const char *restrict package);
phoneme_script_s* phoneme_script_load_package_json(phoneme_script_s *restrict ps, json_inode_t *restrict package);
phoneme_s* phoneme_script_get_phoneme(phoneme_script_s *restrict ps, const char *restrict *restrict script);
phoneme_buffer_s* phoneme_script_load(phoneme_script_s *restrict ps, const char *restrict script_path, phoneme_buffer_s *restrict pb);
phoneme_buffer_s* phoneme_script_run(phoneme_script_s *restrict ps, const char *restrict script, phoneme_buffer_s *restrict pb);

#endif
