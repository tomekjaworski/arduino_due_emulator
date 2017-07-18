#if !defined(_MEMORY_MAP_HPP_)
#define _MEMORY_MAP_HPP_

#include <stdint.h>

class IMemoryLoaderConnector
{
public:
	virtual bool CanRead(uint32_t address) const = 0;
	virtual bool CanWrite(uint32_t address) const = 0;

	virtual bool Read(uint32_t address, uint8_t& value) = 0;
	virtual bool Write(uint32_t address, uint8_t value) = 0;
};


#endif // _MEMORY_MAP_HPP_

