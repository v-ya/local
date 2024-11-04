#include "adora.h"

/*
[Instruction Prefixes] [REX] <Opcode> [ModR/M] [SIB] [Displacement] [Immediate];

// [REX]       <Opcode>  [ModR/M]    [SIB]
// 0b0100WRXB  <Opcode>  0bmmxxxyyy  0bssiiibbb
// 0b0100WR0B  <Opcode>  0bmmxxxyyy              =>  Mod: mm, Reg/Opcode: Rxxx, R/M: Byyy
// 0b0100WRXB  <Opcode>  0bmmxxxyyy  0bssiiibbb  =>  Mod: mm != 11, Reg/Opcode: Rxxx, R/M: yyy == 100, Scale: ss, Index: Xiii, Base: Bbbb

// Reg:          r8   r16   r32   r64    mm   xmm   opcode
//	 000:    al    ax   eax   rax   mm0  xmm0        0
//	 001:    cl    cx   ecx   rcx   mm1  xmm1        1
//	 010:    dl    dx   edx   rdx   mm2  xmm2        2
//	 011:    bl    bx   ebx   rbx   mm3  xmm3        3
//	 100:    ah    sp   esp   rsp   mm4  xmm4        4
//	 101:    ch    bp   ebp   rbp   mm5  xmm5        5
//	 110:    dh    si   esi   rsi   mm6  xmm6        6
//	 111:    bh    di   edi   rdi   mm7  xmm7        7

// Reg:          r8   r16   r32   r64    mm   xmm   opcode
//	0000:    al    ax   eax   rax   mm0  xmm0        0
//	0001:    cl    cx   ecx   rcx   mm1  xmm1        1
//	0010:    dl    dx   edx   rdx   mm2  xmm2        2
//	0011:    bl    bx   ebx   rbx   mm3  xmm3        3
//	0100:   spl    sp   esp   rsp   mm4  xmm4        4
//	0101:   bpl    bp   ebp   rbp   mm5  xmm5        5
//	0110:   sil    si   esi   rsi   mm6  xmm6        6
//	0111:   dil    di   edi   rdi   mm7  xmm7        7
//	1000:   r8l   r8w   r8d    r8        xmm8        8
//	1001:   r9l   r9w   r9d    r9        xmm9        9
//	1010:  r10l  r10w  r10d   r10       xmm10       10
//	1011:  r11l  r11w  r11d   r11       xmm11       11
//	1100:  r12l  r12w  r12d   r12       xmm12       12
//	1101:  r13l  r13w  r13d   r13       xmm13       13
//	1110:  r14l  r14w  r14d   r14       xmm14       14
//	1111:  r15l  r15w  r15d   r15       xmm15       15

// ModR/M:
//	R/M\Mod     00            01             10    11
//	0000:    [rax]   [rax]+disp8   [rax]+disp32   Reg
//	0001:    [rcx]   [rcx]+disp8   [rcx]+disp32   Reg
//	0010:    [rdx]   [rdx]+disp8   [rdx]+disp32   Reg
//	0011:    [rbx]   [rbx]+disp8   [rbx]+disp32   Reg
//	0100:    [SIB]   [SIB]+disp8   [SIB]+disp32   Reg
//	0101:    [rip]   [rbp]+disp8   [rbp]+disp32   Reg
//	0110:    [rsi]   [rsi]+disp8   [rsi]+disp32   Reg
//	0111:    [rdi]   [rdi]+disp8   [rdi]+disp32   Reg
//	1000:     [r8]    [r8]+disp8    [r8]+disp32   Reg
//	1001:     [r9]    [r9]+disp8    [r9]+disp32   Reg
//	1010:    [r10]   [r10]+disp8   [r10]+disp32   Reg
//	1011:    [r11]   [r11]+disp8   [r11]+disp32   Reg
//	1100:    [r12]   [r12]+disp8   [r12]+disp32   Reg
//	1101:    [r13]   [r13]+disp8   [r13]+disp32   Reg
//	1110:    [r14]   [r14]+disp8   [r14]+disp32   Reg
//	1111:    [r15]   [r15]+disp8   [r15]+disp32   Reg

// SIB:
// SS     = 0 (1), 1 (2), 2 (4), 3 (8)
// Index  = 4 (none), other Reg
// Base   = 5 (*), other Reg
// ModR/M = [Base + Index * SS]
// Base == 5:
//	Mod = 00, ModR/M = [Index * SS] + disp32
//	Mod = 01, ModR/M = [Index * SS + rbp] + disp8
//	Mod = 10, ModR/M = [Index * SS + rbp] + disp32

[Instruction Prefixes]:
	uint8_t prefix[];
[REX]:
	// 0b0100WRXB:
	// W: 1 => 64 bits operand-size
	// R: extension R/M
	// X: extension Index
	// B: extension R/M, Base, Reg/Opcode
	uint8_t rex;
<Opcode>:
	union {
		uint8_t u1[1];
		uint8_t u2[2];
		uint8_t u3[3];
	} opcode;
[ModR/M]:
	// 0b-----210: R/M
	// 0b--543---: Reg/Opcode
	// 0b76------: Mod
	uint8_t modrm;
[SIB]:
	// 0b-----210: Base (register number)
	// 0b--543---: Index (register number)
	// 0b76------: Scale (1, 2, 4, 8)
	uint8_t sib;
[Displacement][Immediate]:
	// [Displacement] address offset
	// [Immediate] const number
	union {
		void s0;
		int8_t s1;
		int16_t s2;
		int32_t s4;
	} data;
*/

/*
[Instruction Prefixes]:
	group-1:
		- lock memory
		f0: <LOCK>
		- used only with (MOVS, CMPS, SCAS, LODS, STOS, INS, OUTS) instructions
		f2: <REPNE> <REPNZ>
		f3: <REP> <REPE> <REPZ>
		- used with (near CALL, near RET, near JMP, short Jcc, near Jcc) instruction
		f2: <BND>
	group-2:
		- segment override prefixes: use with any branch instruction is reserved
		26: <ES>
		2e: <CS>
		36: <SS>
		3e: <DS>
		64: <FS>
		65: <GS>
		- branch hints: used only with Jcc instructions
		2e: <branch not taken>
		3e: <branch taken>
	group-3:
		- the prefix selects the non-default size
		66: <operand-size>
	group-4:
		- the prefix selects the non-default size
		67: <address-size>
*/

/*
<Opcode>:
	(66|f2|f3) 0f -- --
	0f -- --
	(66|f2|f3) 0f --
	0f --
	--
*/

/*
NP:    not used prefix 66/f2/f3
NFx:   not used prefix f2/f3
REX.W: to 64 bits
*/

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
