#include "Channel.h"
#include <sys/epoll.h>
#include <iostream>
#include "IChannelCallback.h"
#include "EventLoop.h"

using namespace std;

Channel::Channel(EventLoop* pLoop, int sockfd)
	:_sockfd(sockfd)
	,_events(0)
	,_revents(0)
	,_index(-1)
	,_pCallback(NULL)
	,_pLoop(pLoop)
{
	cout << "Channel new Channel finish!" << " sockfd "<< sockfd << endl;
}

Channel::~Channel()
{}

void Channel::setCallback(IChannelCallback* pCallback)
{
	_pCallback = pCallback;
}
void Channel::handleEvent()
{
	if(_revents & EPOLLIN)
	{
		_pCallback->handleRead();
	}

	if(_revents & EPOLLOUT)
	{
		_pCallback->handleWrite();
	}
}

void Channel::Channel::setRevents(int revents)
{
	_revents = revents;
}
void Channel::setIndex(int index)
{
	_index = index;
}

void Channel::enableReading()
{
	_events |= EPOLLIN;
	update();
}
void Channel::enableWriting()
{
	_events |= EPOLLOUT;
	update();
}
void Channel::disableWriting()
{
	_events &= ~EPOLLOUT;
	update();
}
bool Channel::isWriting()
{
	return _events & EPOLLOUT;
}

int Channel::getEvents()
{
	return _events;
}
int Channel::getSockfd()
{
	return _sockfd;
}
int Channel::getIndex()
{
	return _index;
}

void Channel::update()
{
	_pLoop->update(this);
}