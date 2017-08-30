/**********************************************************************
  Copyright (c), 2015-2016, SkyLight ELECTRONICS(SHENTHEN) Co., Ltd.

  文件名称: eventhandle.cpp
  文件描述: 业务处理功能:拆包，解析，业务处理，生成响应数据
  函数列表: CEventHandle::SubEvent
  修订记录:
         1. 作者: 聂文峰
            日期: 2015-06-19
            内容: 创建文件
**********************************************************************/
#ifndef __EVENTTHANDLE_H__
#define __EVENTTHANDLE_H__
#include <string>
#include <unistd.h>
#include "common.h"
#include "http_util.h"
#include "xmlanalysis.h"
#include "handlelogic.h"

#define MAXCONTENT 1024

class CEventHandle
{

public:

	bool SubEvent(const char* szbuf, const int ilen, char* szOut, int &iOutLen);

private:

	CXMLParse* m_pXml;

	CHandleLogic* m_pLogic;

};

#endif
