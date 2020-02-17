/***********************************************************************
* Filename: ShareArea.h
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Define the ShareArea. You can put some public variable or public function in this area.
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/

#pragma once
#include "..\DiagWinLibRomsSConfigAgent\Configuration.h"
#include <boost/regex.hpp>
#include <boost/asio.hpp>

class HttpRequest {

public:
	HttpRequest(boost::asio::io_service& ioservice) :io_service(ioservice) {}

	int boost_http_sync_client(const std::string& server, const std::string& port, const std::string& path,
		std::string& out_response_status_line, std::string& out_response_header, std::string& out_response_data);

	int parse_url(const std::string& url, std::string& out_server, std::string& out_port, std::string& out_path);

	int get_url_response(const std::string& url, std::string& out_response_data);

	int parse_hq_sinajs_cn_and_get_last_price(const std::string& market_data, double& last_price);

	boost::asio::io_service& io_service;
};

class ShareArea
{
public:
	static ShareArea *GetInstance();

	/***********************************************************************
	* Module Name: SetConfig
	*
	* Description: Set a configuration operate.
	*
	* Calling Arguments:
	* Name		Input/Output		Type		Description
	* config	config				Config		config operator
	***********************************************************************/
	 void SetConfig(Config config);

	/***********************************************************************
	* Module Name: GetConfig
	*
	* Description: Get a configuration operator.
	***********************************************************************/
	Config GetConfig();

	/***********************************************************************
	* Module Name: Split
	*
	* Description: Split a string.
	*
	* Calling Arguments:
	* Name		Input/Output		Type		Description
	* src		src/src				string		original string
	* pattern	pattern				string		pattern string
	*
	* Returns
	* Type				Description
	* vector<string>	Split the string and put the result into vector
	***********************************************************************/
	std::vector<std::string> Split(std::string src, std::string pattern);

	int SendHttpRequest(const std::string & url, std::string & out_response_data) {
		return request_ptr_->get_url_response(url, out_response_data);
	};

	void SetStrand(boost::asio::io_service& io);

	boost::asio::io_service::strand GetStrand();

private:
	static ShareArea *shareArea;
	std::shared_ptr<HttpRequest> request_ptr_;
	Config config_;
	std::shared_ptr<boost::asio::io_service::strand> strand_;
};

