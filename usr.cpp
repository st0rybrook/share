#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mount.h>
#include <sys/capability.h>
#include <stdio.h>
#include <sched.h>
#include <signal.h>
#include <unistd.h>
#include <fstream>
#include <iostream>
using namespace std;
#define STACK_SIZE (1024 * 1024)

static char container_stack[STACK_SIZE];
char *const container_args[] = {
    "/bin/bash",
    NULL};

int pipefd[2];

void set_map(char *file, int inside_id, int outside_id, int len)
{
  FILE *mapfd = fopen(file, "w");
  if (NULL == mapfd)
  {
    perror("open file error");
    return;
  }
  fprintf(mapfd, "%d %d %d", inside_id, outside_id, len);

  fclose(mapfd);
}
void set_map_cpp(char *file, int inside_id, int outside_id, int len)
{
  ofstream OutFile; //实例一个写文件对象

  OutFile.open(file); //创建一个Test.txt文本，并且打开Test.txt文件

  //ofstream OutFile("Test.txt"); //利用构造函数创建txt文本，并且打开该文本

  OutFile << inside_id<<" "<<outside_id<<" "<<len; //把字符串内容"This is a Test!"，写入Test.txt文件

  OutFile.close(); //关闭Test.txt文件

}

void set_uid_map(pid_t pid, int inside_id, int outside_id, int len)
{
  char file[256];
  sprintf(file, "/proc/%d/uid_map", pid);
  set_map_cpp(file, inside_id, outside_id, len);
}

void set_gid_map(pid_t pid, int inside_id, int outside_id, int len)
{
  char file[256];
  sprintf(file, "/proc/%d/gid_map", pid);

  set_map_cpp(file, inside_id, outside_id, len);
  printf("inside_id is %d", inside_id);
  printf("outside_id is %d", outside_id);
}

int container_main(void *arg)
{

  printf("Container [%5d] - inside the container!\n", getpid());

  printf("Container: eUID = %ld;  eGID = %ld, UID=%ld, GID=%ld\n",
         (long)geteuid(), (long)getegid(), (long)getuid(), (long)getgid());

  /* 等待父进程通知后再往下执行（进程间的同步） */
  char ch;
  close(pipefd[1]);
  read(pipefd[0], &ch, 1);

  printf("Container [%5d] - setup hostname!\n", getpid());
  //set hostname
  sethostname("container", 10);

  //remount "/proc" to make sure the "top" and "ps" show container's information
  mount("proc", "/proc", "proc", 0, NULL);

  execv(container_args[0], container_args);
  printf("Something's wrong!\n");
  return 1;
}

int main()
{
  const int gid = getgid(), uid = getuid();

  printf("Parent: eUID = %ld;  eGID = %ld, UID=%ld, GID=%ld\n",
         (long)geteuid(), (long)getegid(), (long)getuid(), (long)getgid());

  pipe(pipefd);

  printf("Parent [%5d] - start a container!\n", getpid());

  int container_pid = clone(container_main, container_stack + STACK_SIZE,
                            CLONE_NEWUTS | CLONE_NEWPID | CLONE_NEWNS | CLONE_NEWUSER | SIGCHLD, NULL);

  printf("Parent [%5d] - Container [%5d]!\n", getpid(), container_pid);

  //To map the uid/gid,
  //   we need edit the /proc/PID/uid_map (or /proc/PID/gid_map) in parent
  //The file format is
  //   ID-inside-ns   ID-outside-ns   length
  //if no mapping,
  //   the uid will be taken from /proc/sys/kernel/overflowuid
  //   the gid will be taken from /proc/sys/kernel/overflowgid

  set_uid_map(container_pid, 0, uid, 1);
  // 这里组有点问题，gid会发生改变
  set_gid_map(container_pid, 0, gid, 1);

  printf("Parent [%5d] - user/group mapping done!\n", getpid());

  /* 通知子进程 */
  close(pipefd[1]);

  waitpid(container_pid, NULL, 0);
  printf("Parent - container stopped!\n");
  return 0;
}
