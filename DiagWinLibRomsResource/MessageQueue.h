// MessageQueue.h : Defines message queue template.
//
// Copyright (c) 2018 FMI Medical System.
//
// Distributed under the Boost Software License, Version 1.65.1 (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#include <mutex>
#include <queue>

template <typename T>
class MessageQueue {
public:
	void Insert(T value);
	void Popup(T &value);
	bool Empety();
private:
	mutable std::mutex lock_;
	std::queue<T> que_;
	std::condition_variable cond_;
};

