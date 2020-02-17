/***********************************************************************
* Filename: SessionRoom.cpp
* Original Author: Ke Ye
* File Creation Date: 2018.3.28
* Subsystem:
* Diagnosis
* description:
* Defines the session manager for server.
*
* Copyright Information ?FMI Technologies Inc. 2018
* Source Control Information:
* Distributed under the Boost Software License, Version 1.65.1  (See accompanying
* file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
**********************************************************************/

#include "SessionRoom.h"

SessionRoom* SessionRoom::room_ = nullptr;

SessionRoom * SessionRoom::GetRoom()
{
	if (room_ == nullptr)
	{
		room_ = new SessionRoom();
	}
	return room_;
}
