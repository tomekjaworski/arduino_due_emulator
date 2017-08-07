#pragma once
#include <cstdint>
/**
types
-----
0	Data Processing / PSR Transfer
1	Multiply
2	Multiply Long
3	Single Data Swap
4	Branch and Exchange
5	Halfword Data Transfer (Register Offset)
6	Halfword Data Transfer (Immediate Offset)
7	Single Data Transfer
8	Undefined
9	Block Data Transfer
10	Branch
11	Coprocessor Data Transfer
12	Coprocessor Data Operation
13	Coprocessor Register Transfer
14	Software Interrupt
**/

struct INSTRUCTION_MASKS {
	unsigned int type;
	uint32_t mask;
	uint32_t value;
};

INSTRUCTION_MASKS masks[15] = { { 0, 0x0c000000, 0x00000000 },{ 1, 0x0fc000f0, 0x00000090 },{ 2, 0x0f8000f0, 0x00800090 }, \
{3, 0x0fb00ff0, 0x01000090},{ 4, 0x0ffffff0, 0x012fff10 },{ 5, 0x0e400f90, 0x00000090 }, \
{6, 0x0e400090, 0x00400090},{ 7, 0x0c000000, 0x04000000 },{ 8, 0x0e000010, 0x06000010 }, \
{9, 0x0e000000, 0x08000000},{ 10, 0x0e000000, 0x0a000000 },{ 11, 0x0e000000, 0x0c000000 }, \
{12, 0x0f000010, 0x0e000000},{ 13, 0x0f000010, 0x0e000010 },{ 14, 0x0f000000, 0x0f000000 } };
