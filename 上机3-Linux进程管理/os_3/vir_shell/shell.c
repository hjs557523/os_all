#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>//提供类型pid_t的定义 
#include <sys/wait.h>

char *my_cmd[4] = {"exit","cmd1","cmd2","cmd3"};

int getCmdId(char *cmd)
{
    int i;
    for( i = 0; i < 4; i++)
    {
        if (strcmp(cmd,my_cmd[i]) == 0) {
            return i;
        }
    }
    return -1;    
}

void myFork(int cmdId)
{
    pid_t pid = fork();
    if (pid < 0) {
        printf("Error:function fork() return error\n");
        exit(0);
    }
    else if (pid == 0) {
        //子进程
        switch (cmdId)
        {
            case 1:
                execl("./cmd1","",NULL);
                break;
            case 2:
                execl("./cmd2","",NULL);
                break;
            case 3:
                execl("./cmd3","",NULL);
                break;
            default:
                printf("Error: command not found.\n");
                exit(0);
        }
    }
}

int main()
{
    char cmd[100];
    int cmdId = -1;
    while(1)
    {
        printf("***************************************************************\n");
        printf("please input your command: \n>");
        scanf("%s",cmd);
        cmdId = getCmdId(cmd);
        if (cmdId == -1) {
            printf("Error: command not found.\n");
        }
        else if (cmdId == 0) {
            printf("exit successfully.\n");
            exit(0);//退出父进程
        }
        else
        {
            myFork(cmdId);//创建并执行对应的子进程
        }

        wait(NULL);//父进程通过阻塞自己,分析子进程是否已经退出变成僵尸进程,并收集这个子进程的信息,把它彻底销毁后返回;否则一直阻塞
    }
}