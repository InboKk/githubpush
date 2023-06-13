#include "data_global.h"

extern pthread_mutex_t mutex_beep;
extern pthread_cond_t cond_beep;
extern unsigned char  cmd_beep; 

void *pthread_beep(void *arg)
{
    int beep_fd;
    int ret = 0;

    beep_fd = open(BEEP_DEV, O_RDWR);
	if(beep_fd == -1){
		printf("open failed.\n");
	}
	printf("pthread_beep\n");
    while(1)
    {
        pthread_mutex_lock(&mutex_beep);//上锁
        pthread_cond_wait(&cond_beep,&mutex_beep);  //等待条件变量
        if(cmd_beep == 1)
        {
            printf("BEEP 0N\n\r");
            ret = write(beep_fd,&cmd_beep, 1);
            if(ret < 0)
            {
                printf("BEEP Control Failed!\r\n");                  
            }            
        }
        if(cmd_beep == 0)
        {
            printf("BEEP 0FF\n\r");
            ret = write(beep_fd,&cmd_beep, 1);
            if(ret < 0)
            {
                printf("BEEP Control Failed!\r\n");                  
            }  
        }
        pthread_mutex_unlock(&mutex_beep); //解锁
    }
    close(beep_fd);
}