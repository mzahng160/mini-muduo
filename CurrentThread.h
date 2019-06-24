#ifndef CURRENTTHREAD_H
#define CURRENTTHREAD_H

#include <unistd.h>
#include <sys/syscall.h>

namespace CurrentThread
{
	extern __thread int t_cacheTid;
	inline void cacheTid()
	{
		t_cacheTid = static_cast<int>(::syscall(SYS_gettid));		
	}

	inline int tid()
	{
		if(t_cacheTid == 0)
		{
			cacheTid();
		}

		return t_cacheTid;
	}
}

#endif