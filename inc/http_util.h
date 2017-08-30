/**********************************************************************
  Copyright (c), 2015-2016, SkyLight ELECTRONICS(SHENTHEN) Co., Ltd.

  文件名称: http_util.cpp
  文件描述: http协议解析模块
  函数列表: CHttp::Parse, CHttp::Package, CHttp::make_400_error, 
  CHttp::make_200_success, CHttp::make_500_error
  修订记录:
         1. 作者: 聂文峰
            日期: 2015-06-19
            内容: 创建文件
**********************************************************************/


#ifndef __HTTP_UTIL_H__
#define __HTTP_UTIL_H__

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <string>
#include "common.h"

using namespace std;

class CHttp
{

public:

	CHttp()
	{
		memset(m_szContent, 0, 256);
	}


public:

	bool Parse(const char* szHttpReq, int nDataLen, char* szOutBuf, int& nOutLen);

	string Package(const char* szHttpReq, int nDataLen, int nRes, int& nHttpLen);

	void make_400_error(string strContent,  int nlen, string& strResp, int& nHttpLen);

	void make_200_success(string strContent,  int nlen, string& strResp, int& nHttpLen);

	void make_500_error(string strContent,  int nlen, string& strResp, int& nHttpLen);



private:

	char m_szContent[256];

};


#endif
