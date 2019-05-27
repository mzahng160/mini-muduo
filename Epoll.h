#ifndef EPOLL_H
#define EPOLL_H

#include "Declear.h"
#include "Define.h"
#include <vector>
#include <sys/epoll.h>

class Epoll
{
public:
	Epoll();
	~Epoll();

	void poll(std::vector<Channel*>* pChannels);
	void update(Channel* pChannel);

private:
	int _epollfd;
	struct epoll_event _events[MAX_EVENTS];
	
};

#endif