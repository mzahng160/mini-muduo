#ifndef BLOCKQUEUE_H
#define BLOCKQUEUE_H

#include <deque>
#include "Condition.h"
#include "Mutex.h"

template <class T>
class BlockQueue
{
public:
	BlockQueue()
		:_cond(_mutex)
	{}
	~BlockQueue()
	{}

	void put(const T& one)
	{
		MutexLockGuard lock(_mutex);
		_queue.push_back(one);
		_cond.notify();
	}

	T take()
	{
		MutexLockGuard lock(_mutex);
		while(_queue.empty())		
		{
			_cond.wait();
		}

		T front(_queue.front());
		_queue.pop_front();
		return front;
	}
	
private:
	std::deque<T> _queue;
	MutexLock _mutex;
	Condition _cond;
};

#endif