#include <sys/utsname.h>
#include <unistd.h>
#include <stdio.h>

int main()
{
    struct utsname a;
    uname(&a);
    printf("%s - %s\n", a.machine, a.sysname);

    char host_name[64] = {0};
    int ret = gethostname(host_name, 64);
    printf("hostname is: %s\n", host_name);
    return 0;
}
