/***********************************************************************
* Filename: WebMsgHelper.cpp
* Original Author: Ke Ye
* File Creation Date: 2018.5.10
* Subsystem:
* Diagnosis
* description:
* Defines the Web bussiness for server.
* It will deal the message from Web.
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/

#include "WebMsgHelper.h"

void WebMsgHelper::Initialize()
{
	login_ptr_ = WLoginPtr(new WLogin());
	site_ptr_ = SitePtr(new Site());
}

void WebMsgHelper::SetID(const string & id)
{
	SN_ = id;
	tag = "Web";
	uid = id;
}

int WebMsgHelper::DealInfo(const string & content)
{	
	try
	{
		return DealOnlineWebInfo(content);
	}
	catch (const std::exception& e)
	{
		throw e;
	}

}

int WebMsgHelper::DealOnlineWebInfo(const string & ctinfo)
{
	try
	{
		web_info_.clear();
		vector<string> m_strs;
		boost::split(m_strs, ctinfo, boost::is_any_of("|"));
		if (m_strs.max_size() > 0)
		{
			string s_header = m_strs[0];

			if (!strcmp(s_header.c_str(), "w_login"))		header_ = w_login;
			else if (!strcmp(s_header.c_str(), "w_site"))	header_ = w_site;
			else return false;

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
						if (!strcmp(str_cpy.c_str(), "Tag"))
						{
							boost::algorithm::replace_all(sub_sub_m_strs[k + 1], "<semicolon>", ";");
							boost::algorithm::replace_all(sub_sub_m_strs[k + 1], "<equal>", "=");
							boost::algorithm::replace_all(sub_sub_m_strs[k + 1], "<bar>", "|");
						}

						web_info_.push_back(sub_sub_m_strs[k]);
					}
				}
			}

			string writeinfo;

			switch (header_)
			{
			case WebMsgHelper::w_login:
				login_ptr_->Set(web_info_);
				break;
			case WebMsgHelper::w_site:
				site_ptr_->Set(web_info_);
				writeinfo = "Identify=" + login_ptr_->user + ";Command=" + site_ptr_->command_ + ";Parameters=" + site_ptr_->parameters_ + ";Tag=" + site_ptr_->tag_ + ";Source=" + site_ptr_->source_ + ";";
				if (SessionRoom::GetRoom()->IsOnline(site_ptr_->target_))
				{
					SessionRoom::GetRoom()->Deliever(site_ptr_->target_, writeinfo);
				}
				else
				{
					return 2;
				}
				break;
			default:
				break;
			}

			return 1;
		}
		return 0;
	}
	catch (const std::exception& e)
	{
		throw e;
	}

}
