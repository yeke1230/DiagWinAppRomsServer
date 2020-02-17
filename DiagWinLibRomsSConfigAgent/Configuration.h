/***********************************************************************
* Filename: Configuration.h
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Define the configution operator.
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/

#pragma once
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/foreach.hpp>
#include <string>
#include <set>
#include <exception>
#include <iostream>

namespace pt = boost::property_tree;
using namespace std;

class Config
{
public:
	string	s_port;			// Server Port
	int		s_state_timer;	// Timer
	int		s_timeout;		// Client Timeout
	string  s_http_url;
	string  s_http_url_switch;

	string	db_switch;
	string	db_name;		// Database name
	string	db_ip;			// Database IP
	string	db_user;		// Database User
	string	db_pas;			// Database Password
	string	db_port;		// Database Port
	string	th_number;		// Threadpool Number
	string	db_pool;		// Database Pool Connection Number
	string	f_root;			// Root File Path
	Config() {}

	void load(const std::string &filename);

private:

};

