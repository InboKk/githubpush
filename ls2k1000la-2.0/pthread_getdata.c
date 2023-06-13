#include "data_global.h"

extern pthread_mutex_t mutex_getdata;  //接收消息的互斥锁
extern pthread_mutex_t mutex_online;

extern struct Sensors_node_data  env_data; //环境数据结构体
extern linklist ListHeader;  //用来保存无线传感结点在线信息的链表
void *pthread_getdata(void *arg)
{
    int clientsocket;
    clientsocket = *((int *)arg);
    int RecvLen;   //recv返回值
    unsigned char RecvBuf[255];   //recv接收数据缓冲
    cJSON * root,* id,* cmd,* params;
    cJSON * temperature,* humidity;
    struct WireLess_SensorsNode Sensors_node;

    memset(RecvBuf, 0, 255);
    pthread_mutex_unlock(&mutex_getdata); //解锁
    /*设置fd的keeplive，30s内没有数据交互，每5s探测一次，共探测3次*/
    set_tcp_keepAlive(clientsocket,30,5,3);
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
            /*json数据解析成功：{"id":35,"cmd":"3","params":{"temperature":66.1}}*/
            else 
            {
                /*对节点id：01——50进行解析*/
                id = cJSON_GetObjectItem(root, "id");
                if(id->type == cJSON_Number)
                {
                    printf("ID:%d\n\r", id->valueint);  
                }
                /*对节点指令进行解析：1、登陆；2、在线*/
                cmd = cJSON_GetObjectItem(root, "cmd");
                if(cmd->type == cJSON_String)
                {
                    printf("CMD:%s\n\r", cmd->valuestring);

                }
                /*if CMD == 2 {表示此时节点在线，将节点ID和上线时间写入数据库中，将ID填充进在线列表中}*/
                /*对节点传递的环境数据进行解析*/
                params = cJSON_GetObjectItem(root, "params");
                if(params)
                {
                    /*为了防止节点掉线之后，aliyun线程仍然上传上一次的数据，这里需要将环境信息结构体清空一下*/
                    /*判断节点数据是否为温度数据*/
                    if(cJSON_GetObjectItem(params, "temperature"))
                    {
                        if(strstr(cmd->valuestring,"1"))//CMD=1代表上线，只发送一次，后续CMD=2
                        {
                            /*将id和type插入链表中*/
                            Sensors_node.id = id->valueint;
                            strcpy(Sensors_node.SensorType, "temperature");   
                            pthread_mutex_lock(&mutex_online);//上锁  
                            InsertLinkNode(ListHeader,Sensors_node);  
                            printf("Success\n\r");
                            pthread_mutex_unlock(&mutex_online);//开锁
                        }
                        temperature = cJSON_GetObjectItem(params, "temperature");
                        if(temperature->type == cJSON_Number)
                        {
                            printf("Temperature:%.1f\n\r", temperature->valuedouble);
                            env_data.temperature = temperature->valuedouble;
                        }
                    }
                    /*判断节点数据是否为湿度数据*/
                    else if(cJSON_GetObjectItem(params, "humidity"))
                    {
                        if(strstr(cmd->valuestring,"1"))//CMD=1代表上线，只发送一次，后续CMD=2
                        {
                            /*将id和type插入链表中*/
                            Sensors_node.id = id->valueint;
                            strcpy(Sensors_node.SensorType, "humidity");   
                            pthread_mutex_lock(&mutex_online);//上锁  
                            InsertLinkNode(ListHeader,Sensors_node);  
                            printf("Success\n\r");
                            pthread_mutex_unlock(&mutex_online);//开锁
                        }                     
                        humidity = cJSON_GetObjectItem(params, "humidity");
                        if(humidity->type == cJSON_Number)
                        {
                            printf("humidity:%.1f\n\r", humidity->valuedouble);
                            env_data.humidity = humidity->valuedouble;
                        }
                    }   
                    /*后续可自由扩展节点数据类型*/                
                }
            }
            cJSON_Delete(root);
            memset(RecvBuf, 0, 255);
        }
        else//接收错误，
        {
            printf("errno: %d\n",errno);
            /*在这里为可以记录当前时间和id，将时间写入数据库中*/
            /*如果keepalive检测不到数据流通，会停止recv，进入这里*/
            time_t rawtime;
            time(&rawtime);
            struct tm *timeinfo;
            timeinfo = localtime(&rawtime);
            char tmp2[255];
            sprintf(tmp2, "Disconnected TIME: %d-%d-%d %2d:%2d:%2d",\
				timeinfo->tm_year+1900, timeinfo->tm_mon+1, timeinfo->tm_mday,\
				timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
            printf("Node ID:%s;%s\n\r",id->valueint,tmp2);//输出当前节点掉线时间
            /*sqliet3写入数据库： ID 上线时间  下线时间  */
            pthread_mutex_lock(&mutex_online);//上锁
            DeleteLinkNode(ListHeader,id->valueint);//删除结点
            pthread_mutex_unlock(&mutex_online);//开锁
            close(clientsocket);
            break;
        }
    }
    close(clientsocket);  
    pthread_exit((void*)1);
}