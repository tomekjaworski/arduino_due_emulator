//

#include "stdafx.h"
#include "thumbs.h"


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

//
union {
	uint16_t raw = 0;
	struct {
		uint16_t	START_BIT : 4;
		uint16_t	L : 1;
		uint16_t	Rd : 3;
		uint16_t	Word8 : 3;
	};
} lost_at;

//static inline void read_16th_inst(uint16_t& p_addr, uint32_t regs[], uint8_t flash[]) {
//	// to decide which opcode this function uses first 5 bits 
//	switch((p_addr & 0xF800) >> 11) {
//		/*Move shifted register instructions family*/
//	case 0b00000: //MOVS Rd, Rs, LSL #Offset5
//		regs[p_addr & 0x0007] = regs[(p_addr & 0x0038) >> 3] << ((p_addr & 0x0560) >> 6); //writtten with bit shift & mask
//		/*TODO: to put other way to write
//		regs[ ]
//		*/
//		break;
//	case 0b00001: //MOVS Rd, Rs, LSR #Offset5
//		regs[p_addr & 0x0007] = regs[(p_addr & 0x0038) >> 3] >> ((p_addr & 0x0560) << 6); //writtten with bit shift & mask
//		break;
//	case 0b00010: //MOVS Rd, Rs, ASR #Offset5
//		uint32_t& Rd = regs[(p_addr & 0x0038) >> 3];
//		if ((Rd & 0x8000) == 0x8000) { //if the top bit is 1
//			//TODO: implement arithmatical bit shift
//		}else { // if the top bit is 0
//			regs[p_addr & 0x0007] = Rd >> ((p_addr & 0x0560) >> 6); //this is the same as logical
//		}
//		break;
//
//		/*Add, Substract instructions family*/
//	case 0b00011:
//		switch(p_addr & 0x0600 >> 8) {
//		case 0b00: //ADDS Rd, Rs, Rn
//			regs[p_addr & 0x0007] = regs[(p_addr & 0x0038) >> 3] + regs[(p_addr & 0x01C0) >> 6];
//			break;
//		case 0b01: //ADDS Rd, Rs, #Offset3
//			regs[p_addr & 0x0007] = regs[(p_addr & 0x0038) >> 3] + ((p_addr & 0x01C0) >> 6);
//			break;
//		case 0b10: //SUBS Rd, Rs, Rn
//			regs[p_addr & 0x0007] = regs[(p_addr & 0x0038) >> 3] - regs[(p_addr & 0x01C0) >> 6];
//			break;
//		case 0b11: //ADDS Rd, Rs, #Offset3
//			regs[p_addr & 0x0007] = regs[(p_addr & 0x0038) >> 3] - ((p_addr & 0x01C0) >> 6);
//			break;
//		}
//		break;
//
//		//move, compare, add, substract immediate family
//	case 0b00100: //MOVS Rd, #Offset8
//		regs[(p_addr & 0b0000011100000000) >> 8] = (p_addr & 0x00FF);
//		break;
//	case 0b00101: //CMP Rd, #Offset8
//		if (regs[(p_addr & 0b0000011100000000) >> 8] - (p_addr & 0x00FF) > 0) {
//			//TODO:
//		};
//		break;
//	case 0b00110: //ADD Rd, Rd, #Offset8
//		regs[(p_addr & 0b0000011100000000) >> 8] += (p_addr & 0x00FF);
//		break;
//	case 0b00111: //SUBS Rd, Rd, #Offset8
//		regs[(p_addr & 0b0000011100000000) >> 8] -= (p_addr & 0x00FF);
//		break;
//	case 0b01000:
//		if ((p_addr & 0b00000100000000000) != 0b0) {
//			//ALU operations
//			uint8_t Rd = (p_addr & 0b111);
//			uint8_t Rs = (p_addr & 0b111000) >> 3;
//			switch ((p_addr & 0b0000001111000000) >> 6) {
//			case 0b0000: //ANDS Rd, Rd, Rs
//				regs[Rd] &= regs[Rs];
//				break;
//			case 0b0001: //EORS Rd, Rd, Rs
//				regs[Rd] ^= regs[Rs];
//				break;
//			case 0b0010: //MOVS Rd, Rd, LSL Rs
//				regs[Rd] <<= regs[Rs];
//				break;
//			case 0b0011: //MOVS Rd, Rd, LSR Rs
//				regs[Rd] >>= regs[Rs];
//				break;
//			case 0b0100: //MOVS Rd, Rd, ASR 
//				//TODO: implement arithmatical bit shift
//				regs[Rd] >>= regs[Rs];
//				break;
//			case 0b0101: //ADCS Rd, Rd, Rs
//				regs[Rd] += regs[Rs];//TODO: implement C bit
//				break;
//			case 0b0110: //SBCS Rd, Rd, Rs
//				regs[Rd] >>= regs[Rs];//TODO: implement C bit
//				break;
//			case 0b0111: //MOVS Rd, Rd, ROR Rs
//				regs[Rd] += regs[Rs];//TODO:
//				break;
//			case 0b1000: //TST Rd, Rs
//				regs[Rd] -= regs[Rs];//TODO
//				break;
//			case 0b1001: //RSBS Rd, Rd, #0
//				regs[Rd] = -regs[Rs]; //TODO:
//				break;
//			case 0b1010: //CMP Rd, Rs
//				regs[Rd] >>= regs[Rs]; //TODO
//				break;
//			case 0b1011: //CMN Rd, Rs
//				regs[Rd] >>= regs[Rs]; //TODO
//				break;
//			case 0b1100: //ORRS Rd, Rd, Rs
//				regs[Rd] |= regs[Rs];
//				break;
//			case 0b1101: //MULS Rd, Rs, Rd
//				regs[Rd] *= regs[Rs];
//				break;
//			case 0b1110: //BICS Rd, Rd, Rs
//				regs[Rd] &= ~regs[Rs];
//				break;
//			case 0b1111: //MVNS Rd, Rs
//				regs[Rd] = ~regs[Rs];
//				break;
//			}
//		}
//		else {
//			//Hi register operations, branch exchange
//			uint8_t Rd = (p_addr & 0b111); //Rs or Hs
//			uint8_t Rs = (p_addr & 0b111000) >> 3; // Rs or Hs
//			switch ((p_addr & 0b0000001111000000) >> 6) {
//			case 0b0001: //ADD Rd, Rd, Hs
//				regs[Rd] += regs[Rs + 8];
//				break;
//			case 0b0010: //ADD Hd, Hd, Rs
//				regs[Rd + 8] += regs[Rs];
//				break;
//			case 0b0011: //ADD Hd, Hd, Hs
//				regs[Rd + 8] += regs[Rs + 8];
//				break;
//			case 0b0101: //CMP Rd, Hs
//				//TODO: implement C flag
//				break;
//			case 0b0110: //CMP Hd, Rs
//				//TODO: implement C flag
//				break;
//			case 0b0111: //CMP Hd, Hs
//				//TODO: implement C flag
//				break;
//			case 0b1001: //MOV Rd, Hs
//				regs[Rd] = regs[Rs + 8];
//				break;
//			case 0b1010: //MOV Hd, Rs
//				regs[Rd + 8] = regs[Rs];
//				break;
//			case 0b1011: //MOV Hd, Hs
//				regs[Rd + 8] = regs[Rs + 8];
//				break;
//			case 0b1100: //BX Rs
//				regs[15] = regs[Rs];
//				//TODO: switch the state of processor
//				break;
//			case 0b1101: //BX Hs
//				regs[15] = regs[Rs + 8];
//				//TODO: switch the state of processor
//				break;
//			}
//		}
//		break;
//	case 0b01001: //PC-relative load LDR Rd, [PC, #Imm]
//		unsigned int Rd = p_addr & 0b0000011100000000 >> 8;
//		unsigned int Word8 = p_addr & 0x00FF;
//		regs[Rd] = flash[regs[15] + Word8]; //TODO: implement reader from address
//		break;
//
//	case 0b01010: //
//	default: // For error message
//		fprintf(stderr, "No instruction detected!\n");
//		break;
//	}
//
//	return;
//}

void read_16th_inst(uint16_t* p_addr, uint32_t regs[], uint8_t flash[]) {
	return;
}

void branch_inst(uint16_t* p_addr, uint32_t regs[], uint8_t flash[]) {
	return;
}

void read_32th_inst(uint32_t* p_addr, uint32_t regs[], uint8_t flash[]) {
	return;
}

//This is for functions which decide thumb instructions

void set_thumb_instruction(uint16_t* p_addr, uint32_t regs[], uint8_t flash[]) {

	if ((*p_addr & 0xe0) != 0xe0) { //if they are 16 bit operations
		read_16th_inst(p_addr, regs, flash);
	}
	else if ((*p_addr & 0x18) == 0x18) {
		branch_inst(p_addr, regs, flash);
	}
	else { //if they are 32 bit operations
		read_32th_inst((uint32_t*)p_addr, regs, flash);

	}

	return;

}
