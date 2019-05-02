#include <unistd.h>
#include <sys/syscall.h>
#include <stdio.h>
#include <stdlib.h>
#define sys_mysetnice 333

int main(){
   int pid = 0;
   int flag = 0;
   int nicevalue = 0;
   int prio = 0;
   int nice = 0;
   int result;

   printf("请输入:pid,flag,nicevalue\n");
   scanf("%d %d %d",&pid,&flag,&nicevalue);
   result = syscall(sys_mysetnice,pid,flag,nicevalue,(void*)&prio,(void*)&nice);
   if(result == 0){
       printf("成功执行了hjs的系统调用!");
       printf("pid:%d,flag:%d,nicevalue:%d,prio:%d,nice:%d\n",pid,flag,nicevalue,prio,nice);
   }else{
       printf("系统调用出现错误, 可能 pid 不存在!\n");
   }

   return 0;

}
