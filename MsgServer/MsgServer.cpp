/////////////////////////////////////////////////////////////////////
//  MsgServer.cpp - Starts Server Sender and Receivers			   //
//  ver 1.0                                                        //
//  Lanaguage:     C++ Visual Studio 2015                          //
//  Platform:      Dell XPS L510X - Windows 10		               //
//	Application:   Project#4 - CSE 687-Object Oriented Design	   //
//  Author:        Rohit Sharma, Syracuse University			   //
//                 (315) 935-1323, rshar102@syr.edu		           //
/////////////////////////////////////////////////////////////////////
/*
* Required Files:
*   MsgServer.cpp
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp

Module Operations:
==================
This class defines startServerReceiverThread and startServerSenderThread and void startServer(); to start client threads.

Public Interface:
=================
void startServerReceiverThread();
void startServerSenderThread();
void startServer();

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


#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Cpp11-BlockingQueue.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Receiver/ClientHandler.h"
#include "MsgServer.h"
#include "../Sender/Sender.h"
#include <string>
#include <iostream>


//----< test stub >--------------------------------------------------

std::thread MsgServer::startServerReceiverThread() {
	Receiver serverReceiver1;
	std::thread t1(
		[&]() {serverReceiver1.executeReceiver("SERVER",8080); }
	);
	return t1;
}

std::thread MsgServer::startServerSenderThread()
{
	Sender serverSender1;
	std::thread t2(
		[&]() { serverSender1.execute(100, 2,"SERVER",8081,"localhost:8080"); } // 2 messages 100 millisec apart
	);
	return t2;
}

void MsgServer::startServer() {
	::SetConsoleTitle(L"HttpMessage Server - Runs Forever");

	Show::attach(&std::cout);
	Show::start();
	Show::title("\n  HttpMessage Server started");
	MsgServer server1;
	std::thread t2;
	std::thread t1 = server1.startServerReceiverThread();
	t2 = server1.startServerSenderThread();
	t1.join();
	t2.join();
}

#ifdef MSG_SERVER
int main(int argc, char* argv[])
{
	MsgServer server;
	server.startServer();
}
#endif