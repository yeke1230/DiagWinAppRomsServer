/***********************************************************************
* Filename: WebMsgHelper.h
* Original Author: Ke Ye
* File Creation Date: 2018.5.10
* Subsystem:
* Diagnosis
* description:
* Defines the Web bussiness for server.
* It will deal the message from Web.
*
* Copyright Information © FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/

#include <iostream>
#include <vector>
#include <boost\algorithm\string.hpp>
#include "..\DiagWinLibRomsSShareAgent\ShareArea.h"
#include "..\DiagWinLibRomsResource\WMsgModels.h"
#include "MsgHelperBase.h"
#include "SessionRoom.h"
#pragma once

class WebMsgHelper : public MsgHelperBase
{
public:
	WebMsgHelper() {};

	/***********************************************************************
	* Module Name: SetID
	*
	* Description: Initialize the share points of web message helper.
	***********************************************************************/
	void Initialize();

	/***********************************************************************
	* Module Name: SetID
	*
	* Description: Set unique id SN_.
	***********************************************************************/
	void SetID(const string& id);

	/***********************************************************************
	* Module Name: DealInfo
	*
	* Description:  Dealing message from machine.
	*
	* Calling Arguments:
	* Name		Input/Output		Type		Description
	* buffer	buffer				string		message read buffer
	*
	* Return/Output codes:
	* 0				Login failed or message head doesn't exit
	* 1				insert update succeccfully
	* 2				can't find the target client in session room
	***********************************************************************/
	int DealInfo(const string& content);

	/***********************************************************************
	* Module Name: DealOnlineCTInfo
	*
	* Description: Dealing online message from machine.
	*
	* Calling Arguments:
	* Name		Input/Output		Type		Description
	* ctinfo	ctinfo				string		after spliting reader buffer
	***********************************************************************/
	int DealOnlineWebInfo(const string& ctinfo);

	~WebMsgHelper() {};

private:

	WLoginPtr	login_ptr_;
	SitePtr		site_ptr_;

	std::vector<string> v_msg_;
	std::vector<string> web_info_;
	string			SN_;

	enum Table {
		w_login, w_site
	} header_;
};
