#include "args.h"
#include "inst.h"

int main(int argc, const char *argv[])
{
	args_t args;
	if (args_get(&args, argc, argv))
	{
		inst_s *restrict inst;
		inst = inst_alloc(args.image_path);
		if (inst)
		{
			inst_wait(inst);
			inst_free(inst);
		}
	}
	return 0;
}
