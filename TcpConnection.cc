#include "TcpConnection.h"
#include "IMuduoUser.h"
#include "Channel.h"
#include "Define.h"
#include "EventLoop.h"

#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <errno.h>
#include <iostream>

using namespace std;

TcpConnection::TcpConnection(int sockfd, EventLoop* pLoop)
	:_sockfd(sockfd)
	,_pLoop(pLoop)
	,_pUser(NULL)
{
	cout << "TcpConnection new Channel finish!" << endl;
	_pSocketChannel = new Channel(_pLoop, _sockfd);	
	_pSocketChannel->setCallback(this);
	_pSocketChannel->enableReading();
}

TcpConnection::~TcpConnection()
{}

void TcpConnection::connectionEstablished()
{
	if(_pUser)
		_pUser->onConnection(this);	
}
void TcpConnection::setUser(IMuduoUser* pUser)
{
	_pUser = pUser;
}

void TcpConnection::handleRead()
{
	int sockfd = _pSocketChannel->getfd();
	char line[MAX_LINE];

	if(sockfd < 0)
	{
		cout << "EPOLLIN error sockfd < 0" << endl;
		return ;
	}
	bzero(line, MAX_LINE);
	int readlen = read(sockfd, line, MAX_LINE);
	if(readlen < 0)
	{
		if(errno == ECONNRESET)
		{
			cout << "ECONNRESET close socket fd:" << socket << endl;
			close(sockfd);
		}		
	}
	else if(readlen == 0)
	{
		cout << "read 0 close sockfd " << sockfd << endl;
		close(sockfd);
	}
	else
	{
		string linesStr(line, readlen);
		_inBuf.append(linesStr);
		_pUser->onMessage(this, &_inBuf);
	}
}

void TcpConnection::handleWrite()
{
	cout << "TcpConnection handleWrite" << endl;

	int sockfd = _pSocketChannel->getfd();

	if(_pSocketChannel->isWriting())
	{
		int n = ::write(sockfd, _outBuf.peek(), _outBuf.readableBytes());
		if(n > 0)
		{
			cout << "write " << n << " byte data again" << endl;
			_outBuf.retrieve(n);		
			if(0 == _outBuf.readableBytes())
				_pSocketChannel->disableWriting();
				_pLoop->queueLoop(this, NULL);
		}
	}
}

void TcpConnection::send(const string& message)
{
	int n = 0;
	if(0 == _outBuf.readableBytes())
	{
		cout << "TcpConnection send " << message << endl;
		n = ::write(_sockfd, message.c_str(), message.size());
		if(n < 0)
			cout << "write error" << endl;		
	}

	if(n < static_cast<int>(message.size()))
	{
		_outBuf.append(message.substr(n, message.size()));
		if(_pSocketChannel->isWriting())
			_pSocketChannel->enableWriting();
	}
}

void TcpConnection::run(void* param)
{
	_pUser->onWriteComplate(this);
}