#ifndef _elfin_r_x86_64_h_
#define _elfin_r_x86_64_h_

enum elfin_r_x86_64_t {
	elfin_r_x86_64__none            =  0, // No reloc
	elfin_r_x86_64__64              =  1, // Direct 64 bit
	elfin_r_x86_64__pc32            =  2, // PC relative 32 bit signed
	elfin_r_x86_64__got32           =  3, // 32 bit GOT entry
	elfin_r_x86_64__plt32           =  4, // 32 bit PLT address
	elfin_r_x86_64__copy            =  5, // Copy symbol at runtime
	elfin_r_x86_64__glob_dat        =  6, // Create GOT entry
	elfin_r_x86_64__jump_slot       =  7, // Create PLT entry
	elfin_r_x86_64__relative        =  8, // Adjust by program base
	elfin_r_x86_64__gotpcrel        =  9, // 32 bit signed PC relative offset to GOT
	elfin_r_x86_64__32              = 10, // Direct 32 bit zero extended
	elfin_r_x86_64__32s             = 11, // Direct 32 bit sign extended
	elfin_r_x86_64__16              = 12, // Direct 16 bit zero extended
	elfin_r_x86_64__pc16            = 13, // 16 bit sign extended pc relative
	elfin_r_x86_64__8               = 14, // Direct 8 bit sign extended
	elfin_r_x86_64__pc8             = 15, // 8 bit sign extended pc relative
	elfin_r_x86_64__dtpmod64        = 16, // ID of module containing symbol
	elfin_r_x86_64__dtpoff64        = 17, // Offset in module's TLS block
	elfin_r_x86_64__tpoff64         = 18, // Offset in initial TLS block
	elfin_r_x86_64__tlsgd           = 19, // 32 bit signed PC relative offset to two GOT entries for GD symbol
	elfin_r_x86_64__tlsld           = 20, // 32 bit signed PC relative offset to two GOT entries for LD symbol
	elfin_r_x86_64__dtpoff32        = 21, // Offset in TLS block
	elfin_r_x86_64__gottpoff        = 22, // 32 bit signed PC relative offset to GOT entry for IE symbol
	elfin_r_x86_64__tpoff32         = 23, // Offset in initial TLS block
	elfin_r_x86_64__pc64            = 24, // PC relative 64 bit
	elfin_r_x86_64__gotoff64        = 25, // 64 bit offset to GOT
	elfin_r_x86_64__gotpc32         = 26, // 32 bit signed pc relative offset to GOT
	elfin_r_x86_64__got64           = 27, // 64-bit GOT entry offset
	elfin_r_x86_64__gotpcrel64      = 28, // 64-bit PC relative offset to GOT entry
	elfin_r_x86_64__gotpc64         = 29, // 64-bit PC relative offset to GOT
	elfin_r_x86_64__gotplt64        = 30, // like GOT64, says PLT entry needed
	elfin_r_x86_64__pltoff64        = 31, // 64-bit GOT relative offset to PLT entry
	elfin_r_x86_64__size32          = 32, // Size of symbol plus 32-bit addend
	elfin_r_x86_64__size64          = 33, // Size of symbol plus 64-bit addend
	elfin_r_x86_64__gotpc32_tlsdesc = 34, // GOT offset for TLS descriptor
	elfin_r_x86_64__tlsdesc_call    = 35, // Marker for call through TLS descriptor
	elfin_r_x86_64__tlsdesc         = 36, // TLS descriptor
	elfin_r_x86_64__irelative       = 37, // Adjust indirectly by program base
	elfin_r_x86_64__relative64      = 38, // 64-bit adjust by program base
	elfin_r_x86_64__pc32_bnd        = 39, // 39 Reserved was R_X86_64_PC32_BND
	elfin_r_x86_64__plt32_bnd       = 40, // 40 Reserved was R_X86_64_PLT32_BND
	elfin_r_x86_64__gotpcrelx       = 41, // Load from 32 bit signed pc relative offset to GOT entry without REX prefix, relaxable
	elfin_r_x86_64__rex_gotpcrelx   = 42, // Load from 32 bit signed pc relative offset to GOT entry with REX prefix, relaxable
	elfin_r_x86_64_number           = 43
};

#endif
