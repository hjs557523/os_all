#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <unistd.h>

#define send_type 1  //2种消息类型
#define recv_type 2

#define send_1_to_recv 1
#define send_2_to_recv 2
#define recv_to_send_1 3
#define recv_to_send_2 4

#define bool int
#define true  1
#define false 0

void *send_thread_1(void *arg);
void *send_thread_2(void *arg);
void *recv_thread(void *arg);
void P(sem_t *sem_ptr);
void V(sem_t *sem_ptr);

//定义4个信号量类型
sem_t send_psx;
sem_t recv_psx;
sem_t final_recv_1;
sem_t final_recv_2;

//pthread_t 为类型 unsigned long int
pthread_t send_pid_1;
pthread_t send_pid_2;
pthread_t recv_pid;

int count = 1;//计数器，声明为全局
bool send_1_over = false;//sender1线程 是否已经结束
bool send_2_over = false;//sender2线程 是否已经结束
int msgid;

// 消息缓存区
struct msgbuf
{
    long mtype;
    char mtext[256];
    int mwtw;//message where to where 表示消息的走向
};

//主线程(进程)
int main(void)
{
    //int sem_init(sem_t *sem, int pshared, unsigned int value);
    //作用:创建(内存)匿名信号量
    //param1: sem_t *sem,sem_t类型(信号量)对象
    //param2: int pashared, 0表示信号量将被进程内的线程共享,信号量应声明为全局变量;非0为进程间共享
    //param3: unsigned int value, 指定信号量值
    sem_init(&send_psx,0,1);      //互斥信号量,初值为1
    sem_init(&recv_psx,0,0);      //同步信号量,初值为0
    sem_init(&final_recv_1,0,0);  //同步信号量,初值为0
    sem_init(&final_recv_2,0,0);  //同步信号量,初值为0

    //int msgget(key_t key, int msgflag);
    //作用:创建一个新的或打开一个已经存在的消息队列，此消息队列与key相对应
    //param1: key：函数ftok的返回值或IPC_PRIVATE
    //param2: msgflag:IPC_CREAT:创建新的消息队列;IPC_EXCL:与IPC_CREAT一同使用，表示如果要创建的消息队列已经存在，则返回错误;IPC_NOWAIT:读写消息队列要求无法满足时，不阻塞。
    //返回值： 调用成功返回队列标识符,否则返回-1
    msgid = msgget(IPC_PRIVATE,0666|IPC_CREAT);
    
    //在以下两种情况下，将创建一个新的消息队列：
    //1、如果键的值是IPC_PRIVATE。
    //2、或者键的值不是IPC_PRIVATE，并且键所对应的消息队列不存在，同时标志中指定IPC_CREAT。
    if(msgid < 0)
    {
        printf("Error: function msgget() return error\n");
        exit(1);
    }

    //创建线程,并将线程加入当前进程
    pthread_create(&send_pid_1,NULL,send_thread_1,NULL);
    pthread_create(&send_pid_2,NULL,send_thread_2,NULL);
    pthread_create(&recv_pid,NULL,recv_thread,NULL);

    //主线程(进程)以阻塞的方式等待指定线程结束,并进行资源回收
    //主线程(进程)会一直等待,直到创建的线程结束,自己才结束
    pthread_join(send_pid_1,NULL);
    pthread_join(send_pid_2,NULL);
    pthread_join(recv_pid,NULL);

    return 0;
}

void *send_thread_1(void *arg)
{
    char info[256];      //消息发送区
    struct msgbuf s_msg; //消息缓存区
    s_msg.mtype = send_type;
    s_msg.mwtw = send_1_to_recv;
    while(true)
    {
        P(&send_psx);

        printf("[%d]\n",count);
        printf("[*] Thread sender1 sends:");

        //注意: scanf + %s 从标准输入读取一串字符，遇回车或空格结束。
        scanf("%s",info);

        if ((strcmp(info,"exit") == 0) || (strcmp(info,"end1") == 0)) 
        {
            strcpy(s_msg.mtext,"end1");

            //int msgsnd(int msqid, const void *msgp, size_t msgsz, int msgflg);
            //作用: msgsnd()将一个新的消息写入队列。
            //param1: msqid,消息队列的识别码
            //param2: msgp,指向消息缓冲区(struct msgbuf)的指针，此位置用来暂时存储发送和接收的消息，是一个用户可定义的通用结构
            //param3: msgsz：消息的大小
            //param4: msgflg：这个参数依然是是控制函数行为的标志，取值可以是：0,表示忽略
            msgsnd(msgid,&s_msg,sizeof(struct msgbuf),0);

            V(&recv_psx);
            break;
        }
        strcpy(s_msg.mtext,info);
        count++;
        msgsnd(msgid,&s_msg,sizeof(struct msgbuf),0); //追加一条消息到消息队列中
        V(&recv_psx);  
    }

    P(&final_recv_1);//final_recv_1 处理 send_thread_1 最后一次接受消息的问题
    msgrcv(msgid,&s_msg,sizeof(struct msgbuf),recv_type,0);//从消息队列中读一条消息
    printf("[*] Thread sender1 receives: %s\n",s_msg.mtext);
    count++;

    //同意sender2发送
    V(&send_psx);
    
    //2个sender线程都发送过 'end' 且收到过 'over' 后，将移除消息队列
    if (send_1_over && send_2_over)
    {
        msgctl(msgid,IPC_RMID,0); //移除消息队列
    }

    pthread_exit(NULL);//线程终止
}


void *send_thread_2(void *arg)
{
    char info[256];      //消息发送区
    struct msgbuf s_msg; //消息缓存区
    s_msg.mtype = send_type;
    s_msg.mwtw = send_2_to_recv;
    while(true)
    {
        //同步信号量
        P(&send_psx);

        printf("[%d]\n",count);
        printf("[*] Thread sender2 sends:");
        scanf("%s",info);
        if ((strcmp(info,"exit") == 0) || (strcmp(info,"end2") == 0)) 
        {
            strcpy(s_msg.mtext,"end2");
            msgsnd(msgid,&s_msg,sizeof(struct msgbuf),0);
            V(&recv_psx);
            break;
        }
        strcpy(s_msg.mtext,info);
        count++;
        msgsnd(msgid,&s_msg,sizeof(struct msgbuf),0); //追加一条消息到消息队列中
        V(&recv_psx);  
    }

    P(&final_recv_2);//final_recv_2 处理 send_thread_2 最后一次接受消息的问题
    msgrcv(msgid,&s_msg,sizeof(struct msgbuf),recv_type,0);//从消息队列中读一条消息
    printf("[*] Thread sender1 receives: %s\n",s_msg.mtext);
    count++;

    //互斥信号量
    V(&send_psx);
    
    //2个sender线程都发送过 'end' 且收到过 'over' 后，将移除消息队列
    if (send_1_over && send_2_over)
    {
        msgctl(msgid,IPC_RMID,0); //移除消息队列
    }

    pthread_exit(NULL);//线程终止
}


void *recv_thread(void *arg)
{
    struct msgbuf r_msg; //消息缓存区
    while(true)
    {
        P(&recv_psx);
        //临界区
        msgrcv(msgid,&r_msg,sizeof(struct msgbuf),send_type,0);
        if (r_msg.mwtw == send_1_to_recv)//根据消息走向判断来源 
        {
            if (strcmp(r_msg.mtext,"end1") == 0) 
            {
                strcpy(r_msg.mtext,"over1");
                r_msg.mtype = recv_type;
                r_msg.mwtw = recv_to_send_1;
                msgsnd(msgid,&r_msg,sizeof(struct msgbuf),0);
                printf("[*] Thread Receiver has received 'end1' from thread sender1 , and returns 'over1'\n");

                V(&final_recv_1);
                send_1_over = true;
            }
            else
            {
                printf("[*] Thread Receiver receives: %s from sender1\n",r_msg.mtext);
                V(&send_psx);
            }
            
        }
        else if(r_msg.mwtw == send_2_to_recv) //根据消息走向判断来源
        {
            if(strcmp(r_msg.mtext,"end2") == 0)
            {
                strcpy(r_msg.mtext,"over2");
                r_msg.mtype = recv_type;
                r_msg.mwtw = recv_to_send_2;
                msgsnd(msgid,&r_msg,sizeof(struct msgbuf),0);
                printf("[*] Thread Receiver has received 'end2' from thread sender2 , and returns 'over2'\n");

                V(&final_recv_2);
                send_2_over = true;
            }
            else
            {
                printf("[*] Thread Receiver receives: %s from sender2\n",r_msg.mtext);
                V(&send_psx);
            }
        }
        
        //2个sender线程,都发送过'end'且收到过'over'后，将跳出循环，结束当前线程
        if (send_1_over && send_2_over) {
            break;
        }
    }

    pthread_exit(NULL);
    
}

void P(sem_t *sem_ptr)
{
    sem_wait(sem_ptr);
}

void V(sem_t *sem_ptr)
{
    sem_post(sem_ptr);;
}

