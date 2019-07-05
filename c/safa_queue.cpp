#include <queue>
#include <mutex>
#include <iostream>

template <typename T>
class SafeQueue
{
public:
    SafeQueue();
    ~SafeQueue();
    bool empty();
    int size();
    void pop();
    T front();
    void push(T *item);
    T back();

private:
    std::queue<T> safeQueue;
};

template <typename T>
bool SafeQueue<T>::empty()
{
    return safeQueue.empty();
}

int main()
{
    std::queue<int> q;
    for (int i = 0; i < 10; i++)
    {
        q.push(i);
    }
    while (!q.empty())
    {
        std::cout << "value is: " << q.front() << std::endl;
        q.pop();
    }
    return 0;
}