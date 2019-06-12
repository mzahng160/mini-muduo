#ifndef EVENTLOOP_H
#define EVENTLOOP_H

#include "Declear.h"
#include "IRun.h"
#include "IChannelCallback.h"
#include "TimerQueue.h"

#include <vector>

class EventLoop : IChannelCallback
{
public:
	class Runner
	{
	public:
		Runner(IRun* r, void* p)
			:_pRun(r), _param(p){}
		void doRun()
		{
			_pRun->run(_param);
		}

	private:
		IRun* _pRun;
		void *_param;		
	};

	EventLoop();
	~EventLoop();

	void loop();
	void update(Channel* pChannel);

	void queueLoop(IRun* pRun, void *param);

	Timer* runAt(Timestamp when, IRun* pRun);
	Timer* runAfter(double delay, IRun* pRun);
	Timer* runEvery(double interval, IRun* pRun);
	void cancelTimer(Timer* timerfd);

	virtual void handleRead();
	virtual void handleWrite();

private:

	void weakup();
	int createEventfd();
	void doPendingFunctors();

	bool _quit;
	Epoll* _pPoller;
	int _eventfd;
	std::vector<Runner> _pendingFunctors;
	TimerQueue* _pTimerQueue;
	Channel* _weakupChannel;
	
};

#endif