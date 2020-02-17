/***********************************************************************
* Filename: WMsgModels.h
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Define the web message models.
*
* Copyright Information © FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/
#pragma once

#include <iostream>
#include <vector>
#include <stdlib.h>
#include <boost/algorithm/string.hpp>

using namespace std;

class WLogin {
public:
	void Set(vector<string> info) {
		for (int i = 0; i < info.size(); i += 2)
		{
			if (!strcmp(info[i].c_str(), "Id") && info[i + 1].c_str() != "")	user = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Psd") && info[i + 1].c_str() != "")	password = info[i + 1].c_str();
		}
	}

	string user;
	string password;
};

class Site
{
public:
	Site() {};
	~Site() {};

	void Set(vector<string> info) {
		for (int i = 0; i < info.size(); i += 2)
		{
			if (!strcmp(info[i].c_str(), "Target"))		target_ = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Command"))	command_ = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Parameters"))	parameters_ = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Tag"))		tag_ = info[i + 1].c_str();
			if (!strcmp(info[i].c_str(), "Source"))		source_ = info[i + 1].c_str();
		}
	}

	string target_;
	string command_;
	string parameters_;
	string source_;
	string tag_;

private:

};


typedef std::shared_ptr<WLogin>	WLoginPtr;
typedef std::shared_ptr<Site>	SitePtr;
