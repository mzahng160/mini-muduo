#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"

#include <unistd.h>
#include <sys/eventfd.h>

#include <iostream>
using namespace std;

EventLoop::EventLoop()
	:_quit(false)
	,_pPoller(new Epoll())
	,_pTimerQueue(new TimerQueue(this))
{
	_eventfd = createEventfd();
	_pEventfdChannel = new Channel(this, _eventfd);
	_pEventfdChannel->setCallback(this);
	_pEventfdChannel->enableReading();	
}
EventLoop::~EventLoop()
{}

void EventLoop::loop()
{
	while(!_quit)
	{
		std::vector<Channel*> channels;
		_pPoller->poll(&channels);

		std::vector<Channel*>::iterator it;
		for (it = channels.begin(); it != channels.end(); ++it)
		{
			(*it)->handleEvent();
		}

		doPendingFunctors();
	}
}
void EventLoop::update(Channel* pChannel)
{
	_pPoller->update(pChannel);
}

void EventLoop::queueLoop(IRun* pRun, void *param)
{
	Runner r(pRun, param);
	_pendingFunctors.push_back(r);
	weakup();
}

void EventLoop::wakeup()
{
	uint64_t one = 1;
	ssize_t n = ::write(_eventfd, &one, sizeof(one));
	if(n != sizeof one)
		cout << "EventLoop::weak() writes" << n << "bytes not 8" << endl;
}

Timer* EventLoop::runAt(Timestamp when, IRun* pRun)
{
	return _pTimerQueue->addTimer(pRun, when, 0.0);
}

Timer* EventLoop::runAfter(double delay, IRun* pRun)
{
	return _pTimerQueue->addTimer(pRun, Timestamp::nowAfter(delay), 0.0);
}

Timer* EventLoop::runEvery(double interval, IRun* pRun)
{
	return _pTimerQueue->addTimer(pRun, Timestamp::nowAfter(interval), interval);
}

void EventLoop::cancelTimer(Timer* timerfd)
{
	return _pTimerQueue->cancelTimer(timerfd);
}

int EventLoop::createEventfd()
{
	int evtfd = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
	if(evtfd < 0)
		cout << "Failed in eventfd" << endl;

	return evtfd;
}

void EventLoop::doPendingFunctors()
{
	std::vector<Runner> tempRuns;
	tempRuns.swap(_pendingFunctors);
	std::vector<Runner>::iterator it;
	for (it = tempRuns.begin(); it != tempRuns.end(); ++it)
	{
		(*it).doRun();
	}
}

void EventLoop::handleRead()
{
	uint64_t one = 1;
	ssize_t n = ::read(_eventfd, &one, sizeof one);
	if(n != sizeof one)
		cout << "EventLoop::handleRead read " << n << " byte not 8" << endl;
}

void EventLoop::handleWrite()
{

}