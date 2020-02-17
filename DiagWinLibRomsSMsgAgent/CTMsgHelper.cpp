/***********************************************************************
* Filename: CTHelper.h
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Defines the CT bussiness for server.
* It will deal the message from CT.
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/

#include "CTMsgHelper.h"

int CTMsgHelper::DealInfo(const string & content)
{
	try
	{
		return  DealOnlineCTInfo(content);
	}
	catch (const sql::SQLException& err)
	{
		throw err;
	}
}


int CTMsgHelper::DealOnlineCTInfo(const string& ctinfo)
{
	ct_info_.clear();
	vector<string> m_strs;
	boost::split(m_strs, ctinfo, boost::is_any_of("|"));
	if (m_strs.max_size() > 0)
	{
		string s_header = m_strs[0];
		string s_syserror;
		if (!strcmp(s_header.c_str(), "c_machine_telemetry"))				header_ = c_machine_telemetry;
		else if (!strcmp(s_header.c_str(), "c_machine_version"))			header_ = c_machine_version;
		else if (!strcmp(s_header.c_str(), "c_machine_configeditor"))		header_ = c_machine_configeditor;
		else if (!strcmp(s_header.c_str(), "c_machine_database"))			header_ = c_machine_database;
		else if (!strcmp(s_header.c_str(), "c_machine_guiconfig"))			header_ = c_machine_guiconfig;
		else if (!strcmp(s_header.c_str(), "c_machine_protocol_gui"))		header_ = c_machine_protocol_gui;
		else if (!strcmp(s_header.c_str(), "c_machine_protocol_service"))	header_ = c_machine_protocol_service;
		else if (!strcmp(s_header.c_str(), "c_machine_logger"))				header_ = c_machine_logger;
		else if (!strcmp(s_header.c_str(), "c_machine"))					header_ = c_machine;
		else if (!strcmp(s_header.c_str(), "c_machine_other_info"))			header_ = c_machine_other_info;
		else if (!strcmp(s_header.c_str(), "c_login"))						header_ = c_login;
		else if (!strcmp(s_header.c_str(), "c_rd_login"))					header_ = c_login;
		else if (!strcmp(s_header.c_str(), "c_machine_tube_history"))		header_ = c_machine_tube_history;
		else if (!strcmp(s_header.c_str(), "c_machine_rotate_history"))		header_ = c_machine_rotate_history;
		else if (!strcmp(s_header.c_str(), "c_rd_machine_version"))			header_ = c_machine_version;
		else if (!strcmp(s_header.c_str(), "c_rd_machine_other_info"))		header_ = c_machine_other_info;
		else if (!strcmp(s_header.c_str(), "c_rd_machine_database"))		header_ = c_machine_database;
		else if (!strcmp(s_header.c_str(), "c_rd_machine_guiconfig"))		header_ = c_machine_guiconfig;
		else if (!strcmp(s_header.c_str(), "c_rd_machine_protocol_gui"))	header_ = c_machine_protocol_gui;
		else if (!strcmp(s_header.c_str(), "c_rd_machine_protocol_service"))header_ = c_machine_protocol_service;
		else if (!strcmp(s_header.c_str(), "c_rd_machine_configeditor"))	header_ = c_machine_configeditor;
		else if (!strcmp(s_header.c_str(), "c_rd_machine_logger"))			header_ = c_machine_logger;
		else if (!strcmp(s_header.c_str(), "c_rd_machine"))					header_ = c_machine;
		else if (!strcmp(s_header.c_str(), "c_rd_machine_tube_history"))	header_ = c_machine_tube_history;
		else if (!strcmp(s_header.c_str(), "c_rd_machine_rotate_history"))	header_ = c_machine_rotate_history;
		else if (!strcmp(s_header.c_str(), "c_rd_machine_telemetry"))		header_ = c_machine_telemetry;
		else if (!strcmp(s_header.c_str(), "c_feedback"))					header_ = c_feedback;
		else return 0;
		// Parse the message after head the second string. 
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
					if (!strcmp(str_cpy.c_str(), "systemerror"))
					{
						boost::algorithm::replace_all(sub_sub_m_strs[k + 1], "<semicolon>", ";");
						boost::algorithm::replace_all(sub_sub_m_strs[k + 1], "<equal>", "=");
						boost::algorithm::replace_all(sub_sub_m_strs[k + 1], "<bar>", "|");
						s_syserror = sub_sub_m_strs[k + 1];
					}

					if (!strcmp(str_cpy.c_str(), "file_path"))
					{
						vector<string> strs;
						// Add the main root file path(read from config.xml) to  the file_path.
						sub_sub_m_strs[k + 1] = ShareArea::GetInstance()->GetConfig().f_root + sub_sub_m_strs[k + 1];
					}

					ct_info_.push_back(sub_sub_m_strs[k]);
				}
			}
		}
		;

		ct_info_.push_back("SN");
		ct_info_.push_back(SN_);
		ct_info_.push_back("Header");
		ct_info_.push_back(s_header);
		string my_feedback;
		// Select operate according table name. 
		try
		{
			switch (header_)
			{
			case c_feedback:
				if (SessionRoom::GetRoom()->IsOnline(ct_info_[1]))
					SessionRoom::GetRoom()->Deliever(ct_info_[1], ct_info_[3]);
				break;
			case c_login:
				i_Login_ptr.reset(new CLoginHelper(std::move(std::make_shared<CLogin>(CLogin(ct_info_)))));
				if (!i_Login_ptr->LoginCheck())
					return 0;
				break;
			case c_machine:
				ma_ptr_->Set(ct_info_);
				ma_ptr_->last_com_time = LoggerManager::getInstance()->GetCurrentTime();
			
				i_DbHelper_ptr.reset(new MachineHelper(ma_ptr_));
				i_DbHelper_ptr->Update();
				// Call web delivered interface.
				if (s_syserror != "" && ma_ptr_->header == "c_machine" &&  ShareArea::GetInstance()->GetConfig().s_http_url_switch == "on")
				{
					string url = ShareArea::GetInstance()->GetConfig().s_http_url + "sno=" + SN_ + "&" + "description=" + ma_ptr_->problem + 
						"&" + "reportTime=" + ma_ptr_->systemerror_occure_epoch;
					LoggerManager::getInstance()->Info("Call Interface : " + url);
					string response;
					int rv = ShareArea::GetInstance()->SendHttpRequest(url, response);
					if (rv == 0) response = "Call Interface Success";
					else		 response = "Call Interface failed";
					LoggerManager::getInstance()->Info(response);
					cout << response << endl;
				}
				break;
			case c_machine_telemetry:
				ma_config_ptr_->Set(ct_info_);
				i_DbHelper_ptr.reset(new MachineTelemetryHelper(std::move(std::make_shared<MachineConfiguration>(MachineConfiguration(ct_info_)))));
				i_DbHelper_ptr->Insert();
				my_feedback = "Feedback=insert table " + s_header +" successed";
				cout << my_feedback << endl;;
				LoggerManager::getInstance()->Info(my_feedback);
				if (ma_config_ptr_->identify_ != "")
				{
					if (SessionRoom::GetRoom()->IsOnline(ma_config_ptr_->identify_))
						SessionRoom::GetRoom()->Deliever(ma_config_ptr_->identify_, my_feedback);
					else
						LoggerManager::getInstance()->Info("target does't exist");
				}
				break;
			case c_machine_version:
				ma_config_ptr_->Set(ct_info_);
				i_DbHelper_ptr.reset(new MachineVersionHelper(std::move(std::make_shared<MachineConfiguration>(MachineConfiguration(ct_info_)))));
				i_DbHelper_ptr->Insert();
				my_feedback = "Feedback=insert table " + s_header + " successed";
				cout << my_feedback << endl;;
				LoggerManager::getInstance()->Info(my_feedback);
				if (ma_config_ptr_->identify_ != "")
				{
					if (SessionRoom::GetRoom()->IsOnline(ma_config_ptr_->identify_))
						SessionRoom::GetRoom()->Deliever(ma_config_ptr_->identify_, my_feedback);
					else
						LoggerManager::getInstance()->Info("target does't exist");
				}
				break;
			case c_machine_configeditor:
				ma_config_ptr_->Set(ct_info_);
				i_DbHelper_ptr.reset(new MachineConfigEditorHelper(std::move(std::make_shared<MachineConfiguration>(MachineConfiguration(ct_info_)))));
				i_DbHelper_ptr->Insert();
				my_feedback = "Feedback=insert table " + s_header + " successed";
				cout << my_feedback << endl;;
				LoggerManager::getInstance()->Info(my_feedback);
				if (ma_config_ptr_->identify_ != "")
				{
					if (SessionRoom::GetRoom()->IsOnline(ma_config_ptr_->identify_))
						SessionRoom::GetRoom()->Deliever(ma_config_ptr_->identify_, my_feedback);
					else
						LoggerManager::getInstance()->Info("target does't exist");
				}
				break;
			case c_machine_other_info:
				ma_config_ptr_->Set(ct_info_);
				i_DbHelper_ptr.reset(new MachineOtherInfoHelper(std::move(std::make_shared<MachineConfiguration>(MachineConfiguration(ct_info_)))));
				i_DbHelper_ptr->Insert();
				my_feedback = "Feedback=insert table " + s_header + " successed";
				cout << my_feedback << endl;;
				LoggerManager::getInstance()->Info(my_feedback);
				if (ma_config_ptr_->identify_ != "")
				{
					if (SessionRoom::GetRoom()->IsOnline(ma_config_ptr_->identify_))
						SessionRoom::GetRoom()->Deliever(ma_config_ptr_->identify_, my_feedback);
					else
						LoggerManager::getInstance()->Info("target does't exist");
				}
				break;
			case c_machine_database:
				ma_config_ptr_->Set(ct_info_);
				i_DbHelper_ptr.reset(new MachineDataBaseHelper(std::move(std::make_shared<MachineConfiguration>(MachineConfiguration(ct_info_)))));
				i_DbHelper_ptr->Insert();
				my_feedback = "Feedback=insert table " + s_header + " successed";
				cout << my_feedback << endl;;
				LoggerManager::getInstance()->Info(my_feedback);
				// If insert successfully, send feedback to monitor.
				if (ma_config_ptr_->identify_ != "")
				{
					if (SessionRoom::GetRoom()->IsOnline(ma_config_ptr_->identify_))
						SessionRoom::GetRoom()->Deliever(ma_config_ptr_->identify_, my_feedback);
					else
						LoggerManager::getInstance()->Info("target does't exist");
				}
				break;
			case c_machine_protocol_gui:
				ma_config_ptr_->Set(ct_info_);
				i_DbHelper_ptr.reset(new MachineProtocolHelper(std::move(std::make_shared<MachineConfiguration>(MachineConfiguration(ct_info_)))));
				i_DbHelper_ptr->Insert();
				my_feedback = "Feedback=insert table " + s_header + " successed";
				cout << my_feedback << endl;;
				LoggerManager::getInstance()->Info(my_feedback);
				// If insert successfully, send feedback to monitor.
				if (ma_config_ptr_->identify_ != "")
				{
					if (SessionRoom::GetRoom()->IsOnline(ma_config_ptr_->identify_))
						SessionRoom::GetRoom()->Deliever(ma_config_ptr_->identify_, my_feedback);
					else
						LoggerManager::getInstance()->Info("target does't exist");
				}
				break;
			case c_machine_protocol_service:
				ma_config_ptr_->Set(ct_info_);
				i_DbHelper_ptr.reset(new MachineProtocolHelper(std::move(std::make_shared<MachineConfiguration>(MachineConfiguration(ct_info_)))));
				i_DbHelper_ptr->Insert();
				my_feedback = "Feedback=insert table " + s_header + " successed";
				cout << my_feedback << endl;;
				LoggerManager::getInstance()->Info(my_feedback);
				// If insert successfully, send feedback to monitor.
				if (ma_config_ptr_->identify_ != "")
				{
					if (SessionRoom::GetRoom()->IsOnline(ma_config_ptr_->identify_))
						SessionRoom::GetRoom()->Deliever(ma_config_ptr_->identify_, my_feedback);
					else
						LoggerManager::getInstance()->Info("target does't exist");
				}
				break;
			case c_machine_guiconfig:
				ma_config_ptr_->Set(ct_info_);
				i_DbHelper_ptr.reset(new MachineGUIconfigHelper(std::move(std::make_shared<MachineConfiguration>(MachineConfiguration(ct_info_)))));
				i_DbHelper_ptr->Insert();
				my_feedback = "Feedback=insert table " + s_header + " successed";
				cout << my_feedback << endl;;
				LoggerManager::getInstance()->Info(my_feedback);
				// If insert successfully, send feedback to monitor.
				if (ma_config_ptr_->identify_ != "")
				{
					if (SessionRoom::GetRoom()->IsOnline(ma_config_ptr_->identify_))
						SessionRoom::GetRoom()->Deliever(ma_config_ptr_->identify_, my_feedback);
					else
						LoggerManager::getInstance()->Info("target does't exist");

				}
				break;
			case c_machine_logger:
				ma_config_ptr_->Set(ct_info_);
				i_DbHelper_ptr.reset(new MachineLoggerHelper(std::move(std::make_shared<MachineConfiguration>(MachineConfiguration(ct_info_)))));
				i_DbHelper_ptr->Insert();
				my_feedback = "Feedback=insert table " + s_header + " successed";
				cout << my_feedback << endl;;
				LoggerManager::getInstance()->Info(my_feedback);
				// If insert successfully, send feedback to monitor.
				if (ma_config_ptr_->identify_ != "")
				{
					if (SessionRoom::GetRoom()->IsOnline(ma_config_ptr_->identify_))
						SessionRoom::GetRoom()->Deliever(ma_config_ptr_->identify_, my_feedback);
					else
						LoggerManager::getInstance()->Info("target does't exist");
				}
				break;
			case c_machine_tube_history:
				ma_tube_history_ptr_->Set(ct_info_);

				i_HistoryHelper_ptr.reset(new MachineTubeHistoryHelper(ma_tube_history_ptr_));
				i_DbHelper_ptr.reset(new MachineTubeHistoryHelper(ma_tube_history_ptr_));

				// Check if there exist the same day's data.
				// if exist, then update, else insert.
				if (i_HistoryHelper_ptr->IsSameEpoch())
					i_DbHelper_ptr->Update();
				else
					i_DbHelper_ptr->Insert();

				if (ct_info_.back() == "c_rd_machine_tube_history")
				{
					ct_info_.pop_back();
					ct_info_.emplace_back("c_rd_machine_tube");
				}

				if (ct_info_.back() == "c_machine_tube_history")
				{
					ct_info_.pop_back();
					ct_info_.emplace_back("c_machine_tube");
				}

				ma_tube_ptr_->Set(ct_info_);
				i_DbHelper_ptr.reset(new MachineTubeHelper(ma_tube_ptr_));
				i_DbHelper_ptr->Update();

				break;
			case c_machine_rotate_history:

				ma_rotate_history_ptr_->Set(ct_info_);

				i_HistoryHelper_ptr.reset(new MachineRotateHistoryHelper(ma_rotate_history_ptr_));
				i_DbHelper_ptr.reset(new MachineRotateHistoryHelper(ma_rotate_history_ptr_));

				// Check if there exist the same day's data.
				// if exist, then update, else insert.
				if (i_HistoryHelper_ptr->IsSameEpoch())
					i_DbHelper_ptr->Update();
				else
					i_DbHelper_ptr->Insert();

				if (ct_info_.back() == "c_rd_machine_rotate_history")
				{
					ct_info_.pop_back();
					ct_info_.emplace_back("c_rd_machine_rotate");
				}

				if (ct_info_.back() == "c_machine_rotate_history")
				{
					ct_info_.pop_back();
					ct_info_.emplace_back("c_machine_rotate");
				}

				ma_rotate_ptr_->Set(ct_info_);
				i_DbHelper_ptr.reset(new MachineRotateHelper(ma_rotate_ptr_));
				i_DbHelper_ptr->Update();
				break;
			default:
				break;
			}
		}
		catch (const sql::SQLException& e)
		{
			// If insert failed, send feedback to monitor.
			my_feedback = "Feedback=insert table " + s_header + " failed";
			cout << my_feedback << endl;;
			LoggerManager::getInstance()->Info(my_feedback);
			SessionRoom::GetRoom()->Deliever(ma_config_ptr_->identify_, my_feedback);
			throw e;
		}
		catch (const std::exception& e) {
			throw e;
		}

		return 1;
	}
}


void CTMsgHelper::Initialize()
{
	InitHelper();
}



void CTMsgHelper::SetID(const string& id)
{
	SN_ = id;
	tag = "CT";
	uid = id;
}

void CTMsgHelper::InitHelper()
{
	ma_ptr_					= MachinePtr(new Machine());
	ma_config_ptr_			= MachineConfigurationPtr(new MachineConfiguration());
	ma_tube_ptr_			= MachineTubePtr(new MachineTube());
	ma_tube_history_ptr_	= MachineTubeHistoryPtr(new MachineTubeHistory());
	ma_rotate_ptr_			= MachineRotatePtr(new MachineRotate());
	ma_rotate_history_ptr_	= MachineRotateHistoryPtr(new MachineRotateHistory());
	login_ptr				= CLoginPtr(new CLogin());
}
