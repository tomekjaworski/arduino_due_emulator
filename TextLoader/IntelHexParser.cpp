// TextLoader.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "TextLoader.hpp"
#include <fstream>
#include <boost\algorithm\string.hpp>
#include <boost\format.hpp>
namespace b = boost;


IntelHexParser::IntelHexParser(const std::string& file_name)
	: file_name(file_name)
{
}

IntelHexParser::~IntelHexParser()
{
}


int32_t IntelHexParser::Load(IMemoryLoaderConnector & connector)
{
	std::ifstream file(this->file_name, std::ios::in);
	if (!file.is_open())
		throw IntelHexParserException(b::str(b::format("Unable to load file %1%") % this->file_name));

	int row_num = 0;
	uint32_t address_high = 0;
	uint32_t segment = 0;
	int bytes_read = 0;

	try {
		while (file.good())
		{
			std::string row;
			std::getline(file, row);
			row_num++;
			row = b::to_upper_copy(b::trim_copy(row));

			// ignore empty lines
			if (row.empty())
				continue;

			// expect ':'
			if ((row[0] != ':'))
				throw IntelHexParserException("Invalid row start");

			if (row.size() > 521 || ((row.size() % 2) == 0))
				throw IntelHexParserException("Invalid row length");


			// read the data
			uint8_t data[600];
			int count = row.size() / 2;
			for (uint32_t i = 1; i < row.size(); i += 2)
			{
				if (!isxdigit(row[i]) || !isxdigit(row[i + 1]))
					throw IntelHexParserException("Hex digit expected");

				data[i / 2] = row[i + 1] >= 'A' ? row[i + 1] - 'A' + 10 : row[i + 1] - '0';
				data[i / 2] |= (row[i] >= 'A' ? row[i] - 'A' + 10 : row[i] - '0') << 4;
			}

			// verify the checksum
			uint8_t chk = 0;
			for (int i = 0; i < count; i++)
				chk += data[i];

			if (chk != 0x00)
				throw IntelHexParserException("Invalid checksum");

			int byte_count = data[0];
			int record_type = data[3];
			uint32_t address_low = (uint32_t)data[2] | (uint32_t)data[1] << 8;

			// last record?
			if (record_type == 1)
				break;

			// segment offset? Typical for x86 real mode
			if (record_type == 2)
			{
				if (byte_count != 2)
					throw new IntelHexParserException("Byte count expected to be 2 (x16 segment)");

				segment = (uint32_t)data[5] | (uint32_t)data[4] << 8;
				continue;
			}

			// high word of an address
			if (record_type == 4)
			{
				if (byte_count != 2)
					throw new IntelHexParserException("Byte count expected to be 2 (high word)");
				address_high = (uint32_t)data[5] | (uint32_t)data[4] << 8;
				address_high <<= 16;
				continue;
			}

			// data 
			if (record_type == 0)
			{
				uint32_t addr = address_high | address_low;
				addr += segment << 4;
				for (int i = 0; i < byte_count; i++)
				{
					if (!connector.CanWrite(addr))
						throw IntelHexParserException(b::str(b::format("Unable to write data to address %08x") % addr));
					connector.Write(addr, data[i + 4]);
					addr++;
					bytes_read++;
				}
				continue;
			}

			// entry point
			if (record_type == 5)
			{
				// for future use
				uint32_t ep_addr = data[4] << 24 | data[5] << 16 | data[6] << 8 | data[7];
				continue;
			}

			throw IntelHexParserException(b::str(b::format("Unknown record type: %02x") % record_type));


		}
	}
	catch (IntelHexParserException& ex)
	{
		throw IntelHexParserException(b::str(b::format("Error on loading Intel Hex file %1% (row=%2%): %3%") %
			file_name % row_num % ex.what()));
	}

	return bytes_read;
}


