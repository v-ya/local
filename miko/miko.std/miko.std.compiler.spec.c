#include "miko.std.compiler.h"
#include "miko.std.spec.h"

const miko_source_s* miko_std_compiler_create_default_spec(void)
{
	miko_source_s *restrict source;
	refer_nstring_t spec;
	source = NULL;
	if ((spec = refer_dump_nstring_with_length(_miko_std_spec_data_, (uintptr_t) _miko_std_spec_size_)))
	{
		source = miko_source_alloc("miko.std.spec", spec);
		refer_free(spec);
	}
	return source;
}
