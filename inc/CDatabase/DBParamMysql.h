#pragma once
#include "DBParam.h"
class CDBParamMysql :	public CDBParam
{
public:
	CDBParamMysql(void);
	~CDBParamMysql(void);
	char m_szStrServerIP[20];
	char m_szStrUser[20];
	char m_szStrPwd[20];
	char m_szStrDatabase[20];
	unsigned int m_Port;
};

