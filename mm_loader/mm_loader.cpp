// mm_loader.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdint.h>

#include <stdexcept>
#include <iostream>
#include <string>

#include "..\MemoryMap\DummyMemoryMap.hpp"
#include "..\TextLoader\TextLoader.hpp"


void intelhex_test(const char* file_name, bool expected_result)
{
	DummyMemoryMap dmm;
	IntelHEXParser p(file_name);

	std::cout << "Test for " << file_name << ": ";

	bool exception = false;
	std::string msg = "";
	int bytes = -1;
	try {
		bytes = p.Load(dmm);
	}
	catch (const std::exception& ex)
	{
		exception = true;
		msg = ex.what();
	}
	catch (...)
	{
		exception = true;
		msg = "(...)";
	}



	if (!exception == expected_result)
		std::cout << " AS EXPECTED";
	else
		std::cout << " ERROR";
	if (exception)
		std::cout << " Exception: " << msg;
	else
		std::cout << "; Byte count=" << bytes;

	std::cout << std::endl;


}

int main()
{
	intelhex_test("test_hex\\test1_ok.hex", 1);
	intelhex_test("test_hex\\test2_ok.hex", 1);
	intelhex_test("test_hex\\test3_ok.hex", 1);
	intelhex_test("test_hex\\test4_ok.hex", 1);
	intelhex_test("test_hex\\test5_ok.hex", 0);
	intelhex_test("test_hex\\test6_ok.hex", 0);
	intelhex_test("test_hex\\test7_ok.hex", 1);
	intelhex_test("test_hex\\test8_ok.hex", 1);
	intelhex_test("test_hex\\test9_ok.hex", 1);
	intelhex_test("test_hex\\test10_ok.hex", 1);
	return 0;
}
