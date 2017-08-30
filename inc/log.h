/**********************************************************************
  Copyright (c), 2015-2016, SkyLight ELECTRONICS(SHENTHEN) Co., Ltd.

  文件名称: log.cpp
  文件描述: 日志模块
  函数列表: CLog::Init，CLog::ChangeLogLevel
  修订记录:
         1. 作者: 聂文峰
            日期: 2015-06-19
            内容: 创建文件
**********************************************************************/

#ifndef __LOG_H__
#define __LOG_H__

#include <log4cplus/logger.h>
#include <log4cplus/configurator.h>
#include <iomanip>
#include <log4cplus/logger.h>
#include <log4cplus/fileappender.h>
#include <log4cplus/consoleappender.h>
#include <log4cplus/layout.h>
#include <time.h>
#include <string.h>
#include <log4cplus/loggingmacros.h>

#define TRACE 	0
#define DEBUG 	1
#define INFO 	2
#define WARN 	3
#define ERROR 	4
#define FATAL 	5
#define OFF 	6

#define RESPFALSE		0
#define RESPTRUE		1
#define RESPINFO		2//appreq反馈


using namespace std;
using namespace log4cplus;

#define WriteLogTRACE 	LOG4CPLUS_TRACE
#define WriteLogDEBUG 	LOG4CPLUS_DEBUG
#define WriteLogINFO 	LOG4CPLUS_INFO
#define WriteLogWARN 	LOG4CPLUS_WARN
#define WriteLogERROR	LOG4CPLUS_ERROR
#define WriteLogFATAL 	LOG4CPLUS_FATAL

//extern Logger m_pTestLogger;

class CLog
{

public:

	CLog(){};

	virtual ~CLog(){};

public:
	Logger m_rec;

	bool Init(const char * path);

	void ChangeLogLevel(int loglevel);

	//void WriteLog(int szlevel, const char* szInfo);

};


#endif