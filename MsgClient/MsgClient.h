#pragma once
#include <thread>
/////////////////////////////////////////////////////////////////////
//  MsgClient.h - Starts Client Sender and Receivers			   //
//  ver 1.0                                                        //
//  Lanaguage:     C++ Visual Studio 2015                          //
//  Platform:      Dell XPS L510X - Windows 10		               //
//	Application:   Project#4 - CSE 687-Object Oriented Design	   //
//  Author:        Rohit Sharma, Syracuse University			   //
//                 (315) 935-1323, rshar102@syr.edu		           //
/////////////////////////////////////////////////////////////////////
/*
* Required Files:
*   MsgClient.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp

Module Operations:
==================
This class defines startClientSenderThread and startClientSenderThread and void startClient();to start client threads.

Public Interface:
=================
void startClientSenderThread();
void startClientReceiverThread();
void startClient();

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

class MsgClient
{
public:
	std::thread startClientSenderThread();
	std::thread startClientReceiverThread();
	void startClient();
};