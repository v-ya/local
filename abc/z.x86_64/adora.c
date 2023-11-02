#include "adora.h"

void abc_z_adora_isa_initial__x86_64(abc_adora_isa_s *restrict isa, const abc_adora_isa_t *restrict func)
{
	if (func->version(isa) >= abc_adora_isa_version)
	{
		// register iset
		// register instr
		// link instr
		// finally
		func->finally(isa, "x86_64");
	}
}
