// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the TEXTLOADER_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// TEXTLOADER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef TEXTLOADER_EXPORTS
#define TEXTLOADER_API __declspec(dllexport)
#else
#define TEXTLOADER_API __declspec(dllimport)
#endif


#include <stdexcept>
#include "..\MemoryMap\MemoryMap.hpp"


class LoaderExecption : public std::runtime_error
{
public:
	LoaderExecption(const std::string& msg)
		: std::runtime_error(msg)
	{
	}
};


class IntelHexParserException : public LoaderExecption
{
public:
	IntelHexParserException(const std::string& msg)
		: LoaderExecption(msg)
	{
	}
};


class IntelHexParser
{
private:
	std::string file_name;

public:
	TEXTLOADER_API IntelHexParser(const std::string& file_name);
	TEXTLOADER_API ~IntelHexParser();

	TEXTLOADER_API int32_t Load(IMemoryLoaderConnector& connector);

};

class ELFParser
{
private:
	std::string file_name;

public:
	TEXTLOADER_API ELFParser(const std::string& file_name);
	TEXTLOADER_API ~ELFParser();

	TEXTLOADER_API int32_t Load(IMemoryLoaderConnector& connector);


};
