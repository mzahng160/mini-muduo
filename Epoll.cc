#include "Epoll.h"
#include "Channel.h"
#include <iostream>
#include <sys/epoll.h>

using namespace std;

const int kNew = -1;
const int kAdded = 1;


Epoll::Epoll()
{
	_epollfd = ::epoll_create(1);
	if(_epollfd < 0)
		cout << "epoll_create error, errno:" << errno << endl;
}
Epoll::~Epoll()
{}

void Epoll::poll(std::vector<Channel*>* pChannels)
{
	int fds = ::epoll_wait(_epollfd, _events, MAX_EVENTS, -1);
	if(fds == -1)
	{
		cout << "epoll_wait error, errno:" << errno << endl;
		return;
	}

	for (int i = 0; i < fds; ++i)
	{
		Channel* pChannel = static_cast<Channel*>(_events[i].data.ptr);
		pChannel->setRevents(_events[i].events);
		pChannels->push_back(pChannel);
	}
}

void Epoll::update(Channel* pChannel)
{
	int index = pChannel->getIndex();
	if(index == kNew)
	{
		struct epoll_event ev;
		ev.data.ptr = pChannel;
		ev.events = pChannel->getEvents();
		int fd = pChannel->getSockfd();
		pChannel->setIndex(kAdded);
		::epoll_ctl(_epollfd, EPOLL_CTL_ADD, fd, &ev);		
	}
	else
	{
		struct epoll_event ev;
		ev.data.ptr = pChannel;
		ev.events = pChannel->getSockfd();
		int fd = pChannel->getSockfd();
		::epoll_ctl(_epollfd, EPOLL_CTL_MOD, fd, &ev);
	}
}