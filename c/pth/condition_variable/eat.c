#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define TOTAL_NUM 10000
static int pot_num = 0;
static int eat_num = 0;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

void *func_thread()
{

    for (int i = 0; i < TOTAL_NUM; i++)
    {
        pthread_mutex_lock(&mtx);
        while (pot_num == 50)
        {
            pthread_cond_wait(&cond, &mtx);
        }

        if (pot_num < 50)
        {
            pot_num += 1;
            printf("放入1个丸子，已放入%d个丸子，锅里共有%d个丸子.\n", i + 1, pot_num);
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mtx);
    }
    return NULL;
}

int main()
{
    pthread_t pth;
    pthread_create(&pth, NULL, func_thread, NULL);

    while (eat_num < TOTAL_NUM)
    {
        pthread_mutex_lock(&mtx);
        while (pot_num == 0)
        {
            pthread_cond_wait(&cond, &mtx);
        }
        if (pot_num > 0)
        {
            pot_num -= 1;
            eat_num += 1;
            printf("吃掉1个丸子，共吃掉%d个丸子，锅里还剩%d个丸子.\n", eat_num, pot_num);
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mtx);
    }

    pthread_join(pth, NULL);
    return 0;
}