#include <unistd.h>
#include <stdio.h>

int main()
{
    char host_name[64] = {0};
    int ret = gethostname(host_name, 64);
    printf("hostname is: %s\n", host_name);
}