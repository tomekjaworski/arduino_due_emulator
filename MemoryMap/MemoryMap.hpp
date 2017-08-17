#if !defined(_MEMORY_MAP_HPP_)
#define _MEMORY_MAP_HPP_

#include <stdint.h>

class IMemoryLoaderConnector
{
public:
	virtual bool LoaderCanRead(uint32_t address) const = 0;
	virtual bool LoaderCanWrite(uint32_t address) const = 0;

	virtual bool LoaderRead(uint32_t address, uint8_t& value) = 0;
	virtual bool LoaderWrite(uint32_t address, uint8_t value) = 0;
};


#endif // _MEMORY_MAP_HPP_

