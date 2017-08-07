#include "stdafx.h"
#include "VirtualMemoryMap.h"

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
	uint32_t page_id = address / 128;
	address %= 128;

	if (pages.find(page_id) == pages.end())
		pages[page_id] = std::make_unique<uint8_t[]>(128); // .insert({ page_id, std::make_shared<uint8_t[128]>() });

	pages[page_id][address] = value;
	return true;
}


