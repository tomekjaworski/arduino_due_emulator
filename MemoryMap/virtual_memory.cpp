#include "stdafx.h"
#include "VirtualMemoryMap.h"

/** SECTIONS OF MEMORY*/
#define CODE_SECTION_START	0x00000000
#define CODE_SECTION_STOP	0x1FFFFFFF

#define	SRAM_SECTION_START	0x20000000
#define SRAM_SECTION_STOP	0x3FFFFFFF

/** SUBSECTIONS OF CODE SECTION*/
#define BOOT_START			0x00000000
#define BOOT_END			0x0007FFFF
#define FLASH_0_START		0x00080000
#define FLASH_0_END			0x000BFFFF
#define FLASH_1_START		0x000C0000
#define	FLASH_1_END			0x000FFFFF
#define	ROM_START			0x00100000
#define ROM_STOP			0x001FFFFF

/** SUBSECTIONS OF SRAM SECTION*/
#define SRAM_0_START		0x20000000
#define	SRAM_0_END			0x2007FFFF
#define	SRAM_1_START		0x20080000
#define	SRAM_1_END			0x200FFFFF

bool VirtualMemoryMap::CanRead(uint32_t address) const
{
	return false;
}

bool VirtualMemoryMap::CanWrite(uint32_t address) const
{
	return true;
}

bool VirtualMemoryMap::Read(uint32_t address, uint8_t & value)
{
	return false;
}

bool VirtualMemoryMap::Write(uint32_t address, uint8_t value)
{
	
	return true;
}