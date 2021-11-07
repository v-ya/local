#include <pocket-builder-kiya.h>
#include <cparse.h>
#include <hashmap.h>
#include <mlog.h>

extern mlog_s *$mlog;

static const char* scope_get_func_check(vattr_vlist_t *it, vattr_vlist_t *b, vattr_vlist_t *e)
{
	vattr_vlist_t *name;
	uintptr_t depth;
	// get name
	name = vattr_last(it);
	if (name == b || ((cparse_value_s *) name->value)->type != cparse_value$key)
		goto label_fail;
	it = vattr_next(it);
	// find ')'
	if (it == e)
		goto label_fail;
	if (*it->vslot->key == '*')
		goto label_fail;
	depth = 0;
	while (it != e)
	{
		const char *restrict k;
		cparse_value_type_t t;
		k = it->vslot->key;
		t = ((cparse_value_s *) it->value)->type;
		if (t == cparse_value$operator)
		{
			if (*k == ')')
			{
				if (!depth)
					break;
				else --depth;
			}
			else if (*k == '(')
				++depth;
			else if (*k == ';')
				goto label_fail;
		}
		else if (t != cparse_value$key && t != cparse_value$number)
			goto label_fail;
		it = vattr_next(it);
	}
	if (it == e)
		goto label_fail;
	// check ';'
	if ((it = vattr_next(it)) == e)
		goto label_fail;
	if (*it->vslot->key == ';' ||
		((cparse_value_s *) it->value)->type == cparse_value$key)
		return name->vslot->key;
	label_fail:
	return NULL;
}

static cparse_scope_s* scope_get_func(cparse_scope_s *scope, pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index)
{
	hashmap_t name_pool;
	cparse_scope_s *status;
	const char *path[2];
	status = NULL;
	path[1] = NULL;
	if (hashmap_init(&name_pool))
	{
		vattr_vslot_t *slot;
		if ((slot = vattr_get_vslot(scope->scope, "(")))
		{
			vattr_vlist_t *b, *e;
			vattr_vlist_t *it, *it_end;
			b = vattr_last_end(scope->scope);
			e = vattr_next_end(scope->scope);
			it_end = vattr_vslot_next_end(slot);
			for (it = vattr_vslot_first(slot); it != it_end; it = vattr_vslot_next(it))
			{
				const char *name;
				if ((name = scope_get_func_check(it, b, e)) && !hashmap_find_name(&name_pool, name))
				{
					if (!hashmap_set_name(&name_pool, name, NULL, NULL))
						goto label_fail;
					path[0] = name;
					if (!pocket_saver_create_null(saver, index, path))
						goto label_fail;
				}
			}
		}
		status = scope;
		label_fail:
		hashmap_uini(&name_pool);
	}
	return status;
}

static cparse_inst_s *inst;

const char* initial(uintptr_t argc, const char *argv[])
{
	if ((inst = cparse_inst_alloc()))
		return NULL;
	return "cparse_inst_alloc fail\n";
}

void finally(void)
{
	if (inst)
		refer_free(inst);
	inst = NULL;
}

pocket_saver_index_t* kmain(pocket_saver_s *restrict saver, pocket_saver_index_t *restrict index, uintptr_t argc, const char *const argv[])
{
	if (argc == 2)
	{
		cparse_scope_s *scope;
		scope = cparse_scope_load(inst, argv[1]);
		if (scope)
		{
			// delete comment
			vattr_delete(scope->scope, "//");
			vattr_delete(scope->scope, "/*");
			// scan api to saver
			if (!scope_get_func(scope, saver, index))
			{
				mlog_printf($mlog, "%s: (%s) insert api to saver fail\n", argv[0], argv[1]);
				index = NULL;
			}
			refer_free(scope);
			return index;
		}
		else mlog_printf($mlog, "%s: cparse load (%s) fail\n", argv[0], argv[1]);
	}
	else mlog_printf($mlog, "%s <c-header path>\n", argv[0]);
	return NULL;
}
