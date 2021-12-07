#include "elfin.def.h"
#include <memory.h>

static const char *const s_undefine = "?";

const char* elfin_inner_get__ident_class(uint32_t v)
{
	static const char *const a[elfin_ident_class_number] = {
		[elfin_ident_class__none] = "none",
		[elfin_ident_class__32]   = "32-bits",
		[elfin_ident_class__64]   = "64-bits",
	};
	if (v < elfin_ident_class_number)
		return a[v];
	return s_undefine;
}

const char* elfin_inner_get__ident_data(uint32_t v)
{
	static const char *const a[elfin_ident_data_number] = {
		[elfin_ident_data__none]  = "none",
		[elfin_ident_data__2c_le] = "2's complement, little endian",
		[elfin_ident_data__2c_be] = "2's complement, big endian",
	};
	if (v < elfin_ident_data_number)
		return a[v];
	return s_undefine;
}

const char* elfin_inner_get__ident_osabi(uint32_t v)
{
	switch (v)
	{
		case elfin_ident_osabi__sysv:       return "UNIX System V ABI";
		case elfin_ident_osabi__hpux:       return "HP-UX";
		case elfin_ident_osabi__netbsd:     return "NetBSD";
		case elfin_ident_osabi__gnu:        return "Object uses GNU ELF extensions";
		case elfin_ident_osabi__solaris:    return "Sun Solaris";
		case elfin_ident_osabi__aix:        return "IBM AIX";
		case elfin_ident_osabi__irix:       return "SGI Irix";
		case elfin_ident_osabi__freebsd:    return "FreeBSD";
		case elfin_ident_osabi__tru64:      return "Compaq TRU64 UNIX";
		case elfin_ident_osabi__modesto:    return "Novell Modesto";
		case elfin_ident_osabi__openbsd:    return "OpenBSD";
		case elfin_ident_osabi__arm_aeabi:  return "ARM EABI";
		case elfin_ident_osabi__arm:        return "ARM";
		case elfin_ident_osabi__standalone: return "Standalone (embedded) application";
		default: return s_undefine;
	}
}

const char* elfin_inner_get__type(uint32_t v)
{
	static const char *const a[elfin_type_number] = {
		[elfin_type__none] = "none",
		[elfin_type__rel]  = "relocatable",
		[elfin_type__exec] = "executable",
		[elfin_type__dyn]  = "shared object",
		[elfin_type__core] = "core",
	};
	if (v < elfin_type_number)
		return a[v];
	return s_undefine;
}

const char* elfin_inner_get__machine(uint32_t v)
{
	static const char *const a[elfin_machine_number] = {
		[elfin_machine__none]          = "No machine",
		[elfin_machine__m32]           = "AT&T WE 32100",
		[elfin_machine__sparc]         = "SUN SPARC",
		[elfin_machine__386]           = "Intel 80386",
		[elfin_machine__68k]           = "Motorola m68k family",
		[elfin_machine__88k]           = "Motorola m88k family",
		[elfin_machine__iamcu]         = "Intel MCU",
		[elfin_machine__860]           = "Intel 80860",
		[elfin_machine__mips]          = "MIPS R3000 big-endian",
		[elfin_machine__s370]          = "IBM System/370",
		[elfin_machine__mips_rs3_le]   = "MIPS R3000 little-endian",
		[elfin_machine__parisc]        = "HPPA",
		[elfin_machine__vpp500]        = "Fujitsu VPP500",
		[elfin_machine__sparc32plus]   = "Sun's \"v8plus\"",
		[elfin_machine__960]           = "Intel 80960",
		[elfin_machine__ppc]           = "PowerPC",
		[elfin_machine__ppc64]         = "PowerPC 64-bit",
		[elfin_machine__s390]          = "IBM S390",
		[elfin_machine__spu]           = "IBM SPU/SPC",
		[elfin_machine__v800]          = "NEC V800 series",
		[elfin_machine__fr20]          = "Fujitsu FR20",
		[elfin_machine__rh32]          = "TRW RH-32",
		[elfin_machine__rce]           = "Motorola RCE",
		[elfin_machine__arm]           = "ARM",
		[elfin_machine__fake_alpha]    = "Digital Alpha",
		[elfin_machine__sh]            = "Hitachi SH",
		[elfin_machine__sparcv9]       = "SPARC v9 64-bit",
		[elfin_machine__tricore]       = "Siemens Tricore",
		[elfin_machine__arc]           = "Argonaut RISC Core",
		[elfin_machine__h8_300]        = "Hitachi H8/300",
		[elfin_machine__h8_300h]       = "Hitachi H8/300H",
		[elfin_machine__h8s]           = "Hitachi H8S",
		[elfin_machine__h8_500]        = "Hitachi H8/500",
		[elfin_machine__ia_64]         = "Intel Merced",
		[elfin_machine__mips_x]        = "Stanford MIPS-X",
		[elfin_machine__coldfire]      = "Motorola Coldfire",
		[elfin_machine__68hc12]        = "Motorola M68HC12",
		[elfin_machine__mma]           = "Fujitsu MMA Multimedia Accelerator",
		[elfin_machine__pcp]           = "Siemens PCP",
		[elfin_machine__ncpu]          = "Sony nCPU embeeded RISC",
		[elfin_machine__ndr1]          = "Denso NDR1 microprocessor",
		[elfin_machine__starcore]      = "Motorola Start*Core processor",
		[elfin_machine__me16]          = "Toyota ME16 processor",
		[elfin_machine__st100]         = "STMicroelectronic ST100 processor",
		[elfin_machine__tinyj]         = "Advanced Logic Corp. Tinyj emb.fam",
		[elfin_machine__x86_64]        = "AMD x86-64 architecture",
		[elfin_machine__pdsp]          = "Sony DSP Processor",
		[elfin_machine__pdp10]         = "Digital PDP-10",
		[elfin_machine__pdp11]         = "Digital PDP-11",
		[elfin_machine__fx66]          = "Siemens FX66 microcontroller",
		[elfin_machine__st9plus]       = "STMicroelectronics ST9+ 8/16 mc",
		[elfin_machine__st7]           = "STmicroelectronics ST7 8 bit mc",
		[elfin_machine__68hc16]        = "Motorola MC68HC16 microcontroller",
		[elfin_machine__68hc11]        = "Motorola MC68HC11 microcontroller",
		[elfin_machine__68hc08]        = "Motorola MC68HC08 microcontroller",
		[elfin_machine__68hc05]        = "Motorola MC68HC05 microcontroller",
		[elfin_machine__svx]           = "Silicon Graphics SVx",
		[elfin_machine__st19]          = "STMicroelectronics ST19 8 bit mc",
		[elfin_machine__vax]           = "Digital VAX",
		[elfin_machine__cris]          = "Axis Communications 32-bit emb.proc",
		[elfin_machine__javelin]       = "Infineon Technologies 32-bit emb.proc",
		[elfin_machine__firepath]      = "Element 14 64-bit DSP Processor",
		[elfin_machine__zsp]           = "LSI Logic 16-bit DSP Processor",
		[elfin_machine__mmix]          = "Donald Knuth's educational 64-bit proc",
		[elfin_machine__huany]         = "Harvard University machine-independent object files",
		[elfin_machine__prism]         = "SiTera Prism",
		[elfin_machine__avr]           = "Atmel AVR 8-bit microcontroller",
		[elfin_machine__fr30]          = "Fujitsu FR30",
		[elfin_machine__d10v]          = "Mitsubishi D10V",
		[elfin_machine__d30v]          = "Mitsubishi D30V",
		[elfin_machine__v850]          = "NEC v850",
		[elfin_machine__m32r]          = "Mitsubishi M32R",
		[elfin_machine__mn10300]       = "Matsushita MN10300",
		[elfin_machine__mn10200]       = "Matsushita MN10200",
		[elfin_machine__pj]            = "picoJava",
		[elfin_machine__openrisc]      = "OpenRISC 32-bit embedded processor",
		[elfin_machine__arc_compact]   = "ARC International ARCompact",
		[elfin_machine__xtensa]        = "Tensilica Xtensa Architecture",
		[elfin_machine__videocore]     = "Alphamosaic VideoCore",
		[elfin_machine__tmm_gpp]       = "Thompson Multimedia General Purpose Proc",
		[elfin_machine__ns32k]         = "National Semi. 32000",
		[elfin_machine__tpc]           = "Tenor Network TPC",
		[elfin_machine__snp1k]         = "Trebia SNP 1000",
		[elfin_machine__st200]         = "STMicroelectronics ST200",
		[elfin_machine__ip2k]          = "Ubicom IP2xxx",
		[elfin_machine__max]           = "MAX processor",
		[elfin_machine__cr]            = "National Semi. CompactRISC",
		[elfin_machine__f2mc16]        = "Fujitsu F2MC16",
		[elfin_machine__msp430]        = "Texas Instruments msp430",
		[elfin_machine__blackfin]      = "Analog Devices Blackfin DSP",
		[elfin_machine__se_c33]        = "Seiko Epson S1C33 family",
		[elfin_machine__sep]           = "Sharp embedded microprocessor",
		[elfin_machine__arca]          = "Arca RISC",
		[elfin_machine__unicore]       = "PKU-Unity & MPRC Peking Uni. mc series",
		[elfin_machine__excess]        = "eXcess configurable cpu",
		[elfin_machine__dxp]           = "Icera Semi. Deep Execution Processor",
		[elfin_machine__altera_nios2]  = "Altera Nios II",
		[elfin_machine__crx]           = "National Semi. CompactRISC CRX",
		[elfin_machine__xgate]         = "Motorola XGATE",
		[elfin_machine__c166]          = "Infineon C16x/XC16x",
		[elfin_machine__m16c]          = "Renesas M16C",
		[elfin_machine__dspic30f]      = "Microchip Technology dsPIC30F",
		[elfin_machine__ce]            = "Freescale Communication Engine RISC",
		[elfin_machine__m32c]          = "Renesas M32C",
		[elfin_machine__tsk3000]       = "Altium TSK3000",
		[elfin_machine__rs08]          = "Freescale RS08",
		[elfin_machine__sharc]         = "Analog Devices SHARC family",
		[elfin_machine__ecog2]         = "Cyan Technology eCOG2",
		[elfin_machine__score7]        = "Sunplus S+core7 RISC",
		[elfin_machine__dsp24]         = "New Japan Radio (NJR) 24-bit DSP",
		[elfin_machine__videocore3]    = "Broadcom VideoCore III",
		[elfin_machine__latticemico32] = "RISC for Lattice FPGA",
		[elfin_machine__se_c17]        = "Seiko Epson C17",
		[elfin_machine__ti_c6000]      = "Texas Instruments TMS320C6000 DSP",
		[elfin_machine__ti_c2000]      = "Texas Instruments TMS320C2000 DSP",
		[elfin_machine__ti_c5500]      = "Texas Instruments TMS320C55x DSP",
		[elfin_machine__ti_arp32]      = "Texas Instruments App. Specific RISC",
		[elfin_machine__ti_pru]        = "Texas Instruments Prog. Realtime Unit",
		[elfin_machine__mmdsp_plus]    = "STMicroelectronics 64bit VLIW DSP",
		[elfin_machine__cypress_m8c]   = "Cypress M8C",
		[elfin_machine__r32c]          = "Renesas R32C",
		[elfin_machine__trimedia]      = "NXP Semi. TriMedia",
		[elfin_machine__qdsp6]         = "QUALCOMM DSP6",
		[elfin_machine__8051]          = "Intel 8051 and variants",
		[elfin_machine__stxp7x]        = "STMicroelectronics STxP7x",
		[elfin_machine__nds32]         = "Andes Tech. compact code emb. RISC",
		[elfin_machine__ecog1x]        = "Cyan Technology eCOG1X",
		[elfin_machine__maxq30]        = "Dallas Semi. MAXQ30 mc",
		[elfin_machine__ximo16]        = "New Japan Radio (NJR) 16-bit DSP",
		[elfin_machine__manik]         = "M2000 Reconfigurable RISC",
		[elfin_machine__craynv2]       = "Cray NV2 vector architecture",
		[elfin_machine__rx]            = "Renesas RX",
		[elfin_machine__metag]         = "Imagination Tech. META",
		[elfin_machine__mcst_elbrus]   = "MCST Elbrus",
		[elfin_machine__ecog16]        = "Cyan Technology eCOG16",
		[elfin_machine__cr16]          = "National Semi. CompactRISC CR16",
		[elfin_machine__etpu]          = "Freescale Extended Time Processing Unit",
		[elfin_machine__sle9x]         = "Infineon Tech. SLE9X",
		[elfin_machine__l10m]          = "Intel L10M",
		[elfin_machine__k10m]          = "Intel K10M",
		[elfin_machine__aarch64]       = "ARM AARCH64",
		[elfin_machine__avr32]         = "Amtel 32-bit microprocessor",
		[elfin_machine__stm8]          = "STMicroelectronics STM8",
		[elfin_machine__tile64]        = "Tilera TILE64",
		[elfin_machine__tilepro]       = "Tilera TILEPro",
		[elfin_machine__microblaze]    = "Xilinx MicroBlaze",
		[elfin_machine__cuda]          = "NVIDIA CUDA",
		[elfin_machine__tilegx]        = "Tilera TILE-Gx",
		[elfin_machine__cloudshield]   = "CloudShield",
		[elfin_machine__corea_1st]     = "KIPO-KAIST Core-A 1st gen.",
		[elfin_machine__corea_2nd]     = "KIPO-KAIST Core-A 2nd gen.",
		[elfin_machine__arcv2]         = "Synopsys ARCv2 ISA. ",
		[elfin_machine__open8]         = "Open8 RISC",
		[elfin_machine__rl78]          = "Renesas RL78",
		[elfin_machine__videocore5]    = "Broadcom VideoCore V",
		[elfin_machine__78kor]         = "Renesas 78KOR",
		[elfin_machine__56800ex]       = "Freescale 56800EX DSC",
		[elfin_machine__ba1]           = "Beyond BA1",
		[elfin_machine__ba2]           = "Beyond BA2",
		[elfin_machine__xcore]         = "XMOS xCORE",
		[elfin_machine__mchp_pic]      = "Microchip 8-bit PIC(r)",
		[elfin_machine__km32]          = "KM211 KM32",
		[elfin_machine__kmx32]         = "KM211 KMX32",
		[elfin_machine__emx16]         = "KM211 KMX16",
		[elfin_machine__emx8]          = "KM211 KMX8",
		[elfin_machine__kvarc]         = "KM211 KVARC",
		[elfin_machine__cdp]           = "Paneve CDP",
		[elfin_machine__coge]          = "Cognitive Smart Memory Processor",
		[elfin_machine__cool]          = "Bluechip CoolEngine",
		[elfin_machine__norc]          = "Nanoradio Optimized RISC",
		[elfin_machine__csr_kalimba]   = "CSR Kalimba",
		[elfin_machine__z80]           = "Zilog Z80",
		[elfin_machine__visium]        = "Controls and Data Services VISIUMcore",
		[elfin_machine__ft32]          = "FTDI Chip FT32",
		[elfin_machine__moxie]         = "Moxie processor",
		[elfin_machine__amdgpu]        = "AMD GPU",
		[elfin_machine__riscv]         = "RISC-V",
		[elfin_machine__bpf]           = "Linux BPF -- in-kernel virtual machine",
		[elfin_machine__csky]          = "C-SKY",
	};
	if (v < elfin_machine_number)
		return a[v]?a[v]:s_undefine;
	return s_undefine;
}

const char* elfin_inner_get__section_type(uint32_t v)
{
	static const char *const a[elfin_section_type_number] = {
		[elfin_section_type__null]               = "Section header table entry unused",
		[elfin_section_type__program_bits]       = "Program data",
		[elfin_section_type__symbol_table]       = "Symbol table",
		[elfin_section_type__string_table]       = "String table",
		[elfin_section_type__rela]               = "Relocation entries with addends",
		[elfin_section_type__hash]               = "Symbol hash table",
		[elfin_section_type__dynamic]            = "Dynamic linking information",
		[elfin_section_type__note]               = "Notes",
		[elfin_section_type__no_bits]            = "Program space with no data (bss)",
		[elfin_section_type__rel]                = "Relocation entries, no addends",
		[elfin_section_type__shlib]              = "Reserved",
		[elfin_section_type__dynamic_symbol]     = "Dynamic linker symbol table",
		[elfin_section_type__init_array]         = "Array of constructors",
		[elfin_section_type__fini_array]         = "Array of destructors",
		[elfin_section_type__preinit_array]      = "Array of pre-constructors",
		[elfin_section_type__group]              = "Section group",
		[elfin_section_type__symbol_table_shndx] = "Extended section indices",
	};
	if (v < elfin_section_type_number)
		return a[v];
	return s_undefine;
}

const char* elfin_inner_get__section_flags(uint64_t flags, char *restrict buffer)
{
	static const struct {
		uintptr_t length;
		const char *name;
	} fa[32] = {
		#define d_flag(_i, _n)  [_i] = {.length = sizeof(_n) - 1, .name = _n}
		d_flag(0,  "write"),
		d_flag(1,  "alloc"),
		d_flag(2,  "execinstr"),
		d_flag(4,  "merge"),
		d_flag(5,  "strings"),
		d_flag(6,  "info_link"),
		d_flag(7,  "link_order"),
		d_flag(8,  "os_nonconforming"),
		d_flag(9,  "group"),
		d_flag(10, "tls"),
		d_flag(11, "compressed"),
		d_flag(21, "gnu_retain"),
		d_flag(30, "ordered"),
		d_flag(31, "exclude"),
		#undef d_flag
	};
	uintptr_t n;
	n = 0;
	for (uint32_t i = 0; i < 32; ++i)
	{
		if ((flags & ((uint64_t) 1 << i)) && fa[i].name)
		{
			memcpy(buffer + n, fa[i].name, fa[i].length);
			n += fa[i].length;
			buffer[n++] = ' ';
		}
	}
	if (n) --n;
	buffer[n] = 0;
	return buffer;
}

const char* elfin_inner_get__section_res_index(uint32_t v)
{
	switch (v)
	{
		case elfin_section_res_index__undef:  return "(undef)";
		case elfin_section_res_index__before: return "(before)";
		case elfin_section_res_index__after:  return "(after)";
		case elfin_section_res_index__abs:    return "(abs)";
		case elfin_section_res_index__common: return "(common)";
		case elfin_section_res_index__xindex: return "(xindex)";
		default: return s_undefine;
	}
}

const char* elfin_inner_get__symbol_bind(uint32_t v)
{
	static const char *const a[elfin_symbol_bind_number] = {
		[elfin_symbol_bind__local]  = "local",
		[elfin_symbol_bind__global] = "global",
		[elfin_symbol_bind__weak]   = "weak",
	};
	if (v < elfin_symbol_bind_number)
		return a[v];
	return s_undefine;
}

const char* elfin_inner_get__symbol_type(uint32_t v)
{
	static const char *const a[elfin_symbol_type_number] = {
		[elfin_symbol_type__no_type] = "notype",
		[elfin_symbol_type__object]  = "object",
		[elfin_symbol_type__func]    = "func",
		[elfin_symbol_type__section] = "section",
		[elfin_symbol_type__file]    = "file",
		[elfin_symbol_type__common]  = "common",
		[elfin_symbol_type__tls]     = "tls",
	};
	if (v < elfin_symbol_type_number)
		return a[v];
	return s_undefine;
}

const char* elfin_inner_get__symbol_visibility(uint32_t v)
{
	static const char *const a[elfin_symbol_visibility_number] = {
		[elfin_symbol_visibility__default]   = "default",
		[elfin_symbol_visibility__internal]  = "internal",
		[elfin_symbol_visibility__hidden]    = "hidden",
		[elfin_symbol_visibility__protected] = "protected",
	};
	if (v < elfin_symbol_visibility_number)
		return a[v];
	return s_undefine;
}
