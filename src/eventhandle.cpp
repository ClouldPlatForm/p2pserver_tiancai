
#include "eventhandle.h"
/*************************************************************************
 函数名称: CEventHandle::SubEvent
 功能说明: 主业务处理程序，输入数据处理：拆包，解析，业务处理，生成响应数据
 输入参数: szbuf 输入字符串，接收到的数据
 		    ilen 输入字符串长度
 输出参数: szOut 输出字符串，响应到的数据
 		    iOutLen 输出字符串长度
 返 回 值: true/false
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
bool CEventHandle::SubEvent(const char* szbuf,const int ilen, char* szOut, int &iOutLen)
{
	int nRXmlLen = 0, nXmlLen = 0;
	int nRes = RESPFALSE;//1 成功 or 0 失败标志
	char szTmpBuf[MAXCONTENT] = {0};//挖出的xml放在szTmpBuf中
	char szLog[MAXCONTENT] = {0};
	string strLog;
	SINPUTINFO sIPC, sINFO;
	CHttp oHttp;
	CXMLParse oXML;
	CHandleLogic oLogic;
	string strHttpBuf;
	
	do{
		//解析包
		if(!oHttp.Parse(szbuf, ilen, szTmpBuf, nXmlLen))
		{
			nRes = RESPFALSE;//reps 500
			WriteLogERROR(g_log.m_rec, "Parse http error");
			break;
		}
		else
		{
			snprintf(szLog, MAXCONTENT, "xmllen:%d, xmlbuf:%s", nXmlLen, szTmpBuf);
			WriteLogINFO(g_log.m_rec, szLog);
		}
			
		//parse xml//memset(szLog, 0, MAXCONTENT);
		if (!oXML.Analysis(szTmpBuf, nXmlLen, sIPC))
		{
			nRes = RESPFALSE;//reps 500
			sprintf(szLog, "Analysis xml error");
			WriteLogERROR(g_log.m_rec, szLog);
			break;			
		}	
		else
		{
			sprintf(szLog, "Analysis-out sn: %s; map:%s:%d, %d, %d;local: %s:%d, %d, %d", 
					sIPC.strsn.c_str(), sIPC.straddr.c_str(), sIPC.mphttp, sIPC.mprtsp, sIPC.mprtp
					,sIPC.straddr_pri.c_str(), sIPC.mphttp_pri, sIPC.mprtsp_pri, sIPC.mprtp_pri);
			WriteLogINFO(g_log.m_rec, szLog);
		}
	
		//分析业务
		if(IPCREGSIG == sIPC.flag)// 注册
		{	

			if (oLogic.RegisterByCallFN(sIPC))
			{
				nRes = RESPTRUE;
				sprintf(szLog, "Register-out sn: %s; map:%s:%d, %d, %d;local: %s:%d, %d, %d; random:%s", 
				sIPC.strsn.c_str(), sIPC.straddr.c_str(), sIPC.mphttp, sIPC.mprtsp, sIPC.mprtp
				,sIPC.straddr_pri.c_str(), sIPC.mphttp_pri, sIPC.mprtsp_pri, sIPC.mprtp_pri,sIPC.strrandom.c_str());
				WriteLogINFO(g_log.m_rec, szLog);	
			}
			else
			{
				nRes = RESPFALSE;
				sprintf(szLog, "Register-out sn: %s; map:%s:%d, %d, %d;local: %s:%d, %d, %d; random:%s", 
				sIPC.strsn.c_str(), sIPC.straddr.c_str(), sIPC.mphttp, sIPC.mprtsp, sIPC.mprtp
				,sIPC.straddr_pri.c_str(), sIPC.mphttp_pri, sIPC.mprtsp_pri, sIPC.mprtp_pri,sIPC.strrandom.c_str());
				WriteLogERROR(g_log.m_rec, szLog);
				break;
			}
		}
		else if (APPREQSIG == sIPC.flag)//APP查询信号
		{
			nRes = RESPFALSE;
			break;
			//check();
		}
		else if(IPCCHKSIG == sIPC.flag)//验证信号
		{
			nRes = RESPFALSE;
			break;
		}
		else
		{
			nRes = RESPFALSE;
			break;
			//不在业务内
		}
	
	}while(0);
	
	//准备反馈信息，在szTmpBuf中
	{
		//pack xml
		memset(szTmpBuf, 0, MAXCONTENT);
		nRXmlLen = oXML.Package(nRes, sINFO, szTmpBuf);//xml打包发送数据
		if(-1 == nRXmlLen) 
		{
			iOutLen = 0;
			return false;
		}
			
		//Package http
		strHttpBuf = oHttp.Package(szTmpBuf, nRXmlLen, nRes, iOutLen);//http打包发送数据

		/*sprintf(szLog, "httplen:%d oHttp.Package-out \n%s", iOutLen, strHttpBuf.c_str());
		WriteLogINFO(g_log.m_rec, szLog);*/
	}

	memcpy(szOut, strHttpBuf.c_str(), iOutLen);
	
	return true;
}
