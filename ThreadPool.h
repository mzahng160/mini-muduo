#ifndef THREADPOOL_H
#define THREADPOLL_H

#include "IRun.h"
#include "Declear.h"
#include "BlockQueue.h"

#include <vector>

class TheradPool
{
public:
	TheradPool();
	void start(int numThreads);
	void addTask(IRun* ptask);
	~TheradPool();

public:
	virtual void run(void* param);

private:
	void runInThread();
	BlockQueue<IRun*> _tasks;
	vector<Thread*> _threads;
	
};

#endif