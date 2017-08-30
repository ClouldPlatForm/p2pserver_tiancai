
#include "http_util.h"

bool CHttp::Parse(const char* szHttpReq, int nDataLen, char* szOutBuf, int& nOutLen)
{
	if(!memcmp(szHttpReq, "POST ", 5) == 0)
	{
		return false;
	}
	if((strlen(szHttpReq) >= 1024)||nDataLen >= 1024)
	{
		return false;
	}
	//pHeadEnd定位到xml部分
	char* pHeadEnd = strstr((char*)szHttpReq, "<?xml");//<?xml是标准头，xmlParseMemory中间空白符有都会解析错误
	if(!pHeadEnd)
	{
		return false;
	}

	char* pContentLen = strstr((char*)szHttpReq, "Content-Length:");
	if(pContentLen != NULL)
	{
//		printf("+++++++++++++++++xml\n");
		//提取lenth
		pContentLen += strlen("Content-Length:");
		char* pContentLenEnd = strstr(pContentLen, "\r\n");
		char szBuf[8] = {0};
		memcpy(szBuf, pContentLen, pContentLenEnd - pContentLen);
		if(szBuf[0] > 0)//szBuf不为空
		{
			nDataLen = atoi(szBuf);
			strcpy(szOutBuf, pHeadEnd);
			//memcpy(szOutBuf, pHeadEnd, nDataLen);
			//nOutLen = nDataLen;
       		nOutLen = strlen(pHeadEnd);
		}
		else
		{
			return false;
		}
	}

	//上日志
	/*char szLog[1024] = {0};*/
	//printf("xml:%s, len:%d\n", szOutBuf, nDataLen);
	/*sprintf(szLog, "xml:%s, len:%d", szOutBuf, nDataLen);
	WriteLogINFO(g_log.m_rec, szLog);*/

	return true;
}

string CHttp::Package(const char* szContent, int nDataLen, int nRes, int& nHttpLen)
{
	string strBuf;

	if (RESPFALSE != nRes)
	{
		make_200_success(szContent, nDataLen, strBuf, nHttpLen);
	}
	else
	{
		make_500_error(szContent, nDataLen, strBuf, nHttpLen);
	}
  
	return strBuf;
}


void CHttp::make_400_error(string strContent,  int nlen, string& strResp, int& nHttpLen)
{
	char szHttp[512] = {0};
	sprintf(szHttp, "HTTP/1.1 400 Bad Request\r\nContent-Length: %d\r\nContent-Type: text/xml\r\nConnection: Keep-Alive\r\n\r\n", nlen);
	strResp += szHttp;
	strResp += strContent;
	//nHttpLen = strlen(strResp.c_str());
  nHttpLen = strlen(szHttp) + nlen;
}

void CHttp::make_200_success(string strContent, int nlen, string& strResp, int& nHttpLen)
{
	char szHttp[512] = {0};
	sprintf(szHttp, "HTTP/1.1 200 OK\r\nContent-Length: %d\r\nContent-Type: text/xml\r\nConnection: Keep-Alive\r\n\r\n", nlen);
	strResp += szHttp;
	strResp += strContent;
	//nHttpLen = strlen(strResp.c_str());
  nHttpLen = strlen(szHttp) + nlen;
}

void CHttp::make_500_error(string strContent,  int nlen, string& strResp, int& nHttpLen)
{
	char szHttp[512] = {0};
	sprintf(szHttp, "HTTP/1.1 500 Internal Server Error\r\nContent-Length: %d\r\nContent-Type: text/xml\r\nConnection: Keep-Alive\r\n\r\n", nlen);
	strResp += szHttp;
	strResp += strContent;
	nHttpLen = strlen(szHttp) + nlen;
}
