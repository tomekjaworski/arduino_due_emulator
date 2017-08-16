// Test1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include <map>
#include "cpu.h"
#include "instructionMask.h"
#include "thumbs.h"

CPSR flags;
REGISTERS registers;
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

unsigned int decode_cond(uint32_t cond) {
	switch (cond) {
	case 0:
		printf("Suffix: EQ\n");
		return flags.Z;
		break;
	case 1:
		printf("Suffix: NE\n");
		return !flags.Z;
		break;
	case 2:
		printf("Suffix: CS\n");
		return flags.C;
		break;
	case 3:
		printf("Suffix: CC\n");
		return !flags.C;
		break;
	case 4:
		printf("Suffix: MI\n");
		return flags.N;
		break;
	case 5:
		printf("Suffix: PL\n");
		return !flags.N;
		break;
	case 6:
		printf("Suffix: VS\n");
		return flags.V;
		break;
	case 7:
		printf("Suffix: VC\n");
		return !flags.V;
		break;
	case 8:
		printf("Suffix: HI\n");
		return (flags.C && !flags.Z);
		break;
	case 9:
		printf("Suffix: LS\n");
		return (!flags.C && flags.Z);
		break;
	case 10:
		printf("Suffix: GE\n");
		if (flags.N == flags.V) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 11:
		printf("Suffix: LT\n");
		if (flags.N != flags.V) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 12:
		printf("Suffix: GT\n");
		if (flags.Z == 0 && (flags.N == flags.V)) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 13:
		printf("Suffix: LE\n");
		if (flags.Z == 1 && (flags.N != flags.V)) {
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

	registers.registers[R15] = registers.registers[inst._Rn];

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
		registers.registers[R15] += inst.offset;
	}
	else {
		printf("Branch with link");
		registers.registers[R14] = registers.registers[R15];
	}
}

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
			registers.registers[inst._Rn] = result;
			if (inst._S) {
				if (!result) {
					flags.Z = 1;
				}
				flags.N = result >> 31;
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
		flags.C = tmp_carry;
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
		flags.C = tmp_carry;

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
		flags.C = tmp_carry;

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
		flags.C = tmp_carry;

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
		flags.C = tmp_carry;

	return result;
}

uint32_t immediate(const uint32_t instruction) {
	DATA_PROCESSING inst;
	inst.op = instruction;
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
		uint32_t  result = y | z;
		return result;
	}
	else {
		int isRegister = inst.operand2 >> 4;
		int shift_type = -1;
		uint32_t shift_count = -1;
		uint32_t number = 0;
		shift_type = (inst.operand2 >> 5) & 0x00000003;

		if (isRegister) {
			int Rs = inst.operand2 >> 8;
			number = registers.registers[Rs] & 0x000000FF;
		}
		else {
			number = inst.operand2 & 0x0000000F;
			shift_count = (inst.operand2 >> 7);
		}

		switch (shift_type)
		{
		case 0:
			logical_shift_left(number, shift_count, inst._S);
			break;
		case 1:
			logical_shift_right(number, shift_count, inst._S);
			break;
		case 2:
			arithmetic_shift_right(number, shift_count, inst._S);
			break;
		case 3:
			rotate_right(number, shift_count, inst._S);
			break;
		default:
			printf("Unexpected result in immediate function.");
			break;
		}
	}
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
	registers.registers[register_n] = operand2;
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

int main()
{

	
	for (int i = 0; i < 15; i++) {
		printf("%d, %x, %x\n", masks[i].type, masks[i].mask, masks[i].value);
	}
	
	while (1) {};

	flags.C = 5;
	int32_t a = -0x00000001;
	int32_t result1 = rotate_right(a, 1, 1);
	printf("%x\n", result1);
	printf("%u", flags.C);
	printf("\n------\n");

	/*
	int32_t a = 1;
	int32_t result = rotate_right(a, 1, 0);
	printf("%x\n", result);
	printf("%u", flags.C);
	*/

	flags.C = 5;
	int32_t b = -0x10000000;
	int32_t result2 = rotate_right(b, 1, 1);
	printf("%x\n", result2);
	printf("%u", flags.C);
	printf("\n------\n");

	flags.C = 5;
	int32_t c = -0x10000000;
	int32_t result3 = rotate_right(c, 33, 1);
	printf("%x\n", result3);
	printf("%u", flags.C);
	printf("\n------\n");

	flags.C = 5;
	int32_t d = -0x00000001;
	int32_t result4 = rotate_right(d, 33, 1);
	printf("%x\n", result4);
	printf("%u", flags.C);
	printf("\n------\n");

	flags.C = 5;
	int32_t e = -0x10000000;
	int32_t result5 = rotate_right(e, 32, 1);
	printf("%x\n", result5);
	printf("%u", flags.C);
	printf("\n------\n");

	flags.C = 5;
	int32_t f = -0x00000001;
	int32_t result6 = rotate_right(f, 32, 1);
	printf("%x\n", result6);
	printf("%u", flags.C);
	printf("\n------\n");

	//This is the start point for test unit of thumb instruction
	/*registers initialization*/
	registers.registers[0] = 0x0;
	registers.registers[1] = 0x1;
	registers.registers[2] = 0x2;
	registers.registers[3] = 0x4;
	registers.registers[4] = 0x8;
	registers.registers[5] = 0x10;
	registers.registers[6] = 0x20;
	registers.registers[7] = 0x1200;
	registers.registers[8] = 0x80;
	registers.registers[9] = 0x100;
	registers.registers[10] = 0x200;
	registers.registers[11] = 0x400;
	registers.registers[12] = 0x800;
	registers.registers[13] = 0x1000;
	registers.registers[14] = 0x2000;
	registers.registers[15] = 0x4000;

	/*test for thumb instructions*/
	uint16_t pg_code = 0b0001110010111010;
	uint8_t flash[512 * 1024] = {};
	set_thumb_instruction(pg_code, registers.registers, flash);
	//This is the end of test unit of thumb instruction

	
	/*TODO: write here the codes to load memory map*/

	/*setting initial values for SystemTimer & PC */
	int SystemTimer = 0x00FFFFFF; //TODO: we have to check the interruption period
	registers.registers[15] = flash[0x00000004]; //PC

	/*set initial values for other registers*/
	registers.registers[13] = flash[0x00000000]; //SP
	registers.registers[14] = 0xFFFFFFFF; //LR
	registers.PSR = 0x01000000;
	registers.PRIMASK = 0x0;
	registers.FAULTMASK = 0x0;
	registers.BASEPRI = 0x0;
	registers.CONTROL = 0x0;

	for (;;) {//main loop for one instruction cycle
		//TODO: Detect in whether arm or thumb instruction mode at this point
		//TODO: Check binary codes for which instructions should be executed
		//TODO: Execute the instruction
		//TODO: Change the value of cycle counter
		//TODO: Check the interruption if the counter value is less than 0
	}
	return 0;
}

