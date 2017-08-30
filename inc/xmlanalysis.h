/**********************************************************************
  Copyright (c), 2015-2016, SkyLight ELECTRONICS(SHENTHEN) Co., Ltd.

  文件名称: xmlanalysis.cpp
  文件描述: xml数据解析与打包
  函数列表: CXMLParse::TinyAna_uPNP, CXMLParse::TinyAnalysis, CXMLParse::TinyPackage
  修订记录:
         1. 作者: 聂文峰
            日期: 2015-06-19
            内容: 创建文件
**********************************************************************/


#ifndef __XML_ANALYSIS_H__
#define __XML_ANALYSIS_H__


#include <stdio.h>
#include <string.h>
#include "common.h"
#include "tinyxml2.h"
#include "config.h"
#include "handlelogic.h"

using namespace tinyxml2;


class CXMLParse
{

public:

	inline int Package(int flag, SINPUTINFO &sINFO, char* szBuf)
	{
		//default tinyxml2
		return TinyPackage(flag, sINFO, szBuf);
	}

	inline bool Analysis(char* szbuf, int ilen, SINPUTINFO& IPC)
	{
		//default tiny
		return TinyAnalysis(szbuf, ilen, IPC);
	}
private:
	
	int TinyPackage(int flag, SINPUTINFO &sINFO, char* szBuf);
	bool TinyAnalysis(char* szbuf, int ilen, SINPUTINFO& IPC);
	bool TinyAna_uPNP(XMLElement *Node, SINPUTINFO& IPC);

};


#endif
