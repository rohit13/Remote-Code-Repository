#pragma once
/////////////////////////////////////////////////////////////////////
//  Sender.h - Sender class for both Client and Server//
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
makeMessage(size_t n, const std::string& body, EndPoint ep, std::string fromAdd)
sendMessage(HttpMessage& msg, Socket& socket)
getFiles(std::string filename, Socket & socket, const size_t toPort, std::string fromAdd)

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
#include "../Logger/Logger.h"
#include "../Utilities/Utilities.h"
#include "../XmlParser/XmlParser.h"
#include "../XmlParser/xmldocument.h"
#include "../XmlParser/XmlElement.h"
#include <string>
#include <iostream>
#include <thread>

using namespace XmlProcessing;
using namespace Utilities;
using sPtr = std::shared_ptr < AbstractXmlElement >;
using Show = StaticLogger<1>;
/////////////////////////////////////////////////////////////////////
// ClientCounter creates a sequential number for each client
//
class ClientCounter
{
public:
	ClientCounter() { ++clientCount; }
	size_t count() { return clientCount; }
private:
	static size_t clientCount;
};

size_t ClientCounter::clientCount = 0;

class Sender {

	using EndPoint = std::string;
public:
	HttpMessage Sender::makeMessage(size_t n, const std::string& body, EndPoint ep, std::string fromAdd)
	{
		HttpMessage msg;
		HttpMessage::Attribute attrib;
		EndPoint myEndPoint = fromAdd; 
		switch (n)
		{
		case 1:
			msg.clear();
			msg.addAttribute(HttpMessage::attribute("POST", "Message"));
			msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
			msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
			msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

			msg.addBody(body);
			if (body.size() > 0)
			{
				attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
				msg.addAttribute(attrib);
			}
			break;
		case 2:
			msg.clear();
			msg.addAttribute(HttpMessage::attribute("GET", "Message"));
			msg.addAttribute(HttpMessage::Attribute("mode", "oneway"));
			msg.addAttribute(HttpMessage::parseAttribute("toAddr:" + ep));
			msg.addAttribute(HttpMessage::parseAttribute("fromAddr:" + myEndPoint));

			msg.addBody(body);
			if (body.size() > 0)
			{
				attrib = HttpMessage::attribute("content-length", Converter<size_t>::toString(body.size()));
				msg.addAttribute(attrib);
			}
			break;
		default:
			msg.clear();
			msg.addAttribute(HttpMessage::attribute("Error", "unknown message type"));
		}
		return msg;
	}
	//----< send message using socket >----------------------------------

	void Sender::sendMessage(HttpMessage& msg, Socket& socket)
	{
		std::string msgString = msg.toString();
		socket.send(msgString.size(), (Socket::byte*)msgString.c_str());
	}
	//----< send file using socket >-------------------------------------
	/*
	* - Sends a message to tell receiver a file is coming.
	* - Then sends a stream of bytes until the entire file
	*   has been sent.
	* - Sends in binary mode which works for either text or binary.
	*/
	void Sender::getFiles(std::string filename, Socket & socket, const size_t toPort, std::string fromAdd)
	{
		HttpMessage msg = makeMessage(2, "", "localhost::" + std::to_string(toPort), fromAdd);
		msg.addAttribute(HttpMessage::Attribute("file", filename));
		sendMessage(msg, socket);
	}


	bool Sender::sendFile(const std::string& filename, Socket& socket, const size_t toPort, std::string fromAdd)
	{
		std::string fqname = "../TestFiles/" + filename;
		if (std::to_string(toPort) == "8081") {
			fqname = filename;
		}
		FileSystem::FileInfo fi(fqname);
		size_t fileSize = fi.size();
		std::string sizeString = Converter<size_t>::toString(fileSize);
		FileSystem::File file(fqname);
		file.open(FileSystem::File::in, FileSystem::File::binary);
		if (!file.isGood())
			return false;
		HttpMessage msg = makeMessage(1, "", "localhost::" + std::to_string(toPort), fromAdd);
		msg.addAttribute(HttpMessage::Attribute("file", filename));
		msg.addAttribute(HttpMessage::Attribute("content-length", sizeString));
		sendMessage(msg, socket);
		const size_t BlockSize = 2048;
		Socket::byte buffer[BlockSize];
		while (true)
		{
			FileSystem::Block blk = file.getBlock(BlockSize);
			if (blk.size() == 0)
				break;
			for (size_t i = 0; i < blk.size(); ++i)
				buffer[i] = blk[i];
			socket.send(blk.size(), buffer);
			if (!file.isGood())
				break;
		}
		file.close();
		return true;
	}
	//----< this defines the behavior of the client >--------------------
	std::string Sender::getDepsFlag(HttpMessage msg) {
		std::string deps;
		if (msg.bodyString().size() > 200) {
			XmlParser parser(msg.bodyString(), "str");
			XmlDocument* pDoc = parser.buildDocument();
			if (pDoc != nullptr && pDoc->xmlRoot() != nullptr && pDoc->xmlRoot()->value() == "input") {
				//std::vector<sPtr> operation = pDoc->element("input").descendents().element("operation").descendents().select();
				std::vector<sPtr> elem = pDoc->element("input").descendents().element("depsflag").descendents().select();
				if (elem.size() > 0)
				{
					for (auto ele : elem)
					{
						if (ele->value() == "Yes")
						{
							deps = "Yes";
							std::cout << "\n DepsFlag" << deps;
						}
						else {
							deps = "No";
							std::cout << "\n DepsFlag" << deps;
						}
					}
				}
			}			
		}
		return deps;
	}

	void Sender::sendServerFiles(HttpMessage msg)
	{
		SocketSystem ss;
		SocketConnecter si;
		while (!si.connect("localhost", 8081))
		{
			::Sleep(100);
		}
		Sender send;
		std::string fileToSend = msg.findValue("file");
		std::string DepsFlag = "Yes";
		DepsFlag = getDepsFlag(msg);
		std::cout << "\n DepsFlag Final" << DepsFlag;
		std::vector<std::string> dirs = FileSystem::Directory::getDirectories("../ServerRepository", fileToSend + "*");
		std::string path;
		if (dirs.size() == 0)
			Show::write("\nNo such file present in the repository => " + fileToSend + "\n");
		if (dirs.size() == 1)
		{
			std::vector<std::string> files = FileSystem::Directory::getFiles("../ServerRepository/" + dirs[0], fileToSend + "*");
			send.sendFile("../ServerRepository/" + dirs[0] + "/" + files[0], si,8081, "localhost:8080");
			path = "../ServerRepository/" + dirs[0] + "/";
			if (DepsFlag == "Yes")
				sendFileDependencies(msg, path, si);
		}
		if (dirs.size() > 1)
		{
			std::string latest_dir = getLatestDir(dirs, fileToSend);
			std::vector<std::string> files = FileSystem::Directory::getFiles("../ServerRepository/" + latest_dir, fileToSend + "*");
			send.sendFile("../ServerRepository/" + latest_dir + "/" + files[0], si,8081, "localhost:8080");
			path = "../ServerRepository/" + latest_dir + "/";
			if (DepsFlag == "Yes")
				sendFileDependencies(msg, path, si);
		}
		std::vector<std::string> files = FileSystem::Directory::getFiles("../ServerRepository", fileToSend);
		if (files.size() < 1) {
		}
		// shut down server's client handler
		msg = makeMessage(1, "quit", "toAddr:localhost:8081","localhost:8080");
		send.sendMessage(msg, si);

	}
	void Sender::sendFileDependencies(HttpMessage msg, std::string path, Socket& socket)
	{
		Sender send;
		std::vector<std::string> depFile = FileSystem::Directory::getFiles(path, "*.xml");

		std::string src =path;
		if (depFile.size() > 0) {
			src = path + depFile[0];
		}
		XmlParser parser(src);
		XmlDocument* pDoc = parser.buildDocument();
		std::vector<std::string> depFileNames;
		std::vector<sPtr> found = pDoc->element("dependencies").descendents().element("dependency").descendents().select();
		if (found.size() > 0)
		{
			for (auto pElem : found)
			{
					depFileNames.push_back(pElem->value());
			}
		}
		for (std::string f : depFileNames) {
			std::string fileToSend = Utilities::StringHelper::trim(f);
			std::vector<std::string> dirs = FileSystem::Directory::getDirectories("../ServerRepository/", fileToSend + "*");
			if (dirs.size() == 0)
				Show::write("No such file present in the repository => " + fileToSend + "\n");
			if (dirs.size() > 0)
			{
				std::string latest_dir = getLatestDir(dirs, fileToSend);
				std::vector<std::string> files = FileSystem::Directory::getFiles("../ServerRepository/" + latest_dir, fileToSend + "*");
				send.sendFile("../ServerRepository/" + latest_dir + "/" + files[0], socket,8081, "localhost:8080");
			}
		}
	}

	std::string Sender::getLatestDir(std::vector<std::string> dirs, std::string fileToSend)
	{
		return dirs[dirs.size() - 1];
	}

	std::vector<std::string> populateXmlMessages(std::vector<std::string> xmlfiles) {
		//populate xml messages
		std::vector<std::string> xmlMessages;
		for (std::string xml : xmlfiles) {
			std::string src = "../TestFiles/" + xml;
			XmlParser parser(src);
			XmlDocument* pDoc = parser.buildDocument();
			std::string xmlDoc = pDoc->toString();
			parser.compress(xmlDoc);
			xmlMessages.push_back(xmlDoc);
		}
		return xmlMessages;
	}

	std::vector<std::string> populateFilesToSend(std::vector<std::string> xmlfiles) {
		std::vector<std::string> filesToSend;
		for (std::string xml : xmlfiles) {
			std::string src = "../TestFiles/" + xml;
			XmlParser parser(src);
			XmlDocument* pDoc = parser.buildDocument();
			std::vector<sPtr> found = pDoc->element("input").descendents().element("Operation").descendents().select();
			std::vector<sPtr> elem = pDoc->element("input").descendents().element("file").descendents().select();
			if (found.size() > 0)
			{
				for (auto pElem : found)
				{
					if (pElem->value() == "Check-In")
					{
						for (auto ele : elem) {
							filesToSend.push_back(ele->value());
						}
					}
				}
			}
		}
		return filesToSend;
	}

	std::vector<std::string> populateFilesToGet(std::vector<std::string> xmlfiles) {
		std::vector<std::string> filesToGet;
		for (std::string xml : xmlfiles) {
			std::string src = "../TestFiles/" + xml;
			XmlParser parser(src);
			XmlDocument* pDoc = parser.buildDocument();
			std::vector<sPtr> found = pDoc->element("input").descendents().element("Operation").descendents().select();
			std::vector<sPtr> elem = pDoc->element("input").descendents().element("file").descendents().select();
			if (found.size() > 0)
			{
				for (auto pElem : found)
				{
					if (pElem->value() == "Extraction")
					{
						for (auto ele : elem) {
							filesToGet.push_back(ele->value());
						}
					}
				}
			}
		}
		return filesToGet;
	}

	std::string showSenderCounter(const std::string component) {
		ClientCounter counter;
		size_t myCount = counter.count();
		std::string myCountString = Utilities::Converter<size_t>::toString(myCount);

		Show::attach(&std::cout);
		Show::start();

		Show::title(
			"Starting HttpMessage " + component + " SENDER: " + myCountString +
			" on thread " + Utilities::Converter<std::thread::id>::toString(std::this_thread::get_id())
			);
		return myCountString;
	}

	void Sender::execute(const size_t TimeBetweenMessages, const size_t NumMessages, const std::string component, const size_t toPort, std::string fromAdd)
	{
		if (component == "CLIENT") {
			std::string myCountString = showSenderCounter(component);
			try
			{
				SocketSystem ss;
				SocketConnecter si;
				while (!si.connect("localhost", toPort))
				{
					Show::write("\n"+ component +" SENDER waiting to connect");
					::Sleep(100);
				}
				std::vector<std::string> xmlfiles = FileSystem::Directory::getFiles("../TestFiles", "*.xml");//find all xml files
				std::vector<std::string> xmlMessages = populateXmlMessages(xmlfiles);
				std::vector<std::string> filesToSend = populateFilesToSend(xmlfiles);
				std::vector<std::string> filesToGet = populateFilesToGet(xmlfiles);

				for (size_t i = 0; i < filesToSend.size(); ++i)
				{
					Show::write("\n\n  sending file to send" + filesToSend[i]);
					sendFile(filesToSend[i], si, toPort, fromAdd);
				}
				HttpMessage msg;// send a set of messages
				for (size_t i = 0; i < xmlMessages.size(); ++i)
				{
					std::string msgBody = xmlMessages[i];
					msg = makeMessage(1, msgBody, "localhost::" + std::to_string(toPort), fromAdd);
					sendMessage(msg, si);
					Show::write("\n\n  " + component + " SENDER" + myCountString + " sent\n" + msg.toIndentedString());
					::Sleep(TimeBetweenMessages);
				}

				for (size_t i = 0; i < filesToGet.size(); ++i)
				{
					Show::write("\n\n  sending message to server to get file: " + filesToGet[i]);
					getFiles(filesToGet[i], si, toPort, fromAdd);
				}
				msg = makeMessage(1, "quit", "toAddr:localhost:" + std::to_string(toPort), fromAdd);// shut down server's client handler
				sendMessage(msg, si);
				Show::write("\n\n CLIENT SENDER" + myCountString + " sent\n" + msg.toIndentedString());
				Show::write("\n\n ClIENT SENDER FINISHED");
			}
			catch (std::exception& exc)
			{
				std::string exMsg = "\n  " + std::string(exc.what()) + "\n\n";
				Show::write(exMsg);
			}
		}
	}
};