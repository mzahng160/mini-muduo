#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include "IMuduoUser.h"
#include "TcpServer.h"
#include "Timer.h"
#include "IRun.h"
#include "ThreadPool.h"

class EchoServer : public IMuduoUser
					,public IRun2
{
public:
	EchoServer(EventLoop* pLoop);
	~EchoServer();

	void start();
	virtual void onConnection(TcpConnection* pCon);
	virtual void onMessage(TcpConnection* pCon, Buffer* message);
    virtual void onWriteComplate(TcpConnection* pCon);

    virtual void run2(const string& str, void* tcp);
private:
	int fib(int n);
	EventLoop* _pLoop;
	TcpServer _pServer;
	ThreadPool _threadpool;

	Timer* 	_timer;
	int 	_index;
};

#endif