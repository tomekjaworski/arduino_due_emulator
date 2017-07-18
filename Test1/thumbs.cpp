#include "thumbs.h"


static inline void ReadMemory(unsigned __int16*) {


}

static inline void read_16instruction(unsigned __int16*) {
	switch () {


	}


}


static inline void read_32instruction(unsigned __int32*) {


}


//This is for functions which decide thumb instructions

void set_thumb_instruction(unsigned __int16* p_addr) {

	if (*p_addr & 0xe0 != 0xe0) { //if they are 16 bit operations
		read_16instruction(p_addr);
	}
	else if (*p_addr & 0x18 == 0x18) {
		branch_instruction(p_addr);
	}
	else { //if they are 32 bit operations
		read_32instruction((unsigned __int32*)p_addr);

	}

}
