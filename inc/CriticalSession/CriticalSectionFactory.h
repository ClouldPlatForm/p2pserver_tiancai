#pragma once
#include "CriticalSession.h"
class CCriticalSectionFactory
{
public:
	CCriticalSectionFactory();
	~CCriticalSectionFactory();
	bool CreateProduct();
	CCriticalSession * m_pCS;

};
