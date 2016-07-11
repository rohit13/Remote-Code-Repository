##########################
# ReadMe.txt
# Rohit Sharma - SUID -242093353
# Project3_DependencyAnalyzer
##########################

Instructions for running the project.

1. Use compile.bat to comiple the project.
2. Use run.bat to run the project.
	  
Notes :
-------------------------------------------------------------------------------
- 8080 is port on which server will run.
- 8081 is port on which client will run.
- Both Server and Client have two root folder which are "ServerRepository" and "ClientRepository".
- Files downloaded will be save on "ClientRepository" folder.
- There is 'TestFiles' folder on project level, it contains the input xml files I have used for demonstration.

Demonstration instructions:

To run Check-In (default setting)

1. Place these three files in TestFiles folder.(already present there)

simpleInputXml.xml
simpleInputXml1.xml
simpleInputXml2.xml

2. Run project. 
3. Files will be checked in in "ServerRepository" folder.

To run extraction:

1. Create Backup in non xml format for these three files(mentioned in step 1):
simpleInputXml.xml --> simpleInputXml.xml_bkp
simpleInputXml1.xml --> simpleInputXml1.xml_bkp
simpleInputXml2.xml --> simpleInputXml2.xml_bkp

2. rename checkout files to xml files as below, run project again. files will be extracted in  "ClientRepository" folder.
simpleInputXml.xml_checkout  --> simpleInputXml.xml
simpleInputXml1.xml_checkout --> simpleInputXml1.xml
simpleInputXml2.xml_checkout --> simpleInputXml2.xml

Note: to check if dependent files getting extracted, use only below mentioned two files

simpleInputXml.xml_checkout  --> simpleInputXml.xml
simpleInputXml1.xml_checkout --> simpleInputXml1.xml

Note: To check dependency please DO NOT use simpleInputXml2.xml_checkout --- as it will send the get message to server for depends on file Sockets.h as well.
Note: First run Check-In then only run extraction.