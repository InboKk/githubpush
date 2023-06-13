#include "data_global.h"
#include "sem.h"

extern int shmid;
extern int semid;

extern key_t shm_key;
extern key_t sem_key;

extern struct Sensors_node_data  env_data; //环境数据结构体

/*封装共享内存数据*/
struct shm_addr
{
    struct Sensors_node_data  env_data;
};
struct shm_addr *shm_buf;  //共享内存结构体指针：向这个结构体指针中填充数据

int analog_data(struct Sensors_node_data *data);
/*数据上云线程，将采集到的传感器节点数据通过共享内存上传到阿里云端*/
void *pthread_aliyun(void *arg)
{
    printf("pthread_aliyun start!\r\n");
    //信号量创建
    if(sem_key = ftok("/tmp",'i') < 0)  //通过ftok算法创建出唯一的key值，失败返回-1，在阿里云进程中一定要保证ftok参数一样
    {
        perror("ftok failed .\n");
        exit(-1);
    }
	semid = semget(sem_key,1,IPC_CREAT|IPC_EXCL|0666);
	if(semid == -1)	{
		if(errno == EEXIST){
			semid = semget(sem_key,1,0777);
		}else{
			perror("fail to semget");
			exit(1);
		}
	}else{
		init_sem (semid, 0, 1);
	}
    //建立共享内存
	if((shm_key = ftok("/tmp",'i')) < 0){
		perror("ftok failed .\n");
		exit(-1);
	}
	shmid = shmget(shm_key,1024,IPC_CREAT|IPC_EXCL|0666);
	if(shmid == -1)	{
		if(errno == EEXIST){
			shmid = shmget(shm_key,1024,0777);
		}else{
			perror("fail to shmget");
			exit(1);
		}
	}
    if((shm_buf = (struct shm_addr *)shmat(shmid,NULL,0)) == (void *)-1)//  返回映射到进程地址空间共享区的开始地址。
    {
		perror("fail to shmat");
		exit(1);
    }
    /*开始向共享内存内部填充数据*/
    bzero (shm_buf, sizeof (struct shm_addr));  //清空共享内存
    while(1)
    {
        sem_p(semid,0);  //写数据，先进行P操作
        //通过结构体获取实际数据进行上传
        shm_buf->env_data.temperature = env_data.temperature;
        shm_buf->env_data.humidity = env_data.humidity;
        //analog_data(&shm_buf->env_data);  //填充模拟数据
       // printf("%.1f\r\n",shm_buf->env_data.temperature);
        sleep(1);
        sem_v(semid,0);  //数据填充结束，执行V操作
    } 
}

int analog_data(struct Sensors_node_data *data)
{
    data->temperature = 20.1;
    data->humidity = 66.5;
}
