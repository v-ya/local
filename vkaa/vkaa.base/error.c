#include "../vkaa.error.h"
#include "../vkaa.syntax.h"
#include <memory.h>

// error

static void vkaa_error_free_func(vkaa_error_s *restrict r)
{
	hashmap_uini(&r->n2i);
	hashmap_uini(&r->i2n);
}

vkaa_error_s* vkaa_error_alloc(void)
{
	vkaa_error_s *restrict r;
	if ((r = (vkaa_error_s *) refer_alloz(sizeof(vkaa_error_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_error_free_func);
		if (hashmap_init(&r->n2i) && hashmap_init(&r->i2n))
			return r;
		refer_free(r);
	}
	return NULL;
}

vkaa_error_s* vkaa_error_add_error(vkaa_error_s *restrict error, const char *restrict name)
{
	hashmap_vlist_t *restrict vl;
	uintptr_t id;
	id = (uintptr_t) error->i2n.number + 2;
	if (name && !hashmap_find_name(&error->n2i, name) &&
		!hashmap_find_head(&error->i2n, (uint64_t) id))
	{
		if ((vl = hashmap_set_name(&error->n2i, name, (void *) id, NULL)))
		{
			if (hashmap_set_head(&error->i2n, (uint64_t) id, vl->name, NULL))
				return error;
			hashmap_delete_name(&error->n2i, name);
		}
	}
	return NULL;
}

uintptr_t vkaa_error_get_id(const vkaa_error_s *restrict error, const char *restrict name)
{
	return (uintptr_t) hashmap_get_name(&error->n2i, name) - 1;
}

const char* vkaa_error_get_name(const vkaa_error_s *restrict error, uintptr_t id)
{
	return (const char *) hashmap_get_head(&error->i2n, (uint64_t) (id + 1));
}

// elog

static void vkaa_elog_print(mlog_s *restrict mlog, const vkaa_syntax_source_s *restrict source, uint32_t syntax_pos_start, uint32_t syntax_pos_stop, uintptr_t exec_pos, const char *restrict message)
{
	const char *restrict name;
	name = (source && source->name)?source->name:"";
	if (message)
		mlog_printf(mlog, "[%s:%zu] %s\n", name, exec_pos, message);
	if (source)
	{
		uintptr_t i, n;
		uint32_t r1, r2, c1, c2;
		char hint[256];
		n = source->source->length;
		if (n > syntax_pos_stop)
			n = syntax_pos_stop;
		if (n > syntax_pos_start)
		{
			n -= syntax_pos_start;
			if (n >= sizeof(hint))
				n = sizeof(hint) - 1;
			memcpy(hint, source->source->string + syntax_pos_start, n);
		}
		else n = 0;
		hint[n] = 0;
		for (i = 0; i < n; ++i)
		{
			switch (hint[i])
			{
				case '\t':
				case '\r':
				case '\n':
					hint[i] = ' ';
					break;
				default:
					break;
			}
		}
		r1 = vkaa_syntax_source_linepos(source, syntax_pos_start, &c1);
		r2 = vkaa_syntax_source_linepos(source, syntax_pos_stop, &c2);
		mlog_printf(mlog, "\t(%u:%u-%u:%u) %s\n", r1, c1, r2, c2, hint);
	}
}

static void vkaa_elog_free_func(vkaa_elog_s *restrict r)
{
	vkaa_elog_exec_t *restrict p;
	uintptr_t i, n;
	if (r->parse) refer_free(r->parse);
	if (r->exec) refer_free(r->exec);
	if (r->error) refer_free(r->error);
	p = r->exec_array;
	n = r->exec_number;
	for (i = 0; i < n; ++i)
	{
		if (p[i].source)
			refer_free(p[i].source);
	}
	exbuffer_uini(&r->exec_buffer);
}

vkaa_elog_s* vkaa_elog_alloc(mlog_s *parse, mlog_s *exec, const vkaa_error_s *error)
{
	vkaa_elog_s *restrict r;
	if ((r = (vkaa_elog_s *) refer_alloz(sizeof(vkaa_elog_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_elog_free_func);
		if (exbuffer_init(&r->exec_buffer, 0))
		{
			r->parse = (mlog_s *) refer_save(parse);
			r->exec = (mlog_s *) refer_save(exec);
			r->error = (const vkaa_error_s *) refer_save(error);
			r->exec_curr = ~(uintptr_t) 0;
			return r;
		}
		refer_free(r);
	}
	return NULL;
}

static uintptr_t vkaa_elog_get_empty(vkaa_elog_s *restrict elog, const vkaa_syntax_source_s *restrict source, uintptr_t exec_pos_start, uint32_t syntax_pos_start)
{
	vkaa_elog_exec_t *restrict p;
	uintptr_t i;
	i = elog->exec_number;
	if ((p = (vkaa_elog_exec_t *) exbuffer_need(&elog->exec_buffer, sizeof(vkaa_elog_exec_t) * (i + 1))))
	{
		elog->exec_array = p;
		p += i;
		p->index_skip = 0;
		p->index_parent = ~(uintptr_t) 0;
		p->exec_pos_start = exec_pos_start;
		p->exec_pos_stop = exec_pos_start;
		p->source = (const vkaa_syntax_source_s *) refer_save(source);
		p->syntax_pos_start = syntax_pos_start;
		p->syntax_pos_stop = syntax_pos_start;
		elog->exec_number = i + 1;
		return i;
	}
	return ~(uintptr_t) 0;
}

vkaa_elog_s* vkaa_elog_push(vkaa_elog_s *restrict elog, const vkaa_syntax_source_s *source, uintptr_t exec_pos_start, uint32_t syntax_pos_start)
{
	uintptr_t index_new;
	if (~(index_new = vkaa_elog_get_empty(elog, source, exec_pos_start, syntax_pos_start)))
	{
		elog->exec_array[index_new].index_parent = elog->exec_curr;
		elog->exec_curr = index_new;
		return elog;
	}
	return NULL;
}

vkaa_elog_s* vkaa_elog_fence(vkaa_elog_s *restrict elog, const vkaa_syntax_source_s *source, uintptr_t exec_pos, uint32_t syntax_pos)
{
	vkaa_elog_exec_t *restrict p;
	uintptr_t index_curr, index_new;
	p = elog->exec_array;
	if (~(index_curr = elog->exec_curr) && p[index_curr].source == source)
	{
		p[index_curr].index_skip = elog->exec_number - index_curr;
		p[index_curr].exec_pos_stop = exec_pos;
		p[index_curr].syntax_pos_stop = syntax_pos;
		if (~(index_new = vkaa_elog_get_empty(elog, source, exec_pos, syntax_pos)))
		{
			p = elog->exec_array;
			p[index_new].index_parent = p[index_curr].index_parent;
			elog->exec_curr = index_new;
			return elog;
		}
	}
	return NULL;
}

vkaa_elog_s* vkaa_elog_pop(vkaa_elog_s *restrict elog, uintptr_t exec_pos_stop, uint32_t syntax_pos_stop)
{
	vkaa_elog_exec_t *restrict p;
	uintptr_t index_curr;
	if (~(index_curr = elog->exec_curr))
	{
		p = elog->exec_array;
		p[index_curr].index_skip = elog->exec_number - index_curr;
		p[index_curr].exec_pos_stop = exec_pos_stop;
		p[index_curr].syntax_pos_stop = syntax_pos_stop;
		elog->exec_curr = p[index_curr].index_parent;
		return elog;
	}
	return NULL;
}

const vkaa_elog_exec_t* vkaa_elog_find(const vkaa_elog_s *restrict elog, uintptr_t exec_pos)
{
	const vkaa_elog_exec_t *restrict p, *restrict hit;
	uintptr_t i, n;
	hit = NULL;
	p = elog->exec_array;
	n = elog->exec_number;
	i = 0;
	while (i < n)
	{
		if (exec_pos >= p[i].exec_pos_start && exec_pos < p[i].exec_pos_stop)
		{
			hit = (p += i);
			n = p[0].index_skip;
			i = 1;
		}
		else i += p[i].index_skip;
	}
	return hit;
}

void vkaa_elog_print_parse(const vkaa_elog_s *restrict elog, const vkaa_syntax_source_s *restrict source, uint32_t syntax_pos, uintptr_t exec_pos, const char *restrict message)
{
	const char *restrict s;
	uintptr_t i, n;
	uint32_t syntax_pos_start, syntax_pos_stop;
	syntax_pos_start = syntax_pos_stop = 0;
	if (source)
	{
		s = source->source->string;
		n = source->source->length;
		if (((uintptr_t) syntax_pos <= n))
		{
			i = (uintptr_t) syntax_pos;
			while (i && s[i - 1] != '\n')
				i -= 1;
			syntax_pos_start = (uint32_t) i;
			i = (uintptr_t) syntax_pos;
			while (i < n && s[i] != '\n')
				i += 1;
			syntax_pos_stop = (uint32_t) i;
		}
	}
	vkaa_elog_print(elog->parse, source, syntax_pos_start, syntax_pos_stop, exec_pos, message);
}

void vkaa_elog_print_exec(const vkaa_elog_s *restrict elog, uintptr_t exec_pos, const char *restrict message)
{
	const vkaa_elog_exec_t *restrict exec;
	if ((exec = vkaa_elog_find(elog, exec_pos)))
		vkaa_elog_print(elog->parse, exec->source, exec->syntax_pos_start, exec->syntax_pos_stop, exec_pos, message);
	else vkaa_elog_print(elog->parse, NULL, 0, 0, exec_pos, message);
}
