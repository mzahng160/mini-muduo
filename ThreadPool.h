#ifndef THREADPOOL_H
#define THREADPOLL_H

#include "IRun.h"
#include "Declear.h"
#include "BlockQueue.h"

#include <vector>

class ThreadPool : public IRun
{
public:
	ThreadPool();
	void start(int numThreads);
	void addTask(IRun* ptask);	

public:
	virtual void run(void* param);

private:
	void runInThread();
	BlockQueue<IRun*> _tasks;
	std::vector<Thread*> _threads;
	
};

#endif