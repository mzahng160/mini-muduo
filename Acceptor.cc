#include "Acceptor.h"
#include "Channel.h"
#include "EventLoop.h"
#include "Define.h"
#include "IAcceptorCallback.h"
#include <arpa/inet.h>
#include <fcntl.h>
#include <errno.h>
#include <iostream>
using namespace std;

Acceptor::Acceptor(EventLoop* pLoop)
	:_listenfd(-1)
	,_pSocketAChannel(NULL)
	,_pCallback(NULL)
	,_pLoop(pLoop)
{}
Acceptor::~Acceptor()
{}

void Acceptor::start()
{
	_listenfd = createAndListen();
	_pSocketAChannel = new Channel(_pLoop, _listenfd);
	_pSocketAChannel->setCallback(this);
	_pSocketAChannel->enableReading();
}

int Acceptor::createAndListen()
{
	int on = 1;
	short port = PORT;

	int listenfd = socket(AF_INET, SOCK_STREAM, 0);

	struct sockaddr_in serveraddr;
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(port);

	fcntl(listenfd, F_SETFL, O_NONBLOCK);
	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));

	int bindret = bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
	if(-1 == bindret)
		cout << "bind error, errno:" << errno << endl;

	int listenret = listen(listenfd, MAX_LISTENFD);
	if(-1 == listenret)
		cout << "listen error, errno:" << errno << endl;

	return listenfd;
}

void Acceptor::handleRead()
{
	struct sockaddr_in cliaddr;
	socklen_t clilen = sizeof(struct sockaddr_in);
	int connfd = accept(_listenfd, (sockaddr*)&cliaddr, (socklen_t*)&clilen);
	if(connfd > 0)
	{
		cout << "new connection from " << 
				"[" << inet_ntoa(cliaddr.sin_addr) << 
				":" << ntohs(cliaddr.sin_port) << "]" <<
				"accept socket fd:" << connfd << endl;							
	}
	else
		cout << "accept error, connfd:" << connfd << " errono" << errno << endl;

	fcntl(connfd, F_SETFL, O_NONBLOCK);
	_pCallback->newConnection(connfd);
}
void Acceptor::handleWrite()
{

}

void Acceptor::setCallback(IAcceptorCallback* pCallback)
{
	_pCallback = pCallback;
}