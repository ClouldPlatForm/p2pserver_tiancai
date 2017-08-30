/**********************************************************************
  Copyright (c), 2015-2016, SkyLight ELECTRONICS(SHENTHEN) Co., Ltd.

  文件名称: handlelogic.cpp
  文件描述: 注册业务处理功能
  函数列表: CHandleLogic::CheckIPCINFO, checkID, CHandleLogic::RegisterByCallFN 
  修订记录:
         1. 作者: 聂文峰
            日期: 2015-06-19
            内容: 创建文件
**********************************************************************/

#ifndef __HANGLELOGIC_H__
#define __HANGLELOGIC_H__

#include <unistd.h>
#include <iostream>
#include <mysql/mysql.h>
#include <string>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include "config.h"
#include "common.h"
#include "log.h"
#include "DBPool.h"

using namespace std;

//ipcreq 相关msyql操作
#define SETCODEPAGE 	0
#define SIPCAMERA		1
#define SP2PIP			2
#define IP2PIP			3
#define UP2PIP			4
#define DMAPPORT		5
#define IMAPPORT		6
#define UMAPPORT		7

//t_platform_mapping里的port_type定义
#define PTYPEHTTP		"HTTP"//0
#define PTYPERTSP		"RTSP"//1
#define PTYPERTP		"RTP"//2
//t_platform_mapping里的socket_type定义
#define STYPETCP		"TCP"//0
#define STYPEUDP		"UDP"//1
#define STYPEHTTP		"HTTP"//2
//CHandleLogic::Register中查询结果数组下标
#define T_ipcamera				0
#define T_p2p					1
#define T_mapping_http			2
/*#define T_mapping_http		2
#define T_mapping_rtsp			3
#define T_mapping_rtp			4*/

//define for CheckIPCINFO
#define IPCSNMAX 			32
#define IPCSRANDOMMAX		32
#define P2PIPMAX			32
#define MAPPORTMAX 			65535


class CHandleLogic
{

public:

	//bool Register(SINPUTINFO& sIPC);
	bool CheckIPCINFO(SINPUTINFO& sIPC);
	
//	bool Heartbeat(SINPUTINFO& sIPC);
	bool RegisterByCallFN(SINPUTINFO& sIPC);
private:
	//char* getDateTime(void);


};

#endif
