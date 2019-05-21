#include <pthread.h>
#include <unistd.h>
#include <stdio.h>

#define MAX_TH_NUM 8

void* pth_func(void* arg)
{
    printf("tid is: %ld\n", pthread_self());
    sleep(2);
}


int main(void)
{
    pthread_t ths[MAX_TH_NUM];
    for (int i = 0; i < MAX_TH_NUM; i++)
    {
        pthread_create(&ths[i], NULL, pth_func, NULL);
    }

    for (int i = 0; i < MAX_TH_NUM; i++)
    {
        pthread_join(ths[i], NULL);
        printf("pid %ld over\n", ths[i]);
    }
    return 0;
}