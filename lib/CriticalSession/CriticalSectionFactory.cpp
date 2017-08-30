#include "CriticalSectionFactory.h"
#include "CriticalSessionLinux.h"
#include "CriticalSessionWin.h"

CCriticalSectionFactory::CCriticalSectionFactory()
{
	m_pCS = NULL;
}


CCriticalSectionFactory::~CCriticalSectionFactory()
{
	if (NULL != m_pCS)
	{
		delete m_pCS;
		m_pCS = NULL;
	}
}

bool CCriticalSectionFactory::CreateProduct()
{
#ifdef WIN
	m_pCS = new CCriticalSessionWin();
#else
	m_pCS = new CCriticalSessionLinux();
#endif
	if (NULL == m_pCS)
	{
		return false;
	}
	return true;
}
