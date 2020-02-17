#include "MessageQueue.h"

template<typename T>
inline void MessageQueue<T>::Insert(T value)
{
	std::lock_guard<std::mutex> lock(lock_);
	que_.push_back(value);
	cond_.notify_one();
}

template<typename T>
inline void MessageQueue<T>::Popup(T & value)
{
	std::unique_lock<std::mutex> lock(lock_);
	cond_.wait(lk, [this] {return !que_.empety(); });
	value = que_.front();
	que_.pop();
}

template<typename T>
bool MessageQueue<T>::Empety()
{
	return false;
}


