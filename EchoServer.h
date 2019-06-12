#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include "IMuduoUser.h"
#include "TcpServer.h"
#include "Timer.h"
#include "IRun.h"

class EchoServer : public IMuduoUser
					,public IRun
{
public:
	EchoServer(EventLoop* pLoop);
	~EchoServer();

	void start();
	virtual void onConnection(TcpConnection* pCon);
	virtual void onMessage(TcpConnection* pCon, Buffer* message);
    virtual void onWriteComplate(TcpConnection* pCon);

    virtual void run(void* param);
private:
	EventLoop* _pLoop;
	TcpServer _pServer;

	Timer* 	_timer;
	int 	_index;
};

#endif