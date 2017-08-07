//

#include <stdio.h>
#include <stdlib.h>
#include "stdafx.h"
#include "thumbs.h"

//Unions for instruction families

//union for move shifted register
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 3; //
		uint16_t	OPCODE : 2;
		uint16_t	OFFSET : 5;
		uint16_t	Rs : 3;
		uint16_t	Rd : 3;
	};
}movs;

//union for add/substract
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 5; //
		uint16_t	IMM : 1; // Immidiate flag
		uint16_t	OPCODE : 1;
		uint16_t	OFFSET : 3; // Rn/Offset3
		uint16_t	Rs : 3; // Rs
		uint16_t	Rd : 3; // Rd
	};
} adsub;

//union for move/compare/add/substract immediate
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 3;
		uint16_t	OPCODE : 2;
		uint16_t	Rd : 3;
		uint16_t	OFFSET : 8;
	};
} lo_imms;

//union for move/compare/add/substract immediate
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 6;
		uint16_t	OPCODE : 4;
		uint16_t	Rs : 3;
		uint16_t	Rd : 3;
	};
} alu;

//union for hi register operations/branch exchange
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 6;
		uint16_t	OPCODE : 2;
		uint16_t	H1 : 1; // High flag1
		uint16_t	H2 : 1; // High flag2
		uint16_t	Rs : 3; // Rs/Hs
		uint16_t	Rd : 3; // Rd/Hd
	};
} hireg;

//union for PC-relative load
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 5;
		uint16_t	Rd : 3;
		uint16_t	Word8 : 8;
	};
} pc_load;

//union for load/store with register offset
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 4;
		uint16_t	L : 1;
		uint16_t	B : 1;
		uint16_t	__MASK : 1;
		uint16_t	Ro : 3;
		uint16_t	Rb : 3;
		uint16_t	Rd : 3;
	};
} lost_offset;

//union for load/store sign-extended bute/halfword
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 4;
		uint16_t	H : 1;
		uint16_t	S : 1;
		uint16_t	__MASK : 1;
		uint16_t	Ro : 3;
		uint16_t	Rb : 3;
		uint16_t	Rd : 3;
	};
} lost_signex;

//union for load/store with immediate offset
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 3;
		uint16_t	B : 1;
		uint16_t	L : 1;
		uint16_t	OFFSET : 5;
		uint16_t	Rb : 3;
		uint16_t	Rd : 3;
	};
} lost_imm;

//union for load/store halfword
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 4;
		uint16_t	L : 1;
		uint16_t	OFFSET : 5;
		uint16_t	Rb : 3;
		uint16_t	Rd : 3;
	};
} lost_hw;

//union for sp-relative load/store
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 4;
		uint16_t	L : 1;
		uint16_t	Rd : 3;
		uint16_t	Word8 : 8;
	};
} sp_lost;

//union for load address
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 4;
		uint16_t	SP : 1;
		uint16_t	Rd : 3;
		uint16_t	Word8 : 8;
	};
} load_addr;

//union for add offset to SP
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 8;
		uint16_t	SP : 1;
		uint16_t	SWord7 : 7;
	};
} add_sp;

//union for push/pop registers
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 8;
		uint16_t	SP : 1;
		uint16_t	SWord7 : 7;
	};
} pp_reg;

//union for multiple load/store
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 4;
		uint16_t	L : 1;
		uint16_t	Rb : 3;
		uint16_t	Rlist : 8;
	};
} mul_lost;

//union for conditional branch
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 4;
		uint16_t	Cond : 4;
		uint16_t	SOffset8 : 8;
	};
} cond_br;

//union for software interrupt
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 8;
		uint16_t	Value8 : 8;
	};
} soft_ir;

//union for unconditional branch
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 5;
		uint16_t	Offset11 : 11;
	};
} uncond_branch;

//union for long branch with link
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 4;
		uint16_t	H : 1;
		uint16_t	Offset : 11;
	};
} lbr_wl;

void set_conditon_flags(uint32_t) {
	/*This function is for setting condition flags against the result of thumb functions*/

	return;
}


/*This function is under reconstruction*/
/*This function is for reading 16bit thumb instructions
  from binary codes.
  1. read first 5 bit
  2. decide which function is to be executed*/
inline void read_16th_inst(uint16_t p_addr, uint32_t regs[], uint8_t flash[]) {

	switch ((p_addr & 0xF800) >> 11) { //to decide which opcode this function uses first 5 bits

	/*Format1: Move shifted register instructions family*/
	case 0b00000: //MOVS Rd, Rs, LSL #Offset5
	{
		uint16_t Rd = p_addr & 0x0007;
		uint16_t Rs = (p_addr & 0x0038) >> 3;
		uint16_t Offset5 = (p_addr & 0x07C0) >> 6;

		regs[Rd] = regs[Rs] << Offset5; //writtten with bit shift & mask

		#ifdef _DEBUG //comment for debug mode
		printf("left bit shift:\n");
		printf("Rd:reg[%d] = 0x%08x, Rs:regs[%d] = 0x%08x, Offset5 = 0x%04x\n", Rd, regs[Rd], Rs, regs[Rs], Offset5);
		#endif //_DEBUG
		break;
	}
	case 0b00001: //MOVS Rd, Rs, LSR #Offset5
	{
		uint16_t Rd = p_addr & 0x0007;
		uint16_t Rs = (p_addr & 0x0038) >> 3;
		uint16_t Offset5 = (p_addr & 0x07C0) >> 6;

		regs[Rd] = regs[Rs] >> Offset5; //writtten with bit shift & mask

		#ifdef _DEBUG //comment for debug mode
		printf("right bit shift:\n");
		printf("Rd:reg[%d] = 0x%08x, Rs:regs[%d] = 0x%08x, Offset5 = %u\n", Rd, regs[Rd], Rs, regs[Rs], Offset5);
		#endif //_DEBUG
		break;
	}
	case 0b00010: //MOVS Rd, Rs, ASR #Offset5
	{
		uint16_t Rd = (p_addr & 0x0007);
		uint16_t Rs = (p_addr & 0x0038) >> 3;
		uint32_t Offset5 = (p_addr & 0x07C0) >> 6;
		if ((regs[Rd] & 0x80000000) == 0x80000000) { //if the top bit is 1
			regs[Rd] = ((regs[Rs] >> Offset5) | (~0 << (32-Offset5)));
			
			/*This may be also OK.*/
			//int tmp = (int)regs[Rs];
			//for (int i = 0; i < Offset5; i++) {
			//	tmp = tmp / 2;
			//}
			//regs[Rd] = (int32_t)tmp;
		}
		else { // if the top bit is 0
			regs[Rd] = (regs[Rs] >> Offset5); //this is the same as logical right shift bit
		}
		#ifdef _DEBUG //comment for debug mode
		printf("arithmetic right bit shift:\n");
		printf("Rd:reg[%d] = 0x%08x, Rs:regs[%d] = 0x%08x, Offset5 = %u\n", Rd, regs[Rd], Rs, regs[Rs], Offset5);
		#endif //_DEBUG
		break;
	}
	/*Format2: Add, Substract instructions family*/
	case 0b00011:
	{
		int32_t Rd = (p_addr & 0x0007);
		uint16_t Rs = (p_addr & 0x0038) >> 3;
		uint16_t Offset3 = (p_addr & 0x01C0) >> 6; // Offset3/Rn
		uint8_t op_flag = (p_addr & 0x0200) == 0x0200;
		uint8_t imm_flag = (p_addr & 0x0400) == 0x0400;
		char* im_offset = imm_flag ? "#Offset3" : "Rn";
		uint8_t immv = imm_flag ? Offset3 : regs[Offset3];
		#ifdef _DEBUG //comment for debug mode
		if (op_flag) {
			printf("ADDS: Rd, Rs, %s\n", im_offset);
		}
		else{
			printf("SUBS: Rd, Rs, %s\n", im_offset);
		}
		#endif //_DEBUG
		switch ((p_addr & 0x0600) >> 9) {
		case 0b00: //ADDS Rd, Rs, Rn
			regs[Rd] = regs[Rs] + regs[Offset3];
			break;
		case 0b01: //ADDS Rd, Rs, #Offset3
			regs[Rd] = regs[Rs] + Offset3;
			break;
		case 0b10: //SUBS Rd, Rs, Rn
			regs[Rd] = regs[Rs] - regs[Offset3];
			break;
		case 0b11: //ADDS Rd, Rs, #Offset3
			regs[Rd] = regs[Rs] - Offset3;
			break;
		}
		#ifdef _DEBUG //comment for debug mode
		printf("Rd:reg[%d] = 0x%08x, Rs:regs[%d] = 0x%08x, %s = 0x%04x\n", Rd, regs[Rd], Rs, regs[Rs], im_offset, immv);
		//TODO: output is needed which number of register assigned as Rn
		#endif //_DEBUG
		break;
	}

	/*Format3: move, compare, add, substract immediate family*/
	case 0b00100: //MOVS Rd, #Offset8
		regs[(p_addr & 0b0000011100000000) >> 8] = (p_addr & 0x00FF);
		break;
	case 0b00101: //CMP Rd, #Offset8
		if (regs[(p_addr & 0b0000011100000000) >> 8] - (p_addr & 0x00FF) > 0) {
			/*TODO: implement setting condition flags*/
		}
		break;
	case 0b00110: //ADD Rd, Rd, #Offset8
		regs[(p_addr & 0b0000011100000000) >> 8] += (p_addr & 0x00FF);
		break;
	case 0b00111: //SUBS Rd, Rd, #Offset8
		regs[(p_addr & 0b0000011100000000) >> 8] -= (p_addr & 0x00FF);
		break;
	/*ALU operations & branch*/
	case 0b01000:
		if ((p_addr & 0b00000100000000000) != 0b0) {
			/*Format4: ALU operations*/
			{
				uint8_t Rd = (p_addr & 0b111);
				uint8_t Rs = (p_addr & 0b111000) >> 3;
				switch ((p_addr & 0b0000001111000000) >> 6) {
				case 0b0000: //ANDS Rd, Rd, Rs
					regs[Rd] &= regs[Rs];
					break;
				case 0b0001: //EORS Rd, Rd, Rs
					regs[Rd] ^= regs[Rs];
					break;
				case 0b0010: //MOVS Rd, Rd, LSL Rs
					regs[Rd] <<= regs[Rs];
					break;
				case 0b0011: //MOVS Rd, Rd, LSR Rs
					regs[Rd] >>= regs[Rs];
					break;
				case 0b0100: //MOVS Rd, Rd, ASR 
				/*TODO: implement arithmatical bit shift*/
					regs[Rd] >>= regs[Rs];
					break;
				case 0b0101: //ADCS Rd, Rd, Rs
					regs[Rd] += regs[Rs];//TODO: implement C bit
					break;
				case 0b0110: //SBCS Rd, Rd, Rs
					regs[Rd] >>= regs[Rs];//TODO: implement C bit
					break;
				case 0b0111: //MOVS Rd, Rd, ROR Rs
					regs[Rd] += regs[Rs];//TODO:
					break;
				case 0b1000: //TST Rd, Rs
					regs[Rd] -= regs[Rs];//TODO
					break;
				case 0b1001: //RSBS Rd, Rd, #0
					regs[Rd] = -regs[Rs]; //TODO:
					break;
				case 0b1010: //CMP Rd, Rs
					regs[Rd] >>= regs[Rs]; //TODO
					break;
				case 0b1011: //CMN Rd, Rs
					regs[Rd] >>= regs[Rs]; //TODO
					break;
				case 0b1100: //ORRS Rd, Rd, Rs
					regs[Rd] |= regs[Rs];
					break;
				case 0b1101: //MULS Rd, Rs, Rd
					regs[Rd] *= regs[Rs];
					break;
				case 0b1110: //BICS Rd, Rd, Rs
					regs[Rd] &= ~regs[Rs];
					break;
				case 0b1111: //MVNS Rd, Rs
					regs[Rd] = ~regs[Rs];
					break;
				}
			}
		}
		else {
			/*Hi register operations, branch exchange*/
			{
				uint8_t Rd = (p_addr & 0b111); //Rs or Hs
				uint8_t Rs = (p_addr & 0b111000) >> 3; // Rs or Hs
				switch ((p_addr & 0b0000001111000000) >> 6) {
				case 0b0001: //ADD Rd, Rd, Hs
					regs[Rd] += regs[Rs + 8];
					break;
				case 0b0010: //ADD Hd, Hd, Rs
					regs[Rd + 8] += regs[Rs];
					break;
				case 0b0011: //ADD Hd, Hd, Hs
					regs[Rd + 8] += regs[Rs + 8];
					break;
				case 0b0101: //CMP Rd, Hs
					/*TODO: implement C flag*/
					break;
				case 0b0110: //CMP Hd, Rs
					/*TODO: implement C flag*/
					break;
				case 0b0111: //CMP Hd, Hs
					/*TODO: implement C flag*/
					break;
				case 0b1001: //MOV Rd, Hs
					regs[Rd] = regs[Rs + 8];
					break;
				case 0b1010: //MOV Hd, Rs
					regs[Rd + 8] = regs[Rs];
					break;
				case 0b1011: //MOV Hd, Hs
					regs[Rd + 8] = regs[Rs + 8];
					break;
				case 0b1100: //BX Rs
					regs[15] = regs[Rs];
					/*TODO: switch the state of processor*/
					break;
				case 0b1101: //BX Hs
					regs[15] = regs[Rs + 8];
					/*TODO: switch the state of processor*/
					break;
				}
			}
		}
		break;
	case 0b01001: //PC-relative load LDR Rd, [PC, #Imm]
	{
		uint32_t Rd = p_addr & 0b0000011100000000 >> 8;
		uint32_t Word8 = p_addr & 0x00FF;
		regs[Rd] = flash[regs[15] + Word8]; //TODO: implement reader from address
		break;
	}
	case 0b01010: /*TODO: implement this instruction*/
		break;
	/*TODO: implement other instructions*/
	default: // For error message
		fprintf(stderr, "No instruction detected!\n");
		break;
	}

	return;
}


//function for 
inline void read_16th_inst(uint16_t* p_addr, uint32_t regs[], uint8_t flash[]) {
	
	return;
}

inline void branch_inst(uint16_t p_addr, uint32_t regs[], uint8_t flash[]) {
	return;
}

inline void read_32th_inst(uint32_t p_addr, uint32_t regs[], uint8_t flash[]) {
	return;
}

//This is for functions which decide thumb instructions

void set_thumb_instruction(uint16_t p_addr, uint32_t regs[], uint8_t flash[]) {

	if ((p_addr & 0xe0) != 0xe0) { //if they are 16 bit operations
		read_16th_inst(p_addr, regs, flash);
		#ifdef _DEBUG
		printf("16bit thumb instruction executed.\n");
		#endif //_DEBUG
	}
	else if ((p_addr & 0x18) == 0x18) {
		branch_inst(p_addr, regs, flash);
	}
	else { //if they are 32 bit operations
		read_32th_inst((uint32_t)p_addr, regs, flash);
		#ifdef _DEBUG
		printf("32bit thumb instruction executed.\n");
		#endif //_DEBUG
	}

	return;

}
