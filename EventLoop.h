#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Declear.h"
#include "IRun.h"
#include "IChannelCallback.h"
#include "TimerQueue.h"
#include "Mutex.h"
#include <vector>

class EventLoop : IChannelCallback
{
public:

	EventLoop();
	~EventLoop();

	void loop();
	void update(Channel* pChannel);

	//void queueLoop(IRun* pRun, void *param);
	void queueInLoop(Task& task);
	void runInLoop(Task& task);

	Timer* runAt(Timestamp when, IRun0* pRun);
	Timer* runAfter(double delay, IRun0* pRun);
	Timer* runEvery(double interval, IRun0* pRun);
	void cancelTimer(Timer* timerfd);
	bool idInLoopThread();

	virtual void handleRead();
	virtual void handleWrite();

private:

	void wakeup();
	int createEventfd();
	void doPendingFunctors();

	bool _callingPendingFunctors;
	bool _quit;
	Epoll* _pPoller;
	int _eventfd;
	const pid_t _threadId;
	MutexLock _mutex;
	std::vector<Task> _pendingFunctors;
	TimerQueue* _pTimerQueue;
	Channel* _pEventfdChannel;
	
};

#endif