#pragma once
#ifdef WIN
#include <winsock2.h>
#include <Windows.h>
#else
#include <pthread.h>
#endif
#include "DatabaseCom.h"
#include "mysql/mysql.h"
#include <stdio.h>

class CDatabaseMySQL :public CDatabaseCom
{
public:
	CDatabaseMySQL(void);
	~CDatabaseMySQL(void);
	virtual bool InitConnect(CDBParam * pParam);
	virtual bool Query(const char *const pStrSql, fnDBRowDataPtr fnRowData,void* pList);
	virtual int Exec(const char * const pStrSql);
	virtual string GetLastError();
private:
#ifdef WIN
      CRITICAL_SECTION m_Lock;
#else
      pthread_mutex_t m_Mutex;
#endif

	MYSQL * m_pMySql;	
};

