#include "data_global.h"

extern linklist ListHeader;  //用来保存无线传感结点在线信息的链表
extern pthread_mutex_t mutex_online;//全局链表的互斥锁
extern unsigned char  cmd_online;//打印在线信息 
void *pthread_online (void *arg)
{
    linklist listnode1;
    /*创建空链表，这个链表用来保存在线信息*/
    ListHeader = CreateEmptyLinklist();
    while(1)
    {
        if(cmd_online == 100 )  //打印在线信息
        {
            pthread_mutex_lock(&mutex_online);//上锁

            TraverseLinknode(ListHeader);//打印在线信息
            
            pthread_mutex_unlock(&mutex_online);//开锁
        }
        cmd_online = 0;
        /*else
        {
            pthread_mutex_lock(&mutex_online);//上锁

            listnode1 = SearchLinkNode(ListHeader,cmd_online);
            printf("Type:%s\r\n",listnode1->data.SensorType);

            pthread_mutex_unlock(&mutex_online);//开锁
        }*/
        sleep(1);
    }

}