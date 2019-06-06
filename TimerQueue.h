#ifndef _TIMERQUEUE_H
#define _TIMERQUEUE_H

#include "Declear.h"
#include "IChannelCallback.h"
#include "IRun.h"
#include "Timestamp.h"
#include <set>
#include <vector>

class TimerQueue : public IChannelCallback
{
public:

    class Timer
    {
    public:
        Timer(Timestamp stamp, IRun* pRun, double interval)
          :_stamp(stamp)
          ,_id(stamp)
          ,_pRun(pRun)
          ,_interval(interval)        
          {}
        Timestamp getStamp()
        {
            return _stamp;
        }

        Timestamp getId()
        {
            return _id;
        }

        void run()
        {
          _pRun->run(this);
        }

        Timestamp isRepeat()
        {
            return _interval > 0.0;
        }

        void moveToNext()
        {
            _stamp = Timestamp::nowAfter(_interval);
        }
    private:
        Timestamp _stamp;
        Timestamp _id;
        IRun* _pRun;
        double _interval;    
    };
  
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

private:
    typedef std::pair<Timestamp, Timer*> Entry;
    typedef std::set<Entry> TimerList;

    int createTimerId();
    vector<TimerQueue::Entry> getExpired(Timestamp now);
    void resetTimerfd(int timerfd, Timestamp stamp);
    bool insert(Timer* pItem);

    int _timerfd;
    TimerList _timers;
    EventLoop* _pLoop;
    Channel* _timerfdChannel;
    AddTimerWrapper* _addTimerWrapper;
    CancelTimerWrapper* _cancelTimerWrapper;
};

#endif