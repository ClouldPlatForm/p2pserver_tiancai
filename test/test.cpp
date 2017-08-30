/*功能：连接uPNP服务器，读取本地test.txt，发送到server，显示响应*/

#include <sys/types.h>    
#include <sys/time.h>       /* See NOTES */
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <sys/epoll.h>


#define TEST1 "post.txt"
#define TEST2 "xml.txt"
#define TEST3 "error.txt"
#define TIMES (1)

//#define TESTIP "192.168.139.213"
#define TESTIP "127.0.0.1"
//#define TESTIP "127.0.0.1"
#define TESTPORT 8090
int main(int argc, char **argv)
{
	int sockfd = 0, fd = 0, fd2 = 0;
	int cnt, ret = 0, iPort = 0;
  char *cIp = NULL;
	//socklen_t len;
	struct sockaddr_in  saddr;
	char buf[2048] = { 0 };
 	int a = TIMES;
 	int to = 6300000;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);

	saddr.sin_family = AF_INET;
  /*if(argv[1])
  {
   if ('0' == argv[1][0])
   saddr.sin_port = htons(8090);
   else if ('1' == argv[1][0])
   saddr.sin_port = htons(8091);
   else if ('2' == argv[1][0])
   saddr.sin_port = htons(8092);
  }
  else
  {
  }*/

  
  if(argc >= 2)
  {
    cIp = argv[1];
    saddr.sin_addr.s_addr = inet_addr(cIp);
  }else
  {
		saddr.sin_addr.s_addr = inet_addr(TESTIP);
  }
  if(argc >= 3)
  {
    iPort = atoi(argv[2]);
    if(iPort > 0)
    {
     saddr.sin_port = htons(iPort);
    }
  }
  else
  {
    saddr.sin_port = htons(TESTPORT);
  }
  int  bDontLinger = 0;
  ret = setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const char *)&bDontLinger, sizeof(bDontLinger));
  if (-1 == ret)
	{
     perror("connect error:");
		goto _out;
	}
  /*if((argv[1])&&(isalnum(argv[1][0])))
   {
    to = argv[1][0] - '0';
    if((argv[2])&&(isalnum(argv[2][0]))) a = atoi(argv[2]);
   }*/
  
 
   // fork();
	ret = connect(sockfd, (const struct sockaddr *)&saddr, sizeof(saddr));
	if (-1 == ret)
	{
     perror("connect error:");
		goto _out;
	}
 printf("connect ok\n");
 
 fd = open(TEST1, O_RDONLY);
 if(!fd) perror(TEST1); 
  fd2 = open(TEST2, O_RDONLY);
   if(!fd) perror(TEST1); 
 
	if (fd < 0)
	{
		perror("open failed\n");
		ret = -1;
		goto _out;
	}

	cnt = read(fd, buf, sizeof(buf));
  printf("%d, %s\n", cnt, buf);
	if (-1 != cnt)
 {
 ret = write(sockfd, buf, strlen(buf));
 if(ret) printf("send post ok\n");
 else 
 {
 printf("send post failed \n");
 goto _out;
 }
 }
		
   
   memset(buf, 0, sizeof(buf));
   
   usleep(to);
   	cnt = read(fd2, buf, sizeof(buf));
    printf("%d, %s\n", cnt, buf);
	if (-1 != cnt)
 {
  	ret = write(sockfd, buf, strlen(buf));
    if(ret) printf("send xml ok\n");
    else 
 {
 printf("send post failed \n");
 goto _out;
 }
 }
	
   
	memset(buf, 0, sizeof(buf));	
	cnt = read(sockfd, buf, sizeof(buf));
	if (cnt > 0) 
  {
    printf("%s\n", buf);
    if(strstr(buf, "NOK"))
    {
     printf("Server reps:nok\n");
     printf("***********a = %d*********\n", a);
    }
    else if(strstr(buf, "OK"))
    {
     printf("Server reps:ok\n");
     printf("***********a = %d*********\n", a);
    } 
  }
  else if(0 == cnt)
  {
     printf("Server closed\n");
  }
  else
  {
    perror("error:");
  }
  
  close(sockfd);
  //sleep(1);

 

_out:
	if (sockfd) close(sockfd);
	if (fd) close(fd);
 if (fd2) close(fd2);
 return ret;
}
