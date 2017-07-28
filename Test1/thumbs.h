/*Unions for bit field*/

#include <cstdint>

inline void read_16th_inst(uint16_t p_addr, uint32_t regs[], uint8_t flash[]);

inline void branch_inst(uint16_t p_addr, uint32_t regs[], uint8_t flash[]);

void set_thumb_instruction(uint16_t p_addr, uint32_t regs[], uint8_t flash[]);

inline void read_32th_inst(uint32_t p_addr, uint32_t regs[], uint8_t flash[]);

#pragma once
