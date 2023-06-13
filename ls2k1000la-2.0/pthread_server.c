#include "data_global.h"

extern pthread_mutex_t mutex_getdata;

void *pthread_server(void *arg)
{
      pthread_t threadid;
      /*创建TCP服务器socket，等待连接请求*/
      int serverSocket;  //服务器socket
      int clientsocket;  //客户端socket
      struct sockaddr_in server_addr;
      struct sockaddr_in client_addr;
      int addr_len = sizeof(client_addr);
      /*创建一个服务器socket文件描述符*/
      signal(SIGCHLD,SIG_IGN);
      if((serverSocket = socket(AF_INET,SOCK_STREAM,0)) < 0)
      {
          perror( "error: create server socket!!!");
          exit(1);
      }
      /*填充socket协议地址，就是本机IP和端口*/
      bzero(&server_addr,sizeof(server_addr));
      server_addr.sin_family =AF_INET;
      server_addr.sin_port = htons(SERVER_PORT);
      server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
      /*将协议地址与socketfd进行绑定*/
      if(bind(serverSocket,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0)
      {
          perror("error: bind error ");
          exit(1);
      }
      /*使用listen将主动套接字改为被动套接字，等待客户端连接*/
      if(listen(serverSocket,10)<0)
      {
          perror("error: listen !!!!");
          exit(1);
      }
      /*在循环中接受来自客户端的socket连接请求*/
      while(1)
      {

          clientsocket = accept(serverSocket,(struct sockaddr *)&client_addr,(socklen_t*)&addr_len);		
          if(clientsocket != -1)
          {
              printf("Get connect from client %d : %s\n", clientsocket, inet_ntoa(client_addr.sin_addr));
              
              /*为每一个建立连接的节点分配一个线程，用来接收节点数据*/
              pthread_mutex_lock(&mutex_getdata);//上锁
              pthread_create(&threadid, NULL, pthread_getdata, (void *)&clientsocket); 
              pthread_detach(threadid);
          }
      }
      close(serverSocket);
}