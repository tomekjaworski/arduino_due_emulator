#include "stdafx.h"
#include "DummyMemoryMap.hpp"

bool DummyMemoryMap::CanRead(uint32_t address) const
{
	return false;
}

bool DummyMemoryMap::CanWrite(uint32_t address) const
{
	return true;
}

bool DummyMemoryMap::Read(uint32_t address, uint32_t & value)
{
	return false;
}

bool DummyMemoryMap::Write(uint32_t address, uint32_t value)
{
	uint32_t page_id = address / 128;
	address %= 128;

	if (pages.find(page_id) == pages.end())
		pages[page_id] = std::make_unique<uint8_t[]>(128); // .insert({ page_id, std::make_shared<uint8_t[128]>() });

	pages[page_id][address] = value;
	return true;
}


