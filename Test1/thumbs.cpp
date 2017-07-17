#include "thumbs.h"


void read_16instruction(unsigned __int16*) {
	



}



//This is for functions which decide thumb instructions

void set_instruction(unsigned __int8* p_addr) {

	if (*p_addr &  0xf800==0xe000) { //if they are 16 bit operations
		read_16instruction()
	}
	else { //if they are 32 bit operations


	}

}
