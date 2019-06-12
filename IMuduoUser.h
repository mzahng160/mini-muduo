#ifndef IMUDUOUSER_H
#define IMUDUOUSER_H

#include "Declear.h"
#include <string>

class IMuduoUser
{
public:
	virtual void onConnection(TcpConnection* pCon) = 0;
	virtual void onMessage(TcpConnection* pCon, Buffer* data) = 0;
    virtual void onWriteComplate(TcpConnection* pCon) = 0;
};

#endif