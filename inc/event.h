
/**********************************************************************
  Copyright (c), 2015-2016, SkyLight ELECTRONICS(SHENTHEN) Co., Ltd.

  文件名称: event.cpp
  文件描述: epoll连接功能，支持keepalive
  函数列表: CEPHandler::TCPKeepAlive,  CEPHandler::make_socket_non_blocking
	CEPHandler::Init, CEPHandler::AcceptHttpEpoll, CEPHandler::ReadEpoll, 
	CEPHandler::WriteEpoll,CEPHandler::EPRecv, CEPHandler::EPSend,getContentLenth
	CEPHandler::Recv, CEPHandler::doWork, CEPHandler::Send
  修订记录:
         1. 作者: 聂文峰
            日期: 2015-06-19
            内容: 创建文件
**********************************************************************/

#ifndef __EVENT_H__
#define __EVENT_H__

#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <semaphore.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <cstring>
#include <errno.h>
#include <string>
#include <list>
#include <queue>
#include <map>
#include <memory>
#include "common.h"
#include "eventhandle.h"

#define MAX_EVENTS 		50000
#define EPOLL_TIME_OUT 	-1//1000
#define RECV_TIME_OUT	3//防止连接超长接收
#define SEND_TIME_OUT	3//3s
#define MAX_RECVLEN		1500//MTU
#define MAX_SENDLEN		1500

struct SEvents
{
	int		ifd;
	char * 	phead;//接收错误标识
	int 	ilen;//Content-Length
	string	strContent;//接收缓冲
	long	lRecvTime; 

	SEvents(int fd, const char* buf, long time)
	:ifd(fd), phead(NULL), ilen(-1), strContent(buf),lRecvTime(time)
	{

	}
	~SEvents(){};
};

class  CEPHandler
{
public:
	CEPHandler();
	~CEPHandler();

	pthread_mutex_t m_pmtRecv, m_pmtRecved, m_pmtSend, m_pmtRes;
	//pthread_mutex_t m_pmtEp, m_pmtaEp;//epoll几个函数线程安全
	queue<shared_ptr<SEvents>> m_qRecv,m_qRecved,m_qSend;
	map<int , shared_ptr<SEvents>> m_mapRes;

	static void* EPRecv(void * p);//EPOLL_CTL_DEL
	static void* Recv(void * p);
	static void* doWork(void * p);//pop m_qRecv, insert m_mapRes
	static void* EPSend(void * p);//EPOLL_CTL_DEL
	static void* Send(void * p);
	bool Init(short port);
	static int make_socket_non_blocking (int sfd);
	static bool TCPKeepAlive(int fd, int interval); 
private:
	int m_listenFd;
	int m_erfd, m_esfd;
	struct epoll_event ev,m_evRecv[MAX_EVENTS], m_evSend[MAX_EVENTS];

	void AcceptHttpEpoll ();
	void ReadEpoll (int i);
	void WriteEpoll (int i);

};
int getContentLenth(const char *buf);
#endif
