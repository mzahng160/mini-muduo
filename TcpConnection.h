#ifndef TCPCONNECTION_H
#define TCPCONNECTION_H

#include "Declear.h"
#include "IChannelCallback.h"
#include "IRun.h"
#include "Buffer.h"

#include <string>

class TcpConnection : public IChannelCallback
						,public IRun
{
public:
	TcpConnection(int sockfd, EventLoop* pLoop);
	~TcpConnection();

	void send(const std::string& message);
	void connectionEstablished();
	void setUser(IMuduoUser* pUser);

	virtual void handleRead();
	virtual void handleWrite();
	virtual void run(void* param);

private:
	int _sockfd;
	Channel* _pSocketChannel;
	EventLoop* _pLoop;
	IMuduoUser* _pUser;
	Buffer _inBuf;
	Buffer _outBuf;
};

#endif