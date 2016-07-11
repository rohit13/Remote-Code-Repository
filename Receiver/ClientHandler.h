#pragma once
/////////////////////////////////////////////////////////////////////
//  ClientHandler.h - Receiver and ClientHandler class for both Client Server//
//  ver 1.0                                                        //
//  Lanaguage:     C++ Visual Studio 2015                          //
//  Platform:      Dell XPS L510X - Windows 10		               //
//	Application:   Project#4 - CSE 687-Object Oriented Design	   //
//  Author:        Rohit Sharma, Syracuse University			   //
//                 (315) 935-1323, rshar102@syr.edu		           //
/////////////////////////////////////////////////////////////////////
/*
* Required Files:
*   HttpMessage.h, HttpMessage.cpp
*   Cpp11-BlockingQueue.h
*   Sockets.h, Sockets.cpp
*   FileSystem.h, FileSystem.cpp
*   Logger.h, Logger.cpp
*   Utilities.h, Utilities.cpp

Public Interface:
=================
void executeReceiver(std::string component, size_t listenPort);
void serverReceiver(std::string component, size_t listenPort);
void clientReceiver(std::string component, size_t listenPort);

ClientHandler(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
void operator()(Socket socket);
void createMetaData(HttpMessage msg);
std::string returnDirPath(std::string onlyFileName);
HttpMessage readMessage(Socket& socket);
void readFile(std::string& filename, size_t fileSize, Socket& socket);

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
#include "../XmlParser/XmlParser.h"
#include "../XmlParser/xmldocument.h"
#include "../Sender/sender.h"
#include "../XmlParser/XmlElement.h"
#include <ctime>
#include <Windows.h>
#include <string>
#include <iostream>
#include <chrono>
#include <time.h>

using namespace XmlProcessing;
using sPtr = std::shared_ptr < AbstractXmlElement >;
using Show = StaticLogger<1>;
using namespace Utilities;

class Receiver {
public:
	void executeReceiver(std::string component, size_t listenPort);
	void serverReceiver(std::string component, size_t listenPort);
	void clientReceiver(std::string component, size_t listenPort);
};

class ClientHandler
{
public:
	ClientHandler(BlockingQueue<HttpMessage>& msgQ) : msgQ_(msgQ) {}
	void operator()(Socket socket);
	std::string component_;
	void createMetaData(HttpMessage msg);
	std::string returnDirPath(std::string onlyFileName);
	void checkForMetadata(HttpMessage msg);
private:
	bool connectionClosed_;
	HttpMessage readMessage(Socket& socket);
	void readFile(std::string& filename, size_t fileSize, Socket& socket);
	BlockingQueue<HttpMessage>& msgQ_;
};
//----< this defines processing to frame messages >------------------

HttpMessage ClientHandler::readMessage(Socket& socket)
{
	connectionClosed_ = false; HttpMessage msg;
	while (true) {// read message attributes
		std::string attribString = socket.recvString('\n');
		if (attribString.size() > 1){
			HttpMessage::Attribute attrib = HttpMessage::parseAttribute(attribString);
			msg.addAttribute(attrib);
		}else{break;}
	}
	if (msg.attributes().size() == 0) {// If client is done, connection breaks and recvString returns empty string
		connectionClosed_ = true;
		return msg;
	}
	if (msg.attributes()[0].first == "POST") {// read body if POST - all messages in this demo are POSTs
		std::string filename = msg.findValue("file");
		if (filename != "")	{
			size_t contentSize;	std::string sizeString = msg.findValue("content-length");
			if (sizeString != "")
				contentSize = Converter<size_t>::toValue(sizeString);
			else
				return msg;
			readFile(filename, contentSize, socket);
		}
		if (filename != ""){	// construct message body
			msg.removeAttribute("content-length");
			std::string bodyString = "<file>" + filename + "</file>";
			std::string sizeString = Converter<size_t>::toString(bodyString.size());
			msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
			msg.addBody(bodyString);
		}else{	// read message body
			size_t numBytes = 0;
			size_t pos = msg.findAttribute("content-length");
			if (pos < msg.attributes().size()){
				numBytes = Converter<size_t>::toValue(msg.attributes()[pos].second);
				Socket::byte* buffer = new Socket::byte[numBytes + 1];
				socket.recv(numBytes, buffer);
				buffer[numBytes] = '\0';
				std::string msgBody(buffer);
				msg.addBody(msgBody);
				delete[] buffer;
			}
			checkForMetadata(msg);
		}
	}
	//if (msg.attributes()[0].first == "GET"){/*std::string filename = msg.findValue("file"); if (filename != ""){}*/}
	return msg;
}

void ClientHandler::checkForMetadata(HttpMessage msg) {
	if (component_ == "SERVER") {
		createMetaData(msg);
	}
}

std::wstring s2ws(const std::string& s)
{
	int len;
	int slength = (int)s.length() + 1;
	len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
	wchar_t* buf = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
	std::wstring r(buf);
	delete[] buf;
	return r;
}


const std::string currentDateTime() {
		time_t now;
		char the_date[20];
		struct tm  tstruct;
		the_date[0] = '\0';
		now = time(NULL);
		localtime_s(&tstruct, &now);
		if (now != -1)
		{
			strftime(the_date, 20, "%m%d%Y%I%M%S", &tstruct);
		}
		return std::string(the_date);
}
//----< read a binary file from socket and save >--------------------
/*
* This function expects the sender to have already send a file message,
* and when this function is running, continuosly send bytes until
* fileSize bytes have been sent.
*/

std::string ClientHandler::returnDirPath(std::string onlyFileName) {
	std::string filePath;
	std::string fqname;
	if (component_ == "SERVER") {
		filePath = "../ServerRepository/" + onlyFileName + "_" + currentDateTime() + "/";
		std::wstring stemp = s2ws(filePath);
		LPCWSTR result = stemp.c_str();
		CreateDirectory(result, NULL);
		fqname = "../ServerRepository/" + onlyFileName + "_" + currentDateTime() + "/" + onlyFileName;
	}
	else {
		fqname = "../ClientRepository/" + onlyFileName;
	}
	return fqname;
}
void ClientHandler::readFile(std::string& filename, size_t fileSize, Socket& socket)
{
	std::string onlyFileName = FileSystem::Path::getName(filename, true);
	std::string fqname;
	
	fqname = returnDirPath(onlyFileName);

	FileSystem::File file(fqname);
	file.open(FileSystem::File::out, FileSystem::File::binary);
	if (!file.isGood())
	{
		/*
		* This error handling is incomplete.  The client will continue
		* to send bytes, but if the file can't be opened, then the server
		* doesn't gracefully collect and dump them as it should.  That's
		* an exercise left for students.
		*/
		Show::write("\n\n  can't open file " + fqname);
	}

	const size_t BlockSize = 2048;
	Socket::byte buffer[BlockSize];

	size_t bytesToRead;
	while (true)
	{
		if (fileSize > BlockSize)
			bytesToRead = BlockSize;
		else
			bytesToRead = fileSize;
		socket.recv(bytesToRead, buffer);
		FileSystem::Block blk;
		for (size_t i = 0; i < bytesToRead; ++i)
			blk.push_back(buffer[i]);
		file.putBlock(blk);
		if (fileSize < BlockSize)
			break;
		fileSize -= BlockSize;
	}
	file.close();
}
//----< receiver functionality is defined by this function >---------

void ClientHandler::operator()(Socket socket)
{
	/*
	* There is a potential race condition due to the use of connectionClosed_.
	* If two clients are sending files at the same time they may make changes
	* to this member in ways that are incompatible with one another.  This
	* race is relatively benign in that it simply causes the readMessage to
	* be called one extra time.
	*
	* The race is easy to fix by changing the socket listener to pass in a
	* copy of the clienthandler to the clienthandling thread it created.
	* I've briefly tested this and it seems to work.  However, I did not want
	* to change the socket classes this late in your project cycle so I didn't
	* attempt to fix this.
	*/
	while (true)
	{
		HttpMessage msg = readMessage(socket);
		if (connectionClosed_ || msg.bodyString() == "quit")
		{
			Show::write("\n\n  clienthandler thread is terminating");
			break;
		}
		msgQ_.enQ(msg);
	}
}

std::vector<std::string> getDirectories(std::vector<sPtr> filenames) {
	std::vector<std::string> dirs;
	for (sPtr file : filenames) {
		std::string str = file->value();
		str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
		size_t endpos = str.find_last_not_of(" \t");
		if (std::string::npos != endpos)
		{
			str = str.substr(0, endpos + 1);
		}
		dirs = FileSystem::Directory::getDirectories("../ServerRepository/", str + "*");
	}
	return dirs;
}

void populateMetadataFile(std::vector<std::string> dirs, XmlDocument* pDoc) {
	std::ofstream outputFile;
	if (dirs.size() > 0) {
		outputFile.open("../ServerRepository/" + dirs[dirs.size() - 1] + "/metadata.xml");
	}
	//add status tag to metadata
	sPtr root = makeTaggedElement("metadata");
	std::vector<sPtr> found = pDoc->element("input").descendents().element("status").descendents().select();
	for (sPtr file : found) {
		sPtr child = makeTaggedElement("status");
		child->addChild(makeTextElement(file->value()));
		root->addChild(child);
	}

	//add file tag to metadata
	std::vector<sPtr> found1 = pDoc->element("input").descendents().element("file").descendents().select();
	for (sPtr file : found1) {
		sPtr child = makeTaggedElement("file");
		child->addChild(makeTextElement(file->value()));
		root->addChild(child);
	}
	//add dependencies tag to metadata
	sPtr childP = makeTaggedElement("dependencies");
	std::vector<sPtr> found2 = pDoc->element("input").descendents().element("dependencies").descendents().element("dependency").descendents().select();
	for (sPtr file1 : found2) {
		sPtr child = makeTaggedElement("dependency");
		child->addChild(makeTextElement(file1->value()));
		childP->addChild(child);
	}
	root->addChild(childP);
	outputFile << root->toString();
	outputFile.close();
}

void ClientHandler::createMetaData(HttpMessage msg) {
	if (msg.bodyString().size() > 200) {
		XmlParser parser(msg.bodyString(), "str");
		XmlDocument* pDoc = parser.buildDocument();
		if (pDoc != nullptr && pDoc->xmlRoot() != nullptr && pDoc->xmlRoot()->value() == "input") {
			std::vector<sPtr> filenames = pDoc->element("input").descendents().element("file").descendents().select();
			std::vector<std::string> dirs = getDirectories(filenames);
			populateMetadataFile(dirs, pDoc);
		}
	}
}

void Receiver :: serverReceiver(std::string component, size_t listenPort) {
		Sender sender;
		BlockingQueue<HttpMessage> msgQ;
		try
		{
			SocketSystem ss;
			SocketListener sl(listenPort, Socket::IP6);
			ClientHandler cp(msgQ);
			cp.component_ = component;
			sl.start(cp);
			/*
			* Since this is a server the loop below never terminates.
			* We could easily change that by sending a distinguished
			* message for shutdown.
			*/
			while (true)
			{
				HttpMessage msg = msgQ.deQ();
				if (msg.attributes().size()>0 && msg.attributes()[0].first == "GET") {
					std::string filename = msg.findValue("file");
					Show::write("\n\n  Get Files Request :" + filename);
					sender.sendServerFiles(msg);
				}
				Show::write("\n\n  " + component + " RECEIVER received message content :\n" + msg.bodyString());
			}
		}
		catch (std::exception& exc)
		{
			Show::write("\n  Exeception caught: ");
			std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
			Show::write(exMsg);
		}
}

void Receiver::clientReceiver(std::string component, size_t listenPort) {
	BlockingQueue<HttpMessage> msgQ;
	try
	{
		SocketSystem ss;
		SocketListener sl(listenPort, Socket::IP6);
		ClientHandler cp(msgQ);
		cp.component_ = component;
		sl.start(cp);
		/*
		* Since this is a server the loop below never terminates.
		* We could easily change that by sending a distinguished
		* message for shutdown.
		*/
		while (true)
		{
			HttpMessage msg = msgQ.deQ();
			Show::write("\n\n  RECEIVER " + component + " recvd message contents:\n" + msg.bodyString());
		}
	}
	catch (std::exception& exc)
	{
		Show::write("\n  Exeception caught: ");
		std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
		Show::write(exMsg);
	}
}

void Receiver::executeReceiver(std::string component, size_t listenPort)
{
	if (component == "SERVER") {
		serverReceiver(component, listenPort);
	}

	if (component == "CLIENT") {
		clientReceiver(component, listenPort);
	}

}