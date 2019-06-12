#ifndef TIMER_H
#define TIMER_H

#include "Timestamp.h"
#include "IRun.h"

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

        bool isRepeat()
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

#endif    