#include "../header/miko.h"
#include "../header/miko.type.h"
#include "../header/miko.api.h"
#include "../header/miko.wink.h"
#include "../header/miko.std.h"
#include "../header/miko.std.type.h"
#include "../header/miko.std.api.h"
#include "../header/miko.std.marco.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <yaw.h>

void test_syntax_print(mlog_s *restrict mlog, miko_std_syntax_s *restrict syntax, uint32_t indent);

void test_std(mlog_s *restrict mlog)
{
	const miko_source_s *restrict spec;
	miko_std_syntaxor_s *restrict syntaxor;
	miko_std_syntax_s *restrict syntax;
	if ((spec = miko_std_compiler_create_default_spec()))
	{
		// mlog_printf(mlog, "%s", spec->source->string);
		if ((syntaxor = miko_std_syntaxor_create_spec(mlog)))
		{
			mlog_printf(mlog, "syntaxor %p okay\n", syntaxor);
			if ((syntax = miko_std_syntaxor_create(syntaxor, spec)))
			{
				mlog_printf(mlog, "syntax %p\n", syntax);
				test_syntax_print(mlog, syntax, 0);
				refer_free(syntax);
			}
			refer_free(syntaxor);
		}
		refer_free(spec);
	}
}

int main(int argc, const char *argv[])
{
	mlog_s *mlog;
	miko_env_s *env;
	miko_wink_gomi_s *gomi;
	if ((mlog = mlog_alloc(0)))
	{
		mlog_set_report(mlog, mlog_report_stdout_func, NULL);
		mlog_printf(mlog, "miko.bin start ...\n");
		if ((env = miko_env_create(mlog, (miko_env_register_f []) {miko_std_env_register, NULL})))
		{
			mlog_printf(mlog, "miko.env = %p ...\n", env);
			if ((gomi = miko_wink_gomi_alloc()))
			{
				mlog_printf(mlog, "miko.wink.gomi = %p ...\n", gomi);
				miko_wink_gomi_default_report(gomi, mlog, 1);
				miko_wink_gomi_call_cycle(gomi, 200);
				test_std(mlog);
				refer_free(gomi);
			}
			refer_free(env);
		}
		mlog_printf(mlog, "miko.bin end ...\n");
		refer_free(mlog);
	}
	return 0;
}

void test_syntax_print_scope(mlog_s *restrict mlog, miko_std_syntax_s *restrict syntax, uint32_t indent, uint32_t isblock)
{
	miko_std_syntax_s *restrict value;
	vattr_vlist_t *restrict vlist;
	refer_nstring_t nstr;
	uintptr_t pos;
	nstr = syntax->data.syntax;
	if (nstr && nstr->length >= 2)
	{
		if (!isblock) mlog_printf(mlog, "%c", nstr->string[0]);
		else mlog_printf(mlog, "\n%*s%c\n", indent, "", nstr->string[0]);
		indent += 4;
	}
	pos = 0;
	for (vlist = syntax->data.scope->vattr; vlist; vlist = vlist->vattr_next)
	{
		value = (miko_std_syntax_s *) vlist->value;
		if (isblock && !pos)
			mlog_printf(mlog, "%*s", indent, "");
		if (pos) mlog_printf(mlog, " ");
		test_syntax_print(mlog, value, indent);
		pos += 1;
		if (isblock && !strcmp(value->id.name, miko_std_syntax__semicolon))
		{
			mlog_printf(mlog, "\n");
			pos = 0;
		}
		if (!value->id.name || !strcmp(value->id.name, miko_std_syntax__scope))
			pos = 0;
	}
	if (nstr && nstr->length >= 2)
	{
		indent -= 4;
		if (!isblock) mlog_printf(mlog, "%c", nstr->string[1]);
		else mlog_printf(mlog, "%s%*s%c\n", pos?"\n":"", indent, "", nstr->string[1]);
	}
}

void test_syntax_print(mlog_s *restrict mlog, miko_std_syntax_s *restrict syntax, uint32_t indent)
{
	if (syntax->data.scope)
	{
		if (!syntax->id.name || !strcmp(syntax->id.name, miko_std_syntax__scope))
			test_syntax_print_scope(mlog, syntax, indent, 1);
		else test_syntax_print_scope(mlog, syntax, indent, 0);
	}
	else if (syntax->data.syntax)
		mlog_printf(mlog, "%s", syntax->data.syntax->string);
}
