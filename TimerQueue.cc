#include "TimerQueue.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Task.h"
#include "Define.h"

#include <strings.h>
#include <unistd.h>
#include <inttypes.h>
#include <sys/timerfd.h>
#include <inttypes.h>
#include <iostream>

using namespace std;

#ifndef UINTPTR_MAX 
#define UINTPTR_MAX 0xffffffff
#endif

TimerQueue::TimerQueue(EventLoop* pLoop)
    :_timerfd(createTimerId())
    ,_pLoop(pLoop)
	,_timerfdChannel(new Channel(_pLoop, _timerfd))
{
	_timerfdChannel->setCallback(this);
	_timerfdChannel->enableReading();
}

TimerQueue::~TimerQueue()
{
	::close(_timerfd);
}

void TimerQueue::run2(const std::string& str, void* timer)
{
	if(str == "addTimer")
	{
		doAddTimer((Timer*)timer);
	}
	else if(str == "cancelTimer")
	{
		doCancelTimer((Timer*)timer);
	}
}

void TimerQueue::doAddTimer(Timer* param)
{
	bool earliestChanged = insert(param);
	if(earliestChanged)
	{
		resetTimerfd(_timerfd, param->getStamp());
	}
}

void TimerQueue::doCancelTimer(Timer* param)
{
	Entry e(param->getId(), param);
	TimerList::iterator it;
	for (it = _timers.begin(); it != _timers.end(); it++)
	{
		if (it->second == param)
		{
			_timers.erase(it);
			break;
		}
	}
}

Timer* TimerQueue::addTimer(IRun0* pRun, Timestamp when, double interval)
{

	Timer* pTimer = new Timer(when, pRun, interval);
	Task task(this, "addTimer", pTimer);
	_pLoop->queueInLoop(task);
	return pTimer;
}

void TimerQueue::cancelTimer(Timer* timerId)
{
	Task task(this, "cancelTimer", timerId);
	_pLoop->queueInLoop(task);
}

void TimerQueue::handleRead()
{
	Timestamp now(Timestamp::now());
	readTimerfd(_timerfd, now);

	std::vector<Entry> expire = getExpired(now);
	std::vector<Entry>::iterator it;
	for(it = expire.begin(); it != expire.end(); ++it)
	{
		it->second->timeout();
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
