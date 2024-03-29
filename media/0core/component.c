#include "component.h"
#include "../0bits/fdct.h"
#include "../0bits/zigzag.h"

refer_t media_component_create__fdct_i32_r8p16(void)
{
	return media_fdct_2d_i32_alloc(8, 16);
}

refer_t media_component_create__zigzag_8x8(void)
{
	return media_zigzag_alloc(8, 8);
}
