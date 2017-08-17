#if !defined(_VIRTUAL_MEMORY_MAP_HPP_)
#define _VIRTUAL_MEMORY_MAP_HPP_


#include "MemoryMap.hpp"

#include <unordered_map>
#include <memory>


class VirtualMemoryMap : public IMemoryLoaderConnector
{
private:
	uint32_t flash_0[256 * 1024 / 4];
	uint32_t flash_1[256 * 1024 / 4];
	uint32_t rom	[128 * 1024 / 4];
	uint32_t sram_0	[64  * 1024 / 4];
	uint32_t sram_1 [64 * 1024 / 4];

public:


	// Inherited via IMemoryLoaderSink
	virtual bool CanRead(uint32_t address) const override;

	virtual bool CanWrite(uint32_t address) const override;

	virtual bool Read(uint32_t address, uint32_t & value) override;

	virtual bool Write(uint32_t address, uint32_t value) override;

	void DumpMemory() {
		int flash_size = 256 * 1024 / 4;
		int rom_size = 128 * 1024 / 4;
		int sram_size = 64 * 1024 / 4;
		int check_content = 16;

		printf("FLASH 0\n");
		printf("---------\n");
		for (int i = 0; i < (flash_size / 100); i++) {
			printf("%2x ", flash_0[i]);
			if (i % 15 == 0 && i!=0) {
				printf("\n");
			}
		}
		printf("FLASH 1\n");
		printf("---------\n");
		for (int i = 0; i < check_content; i++) {
			printf("%x\n", flash_1[i]);
		}
		printf("ROM\n");
		printf("---------\n");
		for (int i = 0; i < check_content; i++) {
			printf("%x\n", rom[i]);
		}
		printf("SRAM 0\n");
		printf("---------\n");
		for (int i = 0; i < check_content; i++) {
			printf("%x\n", sram_0[i]);
		}
		printf("SRAM 1\n");
		printf("---------\n");
		for (int i = 0; i < check_content; i++) {
			printf("%x\n", sram_1[i]);
		}
	}
};

#endif // _DUMMY_MEMORY_MAP_HPP_

