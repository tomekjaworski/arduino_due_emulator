#include "thumbs.h"


static inline void ReadMemory(unsigned __int16* p_addr) {


}

static inline void read_16th_inst(unsigned __int16& p_addr) {
	// to decide which opcode this function uses first 5 bits 
	switch(p_addr & 0xF800) {
		//Move shifted register instructions
	case 0x00: //MOVS Rd, Rs, LSL #Offset5
		regs[ p_addr & 0x0007] = regs[ (p_addr & 0x0038) << 3] << ((p_addr & 0x0560) << 6)// immediate value of bit shift
		break;
	case 0x: //
		break;
	case 0x: //
		break;
	case 0x: //
		break;
	case 0x: //
		break;
	case 0x: //
		break;

	}


}


static inline void read_32th_inst(unsigned __int32* p_addr) {
	switch () {


	}

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

}
