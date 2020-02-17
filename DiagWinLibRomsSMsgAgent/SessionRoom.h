/***********************************************************************
* Filename: SessionRoom.h
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

#pragma once
#include <iostream>
#include <thread>
#include <set>
#include <mutex>

using namespace std;


class Participant
{
public:
	virtual ~Participant() {}
	virtual void Deliever(const string& msg) = 0;
	virtual string GetID() = 0;
};

typedef std::shared_ptr<Participant> ParticipantPtr;


class SessionRoom
{
public:
	static SessionRoom *GetRoom();

	void Join(ParticipantPtr participant) {
		std::lock_guard<std::mutex> locker_(lock);
		subscribers_.insert(participant);//add a client
	}

	void Leave(ParticipantPtr participant) {
		std::lock_guard<std::mutex> locker_(lock);
		subscribers_.erase(participant);
	}

	bool IsOnline(const string& uid) {
		for (auto connector : subscribers_)
		{
			if (uid == connector->GetID())
			{
				return true;
			}
			//return;
		}
		return false;
	}

	void Deliever(const string& uid , const string& msg)
	{
		//recent_msgs_.push_back(msg);
		//while (recent_msgs_.size() > max_recent_msgs)
		//	recent_msgs_.pop_front();//delete expired message

		for (auto connector : subscribers_)
		{
			if (uid == connector->GetID())
			{
				connector->Deliever(msg);
				
			}
			//return;
		}

	}

	std::set<ParticipantPtr> subscribers_;
private:
	std::mutex lock;
	SessionRoom() {}
	static SessionRoom* room_;
	enum { max_recent_msgs = 100 };
	//MessageQueue recent_msgs_;
};