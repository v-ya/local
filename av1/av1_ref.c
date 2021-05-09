#include "av1_ref.h"
#include <memory.h>

av1_ref_t* av1_ref_init(av1_ref_t *restrict ref)
{
	return (av1_ref_t *) memset(ref, 0, sizeof(*ref));
}
