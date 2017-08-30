#pragma once
#include "CriticalSession.h"
#ifndef WIN
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h> 
#endif
class CCriticalSessionLinux :	public CCriticalSession
{
public:
	CCriticalSessionLinux();
	~CCriticalSessionLinux();
	virtual void Lock();
	virtual void UnLock();
private:
#ifndef WIN
	pthread_mutex_t m_mutex;
#endif
};

