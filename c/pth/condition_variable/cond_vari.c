#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

typedef enum tstate
{
    TS_ALIVE,
    TS_TERMINATED,
    TS_JOINED,
} ThreadState;

typedef struct thread
{
    pthread_t tid;
    ThreadState state;
    int sleepTime;
} Thread;

static pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
static Thread *threads;

static int total_thread_num = 0;
static int alive_thread_num = 0;
static int unjoined_thread_num = 0;

void *thread_func(void *arg)
{
    Thread *th = (Thread *)arg;
    // sleep(th->sleepTime);
    printf("Thread %ld terminateing.\n", th->tid);

    pthread_mutex_lock(&mtx);
    printf("acquire %ld\n", th->tid);
    unjoined_thread_num += 1;
    th->state = TS_TERMINATED;
    pthread_mutex_unlock(&mtx);

    pthread_cond_signal(&cond);
    return NULL;
}

int main()
{
    total_thread_num = 10000;
    threads = (Thread *)malloc(sizeof(Thread) * total_thread_num);
    for (int i = 0; i < total_thread_num; i++)
    {
        threads[i].state = TS_ALIVE;
        threads[i].sleepTime = i;

        pthread_create(&threads[i].tid, NULL, thread_func, &threads[i]);
        alive_thread_num += 1;
    }
    while (alive_thread_num > 0)
    {
        pthread_mutex_lock(&mtx);

        while (unjoined_thread_num == 0)
        {
            pthread_cond_wait(&cond, &mtx);
        }

        for (int i = 0; i < total_thread_num; i++)
        {
            if (threads[i].state == TS_TERMINATED)
            {
                pthread_join(threads[i].tid, NULL);
                threads[i].state = TS_JOINED;
                alive_thread_num--;
                unjoined_thread_num--;

                printf("joined idx thread %d, (%d alive).\n", i, alive_thread_num);
            }
        }
        pthread_mutex_unlock(&mtx);
    }
    return 0;
}