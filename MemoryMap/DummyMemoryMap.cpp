#include "stdafx.h"
#include "DummyMemoryMap.hpp"

bool DummyMemoryMap::CanLoaderRead(uint32_t address)
{
	return false;
}

bool DummyMemoryMap::CanLoaderWrite(uint32_t address)
{
	return true;
}

bool DummyMemoryMap::LoaderRead(uint32_t address, uint8_t & value)
{
	return false;
}

bool DummyMemoryMap::LoaderWrite(uint32_t address, uint8_t value)
{
	uint32_t page_id = address / 128;
	address %= 128;

	if (pages.find(page_id) == pages.end())
		pages[page_id] = std::make_unique<uint8_t[]>(128); // .insert({ page_id, std::make_shared<uint8_t[128]>() });

	pages[page_id][address] = value;
	return true;
}


