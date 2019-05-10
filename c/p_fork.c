#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>

int main()
{
    int pid = 0;
    pid = fork();
    if (-1 == pid)
    {
        _exit(-1);
    }
    else if (0 == pid)
    {
        printf("this is child process pid is %d, parent pid is %d.\n", getpid(), getppid());
    }
    else
    {
        printf("this is parent process pid is %d, child process is %d.\n", getpid(), pid);
    }

    return 0;
}