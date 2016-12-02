//
//  ThreadPool.cpp
//
//  by jiahuan.liu
//  11/02/2016
//

#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t thNum, size_t qSize, bool bStart)
:_threads(thNum), _tasks(qSize), _bStop(false)
{
    if (bStart)
    {
        start();
    }
}

void ThreadPool::start()
{
    for (auto& th : _threads)
    {
        std::thread t
        ([this]{
            Task task;
            while (!isStop() || _tasks.size() != 0)
            {
                if (_tasks.pop(task))
                    task();
            }
        });
        th.swap(t);
    }
}

void ThreadPool::stop()
{
    _bStop = true;
}

bool ThreadPool::isStop()
{
    return _bStop.load(std::memory_order_acquire);
}

void ThreadPool::join()
{
    for (auto& th : _threads)
        th.join();
}