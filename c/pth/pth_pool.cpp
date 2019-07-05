#include <pthread.h>
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <time.h>

typedef struct PthPool
{
    int size;
    pthread_t *ths;
    bool running;
    pthread_t manage_th;
} thread_pool;

void *worker_func(void *arg)
{
    printf("worker id is: %ld\n", pthread_self());
    sleep(1);
}

void *manage_func(void *arg)
{
    thread_pool *pool = (thread_pool *)arg;
    while (pool->running)
    {
        for (int i = 0; i < pool->size; i++)
        {
            pthread_create(&pool->ths[i], NULL, worker_func, NULL);
            pthread_detach(pool->ths[i]);
        }
        sleep(1);
    }
}

thread_pool *create_pool(int size)
{
    thread_pool *pool;
    pool = (thread_pool *)malloc(sizeof(thread_pool));
    pool->size = size;
    pool->ths = (pthread_t *)malloc(size * sizeof(thread_pool));
    return pool;
}

void start(thread_pool *pool)
{
    pool->running = true;
    pthread_create(&pool->manage_th, NULL, manage_func, pool);
}

void shutdown(thread_pool *pool)
{
    pool->running = false;
    pthread_join(pool->manage_th, NULL);
}

int main()
{
    thread_pool *pool;
    pool = create_pool(4);
    std::cout << "pool size is: " << pool->size << std::endl;
    start(pool);
    sleep(1);
    shutdown(pool);
    return 0;
}
