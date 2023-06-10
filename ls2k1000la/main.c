#include <stdio.h>
#include <pthread.h>
#include "data_global.h"

/*资源释放函数，统一释放各种资源*/
void release_pthread_resource(int signo);
/*定义线程之间同步互斥的互斥锁和条件变量*/
extern pthread_mutex_t mutex_beep;
extern pthread_mutex_t mutex_getdata;

extern pthread_cond_t cond_beep;
/*定义进程之间通信需要的共享内存、信号量、消息队列*/
extern int shmid;
extern int semid;
extern int msgid; 
/*定义线程ID*/
pthread_t id_aliyun;    //数据上云线程id
pthread_t id_cmd;    //数据上云线程id
pthread_t id_beep;
pthread_t id_server;

/*main函数：对各种线程进行创建、互斥锁、条件变量的创建*/
int main(int argc, const char *argv[])
{
    //初始化互斥锁
    pthread_mutex_init(&mutex_beep,NULL);
    pthread_mutex_init(&mutex_getdata,NULL);
    //初始化条件变量
    pthread_cond_init(&cond_beep,NULL);
    //等待接受信号，信号处理函数
	signal (SIGINT, release_pthread_resource);
    pthread_create(&id_aliyun,		NULL,pthread_aliyun,NULL);
    pthread_create(&id_cmd,		NULL,pthread_cmd,NULL);
    pthread_create(&id_beep,	NULL,pthread_beep,NULL);
    pthread_create(&id_server,	NULL,pthread_server,NULL);
    //等待线程退出
    pthread_join(id_aliyun,NULL);  
    pthread_join(id_cmd,NULL);  
    pthread_join(id_beep,NULL); 
    pthread_join(id_server,NULL); 

    return 0;
}

/*释放线程资源函数，对线程进行删除*/
void release_pthread_resource(int signo)
{
    //释放锁和条件变量
    pthread_mutex_destroy (&mutex_beep);
    pthread_mutex_destroy (&mutex_getdata);
    pthread_cond_destroy (&cond_beep); 
    //分离线程的资源
    pthread_detach(id_aliyun);
    pthread_detach(id_cmd);
    pthread_detach(id_beep);
    pthread_detach(id_server);
    //销毁信号量和共享内存
	shmctl (shmid, IPC_RMID, NULL);
	semctl (semid, 1, IPC_RMID, NULL);
    msgctl (msgid, IPC_RMID, NULL);

    printf("all pthread is detached\n");

    exit(0);
}