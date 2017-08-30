#pragma once
#include "CriticalSession.h"
#ifdef WIN
#include <Windows.h>
#endif
class CCriticalSessionWin :public CCriticalSession
{
public:
	CCriticalSessionWin();
	~CCriticalSessionWin();
	virtual void Lock();
	virtual void UnLock();
private:
#ifdef WIN
	CRITICAL_SECTION m_CS;
#endif
};

