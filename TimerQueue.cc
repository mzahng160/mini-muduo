#include "TimerQueue.h"
#include "Channel.h"
#include <strings.h>

#include <unistd.h>
//#include <stdio.h>
#include <inttypes.h>
#include <sys/timerfd.h>
#include <inttypes.h>
#include <iostream>
#include "EventLoop.h"

using namespace std;

#ifndef UINTPTR_MAX 
#define UINTPTR_MAX 0xffffffff
#endif

TimerQueue::TimerQueue(EventLoop* pLoop)
    :_timerfd(createTimerId())
    ,_pLoop(pLoop)
	,_timerfdChannel(new Channel(_pLoop, _timerfd))
    ,_addTimerWrapper(new AddTimerWrapper(this))
    ,_cancelTimerWrapper(new CancelTimerWrapper(this))
{
	_timerfdChannel->setCallback(this);
	_timerfdChannel->enableReading();
}

TimerQueue::~TimerQueue()
{
	::close(_timerfd);
}

Timer* TimerQueue::addTimer(IRun* pRun, Timestamp when, double interval)
{

	Timer* pTimer = new Timer(when, pRun, interval);
	_pLoop->queueLoop(_addTimerWrapper, pTimer);
	return pTimer;
}

void TimerQueue::cancelTimer(Timer* timerId)
{
	_pLoop->queueLoop(_cancelTimerWrapper, (void*)timerId);
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
	Timer* timer = static_cast<Timer*>(param);
	Entry e(timer->getId(), timer);
	TimerList::iterator it;
	for (it = _timers.begin(); it != _timers.end(); it++)
	{
		if (it->second == timer)
		{
			_timers.erase(it);
			break;
		}
	}
}

void TimerQueue::handleRead()
{
	Timestamp now(Timestamp::now());
	readTimerfd(_timerfd, now);

	std::vector<Entry> expire = getExpired(now);
	std::vector<Entry>::iterator it;
	for(it = expire.begin(); it != expire.end(); ++it)
	{
		it->second->run();
	}

	reset(expire, now);
}

void TimerQueue::handleWrite()
{

}

int TimerQueue::createTimerId()
{
	int timerfd = ::timerfd_create(CLOCK_MONOTONIC, 
			TFD_NONBLOCK | TFD_CLOEXEC);
	if(timerfd < 0)
	{
		cout << "failed in timerfd_create" << endl;
	}

	return timerfd;
}

vector<TimerQueue::Entry> TimerQueue::getExpired(Timestamp now)
{
	std::vector<Entry> expired;
	Entry sentry(now, reinterpret_cast<Timer*>(UINTPTR_MAX));
	TimerList::iterator end = _timers.lower_bound(sentry);
	copy(_timers.begin(), end, back_inserter(expired));
	_timers.erase(_timers.begin(), end);
	return expired;
}

void TimerQueue::readTimerfd(int timerfd, Timestamp now)
{
	uint64_t howmany;
	ssize_t n = ::read(timerfd, &howmany, sizeof(howmany));
	if(n != sizeof(howmany))
		cout << "Timer::readTimerfd() error" << endl;
}

void TimerQueue::reset(const std::vector<Entry>& expired, Timestamp now)
{
	std::vector<Entry>::const_iterator it;
	for(it = expired.begin(); it != expired.end(); ++it)
	{
		if(it->second->isRepeat())
		{
			it->second->moveToNext();
			insert(it->second);
		}
	}

	Timestamp nextExpired;
	if(!_timers.empty())
		nextExpired = _timers.begin()->second->getStamp();

	if(nextExpired.valid())
		resetTimerfd(_timerfd, nextExpired);
}

void TimerQueue::resetTimerfd(int timerfd, Timestamp stamp)
{
	struct itimerspec newValue;
	struct itimerspec oldValue;
	bzero(&newValue, sizeof(newValue));
	bzero(&oldValue, sizeof(oldValue));

	newValue.it_value = howMuchTimeFromNow(stamp);
	int ret = ::timerfd_settime(timerfd, 0, &newValue, &oldValue);
	if(ret)
		cout << "timerfd_settime error" << endl;
}

struct timespec TimerQueue::howMuchTimeFromNow(Timestamp when)
{
	int64_t microseconds = when.microSecondsSinceEpoch()
				- Timestamp::now().microSecondsSinceEpoch();

	if(microseconds < 100)
		microseconds = 100;

	struct timespec ts;
	ts.tv_sec = static_cast<time_t>(
			microseconds / Timestamp::kMicroSecondsPerSecond);
	ts.tv_nsec = static_cast<long>(
			(microseconds % Timestamp::kMicroSecondsPerSecond) * 1000);

	return ts;
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

	pair<TimerList::iterator, bool> result 
		= _timers.insert(Entry(when, pItem));
	if(!(result.second))
	{
		cout << "_timers.insert() error" << endl;
	}

	return earliestChanged;
}
