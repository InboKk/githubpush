#include "data_global.h"

/*定义线程同步互斥操作所需要的互斥锁和条件变量*/
pthread_mutex_t mutex_beep;
pthread_mutex_t mutex_getdata;

pthread_cond_t cond_beep;

int shmid;   //进程间通信IPC：共享内存ID；用于数据上传线程和阿里云和MQTT进程之间通信
int semid;   //进程间通信IPC：信号量ID；用于数据上传线程和阿里云和MQTT进程之间通信
int msgid;   //进程间通信IPC：消息队列ID：用于阿里云下发的指令和线程之间通信

key_t shm_key;   //共享内存的key值，每个key独一无二，进程通信双方的key值必须一样
key_t sem_key;   //信号量key值
key_t msg_key;   //消息队列key值

struct Sensors_node_data  env_data;   //传感器节点数据结构体

/*消息队列发送函数*/
int send_cmd(long type,unsigned char data)
{
    struct msg_cmd msgbuf;

    msgbuf.msgtype = type;
    msgbuf.data[0] = data;

    if(msgsnd(msgid,&msgbuf,sizeof(msgbuf),0) == -1)
    {
		perror("fail to msgsnd type");
		exit(1);
    }
    return 0;
}

