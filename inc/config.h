/**********************************************************************
  Copyright (c), 2015-2016, SkyLight ELECTRONICS(SHENTHEN) Co., Ltd.

  文件名称: config.cpp
  文件描述: 读取服务器配置文件，
  函数列表: CConfig::CheckConfig; CConfig::TinyReadConfigXML
  修订记录:
         1. 作者: 聂文峰
            日期: 2015-06-19
            内容: 创建文件
**********************************************************************/
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <string>
#include "tinyxml2.h"
#include "common.h"

#define CONFIG  "../config/config.xml"

using namespace std;
using namespace tinyxml2;

class CConfig
{
public:
	string strAddr;//host
	string strUser;//
	string strPwd;
	string strDbname;//
	int nPort;
	int nListenPort;
	int loglevel;

	CConfig()
	:strAddr(""),strUser(""),strPwd("")
	,strDbname(""),nPort(0),nListenPort(0),loglevel(0)
	{

	}

	inline bool ReadConfigXML()
	{
		return TinyReadConfigXML();//default use tiny
	}

private:
	
	bool CheckConfig(void);
	bool TinyReadConfigXML();
};


#endif