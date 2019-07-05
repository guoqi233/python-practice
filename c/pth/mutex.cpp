#include <pthread.h>
#include <iostream>

int glob = 0;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

void *incr(void *arg)
{
    int loc;
    int loop = *((int *)arg);
    for (int i = 0; i < loop; i++)
    {
        pthread_mutex_lock(&mtx);
        loc = glob;
        loc += 1;
        glob = loc;
        pthread_mutex_unlock(&mtx);
    }
}

int main()
{
    int loops = 1000000;
    pthread_t th1;
    pthread_t th2;

    pthread_create(&th1, NULL, incr, &loops);
    pthread_create(&th1, NULL, incr, &loops);

    pthread_join(th1, NULL);
    pthread_join(th1, NULL);
    std::cout << "result is: " << glob << std::endl;
    return 0;
}