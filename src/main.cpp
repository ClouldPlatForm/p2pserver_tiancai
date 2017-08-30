#include <iostream>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <sys/resource.h>
#include <sys/time.h>
#include "thread.h"
#include "common.h"
#include "xmlanalysis.h"
#include "handlelogic.h"
#include "eventhandle.h"
#include "config.h"
#include "event.h"
#include "DBPool.h"

#define INITCONNNUM 500//数据库连接池最大连接数
#define LOG_PATH "../log/upnpserver_log.log"
#define  __P2PSERVER_VERSION__  "p2pserver_v1.0.00.008"

using namespace std;

CLog g_log;//全局日志
CConfig g_config;//全局服务器配置
CEPHandler g_ephandler;//全局业务处理
CDBPool g_dbpool;//全局数据库连接池

/*************************************************************************
 函数名称: daemon
 功能说明: 实现进程守护化
 输入参数: nochdir         切换当前目录标志，0切换，非0不切换
           noclose         关闭文件标志，0:0、1、2>>/dev/null,1:不关闭
 输出参数: 无
 返 回 值: 0/-1 0成功，-1失败
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/

int daemon( int nochdir, int noclose )
{
	pid_t pid;

	if ( !nochdir && chdir(".") != 0 )
	{
		printf("chdir failed \n");
		return -1;
	}

	if ( !noclose )
	{
		int fd = open("/dev/null", O_RDWR);

		if ( fd < 0 )
		{
			return -1;
		}
		//dup2,将stdin,stdout,stderr指向/dev/NULL
		if ( dup2( fd, 0 ) < 0 || dup2( fd, 1 ) < 0 || dup2( fd, 2 ) < 0 ) 
		{
			close(fd);
			return -1;
		}
		close(fd);
	}

	pid = fork();
	if (pid < 0)
	{
		printf("fork failed \n");
		return -1;
	}

	if (pid > 0)
	{
		printf("fork father \n");
		_exit(0);//父进程退出
	}


	if ( setsid() < 0 )//建立新会话
	{
		printf("setsid failed \n");
		return -1;
	}

	return 0;//成功创建daemon子进程[luther.gliethttp].
}

/*************************************************************************
 函数名称: main
 功能说明: 主程序，初始化并启动程序，若带“-v”参数则返回软件版本号
 输入参数: argc 参数个数
 		    argv 参数指针
 输出参数: 无
 返 回 值: 0/-1 0成功，-1失败
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
int main(int argc, char** argv)
{
	//提供版本查询功能
	
	if (argv[1] != NULL)
	{
		//测试全部
		if (!strcmp(argv[1], "-v"))
		{
			printf("%s\n", __P2PSERVER_VERSION__); 
		}

		return 0;
	}
	//设置系统资源限制
	{
		struct rlimit r_limit;
		r_limit.rlim_cur = 50000;
		r_limit.rlim_max = 60000;//open files limit num
		if(-1 == setrlimit(RLIMIT_NOFILE, &r_limit))
		{
			
			perror("setrlimit RLIMIT_NOFILE failed:");
			printf("exit\n");
			return 0;
		}
		r_limit.rlim_cur = 1<<23;//8M
		r_limit.rlim_max = 1<<24;//16M, 栈大小
		if(-1 == setrlimit(RLIMIT_STACK, &r_limit))
		{
			perror("setrlimit RLIMIT_STACK failed:");
			return 0;
		}
		//getrlimit(RLIMIT_NOFILE, &r_limit);
		//printf("%d;%d=======", r_limit.rlim_cur, r_limit.rlim_max);
	}

	//初始化日志
	if (!g_log.Init(LOG_PATH))
	{
		cout<< "log init failed"<<endl;
		return 0;
	}

	//使用ReadConfigXML初始化g_config
	if(!g_config.ReadConfigXML())
	{
		printf("%s\n", "ReadConfigXML failed");
		WriteLogFATAL(g_log.m_rec, "ReadConfigXML failed");
		return 0;
	}

	//设置log级别
	g_log.ChangeLogLevel(g_config.loglevel);

	//初始化epoll模型
	if(!g_ephandler.Init(g_config.nListenPort))
    {
    	printf("%s\n", "Init failed!");
        WriteLogFATAL(g_log.m_rec, "Init failed!");
        return false;
    }
    
    //初始化数据库连接池
    
    CDBParamMysql db;
    strncpy(db.m_szStrServerIP, g_config.strAddr.c_str(), 
    	sizeof(db.m_szStrServerIP));
    strncpy(db.m_szStrUser, g_config.strUser.c_str(), 
    	sizeof(db.m_szStrUser));
    strncpy(db.m_szStrPwd, g_config.strPwd.c_str(), 
    	sizeof(db.m_szStrPwd));
    strncpy(db.m_szStrDatabase, g_config.strDbname.c_str(), 
    	sizeof(db.m_szStrDatabase));
    db.m_Port = g_config.nPort;
	if(!g_dbpool.InitDBPool(&db,static_cast<enum DBTYPE>(0), INITCONNNUM))
	{
		WriteLogFATAL(g_log.m_rec, "InitDBPool failed!");
        return false;
	}

	//成为新会话领头进程，0,1,2--》\dev\NULL

#ifndef _DEBUGMODE
	
	int ret = 0;
	ret = daemon(0, 0);
	if (ret) 
	{
		printf("daemon failed");
		return -1;
	}

#endif

	signal(SIGPIPE, SIG_IGN);
	int v = 50;
	int n = 1;//收发一个反而更好

	thpool_t* threadpool;             /* make a new thread pool structure     */
	threadpool=CThread::thpool_init(2*n + v + 2);        /* initialise it to 2 + 3 * v number of threads */
	
	CThread::thpool_add_work(threadpool, g_ephandler.EPRecv, static_cast<void *>(&g_ephandler));
	CThread::thpool_add_work(threadpool, g_ephandler.EPSend, static_cast<void *>(&g_ephandler));
	do{
		CThread::thpool_add_work(threadpool, g_ephandler.Recv, static_cast<void *>(&g_ephandler));
		CThread::thpool_add_work(threadpool, g_ephandler.Send, static_cast<void *>(&g_ephandler));
	}while(--n);

	do{			
		CThread::thpool_add_work(threadpool, g_ephandler.doWork, static_cast<void *>(&g_ephandler));	
	}while(--v);
	
	
	while(1)
	{
		sleep(1);
	}
	return 0;
}


