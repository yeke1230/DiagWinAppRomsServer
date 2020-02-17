/***********************************************************************
* Filename: Server.h
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Defines the session for server.
* It will receive the message from CT and website.
*
* Copyright Information © FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/


#pragma once
#include <iostream>
#include <fstream>
#include <time.h>
#include <thread>
#include <boost/asio.hpp>
#include <boost/thread.hpp>
#include <boost/regex.hpp>
#include <boost/algorithm/string.hpp>
#include "..\DiagWinLibRomsSDBAgent\DBhelper.h"
#include "..\DiagWinLibRomsSConfigAgent\Configuration.h"
#include "..\DiagWinLibRomsSShareAgent\ShareArea.h"
#include "..\DiagWinLibRomsSMsgAgent\CTMsgHelper.h"
#include "..\DiagWinLibRomsSMsgAgent\WebMsgHelper.h"
#include "..\DiagWinLibRomsSMsgAgent\MsgHelperBase.h"
#include "..\DiagWinLibRomsResource\WinFontColor.h"
#include <boost/locale.hpp>

using namespace std;
using namespace boost::asio;
using namespace ip;

typedef std::deque<string> MessageQueue;

#define BUFFLENGTH 5120

#define DEBUG

class Session :
	public Participant,
	public std::enable_shared_from_this<Session>
{
public:

	Session(tcp::socket socket, boost::asio::io_service& io_service/*, SessionRoom& room*/)
		: socket_(std::move(socket)), input_deadline_(io_service)/*, room_(room)*/
	{
		input_deadline_.expires_at(boost::posix_time::pos_infin);
	};

	~Session() {
		//room_.leave(shared_from_this());
		string s = "client: " + SN_ + " disconnected...";
		cout << s << endl;
		LoggerManager::getInstance()->Info(s);
	}

	/***********************************************************************
	* Module Name: Start
	*
	* Description: This function will start a session bussiness that will
	*			   receive message from ct machine. And start a deadline
	*			   timer to record the tcp status. if cant receive message
	*			   within "timeout"(read from xml), it will call 
	*			   CheckDeadline.
	***********************************************************************/
	void Start();


	/***********************************************************************
	* Module Name: Read
	*
	* Description: This function will start reading message from client and 
	*			   set the timeout of timer. and binding then call Doread.
	***********************************************************************/
	void Read();


	/***********************************************************************
	* Module Name: DoRead
	*
	* Description: This function will will deal the real reading bussiness
	*			   including Login Check and message resouce, etc..
	***********************************************************************/
	void DoRead(const boost::system::error_code& error, size_t bytes_transferred);


	/***********************************************************************
	* Module Name: Write
	*
	* Description: This function will write write_buffer_ to client
	***********************************************************************/
	void Write();

	/***********************************************************************
	* Module Name: Deliever
	*
	* Description: Send message to other client.
	***********************************************************************/
	void Deliever(const string& msg);

	string GetID();


private:

	string SN_;						// Client series number
	string gb2312_read_buffer;		// Convert read_buffer to gb2312
	string gb2312_write_buffer;		// Convert wirte_buffer to gb2312
	void Stop();					// Stop the client
	void StopNoBack();		// Stop the client and dont write back
	bool Stopped() const {  return !socket_.is_open(); }
	void CheckDeadLine(deadline_timer* deadline);
	bool CheckRsc();				// Login message format check

	//std::shared_ptr<boost::asio::io_service::strand> strand_;
	tcp::socket						socket_;
	deadline_timer					input_deadline_;

	string			clientip_;		// Client ip
	unsigned int	clientPort_;	// Client port

	bool can_run_sessions_		= false;
	bool can_upload_			= false;
	bool can_echo_				= false;
	bool is_fisrt_message_		= true;
	bool is_failed_happened_	= false;

	enum Option { login, upload, download, echo, read, close }	option_type_;
	enum MessageType { machine, rd_machine}						msg_type_;
	enum MessageResource {ct_message, web_message}				msg_resource_;

	char read_buffer_[BUFFLENGTH]	= { '\0' };
	char write_buffer_[BUFFLENGTH]	= { '\0' };

	vector<string>	login_info_;
	vector<string>	v_msg_;
	vector<string>	v_failed_;

	MsgHelperBasePtr base_ptr_= MsgHelperBasePtr(new CTMsgHelper());

};

#pragma endregion

class Server
{
public:
	Server(boost::asio::io_service& io_service, short port)
		: io_service_(io_service), acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
		socket_(io_service)
	{

		DoAccept();

	}

	void DoAccept()
	{
		acceptor_.async_accept(socket_,
			[this](boost::system::error_code ec)
		{
			if (!ec)
			{
				std::make_shared<Session>(std::move(socket_), io_service_)->Start();
			}

			DoAccept();
		});
	}


private:
	boost::asio::io_service&	io_service_;
	tcp::acceptor				acceptor_;
	tcp::socket					socket_;
	////SessionRoom room_;
};