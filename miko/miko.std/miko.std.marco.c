#include "miko.std.marco.h"
#include "miko.std.syntaxor.h"

static refer_nstring_t miko_std_marco_name_initial_func(refer_nstring_t *restrict v)
{
	return (refer_nstring_t) refer_save(*v);
}
static void miko_std_marco_name_finally_func(refer_nstring_t *restrict v)
{
	refer_ck_free(*v);
}

static miko_vector_s_t miko_std_marco_create_name(const miko_std_marco_index_t *restrict index, const miko_std_syntax_s *restrict param)
{
	miko_vector_s_t name;
	const miko_std_syntax_s *restrict v;
	vattr_vlist_t *restrict vlist;
	if (param->data.scope && (name = miko_vector_alloc(sizeof(refer_nstring_t),
		(miko_vector_initial_f) miko_std_marco_name_initial_func,
		(miko_vector_finally_f) miko_std_marco_name_finally_func)))
	{
		for (vlist = param->data.scope->vattr; vlist; vlist = vlist->vattr_next)
		{
			v = (const miko_std_syntax_s *) vlist->value;
			if (v->id.index != index->name)
				goto label_fail;
			if (!miko_vector_push(name, &v->data.syntax, 1))
				goto label_fail;
			if (!(vlist = vlist->vattr_next))
				break;
			v = (const miko_std_syntax_s *) vlist->value;
			if (v->id.index != index->comma)
				goto label_fail;
			if (!vlist->vattr_next)
				goto label_fail;
		}
		return name;
		label_fail:
		refer_free(name);
	}
	return NULL;
}

static void miko_std_marco_free_func(miko_std_marco_s *restrict r)
{
	refer_ck_free(r->name_cache);
	refer_ck_free(r->marco);
}

miko_std_marco_s* miko_std_marco_alloc(const miko_std_marco_index_t *restrict index, const miko_std_syntax_s *restrict param, const miko_std_marco_scope_s *restrict marco)
{
	miko_std_marco_s *restrict r;
	if (marco && (r = (miko_std_marco_s *) refer_alloz(sizeof(miko_std_marco_s))))
	{
		refer_hook_free(r, marco);
		if ((!param || (r->name_cache = miko_std_marco_create_name(index, param))) &&
			(r->marco = (const miko_std_marco_scope_s *) refer_save(marco)))
		{
			if (r->name_cache)
				r->name_array = miko_vector_map(r->name_cache, 0, miko_vector_auto, &r->name_count);
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static vattr_vlist_t* miko_std_marco_push_args(vattr_s *restrict args, const char *restrict name, vattr_vlist_t *restrict start, vattr_vlist_t *restrict stop)
{
	vattr_s *restrict argv;
	vattr_vlist_t *result;
	result = NULL;
	if ((argv = vattr_alloc()))
	{
		if (miko_std_syntax_scope_append_vlist_copy(argv, start, stop))
			result = vattr_insert_tail(args, name, argv);
		refer_free(argv);
	}
	return result;
}

miko_std_marco_args_s* miko_std_marco_create_args(const miko_std_marco_s *restrict r, const miko_std_marco_index_t *restrict index, const miko_std_syntax_s *restrict param)
{
	const refer_nstring_t *restrict p;
	vattr_vlist_t *restrict vlist;
	vattr_vlist_t *restrict comma;
	vattr_s *restrict args;
	uintptr_t i, n;
	if (param && param->data.scope && param->id.index == index->param &&
		r->name_cache && (args = vattr_alloc()))
	{
		p = r->name_array;
		n = r->name_count;
		vlist = param->data.scope->vattr;
		for (i = 0; i < n && vlist; ++i)
		{
			comma = miko_std_syntax_scope_find_id(vlist, NULL, index->comma);
			if (!miko_std_marco_push_args(args, p[i]->string, vlist, comma))
				goto label_fail;
			vlist = comma?comma->vattr_next:comma;
		}
		if (vlist && !miko_std_marco_push_args(args, "__va_args__", vlist, NULL))
			goto label_fail;
		return args;
		label_fail:
		refer_free(args);
	}
	return NULL;
}

miko_std_marco_scope_s* miko_std_marco_create_scope(const miko_std_marco_s *restrict r)
{
	vattr_s *restrict scope;
	if ((scope = vattr_alloc()))
	{
		if (miko_std_syntax_scope_append_vlist_copy(scope, r->marco->vattr, NULL))
			return scope;
		refer_free(scope);
	}
	return NULL;
}

void miko_std_marco_print(mlog_s *restrict mlog, const miko_source_s *restrict source, uintptr_t pos)
{
	miko_log_s *restrict log;
	miko_log_into_t into;
	if (mlog && (log = miko_log_alloc()))
	{
		into = miko_log_into_name(log, miko_std_error__marco);
		miko_log_add_source(log, miko_log_level__error, source, pos);
		miko_log_back(log, into);
		miko_log_print(log, mlog);
		refer_free(log);
	}
}
