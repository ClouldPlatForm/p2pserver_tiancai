
#include "config.h"

/*************************************************************************
 函数名称: CConfig::CheckConfig
 功能说明: 检查读取的配置正确性
 输入参数: 无
 输出参数: 无
 返 回 值: true/false 
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
bool CConfig::CheckConfig(void)
{
	if(strAddr.empty()||strUser.empty()
		||strPwd.empty()||strDbname.empty())
	{
		return false;
	}
	if(strAddr.size() > 16||strUser.size() > 16
		||strPwd.size() > 16||strDbname.size() > 16)
	{
		return false;
	}
	if((nPort <= 0)||(nListenPort <= 0)
		||(nListenPort > 65535)||(nListenPort > 65535))
	{
		return false;
	}
	return true;
}
/*************************************************************************
 函数名称: CConfig::TinyReadConfigXML
 功能说明: 使用tinyxml2读取的配置文件，并填充CConfig成员
 输入参数: 无
 输出参数: 无
 返 回 值: true/false 
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
bool CConfig::TinyReadConfigXML()
{

	int ret = 0;
	XMLDocument *pDoc = new XMLDocument();
	XMLElement *Root = NULL;
    pDoc->LoadFile(CONFIG);

	Root = pDoc->FirstChildElement("CONFIG");
	if (Root)//
	{
		for(XMLElement *Node = Root->FirstChildElement();Node;Node= Node->NextSiblingElement())
		{
			if(XMLUtil::StringEqual(Node->Name(), "DB_IP"))
			{
				strAddr =  Node->GetText();
			}
			if(XMLUtil::StringEqual(Node->Name(), "DB_PORT"))
			{
				nPort =  atoi(Node->GetText());
			}
			if(XMLUtil::StringEqual(Node->Name(), "DB_USER"))
			{
				strUser =  Node->GetText();
			}
			if(XMLUtil::StringEqual(Node->Name(), "DB_PASSWORD"))
			{
				strPwd =  Node->GetText();
			}
			if(XMLUtil::StringEqual(Node->Name(), "DB_NAME"))
			{
				strDbname =  Node->GetText();
			}
			if(XMLUtil::StringEqual(Node->Name(), "P2P_PORT"))
			{
				nListenPort =  atoi(Node->GetText());
			}
			if(XMLUtil::StringEqual(Node->Name(), "LOG_LEVEL"))
			{
				loglevel =  atoi(Node->GetText());
			}
		}
		if(!CheckConfig())
		{
			ret = 1;
		}
	}
	else 
	{
		WriteLogERROR(g_log.m_rec, "TinyReadConfigXML failed");
		ret = 1;
	}

	if(pDoc) delete pDoc;
	if (0 == ret)
	{
		char szLog[512] = {0};
		sprintf(szLog, "DB DESCRIBE %s, %s, %s,%s,%d,%d", strAddr.c_str(), 
			strUser.c_str(), strPwd.c_str(), strDbname.c_str(), 
			nPort, nListenPort);
		WriteLogINFO(g_log.m_rec, szLog);
		return true;
	}	
	else
	{
		return false;
	}	
}

