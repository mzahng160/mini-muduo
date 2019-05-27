#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include "IMuduoUser.h"
#include "TcpServer.h"

class EchoServer : public IMuduoUser
{
public:
	EchoServer(EventLoop* pLoop);
	~EchoServer();

	void start();
	virtual void onConnection(TcpConnection* pCon);
	virtual void onMessage(TcpConnection* pCon, std::string* message);

private:
	EventLoop* _pLoop;
	TcpServer _pServer;
};

#endif