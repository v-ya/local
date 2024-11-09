#ifndef _miko_std_api_syntaxor_h_
#define _miko_std_api_syntaxor_h_

#include <mlog.h>
#include "miko.std.type.syntaxor.h"

// miko.std.tstring

tparse_tstring_s* miko_std_tstring_create_number(void);

// miko.std.syntaxor

miko_std_syntaxor_s* miko_std_syntaxor_alloc(mlog_s *restrict mlog);
miko_index_t miko_std_syntaxor_name2index(const miko_std_syntaxor_s *restrict r, const char *restrict id_name);
const miko_std_syntax_id_t* miko_std_syntaxor_name2id(const miko_std_syntaxor_s *restrict r, const char *restrict id_name);
const miko_std_syntax_id_t* miko_std_syntaxor_index2id(const miko_std_syntaxor_s *restrict r, miko_index_t id_index);
miko_index_t miko_std_syntaxor_add_none(miko_std_syntaxor_s *restrict r, const char *restrict id_name);
miko_index_t miko_std_syntaxor_add_keyword(miko_std_syntaxor_s *restrict r, const char *restrict id_name, miko_std_syntaxor_chars_t head, const char *const restrict keyword[]);
miko_index_t miko_std_syntaxor_add_tword(miko_std_syntaxor_s *restrict r, const char *restrict id_name, miko_std_syntaxor_chars_t head, miko_std_syntaxor_chars_t inner, miko_std_syntaxor_chars_t tail);
miko_index_t miko_std_syntaxor_add_tstring(miko_std_syntaxor_s *restrict r, const char *restrict id_name, miko_std_syntaxor_chars_t head, tparse_tstring_s* (*tstring_create_func)(void));
miko_std_syntaxor_s* miko_std_syntaxor_set_scope(miko_std_syntaxor_s *restrict r, const char *restrict id_name, const char *restrict syntax_start, const char *restrict syntax_stop, const char *restrict syntax_scope, uintptr_t must_match, const char *const restrict allow_syntax_name[]);
miko_std_syntaxor_s* miko_std_syntaxor_okay(miko_std_syntaxor_s *restrict r);
miko_std_syntax_s* miko_std_syntaxor_create(miko_std_syntaxor_s *restrict r, const miko_source_s *restrict source);

// miko.std.syntax

miko_std_syntax_s* miko_std_syntax_create_copy(const miko_std_syntax_s *restrict syntax);
const miko_std_syntax_s* miko_std_syntax_verify(const miko_std_syntax_s *restrict syntax, miko_index_t id_index, const char *restrict data_syntax);

// miko.std.syntax.reader

miko_std_syntax_reader_s* miko_std_syntax_reader_alloc(const miko_std_syntaxor_s *restrict syntaxor, miko_count_t syntax_slot);
miko_count_t miko_std_syntax_reader_slot_count(const miko_std_syntax_reader_s *restrict r);
miko_index_t miko_std_syntax_reader_now_group(const miko_std_syntax_reader_s *restrict r);
miko_index_t miko_std_syntax_reader_new_group(miko_std_syntax_reader_s *restrict r);
miko_index_t miko_std_syntax_reader_add_inode_by_nstring(miko_std_syntax_reader_s *restrict r, miko_index_t id_index, refer_nstring_t syntax, miko_offset_t slot);
miko_index_t miko_std_syntax_reader_add_inode_by_cstring(miko_std_syntax_reader_s *restrict r, miko_index_t id_index, const char *restrict syntax, miko_offset_t slot);
miko_index_t miko_std_syntax_reader_enable_inode_allow_ignore(miko_std_syntax_reader_s *restrict r);
miko_index_t miko_std_syntax_reader_enable_inode_must_finish(miko_std_syntax_reader_s *restrict r);
miko_index_t miko_std_syntax_reader_enable_inode_replace_scope(miko_std_syntax_reader_s *restrict r);
miko_index_t miko_std_syntax_reader_group_okaynext(miko_std_syntax_reader_s *restrict r, miko_index_t src_group_id, miko_index_t dst_group_id);
miko_index_t miko_std_syntax_reader_group_rollback(miko_std_syntax_reader_s *restrict r, miko_index_t src_group_id, miko_index_t target_group_id, miko_index_t depend_group_id);
miko_std_syntax_reader_s* miko_std_syntax_reader_finish(miko_std_syntax_reader_s *restrict r);
miko_std_syntax_reader_s* miko_std_syntax_reader_fetch(miko_std_syntax_reader_s *restrict r, miko_std_syntax_s *syntax_slot[], miko_count_t slot_count, vattr_vlist_t *restrict vlist_start, vattr_vlist_t *restrict vlist_stop, vattr_vlist_t *restrict *restrict vlist_rpos);

// miko.std.syntaxor.preset

miko_std_syntaxor_s* miko_std_syntaxor_create_spec(mlog_s *restrict mlog, miko_std_syntaxor_spec_t *restrict index);
miko_std_syntax_s* miko_std_syntaxor_marco_spec(miko_std_syntaxor_s *restrict syntaxor, const miko_std_syntaxor_spec_t *restrict index, miko_std_syntax_s *restrict syntax);

#endif
