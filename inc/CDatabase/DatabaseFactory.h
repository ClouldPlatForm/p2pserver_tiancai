#pragma once
#include "DBParam.h"
#include "DatabaseCom.h"
enum DBTYPE
{
	DBTYPE_MYSQL = 0,
	DBTYPE_MSSQL = 1,
	DBTYPE_ORACLE = 2,
	DBTYPE_SYSBASE = 3,
	DBTYPE_DB2 = 4,
	DBTYPE_ACESS = 5,
};
class  CDatabaseFactory
{
public:
	CDatabaseFactory();
	~CDatabaseFactory();
	bool SelectDb(DBTYPE DbType);
	CDatabaseCom * m_pDataBase;
};

