#include "data_global.h"

extern pthread_mutex_t mutex_beep;
extern pthread_cond_t cond_beep;
extern unsigned char  cmd_beep; 
/*通过消息队列接收来自阿里云下发的消息，并对消息进行解析，根据消息类型执行不同的指令*/
extern int msgid;
extern key_t msg_key;

struct msg_cmd msgbuf;

void *pthread_cmd(void *arg)
{
    int ret = 0;
    int led_fd;
    /*使用ftok生成key*/
    if((msg_key = ftok("/tmp",'g')) < 0)  
    {
		perror("ftok failed .\n");
		exit(-1);
    }
    /*使用msgget创建消息队列*/
    msgid = msgget(msg_key,IPC_CREAT|IPC_EXCL|0666);
    if(msgid == -1)
    {
        if(errno == EEXIST)
        {
            msgid = msgget(msg_key,0777);
        }
        else
        {
			perror("fail to msgget");
			exit(1);
        }
    }
    /*创建成功，在循环中读取消息队列*/
    while(1)
    {
        bzero(&msgbuf,sizeof(msgbuf));//先清空之前的消息队列
        msgrcv(msgid,&msgbuf,sizeof(msgbuf),0,0);
        printf ("Get type:%ld msg\n", msgbuf.msgtype);
        printf ("data = %d\n", msgbuf.data[0]);
        switch(msgbuf.msgtype)
        {
            case 1:
                /*通常我们不在这个进程中直接对命令进行处理，为了解耦，我们再创建一个led线程处理相应命令，不同线程之间共享数据
                需要考虑线程之间通信的互斥问题，在写数据的时候，先上互斥锁
                在数据写完后，解锁，并使用条件变量通知led线程，led线程接收条件变量，对数据进行处理*/
                //pthread_mutex_lock(&mutex_led);

                led_fd = open(LED_DEV, O_RDWR);
                if(msgbuf.data[0] == 1)
                {
                    printf("LED ON\n\r"); 
                    ret = write(led_fd,msgbuf.data, 1);
                    if(ret < 0)
                    {
                        printf("LED Control Failed!\r\n");                  
                    }
                }
                if(msgbuf.data[0] == 0)
                {
                    printf("LED OFF\n\r"); 
                    ret = write(led_fd,msgbuf.data, 1);
                    if(ret < 0)
                    {
                        printf("LED Control Failed!\r\n");                  
                    }
                } 
                close(led_fd); 
				//pthread_mutex_unlock(&mutex_led);
				//pthread_cond_signal(&cond_led);             
                break;
            case 2:
                pthread_mutex_lock(&mutex_beep);//上锁
                /*对标志位进行操作*/

                cmd_beep = msgbuf.data[0];
                pthread_mutex_unlock(&mutex_beep);//开锁
                pthread_cond_signal(&cond_beep);//条件变量通知beep线程
                break;
            case 3:
                printf("SET INTERVAL\n\r");
                break;   
            default: 
                break;
                
        }

    }

}