/***********************************************************************
* Filename: LoggerManager.cpp
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


#include "LoggerManager.h"

//char LoggerManager::filename_[1024] = { '\n' };

LoggerManager* LoggerManager::logger = nullptr;

LoggerManager * LoggerManager::getInstance()
{
	if (logger == nullptr)
	{
		logger = new LoggerManager();
	}
	return logger;
}

//void LoggerManager::SetLog(char *filename) {
//	memcpy(filename_, filename, strlen(filename));
//}