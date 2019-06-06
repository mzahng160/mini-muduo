#include "TimerQueue.h"
#include "Channel.h"

#include <unistd.h>
//#include <stdio.h>
//#include <inttypes.h>
#include <sys/timerfd.h>

TimerQueue::TimerQueue(EventLoop* pLoop)
    :_timerfd(createTimerId())
    ,_pLoop(pLoop)
    ,_timerfdChannel(new Channel(pLoop, _timerfd))
    ,_addTimerWrapper(new AddTimerWrapper(this))
    ,_cancelTimerWrapper(new CancelTimerWrapper(this))
{
	_timerfdChannel;
}
TimerQueue::~TimerQueue()
{
	::close(_timerfd);
}

void TimerQueue::doAddTimer(void* param)
{
	Timer* pTimer = static_cast<Timer*>(param);
	bool earliestChanged = insert(pTimer);
	if(earliestChanged)
	{
		resetTimerfd(_timerfd, pTimer->getStamp());
	}
}

void TimerQueue::doCancelTimer(void* param)
{

}

int TimerQueue::createTimerId()
{
	
}

void TimerQueue::resetTimerfd(int timerfd, Timestamp stamp)
{
	struct itimerspec newValue;
}

bool TimerQueue::insert(Timer* pItem)
{
	bool earliestChanged = false;
	Timestamp when = pItem->getStamp();
	TimerList::iterator it = _timers.begin();
	if(it == _timers.end() || when < it->first)
	{
		earliestChanged = true;
	}
}