/**********************************************************************
  Copyright (c), 2015-2016, SkyLight ELECTRONICS(SHENTHEN) Co., Ltd.

  文件名称: DBPool.cpp
  文件描述: 数据库连接池
  函数列表: InitDBPool；GetConnection；ReleaseConnection；CreateConnection；
  			InitConnection；DestoryConnection；DestoryConnPool
  修订记录:
         1. 作者: 聂志辉
            日期: 2015-06-19
            内容: 创建文件
**********************************************************************/
#ifndef __DBPool_H__
#define __DBPool_H__

#include "CDatabase/DatabaseFactory.h"
#include "CriticalSession/CriticalSectionFactory.h"
#include "CDatabase/DBParam.h"
#include "CDatabase/DBParamMysql.h"
#include <string>
#include <list>

class CDBPool
{
public:
	CDBPool();
	~CDBPool();
	bool InitDBPool(CDBParam * pDBParam,DBTYPE DbType, int maxSize);	
	CDatabaseFactory*GetConnection(); //获得数据库连接
	void ReleaseConnection(CDatabaseFactory *pconnFactory); //将数据库连接放回到连接池的容器中
	string m_strError;
	
private:
	int m_curSize; //当前已建立的数据库连接数量
	int m_maxSize; //连接池中定义的最大数据库连接数		
	list<CDatabaseFactory*> m_connList; //连接池的容器队列
	//CCriticalSectionFactory m_lock; //线程锁
	CDatabaseFactory*CreateConnection(); //创建一个连接
	bool InitConnection(int iInitialSize); //初始化数据库连接池
	void DestoryConnection(CDatabaseFactory *pconnFactory); //销毁数据库连接对象
	void DestoryConnPool(); //销毁数据库连接池
	CDBParam* m_pDBParam;
	CCriticalSectionFactory m_CriticalSectionFactory;
	DBTYPE m_DbType;
};



#endif
