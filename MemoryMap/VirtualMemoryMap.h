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

};

#endif // _DUMMY_MEMORY_MAP_HPP_

