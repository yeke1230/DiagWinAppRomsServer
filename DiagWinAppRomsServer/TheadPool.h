/***********************************************************************
* Filename: ThreadPool.h
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Defines the Server thread pool.
* Thread pool number is read from config.xml
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/


#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <thread>

using namespace std;

using namespace boost::asio;
using namespace ip;

#pragma once

class ServerThreadPool
{
public:
	ServerThreadPool(int threadNum = std::thread::hardware_concurrency())
		: work_(new boost::asio::io_service::work(service_))
	{
		for (int i = 0; i < threadNum; ++i)
		{
			// Run mutiple run() function means you use a multi thread server in cpu.
			threads_.emplace_back([this]() { service_.run(); });
		}
	}
	// Forbidden copy object like B a(b);
	ServerThreadPool(const ServerThreadPool &) = delete;

	// Forbidden copy object by using "=";
	ServerThreadPool &operator=(const ServerThreadPool &) = delete;

	boost::asio::io_service &GetIOService()
	{
		return service_;
	}
	void stop()
	{
		work_.reset();
		for (auto &t : threads_)
		{
			t.join();
		}
	}
private:
	boost::asio::io_service service_;
	std::unique_ptr<boost::asio::io_service::work> work_;
	std::vector<std::thread> threads_;
};
