#include "TaskPool.h"
#include <chrono>

int main()
{
    TaskPool threadPool;
    threadPool.init(4);

    Task *task = NULL;
    for (int i = 0; i < 10; i++)
    {
        task = new Task();
        threadPool.addTask(task);
    }

    std::this_thread::sleep_for(std::chrono::seconds(5));
    threadPool.stop();

    return 0;
}