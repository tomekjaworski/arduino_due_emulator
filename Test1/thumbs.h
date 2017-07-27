/*Unions for bit field*/

#include <cstdint>

static inline void read_16th_inst(uint16_t& p_addr, uint32_t* regs[], uint8_t flash[]);

static inline void set_thumb_instruction(unsigned __int16* p_addr);

void branch_inst(uint16_t* p_addr, uint32_t regs[], uint8_t flash[]);

void set_thumb_instruction(uint16_t* p_addr, uint32_t regs[], uint8_t flash[]);

static inline void read_32th_inst(uint32_t& p_addr, uint32_t* regs[], uint8_t flash[]);

#pragma once
