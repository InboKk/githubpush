#ifndef __DATA_GLOBAL__H__
#define __DATA_GLOBAL__H__
/*在全局文件中包含各种头文件，其他线程只需要包含data_global.h即可*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <pthread.h>
#include <termios.h>
#include <syscall.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/ioctl.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <linux/input.h>
#include <sys/socket.h>
#include <netinet/in.h> 
#include <netinet/tcp.h> 
#include <netinet/ip.h>
#include "cJSON.h"

#define   LED_DEV      "/dev/gpioled"  //led灯的设备名
#define	  BEEP_DEV   "/dev/beep"  //beep蜂鸣器的设备名

#define SERVER_PORT 8888

unsigned char  cmd_beep;  //蜂鸣器命令标志位

struct Sensors_node_data{
    float temperature;   //节点温度数据
    float humidity;     //节点湿度数据
};

extern void *pthread_aliyun (void *arg);   //对数据上云线程进行声明
extern void *pthread_cmd (void *arg);    //接受云端指令并通知相应线程
extern void *pthread_beep (void *arg);   //beep线程
extern void *pthread_server (void *arg);   //TCP服务器线程，接受来自客户端的连接请求
extern void *pthread_getdata (void *arg);  //获取来自客户端的数据

extern int send_cmd(long type,unsigned char data);
/*定义消息队列结构体*/
/*消息队列必须以一个long int长整形开始，接受者以此确定消息的类型*/
struct msg_cmd
{
    long msgtype;  //下发指令的消息类型：L（LED）、B（BEEP）、I（Interval）
    unsigned char data[12];
};

#endif 