#ifndef _IRUN_H
#define _IRUN_H

#include <string>

class IRun0
{
public:
	virtual void  run0() = 0;
};

class IRun2
{
public:
	virtual void  run2(const std::string& str, void* param) = 0;
};
#endif