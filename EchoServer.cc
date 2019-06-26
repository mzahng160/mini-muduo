#include "EchoServer.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Buffer.h"
#include "CurrentThread.h"
#include "Task.h"

#include <iostream>

using namespace std;

__thread int CurrentThread::t_cacheTid; 

#define MESSAGE_LENGTH 	8

EchoServer::EchoServer(EventLoop* pLoop)
	:_pLoop(pLoop)
	,_pServer(pLoop)
	,_index(0)	
{
	_pServer.setCallback(this);
}
EchoServer::~EchoServer()
{}

void EchoServer::start()
{
	_pServer.start();
	_threadpool.start(3);
}
void EchoServer::onConnection(TcpConnection* pCon)
{
	cout << "onConnection" << endl;
}
void EchoServer::onMessage(TcpConnection* pCon, Buffer* message)
{
	while(message->readableBytes() >= MESSAGE_LENGTH)
	{
		string data = message->retrieveAsString(MESSAGE_LENGTH);		
		Task task(this, data, pCon);
		_threadpool.addTask(task);
	}
}

void  EchoServer::onWriteComplate(TcpConnection* pCon)
{
	cout << "onWriteComplate" << endl;
}

void EchoServer::run2(const string& str, void* tcp)
{	
	cout << "fib(30) = "<< fib(30) << "tid: " << CurrentThread::tid() << endl;
	((TcpConnection*)tcp)->send(str + "\n");
}

int EchoServer::fib(int n)
{
	return (n == 1 || n == 2) ? 1 : (fib(n-1) + fib(n-2));
}