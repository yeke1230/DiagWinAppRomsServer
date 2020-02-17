#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <time.h>
#include <stdio.h>
#include <io.h>
#include <direct.h>
//#include <boost\filesystem.hpp>
using namespace std;

#define DTTMFMT "%Y-%m-%d %H:%M:%S"
#define DTTMFMT2 "%Y-%m-%d"
#define DTTMSZ 21

class LoggerManager
{

public:

	/***********************************************************************
	* Module Name: GetCurrentTime
	*
	* Description: Get current time.
	***********************************************************************/
	const static std::string GetCurrentTime() {
		char buff[21];
		time_t t = time(0);
		strftime(buff, DTTMSZ, DTTMFMT, localtime(&t));
		return buff;
	}

	/***********************************************************************
	* Module Name: Info
	*
	* Description: Log normal information.
	*
	* Calling Arguments:
	* Name		Input/Output		Type		Description
	* content	content				string		info content
	***********************************************************************/
	void static Info(string content) {
		string m_file = "serverlog/" + GetFilename() + ".txt";
		fstream filestr;
		filestr.open(&m_file[0], fstream::out | fstream::app);
		filestr << GetCurrentTime() << " [Info] : " << content << std::endl;
		filestr.close();

	}

	/***********************************************************************
	* Module Name: Error
	*
	* Description: Log error information.
	*
	* Calling Arguments:
	* Name		Input/Output		Type		Description
	* content	content				string		info content
	***********************************************************************/
	void static Error(string content) {
		string m_file = "serverlog/" + GetFilename() + ".txt";
		fstream filestr;
		filestr.open(&m_file[0], fstream::out | fstream::app);
		filestr << GetCurrentTime() << " [Error] : " << content << std::endl;
		filestr.close();
	}


	const static void CreateDir() {
		if (_mkdir("serverlog") == -1)
		{
			if (errno == EEXIST) {
				// alredy exists
			}
			else {
				// something else
				std::cerr << "Directory Created: " << "serverlog" << std::endl;
			}
		}
	}
private:
	const static std::string GetFilename() {
		char buff[21];
		time_t t = time(0);
		strftime(buff, DTTMSZ, DTTMFMT2, localtime(&t));
		return buff;
	}

	char static filename_[1024];


	LoggerManager() {
	}
};