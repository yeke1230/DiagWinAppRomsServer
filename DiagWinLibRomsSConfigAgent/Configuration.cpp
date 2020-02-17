/***********************************************************************
* Filename: Configuration.cpp
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Realize the configution operator.
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/


#include "Configuration.h"

void Config::load(const std::string & filename)
{
	// Create empty property tree object
	pt::ptree tree;

	// Parse the XML into the property tree.
	pt::read_xml(filename, tree);

	// Get atrribute.
	s_port = tree.get<string>("ROMS.Server.<xmlattr>.port");
	s_timeout = tree.get<int>("ROMS.Server.<xmlattr>.timeout");
	db_ip = tree.get<string>("ROMS.DataBase.<xmlattr>.ipaddress");
	db_name = tree.get<string>("ROMS.DataBase.<xmlattr>.name");
	db_port = tree.get<string>("ROMS.DataBase.<xmlattr>.port");
	db_user = tree.get<string>("ROMS.DataBase.<xmlattr>.user");
	db_pas = tree.get<string>("ROMS.DataBase.<xmlattr>.password");
	th_number = tree.get<string>("ROMS.ThreadPool.<xmlattr>.number");
	db_pool = tree.get<string>("ROMS.DataBasePool.<xmlattr>.number");
	f_root = tree.get<string>("ROMS.RootFilePath");
	s_http_url = tree.get<string>("ROMS.HttpURL");
	s_http_url_switch = tree.get<string>("ROMS.HttpURL.<xmlattr>.switch");
	db_switch = tree.get<string>("ROMS.DataBase.<xmlattr>.switch");
	s_state_timer = tree.get<int>("ROMS.StateTimer.<xmlattr>.time");
}
