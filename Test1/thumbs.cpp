#include "thumbs.h"


// This file is for THUMB instruction set

/* Templete for bit field re-invention*/
template <class T, uint16_t s, uint16_t e = s>
struct bits {
	T ref;
	static constexpr T mask = (T)(~((T)(~0) << (e - s + 1))) << s;
	void operator=(const T val) { ref = (ref & ~mask) | ((val & (mask >> s)) << s); }
	operator T() const { return (ref & mask) >> s; }
};

template <uint16_t s, uint16_t e = s>
using bits16 = bits<uint16_t, s, e>;

/*Unions for bit field*/

//union for move shifted register
union {
	uint16_t raw = 0;
	bits16<0, 1, 2> FAULT_QUEUE;
	bits16<3, 4>    CT_PIN_POLARITY;
	bits16<5, 6, 7, 8>    INT_PIN_POLARITY;
	bits16<4>    INT_CT_MODE;
	bits16<5, 6> OPCODE;
	bits16<7>    RESOLUTION;
} movs;


static inline void ReadMemory(uint16_t& p_addr) {


}

static inline void read_16th_inst(uint16_t& p_addr, uint32_t* regs[]) {
	// to decide which opcode this function uses first 5 bits 
	switch((p_addr & 0xF800) >> 11) {
		/*Move shifted register instructions family*/
	case 0b00000: //MOVS Rd, Rs, LSL #Offset5
		regs[ p_addr & 0x0007] = regs[ (p_addr & 0x0038) >> 3] << ((p_addr & 0x0560) >> 6) //writtten with bit shift & mask
		/*TODO: to put other way to write
		regs[ ]
		*/
		break;
	case 0b00001: //MOVS Rd, Rs, LSR #Offset5
		regs[p_addr & 0x0007] = regs[(p_addr & 0x0038) >> 3] >> ((p_addr & 0x0560) << 6) //writtten with bit shift & mask
		break;
	case 0b00010: //MOVS Rd, Rs, ASR #Offset5
		uint32_t& Rd = regs[(p_addr & 0x0038) >> 3];
		if ((Rd & 0x8000) == 0x8000) { //if the top bit is 1
			//TODO: implement arithmatical bit shift
		}else { // if the top bit is 0
			regs[p_addr & 0x0007] = Rd >> ((p_addr & 0x0560) >> 6) //this is the same as logical
		}
		break;

		/*Add, Substract instructions family*/
	case 0b00011:
		switch(p_addr & 0x0600 >> 8) {
		case 0b00: //ADDS Rd, Rs, Rn
			regs[p_addr & 0x0007] = reg[(p_addr & 0x0038) >> 3] + reg[(p_addr & 0x01C0) >> 6];
			break;
		case 0b01: //ADDS Rd, Rs, #Offset3
			regs[p_addr & 0x0007] = reg[(p_addr & 0x0038) >> 3] + ((p_addr & 0x01C0) >> 6);
			break;
		case 0b10: //SUBS Rd, Rs, Rn
			regs[p_addr & 0x0007] = reg[(p_addr & 0x0038) >> 3] - reg[(p_addr & 0x01C0) >> 6];
			break;
		case 0b11: //ADDS Rd, Rs, #Offset3
			regs[p_addr & 0x0007] = reg[(p_addr & 0x0038) >> 3] - ((p_addr & 0x01C0) >> 6);
			break;
		}
		break;

		//move, compare, add, substract immediate family
	case 0b00100: //MOVS Rd, #Offset8
		regs[(p_addr & 0b0000011100000000) >> 8] = (p_addr & 0x00FF);
		break;
	case 0b00101: //CMP Rd, #Offset8
		regs[(p_addr & 0b0000011100000000) >> 8] - (p_addr & 0x00FF);
		break;
	case 0b00110: //ADD Rd, Rd, #Offset8
		regs[(p_addr & 0b0000011100000000) >> 8] += (p_addr & 0x00FF);
		break;
	case 0b00111: //SUBS Rd, Rd, #Offset8
		regs[(p_addr & 0b0000011100000000) >> 8] -= (p_addr & 0x00FF);
		break;
	case 0b01000:
		if ((p_addr & 0b00000100000000000) != 0b0 ) {
			//ALU operations
			uint8_t Rd = (p_addr & 0b111);
			uint8_t Rs = (p_addr & 0b111000) >> 3;
			switch ((p_addr & 0000001111000000) >> 6) {
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
				//TODO: implement arithmatical bit shift
				regs[Rd] >>= regs[Rs];
				break;
			case 0b0101: //ADCS Rd, Rd, Rs
				regs[Rd] += regs[Rs] + ;//TODO: implement C bit
				break;
			case 0b0110: //SBCS Rd, Rd, Rs
				regs[Rd] >>= regs[Rs] - ;//TODO: implement C bit
				break;
			case 0b0111: //MOVS Rd, Rd, ROR Rs
				regs[Rd] += regs[Rs] + ;//TODO:
				break;
			case 0b1000: //TST Rd, Rs
				regs[Rd] -= regs[Rs] + ;//TODO
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
			case 0b1010: //MVNS Rd, Rs
				regs[Rd] = ~regs[Rs];
				break;
			}
			//Hi register operations, branch exchange
			default:
				printf("Error: no thumb instruction for bit codes!\n");
				break;
			}
		}
		break;
	default:

		break;
	}

	return;
}


static inline void read_32th_inst(unsigned __int32* p_addr) {
	switch () {


	}
	return;
}


//This is for functions which decide thumb instructions

void set_thumb_instruction(unsigned __int16* p_addr) {

	if (*p_addr & 0xe0 != 0xe0) { //if they are 16 bit operations
		read_16th_inst(p_addr);
	}
	else if (*p_addr & 0x18 == 0x18) {
		branch_inst(p_addr);
	}
	else { //if they are 32 bit operations
		read_32thinst((unsigned __int32*)p_addr);

	}

	return;

}
