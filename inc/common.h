/**********************************************************************
  Copyright (c), 2015-2016, SkyLight ELECTRONICS(SHENTHEN) Co., Ltd.

  文件名称: main.cpp
  文件描述: 实现守护程序，初始化数据库连接池，初始化线程池，
        初始化日志，
  函数列表: daemon; main
  修订记录:
         1. 作者: 聂文峰
            日期: 2015-06-19
            内容: 创建文件
**********************************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

#include <string>
#include <sys/time.h>
#include "log.h"

using namespace std;

extern class CLog g_log;
extern class CConfig g_config;
extern class CEPHandler g_ephandler;
extern class CDBPool g_dbpool;

#define IPCREGSIG	1
#define APPREQSIG	2
#define IPCCHKSIG	3

struct SINPUTINFO
{
	string strsn;
  string straddr;
  string straddr_pri;
  string strrandom;
  int    flag;//1 - ipcreg, 2 - appreq, 3 - ipcchk，实际只实现了1-ipcreg
  int    user_id;
  int    mphttp;
  int    mprtsp;
  int    mprtp;
  int    mphttp_pri;
  int    mprtsp_pri;
  int    mprtp_pri;

public:
  SINPUTINFO()
  :strsn(""),straddr(""),straddr_pri(""),
  strrandom(""),flag(0),user_id(0),mphttp(0),
  mprtsp(0),mprtp(0),mphttp_pri(0),mprtsp_pri(0),
  mprtp_pri(0)
  {

  }    
};

struct SResponse
{
   string strstatusCode;
   string strstatusString;
};



#endif
