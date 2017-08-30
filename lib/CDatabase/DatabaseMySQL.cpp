#include "DatabaseMySQL.h"
#include "DBParamMysql.h"
#include <string.h>
CDatabaseMySQL::CDatabaseMySQL(void)
{
#ifndef WIN
	pthread_mutex_init(&m_Mutex,NULL);
#else
	InitializeCriticalSection(&m_Lock);
#endif
	m_pMySql = NULL;
}


CDatabaseMySQL::~CDatabaseMySQL(void)
{
	mysql_close(m_pMySql);

#ifndef WIN
	pthread_mutex_destroy(&m_Mutex);
#else
	DeleteCriticalSection(&m_Lock);
#endif
}

bool CDatabaseMySQL::InitConnect(CDBParam *pParam)
{
	bool bRet = false;
	if (NULL == pParam)
	{
		return bRet;
	}
	CDBParamMysql * pMysqlParam = (CDBParamMysql *)pParam;
#ifdef WIN
	EnterCriticalSection(&m_Lock);
#else
	pthread_mutex_lock(&m_Mutex);
#endif
	m_pMySql = mysql_init(m_pMySql);
	if (NULL == m_pMySql)
	{
#ifndef WIN
		pthread_mutex_unlock(&m_Mutex);
#else
		LeaveCriticalSection(&m_Lock);
#endif
		return bRet;
	}
	
	my_bool value = true;
	mysql_options(m_pMySql, MYSQL_OPT_RECONNECT, &value);//auto reconnection 

	if (!mysql_real_connect(m_pMySql, pMysqlParam->m_szStrServerIP, pMysqlParam->m_szStrUser,
		pMysqlParam->m_szStrPwd, pMysqlParam->m_szStrDatabase, pMysqlParam->m_Port, NULL, 0))
	{
		m_strError = mysql_error(m_pMySql);
		printf("%s\r\n",m_strError.c_str());
#ifndef WIN
		pthread_mutex_unlock(&m_Mutex);
#else
		LeaveCriticalSection(&m_Lock);
#endif
		return bRet;
	}
	m_bConn = true;
#ifndef WIN
	pthread_mutex_unlock(&m_Mutex);
#else
	LeaveCriticalSection(&m_Lock);
#endif
	bRet = true;
	return bRet;
}

bool CDatabaseMySQL::Query(const char *const pStrSql, fnDBRowDataPtr fnRowData, void* pList)
{
	if (!m_bConn || NULL == fnRowData || NULL == pStrSql || NULL ==pList)
	{
		return false;
	}


#ifdef WIN	
	EnterCriticalSection(&m_Lock);
#else
	pthread_mutex_lock(&m_Mutex);
#endif
	printf("SQL:%s\r\n", pStrSql);
	int nQuery = mysql_real_query(m_pMySql, pStrSql, strlen(pStrSql));
	if (0 != nQuery)
	{
		m_strError = mysql_error(m_pMySql);
		printf("%s\r\n",m_strError.c_str());
#ifndef WIN
		pthread_mutex_unlock(&m_Mutex);
#else
		LeaveCriticalSection(&m_Lock);
#endif
		return false;
	}


	MYSQL_RES *pRes = NULL;
	pRes = mysql_store_result(m_pMySql);
	if (NULL == pRes)
	{
		m_strError = mysql_error(m_pMySql);
		printf("%s\r\n",m_strError.c_str());
#ifndef WIN
		pthread_mutex_unlock(&m_Mutex);
#else
		LeaveCriticalSection(&m_Lock);
#endif
		return false;
	}


	unsigned int  nLength = mysql_num_fields(pRes);
	unsigned long * pLengths = NULL;
	MYSQL_ROW row;
	while (row = mysql_fetch_row(pRes))
	{
		pLengths = mysql_fetch_lengths(pRes);
		fnRowData(nLength, row, pLengths,pList);
	}
//#ifdef WIN
	if (NULL != m_pMySql)
	{
		do
		{
			mysql_free_result(pRes);
		} while (mysql_next_result(m_pMySql));
	}
//#endif
#ifndef WIN
	pthread_mutex_unlock(&m_Mutex);
#else
	LeaveCriticalSection(&m_Lock);
#endif
	return true;
}
int CDatabaseMySQL::Exec(const char * const pStrSql)
{
	if (!m_bConn)
	{
		return 0;
	}


#ifdef WIN	
	EnterCriticalSection(&m_Lock);
#else
	pthread_mutex_lock(&m_Mutex);
#endif
	printf("Sql:%s\r\n", pStrSql);
	int nQuery = mysql_real_query(m_pMySql, pStrSql, strlen(pStrSql));
	if (0 != nQuery)
	{	
		m_strError = mysql_error(m_pMySql);
		printf("%s\r\n", m_strError.c_str());
#ifndef WIN
		pthread_mutex_unlock(&m_Mutex);
#else
		LeaveCriticalSection(&m_Lock);
#endif
		return 0;
	}
#ifndef WIN
	pthread_mutex_unlock(&m_Mutex);
#else
	LeaveCriticalSection(&m_Lock);
#endif	
	return 1;
}
string CDatabaseMySQL::GetLastError()
{
	return m_strError;
}
