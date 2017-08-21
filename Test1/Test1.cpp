// Test1.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <cstdint>
#include <map>
#include "cpu.h"
#include "instructionMask.h"
#include "thumbs.h"

CPU cpu;
uint32_t binary_data;

/**
	FUNCTION PROTOTYPES
*/
__forceinline uint32_t immediate(const uint32_t);
__forceinline unsigned int decode_cond(uint32_t);
__forceinline void branch_exchange(const uint32_t);
__forceinline void branch_and_branch_with_link(const uint32_t);
__forceinline void decode_opcode(const uint32_t);
__forceinline uint32_t logical_shift_left(uint32_t, unsigned int, uint32_t);
__forceinline uint32_t logical_shift_right(uint32_t, unsigned int, uint32_t);
__forceinline int32_t arithmetic_shift_right(int32_t, unsigned int, uint32_t);
__forceinline uint32_t rotate_right(uint32_t, unsigned int, uint32_t);
__forceinline uint32_t rotate_right_extended(uint32_t, uint32_t);
__forceinline uint32_t immediate(const uint32_t);

__forceinline uint32_t arm_and(uint32_t, uint32_t);
__forceinline uint32_t arm_add(uint32_t, uint32_t);

/**
31-28 Cond
27-4 mask
11-0 Operand2
*/
union BRANCH_EXCHANGE
{
	uint32_t op;
	struct {
		uint32_t cond : 4;
		uint32_t __mask0 : 24;
		uint32_t _Rn : 4;
	};
};

static_assert(sizeof(BRANCH_EXCHANGE) == 4, "sizeof(BRANCH_EXCHANGE) == 4");

/**
31-28 Cond
27-25 mask
24 Link Bit
23-0 offset
*/
union BRANCH_AND_BRANCH_WITH_LINK
{
	uint32_t op;
	struct {
		uint32_t cond : 4;
		uint32_t __mask0 : 3;
		uint32_t _L : 1;
		uint32_t offset : 24;
	};
};

static_assert(sizeof(BRANCH_AND_BRANCH_WITH_LINK) == 4, "sizeof(BRANCH_AND_BRANCH_WITH_LINK) == 4");

/**
31-28 Cond
27-26 0 Mask
25 Immediate Operand
24-21 Opcode
20 Set Condition
19-16 Rn
15-12 Rd
11-0 Operand2
*/
union DATA_PROCESSING
{
	uint32_t op;
	struct {
		uint32_t cond : 4;
		uint32_t __mask0 : 2;
		uint32_t _I : 1;
		uint32_t opcode : 4;
		uint32_t _S : 1;
		uint32_t _Rn : 4;
		uint32_t _Rd : 4;
		uint32_t operand2 : 12;
	};
};

static_assert(sizeof(DATA_PROCESSING) == 4, "sizeof(DATA_PROCESSING) == 4");

union MULT1
{
	uint32_t op;
	struct {
		uint32_t cond : 4;
		uint32_t __mask0 : 6;
		uint32_t _A : 1;
		uint32_t _S : 1;

		uint32_t _Rd : 4;
		uint32_t _Rn : 4;
		uint32_t _Rs : 4;

		uint32_t __mask1 : 4;

		uint32_t _Rm : 4;
	};
};

static_assert(sizeof(MULT1) == 4, "sizeof(MULTI1) == 4");

/**
	USEFUL FUNCTIONS FOR PROCESSING INSTRUCTION
*/

/**
	This function uses 4 bit condition value for checking either this condition true or false.

	@param 4 bit condition value
	@return If the conditions are met then it returns true, otherwise it returns false.
*/
unsigned int decode_cond(uint32_t cond) {
	switch (cond) {
	case 0:
		printf("Suffix: EQ\n");
		return cpu.flags.Z;
		break;
	case 1:
		printf("Suffix: NE\n");
		return !cpu.flags.Z;
		break;
	case 2:
		printf("Suffix: CS\n");
		return cpu.flags.C;
		break;
	case 3:
		printf("Suffix: CC\n");
		return !cpu.flags.C;
		break;
	case 4:
		printf("Suffix: MI\n");
		return cpu.flags.N;
		break;
	case 5:
		printf("Suffix: PL\n");
		return !cpu.flags.N;
		break;
	case 6:
		printf("Suffix: VS\n");
		return cpu.flags.V;
		break;
	case 7:
		printf("Suffix: VC\n");
		return !cpu.flags.V;
		break;
	case 8:
		printf("Suffix: HI\n");
		return (cpu.flags.C && !cpu.flags.Z);
		break;
	case 9:
		printf("Suffix: LS\n");
		return (!cpu.flags.C && cpu.flags.Z);
		break;
	case 10:
		printf("Suffix: GE\n");
		if (cpu.flags.N == cpu.flags.V) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 11:
		printf("Suffix: LT\n");
		if (cpu.flags.N != cpu.flags.V) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 12:
		printf("Suffix: GT\n");
		if (cpu.flags.Z == 0 && (cpu.flags.N == cpu.flags.V)) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 13:
		printf("Suffix: LE\n");
		if (cpu.flags.Z == 1 && (cpu.flags.N != cpu.flags.V)) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 14:
		printf("Suffix: AL\n");
		return 1;
		break;
	case 15:
		printf("Suffix: This field is reserved!\n");
		break;
	};
}

void branch_exchange(const uint32_t instruction) {
	BRANCH_EXCHANGE inst;
	inst.op = instruction;
	/**
	This instruction performs a branch by copying the contents of a general register, Rn,
	into the program counter, PC(R15). The branch causes a pipeline flush and refill from the
	address specified by Rn. This instruction also permits the instruction set to be
	exchanged. When the instruction is executed, the value of Rn[0] determines whether
	the instruction stream will be decoded as ARM or THUMB instructions.
	*/

	cpu.registers.general[R15] = cpu.registers.general[inst._Rn];

	/**
	pipeline flush and refill from the address specified by RN
	*/
	uint32_t least_significant_bit = inst.op & (0x00000001);

	if (least_significant_bit == 1) {
		printf("THUMB INSTRUCTION SET");
	}
	else {
		printf("ARM INSTRUCTION SET");
	}
};

void branch_and_branch_with_link(const uint32_t instruction) {
	BRANCH_AND_BRANCH_WITH_LINK inst;
	inst.op = instruction;
	uint32_t link_bit = (inst.op & 0x01000000);
	if (link_bit == 0) {
		printf("Branch without link");
		cpu.registers.general[R15] += inst.offset;
	}
	else {
		printf("Branch with link");
		cpu.registers.general[R14] = cpu.registers.general[R15];
	}
}

/**
	This function uses 32 bit instruction value for deciding to which operations going to be run.
	After that chooise, it runs that operation.

	@param 32 bit instruction value
	@return Nothing
*/
void decode_opcode(const uint32_t instruction) {
	DATA_PROCESSING inst;
	inst.op = instruction;
	uint32_t operand1 = inst._Rn;
	uint32_t operand2 = immediate(inst._I);
	uint32_t result;
	if (decode_cond(inst.cond)) {
		switch (inst.opcode) {
		case 0:
			printf("Operation: AND\n");
			result = arm_and(operand1, operand2);
			cpu.registers.general[inst._Rn] = result;
			if (inst._S) {
				if (!result) {
					cpu.flags.Z = 1;
				}
				cpu.flags.N = result >> 31;
			}
			break;
		case 1:
			printf("Operation: EOR\n");
			break;
		case 2:
			printf("Operation: SUB\n");
			break;
		case 3:
			printf("Operation: RSB\n");
			break;
		case 4:
			printf("Operation: ADD\n");
			break;
		case 5:
			printf("Operation: ADC\n");
			break;
		case 6:
			printf("Operation: SBC\n");
			break;
		case 7:
			printf("Operation: RSC\n");
			break;
		case 8:
			printf("Operation: TST\n");
			break;
		case 9:
			printf("Operation: TEQ\n");
			break;
		case 10:
			printf("Operation: CMP\n");
			break;
		case 11:
			printf("Operation: CMN\n");
			break;
		case 12:
			printf("Operation: ORR\n");
			break;
		case 13:
			printf("Operation: MOV\n");
			break;
		case 14:
			printf("Operation: BIC\n");
			break;
		case 15:
			printf("Operation: MVN\n");
			break;
		};
	}
}

uint32_t logical_shift_left(uint32_t number, unsigned int shift_count,  uint32_t set_flags) {
	unsigned int tmp_carry;
	uint32_t result;
	if (shift_count < 32) {
		tmp_carry = ((number >> (31 - shift_count + 1)) & 0x00000001);
		result = (number << shift_count);
	}
	else if (shift_count == 32) {
		tmp_carry = (number & 0x00000001);
		result = 0;
	}
	else {
		tmp_carry = 0;
		result = 0;
	}
	if (set_flags)
		cpu.flags.C = tmp_carry;
	return result;
};

uint32_t logical_shift_right(uint32_t number, unsigned int shift_count,  uint32_t set_flags) {
	unsigned int tmp_carry;
	uint32_t result;

	if (shift_count < 32) {
		tmp_carry = ((number >> (shift_count - 1)) & 0x00000001);
		result = (number >> shift_count);
	}
	else if (shift_count == 32) {
		tmp_carry = number >> 31;
		result = 0;
	}
	else {
		tmp_carry = 0;
		result = 0;
	}

	if (set_flags)
		cpu.flags.C = tmp_carry;

	return result;
};

int32_t arithmetic_shift_right(int32_t number, unsigned int shift_count, uint32_t set_flags) {
	unsigned int tmp_carry;
	uint32_t result;

	if (0 < shift_count && shift_count < 32) {
		tmp_carry = ((number >> (shift_count - 1)) & 0x00000001);
		result = (number >> shift_count);
	}
	else if ((shift_count >= 32) | (shift_count == 0)) {
		tmp_carry = (number >> 31) & 0x00000001;
		result = (number >> 31);
	}
	
	if (set_flags)
		cpu.flags.C = tmp_carry;

	return result;
};

uint32_t rotate_right(uint32_t number, unsigned int rotate_counter, uint32_t set_flags) {
	unsigned int tmp_carry;
	uint32_t result;

	if (rotate_counter == 32) {
		tmp_carry = number >> 31;
		result = number;
	}
	else {
		if (rotate_counter >= 1)
		{
			rotate_counter = rotate_counter % 32;
			uint32_t y = (number >> rotate_counter) & ~(-1 << (32 - rotate_counter));
			uint32_t z = number << (32 - rotate_counter);
			result = y | z;
			tmp_carry = ((number >> (rotate_counter - 1)) & 0x00000001);
		}
		else
		{
			//EXTENDED ROTATE
			result = rotate_right_extended(number, set_flags);
		}
	}

	if (set_flags)
		cpu.flags.C = tmp_carry;

	return result;
}

uint32_t rotate_right_extended(uint32_t number, uint32_t set_flags) {
	unsigned int tmp_carry;
	uint32_t result;

	tmp_carry = (number & 0x00000001);
	result = number >> 1;
	if (tmp_carry) {
		result = result | 0x80000000;
	}
	else {
		result = result & 0x8FFFFFFF;
	}

	if (set_flags)
		cpu.flags.C = tmp_carry;

	return result;
}

/**
	This function uses 32 bit instruction value to perform some shifting operations according to operand 2 type.

	@param 32 bit instruction value
	@return result of operations
*/
uint32_t immediate(const uint32_t instruction) {
	DATA_PROCESSING inst;
	inst.op = instruction;
	uint32_t result = 0;
	/**
	if Immediate bit 1
	opr2 is immidiate
	else
	opr2 is register
	*/
	if (inst._I == 1) {
		int rotate = (inst.operand2 >> 8) * 2;
		int immediate_value = (inst.operand2 & 0x000000FF);
		uint32_t y = (immediate_value >> (rotate)) & ~(-1 << (32 - (rotate)));
		uint32_t z = immediate_value << (32 - (rotate));
		result = y | z;
	}
	else {
		int isRegister = inst.operand2 >> 4;
		int shift_type = -1;
		uint32_t shift_count = -1;
		uint32_t number = 0;
		shift_type = (inst.operand2 >> 5) & 0x00000003;

		if (isRegister) {
			int Rs = inst.operand2 >> 8;
			number = cpu.registers.general[Rs] & 0x000000FF;
		}
		else {
			number = inst.operand2 & 0x0000000F;
			shift_count = (inst.operand2 >> 7);
		}

		switch (shift_type)
		{
		case 0:
			result = logical_shift_left(number, shift_count, inst._S);
			break;
		case 1:
			result = logical_shift_right(number, shift_count, inst._S);
			break;
		case 2:
			result = arithmetic_shift_right(number, shift_count, inst._S);
			break;
		case 3:
			result = rotate_right(number, shift_count, inst._S);
			break;
		default:
			printf("Unexpected result in immediate function.");
			break;
		}
	}
	return result;
};

/**
ARM INSTRUCTION SET
*/

uint32_t arm_and_or_tst(uint32_t operand1, uint32_t operand2) {
	uint32_t result;
	result = (operand1 & operand2);
	return result;
};

uint32_t arm_add(uint32_t operand1, uint32_t operand2) {
	uint32_t result;

	return result;
}

uint32_t arm_eor(uint32_t operand1, uint32_t operand2) {
	uint32_t result;
	result = (operand1 & (~operand2)) | ((~operand1) & operand2);
	return result;
}

uint32_t arm_bic_clear(uint32_t operand1, uint32_t operand2) {
	uint32_t result;
	result = (operand1 & (~operand2));
	return result;
}

void arm_mov(uint32_t register_n, uint32_t operand2) {
	cpu.registers.general[register_n] = operand2;
}

uint32_t arm_mvn(uint32_t operand2) {
	uint32_t result;
	result = (0xFFFFFFFF & (~operand2)) | ((0x00000000) & operand2);
	return result;
}

uint32_t arm_orr(uint32_t operand1, uint32_t operand2) {
	uint32_t result;
	result = operand1 | operand2;
	return result;
}

uint32_t arm_teq(uint32_t operand1, uint32_t operand2, uint32_t instruction) {
	uint32_t result;
	return result;
}

void dump_flags() {
	printf("FLAGS\n");
	printf("*******************\n");
	printf("C N V Z\n");
	printf("%d %d %d %d\n\n", cpu.flags.C, cpu.flags.N, cpu.flags.V, cpu.flags.Z);
};

void dump_registers() {
	
	printf("REGISTERS\n");
	printf("*******************\n");
	for (int i = 0; i < 16; i++) {
		printf("Register %d => %x\n", i, cpu.registers.general[i]);
	}

	printf("\nPSR PRIMASK FAULTMASK CONTROL BASEPRI\n");
	printf("%3x %7x %9x %7x %7x\n", cpu.registers.PSR, cpu.registers.PRIMASK, cpu.registers.FAULTMASK, cpu.registers.CONTROL, cpu.registers.BASEPRI);
}

/**
	This function writes all flags and registers value to console screen.
	dump_flags function used for printing flags' values
	dump_registers function used for printing registers' values
*/
void dump_cpu() {
	printf("CPU STATE\n\n");
	dump_flags();
	dump_registers();
};

int main()
{
	//dump_cpu();
	uint32_t add;
	add = BITBAND_SRAM(0x200FFFFF, 7);
	
	while (1) {};
	
	return 0;
}

