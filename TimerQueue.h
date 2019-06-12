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
{
public:
  
    class AddTimerWrapper : public IRun
    {
        public:
            AddTimerWrapper(TimerQueue* pQueue)
                :_pQueue(pQueue)
                {}
            virtual void run(void* param)
            {
                _pQueue->doAddTimer(param);
            }
        private:
            TimerQueue* _pQueue;
    };

    class CancelTimerWrapper : public IRun
    {
        public:
            CancelTimerWrapper(TimerQueue* pQueue)
                :_pQueue(pQueue)
                {}

                virtual void run(void* param)
                {
                    _pQueue->doCancelTimer(param);
                }
          private:
              TimerQueue* _pQueue;

    };

    TimerQueue(EventLoop* pLoop);
    ~TimerQueue();
    void doAddTimer(void* param);
    void doCancelTimer(void* param);
    Timer* addTimer(IRun* pRun, Timestamp when,
            double interval);
    void cancelTimer(Timer* timerId);

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
    AddTimerWrapper* _addTimerWrapper;
    CancelTimerWrapper* _cancelTimerWrapper;
};

#endif