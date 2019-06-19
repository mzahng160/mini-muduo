#ifndef THREAD_H
#define THREAD_H

#include "BlockQueue.h"
#include "IRun.h"

class Thread
{
public:
	Thread(IRun* pRun);
	void start();
	void run();
	pid_t gettid();	

private:
	IRun* _run;	
	
};

#endif