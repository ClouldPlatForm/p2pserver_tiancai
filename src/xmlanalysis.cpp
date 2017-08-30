
#include "xmlanalysis.h"
/*************************************************************************
 函数名称: CXMLParse::TinyPackage
 功能说明: 响应打包函数
 输入参数: flag 状态标识
 			sINFO flag为RESPINFO时，sINFO为输出的IPC设备upnp信息
 输出参数: szBuf 输出字符串
 返 回 值: >0：szBuf长度 成功, -1: 失败
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
int CXMLParse::TinyPackage(int flag, SINPUTINFO &sINFO, char* szBuf)
{
	SResponse sRes = {"", ""};
	XMLDocument *pDoc = new XMLDocument();
    if (NULL==pDoc) 
    {
        return -1;
    }
	
	int buffersize = 0;
	XMLDeclaration *declare = pDoc->NewDeclaration("xml version=\"1.0\" encoding=\"UTF-8\"");
	pDoc->LinkEndChild(declare);

	
	if(RESPINFO != flag)
	{
		if(RESPTRUE == flag)
		{
			sRes.strstatusCode = "0";
			sRes.strstatusString = "OK";
		}
		else
		{
			sRes.strstatusCode = "2";
			sRes.strstatusString = "NOK";
		}
		XMLElement *root = pDoc->NewElement("ResponseStatus");
		if (root == NULL)
		{
			return -1;//ÎÞ·¨ÏìÓ¦
		}
		pDoc->LinkEndChild(root);
		XMLElement *child = pDoc->NewElement("statusCode");
		child->SetText(sRes.strstatusCode.c_str());
		root->LinkEndChild(child);
		/*child = new XMLElement(pDoc);//error XMLElement(tinyxml2::XMLDocument*)’ is private
		child->SetName("statusString");*/
		child = pDoc->NewElement("statusString");
		child->SetText(sRes.strstatusString.c_str());
		root->LinkEndChild(child);
	}
	

	XMLPrinter xpTmp;
	pDoc->Print(&xpTmp);
	buffersize = xpTmp.CStrSize() - 1;//不带末尾0
	/*char szLog[512] = {0};
	sprintf(szLog, "buffersize:%d, xpTmp:%s", buffersize, xpTmp.CStr());
	WriteLogINFO(g_log.m_rec, szLog);*/
	
	memcpy(szBuf, xpTmp.CStr(), buffersize);
	
	if(pDoc) delete pDoc;//nie:only needed delete the document, every linked elements will be clear.
		
	return buffersize;
}
/*************************************************************************
 函数名称: CXMLParse::TinyAnalysis
 功能说明: 响应打包函数
 输入参数: szbuf 输入字符串
 			ilen 输入字符串长度
 输出参数: szbuf 解析得到的IPC设备upnp信息
 返 回 值: true/false
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
bool CXMLParse::TinyAnalysis(char* szbuf, int ilen, SINPUTINFO& IPC)
{
	int ret = 0;
	const char * texttmp = 0;
	XMLDocument *pDoc = new XMLDocument();
	XMLElement *Root = NULL;
    if (NULL==pDoc) 
    {
       	WriteLogERROR(g_log.m_rec, "doc == null");
		ret = -1;
		goto _out;
    }
	if(0 != pDoc->Parse(szbuf, ilen))
	{
		char tmp[64] = {0};
		snprintf(tmp, sizeof(tmp), "XML Parse failed, xml-len:%d", ilen);
		WriteLogERROR(g_log.m_rec, tmp);
		ret = -1;
		goto _out;
	}

	Root = pDoc->FirstChildElement("P2Pinfo");
	if (Root)//
	{
		IPC.flag = IPCREGSIG;

		for(XMLElement *Node=Root->FirstChildElement(); Node; Node = Node->NextSiblingElement())
		{
			if(XMLUtil::StringEqual(Node->Name(), "SN"))
			{
				if((texttmp = Node->GetText()))	IPC.strsn = texttmp;			
			}
			else if(XMLUtil::StringEqual(Node->Name(), "NAT"))
			{
				for(XMLElement *Node2=Node->FirstChildElement(); Node2; Node2 = Node2->NextSiblingElement())
				{
									
					if (XMLUtil::StringEqual(Node2->Name(), "uPNP"))
					{
						if (!CXMLParse::TinyAna_uPNP(Node2->FirstChildElement(), IPC))
						{
							
							ret = -1;
							goto _out;
						}
					}
					else if (XMLUtil::StringEqual(Node2->Name(), "MODE"))
					{
						if(!XMLUtil::StringEqual(Node2->GetText(),"uPNP")) 
						{
							WriteLogERROR(g_log.m_rec, "MODE no match");
							ret = -1;
							goto _out;
						}
							
					}
					else if (XMLUtil::StringEqual(Node2->Name(), "STUN"))
					{
						//非所需信息
					}
					else
					{
						WriteLogERROR(g_log.m_rec, "unknow element");
						ret = -1;
						goto _out;
					}				
				}		
			}
			else if(XMLUtil::StringEqual(Node->Name(), "random"))
			{
				if((texttmp = Node->GetText()))	IPC.strrandom = texttmp;	
			}
			else
			{
				WriteLogERROR(g_log.m_rec, "unknow element");
				ret = -1;
				goto _out;
			}
		}
	}
	else 
	{
		WriteLogERROR(g_log.m_rec, "P2Pinfo do not exist");
		ret = -1;
		goto _out;
	}

_out:
	if(pDoc) delete pDoc;
	if (0 == ret)
		return true;
	else
		return false;
}
//upnp部分xml解析
bool CXMLParse::TinyAna_uPNP(XMLElement *Node, SINPUTINFO& IPC)
{
  	//int ret = 0; 
  	const char * texttmp = 0;
	for(; Node; Node = Node->NextSiblingElement())
	{
		if (XMLUtil::StringEqual(Node->Name(), "Mapping"))
		{
			if (Node->Attribute("name", "HTTP"))
			{
				//sstream sstmp(texttmp);
				if((texttmp = Node->GetText()))	IPC.mphttp = atoi(texttmp);
			}
			else if (Node->Attribute("name", "RTSP"))
			{
				if((texttmp = Node->GetText()))	IPC.mprtsp = atoi(texttmp);
			}
			else if (Node->Attribute("name", "RTP"))
			{
				if((texttmp = Node->GetText()))	IPC.mprtp = atoi(texttmp);
			}
			else
			{
				WriteLogERROR(g_log.m_rec, "unknow element");
				return false;
			}
		}
		else if (XMLUtil::StringEqual(Node->Name(), "MappedIP"))
		{
			if((texttmp = Node->GetText()))	IPC.straddr = texttmp;
		}
		else if (XMLUtil::StringEqual(Node->Name(), "private"))
		{
			for (XMLElement *Node2 = Node->FirstChildElement(); Node2; Node2 = Node2->NextSiblingElement())
			{
      			if (XMLUtil::StringEqual(Node2->Name(), "Mapping"))
				{
					if (Node2->Attribute("name", "HTTP"))
					{
						if((texttmp = Node2->GetText()))	IPC.mphttp_pri = atoi(texttmp);
					}
					else if (Node2->Attribute("name", "RTSP"))
					{
						if((texttmp = Node2->GetText()))	IPC.mprtsp_pri = atoi(texttmp);
					}
					else if (Node2->Attribute("name", "RTP"))
					{
						if((texttmp = Node2->GetText()))	IPC.mprtp_pri = atoi(texttmp);
					}
					else
					{
						WriteLogERROR(g_log.m_rec, "unknow element");
						return false;
					}

				}
				else if (XMLUtil::StringEqual(Node2->Name(), "MappedIP"))
				{
					if((texttmp = Node2->GetText()))
					{
						IPC.straddr_pri = texttmp;
					}	
				}
				else
				{
					WriteLogERROR(g_log.m_rec, "unknow element");
					return false;
				}
			}
			
		}
		else
		{
			WriteLogERROR(g_log.m_rec, "unknow element");
			return false;
		}
	}

	return true;
}


