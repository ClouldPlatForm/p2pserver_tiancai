#pragma once
class CCriticalSession
{
public:
	CCriticalSession();
	~CCriticalSession();
	virtual void Lock()= 0;
	virtual void UnLock() = 0;
};

