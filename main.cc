#include "TcpServer.h"
#include "EventLoop.h"
#include "EchoServer.h"
#include "Channel.h"

int main(int argc, char*argv[])
{
	EventLoop loop;
	EchoServer echoserver(&loop);
	echoserver.start();
	loop.loop();



	return 0;	
}