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
	virtual bool CanRead(uint32_t address) const override;

	virtual bool CanWrite(uint32_t address) const override;

	virtual bool Read(uint32_t address, uint32_t & value) override;

	virtual bool Write(uint32_t address, uint32_t value) override;

};

#endif // _DUMMY_MEMORY_MAP_HPP_

