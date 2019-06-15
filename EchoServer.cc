#include "EchoServer.h"
#include "TcpConnection.h"
#include "EventLoop.h"
#include "Buffer.h"

#include <iostream>

using namespace std;

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
}
void EchoServer::onConnection(TcpConnection* pCon)
{
	cout << "onConnection" << endl;
}
void EchoServer::onMessage(TcpConnection* pCon, Buffer* message)
{
	while(message->readableBytes() > MESSAGE_LENGTH)
	{
		string data = message->retrieveAsString(MESSAGE_LENGTH);
		cout << "EchoServer onMessage " << data << endl;
		pCon->send(data + '\n');
	}

	_timer = _pLoop->runEvery(0.5, this);
}

void  EchoServer::onWriteComplate(TcpConnection* pCon)
{
	cout << "onWriteComplate" << endl;
}

void EchoServer::run(void* param)
{
	cout << "EchoServer index "<<_index << endl;
	if(_index++ == 3)
	{
		_pLoop->cancelTimer(_timer);
		_index = 0;
	}
}