#ifndef _elfin_def_h_
#define _elfin_def_h_

#include <stdint.h>

struct elfin_indent_t {
	uint8_t magic[4];     // 0x7f, 'E', 'L', 'F'
	uint8_t class;        // (enum elfin_indent_class_t)
	uint8_t data;         // (enum elfin_indent_data_t)
	uint8_t version;      // (enum elfin_version_t) must => elfin_version__current
	uint8_t osabi;        // (enum elfin_indent_osabi_t)
	uint8_t abiversion;   // abi version
	uint8_t pad[7];
};

enum elfin_indent_class_t {
	elfin_indent_class__none  = 0, // invalid
	elfin_indent_class__32    = 1, // 32 bits
	elfin_indent_class__64    = 2, // 64 bits
	elfin_indent_class_number = 3
};

enum elfin_indent_data_t {
	elfin_indent_data__none  = 0, // invalid
	elfin_indent_data__2c_le = 1, // 负数补码 小端序
	elfin_indent_data__2c_be = 2, // 负数补码 大端序
	elfin_indent_data_number = 3
};

enum elfin_indent_osabi_t {
	elfin_indent_osabi__sysv       =   0, // UNIX System V ABI
	elfin_indent_osabi__hpux       =   1, // HP-UX
	elfin_indent_osabi__netbsd     =   2, // NetBSD
	elfin_indent_osabi__gnu        =   3, // Object uses GNU ELF extensions
	elfin_indent_osabi__linux      = elfin_indent_osabi__gnu,
	elfin_indent_osabi__solaris    =   6, // Sun Solaris
	elfin_indent_osabi__aix        =   7, // IBM AIX
	elfin_indent_osabi__irix       =   8, // SGI Irix
	elfin_indent_osabi__freebsd    =   9, // FreeBSD
	elfin_indent_osabi__tru64      =  10, // Compaq TRU64 UNIX
	elfin_indent_osabi__modesto    =  11, // Novell Modesto
	elfin_indent_osabi__openbsd    =  12, // OpenBSD
	elfin_indent_osabi__arm_aeabi  =  64, // ARM EABI
	elfin_indent_osabi__arm        =  97, // ARM
	elfin_indent_osabi__standalone = 255, // Standalone (embedded) application
};

enum elfin_type_t {
	elfin_type__none           = 0, // no file type
	elfin_type__rel            = 1, // relocatable file
	elfin_type__exec           = 2, // executable file
	elfin_type__dyn            = 3, // shared object file
	elfin_type__core           = 4, // core file
	elfin_type_number          = 5, // number of defined types
	elfin_type__os_low         = 0xfe00, // OS-specific range start
	elfin_type__os_high        = 0xfeff, // OS-specific range end
	elfin_type__processor_low  = 0xff00, // Processor-specific range start
	elfin_type__processor_high = 0xffff, // Processor-specific range end
};

enum elfin_machine_t {
	elfin_machine__none          =   0, // No machine
	elfin_machine__m32           =   1, // AT&T WE 32100
	elfin_machine__sparc         =   2, // SUN SPARC
	elfin_machine__386           =   3, // Intel 80386
	elfin_machine__68k           =   4, // Motorola m68k family
	elfin_machine__88k           =   5, // Motorola m88k family
	elfin_machine__iamcu         =   6, // Intel MCU
	elfin_machine__860           =   7, // Intel 80860
	elfin_machine__mips          =   8, // MIPS R3000 big-endian
	elfin_machine__s370          =   9, // IBM System/370
	elfin_machine__mips_rs3_le   =  10, // MIPS R3000 little-endian
	                                    // reserved 11-14
	elfin_machine__parisc        =  15, // HPPA
	                                    // reserved 16
	elfin_machine__vpp500        =  17, // Fujitsu VPP500
	elfin_machine__sparc32plus   =  18, // Sun's "v8plus"
	elfin_machine__960           =  19, // Intel 80960
	elfin_machine__ppc           =  20, // PowerPC
	elfin_machine__ppc64         =  21, // PowerPC 64-bit
	elfin_machine__s390          =  22, // IBM S390
	elfin_machine__spu           =  23, // IBM SPU/SPC
	                                    // reserved 24-35
	elfin_machine__v800          =  36, // NEC V800 series
	elfin_machine__fr20          =  37, // Fujitsu FR20
	elfin_machine__rh32          =  38, // TRW RH-32
	elfin_machine__rce           =  39, // Motorola RCE
	elfin_machine__arm           =  40, // ARM
	elfin_machine__fake_alpha    =  41, // Digital Alpha
	elfin_machine__sh            =  42, // Hitachi SH
	elfin_machine__sparcv9       =  43, // SPARC v9 64-bit
	elfin_machine__tricore       =  44, // Siemens Tricore
	elfin_machine__arc           =  45, // Argonaut RISC Core
	elfin_machine__h8_300        =  46, // Hitachi H8/300
	elfin_machine__h8_300h       =  47, // Hitachi H8/300H
	elfin_machine__h8s           =  48, // Hitachi H8S
	elfin_machine__h8_500        =  49, // Hitachi H8/500
	elfin_machine__ia_64         =  50, // Intel Merced
	elfin_machine__mips_x        =  51, // Stanford MIPS-X
	elfin_machine__coldfire      =  52, // Motorola Coldfire
	elfin_machine__68hc12        =  53, // Motorola M68HC12
	elfin_machine__mma           =  54, // Fujitsu MMA Multimedia Accelerator
	elfin_machine__pcp           =  55, // Siemens PCP
	elfin_machine__ncpu          =  56, // Sony nCPU embeeded RISC
	elfin_machine__ndr1          =  57, // Denso NDR1 microprocessor
	elfin_machine__starcore      =  58, // Motorola Start*Core processor
	elfin_machine__me16          =  59, // Toyota ME16 processor
	elfin_machine__st100         =  60, // STMicroelectronic ST100 processor
	elfin_machine__tinyj         =  61, // Advanced Logic Corp. Tinyj emb.fam
	elfin_machine__x86_64        =  62, // AMD x86-64 architecture
	elfin_machine__pdsp          =  63, // Sony DSP Processor
	elfin_machine__pdp10         =  64, // Digital PDP-10
	elfin_machine__pdp11         =  65, // Digital PDP-11
	elfin_machine__fx66          =  66, // Siemens FX66 microcontroller
	elfin_machine__st9plus       =  67, // STMicroelectronics ST9+ 8/16 mc
	elfin_machine__st7           =  68, // STmicroelectronics ST7 8 bit mc
	elfin_machine__68hc16        =  69, // Motorola MC68HC16 microcontroller
	elfin_machine__68hc11        =  70, // Motorola MC68HC11 microcontroller
	elfin_machine__68hc08        =  71, // Motorola MC68HC08 microcontroller
	elfin_machine__68hc05        =  72, // Motorola MC68HC05 microcontroller
	elfin_machine__svx           =  73, // Silicon Graphics SVx
	elfin_machine__st19          =  74, // STMicroelectronics ST19 8 bit mc
	elfin_machine__vax           =  75, // Digital VAX
	elfin_machine__cris          =  76, // Axis Communications 32-bit emb.proc
	elfin_machine__javelin       =  77, // Infineon Technologies 32-bit emb.proc
	elfin_machine__firepath      =  78, // Element 14 64-bit DSP Processor
	elfin_machine__zsp           =  79, // LSI Logic 16-bit DSP Processor
	elfin_machine__mmix          =  80, // Donald Knuth's educational 64-bit proc
	elfin_machine__huany         =  81, // Harvard University machine-independent object files
	elfin_machine__prism         =  82, // SiTera Prism
	elfin_machine__avr           =  83, // Atmel AVR 8-bit microcontroller
	elfin_machine__fr30          =  84, // Fujitsu FR30
	elfin_machine__d10v          =  85, // Mitsubishi D10V
	elfin_machine__d30v          =  86, // Mitsubishi D30V
	elfin_machine__v850          =  87, // NEC v850
	elfin_machine__m32r          =  88, // Mitsubishi M32R
	elfin_machine__mn10300       =  89, // Matsushita MN10300
	elfin_machine__mn10200       =  90, // Matsushita MN10200
	elfin_machine__pj            =  91, // picoJava
	elfin_machine__openrisc      =  92, // OpenRISC 32-bit embedded processor
	elfin_machine__arc_compact   =  93, // ARC International ARCompact
	elfin_machine__arc_a5        = elfin_machine__arc_compact,
	elfin_machine__xtensa        =  94, // Tensilica Xtensa Architecture
	elfin_machine__videocore     =  95, // Alphamosaic VideoCore
	elfin_machine__tmm_gpp       =  96, // Thompson Multimedia General Purpose Proc
	elfin_machine__ns32k         =  97, // National Semi. 32000
	elfin_machine__tpc           =  98, // Tenor Network TPC
	elfin_machine__snp1k         =  99, // Trebia SNP 1000
	elfin_machine__st200         = 100, // STMicroelectronics ST200
	elfin_machine__ip2k          = 101, // Ubicom IP2xxx
	elfin_machine__max           = 102, // MAX processor
	elfin_machine__cr            = 103, // National Semi. CompactRISC
	elfin_machine__f2mc16        = 104, // Fujitsu F2MC16
	elfin_machine__msp430        = 105, // Texas Instruments msp430
	elfin_machine__blackfin      = 106, // Analog Devices Blackfin DSP
	elfin_machine__se_c33        = 107, // Seiko Epson S1C33 family
	elfin_machine__sep           = 108, // Sharp embedded microprocessor
	elfin_machine__arca          = 109, // Arca RISC
	elfin_machine__unicore       = 110, // PKU-Unity & MPRC Peking Uni. mc series
	elfin_machine__excess        = 111, // eXcess configurable cpu
	elfin_machine__dxp           = 112, // Icera Semi. Deep Execution Processor
	elfin_machine__altera_nios2  = 113, // Altera Nios II
	elfin_machine__crx           = 114, // National Semi. CompactRISC CRX
	elfin_machine__xgate         = 115, // Motorola XGATE
	elfin_machine__c166          = 116, // Infineon C16x/XC16x
	elfin_machine__m16c          = 117, // Renesas M16C
	elfin_machine__dspic30f      = 118, // Microchip Technology dsPIC30F
	elfin_machine__ce            = 119, // Freescale Communication Engine RISC
	elfin_machine__m32c          = 120, // Renesas M32C
	                                    // reserved 121-130
	elfin_machine__tsk3000       = 131, // Altium TSK3000
	elfin_machine__rs08          = 132, // Freescale RS08
	elfin_machine__sharc         = 133, // Analog Devices SHARC family
	elfin_machine__ecog2         = 134, // Cyan Technology eCOG2
	elfin_machine__score7        = 135, // Sunplus S+core7 RISC
	elfin_machine__dsp24         = 136, // New Japan Radio (NJR) 24-bit DSP
	elfin_machine__videocore3    = 137, // Broadcom VideoCore III
	elfin_machine__latticemico32 = 138, // RISC for Lattice FPGA
	elfin_machine__se_c17        = 139, // Seiko Epson C17
	elfin_machine__ti_c6000      = 140, // Texas Instruments TMS320C6000 DSP
	elfin_machine__ti_c2000      = 141, // Texas Instruments TMS320C2000 DSP
	elfin_machine__ti_c5500      = 142, // Texas Instruments TMS320C55x DSP
	elfin_machine__ti_arp32      = 143, // Texas Instruments App. Specific RISC
	elfin_machine__ti_pru        = 144, // Texas Instruments Prog. Realtime Unit
	                                    // reserved 145-159
	elfin_machine__mmdsp_plus    = 160, // STMicroelectronics 64bit VLIW DSP
	elfin_machine__cypress_m8c   = 161, // Cypress M8C
	elfin_machine__r32c          = 162, // Renesas R32C
	elfin_machine__trimedia      = 163, // NXP Semi. TriMedia
	elfin_machine__qdsp6         = 164, // QUALCOMM DSP6
	elfin_machine__8051          = 165, // Intel 8051 and variants
	elfin_machine__stxp7x        = 166, // STMicroelectronics STxP7x
	elfin_machine__nds32         = 167, // Andes Tech. compact code emb. RISC
	elfin_machine__ecog1x        = 168, // Cyan Technology eCOG1X
	elfin_machine__maxq30        = 169, // Dallas Semi. MAXQ30 mc
	elfin_machine__ximo16        = 170, // New Japan Radio (NJR) 16-bit DSP
	elfin_machine__manik         = 171, // M2000 Reconfigurable RISC
	elfin_machine__craynv2       = 172, // Cray NV2 vector architecture
	elfin_machine__rx            = 173, // Renesas RX
	elfin_machine__metag         = 174, // Imagination Tech. META
	elfin_machine__mcst_elbrus   = 175, // MCST Elbrus
	elfin_machine__ecog16        = 176, // Cyan Technology eCOG16
	elfin_machine__cr16          = 177, // National Semi. CompactRISC CR16
	elfin_machine__etpu          = 178, // Freescale Extended Time Processing Unit
	elfin_machine__sle9x         = 179, // Infineon Tech. SLE9X
	elfin_machine__l10m          = 180, // Intel L10M
	elfin_machine__k10m          = 181, // Intel K10M
	                                    // reserved 182
	elfin_machine__aarch64       = 183, // ARM AARCH64
	                                    // reserved 184
	elfin_machine__avr32         = 185, // Amtel 32-bit microprocessor
	elfin_machine__stm8          = 186, // STMicroelectronics STM8
	elfin_machine__tile64        = 187, // Tilera TILE64
	elfin_machine__tilepro       = 188, // Tilera TILEPro
	elfin_machine__microblaze    = 189, // Xilinx MicroBlaze
	elfin_machine__cuda          = 190, // NVIDIA CUDA
	elfin_machine__tilegx        = 191, // Tilera TILE-Gx
	elfin_machine__cloudshield   = 192, // CloudShield
	elfin_machine__corea_1st     = 193, // KIPO-KAIST Core-A 1st gen.
	elfin_machine__corea_2nd     = 194, // KIPO-KAIST Core-A 2nd gen.
	elfin_machine__arcv2         = 195, // Synopsys ARCv2 ISA. 
	elfin_machine__open8         = 196, // Open8 RISC
	elfin_machine__rl78          = 197, // Renesas RL78
	elfin_machine__videocore5    = 198, // Broadcom VideoCore V
	elfin_machine__78kor         = 199, // Renesas 78KOR
	elfin_machine__56800ex       = 200, // Freescale 56800EX DSC
	elfin_machine__ba1           = 201, // Beyond BA1
	elfin_machine__ba2           = 202, // Beyond BA2
	elfin_machine__xcore         = 203, // XMOS xCORE
	elfin_machine__mchp_pic      = 204, // Microchip 8-bit PIC(r)
	                                    // reserved 205-209
	elfin_machine__km32          = 210, // KM211 KM32
	elfin_machine__kmx32         = 211, // KM211 KMX32
	elfin_machine__emx16         = 212, // KM211 KMX16
	elfin_machine__emx8          = 213, // KM211 KMX8
	elfin_machine__kvarc         = 214, // KM211 KVARC
	elfin_machine__cdp           = 215, // Paneve CDP
	elfin_machine__coge          = 216, // Cognitive Smart Memory Processor
	elfin_machine__cool          = 217, // Bluechip CoolEngine
	elfin_machine__norc          = 218, // Nanoradio Optimized RISC
	elfin_machine__csr_kalimba   = 219, // CSR Kalimba
	elfin_machine__z80           = 220, // Zilog Z80
	elfin_machine__visium        = 221, // Controls and Data Services VISIUMcore
	elfin_machine__ft32          = 222, // FTDI Chip FT32
	elfin_machine__moxie         = 223, // Moxie processor
	elfin_machine__amdgpu        = 224, // AMD GPU
	                                    // reserved 225-242
	elfin_machine__riscv         = 243, // RISC-V
	elfin_machine__bpf           = 247, // Linux BPF -- in-kernel virtual machine
	elfin_machine__csky          = 252, // C-SKY
	elfin_machine_number         = 253,

	elfin_machine__alpha         = 0x9026,
};

enum elfin_version_t {
	elfin_version__none    = 0, // invalid
	elfin_version__current = 1, // 当前版本
	elfin_version_number   = 2
};

enum elfin_section_type_t {
	elfin_section_type__null               =  0,         // Section header table entry unused
	elfin_section_type__program_bits       =  1,         // Program data
	elfin_section_type__symbol_table       =  2,         // Symbol table
	elfin_section_type__string_table       =  3,         // String table
	elfin_section_type__rela               =  4,         // Relocation entries with addends
	elfin_section_type__hash               =  5,         // Symbol hash table
	elfin_section_type__dynamic            =  6,         // Dynamic linking information
	elfin_section_type__note               =  7,         // Notes
	elfin_section_type__no_bits            =  8,         // Program space with no data (bss)
	elfin_section_type__rel                =  9,         // Relocation entries, no addends
	elfin_section_type__shlib              = 10,         // Reserved
	elfin_section_type__dynamic_symbol     = 11,         // Dynamic linker symbol table
	elfin_section_type__init_array         = 14,         // Array of constructors
	elfin_section_type__fini_array         = 15,         // Array of destructors
	elfin_section_type__preinit_array      = 16,         // Array of pre-constructors
	elfin_section_type__group              = 17,         // Section group
	elfin_section_type__symbol_table_shndx = 18,         // Extended section indices
	elfin_section_type_number              = 19,         // Number of defined types
	elfin_section_type__os_low             = 0x60000000, // Start OS-specific
	elfin_section_type__gnu_attributes     = 0x6ffffff5, // Object attributes
	elfin_section_type__gnu_hash           = 0x6ffffff6, // GNU-style hash table
	elfin_section_type__gnu_liblist        = 0x6ffffff7, // Prelink library list
	elfin_section_type__checksum           = 0x6ffffff8, // Checksum for DSO content
	elfin_section_type__sunw_low           = 0x6ffffffa, // Sun-specific low bound
	elfin_section_type__sunw_move          = 0x6ffffffa,
	elfin_section_type__sunw_comdat        = 0x6ffffffb,
	elfin_section_type__sunw_syminfo       = 0x6ffffffc,
	elfin_section_type__gnu_version_def    = 0x6ffffffd, // Version definition section
	elfin_section_type__gnu_version_need   = 0x6ffffffe, // Version needs section
	elfin_section_type__gnu_version_symbol = 0x6fffffff, // Version symbol table
	elfin_section_type__sunw_high          = 0x6fffffff, // Sun-specific high bound
	elfin_section_type__os_high            = 0x6fffffff, // End OS-specific type
	elfin_section_type__processor_low      = 0x70000000, // Start of processor-specific
	elfin_section_type__processor_high     = 0x7fffffff, // End of processor-specific
	elfin_section_type__user_low           = 0x80000000, // Start of application-specific
	elfin_section_type__user_high          = 0x8fffffff, // End of application-specific
};

enum elfin_section_flag_t {
	elfin_section_flag__write            = (1u <<  0), // Writable
	elfin_section_flag__alloc            = (1u <<  1), // Occupies memory during execution
	elfin_section_flag__execinstr        = (1u <<  2), // Executable
	elfin_section_flag__merge            = (1u <<  4), // Might be merged
	elfin_section_flag__strings          = (1u <<  5), // Contains nul-terminated strings
	elfin_section_flag__info_link        = (1u <<  6), // 'sh_info' contains SHT index
	elfin_section_flag__link_order       = (1u <<  7), // Preserve order after combining
	elfin_section_flag__os_nonconforming = (1u <<  8), // Non-standard OS specific handling required
	elfin_section_flag__group            = (1u <<  9), // Section is member of a group
	elfin_section_flag__tls              = (1u << 10), // Section hold thread-local data
	elfin_section_flag__compressed       = (1u << 11), // Section with compressed data
	elfin_section_flag__gnu_retain       = (1u << 21), // Not to be GCed by linker
	elfin_section_flag__ordered          = (1u << 30), // Special ordering requirement (Solaris)
	elfin_section_flag__exclude          = (1u << 31), // Section is excluded unless referenced or allocated (Solaris)
	elfin_section_flag__mask_os          = 0x0ff00000, // OS-specific
	elfin_section_flag__mask_processor   = 0xf0000000, // Processor-specific
};

enum elfin_symbol_bind_t {
	elfin_symbol_bind__local          =  0, // Local symbol
	elfin_symbol_bind__global         =  1, // Global symbol
	elfin_symbol_bind__weak           =  2, // Weak symbol
	elfin_symbol_bind_number          =  3, // Number of defined types
	elfin_symbol_bind__os_low         = 10, // Start of OS-specific
	elfin_symbol_bind__gnu_unique     = 10, // Unique symbol
	elfin_symbol_bind__os_high        = 12, // End of OS-specific
	elfin_symbol_bind__processor_low  = 13, // Start of processor-specific
	elfin_symbol_bind__processor_high = 15, // End of processor-specific
};

enum elfin_symbol_type_t {
	elfin_symbol_type__no_type        =  0, // Symbol type is unspecified
	elfin_symbol_type__object         =  1, // Symbol is a data object
	elfin_symbol_type__func           =  2, // Symbol is a code object
	elfin_symbol_type__section        =  3, // Symbol associated with a section
	elfin_symbol_type__file           =  4, // Symbol's name is file name
	elfin_symbol_type__common         =  5, // Symbol is a common data object
	elfin_symbol_type__tls            =  6, // Symbol is thread-local data objec
	elfin_symbol_type_number          =  7, // Number of defined types
	elfin_symbol_type__os_low         = 10, // Start of OS-specific
	elfin_symbol_type__gnu_ifunc      = 10, // Symbol is indirect code object
	elfin_symbol_type__os_high        = 12, // End of OS-specific
	elfin_symbol_type__processor_low  = 13, // Start of processor-specific
	elfin_symbol_type__processor_high = 15, // End of processor-specific
};

#endif
