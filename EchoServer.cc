#include "EchoServer.h"
#include "TcpConnection.h"
#include <iostream>

using namespace std;

#define MESSAGE_LENGTH 	8

EchoServer::EchoServer(EventLoop* pLoop)
	:_pLoop(pLoop)
	,_pServer(pLoop)
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
void EchoServer::onMessage(TcpConnection* pCon, string* message)
{
	while(message->size() > MESSAGE_LENGTH)
	{
		string data = message->substr(0, MESSAGE_LENGTH);
		*message = message->substr(MESSAGE_LENGTH, message->size());
		pCon->send(data + '\n');
	}
}