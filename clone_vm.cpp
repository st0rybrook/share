///clone_vm.c part 1
#define _GNU_SOURCE
#include <sched.h>
#include <sys/wait.h>
#include <string.h>

#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/syscall.h>
#include <unistd.h>
#define gettidv1() syscall(__NR_gettid) //syscall(__NR_gettid)函数的意思就是获取线程ID，这句话的意思是宏定义gettidv1()函数为获取线程id的函数
#define gettidv2() syscall(SYS_gettid)  //这两个syscall没有任何区别，所以函数两个gettidv也没有任何的区别
#define getpid() syscall(SYS_getpid)    //通过宏定义的方式获取pid

#define STACK_SIZE 1024 * 1024

static int child_func(void *arg)
{
    char *buf = (char *)arg;
    printf("in child sees buf = %s \n", buf);
    sprintf(buf, "hello from child tid=%ld,pid=%ld\n", gettidv1(), getpid());

    return 0;
}
///clone_vm.c part 2
int main(int argc, char **argv)
{
    //给子任务分配参数
    char *stack = (char *)malloc(STACK_SIZE);
    if (!stack)
    {
        perror("malloc");
        exit(1);
    }

    //看看第一个命令行参数
    printf("\nthe programme name is %s\n", argv[0]);

    //唤起命令行参数vm，设置CLONE_VM标记
    unsigned long flags = 0;
    if ((argc > 1) && (!strcmp(argv[1], "vm")))
        flags |= CLONE_VM;

    char buf[100];
    sprintf(buf, "hello from parent pid=%ld\n", getpid());
    printf("in parent process before clone\n buf=%s", buf);

    //根据不同的标记创建子进程
    if (clone(child_func, stack + STACK_SIZE, flags | SIGCHLD, buf) == -1)
    {
        perror("clone");
        exit(1);
    }

    int status;
    if (waitpid(-1, &status, 0) == -1)
    {
        perror("wait");
        exit(1);
    }

    printf("in parent process: child exited with status %d.\n buf = %s \n", status, buf);
    return 0;
}