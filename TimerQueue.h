#ifndef _TIMERQUEUE_H
#define _TIMERQUEUE_H

#include "Declear.h"
#include "IChannelCallback.h"

class TimerQueue : public IChannelCallback
{
public:
	TimerQueue();
	~TimerQueue();
	
};

#endif