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
#define ROM_END				0x001FFFFF
#define RESERVED_CODE_START	0x00200000
#define	RESERVED_CODE_END	0x1FFFFFFF

/** SUBSECTIONS OF SRAM SECTION*/
#define SRAM_0_START		0x20000000
#define	SRAM_0_END			0x2007FFFF
#define	SRAM_1_START		0x20080000
#define	SRAM_1_END			0x200FFFFF
#define	NFC_SRAM_START		0x20100000
#define	NFC_SRAM_END		0x2017FFFF
#define	UOTGHS_DMA_START	0x20180000
#define	UOTGHS_DMA_END		0x201FFFFF
#define	UNDEFINED_ABORT_START	0x20200000
#define	UNDEFINED_ABORT_END		0x3FFFFFFF

/**
	TODO
	------------
	1) Check bitbandings.
*/
bool VirtualMemoryMap::LoaderCanRead(uint32_t address) const
{
	if (!(address >= RESERVED_CODE_START && address <= RESERVED_CODE_END) && \
		!(address >= UNDEFINED_ABORT_START && address <= UNDEFINED_ABORT_END)) {
		return true;
	}
	else {
		printf("Someone try to access restirected area! virtual_memory.cpp CanRead\n");
		return false;
	}
}

bool VirtualMemoryMap::LoaderCanWrite(uint32_t address) const
{
	if (!(address >= ROM_START && address <= ROM_END) &&\
		!(address >= RESERVED_CODE_START && address <= RESERVED_CODE_END) &&\
		!(address >= UNDEFINED_ABORT_START && address <= UNDEFINED_ABORT_END)) {
		return true;
	}
	else {
		printf("Someone try to access restirected area! virtual_memory.cpp CanWrite\n");
		return false;
	}
}

bool VirtualMemoryMap::Read(uint32_t address, uint32_t& value) {
	/*if (LoaderCanWrite(address)) {
		if (address >= FLASH_0_START && address <= FLASH_0_END) {
			flash_0[address - FLASH_0_START] = value;
		}

		if (address >= FLASH_1_START && address <= FLASH_1_END) {
			flash_1[address - FLASH_1_START] = value;
		}

		if (address >= ROM_START && address <= ROM_END) {
			rom[address - ROM_START] = value;
		}

		if (address >= SRAM_0_START && address <= SRAM_0_END) {
			sram_0[address - SRAM_0_START] = value;
		}

		if (address >= SRAM_1_START && address <= SRAM_1_END) {
			sram_1[address - SRAM_1_START] = value;
		}
	}*/
	return true;
}

bool VirtualMemoryMap::Write(uint32_t address, uint32_t value)  {
	/*if (LoaderCanWrite(address)) {
		if (address >= FLASH_0_START && address <= FLASH_0_END) {
			flash_0[address - FLASH_0_START] = value;
		}

		if (address >= FLASH_1_START && address <= FLASH_1_END) {
			flash_1[address - FLASH_1_START] = value;
		}

		if (address >= ROM_START && address <= ROM_END) {
			rom[address - ROM_START] = value;
		}

		if (address >= SRAM_0_START && address <= SRAM_0_END) {
			sram_0[address - SRAM_0_START] = value;
		}

		if (address >= SRAM_1_START && address <= SRAM_1_END) {
			sram_1[address - SRAM_1_START] = value;
		}
	}*/
	return true;
}

bool VirtualMemoryMap::LoaderRead(uint32_t address, uint8_t& value) 
{
	if (LoaderCanRead(address)) {
		if (address >= FLASH_0_START && address <= FLASH_0_END) {
			value = flash_0[address - FLASH_0_START];
		}

		if (address >= FLASH_1_START && address <= FLASH_1_END) {
			value = flash_1[address - FLASH_1_START];
		}

		if (address >= ROM_START && address <= ROM_END) {
			value = rom[address - ROM_START];
		}

		if (address >= SRAM_0_START && address <= SRAM_0_END) {
			value = sram_0[address - SRAM_0_START];
		}

		if (address >= SRAM_1_START && address <= SRAM_1_END) {
			value = sram_1[address - SRAM_1_START];
		}
	}
	return false;
}

bool VirtualMemoryMap::LoaderWrite(uint32_t address, uint8_t value) 
{
	if (LoaderCanWrite(address)) {
		if (address >= FLASH_0_START && address <= FLASH_0_END) {
			flash_0[address - FLASH_0_START] = value;
		}

		if (address >= FLASH_1_START && address <= FLASH_1_END) {
			flash_1[address - FLASH_1_START] = value;
		}

		if (address >= ROM_START && address <= ROM_END) {
			rom[address - ROM_START] = value;
		}

		if (address >= SRAM_0_START && address <= SRAM_0_END) {
			sram_0[address - SRAM_0_START] = value;
		}

		if (address >= SRAM_1_START && address <= SRAM_1_END) {
			sram_1[address - SRAM_1_START] = value;
		}
	}
	return true;
}



