#include "../vkaa.h"
#include "../vkaa.std.h"
#include "../vkaa.std.extern.h"
#include <fsys.h>
#include <stdio.h>
#include <signal.h>

void dump_syntax(const vkaa_syntax_s *restrict syntax, int n)
{
	const vkaa_syntax_t *restrict s;
	for (uintptr_t i = 0; i < syntax->syntax_number; ++i)
	{
		s = syntax->syntax_array + i;
		switch (s->type)
		{
			case vkaa_syntax_type_keyword:
				printf("%*c[%zu] keyword: %s\n", n, 0, i, s->data.keyword->string);
				break;
			case vkaa_syntax_type_operator:
				printf("%*c[%zu] operator: %s\n", n, 0, i, s->data.operator->string);
				break;
			case vkaa_syntax_type_comma:
				printf("%*c[%zu] ,\n", n, 0, i);
				break;
			case vkaa_syntax_type_semicolon:
				printf("%*c[%zu] ;\n", n, 0, i);
				break;
			case vkaa_syntax_type_scope:
				printf("%*c[%zu] {}\n", n, 0, i);
				dump_syntax(s->data.scope, n + 4);
				break;
			case vkaa_syntax_type_brackets:
				printf("%*c[%zu] ()\n", n, 0, i);
				dump_syntax(s->data.brackets, n + 4);
				break;
			case vkaa_syntax_type_square:
				printf("%*c[%zu] []\n", n, 0, i);
				dump_syntax(s->data.square, n + 4);
				break;
			case vkaa_syntax_type_string:
				printf("%*c[%zu] \"\": %s\n", n, 0, i, s->data.string->string);
				break;
			case vkaa_syntax_type_multichar:
				printf("%*c[%zu] \'\': %s\n", n, 0, i, s->data.multichar->string);
				break;
			case vkaa_syntax_type_number:
				printf("%*c[%zu] 0: %s\n", n, 0, i, s->data.number->string);
				break;
			default:
				break;
		}
	}
}

refer_nstring_t load_file(const char *restrict path)
{
	fsys_file_s *restrict fp;
	refer_nstring_t r;
	r = NULL;
	if ((fp = fsys_file_alloc(path, fsys_file_flag_read)))
	{
		r = fsys_file_aread_all(fp);
		refer_free(fp);
	}
	return r;
}

static vkaa_var_s* vkaa_type_stdout_create(const vkaa_type_s *restrict type, const vkaa_syntax_s *restrict syntax)
{
	vkaa_var_s *restrict r;
	if (!syntax && (r = (vkaa_var_s *) refer_alloz(sizeof(vkaa_var_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_var_finally);
		if (vkaa_var_initial(r, type))
			return r;
		refer_free(r);
	}
	return NULL;
}

static uintptr_t vkaa_var_stdout_print(const vkaa_function_s *restrict r, vkaa_execute_control_t *restrict control)
{
	refer_nstring_t msg;
	if ((msg = ((vkaa_std_var_string_s *) r->input_list[1])->value))
		mlog_printf((mlog_s *) r->pri_data, "%s", msg->string);
	return 0;
}

const vkaa_std_s* vkaa_me_insert_stdout(const vkaa_std_s *restrict std, mlog_s *restrict mlog)
{
	const vkaa_std_s *rr;
	vkaa_type_s *restrict r;
	uintptr_t id;
	rr = NULL;
	id = vkaa_tpool_genid(std->tpool);
	if ((r = (vkaa_type_s *) refer_alloz(sizeof(vkaa_type_s))))
	{
		refer_set_free(r, (refer_free_f) vkaa_type_finally);
		if (vkaa_type_initial(r, id, "stdout", vkaa_type_stdout_create) &&
			vkaa_std_type_set_function(r, "<<", vkaa_var_stdout_print, mlog,
				vkaa_std_selector_output_must_first, vkaa_std_selector_convert_none,
				0, id, 2, (const uintptr_t []) {id, std->typeid->id_string}) &&
			vkaa_tpool_insert(std->tpool, r) &&
			vkaa_tpool_var_const_enable(std->tpool, r))
			rr = std;
		refer_free(r);
	}
	return rr;
}

static volatile uintptr_t running;

static void signal_int_func(int sig)
{
	printf("signal want to stop ...\n");
	running = 0;
}

int main(int argc, const char *const restrict argv[])
{
	mlog_s *restrict mlog;
	const vkaa_std_s *restrict std;
	vkaa_std_context_s *restrict c;
	refer_nstring_t code;
	const char *restrict source_name;
	int rr;
	rr = -1;
	if (argc == 2)
	{
		source_name = argv[1];
		running = 1;
		signal(SIGINT, signal_int_func);
		if ((mlog = mlog_alloc(0)))
		{
			mlog_set_report(mlog, mlog_report_stdout_func, NULL);
			if ((std = vkaa_std_alloc()))
			{
				if (vkaa_me_insert_stdout(std, mlog) &&
					(c = vkaa_std_context_alloc(std, mlog, mlog)))
				{
					if ((code = load_file(source_name)))
					{
						if (vkaa_std_context_append_source(c, code, source_name))
							rr = (int) vkaa_std_context_exec(c, &running);
						refer_free(code);
					}
					refer_free(c);
				}
				refer_free(std);
			}
			refer_free(mlog);
		}
	}
	else printf("%s <vkaa-script>\n", argv[0]);
	return rr;
}
