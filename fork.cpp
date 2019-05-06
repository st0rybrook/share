#include <unistd.h>
#include <stdio.h>
int main() 
{
		int count = 1;
		int child;
		if(0== fork()) //子进程成功返回0；
		{ //开始创建子进程
			printf("This is child, count is: %d(%p). and his pid is: %d)\n", ++count,&count, getpid());//子进程的内容
		} 
		else 
		{
			printf("This is father, count is: %d(%p), his pid is: %d\n", count, &count, getpid());
		}
}

