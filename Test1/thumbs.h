/*Unions for bit field*/

//union for move shifted register
union {
	uint16_t raw = 0;
	uint16_t FAULT_QUEUE : 2;
	uint16_t CT_PIN_POLARITY;
	bits16<5, 6, 7, 8>    INT_PIN_POLARITY;
	bits16<9>    INT_CT_MODE;
	bits16<5, 6> OPCODE;
	bits16<7>    RESOLUTION;
} movs;

#pragma once
