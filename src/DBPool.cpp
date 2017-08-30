#include "DBPool.h"
using namespace std;

CDBPool::CDBPool()
{

}


/*************************************************************************
 函数名称: CDBPool::InitDBPool
 功能说明: 初始化数据库连接池
 输入参数: m_pDBParam 连接相关数据库的配置信息
 			DbType 数据库类型：当前为MySQL
 			maxSize 数据库连接池的连接上限
 输出参数: 无
 返 回 值: true/false 
 修订记录:
          1.作者: 聂志辉
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
bool CDBPool::InitDBPool(CDBParam * pDBParam, DBTYPE DbType, int maxSize)
{
	if (!m_CriticalSectionFactory.CreateProduct())
	{
		return false;
	}
	m_maxSize = maxSize;
	m_curSize = 0;
	m_pDBParam = pDBParam;
	m_DbType = DbType;
	if (!InitConnection(maxSize / 2))
	{
		return false;
	}
	
	return true;
}


//初始化连接池，创建最大连接数的一半连接数量
bool CDBPool::InitConnection(int iInitialSize)
{
	CDatabaseFactory*pConn = NULL;
	m_CriticalSectionFactory.m_pCS->Lock();
	for (int i = 0; i < iInitialSize; i++)
	{
		pConn = CreateConnection();
		if (pConn)
		{
			m_connList.push_back(pConn);
			++(m_curSize);
		}
		else
		{
			m_strError = "Init Connection Failde!";
			m_CriticalSectionFactory.m_pCS->UnLock();
			return false;
		}
	}
	m_CriticalSectionFactory.m_pCS->UnLock();
	return true;
}

//创建连接,返回一个Connection
CDatabaseFactory* CDBPool::CreateConnection()
{
	CDatabaseFactory*pConn = NULL;
	pConn = new CDatabaseFactory();
	if (NULL == pConn)
	{
		return NULL;
	}
	pConn->SelectDb(m_DbType);
	if (NULL == pConn->m_pDataBase)
	{
		delete pConn;
		pConn = NULL;
		return NULL;
	}
	if (!pConn->m_pDataBase->InitConnect(m_pDBParam))
	{
		delete pConn;
		pConn = NULL;
	}
	return pConn;
}

//在连接池中获得一个连接
CDatabaseFactory*CDBPool::GetConnection()
{
	CDatabaseFactory*pCon = NULL;
	m_CriticalSectionFactory.m_pCS->Lock();

	if (m_connList.size() > 0)
	{   //连接池容器中还有连接
		pCon = m_connList.front(); //得到第一个连接
		m_connList.pop_front();   //移除第一个连接		
		//如果连接为空，则创建连接出错
		if (pCon == NULL)
		{
			--m_curSize;
		}
		m_CriticalSectionFactory.m_pCS->UnLock();
		return pCon;
	}
	else
	{
		if (m_curSize < m_maxSize)
		{ //还可以创建新的连接
			pCon = CreateConnection();
			if (pCon)
			{
				++m_curSize;
				m_CriticalSectionFactory.m_pCS->UnLock();
				return pCon;
			}
			else
			{
				m_CriticalSectionFactory.m_pCS->UnLock();
				return NULL;
			}
		}
		else
		{
			//建立的连接数已经达到maxSize
			m_CriticalSectionFactory.m_pCS->UnLock();
			return NULL;
		}
	}
}

//回收数据库连接
void CDBPool::ReleaseConnection(CDatabaseFactory * pConn)
{
	if (NULL != pConn)
	{
		m_CriticalSectionFactory.m_pCS->Lock();
		m_connList.push_back(pConn);
		m_CriticalSectionFactory.m_pCS->UnLock();
	}
}

//连接池的析构函数
CDBPool::~CDBPool()
{
	DestoryConnPool();
}

//销毁连接池,首先要先销毁连接池的中连接
void CDBPool::DestoryConnPool()
{
	list<CDatabaseFactory*>::iterator icon;
	if(m_CriticalSectionFactory.m_pCS)
	{
		m_CriticalSectionFactory.m_pCS->Lock();
		for (icon = m_connList.begin(); icon != m_connList.end(); ++icon)
		{
			DestoryConnection(*icon); //销毁连接池中的连接
		}
		m_curSize = 0;
		m_connList.clear(); //清空连接池中的连接
		m_CriticalSectionFactory.m_pCS->UnLock();
	}
	
}

//销毁一个连接
void CDBPool::DestoryConnection(CDatabaseFactory* pConn)
{
	if (pConn)
	{
		delete pConn;
		pConn = NULL;
	}
}
