#include "adora.h"

uint64_t abc_z_instr_id(z_operand_size_t opsize, z_operand_encode_t opcode, uint64_t instr)
{
	return ((uint64_t) (opsize & 0xffff) << 48) | ((uint64_t) (opcode & 0xffff) << 32) | (instr & 0xffffffff);
}
