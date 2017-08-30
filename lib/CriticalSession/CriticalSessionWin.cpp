#include "CriticalSessionWin.h"


CCriticalSessionWin::CCriticalSessionWin()
{
#ifdef WIN
	InitializeCriticalSection(&m_CS);
#endif
}


CCriticalSessionWin::~CCriticalSessionWin()
{
#ifdef WIN
	DeleteCriticalSection(&m_CS);
#endif

}
void CCriticalSessionWin::Lock()
{
#ifdef WIN
	EnterCriticalSection(&m_CS);
#endif

}
void CCriticalSessionWin::UnLock()
{
#ifdef WIN
	LeaveCriticalSection(&m_CS);
#endif

}