#ifndef _phoneme_script_h_
#define _phoneme_script_h_

#include "phoneme_type.h"
#include "phoneme_output.h"
#include "phoneme_pool.h"
#include "phoneme.h"
#include <mlog.h>

typedef int (*phoneme_script_sysfunc_f)(dylink_pool_t *restrict dyp, void *pri);

typedef struct phoneme_script_s {
	phoneme_pool_s *phoneme_pool;
	mlog_s *mlog;
	hashmap_t package;
	hashmap_t script;
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
	uint32_t vstack_script_max;
	uint32_t vstack_script_this;
	uint32_t vstack_phoneme_max;
	uint32_t vstack_phoneme_this;
} phoneme_script_s;

typedef const char* (*phoneme_script_keyword_f)(const char *restrict *restrict pos, phoneme_s *restrict phoneme, phoneme_script_s *restrict phoneme_script, phoneme_output_s *phoneme_output);

#define phoneme_script_default_base_time           0.4054
#define phoneme_script_default_base_volume         0.5
#define phoneme_script_default_base_fre_line       440
#define phoneme_script_default_base_fre_step       12
#define phoneme_script_default_space_time          1
#define phoneme_script_default_sdmax               8
#define phoneme_script_default_dmax                32
#define phoneme_script_default_vstack_script_max   16
#define phoneme_script_default_vstack_phoneme_max  16

phoneme_script_s* phoneme_script_alloc(size_t xmsize, mlog_s *restrict mlog, phoneme_script_sysfunc_f sysfunc, void *pri);
phoneme_src_name_s phoneme_script_set_core_path(phoneme_script_s *restrict ps, const char *restrict path);
phoneme_src_name_s phoneme_script_set_package_path(phoneme_script_s *restrict ps, const char *restrict path);
phoneme_script_s* phoneme_script_load_core(phoneme_script_s *restrict ps, const char *restrict core);
phoneme_script_s* phoneme_script_load_package(phoneme_script_s *restrict ps, const char *restrict package);
phoneme_script_s* phoneme_script_load_package_json(phoneme_script_s *restrict ps, json_inode_t *restrict package);
phoneme_s* phoneme_script_get_phoneme(phoneme_script_s *restrict ps, const char *restrict *restrict script);
phoneme_output_s* phoneme_script_load(phoneme_script_s *restrict ps, const char *restrict script_path, phoneme_output_s *restrict po);
phoneme_output_s* phoneme_script_run(phoneme_script_s *restrict ps, const char *restrict script, phoneme_output_s *restrict po);

#endif
