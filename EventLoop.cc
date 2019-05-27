#include "EventLoop.h"
#include "Epoll.h"
#include "Channel.h"

EventLoop::EventLoop()
	:_quit(false)
	,_pPoller(new Epoll())
{
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
	}
}
void EventLoop::update(Channel* pChannel)
{
	_pPoller->update(pChannel);
}