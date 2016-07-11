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
/*
 */
#include "../HttpMessage/HttpMessage.h"
#include "../Sockets/Sockets.h"
#include "../FileSystem/FileSystem.h"
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../Receiver/ClientHandler.h"
#include "../Sender/sender.h"
#include "../XmlParser/XmlParser.h"
#include "../XmlParser/xmldocument.h"
#include "MsgClient.h"
#include <string>
#include <iostream>
#include <thread>

using Show = StaticLogger<1>;
using namespace Utilities;
using Utils = StringHelper;
using namespace XmlProcessing;



//----< entry point - runs two clients each on its own thread >------

std::thread MsgClient::startClientSenderThread() {
	Sender clientSender1;
	std::thread t1(
		[&]() { clientSender1.execute(100, 2,"CLIENT",8080, "localhost:8081"); } // 20 messages 100 millisec apart
	);
	return t1;
}

std::thread MsgClient::startClientReceiverThread()
{
	Receiver clientReceiver1;
	std::thread t2(
		[&]() {clientReceiver1.executeReceiver("CLIENT",8081); }
	);
	return t2;
}

void MsgClient::startClient() {
	::SetConsoleTitle(L"Clients Running on Threads");
	Show::title("Demonstrating two HttpMessage Clients each running on a child thread");
	MsgClient client1;
	std::thread t2;
	std::thread t1 = client1.startClientSenderThread();
	t2 = client1.startClientReceiverThread();
	t1.join();
	t2.join();
}

#ifdef MSG_CLIENT
int main(int argc, char* argv[])
{
	MsgClient msg;
	msg.startClient();
}
#endif
