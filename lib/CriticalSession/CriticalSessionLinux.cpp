#include "CriticalSessionLinux.h"


CCriticalSessionLinux::CCriticalSessionLinux()
{
#ifndef WIN
	pthread_mutex_init(&m_mutex,NULL);
#endif
}


CCriticalSessionLinux::~CCriticalSessionLinux()
{
#ifndef WIN
	pthread_mutex_destroy(&m_mutex);
#endif
}

void CCriticalSessionLinux::Lock()
{
#ifndef WIN
	pthread_mutex_lock(&m_mutex);
#endif

}
void CCriticalSessionLinux::UnLock()
{
#ifndef WIN
	pthread_mutex_unlock(&m_mutex);
#endif

}
