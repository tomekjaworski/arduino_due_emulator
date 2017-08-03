
#include "stdafx.h"
#include "TextLoader.hpp"
#include <fstream>
#include <boost\algorithm\string.hpp>
#include <boost\format.hpp>
namespace b = boost;


ELFParser::ELFParser(const std::string& file_name)
	: file_name(file_name)
{
}

ELFParser::~ELFParser()
{
}


int32_t ELFParser::Load(IMemoryLoaderConnector & connector)
{
	std::ifstream file(this->file_name, std::ios::in | std::ios::binary);
	if (!file.is_open())
		throw IntelHexParserException(b::str(b::format("Unable to load file %1%") % this->file_name));



	return -1;
}

