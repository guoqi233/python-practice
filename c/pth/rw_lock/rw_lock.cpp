#include <pthread.h>
#include <unistd.h>
#include <iostream>

static pthread_rwlock_t rwlock;
static int share_value = 0;

void *read_thread(void *arg)
{
    while (true)
    {
        pthread_rwlock_rdlock(&rwlock);
        std::cout << "Thread - " << pthread_self() << " , " << share_value << std::endl;
        pthread_rwlock_unlock(&rwlock);
        sleep(1);
    }
}

void *write_thread(void *arg)
{
    bool run = true;
    while (run)
    {
        pthread_rwlock_wrlock(&rwlock);
        share_value += 1;
        std::cout << "Thread - " << pthread_self() << " , " << share_value << std::endl;
        std::cout << "share_value increase." << std::endl;
        if (share_value > 5)
        {
            run = false;
        }
        pthread_rwlock_unlock(&rwlock);
        sleep(2);
    }
    return NULL;
}

int main()
{
    pthread_t *ths = (pthread_t *)malloc(sizeof(pthread_t) * 3);
    pthread_create(&ths[0], NULL, write_thread, NULL);

    pthread_t ptid;
    ptid = pthread_create(&ths[1], NULL, read_thread, NULL);
    pthread_detach(ptid);
    ptid = pthread_create(&ths[2], NULL, read_thread, NULL);
    pthread_detach(ptid);

    pthread_join(ths[0], NULL);
    return 0;
}