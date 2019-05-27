#include "TcpConnection.h"
#include "IMuduoUser.h"
#include "Channel.h"
#include "Define.h"
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>

#include <iostream>

using namespace std;

TcpConnection::TcpConnection(int sockfd, EventLoop* pLoop)
	:_sockfd(sockfd)
	,_pLoop(pLoop)
	,_pUser(NULL)
	,_inBuf(new string())
	,_outBuf(new string())
{
	_pChannel = new Channel(_pLoop, _sockfd);
	_pChannel->setCallback(this);
	_pChannel->enableReading();
}

TcpConnection::~TcpConnection()
{}

void TcpConnection::send(const string& message)
{
	int n = 0;
	if(_outBuf->empty())
	{
		n = ::write(_sockfd, message.c_str(), message.size());
		if(n < 0)
			cout << "write error" << endl;		
	}

	if(n < static_cast<int>(message.size()))
	{
		*_outBuf += message.substr(n, message.size());
		if(_pChannel->isWriting())
			_pChannel->enableWriting();
	}
}

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
	int sockfd = _pChannel->getSockfd();
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
		_inBuf->append(line, readlen);
		_pUser->onMessage(this, _inBuf);
	}
}
void TcpConnection::handleWrite()
{
	int sockfd = _pChannel->getSockfd();
	if(_pChannel->isWriting())
	{
		int n = ::write(sockfd, _outBuf->c_str(), _outBuf->size());
		if(n < 0)
		{
			cout << "write " << n << " byte data again" << endl;
			*_outBuf = _outBuf->substr(n, _outBuf->size());
			if(_outBuf->empty())
				_pChannel->disableWriting();
		}
	}
}