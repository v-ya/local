#include <cparse.h>
#include <exbuffer.h>
#include <fsys.h>
#include <tparse/tstack.h>
#include <string.h>

typedef struct param_list_t param_list_t;
struct param_list_t {
	param_list_t *next;
	const char *full;
	const char *name;
	uintptr_t n_full;
	uintptr_t n_name;
};

static exbuffer_t* write_file(exbuffer_t *restrict inner, const char *restrict path)
{
	exbuffer_t *r;
	fsys_file_s *restrict fp;
	uintptr_t rsize;
	r = NULL;
	if ((fp = fsys_file_alloc(path, fsys_file_flag_write | fsys_file_flag_create | fsys_file_flag_truncate)))
	{
		if (fsys_file_write(fp, inner->data, inner->used, &rsize) && rsize == inner->used)
			r = inner;
		refer_free(fp);
	}
	return r;
}

static exbuffer_t* build_header_begin(exbuffer_t *restrict header)
{
	static const char code[] = {
		"// this c-header file is auto generate\n"
		"#ifndef _elfin_item_h_\n"
		"#define _elfin_item_h_\n"
		"\n"
		"#include \"elfin.h\"\n"
		"\n"
	};
	if (exbuffer_append(header, code, sizeof(code) - 1))
		return header;
	return NULL;
}

static exbuffer_t* build_header_end(exbuffer_t *restrict header)
{
	static const char code[] = {
		"\n"
		"#endif\n"
		"\n"
	};
	if (exbuffer_append(header, code, sizeof(code) - 1))
		return header;
	return NULL;
}

static exbuffer_t* build_source_begin(exbuffer_t *restrict source)
{
	static const char code[] = {
		"// this c-source file is auto generate\n"
		"#include \"elfin.item.h\"\n"
		"#include \"inner.item.h\"\n"
		"\n"
		"#define auto_proxy_call(_id, ...)  \\\n"
		"\t" "const struct elfin_item_inst_s *restrict inst;\\\n"
		"\t" "inst = item->inst;\\\n"
		"\t" "if (inst->func._id) return inst->func._id(item, ##__VA_ARGS__);\\\n"
		"\t" "return NULL\n"
		"\n"
	};
	if (exbuffer_append(source, code, sizeof(code) - 1))
		return source;
	return NULL;
}

static exbuffer_t* build_source_end(exbuffer_t *restrict source)
{
	static const char code[] = {
		"#undef auto_proxy_call\n"
		"\n"
	};
	if (exbuffer_append(source, code, sizeof(code) - 1))
		return source;
	return NULL;
}

static exbuffer_t* build_func_define_inner(exbuffer_t *restrict inner, const char *restrict id, const param_list_t *restrict list)
{
	static const char s1[] = "elfin_item_s* elfin_item__";
	static const char s2[] = "(elfin_item_s *restrict item";
	if (!exbuffer_append(inner, s1, sizeof(s1) - 1))
		goto label_fail;
	if (!exbuffer_append(inner, id, strlen(id)))
		goto label_fail;
	if (!exbuffer_append(inner, s2, sizeof(s2) - 1))
		goto label_fail;
	while (list)
	{
		if (!exbuffer_append(inner, ", ", 2))
			goto label_fail;
		if (!exbuffer_append(inner, list->full, list->n_full))
			goto label_fail;
		list = list->next;
	}
	if (!exbuffer_append(inner, ")", 1))
		goto label_fail;
	return inner;
	label_fail:
	return NULL;
}

static exbuffer_t* build_header_inner(exbuffer_t *restrict header, const char *restrict id, const param_list_t *restrict list)
{
	if (build_func_define_inner(header, id, list) &&
		exbuffer_append(header, ";\n", 2))
		return header;
	return NULL;
}

static exbuffer_t* build_source_inner(exbuffer_t *restrict source, const char *restrict id, const param_list_t *restrict list)
{
	static const char s1[] = "\n{\n\tauto_proxy_call(";
	static const char s2[] = ");\n}\n\n";
	if (!build_func_define_inner(source, id, list))
		goto label_fail;
	if (!exbuffer_append(source, s1, sizeof(s1) - 1))
		goto label_fail;
	if (!exbuffer_append(source, id, strlen(id)))
		goto label_fail;
	while (list)
	{
		if (!exbuffer_append(source, ", ", 2))
			goto label_fail;
		if (!exbuffer_append(source, list->name, list->n_name))
			goto label_fail;
		list = list->next;
	}
	if (!exbuffer_append(source, s2, sizeof(s2) - 1))
		goto label_fail;
	return source;
	label_fail:
	return NULL;
}

static const vattr_vlist_t* build_full_param(const vattr_vlist_t *restrict p, char *restrict d, uintptr_t n, char **restrict p_name, char **restrict p_end)
{
	const char *restrict k;
	const cparse_value_s *restrict v;
	uintptr_t last_is_key, length;
	last_is_key = 0;
	*p_name = NULL;
	*p_end = NULL;
	while (p && (v = (cparse_value_s *) p->value))
	{
		k = p->vslot->key;
		if (*k == ',' || *k == ')')
		{
			*p_end = d;
			return p;
		}
		if (last_is_key)
		{
			last_is_key = 0;
			if (!n) break;
			*d++ = ' ';
			--n;
		}
		if (v->type == cparse_value$key)
		{
			*p_name = d;
			last_is_key = 1;
			length = strlen(k);
			if (length > n)
				break;
			memcpy(d, k, length);
			d += length;
			n -= length;
		}
		else if (*k == '*')
		{
			if (!n) break;
			*d++ = '*';
			--n;
		}
		else break;
		p = p->vattr_next;
	}
	return NULL;
}

static const vattr_vlist_t* build_param_list(const vattr_vlist_t *restrict p, tparse_tstack_s *restrict tstack, param_list_t **restrict p_list)
{
	char buffer[1024];
	param_list_t **p_tail;
	param_list_t *v;
	char *name;
	char *end;
	uintptr_t length;
	*p_list = NULL;
	p_tail = p_list;
	tparse_tstack_clear(tstack);
	goto label_entry;
	while (p && (p = build_full_param(p, buffer, sizeof(buffer), &name, &end)) && name)
	{
		length = (uintptr_t) end - (uintptr_t) buffer;
		if (!(v = tparse_tstack_push(tstack, sizeof(param_list_t) + length + 1, NULL)))
			break;
		v->next = NULL;
		v->full = (const char *) memcpy(v + 1, buffer, length + 1);
		v->name = v->full + ((uintptr_t) name - (uintptr_t) buffer);
		v->n_full = length;
		v->n_name = (uintptr_t) end - (uintptr_t) name;
		*p_tail = v;
		p_tail = &v->next;
		label_entry:
		switch (*p->vslot->key)
		{
			case ',': p = p->vattr_next; break;
			case ')': return p;
			default: goto label_fail;
		}
	}
	label_fail:
	return NULL;
}

static const vattr_vlist_t* build_header_and_source_only(const vattr_vlist_t *restrict item, exbuffer_t *restrict header, exbuffer_t *restrict source, tparse_tstack_s *restrict tstack)
{
	// d_item(id, type param, ...)
	const vattr_vlist_t *restrict p;
	const cparse_value_s *restrict v;
	const char *restrict id;
	param_list_t *list;
	// get func id
	p = item->vattr_next;
	if (!p || *p->vslot->key != '(')
		goto label_fail;
	if (!(p = p->vattr_next) || !(v = (cparse_value_s *) p->value))
		goto label_fail;
	if (v->type != cparse_value$key)
		goto label_fail;
	id = p->vslot->key;
	// get param list
	if (!(p = p->vattr_next))
		goto label_fail;
	if (!build_param_list(p, tstack, &list))
		goto label_fail;
	if (build_header_inner(header, id, list) && build_source_inner(source, id, list))
		return item;
	label_fail:
	return NULL;
}

static cparse_scope_s* build_header_and_source(cparse_scope_s *restrict scope, exbuffer_t *restrict header, exbuffer_t *restrict source, tparse_tstack_s *restrict tstack)
{
	vattr_vlist_t *restrict item;
	for (item = vattr_get_vlist_first(scope->scope, "d_item"); item; item = item->vslot_next)
	{
		if (!build_header_and_source_only(item, header, source, tstack))
			return NULL;
	}
	return scope;
}

static int build_proxy_call(cparse_scope_s *restrict scope, exbuffer_t *restrict header, exbuffer_t *restrict source, tparse_tstack_s *restrict tstack)
{
	if (!build_header_begin(header))
		goto label_fail;
	if (!build_source_begin(source))
		goto label_fail;
	if (!build_header_and_source(scope, header, source, tstack))
		goto label_fail;
	if (!build_header_end(header))
		goto label_fail;
	if (!build_source_end(source))
		goto label_fail;
	if (write_file(header, "elfin.item.h") && write_file(source, "elfin.item.build.c"))
		return 0;
	label_fail:
	return -1;
}

int main(void)
{
	cparse_inst_s *restrict inst;
	cparse_scope_s *restrict scope;
	tparse_tstack_s *restrict tstack;
	exbuffer_t h, s;
	int ret;
	ret = -1;
	if ((inst = cparse_inst_alloc()))
	{
		if ((scope = cparse_scope_load(inst, "inner.item.func.inc")))
		{
			// 删除注释
			vattr_delete(scope->scope, "//");
			vattr_delete(scope->scope, "/*");
			if ((tstack = tparse_tstack_alloc()))
			{
				if (exbuffer_init(&h, 0))
				{
					if (exbuffer_init(&s, 0))
					{
						ret = build_proxy_call(scope, &h, &s, tstack);
						exbuffer_uini(&s);
					}
					exbuffer_uini(&h);
				}
				refer_free(tstack);
			}
			refer_free(scope);
		}
		refer_free(inst);
	}
	return ret;
}
