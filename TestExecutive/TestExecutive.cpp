/////////////////////////////////////////////////////////////////////
//  TestExecutive.cpp - This class demonstartes requirement		   //
//  ver 1.0                                                        //
//  Lanaguage:     C++ Visual Studio 2015                          //
//  Platform:      Dell XPS L510X - Windows 10		               //
//	Application:   Project#4 - CSE 687-Object Oriented Design	   //
//  Author:        Rohit Sharma, Syracuse University			   //
//                 (315) 935-1323, rshar102@syr.edu		           //
/////////////////////////////////////////////////////////////////////
/*
Module Operations:
==================
This class defines demonstrateReqs for demonstration purpose.

Public Interface:
=================
void demonstrateReqs();

Build Process:
==============
Required files
Build commands (either one)
- devenv CommPrototype.sln /rebuild debug

Maintenance History:
====================
ver 1.0 : 03 May 16
- first release
*/
#include "TestExecutive.h"
#include "../Utilities/Utilities.h"
#include<iostream>
#include<conio.h>
#define Util StringHelper
using namespace Utilities;

void TestExecutive::demonstrateReqs()
{
	StringHelper::title("Demonstrating - Requirement 3");
	std::cout << std::endl;
	std::cout << std::endl << " Provided a Repository program that provides a checkin process";
	std::cout << std::endl << " Each checkin will support copying and providing metadata for all files in the Repository" << std::endl;
	StringHelper::title("Demonstrating - Requirement 5");
	std::cout << std::endl;
	std::cout << std::endl << " Each checkin will create a new directory with a unique name based on the file name and checkin-date" << std::endl;
	std::cout << std::endl << " Each checkin directory contains checkin metadata containing dependencies among files" << std::endl;
	StringHelper::title("Demonstrating - Requirement 6");
	std::cout << std::endl;
	std::cout << " Each checkin directory have a boolean status property with value as open/closed." << std::endl;
	StringHelper::title("Demonstrating - Requirement 7");
	std::cout << std::endl;
	std::cout << std::endl << " Provided an extraction process for modules or packages with or without dependencies." << std::endl;
	StringHelper::title("Demonstrating - Requirement 8");
	std::cout << std::endl;
	std::cout << std::endl << " Provided a message-passing communication system used to access the Repository's functionality from another process or machine.";
	StringHelper::title("Demonstrating - Requirement 9");
	std::cout << std::endl;
	std::cout << std::endl << " Communication system supports for passing HTTP style messages using asynchronous one-way messaging" << std::endl;
	StringHelper::title("Demonstrating - Requirement 10");
	std::cout << std::endl;
	std::cout << std::endl << " Communication system supports sending and receiving streams of bytes. Streams will be established with an initial exchange of messages." << std::endl;
	StringHelper::title("Demonstrating - Requirement 11");
	std::cout << std::endl;
	std::cout << std::endl << " Automated unit test suite provided that exercises all the file placed inside TestFiles folder in project demonstrating that I met all requirements." << std::endl;
	_getch();
}
#ifdef TESTEXE
int main() {
	TestExecutive exec;
	exec.demonstrateReqs();
}
#endif
