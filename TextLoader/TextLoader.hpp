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


class TEXTLOADER_API IntelHexParserException : public LoaderExecption
{
public:
	IntelHexParserException(const std::string& msg)
		: LoaderExecption(msg)
	{
	}
};


class TEXTLOADER_API IntelHEXParser
{
private:
	std::string file_name;

public:
	IntelHEXParser(const std::string& file_name);
	~IntelHEXParser();

	int32_t Load(IMemoryLoaderConnector& connector);

};


/*
// This class is exported from the TextLoader.dll
class TEXTLOADER_API CTextLoader {
public:
	CTextLoader(void);
	// TODO: add your methods here.
};

extern TEXTLOADER_API int nTextLoader;

TEXTLOADER_API int fnTextLoader(void);
*/