#define BITBAND_SRAM_REF   0x20000000
#define BITBAND_SRAM_BASE  0x22000000

#define BITBAND_SRAM(a,b) ((BITBAND_SRAM_BASE + ((a - BITBAND_SRAM_REF) * 0x20) + (b * 4)))  // Convert SRAM address

#define	R0 0
#define	R1 1
#define R2 2
#define R3 3
#define R4 4
#define R5 5
#define R6 6
#define R7 7
#define R8 8
#define R9 9
#define R10 10
#define R11 11
#define R12 12
#define R13 13
#define R14 14
#define R15 15

typedef struct {
	//GENERAL PURPOSES REGISTERS
	// 0-7 low registers
	// 8-12 high registers

	//SPECIAL PURPOSE REGISTER
	//13 SP Stack pointer
	/*TODO: we have to devide SP to PSP, MSP*/
	//14 LR Link Register
	//15 PC Program Counter
	uint32_t general[16] = {};

	//SPECIAL REGISTERS
	uint32_t PSR = 0, PRIMASK = 0, FAULTMASK = 0, BASEPRI = 0, CONTROL = 0;
}REGISTERS;

typedef struct {
	unsigned int C = 0;
	unsigned int N = 0;
	unsigned int Z = 0;
	unsigned int V = 0;
}CPSR;

typedef struct {
	REGISTERS registers;
	CPSR flags;
}CPU;
#pragma once
