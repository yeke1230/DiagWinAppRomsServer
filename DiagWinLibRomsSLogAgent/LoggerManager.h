/***********************************************************************
* Filename: LoggerManager.h
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Define the log function.
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/



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

	static LoggerManager *getInstance();

	LoggerManager() {
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
		//std::filesystem::path dir("serverlog");
		//if (boost::filesystem::create_directory(dir))
		//{
		//	std::cerr << "Directory Created: " << "serverlog" << std::endl;
		//}
	}

	/***********************************************************************
	* Module Name: GetCurrentTime
	*
	* Description: Get current time.
	***********************************************************************/
	const std::string GetCurrentTime() {
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
	void Info(string content) {
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
	void Error(string content) { 
		string m_file = "serverlog/" + GetFilename() + ".txt";
		fstream filestr;
		filestr.open(&m_file[0], fstream::out | fstream::app);
		filestr << GetCurrentTime() << " [Error] : " << content << std::endl;
		filestr.close();
	}

private:
	const std::string GetFilename() {
		char buff[21];
		time_t t = time(0);
		strftime(buff, DTTMSZ, DTTMFMT2, localtime(&t));
		return buff;
	}

	static LoggerManager *logger;
	char filename_[1024];
};
