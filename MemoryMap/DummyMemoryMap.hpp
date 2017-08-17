#if !defined(_DUMMY_MEMORY_MAP_HPP_)
#define _DUMMY_MEMORY_MAP_HPP_


#include "MemoryMap.hpp"

#include <unordered_map>
#include <memory>


class DummyMemoryMap : public IMemoryLoaderConnector
{
private:
	std::unordered_map<uint32_t, std::unique_ptr<uint8_t[]>> pages;

public:


	// Inherited via IMemoryLoaderSink
	bool CanLoaderRead(uint32_t address);

	bool CanLoaderWrite(uint32_t address);

	bool LoaderRead(uint32_t address, uint8_t & value);

	bool LoaderWrite(uint32_t address, uint8_t value);

};

#endif // _DUMMY_MEMORY_MAP_HPP_

