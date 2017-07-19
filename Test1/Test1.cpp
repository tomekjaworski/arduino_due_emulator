// ConsoleApplication1.cpp : Konsol uygulamasý için giriþ noktasýný tanýmlar.
//

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <cstdint>

typedef struct {
	unsigned int C = 0;
	unsigned int N = 0;
	unsigned int Z = 0;
	unsigned int V = 0;
}CPSR;

typedef struct {
	//GENERAL PURPOSES REGISTERS
	uint32_t R0 = 0, R1 = 0, R2 = 0, R3 = 0, R4 = 0, R5 = 0, R6 = 0, R7 = 0;  //low registers
	uint32_t R8 = 0, R9 = 0, R10 = 0, R11 = 0, R12 = 0; // high registers

	//SPECIAL PURPOSE REGISTER
	//R13 SP Stack pointer
	//R14 LR Link Register
	//R15 PC Program Counter
	uint32_t R13 = 0, R14 = 0, R15 = 0;

	//SPECIAL REGISTERS
	uint32_t PSR = 0, PRIMASK = 0, FAULTMASK = 0, BASEPRI = 0, CONTROL = 0;
}REGISTERS;

/**
BRANCH AND EXCHANGE
*/
#define BAE_START 0
#define BAE_STOP 3
/**
BRANCH AND BRANCH WITH LINK
*/
#define OFFSET_START 0
#define OFFSET_STOP 23
/**
DATA PROCESSING INSTRUCTION
ARM Instruction Set
4.5 Data Processing

26 and 27 are 0 all the time.
*/
#define OPR2_START 0
#define OPR2_STOP 11
#define	RD_START 12
#define RD_STOP 15
#define RN_START 16
#define RN_STOP 19
#define S 20
#define OPCODE_START 21
#define OPCODE_STOP 24
#define IMM 25
#define	COND_START 28
#define	COND_STOP 31

CPSR flags;
REGISTERS registers;
uint32_t binary_data;

unsigned int *get_bits(uint32_t n, unsigned int bitswanted) {
	unsigned int *bits = (unsigned int*)malloc(bitswanted * sizeof(unsigned int));

	unsigned int k;
	for (k = 0; k<bitswanted; k++) {
		unsigned int mask = 1 << k;
		unsigned int masked_n = n & mask;
		unsigned int thebit = masked_n >> k;
		bits[k] = thebit;
	}

	return bits;
}

void print_bits(unsigned int *bits, unsigned int bitswanted) {
	int i;
	for (i = bitswanted - 1; i >= 0;i--) {
		printf("%u ", bits[i]);
	}
	printf("\n");
}

int bit_to_value(unsigned int *bits, int start, int stop) {
	int value = 0;

	int i; int digit = 1;
	for (i = start; i <= stop; i++) {
		value = value + bits[i] * digit;
		digit = digit * 2;
	}
	return value;
}

int bit_to_value_branch(unsigned int *bits, int start, int stop) {
	int value = 0;

	int i; int digit = 2;
	for (i = start; i <= stop; i++) {
		value = value + bits[i] * digit;
		digit = digit * 2;
	}
	return value;
}

void decode_opcode(unsigned int *bits) {
	unsigned int opcode = bit_to_value(bits, OPCODE_START, OPCODE_STOP);
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

unsigned int decode_cond(unsigned int *bits) {
	unsigned int cond = bit_to_value(bits, COND_START, COND_STOP);
	switch (cond) {
	case 0:
		printf("Suffix: EQ\n");
		if (flags.Z == 1) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 1:
		printf("Suffix: NE\n");
		if (flags.Z == 0) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 2:
		printf("Suffix: CS\n");
		if (flags.C == 1) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 3:
		printf("Suffix: CC\n");
		if (flags.C == 0) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 4:
		printf("Suffix: MI\n");
		if (flags.N == 1) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 5:
		printf("Suffix: PL\n");
		if (flags.N == 0) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 6:
		printf("Suffix: VS\n");
		if (flags.V == 1) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 7:
		printf("Suffix: VC\n");
		if (flags.V == 0) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 8:
		printf("Suffix: HI\n");
		if (flags.C == 1 && flags.Z == 0) {
			return 1;
		}
		else {
			return 0;
		}
		break;
	case 9:
		printf("Suffix: LS\n");
		if (flags.C == 0 && flags.Z == 1) {
			return 1;
		}
		else {
			return 0;
		}
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

void branch_exchange(unsigned int *bits) {
	/**
	This instruction performs a branch by copying the contents of a general register, Rn,
	into the program counter, PC(R15). The branch causes a pipeline flush and refill from the
	address specified by Rn. This instruction also permits the instruction set to be
	exchanged. When the instruction is executed, the value of Rn[0] determines whether
	the instruction stream will be decoded as ARM or THUMB instructions.
	*/

	unsigned int register_number = bit_to_value(bits, BAE_START, BAE_STOP);

	switch (register_number) {
	case 0:
		registers.R15 = registers.R0;
		break;
	case 1:
		registers.R15 = registers.R1;
		break;
	case 2:
		registers.R15 = registers.R2;
		break;
	case 3:
		registers.R15 = registers.R3;
		break;
	case 4:
		registers.R15 = registers.R4;
		break;
	case 5:
		registers.R15 = registers.R5;
		break;
	case 6:
		registers.R15 = registers.R6;
		break;
	case 7:
		registers.R15 = registers.R7;
		break;
	case 8:
		registers.R15 = registers.R8;
		break;
	case 9:
		registers.R15 = registers.R9;
		break;
	case 10:
		registers.R15 = registers.R10;
		break;
	case 11:
		registers.R15 = registers.R11;
		break;
	case 12:
		registers.R15 = registers.R12;
		break;
	}
	/**
	pipeline flush and refill from the address specified by RN
	*/

	if (bits[0] == 1) {
		printf("THUMB INSTRUCTION SET");
	}
	else if (bits[0] == 0) {
		printf("ARM INSTRUCTION SET");
	}
	else {
		printf("There is a problem identifying instruction set");
	}
};

void branch_and_branch_with_link(unsigned int *bits) {
	//Link bit is 24
	//Offset is 0-23
	int offset = 0;
	if (bits[24] == 0) {
		printf("Branch without link");
		offset = bit_to_value_branch(bits, OFFSET_START, OFFSET_STOP);
		registers.R15 += offset;
	}
	else {
		printf("Branch with link");
		registers.R14 = registers.R15;
	}
}

void immediate(unsigned int *bits) {
	/**
	if Immediate bit 1
	opr2 is immidiate
	else
	opr2 is register
	*/
	int operand2;
	if (bits[IMM] == 1) {
		int shift = bit_to_value(bits, 8, 11);
		int immediate_value = bit_to_value(bits, 8, 11);
		operand2 = immediate_value
	}
	else if (bits[IMM] == 0) {

	}
	else {
		printf("Unexpected case in immidiate function\n");
	}
};

/**
ARM INSTRUCTION SET
*/
void arm_and(unsigned int *bits) {
	int operand1 = bit_to_value(bits, RN_START, RN_STOP);
	int operand2 = immediate(unsigned int *bits);
};

int main()
{
	binary_data = uint32_t (0x00E00000);

	unsigned int  bitswanted = 32;

	unsigned int *bits = get_bits(binary_data, bitswanted);

	printf("%u =", binary_data);
	print_bits(bits, bitswanted);

	decode_cond(bits);
	decode_opcode(bits);

	while (1) {};
	return 0;
}

