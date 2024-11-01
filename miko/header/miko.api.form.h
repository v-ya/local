#ifndef _miko_api_form_h_
#define _miko_api_form_h_

#include "miko.type.base.h"
#include "miko.type.form.h"

miko_form_w* miko_form_alloc(miko_wink_gomi_s *restrict gomi, miko_count_t max_count);
miko_form_w* miko_form_set_limit(miko_form_w *restrict r, miko_count_t max_count);
miko_form_w* miko_form_push_zero(miko_form_w *restrict r, miko_major_vtype_t vtype, miko_count_t count);
miko_form_w* miko_form_push_copy(miko_form_w *restrict r, const miko_form_t *restrict array, miko_count_t count);
void miko_form_pop(miko_form_w *restrict r, miko_count_t count);
void miko_form_keep_count(miko_form_w *restrict r, miko_count_t count);

miko_form_table_s* miko_form_table_alloc(miko_count_t count);

#endif
