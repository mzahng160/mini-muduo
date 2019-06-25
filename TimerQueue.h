#ifndef _TIMERQUEUE_H
#define _TIMERQUEUE_H

#include "Declear.h"
#include "IChannelCallback.h"
#include "IRun.h"
#include "Timestamp.h"
#include "Timer.h"

#include <set>
#include <vector>

class TimerQueue : public IChannelCallback
                    ,IRun2
{
public:

    TimerQueue(EventLoop* pLoop);
    ~TimerQueue();
    void doAddTimer(Timer* param);
    void doCancelTimer(Timer* param);

    Timer* addTimer(IRun0* pRun, Timestamp when,
            double interval);
    void cancelTimer(Timer* timerId);

    virtual void run2(const std::string& str, void* timer);

    virtual void handleRead();
    virtual void handleWrite();

private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;

    int createTimerId();
    vector<TimerQueue::Entry> getExpired(Timestamp now);
    void readTimerfd(int timerfd, Timestamp now);
    void reset(const std::vector<Entry>& expired, Timestamp now);

    void resetTimerfd(int timerfd, Timestamp stamp);
    bool insert(Timer* pItem);
    struct timespec howMuchTimeFromNow(Timestamp when);

    int _timerfd;
    TimerList _timers;
    EventLoop* _pLoop;
    Channel* _timerfdChannel;
};

#endif