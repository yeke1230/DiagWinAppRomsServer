/***********************************************************************
* Filename: Main.cpp
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Defines the entry point for the console application.
* 
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information: 
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/


#include "Server.h"
#include "..\DiagWinLibRomsSConfigAgent\Configuration.h"
#include "..\DiagWinLibRomsSShareAgent\ShareArea.h"
#include "..\DiagWinLibRomsSDBAgent\DBConnectPool.h"
#include "..\DiagWinLibRomsSMsgAgent\CTMsgHelper.h"
#include "TheadPool.h"


/***********************************************************************
* Module Name: OverCheckTimer
*
* Description: Check the machine state every X minutes(get from config.xml)
*			   the 'StateTime' word. And if currentime - last communication
*			   time > 5. it will set state to 'over', else 'unkonw'
***********************************************************************/
void OverCheckTimer(const boost::system::error_code& e, boost::asio::deadline_timer* timer, const Config& config) {
	if (!e)
	{
		string address = "tcp://" + config.db_ip + ":" + config.db_port;
		sql::Driver *driver = get_driver_instance();
		sql::Connection *con = driver->connect(address.c_str(), config.db_user.c_str(), config.db_pas.c_str());
		con->setSchema(config.db_name.c_str());
		string s_pstmt = "Update c_machine \
						set LAST_KNOWN_STATE = CASE \
							when TIMESTAMPDIFF(MINUTE,  LAST_COMMUNICATION_EPOCH,  NOW()) > 5 AND LAST_COMMUNICATION_EPOCH <> '0001-01-01 00:00:00' Then 'Over' \
							WHEN LAST_COMMUNICATION_EPOCH = '0001-01-01 00:00:00' THEN 'Unknown' \
							WHEN LAST_COMMUNICATION_EPOCH IS NULL THEN 'Unknown' \
							ElSE LAST_KNOWN_STATE\
						END, \
						LAST_KNOWN_PROBLEM = CASE \
							WHEN LAST_KNOWN_STATE = 'Over' then NULL \
							ElSE LAST_KNOWN_PROBLEM\
						end, \
						CURRENT_PROBLEM_OCCURE_EPOCH = CASE \
							WHEN LAST_KNOWN_PROBLEM IS NULL THEN NULL\
							ElSE CURRENT_PROBLEM_OCCURE_EPOCH\
						END";
		PreparedStatementPtr pstmt = PreparedStatementPtr(con->prepareStatement(s_pstmt.c_str()));

		pstmt->executeUpdate();
		pstmt->close();

		s_pstmt = "Update c_rd_machine \
						set LAST_KNOWN_STATE = CASE \
							when TIMESTAMPDIFF(MINUTE,  LAST_COMMUNICATION_EPOCH,  NOW()) > 5 AND LAST_COMMUNICATION_EPOCH <> '0001-01-01 00:00:00' Then 'Over' \
							WHEN LAST_COMMUNICATION_EPOCH = '0001-01-01 00:00:00' THEN 'Unknown' \
							WHEN LAST_COMMUNICATION_EPOCH IS NULL THEN 'Unknown' \
							ElSE LAST_KNOWN_STATE\
						END, \
						LAST_KNOWN_PROBLEM = CASE \
							WHEN LAST_KNOWN_STATE = 'Over' then NULL \
							ElSE LAST_KNOWN_PROBLEM\
						end, \
						CURRENT_PROBLEM_OCCURE_EPOCH = CASE \
							WHEN LAST_KNOWN_PROBLEM IS NULL THEN NULL\
							ElSE CURRENT_PROBLEM_OCCURE_EPOCH\
						END";

		pstmt.reset(con->prepareStatement(s_pstmt.c_str()));

		pstmt->executeUpdate();
		pstmt->close();

		delete con;
		LoggerManager::getInstance()->Info("Timer start to examine.");
		timer->expires_from_now(boost::posix_time::minutes(config.s_state_timer));
		timer->async_wait(boost::bind(OverCheckTimer, boost::asio::placeholders::error, timer, config));
	}
	else
	{
		string error = boost::system::system_error(e).what();
		LoggerManager::getInstance()->Error(error);
	}

};

/***********************************************************************
* Module Name: main
*
* Description: The server entry. The ROMS message server is built under
*			   the frame work boost.asio, a cross-platform framework.
*			   If you want compile the server, you need to install boost
*			   both in windows and linux. Furthor more,  In linux, you
*			   need to install gcc version >4.8 and g++ version >5.0.
***********************************************************************/
int main()
{
	try
	{
		std::cout << "******************************Info*************************************" << endl;
		Config config;
		config.load("config.xml");
		ShareArea::GetInstance()->SetConfig(config);

		// Logging server thread pool size.
		ServerThreadPool pool(stoi(config.th_number));
		string sthpool = "Server Thread Number : " + config.th_number;
		std::cout << left << setw(40) << "Server Thread Number "  << ":" << config.th_number << endl;
		LoggerManager::getInstance()->Info(sthpool);
		ShareArea::GetInstance()->SetStrand(pool.GetIOService());

		try
		{
			// Logging database connection pool size.
			string address = "tcp://" + config.db_ip + ":" + config.db_port;
			ConnectionPool::GetInstance()->InitPool(config.db_user, config.db_pas, address, config.db_name, stoi(config.db_pool));
			string dbpool = "Database Connection Pool Number : " + config.db_pool;
			std::cout << setw(40) << left << "Database Connection Pool Number " << ":" << config.db_pool << endl;
			LoggerManager::getInstance()->Info(dbpool);
		}
		catch (const sql::SQLException& err)
		{
			LoggerManager::getInstance()->Error(err.what());
			cout << "Exception: " << err.what() << "\n";
		}


		// Logging message timeout.
		string timeout = "Server message timeout : " + to_string(config.s_timeout);
		std::cout << setw(40) << left << "Server message timeout " << ":" << config.s_timeout << "m" << endl;
		LoggerManager::getInstance()->Info(timeout);
		
		// Web delievered address.
		string httpurl = "Web delievered address : " + config.s_http_url;
		std::cout << setw(40) << left << "Web delievered address " << ":" << config.s_http_url << endl;
		std::cout << setw(40) << left << "Web delievered status " << ":" << config.s_http_url_switch << endl;
		LoggerManager::getInstance()->Info(httpurl);

		// Start a over state check timer
		int state_timer = ShareArea::GetInstance()->GetConfig().s_state_timer;
		string state_timeout = "Timer cyclling time : " + to_string(config.s_timeout);
		std::cout << setw(40) << left << "Timer cyclling time " << ":" << state_timer << "m" << endl;
		LoggerManager::getInstance()->Info(state_timeout);
		boost::asio::deadline_timer timer(pool.GetIOService(), boost::posix_time::minutes(ShareArea::GetInstance()->GetConfig().s_state_timer));
		timer.async_wait(boost::bind(OverCheckTimer, boost::asio::placeholders::error, &timer, config));

		// Root File Path
		string f_root = ShareArea::GetInstance()->GetConfig().f_root;
		string out_f_root = "Root File Path : " + f_root;
		std::cout << setw(40) << left << "Root File Path " << ":" << f_root << endl;
		LoggerManager::getInstance()->Info(out_f_root);

		cout << "******************************Info*************************************" << endl;

		// Logging server start up.
		string start = "Server Start up...";
		Server s(pool.GetIOService(), stoi(config.s_port));
		cout << start << endl;
		LoggerManager::getInstance()->Info(start);

		pool.stop();


	}
	catch (std::exception& e)
	{
		LoggerManager::getInstance()->Error(e.what());
		cout << "Exception: " << e.what() << "\n";
	}
	catch (const sql::SQLException& err)
	{
		LoggerManager::getInstance()->Error(err.what());
		cout << "Exception: " << err.what() << "\n";
	}
	return 0;
}

