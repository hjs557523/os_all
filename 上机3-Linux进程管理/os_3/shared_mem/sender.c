#include "init.h"

key_t key;
int shmid;
void *shmp;

sem_t *send_psx;
sem_t *recv_psx;
sem_t *final_psx;

void init();
void destroy();
void catch_INT(int sig);
int check_value(sem_t *semtmp,int style);

int main(void)
{
    int ret = -1;
    char input[SHM_SIZE];
    char info[SHM_SIZE];
    signal(SIGINT,catch_INT);
    init();

    memset(input,'\0',sizeof(input));
    
    //send msg to receiver
    while(1)
    {
        printf("[*] Send message: ");
        scanf("%s",input);

        P(send_psx);

        strcpy(info,(char *)shmp);
        strcat(info,input);
        strcpy((char *)shmp,info);

        if (strcmp(input,"exit") == 0) 
        {
            ret = check_value(recv_psx,0);// check recv_psx value,if can release,return 1
            if (ret == 1) 
            {
                V(recv_psx);
            }
            break;
        }
        memset(input,'\0',sizeof(input));
        ret = check_value(recv_psx,0);
        if (ret == 1) 
        {
            V(recv_psx);
        }
        V(send_psx);
    }
    
    // recv end signal
    P(final_psx);
    strcpy(input,(char *)shmp);
    printf("[*] receive message: %s\n",input);
    destroy();
    printf("[*] sender end\n");
    return 0;
}

void init()
{
    key = KEY_NUM;
    shmid = shmget(key,SHM_SIZE,0666|IPC_CREAT);//创建 share memory，成功则返回id (一个与key相关的标识符)
    if (shmid < 0) 
    {
        printf("[*] Error: shmget() return error\n");
        exit(1);
    }

    shmp = shmat(shmid,NULL,0);//shared memory attach，连接到当前进程地址空间，返回指向 share memory 的指针
    memset((char *)shmp,'\0',SHM_SIZE);
    send_psx = sem_open(SEM_SEND,O_CREAT,0666,1);// 有名信号量，适用于不同进程间的同步互斥
    recv_psx = sem_open(SEM_RECV,O_CREAT,0666,0);
    final_psx = sem_open(SEM_FIN,O_CREAT,0666,0);

}

void destroy()
{
    shmdt(shmp);// share memory detach attach 断开共享内存与当前进程地址空间的连接
    shmctl(shmid,IPC_RMID,NULL);// 撤销 share memory
    sem_unlink(SEM_SEND);
    sem_unlink(SEM_RECV);
    sem_unlink(SEM_FIN);
}

int check_value(sem_t *semtmp,int style)
{
    // return 0: can not release this sem
    // return 1: you should release it
    // style means we check value with different ways
    int ret = -1,sem_vl;
    ret = sem_getvalue(semtmp,&sem_vl);
    if (ret == -1) 
    {
        printf("[*] Error:get sem value error\n");
        destroy();
        exit(0);
    }

    if (sem_vl == style) 
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

void catch_INT(int sig)
{
    printf("[*] Catch SIGINT\n");
    destroy();
    exit(0);
}