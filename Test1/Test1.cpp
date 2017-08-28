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
//#include "thumbs.h"

#include <stdexcept>
#include <iostream>
#include <string>

#include "..\MemoryMap\DummyMemoryMap.hpp"
#include "..\TextLoader\TextLoader.hpp"
#include "..\MemoryMap\VirtualMemoryMap.h"

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

__forceinline void read_16thumb_instruction(uint16_t);


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
WARNING WARNING WARNING
-----------------------
THESE OPERATIONS HAS TO BE CHECKED!!!

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
//END OF WARNING



/*
This function is for reading 16bit thumb instructions from binary codes.
1. read first 5 bit
2. decide which function is to be executed
*/
void read_16thumb_instruction(uint16_t inst_code) {

	switch ((inst_code & 0xF800) >> 11) { //to decide which opcode this function uses first 5 bits

	/*Format1: Move shifted register instructions family*/
	case 0b00000: //LSL Rd, Rs, #Offset5
	{
		uint16_t Rd = inst_code & 0x0007;
		uint16_t Rs = (inst_code & 0x0038) >> 3;
		uint16_t Offset5 = (inst_code & 0x07C0) >> 6;

		cpu.registers.general[Rd] = cpu.registers.general[Rs] << Offset5;
		cpu.flags.C = ((cpu.registers.general[Rd] >> (32 - Offset5)) & 0x00000001);

		#ifdef _DEBUG //comment for debug mode
		printf("logical left bit shift(LSL Rd, Rs, #Offset5):\n");
		printf("Rd:reg[%d] = 0x%08x, Rs:cpu.registers.general[%d] = 0x%08x, Offset5 = 0x%04x\n", Rd, cpu.registers.general[Rd], Rs, cpu.registers.general[Rs], Offset5);
		printf("flag C:%1u\n", cpu.flags.C);
		#endif //_DEBUG
		break;
	}
	case 0b00001: //LSR Rd, Rs, #Offset5
	{
		uint16_t Rd = inst_code & 0x0007;
		uint16_t Rs = (inst_code & 0x0038) >> 3;
		uint16_t Offset5 = (inst_code & 0x07C0) >> 6;

		cpu.registers.general[Rd] = cpu.registers.general[Rs] >> Offset5;
		cpu.flags.C = ((cpu.registers.general[Rd] >> (Offset5 - 1)) & 0x00000001);

		#ifdef _DEBUG //comment for debug mode
		printf("logical right bit shift(LSR Rd, Rs, #Offset5):\n");
		printf("Rd:reg[%d] = 0x%08x, Rs:cpu.registers.general[%d] = 0x%08x, Offset5 = %u\n", Rd, cpu.registers.general[Rd], Rs, cpu.registers.general[Rs], Offset5);
		printf("flag C:%1u\n", cpu.flags.C);
		#endif //_DEBUG
		break;
	}
	case 0b00010: //ASR Rd, Rs, #Offset5
	{
		uint16_t Rd = (inst_code & 0x0007);
		uint16_t Rs = (inst_code & 0x0038) >> 3;
		uint32_t Offset5 = (inst_code & 0x07C0) >> 6;
		if ((cpu.registers.general[Rd] & 0x80000000) == 0x80000000) { //if the top bit is 1
			cpu.registers.general[Rd] = ((cpu.registers.general[Rs] >> Offset5) | (~0 << (32 - Offset5)));
		}
		else { // if the top bit is 0
			cpu.registers.general[Rd] = (cpu.registers.general[Rs] >> Offset5); //this is the same as logical right shift bit
		}
#ifdef _DEBUG //comment for debug mode
		printf("arithmetic right bit shift(ASR Rd, Rs, #Offset5):\n");
		printf("Rd:regs[%d] = 0x%08x, Rs:regs[%d] = 0x%08x, Offset5 = %u\n", Rd, cpu.registers.general[Rd], Rs, cpu.registers.general[Rs], Offset5);
		printf("flag C:%1u\n", cpu.flags.C);
#endif //_DEBUG
		break;
	}
	/*Format2: Add, Substract instructions family*/
	case 0b00011:
	{
		int32_t Rd = (inst_code & 0x0007);
		uint16_t Rs = (inst_code & 0x0038) >> 3;
		uint16_t Offset3 = (inst_code & 0x01C0) >> 6; // Offset3/Rn
#ifdef _DEBUG //comment for debug mode
		uint8_t op_flag = (inst_code & 0x0200) == 0x0200;
		uint8_t imm_flag = (inst_code & 0x0400) == 0x0400;
		char* im_offset = imm_flag ? "#Offset3" : "Rn";
		uint8_t immv = imm_flag ? Offset3 : cpu.registers.general[Offset3];
		if (op_flag) {
			printf("ADDS: Rd, Rs, %s\n", im_offset);
		}
		else {
			printf("SUBS: Rd, Rs, %s\n", im_offset);
		}
#endif //_DEBUG
		switch ((inst_code & 0x0600) >> 9) {
		case 0b00: //ADDS Rd, Rs, Rn
			cpu.registers.general[Rd] = cpu.registers.general[Rs] + cpu.registers.general[Offset3];
#ifdef _DEBUG
			//TODO: implement carry flag
#endif //_DEBUG
			break;
		case 0b01: //ADDS Rd, Rs, #Offset3
			cpu.registers.general[Rd] = cpu.registers.general[Rs] + Offset3;
#ifdef _DEBUG
			//TODO: implement carry flag
#endif //_DEBUG
			break;
		case 0b10: //SUBS Rd, Rs, Rn
			cpu.registers.general[Rd] = cpu.registers.general[Rs] - cpu.registers.general[Offset3];
#ifdef _DEBUG
			//TODO: implement carry flag
#endif //_DEBUG
			break;
		case 0b11: //ADDS Rd, Rs, #Offset3
			cpu.registers.general[Rd] = cpu.registers.general[Rs] - Offset3;
#ifdef _DEBUG
			//TODO: implement carry flag
#endif //_DEBUG
			break;
		}
#ifdef _DEBUG //comment for debug mode
		printf("Rd:regs[%d] = 0x%08x, Rs:regs[%d] = 0x%08x, %s = 0x%04x\n", Rd, cpu.registers.general[Rd], Rs, cpu.registers.general[Rs], im_offset, immv);
		//TODO: output is needed which number of register assigned as Rn
#endif //_DEBUG
		break;
	}

	/*Format3: move, compare, add, substract immediate family*/
	case 0b00100: //MOVS Rd, #Offset8
		cpu.registers.general[(inst_code & 0b0000011100000000) >> 8] = (inst_code & 0x00FF);
		break;
	case 0b00101: //CMP Rd, #Offset8
	{
		int res = (int)cpu.registers.general[(inst_code & 0b0000011100000000) >> 8] - (inst_code & 0x00FF);
		if (res == 0) {
			/*TODO: implement setting condition flags*/
			cpu.flags.Z = 1;
			cpu.flags.N = 0;
		}
		else if (res < 0) {
			cpu.flags.N = 1;
		}
		else {
			cpu.flags.Z = 0;
			cpu.flags.N = 0;
		}
#ifdef _DEBUG //comment for debug mode
		printf("flags Z:%1u, N:%1u\n", cpu.flags.Z, cpu.flags.N);
#endif //_DEBUG
		break;
	}
	case 0b00110: //ADD Rd, Rd, #Offset8
		cpu.registers.general[(inst_code & 0b0000011100000000) >> 8] += (inst_code & 0x00FF);
		break;
	case 0b00111: //SUBS Rd, Rd, #Offset8
		cpu.registers.general[(inst_code & 0b0000011100000000) >> 8] -= (inst_code & 0x00FF);
		break;
		/*ALU operations & branch*/
	case 0b01000:
		if ((inst_code & 0b00000100000000000) != 0b0) {
			/*Format4: ALU operations*/
			{
				uint8_t Rd = (inst_code & 0b111);
				uint8_t Rs = (inst_code & 0b111000) >> 3;
				switch ((inst_code & 0b0000001111000000) >> 6) {
				case 0b0000: //ANDS Rd, Rd, Rs
					cpu.registers.general[Rd] &= cpu.registers.general[Rs];
					break;
				case 0b0001: //EORS Rd, Rd, Rs
					cpu.registers.general[Rd] ^= cpu.registers.general[Rs];
					break;
				case 0b0010: //MOVS Rd, Rd, LSL Rs
					cpu.registers.general[Rd] <<= cpu.registers.general[Rs];
					break;
				case 0b0011: //MOVS Rd, Rd, LSR Rs
					cpu.registers.general[Rd] >>= cpu.registers.general[Rs];
					break;
				case 0b0100: //MOVS Rd, Rd, ASR Rs
				{
					uint16_t Rd = (inst_code & 0x0007);
					uint16_t Rs = (inst_code & 0x0038) >> 3;
					if ((cpu.registers.general[Rd] & 0x80000000) == 0x80000000) { //if the top bit is 1
						cpu.registers.general[Rd] = (cpu.registers.general[Rd] >> cpu.registers.general[Rs]) | (~0 << (32 - cpu.registers.general[Rs]));
					}
					else { // if the top bit is 0
						cpu.registers.general[Rd] = (cpu.registers.general[Rd] >> cpu.registers.general[Rs]); //this is the same as logical right shift bit
					}
#ifdef _DEBUG //comment for debug mode
					printf("arithmetic right bit shift:\n");
					printf("Rd:reg[%d] = 0x%08x, Rs:regs[%d] = 0x%08x\n", Rd, cpu.registers.general[Rd], Rs, cpu.registers.general[Rs]);
#endif //_DEBUG
					break;
				}
					cpu.registers.general[Rd] >>= cpu.registers.general[Rs];
					break;
				case 0b0101: //ADCS Rd, Rd, Rs
					cpu.registers.general[Rd] += cpu.registers.general[Rs];//TODO: implement C bit
					break;
				case 0b0110: //SBCS Rd, Rd, Rs
					cpu.registers.general[Rd] >>= cpu.registers.general[Rs];//TODO: implement C bit
					break;
				case 0b0111: //MOVS Rd, Rd, ROR Rs
					cpu.registers.general[Rd] += cpu.registers.general[Rs];//TODO:
					break;
				case 0b1000: //TST Rd, Rs
					cpu.registers.general[Rd] -= cpu.registers.general[Rs];//TODO
					break;
				case 0b1001: //RSBS Rd, Rd, #0
					cpu.registers.general[Rd] = -cpu.registers.general[Rs]; //TODO:
					break;
				case 0b1010: //CMP Rd, Rs
					cpu.registers.general[Rd] >>= cpu.registers.general[Rs]; //TODO
					break;
				case 0b1011: //CMN Rd, Rs
					cpu.registers.general[Rd] >>= cpu.registers.general[Rs]; //TODO
					break;
				case 0b1100: //ORRS Rd, Rd, Rs
					cpu.registers.general[Rd] |= cpu.registers.general[Rs];
					break;
				case 0b1101: //MULS Rd, Rs, Rd
					cpu.registers.general[Rd] *= cpu.registers.general[Rs];
					break;
				case 0b1110: //BICS Rd, Rd, Rs
					cpu.registers.general[Rd] &= ~cpu.registers.general[Rs];
					break;
				case 0b1111: //MVNS Rd, Rs
					cpu.registers.general[Rd] = ~cpu.registers.general[Rs];
					break;
				}
			}
		}
		else {
			/*Format5: Hi register operations, branch exchange*/
			{
				uint8_t Rd = (inst_code & 0b111); //Rs or Hs
				uint8_t Rs = (inst_code & 0b111000) >> 3; // Rs or Hs
				switch ((inst_code & 0b0000001111000000) >> 6) {
				case 0b0001: //ADD Rd, Rd, Hs
					cpu.registers.general[Rd] += cpu.registers.general[Rs + 8];
					break;
				case 0b0010: //ADD Hd, Hd, Rs
					cpu.registers.general[Rd + 8] += cpu.registers.general[Rs];
					break;
				case 0b0011: //ADD Hd, Hd, Hs
					cpu.registers.general[Rd + 8] += cpu.registers.general[Rs + 8];
					break;
				case 0b0101: //CMP Rd, Hs
				{
					int res = (int)cpu.registers.general[(inst_code & 0b0000000000111000) >> 3] - cpu.registers.general[(inst_code & 0x0007) + 8];
					if (res == 0) {
						/*TODO: implement setting condition flags*/
						cpu.flags.Z = 1;
						cpu.flags.N = 0;
					}
					else if (res < 0) {
						cpu.flags.N = 1;
					}
					else {
						cpu.flags.Z = 0;
						cpu.flags.N = 0;
					}
				}
				#ifdef _DEBUG //comment for debug mode
					printf("flags Z:%1u, N:%1u\n", cpu.flags.Z, cpu.flags.N);
				#endif //_DEBUG

					break;
				case 0b0110: //CMP Hd, Rs
				{
					int res = (int)cpu.registers.general[((inst_code & 0b0000000000111000) >> 3)] - cpu.registers.general[(inst_code & 0x0007) + 8];
					if (res == 0) {
						/*TODO: implement setting condition flags*/
						cpu.flags.Z = 0;
						cpu.flags.N = 0;
					}
					else if (res < 0) {
						cpu.flags.N = 1;
					}
					else {
						cpu.flags.Z = 0;
						cpu.flags.N = 0;
					}
#ifdef _DEBUG //comment for debug mode
					printf("flags Z:%1u, N:%1u\n", cpu.flags.Z, cpu.flags.N);
#endif //_DEBUG
					break;
				}
				case 0b0111: //CMP Hd, Hs
				{
					int res = (int)cpu.registers.general[((inst_code & 0b0000000000111000) >> 8) + 8] - cpu.registers.general[(inst_code & 0x0007) + 8];
					if (res == 0) {
						/*TODO: implement setting condition flags*/
						cpu.flags.Z = 0;
						cpu.flags.N = 0;
					}
					else if (res < 0) {
						cpu.flags.N = 1;
					}
					else {
						cpu.flags.Z = 0;
						cpu.flags.N = 0;
					}
#ifdef _DEBUG //comment for debug mode
					printf("flags Z:%1u, N:%1u\n", cpu.flags.Z, cpu.flags.N);
#endif //_DEBUG
					break;
				}
				case 0b1001: //MOV Rd, Hs
					cpu.registers.general[Rd] = cpu.registers.general[Rs + 8];
					break;
				case 0b1010: //MOV Hd, Rs
					cpu.registers.general[Rd + 8] = cpu.registers.general[Rs];
					break;
				case 0b1011: //MOV Hd, Hs
					cpu.registers.general[Rd + 8] = cpu.registers.general[Rs + 8];
					break;
				case 0b1100: //BX Rs
					cpu.registers.general[15] = cpu.registers.general[Rs];
					/*TODO: switch the state of processor*/
					break;
				case 0b1101: //BX Hs
					cpu.registers.general[15] = cpu.registers.general[Rs + 8];
					/*TODO: switch the state of processor*/
					break;
				default:
					break;
				}
			}
		}
		break;
	case 0b01001: //PC-relative load LDR Rd, [PC, #Imm]
	{
		uint32_t Rd = inst_code & 0b0000011100000000 >> 8;
		uint32_t Word8 = inst_code & 0x00FF;
		//TODO: implement reader from address
		break;
	}

	case 0b01010:
	case 0b01011:
	{
		/*Format7: load/store with register offset*/
		uint8_t bw_flag = (inst_code & 0b0000010000000000) >> 10; //the flag for byte/word
		uint16_t Ro = (inst_code & 0b0000000111000000) >> 6; //offset register
		uint16_t Rb = (inst_code & 0b0000000000111000) >> 3; //base register
		uint16_t Rd = (inst_code & 0b0000000000000111); //destination register

		if (bw_flag) { //byte length (STRB)
			//TODO: implement reader from address
		}
		else { //word length (STR)
			//TODO: implement reader from address
		}
		/*TODO: change the length between byte/word*/
		break;
	}
	/*Format9: load store with the immediate offset*/
	case 0b01100:
	case 0b01101:
	case 0b01110:
	case 0b01111:
	{
		uint8_t bw_flag = (inst_code & 0b0001000000000000) >> 12;
		uint8_t ls_flag = (inst_code & 0b0000100000000000) >> 11;
		uint16_t Offset5 = (inst_code & 0b0000011111000000) >> 6;
		uint16_t Rb = (inst_code & 0b0000000000111000) >> 3;
		uint16_t Rd = (inst_code & 0b0000000000000111);
		/*TODO: implement the execution of load/store*/
		break;
	}
	/*Format10: load store halfword*/
	case 0b10000: //store
	{
		uint16_t Offset5 = (inst_code & 0b0000011111000000) >> 6;
		uint16_t Rb = (inst_code & 0b0000000000111000) >> 3;
		uint16_t Rd = (inst_code & 0b0000000000000111);

		/*TODO: implement halfword load*/
		break;
	}
	case 0b10001: //load
	{
		uint16_t Offset5 = (inst_code & 0b0000011111000000) >> 6;
		uint16_t Rb = (inst_code & 0b0000000000111000) >> 3;
		uint16_t Rd = (inst_code & 0b0000000000000111);

		/*TODO: implement halfword load*/
		break;
	}

	/*Format11: SP-relative load/store*/
	case 0b10010: //store
	{
		uint16_t Rd = (inst_code & 0b0000011100000000) >> 8;
		uint16_t Word8 = (inst_code & 0b0000000011111111);

		//TODO: implement reader/writer from address
		break;
	}
	case 0b10011: //load
	{
		uint16_t Rd = (inst_code & 0b0000011100000000) >> 8;
		uint16_t Word8 = (inst_code & 0b0000000011111111);

		//TODO: implement reader/writer from address
		break;
	}

	/*Format12: load address*/
	case 0b10100:
	case 0b10101:
	{
		uint16_t Rd = (inst_code & 0b0000011100000000) >> 8;
		uint16_t Word8 = inst_code & 0b0000000011111111;
		/*TODO: implement load/store*/
		break;
	}

	case 0b10110:
	case 0b10111:
		if ((inst_code & 0b0000111100000000) == 0b0) {
			/*Format13: add offset to Stack Pointer*/
			cpu.registers.general[13] = (uint16_t)(cpu.registers.general[13] + ((int)((inst_code & 0b0000000010000000) >> 7) - 1) * (inst_code & 0b0000000001111111));
		}
		else if ((inst_code & 0b0000011000000000) == 0b0000010000000000) {
			/*Format14: push/pop registers*/
			/*TODO: implement push/pop*/
		}
		else {
			/*TODO: implement other instructions*/
		}
		break;

		/*Format 15: multiple load/store*/
	case 0b11000:
	case 0b11001:
		break;

		/*Format16: conditional branch*/
		/*Format17: software interrupt*/
	case 0b11010:
	case 0b11011:
		/*TODO: implement conditional branch*/
		/*TODO: implement software interrupt*/
		break;

		/*Format18: unconditional branch*/
	case 0b11100:
	{
		uint16_t Offset11 = (inst_code & 0b0000011111111111);
		/*TODO: implement unconditional branch*/
		break;
	}
	/*Format19: long branch with link*/
	case 0b11110:
	case 0b11111:
	{
		uint8_t lh_flag = (inst_code & 0b0000100000000000); // low/high offset bit
														 /*TODO: long branch with link*/
		break;
	}
	/*TODO: implement other instructions*/
	default: // For error message
		fprintf(stderr, "No instruction detected!\n");
		break;
	}

	return;
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
	//uint32_t add;
	//add = BITBAND_SRAM(0x200FFFFF, 7);
	
	VirtualMemoryMap memory;
	
	/*Reading memory from HEX file*/
	const char* file_name = "test_hex\\GccApplication_add.hex";
	IntelHexParser p(file_name);

	#ifdef _DEBUG
	bool exception = false;
	std::string msg = "";
	int bytes = -1;
	try {
		bytes = p.Load(memory);
	}
	catch (const std::exception& ex)
	{
		exception = true;
		msg = ex.what();
	}
	catch (...)
	{
		exception = true;
		msg = "(...)";
	}

	if (!exception == 1)
		std::cout << " AS EXPECTED";
	else
		std::cout << " ERROR";
	if (exception)
		std::cout << " Exception: " << msg;
	else
		std::cout << "; Byte count=" << bytes;

	std::cout << std::endl;
	memory.DumpMemory();
	#endif //_DEBUG

	/*set initial values for registers*/
	cpu.registers.PSR = 0x01000000;
	cpu.registers.BASEPRI = 0x00000000;
	cpu.registers.CONTROL = 0x00000000;
	cpu.registers.FAULTMASK = 0x00000000;
	cpu.registers.PRIMASK = 0x00000000;
	cpu.registers.general[14] = 0xFFFFFFFF; //LR

	
	memory.Read(0x00080000, cpu.registers.general[13]); //SP
	memory.Read(0x00080004, cpu.registers.general[15]); //PC

	#ifdef _DEBUG
	printf("initial value of SP: 0x%08x\n", cpu.registers.general[13]);	
	printf("initial value of PC: 0x%08x\n", cpu.registers.general[15]);
	#endif // _DEBUG
	
	for (;;) {
		uint32_t thumb_code;
		memory.Read(cpu.registers.general[15], thumb_code);
#ifdef _DEBUG
		printf("thumb_code: %08x\n", thumb_code);
#endif //_DEBUG
		if ((thumb_code & 0xe000) != 0xe000) { //if it is 16 bit instruction
		cpu.registers.general[15] += 0x2;
		read_16thumb_instruction((uint16_t)(thumb_code >> 16));
		#ifdef _DEBUG
		printf("16bit thumb instruction executed. code:%04x\n", (uint16_t)(thumb_code >> 16));
		#endif //_DEBUG
		}
		else if ((thumb_code & 0xf800) == 0xe000) {
			//branch_inst(p_addr, regs, flash);
			printf("branch instruction executed.\n");
		}
		else { //if they are 32 bit operations
			//read_32th_inst((uint32_t)p_addr, regs, flash);
			#ifdef _DEBUG
			printf("32bit thumb instruction executed.\n");
			#endif //_DEBUG
		}
	}
	

	return 0;
}

