#include "DatabaseFactory.h"
#include "DatabaseMySQL.h"

CDatabaseFactory::CDatabaseFactory()
{
	m_pDataBase = NULL;
}


CDatabaseFactory::~CDatabaseFactory()
{
	if (NULL != m_pDataBase)
	{
		delete  m_pDataBase;
		m_pDataBase = NULL;
	}
}
bool CDatabaseFactory::SelectDb(DBTYPE DbType)
{
	
	switch(DbType)
	{
	case DBTYPE_MYSQL:
		{
			m_pDataBase = new CDatabaseMySQL();
			if (NULL == m_pDataBase)
			{
				delete m_pDataBase;
				m_pDataBase = NULL;
				return false;
			}
		}
		break;
	default:
		{
			m_pDataBase = new CDatabaseMySQL();
			if (NULL == m_pDataBase)
			{
				delete m_pDataBase;
				m_pDataBase = NULL;
				return false;
			}
		}
		break;
	}
	return true;
}
