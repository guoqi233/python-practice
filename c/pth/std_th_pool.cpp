#include <thread>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <unistd.h>

typedef struct StdThPool
{
	int size;
	std::thread *ths;
	bool running;
	std::thread *manage_th;
} std_th_pool;

void worker_func()
{
	std::cout << "worker thread id is: " << std::this_thread::get_id() << std::endl;
	sleep(1);
}

void manage_func(void *arg)
{
	std_th_pool *pool = (std_th_pool *)arg;
	for (int i = 0; i < pool->size; i++)
	{
		pool->ths[i] = std::thread(worker_func);
		pool->ths[i].detach();
	}
	sleep(1);
}

std_th_pool *create_pool(int size)
{
	std_th_pool *pool;
	pool = (std_th_pool *)malloc(sizeof(std_th_pool));
	pool->size = size;
	// pool->ths = (std::thread *)malloc(size * sizeof(std_th_pool));
	pool->ths = new std::thread[size];
	return pool;
}

void start(std_th_pool *pool)
{
	pool->running = true;
	pool->manage_th = new std::thread(manage_func, pool);
	//std::cout << "mark" << std::endl;
	//pool->manage_th->detach();
}

void shutdown(std_th_pool *pool)
{
	pool->running = false;
	if (pool->manage_th->joinable())
		pool->manage_th->join();
}

int main()
{
	std_th_pool *pool;
	pool = create_pool(4);
	std::cout << "pool size is: " << pool->size << std::endl;
	start(pool);
	sleep(2);
	shutdown(pool);
	delete[] pool->ths;
	delete pool->manage_th;
	delete pool;
	return 0;
}