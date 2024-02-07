#include "bonex.h"

iphyee_bonex_simple_f iphyee_bonex_fix_method(iphyee_bonex_fix_method_t method)
{
	switch (method)
	{
		case iphyee_bonex_fix_method__tx: return iphyee_mat4x4_fix_tx;
		case iphyee_bonex_fix_method__ty: return iphyee_mat4x4_fix_ty;
		case iphyee_bonex_fix_method__tz: return iphyee_mat4x4_fix_tz;
		case iphyee_bonex_fix_method__rx: return iphyee_mat4x4_fix_rx;
		case iphyee_bonex_fix_method__ry: return iphyee_mat4x4_fix_ry;
		case iphyee_bonex_fix_method__rz: return iphyee_mat4x4_fix_rz;
		default: return NULL;
	}
}
