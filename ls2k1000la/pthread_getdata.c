#include "data_global.h"

extern pthread_mutex_t mutex_getdata;

void *pthread_getdata(void *arg)
{
    int clientsocket;
    clientsocket = *((int *)arg);
    int RecvLen;
    unsigned char RecvBuf[255];
    cJSON * root,* id,* cmd,* params;
    cJSON * temperature,* humidity;

    memset(RecvBuf, 0, 255);
    pthread_mutex_unlock(&mutex_getdata); //解锁
    //printf("This is socket: %d\n", clientsocket);
    while(1)
    {
        //sleep(1);
        RecvLen = recv(clientsocket, RecvBuf, 255, 0);
        //printf("%d,%d\n\r", RecvLen,errno);
        if (RecvLen > 0)//接收正确
        {
            printf("Get Msg From Client %d: %s\n\r",clientsocket, RecvBuf);
            /*对获取的json格式数据进行解析，获得根数据*/
            root = cJSON_Parse((char*)RecvBuf);
            if(!root)
            {
                printf("Error");
            }
            else  /*获取节点的id*/
            {
                id = cJSON_GetObjectItem(root, "id");
                if(id->type == cJSON_String)
                {
                    printf("ID:%s\n\r", id->valuestring);  
                }
                cmd = cJSON_GetObjectItem(root, "cmd");
                if(cmd->type == cJSON_String)
                {
                    printf("CMD:%s\n\r", cmd->valuestring);
                }
                params = cJSON_GetObjectItem(root, "params");
                if(params)
                {
                    if(cJSON_GetObjectItem(params, "temperature"))
                    {
                        temperature = cJSON_GetObjectItem(params, "temperature");
                        if(temperature->type == cJSON_Number)
                        {
                            printf("Temperature:%.1f\n\r", temperature->valuedouble);
                        }
                    }
                }
            }

            memset(RecvBuf, 0, 255);
        }
        else//接收错误，
        {
            printf("errno: %d\n",errno);
            close(clientsocket);
            break;
        }
    }
    
    close(clientsocket);  
    pthread_exit((void*)1);
}