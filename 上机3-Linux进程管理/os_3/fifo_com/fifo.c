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

void write_to_fifo(int fd,int sid);
int read_from_fifo(int fd);
void P(sem_t *sem_ptr); //原语操作P() = down() = wait()
void V(sem_t *sem_ptr); //原语操作V() = up() = signal()
void catch_INT(int sig);//捕获 Ctrl_c 退出父进程
void destroy_sem();

int count = 0;

int main(void)
{
    int fd = -1; 
    pid_t pid; //Process ID Type 宏定义 unsigined int
    int ret = -1;
    int child_num = ENABLE_NUM;
    int enable_val = -1;
    int sid = 0;
    int i;
    sem_t *write_psx,*read_psx; //sem_t 信号量数据类型 unsigined int

    write_psx = sem_open(SEM_W,O_CREAT,0666,1); //创建并初始化有名信号量
    read_psx = sem_open(SEM_R,O_CREAT,0666,0);

    const char* fifo_file_name = "fifo_file";


    if(mkfifo(fifo_file_name,0777) < 0) //有名管道创建，成功返回 0，失败返回 -1
    {
        printf("warning: fifo exists\n");
    }


    //非阻塞方式打开：O_NONBLOCK
    //非阻塞:read函数在读设备或者管道，或者socket的时候，对方如果没有发送数据过来，read函数也会立即返回，从代码的角度来说，就是read函数后面的代码会马上被执行。
    //阻塞:read函数在读设备或者管道，或者socket的时候，默认是阻塞的，也就是说，对方如果没有发送数据过来，则read函数就会一直等待数据过来，从代码的角度来说，就是read函数后面的代码不会被执行。
    fd = open(fifo_file_name,O_RDWR|O_CREAT|O_NONBLOCK,0777);
    //file文件不存在,所以在书写第二个参数，要把O_CREAT加上
    //如果不加，程序会报此文件不存在

    if (fd < 0)
    {
        printf("Errpr: open fifo_file failed\n");
        exit(1);
    }
    

    //创建三个子进程:
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

    if(pid == 0) //子进程操作
    {
        P(write_psx);
         
        write_to_fifo(fd,sid);

        V(read_psx);
        V(write_psx);
        exit(0);
    }
    else //父进程操作
    {
        signal(SIGINT,catch_INT);
        while(1)
        {
            //等待所有子进程都写完
            for(i = 0;i < child_num;i++)
            {
                P(read_psx);
            }
            read_from_fifo(fd);
        }

    }
    
    close(fd);
    return 0;
}

void write_to_fifo(int fd,int sid)
{
    int value;
    char buf[10];
    memset(buf,sid+'0',sizeof(char)*10);//将子进程的id好转换为字符类型,并赋值到字符数组里
    printf("Children process %d write %d bytes data\n",sid,(int)sizeof(buf));
    write(fd,buf,sizeof(buf));// 若成功则返回写入的字节数，若出错则返回-1
}

int read_from_fifo(int fd)
{
    int ret;
    char buf[1024];
    memset(buf,'\0',sizeof(char)*1024);
    ret = read(fd,buf,1024);// 若成功则返回读到的字节数，若已到文件末尾则返回0，若出错则返回-1

    while(ret > 0)
    {
        printf("Father process read %d bytes data,content is: %s\n",ret,buf);

        memset(buf,'\0',1024);
        ret = read(fd,buf,1024);
    }
           
    while (1)
    {
        ret = write(fd,"A",1);
        if (ret == -1)
        {
            printf("at the end of the fifo\n");
            break;
        }
        count++;
    }
    printf("fifo capacity = %d bytes\n",count);

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