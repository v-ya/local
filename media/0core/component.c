#include "component.h"
#include "../0bits/fdct.h"

refer_t media_component_create__fdct_i32_r8p16(void)
{
	return media_fdct_2d_i32_alloc(8, 16);
}
