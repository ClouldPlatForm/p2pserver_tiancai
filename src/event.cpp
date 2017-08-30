#include "event.h"



CEPHandler::CEPHandler()
 :m_erfd(0),m_esfd(0)
{
}

CEPHandler::~CEPHandler()
{
    pthread_mutex_destroy(&m_pmtRecv);
    pthread_mutex_destroy(&m_pmtRecved);
    pthread_mutex_destroy(&m_pmtSend);
    pthread_mutex_destroy(&m_pmtRes);
}

/* Set TCP keep alive option to detect dead peers. The interval option 
 * is only used for Linux as we are using Linux-specific APIs to set 
 * the probe send time, interval, and count. */  
bool CEPHandler::TCPKeepAlive(int fd, int interval)  
{  
    int val = 1;  

    //开启keepalive机制  
    if (setsockopt(fd, SOL_SOCKET, SO_KEEPALIVE, &val, sizeof(val)) == -1)  
    {  
        string err("setsockopt SO_KEEPALIVE: ");
        err += strerror(errno);
        WriteLogERROR(g_log.m_rec, err.c_str());  
        return false;  
    }  
    /* Default settings are more or less garbage, with the keepalive time 
     * set to 7200 by default on Linux. Modify settings to make the feature 
     * actually useful. */  
  
    /* Send first probe after interval. */  
    val = interval;  
    if (setsockopt(fd, SOL_TCP, TCP_KEEPIDLE, &val, sizeof(val)) < 0) 
    {  
        string err("setsockopt TCP_KEEPIDLE: ");
        err += strerror(errno);
        WriteLogERROR(g_log.m_rec, err.c_str());  
        return false;  
    }  
  
    /* Send next probes after the specified interval. Note that we set the 
     * delay as interval / 3, as we send three probes before detecting 
     * an error (see the next setsockopt call). */  
    val = interval/3;  
    if (val == 0) val = 1;  
    if (setsockopt(fd, SOL_TCP, TCP_KEEPINTVL, &val, sizeof(val)) < 0) 
    {  
        string err("setsockopt TCP_KEEPINTVL: ");
        err += strerror(errno);
        WriteLogERROR(g_log.m_rec, err.c_str());  
        return false;  
    }  
  
    /* Consider the socket in error state after three we send three ACK 
     * probes without getting a reply. */  
    val = 3;  
    if (setsockopt(fd, SOL_TCP, TCP_KEEPCNT, &val, sizeof(val)) < 0) 
    {  
        string err("setsockopt TCP_KEEPCNT: ");
        err += strerror(errno);
        WriteLogERROR(g_log.m_rec, err.c_str());  
        return false;  
    }  
 
  
    return true;  
}  
//函数  
//功能:设置socket为非阻塞的	
int CEPHandler::make_socket_non_blocking (int sfd)  
{  
    int flags, s;	 

    //得到文件状态标志	
    flags = fcntl (sfd, F_GETFL, 0);	
    if (flags == -1)	
    {  
        WriteLogERROR(g_log.m_rec, strerror(errno));	 
        return -1;  
    }  

    //设置文件状态标志	
    flags |= O_NONBLOCK;	
    s = fcntl (sfd, F_SETFL, flags);	
    if (s == -1)	
    {  
        WriteLogERROR(g_log.m_rec, strerror(errno));	 
        return -1;  
    }  

    return 0;	 
} 
/*************************************************************************
 函数名称: CEPHandler::Init
 功能说明: 初始化epoll连接处理类实例
 输入参数: port 监听端口           
 输出参数: 无
 返 回 值: true/false
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
bool CEPHandler::Init(short port)
{
    m_listenFd = socket(AF_INET, SOCK_STREAM, 0);
    if(make_socket_non_blocking(m_listenFd))
    {
    	WriteLogFATAL(g_log.m_rec, "make_socket_non_blocking m_listenFd failed!");
        return false;
    }

    // bind & listen
    sockaddr_in sin;
    bzero(&sin, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr =htonl(INADDR_ANY);
    sin.sin_port = htons(port);

    int bOn = 1;
    //允许地址可重用
    if(-1 == setsockopt(m_listenFd,SOL_SOCKET ,SO_REUSEADDR,&bOn,sizeof(int)))
    {
        WriteLogWARN(g_log.m_rec, "setsockopt SO_REUSEADDR ERROR!");
        //return false;
    }

    if(-1 == bind(m_listenFd, (const sockaddr*)&sin, sizeof(sin)))
    {
        WriteLogFATAL(g_log.m_rec, "bind m_m_listenFd failed!");
        return false;
    }
    if(-1 == listen(m_listenFd, 10000))
    {
        WriteLogFATAL(g_log.m_rec, "listen m_m_listenFd failed!");
        return false;
    }

    if(pthread_mutex_init(&m_pmtRecv, NULL))
    {
        WriteLogFATAL(g_log.m_rec, "pthread_mutex_init m_pmtRecv failed!");
        return false;
    }

    if(pthread_mutex_init(&m_pmtRecved, NULL))
    {
        WriteLogFATAL(g_log.m_rec, "pthread_mutex_init m_pmtRecved failed!");
        return false;
    }
    if(pthread_mutex_init(&m_pmtSend, NULL))
    {
        WriteLogFATAL(g_log.m_rec, "pthread_mutex_init m_pmtSend failed!");
        return false;
    }
    if(pthread_mutex_init(&m_pmtRes, NULL))
    {
        WriteLogFATAL(g_log.m_rec, "pthread_mutex_init m_pmtRes failed!");
        return false;
    }
    return true;
} 
/*************************************************************************
 函数名称: CEPHandler::AcceptHttpEpoll
 功能说明: accept新tcp连接fd，并加入接收监听
 输入参数: port 监听端口           
 输出参数: 无
 返 回 值: void
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
void CEPHandler::AcceptHttpEpoll ()
{
    struct epoll_event evtmp;
   
    socklen_t clilen = 0;
    sockaddr_in clientaddr;
    int connfd = accept(m_listenFd,(sockaddr *)&clientaddr, &clilen);
    if(connfd<0){
        WriteLogFATAL(g_log.m_rec, strerror(errno));  
        //exit(1);
        return;
    }
    TCPKeepAlive(connfd, 3);
    make_socket_non_blocking(connfd); 
     //设置用于读操作的文件描述符
    
    evtmp.data.fd=connfd;
    //设置用于注测的读操作事件

    evtmp.events = EPOLLIN;
    epoll_ctl(m_erfd,EPOLL_CTL_ADD,connfd,&evtmp);

    evtmp.events = EPOLLOUT;
    epoll_ctl(m_esfd,EPOLL_CTL_ADD,connfd,&evtmp);
}
/*************************************************************************
 函数名称: CEPHandler::ReadEpoll
 功能说明: 可读fd加入可读事件带处理队列
 输入参数: i 可读fd所在 m_evRecv下标          
 输出参数: 无
 返 回 值: void
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
void CEPHandler::ReadEpoll (int i)
{
    struct epoll_event evtmp;
    pthread_mutex_lock(&m_pmtRecv);
    if ((m_evRecv[i].data.fd < 0)||(m_qRecv.size() >= 50000))
    {
        pthread_mutex_unlock(&m_pmtRecv);
        return;
    }
    pthread_mutex_unlock(&m_pmtRecv);

    shared_ptr<SEvents> p(new SEvents(m_evRecv[i].data.fd,"", time(NULL)));
    pthread_mutex_lock(&m_pmtRecv);
    m_qRecv.push(p);
    pthread_mutex_unlock(&m_pmtRecv);
    //设置用于写操作的文件描述符
    evtmp.data.fd=m_evRecv[i].data.fd;
    //evtmp.events = EPOLLOUT;

    //epoll_ctl(m_esfd,EPOLL_CTL_ADD,m_evRecv[i].data.fd,&evtmp);//加入可写监听

    epoll_ctl(m_erfd, EPOLL_CTL_DEL, m_evRecv[i].data.fd, &evtmp);   
}
/*************************************************************************
 函数名称: CEPHandler::WriteEpoll
 功能说明: 可写fd加入可写事件带处理队列，输出响应
 输入参数: i 可读fd所在 m_evSend下标          
 输出参数: 无
 返 回 值: void
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
void CEPHandler::WriteEpoll (int i)
{
    struct epoll_event evtmp;
    if (m_evSend[i].data.fd < 0)
    {
        return;
    }

    pthread_mutex_lock(&m_pmtRes); 
    auto it = m_mapRes.find(m_evSend[i].data.fd); 
    if(it == m_mapRes.end())//not found
    {
        pthread_mutex_unlock(&m_pmtRes);
        return;
    }
    //pthread_mutex_unlock(&m_pmtRes);

    //先存
    pthread_mutex_lock(&m_pmtSend);
    m_qSend.push(it->second);
    pthread_mutex_unlock(&m_pmtSend);
    //再删
    //pthread_mutex_lock(&m_pmtRes);
    m_mapRes.erase(it);
    pthread_mutex_unlock(&m_pmtRes);
            
    evtmp.data.fd=m_evSend[i].data.fd;
    evtmp.events = EPOLLOUT;

    epoll_ctl(m_esfd, EPOLL_CTL_DEL, m_evSend[i].data.fd, &evtmp);  

    //设置用于注测的读操作事件

    /*evtmp.events = EPOLLIN;
    //evtmp.events=EPOLLIN;
    //Keep-Alive, 再入监听队列
    if(-1 == epoll_ctl(m_erfd, EPOLL_CTL_ADD, m_evSend[i].data.fd, &evtmp))
    {
        WriteLogERROR(g_log.m_rec, "Keep-Alive failed, epoll_ctl error");
    } */
}

/*************************************************************************
 函数名称: CEPHandler::EPRecv
 功能说明: 所有可读fd epoll监听
 输入参数: p CEPHandler实例          
 输出参数: 无
 返 回 值: void*
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
void* CEPHandler::EPRecv(void * p)
{

    signal(SIGPIPE, SIG_IGN);
    CEPHandler * ob = static_cast<CEPHandler *>(p);
    if(NULL == ob)
    {
        WriteLogFATAL(g_log.m_rec, "EPRecv param failed!");
        return NULL;
    }
    ob->m_erfd = epoll_create(MAX_EVENTS);
    if(ob->m_erfd < 0)
    {
        WriteLogFATAL(g_log.m_rec, "epoll_create failed!");
        return NULL;
    }

    ob->ev.data.fd = ob->m_listenFd;
    ob->ev.events = EPOLLIN;

    if(epoll_ctl(ob->m_erfd, EPOLL_CTL_ADD, ob->m_listenFd, &ob->ev))
    {
        WriteLogFATAL(g_log.m_rec, "epoll_ctl m_listenFd failed!");
        return NULL;
    }

    while (1)
    {
        int nfds = epoll_wait (ob->m_erfd, ob->m_evRecv, MAX_EVENTS, EPOLL_TIME_OUT);         //等待EPOLL时间的发生，相当于监听，
        //至于相关的端口，需要在初始化EPOLL的时候绑定。
        if (-1 == nfds)
        {
            perror("epoll_wait EPRecv");
            WriteLogERROR(g_log.m_rec, "epoll_wait failed!");
            continue;
        }
        for (int i = 0; i < nfds; i++)
        {
            try
            {
              if (ob->m_evRecv[i].data.fd == ob->m_listenFd)    //如果新监测到一个HTTP用户连接到绑定的HTTP端口，
              //建立新的连接。由于我们新采用了SOCKET连接，所以基本没用。
              {
                ob->AcceptHttpEpoll ();
              }
              /*else if (ob->m_evRecv[i].data.fd == ob->m_listen_sock_fd)       //如果新监测到一个SOCKET用户连接到了绑定的SOCKET端口，
              //建立新的连接。
              {
                OnAcceptSockEpoll ();
              }*/
              else if (ob->m_evRecv[i].events & EPOLLIN) //如果是已经连接的用户，并且收到数据，那么进行读入。
              {
                ob->ReadEpoll (i);
              }

            }
            catch (...)
            {
                WriteLogERROR(g_log.m_rec, "epoll loop catch error");
                return NULL;
            }
        }                                  //进行一些定时的操作，主要就是删除一些短线用户等。
    }
    return NULL;

}
/*************************************************************************
 函数名称: CEPHandler::EPSend
 功能说明: 所有可写fd epoll监听
 输入参数: p CEPHandler实例          
 输出参数: 无
 返 回 值: void*
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
void* CEPHandler::EPSend(void * p)
{
    signal(SIGPIPE, SIG_IGN);
    CEPHandler * ob = static_cast<CEPHandler *>(p);
    if(NULL == ob)
    {
        WriteLogFATAL(g_log.m_rec, "EPSend param failed!");
        return NULL;
    }

    ob->m_esfd = epoll_create(MAX_EVENTS);
    if(ob->m_esfd < 0)
    {
        WriteLogFATAL(g_log.m_rec, "epoll_create failed!");
        return NULL;
    }
    while (1)
    {
        int nfds = epoll_wait (ob->m_esfd, ob->m_evSend, MAX_EVENTS, EPOLL_TIME_OUT); 
         //等待EPOLL时间的发生，相当于监听，
        //至于相关的端口，需要在初始化EPOLL的时候绑定。
        if (nfds < 0)
        {
            perror("epoll_wait EPSend");
            WriteLogERROR(g_log.m_rec, "epoll_wait failed!");
            continue;
        }
        for (int i = 0; i < nfds; i++)
        {
            try
            {               
                if (ob->m_evSend[i].events & EPOLLOUT) //如果是已经连接的用户，并且收到数据，那么进行读入。
                {   
                    ob->WriteEpoll (i);
                }
            }
            catch (...)
            {
                WriteLogERROR(g_log.m_rec, "epoll loop catch error");
                return NULL;
            }
        }                                  //进行一些定时的操作，主要就是删除一些短线用户等。
    }
    return NULL;
}
//获取http头部 Content-Length属性
int getContentLenth(const char *buf)
{
    char *head = NULL,tmp[8] = {0};
    if(buf)
    {
        head = strstr(const_cast<char *>(buf), "Content-Length:");
        if(head)
        {
             head += strlen("Content-Length:");
        }
        else 
        {
            return -1;
        }
                
        char *i = head;
        for(; !isalnum(*i); i++);

        for(int j = 0; j < 8;j++, i++)
        {
            if(!isalnum(*i)) 
            {
                return atoi(tmp);
            }
            tmp[j] = *i;
        }  
    }

    return -1;
}
/*************************************************************************
 函数名称: CEPHandler::Recv
 功能说明: fd读处理，并将收到的数据加入业务处理队列
 输入参数: p CEPHandler实例          
 输出参数: 无
 返 回 值: void*
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
void* CEPHandler::Recv(void * p)
{

    signal(SIGPIPE, SIG_IGN);
    CEPHandler * ob = static_cast<CEPHandler *>(p);

    struct epoll_event evtmp;
    while(ob)
    {    
        pthread_mutex_lock(&ob->m_pmtRecv);
        if(ob->m_qRecv.empty())
        {
            pthread_mutex_unlock(&ob->m_pmtRecv);
            usleep(10000);
            continue;
        }
        auto it = ob->m_qRecv.front();//读入待接收事件数据
        shared_ptr<SEvents> sp(it);//it备份    
        //删

        ob->m_qRecv.pop();//pop后才能退锁
        pthread_mutex_unlock(&ob->m_pmtRecv);
        while(1)//接收完缓冲区为止
        {
            //超时接收放弃
            double dt = difftime(time(NULL), sp->lRecvTime);
            if( dt > RECV_TIME_OUT)
            {
                WriteLogERROR(g_log.m_rec, "Recv time out");  
                evtmp.data.fd = sp->ifd;
                epoll_ctl(ob->m_erfd, EPOLL_CTL_DEL, sp->ifd, &evtmp);
                epoll_ctl(ob->m_esfd, EPOLL_CTL_DEL, sp->ifd, &evtmp);
                close(sp->ifd);
                break;
            }
        
            char szBuf[MAX_RECVLEN] = {0};
            int nlen = recv(sp->ifd, szBuf, MAX_RECVLEN, MSG_DONTWAIT);

            if(nlen > 0)
            {

                sp->strContent += szBuf;
                //printf("%s\n", sp->strContent.c_str());
                if(-1 == sp->ilen)
                {
                    sp->ilen = getContentLenth(sp->strContent.c_str());//必须有http协议头
                }
                if(-1 == sp->ilen)//第一次接收后未得到长度，认为失败,继续接收
                {
                    continue;//读至空报EAGAIN错误，
                }
                else if(sp->ilen >= 0) 
                {
                    printf("%d\n", sp->ilen);
                    printf("%s\n", sp->strContent.c_str());
                    //每次strContent值变化，sp->strContent.c_str()也会变化
                    sp->phead = strstr(const_cast<char *>(sp->strContent.c_str()), "\r\n\r\n");

                    if(NULL == sp->phead) 
                    {
                        continue;
                    }                        

                    sp->phead += 4;
                    if(sp->ilen <= (int)strlen(sp->phead))//接收完成，
                    {
                        //存入接收完成队列
                        pthread_mutex_lock(&ob->m_pmtRecved);
                        ob->m_qRecved.push((sp));
                        pthread_mutex_unlock(&ob->m_pmtRecved); 

                        evtmp.data.fd = sp->ifd;
                        //evtmp.events = EPOLLOUT;
                        //epoll_ctl(ob->m_esfd,EPOLL_CTL_ADD, sp->ifd,&evtmp);//加入可写监听
                        evtmp.events = EPOLLIN;
                        epoll_ctl(ob->m_erfd,EPOLL_CTL_ADD, sp->ifd,&evtmp);//加入可写监听
                       
                        #ifdef _DEBUGMODE
                            cout<<sp->strContent.c_str()<<endl;
                        #endif
                        WriteLogINFO(g_log.m_rec, sp->strContent.c_str());

                        break;
                    }
                    
                    continue;
                }
            }
            else if(-1 == nlen)
            {
                if(EAGAIN == errno||EINTR == errno)//接收完成，再次加入可读监听
                {
                    //再次入队
                    pthread_mutex_lock(&ob->m_pmtRecv);
                    ob->m_qRecv.push(sp);
                    pthread_mutex_unlock(&ob->m_pmtRecv);

                    //evtmp.data.fd = sp->ifd;
                    //evtmp.events = EPOLLIN;
                    //epoll_ctl(ob->m_erfd, EPOLL_CTL_ADD, sp->ifd, &evtmp);
                }
                else 
                {
                    #ifdef  _DEBUGMODE
                        perror("send error");
                    #endif
                    WriteLogINFO(g_log.m_rec, strerror(errno));
            
                    if(EBADF != errno)
                    {
                        if(sp->ifd)
                        {
                            evtmp.data.fd = sp->ifd;
                            epoll_ctl(ob->m_erfd, EPOLL_CTL_DEL, sp->ifd, &evtmp);
                            epoll_ctl(ob->m_esfd, EPOLL_CTL_DEL, sp->ifd, &evtmp);
                            close(sp->ifd);
                        } 
                    }                     
                } 
                break; 
            }
            else if(0 == nlen)//连接已经断开
            {
                evtmp.data.fd = sp->ifd;
                epoll_ctl(ob->m_erfd, EPOLL_CTL_DEL, sp->ifd, &evtmp);
                epoll_ctl(ob->m_esfd, EPOLL_CTL_DEL, sp->ifd, &evtmp);
                close(sp->ifd);
                break;
            }
        }
        
       
    }

    return NULL;
}
/*************************************************************************
 函数名称: CEPHandler::doWork
 功能说明: 业务处理，提取处理队列，并调用相应业务处理函数处理事件
 输入参数: p CEPHandler实例          
 输出参数: 无
 返 回 值: void*
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
void* CEPHandler::doWork (void * p)
{
    signal(SIGPIPE, SIG_IGN);
    CEPHandler * ob = static_cast<CEPHandler *>(p);
    
    while(ob)
    {    
        pthread_mutex_lock(&ob->m_pmtRecved);
        if(ob->m_qRecved.empty())
        {
            pthread_mutex_unlock(&ob->m_pmtRecved);
            usleep(10000);
            continue;
        }
        auto it = ob->m_qRecved.front(); 
        shared_ptr<SEvents> sp(it);//it备份
        ob->m_qRecved.pop();//pop后才能退锁
        pthread_mutex_unlock(&ob->m_pmtRecved);

        char szOutbuf[MAX_SENDLEN] = {0};
        int nOutLen = 0;
        CEventHandle oEvent;
        oEvent.SubEvent(sp->strContent.c_str(), sp->strContent.size(), szOutbuf, nOutLen);
        sp->strContent = szOutbuf;//得到响应信息 

        //sp->strContent = "HTTP/1.1 200 OK\r\nContent-Length: 0\r\nContent-Type: text/xml\r\nConnection: Keep-Alive\r\n\r\n";
        /*#ifdef _DEBUGMODE
            cout<<sp->strContent.c_str()<<endl;
        #endif*/
        //先存
        pthread_mutex_lock(&ob->m_pmtRes);
        ob->m_mapRes.insert(make_pair(sp->ifd, sp));
        pthread_mutex_unlock(&ob->m_pmtRes);                
    }
    
    return NULL;
}
/*************************************************************************
 函数名称: CEPHandler::Send
 功能说明: fd发送相应处理，提取响应数据队列，并发送
 输入参数: p CEPHandler实例          
 输出参数: 无
 返 回 值: void*
 修订记录:
          1.作者: 聂文峰
            日期: 2015-06-19
            内容: 创建函数
 *************************************************************************/
void* CEPHandler::Send(void * p)
{
    signal(SIGPIPE, SIG_IGN);
    CEPHandler * ob = static_cast<CEPHandler *>(p);
    struct epoll_event evtmp;
    while(ob)
    {   
        /*struct timeval tv1,tv2;
        gettimeofday(&tv1, NULL);*/
        pthread_mutex_lock(&ob->m_pmtSend);
        if(ob->m_qSend.empty())
        {
            pthread_mutex_unlock(&ob->m_pmtSend);
            usleep(10000);
            continue;
        }
        auto it = ob->m_qSend.front(); 
        shared_ptr<SEvents> sp(it);//it备份 
         //删
        ob->m_qSend.pop();//pop后才能退锁
        pthread_mutex_unlock(&ob->m_pmtSend);
        #ifdef _DEBUGMODE
            cout<<sp->strContent.c_str()<<endl;
        #endif
        WriteLogINFO(g_log.m_rec, sp->strContent.c_str());
        /*gettimeofday(&tv2, NULL);//gettimeofday线程不安全
        printf("Send waste  %u usec\n",
        (tv2.tv_sec - tv1.tv_sec)*1000000 + tv2.tv_usec - tv1.tv_usec); */
        int nlen = send(sp->ifd, sp->strContent.c_str(),
        sp->strContent.size(), MSG_NOSIGNAL);
        if(nlen >= 0)
        {     
            //注测的可写监听
            evtmp.data.fd = sp->ifd;
            evtmp.events = EPOLLOUT;
            //evtmp.events=EPOLLIN;
            //Keep-Alive, 再入监听队列
            if(-1 == epoll_ctl(ob->m_esfd, EPOLL_CTL_ADD, sp->ifd, &evtmp))
            {
                WriteLogERROR(g_log.m_rec, "Keep-Alive failed, epoll_ctl error");
            }   
            continue;
        }
        else
        {
            
            #ifdef _DEBUGMODE
                int tmp = errno;
                WriteLogERROR(g_log.m_rec, strerror(tmp));
            #endif
            evtmp.data.fd = sp->ifd;
            epoll_ctl(ob->m_erfd, EPOLL_CTL_DEL, sp->ifd, &evtmp);//移除可读可写监听
            epoll_ctl(ob->m_esfd, EPOLL_CTL_DEL, sp->ifd, &evtmp);
            close(sp->ifd);
        }      
    }

    return NULL;   
}