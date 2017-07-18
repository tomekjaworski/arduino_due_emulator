// Test1.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"


int main()
{
	//set registers and flash
	unsigned __int32 regs[16];
	unsigned __int8 flash[512 * 1024] = {};

	//special registers
	unsigned __int8 psr;
	unsigned __int8 primask;
	unsigned __int8 faultmask;
	unsigned __int8 basepri;
	unsigned __int8 control;


	//Set initiation value on regs

	regs[14] = 0xFFFFFFFF;
	psr = 0x01000000;
	primask = 0x00000000;
	faultmask = 0x00000000;
	basepri = 0x00000000;
	control = 0x00000000;

	//read commands file to memory



	//

    return 0;
}

