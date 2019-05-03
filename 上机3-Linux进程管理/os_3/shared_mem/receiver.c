#include "init.h"

key_t key;
int shmid;
void *shmp;

sem_t *send_psx; //信号量结构类型
sem_t *recv_psx;
sem_t *final_psx;

void init();
void close_all();
void catch_INT(int sig);

int main(void)
{
    char input[SHM_SIZE];
    signal(SIGINT,catch_INT);
    init();
    // recv msg from sender

    while(1)
    {
        P(recv_psx);
        strcpy(input,(char *)shmp);
        memset((char *)shmp,'\0',SHM_SIZE);
        printf("[*] receive message: %s\n",input);
        if (strcmp(input,"exit") == 0) 
        {
            strcpy((char *)shmp,"over");
            printf("[*] send message: over\n");
            V(final_psx);
            break;
        }   
    }
    close_all();
    printf("[*] Receiver end\n");
    return 0;
}

void init()
{
    key = KEY_NUM;
    shmid = shmget(key,SHM_SIZE,0666|IPC_CREAT);
    if (shmid < 0) 
    {
        printf("[*] Error: shmget() return error\n");
        exit(1);
    }
    shmp = shmat(shmid,NULL,0);
    send_psx = sem_open(SEM_SEND,O_CREAT,0666,1);
    recv_psx = sem_open(SEM_RECV,O_CREAT,0666,0);
    final_psx = sem_open(SEM_FIN,O_CREAT,0666,0);
}

void close_all()
{
    sem_close(send_psx);
    sem_close(recv_psx);
    sem_close(final_psx);
    shmdt(shmp);
}

void catch_INT(int sig)
{
    printf("[*] Catch SIGINT\n");
    close_all();
    exit(0);
}