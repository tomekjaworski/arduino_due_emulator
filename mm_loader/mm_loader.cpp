// mm_loader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdint.h>

#include <unordered_map>
#include <memory>

class IMemoryLoaderSink
{
public:
	virtual bool CanRead(uint32_t address) const = 0;
	virtual bool CanWrite(uint32_t address) const = 0;

	virtual bool Read(uint32_t address, uint8_t& value) = 0;
	virtual bool Write(uint32_t address, uint8_t value) = 0;
};



class DummyMemoryMap : public IMemoryLoaderSink
{
private:
	std::unordered_map<uint32_t, std::unique_ptr<uint8_t[]>> pages;

public:


	// Inherited via IMemoryLoaderSink
	virtual bool CanRead(uint32_t address) const override {	return false; }

	virtual bool CanWrite(uint32_t address) const override { return false; }

	virtual bool Read(uint32_t address, uint8_t & value) override
	{
		return false;
	}

	virtual bool Write(uint32_t address, uint8_t value) override
	{
		uint32_t page_id = address / 128;
		address %= 128;

		if (pages.find(page_id) == pages.end())
			pages[page_id] = std::make_unique<uint8_t[]>(128); // .insert({ page_id, std::make_shared<uint8_t[128]>() });

		pages[page_id][address] = value;
		return true;
	}

};




int main()
{
	DummyMemoryMap dmm;
    return 0;
}

