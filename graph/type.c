#include "type_pri.h"

static const char graph_string_unknow[] = graph_type_red "unknow" graph_type_back;

char* graph_type_list(char *restrict s, register uint32_t r, register const char *array[], register uint32_t n)
{
	register char *d;
	register const char *restrict c;
	register uint32_t i;
	d = s;
	i = 0;
	do {
		if (r & 1)
		{
			c = array[i];
			do *d++ = *c++; while (*c);
			*d++ = ' ';
		}
		r >>= 1;
	} while (++i < n);
	if (d > s) --d;
	*d = 0;
	return s;
}

const char* graph_bool$string(register graph_bool_t r)
{
	return (const char *[]){
		graph_type_red   "false" graph_type_back,
		graph_type_green "true"  graph_type_back
	}[!!r];
}

const char* graph_physical_device_type$string(register graph_physical_device_type_t r)
{
	static const char *mapping[] = {
		[graph_physical_device_type_other]          = graph_type_red "other" graph_type_back,
		[graph_physical_device_type_integrated_gpu] = graph_type_blue "integrated gpu" graph_type_back,
		[graph_physical_device_type_discrete_gpu]   = graph_type_green "discrete gpu" graph_type_back,
		[graph_physical_device_type_virtual_gpu]    = graph_type_yellow "virtual gpu" graph_type_back,
		[graph_physical_device_type_cpu]            = graph_type_purple "cpu" graph_type_back
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return graph_string_unknow;
}

graph_format_t graph_format4vk(register VkFormat r)
{
	static const graph_format_t mapping[] = {
		[VK_FORMAT_UNDEFINED]                  = graph_format_unknow,
		[VK_FORMAT_R4G4_UNORM_PACK8]           = graph_format_r4g4_unorm_pack8,
		[VK_FORMAT_R4G4B4A4_UNORM_PACK16]      = graph_format_r4g4b4a4_unorm_pack16,
		[VK_FORMAT_B4G4R4A4_UNORM_PACK16]      = graph_format_b4g4r4a4_unorm_pack16,
		[VK_FORMAT_R5G6B5_UNORM_PACK16]        = graph_format_r5g6b5_unorm_pack16,
		[VK_FORMAT_B5G6R5_UNORM_PACK16]        = graph_format_b5g6r5_unorm_pack16,
		[VK_FORMAT_R5G5B5A1_UNORM_PACK16]      = graph_format_r5g5b5a1_unorm_pack16,
		[VK_FORMAT_B5G5R5A1_UNORM_PACK16]      = graph_format_b5g5r5a1_unorm_pack16,
		[VK_FORMAT_A1R5G5B5_UNORM_PACK16]      = graph_format_a1r5g5b5_unorm_pack16,
		[VK_FORMAT_R8_UNORM]                   = graph_format_r8_unorm,
		[VK_FORMAT_R8_SNORM]                   = graph_format_r8_snorm,
		[VK_FORMAT_R8_USCALED]                 = graph_format_r8_uscaled,
		[VK_FORMAT_R8_SSCALED]                 = graph_format_r8_sscaled,
		[VK_FORMAT_R8_UINT]                    = graph_format_r8_uint,
		[VK_FORMAT_R8_SINT]                    = graph_format_r8_sint,
		[VK_FORMAT_R8_SRGB]                    = graph_format_r8_srgb,
		[VK_FORMAT_R8G8_UNORM]                 = graph_format_r8g8_unorm,
		[VK_FORMAT_R8G8_SNORM]                 = graph_format_r8g8_snorm,
		[VK_FORMAT_R8G8_USCALED]               = graph_format_r8g8_uscaled,
		[VK_FORMAT_R8G8_SSCALED]               = graph_format_r8g8_sscaled,
		[VK_FORMAT_R8G8_UINT]                  = graph_format_r8g8_uint,
		[VK_FORMAT_R8G8_SINT]                  = graph_format_r8g8_sint,
		[VK_FORMAT_R8G8_SRGB]                  = graph_format_r8g8_srgb,
		[VK_FORMAT_R8G8B8_UNORM]               = graph_format_r8g8b8_unorm,
		[VK_FORMAT_R8G8B8_SNORM]               = graph_format_r8g8b8_snorm,
		[VK_FORMAT_R8G8B8_USCALED]             = graph_format_r8g8b8_uscaled,
		[VK_FORMAT_R8G8B8_SSCALED]             = graph_format_r8g8b8_sscaled,
		[VK_FORMAT_R8G8B8_UINT]                = graph_format_r8g8b8_uint,
		[VK_FORMAT_R8G8B8_SINT]                = graph_format_r8g8b8_sint,
		[VK_FORMAT_R8G8B8_SRGB]                = graph_format_r8g8b8_srgb,
		[VK_FORMAT_B8G8R8_UNORM]               = graph_format_b8g8r8_unorm,
		[VK_FORMAT_B8G8R8_SNORM]               = graph_format_b8g8r8_snorm,
		[VK_FORMAT_B8G8R8_USCALED]             = graph_format_b8g8r8_uscaled,
		[VK_FORMAT_B8G8R8_SSCALED]             = graph_format_b8g8r8_sscaled,
		[VK_FORMAT_B8G8R8_UINT]                = graph_format_b8g8r8_uint,
		[VK_FORMAT_B8G8R8_SINT]                = graph_format_b8g8r8_sint,
		[VK_FORMAT_B8G8R8_SRGB]                = graph_format_b8g8r8_srgb,
		[VK_FORMAT_R8G8B8A8_UNORM]             = graph_format_r8g8b8a8_unorm,
		[VK_FORMAT_R8G8B8A8_SNORM]             = graph_format_r8g8b8a8_snorm,
		[VK_FORMAT_R8G8B8A8_USCALED]           = graph_format_r8g8b8a8_uscaled,
		[VK_FORMAT_R8G8B8A8_SSCALED]           = graph_format_r8g8b8a8_sscaled,
		[VK_FORMAT_R8G8B8A8_UINT]              = graph_format_r8g8b8a8_uint,
		[VK_FORMAT_R8G8B8A8_SINT]              = graph_format_r8g8b8a8_sint,
		[VK_FORMAT_R8G8B8A8_SRGB]              = graph_format_r8g8b8a8_srgb,
		[VK_FORMAT_B8G8R8A8_UNORM]             = graph_format_b8g8r8a8_unorm,
		[VK_FORMAT_B8G8R8A8_SNORM]             = graph_format_b8g8r8a8_snorm,
		[VK_FORMAT_B8G8R8A8_USCALED]           = graph_format_b8g8r8a8_uscaled,
		[VK_FORMAT_B8G8R8A8_SSCALED]           = graph_format_b8g8r8a8_sscaled,
		[VK_FORMAT_B8G8R8A8_UINT]              = graph_format_b8g8r8a8_uint,
		[VK_FORMAT_B8G8R8A8_SINT]              = graph_format_b8g8r8a8_sint,
		[VK_FORMAT_B8G8R8A8_SRGB]              = graph_format_b8g8r8a8_srgb,
		[VK_FORMAT_A8B8G8R8_UNORM_PACK32]      = graph_format_a8b8g8r8_unorm_pack32,
		[VK_FORMAT_A8B8G8R8_SNORM_PACK32]      = graph_format_a8b8g8r8_snorm_pack32,
		[VK_FORMAT_A8B8G8R8_USCALED_PACK32]    = graph_format_a8b8g8r8_uscaled_pack32,
		[VK_FORMAT_A8B8G8R8_SSCALED_PACK32]    = graph_format_a8b8g8r8_sscaled_pack32,
		[VK_FORMAT_A8B8G8R8_UINT_PACK32]       = graph_format_a8b8g8r8_uint_pack32,
		[VK_FORMAT_A8B8G8R8_SINT_PACK32]       = graph_format_a8b8g8r8_sint_pack32,
		[VK_FORMAT_A8B8G8R8_SRGB_PACK32]       = graph_format_a8b8g8r8_srgb_pack32,
		[VK_FORMAT_A2R10G10B10_UNORM_PACK32]   = graph_format_a2r10g10b10_unorm_pack32,
		[VK_FORMAT_A2R10G10B10_SNORM_PACK32]   = graph_format_a2r10g10b10_snorm_pack32,
		[VK_FORMAT_A2R10G10B10_USCALED_PACK32] = graph_format_a2r10g10b10_uscaled_pack32,
		[VK_FORMAT_A2R10G10B10_SSCALED_PACK32] = graph_format_a2r10g10b10_sscaled_pack32,
		[VK_FORMAT_A2R10G10B10_UINT_PACK32]    = graph_format_a2r10g10b10_uint_pack32,
		[VK_FORMAT_A2R10G10B10_SINT_PACK32]    = graph_format_a2r10g10b10_sint_pack32,
		[VK_FORMAT_A2B10G10R10_UNORM_PACK32]   = graph_format_a2b10g10r10_unorm_pack32,
		[VK_FORMAT_A2B10G10R10_SNORM_PACK32]   = graph_format_a2b10g10r10_snorm_pack32,
		[VK_FORMAT_A2B10G10R10_USCALED_PACK32] = graph_format_a2b10g10r10_uscaled_pack32,
		[VK_FORMAT_A2B10G10R10_SSCALED_PACK32] = graph_format_a2b10g10r10_sscaled_pack32,
		[VK_FORMAT_A2B10G10R10_UINT_PACK32]    = graph_format_a2b10g10r10_uint_pack32,
		[VK_FORMAT_A2B10G10R10_SINT_PACK32]    = graph_format_a2b10g10r10_sint_pack32,
		[VK_FORMAT_R16_UNORM]                  = graph_format_r16_unorm,
		[VK_FORMAT_R16_SNORM]                  = graph_format_r16_snorm,
		[VK_FORMAT_R16_USCALED]                = graph_format_r16_uscaled,
		[VK_FORMAT_R16_SSCALED]                = graph_format_r16_sscaled,
		[VK_FORMAT_R16_UINT]                   = graph_format_r16_uint,
		[VK_FORMAT_R16_SINT]                   = graph_format_r16_sint,
		[VK_FORMAT_R16_SFLOAT]                 = graph_format_r16_sfloat,
		[VK_FORMAT_R16G16_UNORM]               = graph_format_r16g16_unorm,
		[VK_FORMAT_R16G16_SNORM]               = graph_format_r16g16_snorm,
		[VK_FORMAT_R16G16_USCALED]             = graph_format_r16g16_uscaled,
		[VK_FORMAT_R16G16_SSCALED]             = graph_format_r16g16_sscaled,
		[VK_FORMAT_R16G16_UINT]                = graph_format_r16g16_uint,
		[VK_FORMAT_R16G16_SINT]                = graph_format_r16g16_sint,
		[VK_FORMAT_R16G16_SFLOAT]              = graph_format_r16g16_sfloat,
		[VK_FORMAT_R16G16B16_UNORM]            = graph_format_r16g16b16_unorm,
		[VK_FORMAT_R16G16B16_SNORM]            = graph_format_r16g16b16_snorm,
		[VK_FORMAT_R16G16B16_USCALED]          = graph_format_r16g16b16_uscaled,
		[VK_FORMAT_R16G16B16_SSCALED]          = graph_format_r16g16b16_sscaled,
		[VK_FORMAT_R16G16B16_UINT]             = graph_format_r16g16b16_uint,
		[VK_FORMAT_R16G16B16_SINT]             = graph_format_r16g16b16_sint,
		[VK_FORMAT_R16G16B16_SFLOAT]           = graph_format_r16g16b16_sfloat,
		[VK_FORMAT_R16G16B16A16_UNORM]         = graph_format_r16g16b16a16_unorm,
		[VK_FORMAT_R16G16B16A16_SNORM]         = graph_format_r16g16b16a16_snorm,
		[VK_FORMAT_R16G16B16A16_USCALED]       = graph_format_r16g16b16a16_uscaled,
		[VK_FORMAT_R16G16B16A16_SSCALED]       = graph_format_r16g16b16a16_sscaled,
		[VK_FORMAT_R16G16B16A16_UINT]          = graph_format_r16g16b16a16_uint,
		[VK_FORMAT_R16G16B16A16_SINT]          = graph_format_r16g16b16a16_sint,
		[VK_FORMAT_R16G16B16A16_SFLOAT]        = graph_format_r16g16b16a16_sfloat,
		[VK_FORMAT_R32_UINT]                   = graph_format_r32_uint,
		[VK_FORMAT_R32_SINT]                   = graph_format_r32_sint,
		[VK_FORMAT_R32_SFLOAT]                 = graph_format_r32_sfloat,
		[VK_FORMAT_R32G32_UINT]                = graph_format_r32g32_uint,
		[VK_FORMAT_R32G32_SINT]                = graph_format_r32g32_sint,
		[VK_FORMAT_R32G32_SFLOAT]              = graph_format_r32g32_sfloat,
		[VK_FORMAT_R32G32B32_UINT]             = graph_format_r32g32b32_uint,
		[VK_FORMAT_R32G32B32_SINT]             = graph_format_r32g32b32_sint,
		[VK_FORMAT_R32G32B32_SFLOAT]           = graph_format_r32g32b32_sfloat,
		[VK_FORMAT_R32G32B32A32_UINT]          = graph_format_r32g32b32a32_uint,
		[VK_FORMAT_R32G32B32A32_SINT]          = graph_format_r32g32b32a32_sint,
		[VK_FORMAT_R32G32B32A32_SFLOAT]        = graph_format_r32g32b32a32_sfloat,
		[VK_FORMAT_R64_UINT]                   = graph_format_r64_uint,
		[VK_FORMAT_R64_SINT]                   = graph_format_r64_sint,
		[VK_FORMAT_R64_SFLOAT]                 = graph_format_r64_sfloat,
		[VK_FORMAT_R64G64_UINT]                = graph_format_r64g64_uint,
		[VK_FORMAT_R64G64_SINT]                = graph_format_r64g64_sint,
		[VK_FORMAT_R64G64_SFLOAT]              = graph_format_r64g64_sfloat,
		[VK_FORMAT_R64G64B64_UINT]             = graph_format_r64g64b64_uint,
		[VK_FORMAT_R64G64B64_SINT]             = graph_format_r64g64b64_sint,
		[VK_FORMAT_R64G64B64_SFLOAT]           = graph_format_r64g64b64_sfloat,
		[VK_FORMAT_R64G64B64A64_UINT]          = graph_format_r64g64b64a64_uint,
		[VK_FORMAT_R64G64B64A64_SINT]          = graph_format_r64g64b64a64_sint,
		[VK_FORMAT_R64G64B64A64_SFLOAT]        = graph_format_r64g64b64a64_sfloat,
		[VK_FORMAT_B10G11R11_UFLOAT_PACK32]    = graph_format_b10g11r11_ufloat_pack32,
		[VK_FORMAT_E5B9G9R9_UFLOAT_PACK32]     = graph_format_e5b9g9r9_ufloat_pack32,
		[VK_FORMAT_D16_UNORM]                  = graph_format_d16_unorm,
		[VK_FORMAT_X8_D24_UNORM_PACK32]        = graph_format_x8_d24_unorm_pack32,
		[VK_FORMAT_D32_SFLOAT]                 = graph_format_d32_sfloat,
		[VK_FORMAT_S8_UINT]                    = graph_format_s8_uint,
		[VK_FORMAT_D16_UNORM_S8_UINT]          = graph_format_d16_unorm_s8_uint,
		[VK_FORMAT_D24_UNORM_S8_UINT]          = graph_format_d24_unorm_s8_uint,
		[VK_FORMAT_D32_SFLOAT_S8_UINT]         = graph_format_d32_sfloat_s8_uint
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return graph_format_unknow;
}

VkFormat graph_format2vk(register graph_format_t r)
{
	static const VkFormat mapping[] = {
		[graph_format_unknow]                     = VK_FORMAT_UNDEFINED,
		[graph_format_r4g4_unorm_pack8]           = VK_FORMAT_R4G4_UNORM_PACK8,
		[graph_format_r4g4b4a4_unorm_pack16]      = VK_FORMAT_R4G4B4A4_UNORM_PACK16,
		[graph_format_b4g4r4a4_unorm_pack16]      = VK_FORMAT_B4G4R4A4_UNORM_PACK16,
		[graph_format_r5g6b5_unorm_pack16]        = VK_FORMAT_R5G6B5_UNORM_PACK16,
		[graph_format_b5g6r5_unorm_pack16]        = VK_FORMAT_B5G6R5_UNORM_PACK16,
		[graph_format_r5g5b5a1_unorm_pack16]      = VK_FORMAT_R5G5B5A1_UNORM_PACK16,
		[graph_format_b5g5r5a1_unorm_pack16]      = VK_FORMAT_B5G5R5A1_UNORM_PACK16,
		[graph_format_a1r5g5b5_unorm_pack16]      = VK_FORMAT_A1R5G5B5_UNORM_PACK16,
		[graph_format_r8_unorm]                   = VK_FORMAT_R8_UNORM,
		[graph_format_r8_snorm]                   = VK_FORMAT_R8_SNORM,
		[graph_format_r8_uscaled]                 = VK_FORMAT_R8_USCALED,
		[graph_format_r8_sscaled]                 = VK_FORMAT_R8_SSCALED,
		[graph_format_r8_uint]                    = VK_FORMAT_R8_UINT,
		[graph_format_r8_sint]                    = VK_FORMAT_R8_SINT,
		[graph_format_r8_srgb]                    = VK_FORMAT_R8_SRGB,
		[graph_format_r8g8_unorm]                 = VK_FORMAT_R8G8_UNORM,
		[graph_format_r8g8_snorm]                 = VK_FORMAT_R8G8_SNORM,
		[graph_format_r8g8_uscaled]               = VK_FORMAT_R8G8_USCALED,
		[graph_format_r8g8_sscaled]               = VK_FORMAT_R8G8_SSCALED,
		[graph_format_r8g8_uint]                  = VK_FORMAT_R8G8_UINT,
		[graph_format_r8g8_sint]                  = VK_FORMAT_R8G8_SINT,
		[graph_format_r8g8_srgb]                  = VK_FORMAT_R8G8_SRGB,
		[graph_format_r8g8b8_unorm]               = VK_FORMAT_R8G8B8_UNORM,
		[graph_format_r8g8b8_snorm]               = VK_FORMAT_R8G8B8_SNORM,
		[graph_format_r8g8b8_uscaled]             = VK_FORMAT_R8G8B8_USCALED,
		[graph_format_r8g8b8_sscaled]             = VK_FORMAT_R8G8B8_SSCALED,
		[graph_format_r8g8b8_uint]                = VK_FORMAT_R8G8B8_UINT,
		[graph_format_r8g8b8_sint]                = VK_FORMAT_R8G8B8_SINT,
		[graph_format_r8g8b8_srgb]                = VK_FORMAT_R8G8B8_SRGB,
		[graph_format_b8g8r8_unorm]               = VK_FORMAT_B8G8R8_UNORM,
		[graph_format_b8g8r8_snorm]               = VK_FORMAT_B8G8R8_SNORM,
		[graph_format_b8g8r8_uscaled]             = VK_FORMAT_B8G8R8_USCALED,
		[graph_format_b8g8r8_sscaled]             = VK_FORMAT_B8G8R8_SSCALED,
		[graph_format_b8g8r8_uint]                = VK_FORMAT_B8G8R8_UINT,
		[graph_format_b8g8r8_sint]                = VK_FORMAT_B8G8R8_SINT,
		[graph_format_b8g8r8_srgb]                = VK_FORMAT_B8G8R8_SRGB,
		[graph_format_r8g8b8a8_unorm]             = VK_FORMAT_R8G8B8A8_UNORM,
		[graph_format_r8g8b8a8_snorm]             = VK_FORMAT_R8G8B8A8_SNORM,
		[graph_format_r8g8b8a8_uscaled]           = VK_FORMAT_R8G8B8A8_USCALED,
		[graph_format_r8g8b8a8_sscaled]           = VK_FORMAT_R8G8B8A8_SSCALED,
		[graph_format_r8g8b8a8_uint]              = VK_FORMAT_R8G8B8A8_UINT,
		[graph_format_r8g8b8a8_sint]              = VK_FORMAT_R8G8B8A8_SINT,
		[graph_format_r8g8b8a8_srgb]              = VK_FORMAT_R8G8B8A8_SRGB,
		[graph_format_b8g8r8a8_unorm]             = VK_FORMAT_B8G8R8A8_UNORM,
		[graph_format_b8g8r8a8_snorm]             = VK_FORMAT_B8G8R8A8_SNORM,
		[graph_format_b8g8r8a8_uscaled]           = VK_FORMAT_B8G8R8A8_USCALED,
		[graph_format_b8g8r8a8_sscaled]           = VK_FORMAT_B8G8R8A8_SSCALED,
		[graph_format_b8g8r8a8_uint]              = VK_FORMAT_B8G8R8A8_UINT,
		[graph_format_b8g8r8a8_sint]              = VK_FORMAT_B8G8R8A8_SINT,
		[graph_format_b8g8r8a8_srgb]              = VK_FORMAT_B8G8R8A8_SRGB,
		[graph_format_a8b8g8r8_unorm_pack32]      = VK_FORMAT_A8B8G8R8_UNORM_PACK32,
		[graph_format_a8b8g8r8_snorm_pack32]      = VK_FORMAT_A8B8G8R8_SNORM_PACK32,
		[graph_format_a8b8g8r8_uscaled_pack32]    = VK_FORMAT_A8B8G8R8_USCALED_PACK32,
		[graph_format_a8b8g8r8_sscaled_pack32]    = VK_FORMAT_A8B8G8R8_SSCALED_PACK32,
		[graph_format_a8b8g8r8_uint_pack32]       = VK_FORMAT_A8B8G8R8_UINT_PACK32,
		[graph_format_a8b8g8r8_sint_pack32]       = VK_FORMAT_A8B8G8R8_SINT_PACK32,
		[graph_format_a8b8g8r8_srgb_pack32]       = VK_FORMAT_A8B8G8R8_SRGB_PACK32,
		[graph_format_a2r10g10b10_unorm_pack32]   = VK_FORMAT_A2R10G10B10_UNORM_PACK32,
		[graph_format_a2r10g10b10_snorm_pack32]   = VK_FORMAT_A2R10G10B10_SNORM_PACK32,
		[graph_format_a2r10g10b10_uscaled_pack32] = VK_FORMAT_A2R10G10B10_USCALED_PACK32,
		[graph_format_a2r10g10b10_sscaled_pack32] = VK_FORMAT_A2R10G10B10_SSCALED_PACK32,
		[graph_format_a2r10g10b10_uint_pack32]    = VK_FORMAT_A2R10G10B10_UINT_PACK32,
		[graph_format_a2r10g10b10_sint_pack32]    = VK_FORMAT_A2R10G10B10_SINT_PACK32,
		[graph_format_a2b10g10r10_unorm_pack32]   = VK_FORMAT_A2B10G10R10_UNORM_PACK32,
		[graph_format_a2b10g10r10_snorm_pack32]   = VK_FORMAT_A2B10G10R10_SNORM_PACK32,
		[graph_format_a2b10g10r10_uscaled_pack32] = VK_FORMAT_A2B10G10R10_USCALED_PACK32,
		[graph_format_a2b10g10r10_sscaled_pack32] = VK_FORMAT_A2B10G10R10_SSCALED_PACK32,
		[graph_format_a2b10g10r10_uint_pack32]    = VK_FORMAT_A2B10G10R10_UINT_PACK32,
		[graph_format_a2b10g10r10_sint_pack32]    = VK_FORMAT_A2B10G10R10_SINT_PACK32,
		[graph_format_r16_unorm]                  = VK_FORMAT_R16_UNORM,
		[graph_format_r16_snorm]                  = VK_FORMAT_R16_SNORM,
		[graph_format_r16_uscaled]                = VK_FORMAT_R16_USCALED,
		[graph_format_r16_sscaled]                = VK_FORMAT_R16_SSCALED,
		[graph_format_r16_uint]                   = VK_FORMAT_R16_UINT,
		[graph_format_r16_sint]                   = VK_FORMAT_R16_SINT,
		[graph_format_r16_sfloat]                 = VK_FORMAT_R16_SFLOAT,
		[graph_format_r16g16_unorm]               = VK_FORMAT_R16G16_UNORM,
		[graph_format_r16g16_snorm]               = VK_FORMAT_R16G16_SNORM,
		[graph_format_r16g16_uscaled]             = VK_FORMAT_R16G16_USCALED,
		[graph_format_r16g16_sscaled]             = VK_FORMAT_R16G16_SSCALED,
		[graph_format_r16g16_uint]                = VK_FORMAT_R16G16_UINT,
		[graph_format_r16g16_sint]                = VK_FORMAT_R16G16_SINT,
		[graph_format_r16g16_sfloat]              = VK_FORMAT_R16G16_SFLOAT,
		[graph_format_r16g16b16_unorm]            = VK_FORMAT_R16G16B16_UNORM,
		[graph_format_r16g16b16_snorm]            = VK_FORMAT_R16G16B16_SNORM,
		[graph_format_r16g16b16_uscaled]          = VK_FORMAT_R16G16B16_USCALED,
		[graph_format_r16g16b16_sscaled]          = VK_FORMAT_R16G16B16_SSCALED,
		[graph_format_r16g16b16_uint]             = VK_FORMAT_R16G16B16_UINT,
		[graph_format_r16g16b16_sint]             = VK_FORMAT_R16G16B16_SINT,
		[graph_format_r16g16b16_sfloat]           = VK_FORMAT_R16G16B16_SFLOAT,
		[graph_format_r16g16b16a16_unorm]         = VK_FORMAT_R16G16B16A16_UNORM,
		[graph_format_r16g16b16a16_snorm]         = VK_FORMAT_R16G16B16A16_SNORM,
		[graph_format_r16g16b16a16_uscaled]       = VK_FORMAT_R16G16B16A16_USCALED,
		[graph_format_r16g16b16a16_sscaled]       = VK_FORMAT_R16G16B16A16_SSCALED,
		[graph_format_r16g16b16a16_uint]          = VK_FORMAT_R16G16B16A16_UINT,
		[graph_format_r16g16b16a16_sint]          = VK_FORMAT_R16G16B16A16_SINT,
		[graph_format_r16g16b16a16_sfloat]        = VK_FORMAT_R16G16B16A16_SFLOAT,
		[graph_format_r32_uint]                   = VK_FORMAT_R32_UINT,
		[graph_format_r32_sint]                   = VK_FORMAT_R32_SINT,
		[graph_format_r32_sfloat]                 = VK_FORMAT_R32_SFLOAT,
		[graph_format_r32g32_uint]                = VK_FORMAT_R32G32_UINT,
		[graph_format_r32g32_sint]                = VK_FORMAT_R32G32_SINT,
		[graph_format_r32g32_sfloat]              = VK_FORMAT_R32G32_SFLOAT,
		[graph_format_r32g32b32_uint]             = VK_FORMAT_R32G32B32_UINT,
		[graph_format_r32g32b32_sint]             = VK_FORMAT_R32G32B32_SINT,
		[graph_format_r32g32b32_sfloat]           = VK_FORMAT_R32G32B32_SFLOAT,
		[graph_format_r32g32b32a32_uint]          = VK_FORMAT_R32G32B32A32_UINT,
		[graph_format_r32g32b32a32_sint]          = VK_FORMAT_R32G32B32A32_SINT,
		[graph_format_r32g32b32a32_sfloat]        = VK_FORMAT_R32G32B32A32_SFLOAT,
		[graph_format_r64_uint]                   = VK_FORMAT_R64_UINT,
		[graph_format_r64_sint]                   = VK_FORMAT_R64_SINT,
		[graph_format_r64_sfloat]                 = VK_FORMAT_R64_SFLOAT,
		[graph_format_r64g64_uint]                = VK_FORMAT_R64G64_UINT,
		[graph_format_r64g64_sint]                = VK_FORMAT_R64G64_SINT,
		[graph_format_r64g64_sfloat]              = VK_FORMAT_R64G64_SFLOAT,
		[graph_format_r64g64b64_uint]             = VK_FORMAT_R64G64B64_UINT,
		[graph_format_r64g64b64_sint]             = VK_FORMAT_R64G64B64_SINT,
		[graph_format_r64g64b64_sfloat]           = VK_FORMAT_R64G64B64_SFLOAT,
		[graph_format_r64g64b64a64_uint]          = VK_FORMAT_R64G64B64A64_UINT,
		[graph_format_r64g64b64a64_sint]          = VK_FORMAT_R64G64B64A64_SINT,
		[graph_format_r64g64b64a64_sfloat]        = VK_FORMAT_R64G64B64A64_SFLOAT,
		[graph_format_b10g11r11_ufloat_pack32]    = VK_FORMAT_B10G11R11_UFLOAT_PACK32,
		[graph_format_e5b9g9r9_ufloat_pack32]     = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32,
		[graph_format_d16_unorm]                  = VK_FORMAT_D16_UNORM,
		[graph_format_x8_d24_unorm_pack32]        = VK_FORMAT_X8_D24_UNORM_PACK32,
		[graph_format_d32_sfloat]                 = VK_FORMAT_D32_SFLOAT,
		[graph_format_s8_uint]                    = VK_FORMAT_S8_UINT,
		[graph_format_d16_unorm_s8_uint]          = VK_FORMAT_D16_UNORM_S8_UINT,
		[graph_format_d24_unorm_s8_uint]          = VK_FORMAT_D24_UNORM_S8_UINT,
		[graph_format_d32_sfloat_s8_uint]         = VK_FORMAT_D32_SFLOAT_S8_UINT
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_FORMAT_UNDEFINED;
}

const char* graph_format$string(register graph_format_t r)
{
	static const char *mapping[] = {
		#define def_string(_n)  [graph_format_##_n] = #_n
		def_string(unknow),
		def_string(r4g4_unorm_pack8),
		def_string(r4g4b4a4_unorm_pack16),
		def_string(b4g4r4a4_unorm_pack16),
		def_string(r5g6b5_unorm_pack16),
		def_string(b5g6r5_unorm_pack16),
		def_string(r5g5b5a1_unorm_pack16),
		def_string(b5g5r5a1_unorm_pack16),
		def_string(a1r5g5b5_unorm_pack16),
		def_string(r8_unorm),
		def_string(r8_snorm),
		def_string(r8_uscaled),
		def_string(r8_sscaled),
		def_string(r8_uint),
		def_string(r8_sint),
		def_string(r8_srgb),
		def_string(r8g8_unorm),
		def_string(r8g8_snorm),
		def_string(r8g8_uscaled),
		def_string(r8g8_sscaled),
		def_string(r8g8_uint),
		def_string(r8g8_sint),
		def_string(r8g8_srgb),
		def_string(r8g8b8_unorm),
		def_string(r8g8b8_snorm),
		def_string(r8g8b8_uscaled),
		def_string(r8g8b8_sscaled),
		def_string(r8g8b8_uint),
		def_string(r8g8b8_sint),
		def_string(r8g8b8_srgb),
		def_string(b8g8r8_unorm),
		def_string(b8g8r8_snorm),
		def_string(b8g8r8_uscaled),
		def_string(b8g8r8_sscaled),
		def_string(b8g8r8_uint),
		def_string(b8g8r8_sint),
		def_string(b8g8r8_srgb),
		def_string(r8g8b8a8_unorm),
		def_string(r8g8b8a8_snorm),
		def_string(r8g8b8a8_uscaled),
		def_string(r8g8b8a8_sscaled),
		def_string(r8g8b8a8_uint),
		def_string(r8g8b8a8_sint),
		def_string(r8g8b8a8_srgb),
		def_string(b8g8r8a8_unorm),
		def_string(b8g8r8a8_snorm),
		def_string(b8g8r8a8_uscaled),
		def_string(b8g8r8a8_sscaled),
		def_string(b8g8r8a8_uint),
		def_string(b8g8r8a8_sint),
		def_string(b8g8r8a8_srgb),
		def_string(a8b8g8r8_unorm_pack32),
		def_string(a8b8g8r8_snorm_pack32),
		def_string(a8b8g8r8_uscaled_pack32),
		def_string(a8b8g8r8_sscaled_pack32),
		def_string(a8b8g8r8_uint_pack32),
		def_string(a8b8g8r8_sint_pack32),
		def_string(a8b8g8r8_srgb_pack32),
		def_string(a2r10g10b10_unorm_pack32),
		def_string(a2r10g10b10_snorm_pack32),
		def_string(a2r10g10b10_uscaled_pack32),
		def_string(a2r10g10b10_sscaled_pack32),
		def_string(a2r10g10b10_uint_pack32),
		def_string(a2r10g10b10_sint_pack32),
		def_string(a2b10g10r10_unorm_pack32),
		def_string(a2b10g10r10_snorm_pack32),
		def_string(a2b10g10r10_uscaled_pack32),
		def_string(a2b10g10r10_sscaled_pack32),
		def_string(a2b10g10r10_uint_pack32),
		def_string(a2b10g10r10_sint_pack32),
		def_string(r16_unorm),
		def_string(r16_snorm),
		def_string(r16_uscaled),
		def_string(r16_sscaled),
		def_string(r16_uint),
		def_string(r16_sint),
		def_string(r16_sfloat),
		def_string(r16g16_unorm),
		def_string(r16g16_snorm),
		def_string(r16g16_uscaled),
		def_string(r16g16_sscaled),
		def_string(r16g16_uint),
		def_string(r16g16_sint),
		def_string(r16g16_sfloat),
		def_string(r16g16b16_unorm),
		def_string(r16g16b16_snorm),
		def_string(r16g16b16_uscaled),
		def_string(r16g16b16_sscaled),
		def_string(r16g16b16_uint),
		def_string(r16g16b16_sint),
		def_string(r16g16b16_sfloat),
		def_string(r16g16b16a16_unorm),
		def_string(r16g16b16a16_snorm),
		def_string(r16g16b16a16_uscaled),
		def_string(r16g16b16a16_sscaled),
		def_string(r16g16b16a16_uint),
		def_string(r16g16b16a16_sint),
		def_string(r16g16b16a16_sfloat),
		def_string(r32_uint),
		def_string(r32_sint),
		def_string(r32_sfloat),
		def_string(r32g32_uint),
		def_string(r32g32_sint),
		def_string(r32g32_sfloat),
		def_string(r32g32b32_uint),
		def_string(r32g32b32_sint),
		def_string(r32g32b32_sfloat),
		def_string(r32g32b32a32_uint),
		def_string(r32g32b32a32_sint),
		def_string(r32g32b32a32_sfloat),
		def_string(r64_uint),
		def_string(r64_sint),
		def_string(r64_sfloat),
		def_string(r64g64_uint),
		def_string(r64g64_sint),
		def_string(r64g64_sfloat),
		def_string(r64g64b64_uint),
		def_string(r64g64b64_sint),
		def_string(r64g64b64_sfloat),
		def_string(r64g64b64a64_uint),
		def_string(r64g64b64a64_sint),
		def_string(r64g64b64a64_sfloat),
		def_string(b10g11r11_ufloat_pack32),
		def_string(e5b9g9r9_ufloat_pack32),
		def_string(d16_unorm),
		def_string(x8_d24_unorm_pack32),
		def_string(d32_sfloat),
		def_string(s8_uint),
		def_string(d16_unorm_s8_uint),
		def_string(d24_unorm_s8_uint),
		def_string(d32_sfloat_s8_uint)
		#undef def_string
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return mapping[0];
}

const char* graph_color_space$string(register graph_color_space_t r)
{
	static const char *mapping[] = {
		[graph_color_space_srgb] = "srgb"
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return graph_string_unknow;
}

const char* graph_present_mode$string(register graph_present_mode_t r)
{
	static const char *mapping[] = {
		[graph_present_mode_immediate]    = graph_type_blue "immediate" graph_type_back,
		[graph_present_mode_mailbox]      = graph_type_green "mailbox" graph_type_back,
		[graph_present_mode_fifo]         = graph_type_yellow "fifo" graph_type_back,
		[graph_present_mode_fifo_relaxed] = graph_type_yellow "fifo_relaxed" graph_type_back
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return graph_string_unknow;
}

char* graph_queue_flags$list(register char *restrict s, register graph_queue_flags_t r)
{
	static const char *array[] = {
		graph_type_green "graphics" graph_type_back,
		graph_type_blue "compute" graph_type_back,
		"transfer",
		"sparse_binding",
		"protected"
	};
	return graph_type_list(s, r, array, graph_array_number(array));
}

char* graph_image_usage$list(register char *restrict s, register graph_image_usage_t r)
{
	static const char *array[] = {
		"transfer_src",
		"transfer_dst",
		"sampled",
		"storage",
		"color_attachment",
		"depth_stencil_attachment",
		"transient_attachment",
		"input_attachment",
		"shading_rate_image_nv",
		"fragment_density_map_ext"
	};
	return graph_type_list(s, r, array, graph_array_number(array));
}

graph_image_view_type_t graph_image_view_type4vk(register VkImageViewType r)
{
	static const graph_image_view_type_t mapping[graph_image_view_type$number] = {
		[VK_IMAGE_VIEW_TYPE_1D]         = graph_image_view_type_1D,
		[VK_IMAGE_VIEW_TYPE_2D]         = graph_image_view_type_2D,
		[VK_IMAGE_VIEW_TYPE_3D]         = graph_image_view_type_3D,
		[VK_IMAGE_VIEW_TYPE_CUBE]       = graph_image_view_type_cube,
		[VK_IMAGE_VIEW_TYPE_1D_ARRAY]   = graph_image_view_type_1D_array,
		[VK_IMAGE_VIEW_TYPE_2D_ARRAY]   = graph_image_view_type_2D_array,
		[VK_IMAGE_VIEW_TYPE_CUBE_ARRAY] = graph_image_view_type_cube_array
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return graph_image_view_type$number;
}

VkImageViewType graph_image_view_type2vk(register graph_image_view_type_t r)
{
	static const VkImageViewType mapping[graph_image_view_type$number] = {
		[graph_image_view_type_1D]         = VK_IMAGE_VIEW_TYPE_1D,
		[graph_image_view_type_2D]         = VK_IMAGE_VIEW_TYPE_2D,
		[graph_image_view_type_3D]         = VK_IMAGE_VIEW_TYPE_3D,
		[graph_image_view_type_cube]       = VK_IMAGE_VIEW_TYPE_CUBE,
		[graph_image_view_type_1D_array]   = VK_IMAGE_VIEW_TYPE_1D_ARRAY,
		[graph_image_view_type_2D_array]   = VK_IMAGE_VIEW_TYPE_2D_ARRAY,
		[graph_image_view_type_cube_array] = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_IMAGE_VIEW_TYPE_MAX_ENUM;
}

char* graph_surface_transform$list(register char *restrict s, register graph_surface_transform_t r)
{
	static const char *array[] = {
		"identity",
		"r90",
		"r180",
		"r270",
		"hmirror",
		"hmirror_r90",
		"hmirror_r180",
		"hmirror_r270"
	};
	return graph_type_list(s, r, array, graph_array_number(array));
}

char* graph_composite_alpha$list(register char *restrict s, register graph_composite_alpha_t r)
{
	static const char *array[] = {
		"opaque",
		"pre_multiplied",
		"post_multiplied",
		"inherit"
	};
	return graph_type_list(s, r, array, graph_array_number(array));
}

VkVertexInputRate graph_vertex_input_rate2vk(register graph_vertex_input_rate_t r)
{
	static const VkVertexInputRate mapping[graph_vertex_input_rate$number] = {
		[graph_vertex_input_rate_vertex]   = VK_VERTEX_INPUT_RATE_VERTEX,
		[graph_vertex_input_rate_instance] = VK_VERTEX_INPUT_RATE_INSTANCE
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_VERTEX_INPUT_RATE_MAX_ENUM;
}

VkPrimitiveTopology graph_primitive_topology2vk(register graph_primitive_topology_t r)
{
	static const VkPrimitiveTopology mapping[graph_primitive_topology$number] = {
		[graph_primitive_topology_point_list]                    = VK_PRIMITIVE_TOPOLOGY_POINT_LIST,
		[graph_primitive_topology_line_list]                     = VK_PRIMITIVE_TOPOLOGY_LINE_LIST,
		[graph_primitive_topology_line_strip]                    = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP,
		[graph_primitive_topology_triangle_list]                 = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
		[graph_primitive_topology_triangle_strip]                = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
		[graph_primitive_topology_triangle_fan]                  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN,
		[graph_primitive_topology_line_list_with_adjacency]      = VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY,
		[graph_primitive_topology_line_strip_with_adjacency]     = VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY,
		[graph_primitive_topology_triangle_list_with_adjacency]  = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY,
		[graph_primitive_topology_triangle_strip_with_adjacency] = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY,
		[graph_primitive_topology_patch_list]                    = VK_PRIMITIVE_TOPOLOGY_PATCH_LIST
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_PRIMITIVE_TOPOLOGY_MAX_ENUM;
}

VkPolygonMode graph_polygon_mode2vk(register graph_polygon_mode_t r)
{
	static const VkPolygonMode mapping[graph_polygon_mode$number] = {
		[graph_polygon_mode_fill]  = VK_POLYGON_MODE_FILL,
		[graph_polygon_mode_line]  = VK_POLYGON_MODE_LINE,
		[graph_polygon_mode_point] = VK_POLYGON_MODE_POINT
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_POLYGON_MODE_MAX_ENUM;
}

VkBlendFactor graph_blend_factor2vk(register graph_blend_factor_t r)
{
	static const VkBlendFactor mapping[graph_blend_factor$number] = {
		[graph_blend_factor_zero]                     = VK_BLEND_FACTOR_ZERO,
		[graph_blend_factor_one]                      = VK_BLEND_FACTOR_ONE,
		[graph_blend_factor_src_color]                = VK_BLEND_FACTOR_SRC_COLOR,
		[graph_blend_factor_one_minus_src_color]      = VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
		[graph_blend_factor_dst_color]                = VK_BLEND_FACTOR_DST_COLOR,
		[graph_blend_factor_one_minus_dst_color]      = VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
		[graph_blend_factor_src_alpha]                = VK_BLEND_FACTOR_SRC_ALPHA,
		[graph_blend_factor_one_minus_src_alpha]      = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
		[graph_blend_factor_dst_alpha]                = VK_BLEND_FACTOR_DST_ALPHA,
		[graph_blend_factor_one_minus_dst_alpha]      = VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
		[graph_blend_factor_constant_color]           = VK_BLEND_FACTOR_CONSTANT_COLOR,
		[graph_blend_factor_one_minus_constant_color] = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
		[graph_blend_factor_constant_alpha]           = VK_BLEND_FACTOR_CONSTANT_ALPHA,
		[graph_blend_factor_one_minus_constant_alpha] = VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_ALPHA,
		[graph_blend_factor_src_alpha_saturate]       = VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
		[graph_blend_factor_src1_color]               = VK_BLEND_FACTOR_SRC1_COLOR,
		[graph_blend_factor_one_minus_src1_color]     = VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
		[graph_blend_factor_src1_alpha]               = VK_BLEND_FACTOR_SRC1_ALPHA,
		[graph_blend_factor_one_minus_src1_alpha]     = VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_BLEND_FACTOR_ZERO;
}

VkBlendOp graph_blend_op2vk(register graph_blend_op_t r)
{
	static const VkBlendOp mapping[graph_blend_op$number] = {
		[graph_blend_op_add]              = VK_BLEND_OP_ADD,
		[graph_blend_op_subtract]         = VK_BLEND_OP_SUBTRACT,
		[graph_blend_op_reverse_subtract] = VK_BLEND_OP_REVERSE_SUBTRACT,
		[graph_blend_op_min]              = VK_BLEND_OP_MIN,
		[graph_blend_op_max]              = VK_BLEND_OP_MAX
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_BLEND_OP_ADD;
}

VkLogicOp graph_logic_op2vk(register graph_logic_op_t r)
{
	static const VkLogicOp mapping[graph_logic_op$number] = {
		[graph_logic_op_clear]         = VK_LOGIC_OP_CLEAR,
		[graph_logic_op_and]           = VK_LOGIC_OP_AND,
		[graph_logic_op_and_reverse]   = VK_LOGIC_OP_AND_REVERSE,
		[graph_logic_op_copy]          = VK_LOGIC_OP_COPY,
		[graph_logic_op_and_inverted]  = VK_LOGIC_OP_AND_INVERTED,
		[graph_logic_op_no_op]         = VK_LOGIC_OP_NO_OP,
		[graph_logic_op_xor]           = VK_LOGIC_OP_XOR,
		[graph_logic_op_or]            = VK_LOGIC_OP_OR,
		[graph_logic_op_nor]           = VK_LOGIC_OP_NOR,
		[graph_logic_op_equivalent]    = VK_LOGIC_OP_EQUIVALENT,
		[graph_logic_op_invert]        = VK_LOGIC_OP_INVERT,
		[graph_logic_op_or_reverse]    = VK_LOGIC_OP_OR_REVERSE,
		[graph_logic_op_copy_inverted] = VK_LOGIC_OP_COPY_INVERTED,
		[graph_logic_op_or_inverted]   = VK_LOGIC_OP_OR_INVERTED,
		[graph_logic_op_nand]          = VK_LOGIC_OP_NAND,
		[graph_logic_op_set]           = VK_LOGIC_OP_SET
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_LOGIC_OP_CLEAR;
}

VkAttachmentLoadOp graph_attachment_load_op2vk(register graph_attachment_load_op_t r)
{
	static const VkAttachmentLoadOp mapping[graph_attachment_load_op$number] = {
		[graph_attachment_load_op_load]  = VK_ATTACHMENT_LOAD_OP_LOAD,
		[graph_attachment_load_op_clear] = VK_ATTACHMENT_LOAD_OP_CLEAR,
		[graph_attachment_load_op_none]  = VK_ATTACHMENT_LOAD_OP_DONT_CARE
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
}

VkAttachmentStoreOp graph_attachment_store_op2vk(register graph_attachment_store_op_t r)
{
	static const VkAttachmentStoreOp mapping[graph_attachment_store_op$number] = {
		[graph_attachment_store_op_store] = VK_ATTACHMENT_STORE_OP_STORE,
		[graph_attachment_store_op_none]  = VK_ATTACHMENT_STORE_OP_DONT_CARE
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_ATTACHMENT_STORE_OP_DONT_CARE;
}

VkImageLayout graph_image_layout2vk(register graph_image_layout_t r)
{
	static const VkImageLayout mapping[graph_image_layout$number] = {
		[graph_image_layout_undefined]                        = VK_IMAGE_LAYOUT_UNDEFINED,
		[graph_image_layout_general]                          = VK_IMAGE_LAYOUT_GENERAL,
		[graph_image_layout_color_attachment_optimal]         = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		[graph_image_layout_depth_stencil_attachment_optimal] = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
		[graph_image_layout_depth_stencil_read_only_optimal]  = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
		[graph_image_layout_shader_read_only_optimal]         = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		[graph_image_layout_transfer_src_optimal]             = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		[graph_image_layout_transfer_dst_optimal]             = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		[graph_image_layout_preinitialized]                   = VK_IMAGE_LAYOUT_PREINITIALIZED,
		[graph_image_layout_khr_present_src]                  = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_IMAGE_LAYOUT_UNDEFINED;
}

VkDescriptorType graph_desc_type2vk(register graph_desc_type_t r)
{
	static const VkDescriptorType mapping[graph_desc_type$number] = {
		[graph_desc_type_sampler]                = VK_DESCRIPTOR_TYPE_SAMPLER,
		[graph_desc_type_combined_image_sampler] = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
		[graph_desc_type_sampled_image]          = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
		[graph_desc_type_storage_image]          = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,
		[graph_desc_type_uniform_texel_buffer]   = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER,
		[graph_desc_type_storage_texel_buffer]   = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER,
		[graph_desc_type_uniform_buffer]         = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
		[graph_desc_type_storage_buffer]         = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
		[graph_desc_type_uniform_buffer_dynamic] = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC,
		[graph_desc_type_storage_buffer_dynamic] = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER_DYNAMIC,
		[graph_desc_type_input_attachment]       = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_DESCRIPTOR_TYPE_MAX_ENUM;
}

VkPipelineBindPoint graph_pipeline_bind_point2vk(register graph_pipeline_bind_point_t r)
{
	static const VkPipelineBindPoint mapping[graph_pipeline_bind_point$number] = {
		[graph_pipeline_bind_point_graphics] = VK_PIPELINE_BIND_POINT_GRAPHICS,
		[graph_pipeline_bind_point_compute]  = VK_PIPELINE_BIND_POINT_COMPUTE
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_PIPELINE_BIND_POINT_GRAPHICS;
}

VkDynamicState graph_dynamic2vk(register graph_dynamic_t r)
{
	static const VkDynamicState mapping[graph_dynamic$number] = {
		[graph_dynamic_viewport]             = VK_DYNAMIC_STATE_VIEWPORT,
		[graph_dynamic_scissor]              = VK_DYNAMIC_STATE_SCISSOR,
		[graph_dynamic_line_width]           = VK_DYNAMIC_STATE_LINE_WIDTH,
		[graph_dynamic_depth_bias]           = VK_DYNAMIC_STATE_DEPTH_BIAS,
		[graph_dynamic_blend_constants]      = VK_DYNAMIC_STATE_BLEND_CONSTANTS,
		[graph_dynamic_depth_bounds]         = VK_DYNAMIC_STATE_DEPTH_BOUNDS,
		[graph_dynamic_stencil_compare_mask] = VK_DYNAMIC_STATE_STENCIL_COMPARE_MASK,
		[graph_dynamic_stencil_write_mask]   = VK_DYNAMIC_STATE_STENCIL_WRITE_MASK,
		[graph_dynamic_stencil_reference]    = VK_DYNAMIC_STATE_STENCIL_REFERENCE
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_DYNAMIC_STATE_MAX_ENUM;
}

VkSubpassContents graph_subpass_contents2vk(register graph_subpass_contents_t r)
{
	static const VkSubpassContents mapping[graph_subpass_contents$number] = {
		[graph_subpass_contents_inline]    = VK_SUBPASS_CONTENTS_INLINE,
		[graph_subpass_contents_secondary] = VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_SUBPASS_CONTENTS_INLINE;
}

VkIndexType graph_index_type2vk(register graph_index_type_t r)
{
	static const VkIndexType mapping[graph_index_type$number] = {
		[graph_index_type_uint16] = VK_INDEX_TYPE_UINT16,
		[graph_index_type_uint32] = VK_INDEX_TYPE_UINT32
	};
	if ((uint32_t) r < graph_array_number(mapping))
		return mapping[r];
	return VK_INDEX_TYPE_MAX_ENUM;
}
