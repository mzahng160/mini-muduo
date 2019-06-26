#include "ThreadPool.h"
#include "Thread.h"
#include "Task.h"

#include <pthread.h>
#include <iostream>
#include <sstream>

using namespace std;

ThreadPool::ThreadPool()
{}

void ThreadPool::start(int numThreads)
{
	_threads.reserve(numThreads);
	for(int i = 0; i < numThreads; i++)
	{
		Task task(this);
		Thread* p = new Thread(task);
		_threads.push_back(p);
		p->start();
	}
}

void ThreadPool::addTask(Task& task)
{
	_tasks.put(task);
}

void ThreadPool::run0()
{
	runInThread();
}

void ThreadPool::runInThread()
{
	while(true)
	{
		_tasks.take().doTask();
	}
}