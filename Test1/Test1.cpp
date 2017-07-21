// ConsoleApplication1.cpp : Konsol uygulamasý için giriþ noktasýný tanýmlar.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>
#include "cpu.h"

CPSR flags;
REGISTERS registers;
uint32_t binary_data;

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

void decode_opcode(uint32_t opcode) {
	switch (opcode) {
	case 0:
		printf("Operation: AND\n");
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

uint32_t logical_shift_left(uint32_t number, unsigned int shift_count) {
	if (shift_count < 32) {
		flags.C = ((number >> (31 - shift_count + 1)) & 0x00000001);
		return (number << shift_count);
	}
	else if (shift_count == 32) {
		flags.C = (number & 0x00000001);
		return 0;
	}
	else {
		flags.C = 0;
		return 0;
	}
};

uint32_t logical_shift_right(uint32_t number, unsigned int shift_count) {
	
	if (shift_count < 32) {
		flags.C = ((number >> (shift_count - 1)) & 0x00000001);
		return (number >> shift_count);
	}
	else if (shift_count == 32) {
		flags.C = number >> 31;
		return 0;
	}
	else {
		flags.C = 0;
		return 0;
	}
};

int32_t arithmetic_shift_right(int32_t number, unsigned int shift_count) {

	if (0 < shift_count && shift_count < 32) {
		flags.C = ((number >> (shift_count - 1)) & 0x00000001);
		return (number >> shift_count);
	}
	else if ((shift_count >= 32) | (shift_count == 0)) {
		flags.C = (number >> 31) & 0x00000001;
		return (number >> 31);
	}
	
};

uint32_t rotate_right(uint32_t number, unsigned int rotate_counter) {
	rotate_counter = rotate_counter % 32;
	if (rotate_counter >= 1)
	{
		uint32_t y = (number >> rotate_counter) & ~(-1 << (32 - rotate_counter));
		uint32_t z = number << (32 - rotate_counter);
		uint32_t  g = y | z;
		flags.C = ((number >> (rotate_counter - 1)) & 0x00000001);
		return g;
	}
	else
	{
		//EXTENDED ROTATE
		rotate_right_extended(number);
	}
}

uint32_t rotate_right_extended(uint32_t number) {
	flags.C = (number & 0x00000001);
	number >> 1;
	if (flags.C) {
		number = number | 0x80000000;
	}
	else {
		number = number & 0x8FFFFFFF;
	}
	return number;
}

/**
uint32_t immediate(DATA_PROCESSING inst) {
	/**
	if Immediate bit 1
	opr2 is immidiate
	else
	opr2 is register
	
	
	int immediate_bit = (inst.op & 0x02000000);
	if (immediate_bit == 1) {
		int rotate = (inst.operand2 >> 8);
		int immediate_value = (inst.operand2 & 0x0000007F);
		uint32_t y = (immediate_value >> (rotate * 2)) & ~(-1 << (32 - (rotate * 2)));
		uint32_t z = immediate_value << (32 - (rotate * 2));
		uint32_t  g = y | z;
		return g;
	}
	else if (immediate_bit == 0) {
		switch (inst.)
		{
		default:
			break;
		}
	}
	else {
		printf("Unexpected case in immidiate function\n");
	}
};
*/



/**
ARM INSTRUCTION SET

void arm_and(unsigned int *bits) {
	int operand1 = bit_to_value(bits, RN_START, RN_STOP);
	int operand2 = immediate(bits);
};
*/
int main()
{
	/*binary_data = uint32_t (0x00E00000);

	unsigned int  bitswanted = 32;

	unsigned int *bits = get_bits(binary_data, bitswanted);

	printf("%u =", binary_data);
	print_bits(bits, bitswanted);

	decode_cond(bits);
	decode_opcode(bits);*/

	int32_t a = 1;
	int32_t result = rotate_right(a, 3);
	printf("%x\n", result);
	printf("%u", flags.C);


	while (1) {};
	return 0;
}

