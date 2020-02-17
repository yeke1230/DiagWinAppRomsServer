/***********************************************************************
* Filename: Server.cpp
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Realize the session for server.
* It will receive the message from CT and web.
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/

#include "Server.h"

//boost::asio::io_service::strand strand_ = ShareArea::GetStrand();

SessionRoom *gSessionRoom = SessionRoom::GetRoom();

void Session::Start(){
	// record ipaddress and port.
	clientip_ = socket_.remote_endpoint().address().to_string();
	clientPort_ = socket_.remote_endpoint().port();
	cout << "IpAddress : " << clientip_ << endl;
	cout << "Port : " << clientPort_ << endl;
	LoggerManager::getInstance()->Info(clientip_);
	LoggerManager::getInstance()->Info(std::to_string(clientPort_));
	// Join this to session room
	gSessionRoom->Join(shared_from_this());
	// start to read read_buffer_
	Read();

	// Start deadline timer and bind function CheckDeadLine
	input_deadline_.async_wait(
		ShareArea::GetInstance()->GetStrand().wrap(boost::bind(&Session::CheckDeadLine,
			shared_from_this(), &input_deadline_)));
}

void Session::Read() {
	//Socket timeout is read from config.xml.
	input_deadline_.expires_from_now(boost::posix_time::minutes(ShareArea::GetInstance()->GetConfig().s_timeout));

	auto self(shared_from_this());
	socket_.async_read_some(boost::asio::buffer(read_buffer_, BUFFLENGTH),
		ShareArea::GetInstance()->GetStrand().wrap([this, self](boost::system::error_code ec, std::size_t length)
	{
		if (!ec)
		{
			DoRead(ec, length);
		}
		else
		{
			Stop();
		}
	}));
}

void Session::DoRead(const boost::system::error_code& error, size_t bytes_transferred) {
	if (!error)
	{

		try
		{
			gb2312_read_buffer = boost::locale::conv::from_utf<char>(read_buffer_, "gb2312");
			//Check login Inforamation
			if (is_fisrt_message_)
			{
				// Logging the message to file, and print to the console.
				string	m_reader_ = read_buffer_;
				string	s_reader_ = read_buffer_;
				m_reader_ = "Logging message from client : " + gb2312_read_buffer;
				cout << m_reader_ << endl;
				m_reader_ = "Logging message from client : " + s_reader_;
				LoggerManager::getInstance()->Info(m_reader_);

				// Check the format of login message.
				if (!CheckRsc())
				{
					// Logging the message to file, and print to the console.
					string failed_info = "Login failed : Message format wrong!!";
					cout << redcolor << failed_info << endl;
					LoggerManager::getInstance()->Info(failed_info);
					std::strcpy(write_buffer_, "login failed");
					cout << redcolor << "Login failed!!" << endl;
					// end after write back
					Write();
					StopNoBack();
					return;
				}

				is_fisrt_message_ = false;

				base_ptr_->Initialize();
				base_ptr_->SetID(SN_);

				if (!base_ptr_->DealInfo(read_buffer_))
				{
					std::strcpy(write_buffer_, "login failed");
					cout << redcolor << "Login failed!!" << endl;
					// end after write back
					Write();
					StopNoBack();
					return;
				}
				else
				{
					//option_type_ = read;
					string m_info = base_ptr_->tag + " client: " + SN_ + " login successed";
					LoggerManager::getInstance()->Info(m_info);
					cout << greencolor << base_ptr_->tag << " client: " << SN_ << " login successed" << endl;
					std::strcpy(write_buffer_, "login successed");
					Write();
				}
			}
			else
			{
				if (!strcmp(read_buffer_, "close"))
				{
					StopNoBack();
					return;
				}
				// Logging the message to file, and print in the console.
				string	m_reader_ = read_buffer_;
				m_reader_ = "Receive data from " + base_ptr_->tag + " " + SN_ + " : " + gb2312_read_buffer;
				cout << greencolor << m_reader_ << endl;
				m_reader_ = "Receive data from " + base_ptr_->tag + " " + SN_ + " : " + read_buffer_;
				LoggerManager::getInstance()->Info(m_reader_);
				// Parse the read_buffer
				v_msg_.clear();
				v_msg_ = ShareArea::GetInstance()->Split(read_buffer_, "@FMI@");
				if (v_msg_.max_size() > 0)
					for (int i = 0; i < v_msg_.size(); i++) // 
						if (v_msg_[i] != "")
						{
							try
							{
								string mywebback;
								// Return codes :
								// 0		Login failed or message format wrong
								// 1		insert or update succeccfully
								// 2		Target client doesn't exist in session room.
								switch (base_ptr_->DealInfo(v_msg_[i]))
								{
								case 2:
									mywebback = "target doesn't exist";
									std::strcpy(write_buffer_, mywebback.c_str());
									Write();
									StopNoBack();
									break;
								default:
									break;
								}

							}
							catch (const sql::SQLException& e)
							{
								LoggerManager::getInstance()->Error(v_msg_[i]);
								throw e;
							}
							catch (const std::exception& e) {
								string myerr = e.what();
								myerr = "error : " + myerr + "!!!";
								LoggerManager::getInstance()->Error(myerr);
								cout << redcolor << myerr << endl;
							}
						}

				//flush read buffer
				memset(read_buffer_, 0, sizeof(read_buffer_) / sizeof(char));
			}
		}
		catch (const sql::SQLException& err)
		{								
			cout << "# ERR: SQLException in " << __FILE__ << endl;
			cout << redcolor << err.what() << endl;
			string myerr = err.what();
			myerr = SN_ + " Database error : " + myerr + "!!!  ";
			myerr += ", SQLState: " + err.getSQLState();
			LoggerManager::getInstance()->Error(myerr);
			cout << redcolor << myerr << endl;
		}catch (const boost::system::system_error& err)
		{
			cout << redcolor << err.what() << endl;
			LoggerManager::getInstance()->Error(err.what());
		}

		Read();
	}
}

void Session::CheckDeadLine(deadline_timer* deadline) {
	if (Stopped())
		return;

	// Check whether the deadline has passed. We compare the deadline against
	// the current time since a new asynchronous operation may have moved the
	// deadline before this actor had a chance to run.
	if (deadline->expires_at() <= deadline_timer::traits_type::now())
	{
		// The deadline has passed. Stop the session. The other actors will
		// terminate as soon as possible.
		Stop();
	}
	else
	{
		// Put the actor back to sleep.
		deadline->async_wait(
			ShareArea::GetInstance()->GetStrand().wrap(boost::bind(&Session::CheckDeadLine,
				shared_from_this(), deadline)));
	}
}

void Session::Write()
{
	try
	{
		gb2312_write_buffer = boost::locale::conv::from_utf<char>(write_buffer_, "gb2312");
		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer(write_buffer_, BUFFLENGTH),
			ShareArea::GetInstance()->GetStrand().wrap([this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (!ec)
			{
				string mymsg = "Write Message to " + base_ptr_->tag + " " + base_ptr_->uid + " : " + gb2312_write_buffer;
				cout << greencolor << mymsg << endl;
				mymsg = "Write Message to " + base_ptr_->tag + " " + base_ptr_->uid + " : " + write_buffer_;
				LoggerManager::getInstance()->Info(mymsg);
				//DoWrite(ec);
			}
			else
			{
				string mymsg = "Write Message " + gb2312_write_buffer + "failed";
				cout << redcolor << mymsg << endl;
				mymsg = write_buffer_;
				mymsg = "Write Message " + mymsg + "failed";
				LoggerManager::getInstance()->Info(mymsg);
			}

			memset(write_buffer_, 0, sizeof(write_buffer_) / sizeof(char));
		}));
	}
	catch (const boost::system::system_error& err)
	{
		cout << redcolor << err.what() << endl;
		LoggerManager::getInstance()->Error(err.what());
	}

}

void Session::Deliever(const string & msg)
{
	std::strcpy(write_buffer_, msg.c_str());
	Write();
}

string Session::GetID()
{
	return SN_;
}


void Session::Stop() {
	try
	{
		if (Stopped())
			return;

		auto self(shared_from_this());
		boost::asio::async_write(socket_, boost::asio::buffer("close", BUFFLENGTH),
			ShareArea::GetInstance()->GetStrand().wrap([this, self](boost::system::error_code ec, std::size_t /*length*/)
		{
			if (gSessionRoom->IsOnline(base_ptr_->uid))
			{
				gSessionRoom->Leave(shared_from_this());
				cout << base_ptr_->uid + " leave" << endl;
				LoggerManager::getInstance()->Info(base_ptr_->uid + " leave");
			}

			string info = "write message to " + base_ptr_->tag + " " + base_ptr_->uid + " : close ";
			cout << info << endl;
			LoggerManager::getInstance()->Info("write message to " + base_ptr_->tag + " " + base_ptr_->uid + " : close ");
			//socket_.shutdown(boost::asio::socket_base::shutdown_both);
			input_deadline_.cancel();
			socket_.close();
		}));
	}
	catch (const boost::system::system_error& err)
	{
		cout << redcolor << err.what() << endl;
		LoggerManager::getInstance()->Error(err.what());
	}


}

void Session::StopNoBack() {
	if (gSessionRoom->IsOnline(base_ptr_->uid))
	{
		gSessionRoom->Leave(shared_from_this());
		cout << base_ptr_->uid + " leave" << endl;
		LoggerManager::getInstance()->Info(base_ptr_->uid + " leave");
	}

	if (Stopped())
		return;
	//socket_.shutdown(boost::asio::socket_base::shutdown_both);
	input_deadline_.cancel();
	socket_.close();
}

bool Session::CheckRsc()
{
	try
	{
		vector<string> m_strs;
		boost::split(m_strs, read_buffer_, boost::is_any_of("|"));
		for (int i = 1; i < m_strs.size(); i++) // 
		{
			vector<string> sub_m_strs;
			boost::algorithm::split(sub_m_strs, m_strs[i], boost::is_any_of(";"));
			for (int j = 0; j < sub_m_strs.size() - 1; j++)
			{
				vector<string> sub_sub_m_strs;
				boost::algorithm::split(sub_sub_m_strs, sub_m_strs[j], boost::is_any_of("="));
				for (int k = 0; k < sub_sub_m_strs.size(); k++)
				{
					string str_cpy = boost::algorithm::to_lower_copy(sub_sub_m_strs[k]);
					// Convert character "<semicolon>"->";", "<equal>"->"=", "<bar>"->"|" in systemerror. 
					if (!strcmp(str_cpy.c_str(), "id"))
					{
						SN_ = sub_sub_m_strs[k + 1];
					}
				}
			}
		}

		if (m_strs[0] == "c_login" || m_strs[0] == "c_rd_login")
		{
			base_ptr_.reset(new CTMsgHelper());
			msg_resource_ = ct_message;
			return true;
		}

		if (m_strs[0] == "w_login")
		{
			base_ptr_.reset(new WebMsgHelper());
			msg_resource_ = web_message;
			return true;
		}

		return false;
	}
	catch (sql::SQLException &e) {
		cout << "# ERR: SQLException in " << __FILE__;
		cout << "(" << __FUNCTION__ << ") on line "	<< __LINE__ << endl;
		cout << "# ERR: " << e.what();
		cout << " (MySQL error code: " << e.getErrorCode();
		cout << ", SQLState: " << e.getSQLState() << " )" << endl;
		LoggerManager::getInstance()->Error(e.what());
		//return string();
		return false;
	}
}

