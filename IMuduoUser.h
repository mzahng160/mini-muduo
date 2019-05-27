#ifndef IMUDUOUSER_H
#define IMUDUOUSER_H

#include "Declear.h"
#include <string>

class IMuduoUser
{
public:
	virtual void onConnection(TcpConnection* pCon) = 0;
	virtual void onMessage(TcpConnection* pCon, std::string* data) = 0;
};

#endif