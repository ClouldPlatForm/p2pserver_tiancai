#pragma once
#include "DBParam.h"
#include <string>
typedef void ( * fnDBRowDataPtr)(int nFiledCount,char ** row,unsigned long *lens,void *pList);
using namespace std;

class CDatabaseCom
{
public:
	CDatabaseCom(void);
	virtual ~CDatabaseCom(void);
public:
	virtual bool InitConnect(CDBParam * pParam) = 0;
	virtual bool Query(const char *const pStrSql,fnDBRowDataPtr fnRowData,void* pList) = 0;
	virtual int Exec(const char * const pStrSql) = 0 ;
	virtual string GetLastError() = 0;
protected:	
	bool m_bConn;
	string m_strError;
};


