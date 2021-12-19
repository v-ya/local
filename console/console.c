#include "console.type.h"

static void console_free_func(console_s *restrict r)
{
	if (r->name)
		refer_free(r->name);
	if (r->cmds)
		refer_free(r->cmds);
	if (r->hint)
		refer_free(r->hint);
}

static console_s* console_alloc_empty(const char *restrict name)
{
	console_s *restrict r;
	if ((r = (console_s *) refer_alloz(sizeof(console_s))))
	{
		refer_set_free(r, (refer_free_f) console_free_func);
		r->name = refer_dump_nstring(name?name:"");
		r->cmds = vattr_alloc();
		r->hint = vattr_alloc();
		if (r->name && r->cmds && r->hint)
			return r;
		refer_free(r);
	}
	return NULL;
}

console_s* console_alloc(const char *restrict name)
{
	console_s *restrict r;
	if ((r = console_alloc_empty(name)))
	{
		if (console_insert_command_exit(r) &&
			console_insert_command_help(r))
			return r;
		refer_free(r);
	}
	return NULL;
}

console_s* console_insert_command(console_s *restrict console, const char *restrict name, console_command_s *restrict command)
{
	vattr_vlist_t *vl_cmd_this, *vl_hint_tail, *next;
	refer_nstring_t hint_name;
	char *p, *pe;
	if (name && command && !vattr_get_vslot(console->cmds, name))
	{
		vl_hint_tail = vattr_tail(console->hint);
		vl_cmd_this = vattr_insert_tail(console->cmds, name, command);
		hint_name = refer_dump_nstring(name);
		if (vl_cmd_this && hint_name)
		{
			p = (char *) hint_name->string;
			pe = p + hint_name->length;
			while (p <= pe)
			{
				*pe = *p;
				*p = 0;
				if (!vattr_insert_tail(console->hint, hint_name->string, hint_name))
					goto label_fail;
				*p = *pe;
				++p;
			}
			refer_free(hint_name);
			return console;
		}
		label_fail:
		next = vl_hint_tail->vattr_next;
		while ((vl_hint_tail = next))
		{
			next = vl_hint_tail->vattr_next;
			vattr_delete_vlist(vl_hint_tail);
		}
		if (vl_cmd_this)
			vattr_delete_vlist(vl_cmd_this);
		if (hint_name)
			refer_free(hint_name);
	}
	return NULL;
}

void console_delete_command(console_s *restrict console, const char *restrict name)
{
	vattr_vslot_t *vslot;
	vattr_vlist_t *p, *next;
	if (name && (vslot = vattr_get_vslot(console->cmds, name)))
	{
		vattr_delete_vslot(vslot);
		next = vattr_first(console->hint);
		while ((p = next))
		{
			next = p->vattr_next;
			if (!vattr_get_vslot(console->cmds, ((refer_nstring_t) p->value)->string))
				vattr_delete_vlist(p);
		}
	}
}

typedef struct console_do_t {
	console_inst_s *inst;
	mlog_s *mlog;
	exbuffer_t *cname;
	console_s *console;
} console_do_t;

typedef struct console_do_hint_t {
	console_s *console;
	mlog_s *mlog;
} console_do_hint_t;

static uintptr_t console_do_cname_make(exbuffer_t *restrict cname, refer_nstring_t name)
{
	uint8_t *restrict d;
	uintptr_t last_used;
	if (cname)
	{
		// cname := "path<space>name:<space>"
		if ((last_used = cname->used) >= 2)
			last_used -= 2;
		else last_used = 0;
		if (name && (d = exbuffer_need(cname, last_used + name->length + 3)))
		{
			if ((cname->used = last_used))
				d[cname->used++] = ' ';
			memcpy(cname->data + cname->used, name->string, name->length);
			cname->used += name->length;
			d[cname->used++] = ':';
			d[cname->used++] = ' ';
		}
		return last_used;
	}
	return 0;
}

static void console_do_cname_back(exbuffer_t *restrict cname, uintptr_t last_used)
{
	uint8_t *restrict d;
	if (cname && (d = exbuffer_need(cname, last_used + 2)))
	{
		cname->used = last_used;
		d[cname->used++] = ':';
		d[cname->used++] = ' ';
	}
}

static const char* console_do_loop_hint_multi_same(vattr_vlist_t *restrict vlist, uintptr_t n, uintptr_t *restrict rn)
{
	refer_nstring_t v;
	const char *restrict r, *restrict p;
	uintptr_t rm, pm;
	r = NULL;
	rm = ~(uintptr_t) 0;
	do {
		v = (refer_nstring_t) vlist->value;
		if (v->length <= n)
			goto label_fail;
		p = v->string + n;
		pm = v->length - n;
		if (rm > pm) rm = pm;
		if (r)
		{
			for (pm = 0; pm < rm && p[pm] == r[pm]; ++pm) ;
			if (!(rm = pm))
				goto label_fail;
		}
		else r = p;
	} while ((vlist = vlist->vslot_next));
	if ((*rn = rm)) return r;
	label_fail:
	return NULL;
}

static exbuffer_t* console_do_loop_hint(exbuffer_t *restrict hint, const console_do_hint_t *restrict p, uintptr_t argc, const char *const *argv, uintptr_t last_argv_is_finish)
{
	vattr_vslot_t *restrict vslot;
	vattr_vlist_t *restrict vlist;
	console_command_s *restrict cc;
	refer_nstring_t name;
	uintptr_t n;
	if ((vslot = vattr_get_vslot(p->console->hint, argc?argv[0]:"")))
	{
		vlist = vslot->vslot;
		n = strlen(vslot->key);
		if (vlist->vslot_next && argc == 1 && !last_argv_is_finish)
		{
			const char *restrict s;
			// compare multi
			if ((s = console_do_loop_hint_multi_same(vlist, n, &n)))
			{
				if (exbuffer_append(hint, s, n))
					goto label_okay;
			}
			else
			{
				mlog_printf(p->mlog, "\n");
				while (vlist)
				{
					mlog_printf(p->mlog, "%s%c",
						((refer_nstring_t) vlist->value)->string,
						vlist->vslot_next?' ':'\n');
					vlist = vlist->vslot_next;
				}
				goto label_okay;
			}
		}
		else if (n < (name = (refer_nstring_t) vlist->value)->length)
		{
			// compare one
			if (exbuffer_append(hint, name->string + n, name->length - n))
				goto label_okay;
		}
		else
		{
			// compared
			if ((cc = (console_command_s *) vattr_get_first(p->console->cmds, name->string)) &&
				cc->help)
			{
				mlog_printf(p->mlog, "\n");
				cc->help(cc, p->mlog, argc, argv);
				label_okay:
				return hint;
			}
		}
	}
	return NULL;
}

static void console_do_loop(const console_do_t *restrict p, console_s *restrict console)
{
	console_do_hint_t hint;
	const char *const *argv;
	uintptr_t argc;
	hint.console = console;
	hint.mlog = p->mlog;
	console->running = 1;
	while (console->running && (argv = console_inst_get(p->inst, &argc, p->cname, (console_inst_hint_f) console_do_loop_hint, &hint)))
	{
		if (argc)
		{
			console_command_s *restrict cc;
			console_s *restrict c;
			cc = (console_command_s *) vattr_get_first(console->cmds, argv[0]);
			if (cc)
			{
				if ((c = cc->main(cc, p->mlog, argc, argv)))
				{
					argc = console_do_cname_make(p->cname, c->name);
					console_do_loop(p, c);
					console_do_cname_back(p->cname, argc);
				}
			}
			else mlog_printf(p->mlog, "don't find (%s)\n", argv[0]);
		}
	}
}

void console_do(console_s *restrict console, console_inst_s *restrict inst, mlog_s *mlog)
{
	console_do_t v;
	exbuffer_t eb;
	if (console && inst)
	{
		v.inst = inst;
		v.mlog = mlog;
		v.cname = exbuffer_init(&eb, 0);
		console_do_cname_back(v.cname, 0);
		console_do_cname_make(v.cname, console->name);
		console_do_loop(&v, console);
		if (v.cname) exbuffer_uini(v.cname);
	}
}
