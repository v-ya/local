#ifndef _abc_z_x86_64_h_
#define _abc_z_x86_64_h_

#include <stdint.h>
#include "adora.h"

// iset

// type

typedef enum abc_z_type_t abc_z_type_t;

enum abc_z_type_t {
	abc_z_type__imm,
	abc_z_type__disp,
	abc_z_type__base,
	abc_z_type__index,
	abc_z_type__r8,
	abc_z_type__r16,
	abc_z_type__r32,
	abc_z_type__r64,
};

// reg

typedef enum abc_z_reg_t abc_z_reg_t;

#define abc_z_reg(_type, _reg)        abc_z_reg__##_type##__##_reg
#define abc_z_reg_index(_type, _reg)  (abc_z_reg__##_type##__##_reg - abc_z_reg_min_##_type - 1)
#define abc_z_reg_count(_type)        (abc_z_reg_max_##_type - abc_z_reg_min_##_type - 1)

enum abc_z_reg_t {
	#define d_reg_begin(_type)  abc_z_reg_min_##_type = (abc_z_type__##_type << 16)
	#define d_reg_end(_type)    abc_z_reg_max_##_type
	d_reg_begin(base),
	abc_z_reg(base, rax),
	abc_z_reg(base, rcx),
	abc_z_reg(base, rdx),
	abc_z_reg(base, rbx),
	abc_z_reg(base, rsp),
	abc_z_reg(base, rbp),
	abc_z_reg(base, rsi),
	abc_z_reg(base, rdi),
	abc_z_reg(base, r8),
	abc_z_reg(base, r9),
	abc_z_reg(base, r10),
	abc_z_reg(base, r11),
	abc_z_reg(base, r12),
	abc_z_reg(base, r13),
	abc_z_reg(base, r14),
	abc_z_reg(base, r15),
	d_reg_end(base),
	d_reg_begin(index),
	abc_z_reg(index, rax),
	abc_z_reg(index, rcx),
	abc_z_reg(index, rdx),
	abc_z_reg(index, rbx),
	abc_z_reg(index, none),
	abc_z_reg(index, rbp),
	abc_z_reg(index, rsi),
	abc_z_reg(index, rdi),
	abc_z_reg(index, r8),
	abc_z_reg(index, r9),
	abc_z_reg(index, r10),
	abc_z_reg(index, r11),
	abc_z_reg(index, r12),
	abc_z_reg(index, r13),
	abc_z_reg(index, r14),
	abc_z_reg(index, r15),
	d_reg_end(index),
	d_reg_begin(r8),
	abc_z_reg(r8, al),
	abc_z_reg(r8, cl),
	abc_z_reg(r8, dl),
	abc_z_reg(r8, bl),
	abc_z_reg(r8, spl),
	abc_z_reg(r8, bpl),
	abc_z_reg(r8, sil),
	abc_z_reg(r8, dil),
	abc_z_reg(r8, r8l),
	abc_z_reg(r8, r9l),
	abc_z_reg(r8, r10l),
	abc_z_reg(r8, r11l),
	abc_z_reg(r8, r12l),
	abc_z_reg(r8, r13l),
	abc_z_reg(r8, r14l),
	abc_z_reg(r8, r15l),
	abc_z_reg(r8, ah),
	abc_z_reg(r8, ch),
	abc_z_reg(r8, dh),
	abc_z_reg(r8, bh),
	d_reg_end(r8),
	d_reg_begin(r16),
	abc_z_reg(r16, ax),
	abc_z_reg(r16, cx),
	abc_z_reg(r16, dx),
	abc_z_reg(r16, bx),
	abc_z_reg(r16, sp),
	abc_z_reg(r16, bp),
	abc_z_reg(r16, si),
	abc_z_reg(r16, di),
	abc_z_reg(r16, r8w),
	abc_z_reg(r16, r9w),
	abc_z_reg(r16, r10w),
	abc_z_reg(r16, r11w),
	abc_z_reg(r16, r12w),
	abc_z_reg(r16, r13w),
	abc_z_reg(r16, r14w),
	abc_z_reg(r16, r15w),
	d_reg_end(r16),
	d_reg_begin(r32),
	abc_z_reg(r32, eax),
	abc_z_reg(r32, ecx),
	abc_z_reg(r32, edx),
	abc_z_reg(r32, ebx),
	abc_z_reg(r32, esp),
	abc_z_reg(r32, ebp),
	abc_z_reg(r32, esi),
	abc_z_reg(r32, edi),
	abc_z_reg(r32, r8d),
	abc_z_reg(r32, r9d),
	abc_z_reg(r32, r10d),
	abc_z_reg(r32, r11d),
	abc_z_reg(r32, r12d),
	abc_z_reg(r32, r13d),
	abc_z_reg(r32, r14d),
	abc_z_reg(r32, r15d),
	d_reg_end(r32),
	d_reg_begin(r64),
	abc_z_reg(r64, rax),
	abc_z_reg(r64, rcx),
	abc_z_reg(r64, rdx),
	abc_z_reg(r64, rbx),
	abc_z_reg(r64, rsp),
	abc_z_reg(r64, rbp),
	abc_z_reg(r64, rsi),
	abc_z_reg(r64, rdi),
	abc_z_reg(r64, r8),
	abc_z_reg(r64, r9),
	abc_z_reg(r64, r10),
	abc_z_reg(r64, r11),
	abc_z_reg(r64, r12),
	abc_z_reg(r64, r13),
	abc_z_reg(r64, r14),
	abc_z_reg(r64, r15),
	d_reg_end(r64),
	#undef d_reg_begin
	#undef d_reg_end
};

// instr

typedef enum abc_z_instr_t abc_z_instr_t;

enum abc_z_instr_t {
	abc_z_instr_move = 0x00000000,
	abc_z_instr_arithmetic = 0x00010000,
	abc_z_instr_logic = 0x00020000,
	abc_z_instr_control = 0x00030000,
	abc_z_instr_string = 0x00040000,
	abc_z_instr_synchronization = 0x00050000,
	abc_z_instr_other = 0x00060000,
};

// adora

void abc_z_adora_isa_initial__x86_64(abc_adora_isa_s *restrict isa, const abc_adora_isa_t *restrict func);

#endif
