#ifndef THREADPOOL_H
#define THREADPOLL_H

#include "IRun.h"
#include "Declear.h"
#include "BlockQueue.h"

#include <vector>

class ThreadPool : public IRun0
{
public:
	ThreadPool();
	void start(int numThreads);
	void addTask(Task& task);

public:
	virtual void run0();

private:
	void runInThread();
	BlockQueue<Task> _tasks;
	std::vector<Thread*> _threads;
	
};

#endif