#ifndef _abc_z_adora_h_
#define _abc_z_adora_h_

#include "../z.x86_64.h"

typedef enum z_operand_size_t z_operand_size_t;
typedef enum z_operand_encode_t z_operand_encode_t;

enum z_operand_size_t {
	z_operand_size__0,
	z_operand_size__1,
	z_operand_size__2,
	z_operand_size__4,
	z_operand_size__8,
};

// I imm
// M reg/mem
// R reg
enum z_operand_encode_t {
	z_operand_encode__I,
	z_operand_encode__MI,
	z_operand_encode__MR,
	z_operand_encode__RM,
};

// adora.id

uint64_t abc_z_instr_id(z_operand_size_t opsize, z_operand_encode_t opcode, uint64_t instr);

#endif
