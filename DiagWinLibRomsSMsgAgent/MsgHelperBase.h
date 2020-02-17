/***********************************************************************
* Filename: MsgHelperBase.h
* Original Author: Ke Ye
* File Creation Date: 2018.5.10
* Subsystem:
* Diagnosis
* description:
* Defines the message helper base for server.
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/

#pragma once
#include <iostream>

using namespace std;

class MsgHelperBase
{
public:

	virtual void Initialize() = 0;
	/***********************************************************************
	* Module Name: DealInfo
	*
	* Description: Dealing message from machine.
	*
	* Calling Arguments:
	* Name		Input/Output		Type		Description
	* content	read buffer			string		message read buffer
	*
	* Return/Output codes:
	* 0				Login failed or message head doesn't exit
	* 1				insert or update succeccfully
	* 2				Target client doesn't exist in session room.
	***********************************************************************/
	virtual int DealInfo(const string& content) = 0;

	virtual void SetID(const string& id) = 0;

	virtual ~MsgHelperBase() {};

	string tag;
	
	string uid;

private:
};

typedef std::shared_ptr<MsgHelperBase>	MsgHelperBasePtr;
