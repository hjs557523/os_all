#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/wait.h>

#define SEM_R "sem_read"
#define SEM_W "sem_write"
#define ENABLE_NUM 3

void write_to_pipe(int fd[2],int sid);
int read_from_pipe(int fd[2]);
void P(sem_t *sem_ptr); //原语操作P() = down() = wait()
void V(sem_t *sem_ptr); //原语操作V() = up() = signal()
void catch_INT(int sig);//捕获 Ctrl_c 退出父进程
void destroy_sem();

int main(void)
{
    int fd[2]; //文件描述符
    pid_t pid; //Process ID Type 宏定义 unsigined int
    int ret = -1;
    int child_num = ENABLE_NUM;
    int enable_val = -1;
    int sid = 0;
    int i;
    sem_t *write_psx,*read_psx; //sem_t 信号量数据类型 unsigined int

    write_psx = sem_open(SEM_W,O_CREAT,0666,1); //创建并初始化有名信号量
    read_psx = sem_open(SEM_R,O_CREAT,0666,0);

    if(pipe(fd) < 0) //无名管道创建，成功返回 0，失败返回 -1
    {
        printf("Error:pipe creation failed\n");
        exit(1);
    }
    for(i = 0;i < child_num;i++)
    {
        pid = fork();
        sid++;
        if(pid < 0)
        {
            printf("Error: function fork() return error\n");
            destroy_sem();
            exit(0);
        }
        else if(pid == 0)
        {
            break;
        }
    }
    if(pid == 0) //child process
    {
        P(write_psx);
         
        write_to_pipe(fd,sid);

        V(read_psx);
        V(write_psx);
        exit(0);
    }
    else //father process
    {
        signal(SIGINT,catch_INT);
        while(1)
        {
            //等待所有子进程都写完
            for(i = 0;i < child_num;i++)
            {
                P(read_psx);
            }
            read_from_pipe(fd);
        }
    }
    return 0;
}

void write_to_pipe(int fd[2],int sid)
{
    int value;
    char buf[10];
    close(fd[0]);
    memset(buf,sid+'0',sizeof(char)*10);
    printf("Children process %d write %d bytes data\n",sid,(int)sizeof(buf));
    write(fd[1],buf,sizeof(buf));// 若成功则返回写入的字节数，若出错则返回-1
}

int read_from_pipe(int fd[2])
{
    int ret;
    char buf[1024];
    close(fd[1]);
    memset(buf,'\0',sizeof(char)*1024);
    ret = read(fd[0],buf,1024);// 若成功则返回读到的字节数，若已到文件末尾则返回0，若出错则返回-1

    while(ret > 0)
    {
        printf("Father process read %d bytes data,content is: %s\n",ret,buf);
        memset(buf,'\0',1024);
        ret = read(fd[0],buf,1024);
    }
    return ret;
} 

void P(sem_t *sem_ptr)
{
    //信号量 sem<=0 则阻塞当前线程，sem>0 解除阻塞后将sem的值减1，表明公共资源经使用后减少
    sem_wait(sem_ptr);
}

void V(sem_t *sem_ptr)
{
    //增加信号量的值，有线程阻塞信号量时，会使其中一个线程不再阻塞
    sem_post(sem_ptr);
}

void catch_INT(int sig)
{
    printf("catch ctrl + c and exit\n");
    destroy_sem();
    exit(0);
}

void destroy_sem()
{
    // 从系统中删除有名信号量
    sem_unlink(SEM_W);
    sem_unlink(SEM_R);
}