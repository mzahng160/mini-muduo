#include "TcpServer.h"
#include "Acceptor.h"
#include "TcpConnection.h"

TcpServer::TcpServer(EventLoop* pLoop):
	_pAcceptor(NULL)
	,_pLoop(pLoop)
	,_pUser(NULL)
{

}

TcpServer::~TcpServer()
{

}


void TcpServer::start()
{
	_pAcceptor = new Acceptor(_pLoop);
	_pAcceptor->setCallback(this);
	_pAcceptor->start();
}

void TcpServer::newConnection(int sockfd)
{
	TcpConnection* tcp = new TcpConnection(sockfd, _pLoop);
	_connections[sockfd] = tcp;
	tcp->setUser(_pUser);
	tcp->connectionEstablished();
}
void TcpServer::setCallback(IMuduoUser* pUser)
{
	_pUser = pUser;
}