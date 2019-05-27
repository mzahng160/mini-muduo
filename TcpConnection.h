#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "Declear.h"
#include "IChannelCallback.h"
#include <string>

class TcpConnection : public IChannelCallback
{
public:
	TcpConnection(int sockfd, EventLoop* pLoop);
	~TcpConnection();

	void send(const std::string& message);
	void connectionEstablished();
	void setUser(IMuduoUser* pUser);

	virtual void handleRead();
	virtual void handleWrite();

private:
	int _sockfd;
	Channel* _pChannel;
	EventLoop* _pLoop;
	IMuduoUser* _pUser;
	std::string* _inBuf;
	std::string* _outBuf;
};

#endif