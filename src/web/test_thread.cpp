//
//  test_thread.cpp
//
//  by jiahuan.liu
//  11/07/2016
//

#include <mutex>
#include <queue>
#include <unistd.h>

#include "Common.h"
#include "ThreadUtil.h"

#define COUNT 100000

void test_thread_util();

int main()
{
    LOG << "begin" << END;
    __TRY__
    test_thread_util();
    __CATCH__(Exception& e)
    LOG << e.what() << END;
    __END__
    LOG << "end" << END;
}

template<typename T>
class LQueue //semaphore and std::mutex
{
public:
    void push(const T& data)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _q.push(data);
        _sem.post();
    }
    bool pop(T& data)
    {
        _sem.wait();
        std::lock_guard<std::mutex> lock(_mtx);
        data = _q.front();
        _q.pop();
        return true;
    }
private:
    Semaphore _sem;
    std::mutex _mtx;
    std::queue<T> _q;
};

template<typename T>
class SLQueue //spin lock using std::mutex
{
public:
    void push(const T& data)
    {
        std::lock_guard<std::mutex> lock(_mtx);
        _q.push(data);
    }
    bool pop(T& data)
    {
        while (true)
        {
            std::lock_guard<std::mutex> lock(_mtx);
            if (_q.empty()) continue;
            data = _q.front();
            _q.pop();
            return true;
        }
    }
private:
    std::mutex _mtx;
    std::queue<T> _q;
};

template<typename T>
void consumer_func(T* queue)
{
    size_t count = COUNT;
    size_t value = 0;
    while (count > 0) {
        //usleep(1);
        if (queue->pop(value))
        {
            --count;
        }
    }
}

template<typename T>
void bounded_producer_func(T* queue)
{
    size_t count = COUNT;
    while (count > 0) {
        //usleep(1);
        if (queue->push(count))
        {
            --count;
        }
    }
}

template<typename T>
void producer_func(T* queue)
{
    for (int count = 0; count < COUNT; ++count) {
        //usleep(1);
        queue->push(count);
    }
}

template<typename T1, typename T2>
long double run_test(T1 producer_func, T2 consumer_func)
{
    typedef std::chrono::high_resolution_clock clock_t;
    typedef std::chrono::time_point<clock_t> time_t;
    time_t start;
    time_t end;
    
    start = clock_t::now();
    
    ThreadPool pool(4, 8);
    
    bool added;
    added = pool.addTask(producer_func);
    added = pool.addTask(producer_func);
    added = pool.addTask(consumer_func);
    added = pool.addTask(consumer_func);
    
    pool.stop();
    pool.join();
    
    end = clock_t::now();
    
    return
    (end - start).count()
    * ((double) std::chrono::high_resolution_clock::period::num
       / std::chrono::high_resolution_clock::period::den);
}

void test_thread_util()
{
    long double t1, t2, t3;
    
    typedef LFQueue<size_t> Q1;
    Q1 q1(65536);
    
    t1 = run_test(std::bind(&bounded_producer_func<Q1>, &q1),
                  std::bind(&consumer_func<Q1>, &q1));
    
    std::cout << "Lock free queue completed "
    << COUNT
    << " iterations in "
    << t1
    << " seconds. "
    << ((long double) COUNT / t1) / 1000000
    << " million enqueue/dequeue pairs per second."
    << std::endl;
    
    typedef LQueue<size_t> Q2;
    Q2 q2;
    t2 = run_test(std::bind(&producer_func<Q2>, &q2),
                  std::bind(&consumer_func<Q2>, &q2));
    
    std::cout << "semaphore queue completed "
    << COUNT
    << " iterations in "
    << t2
    << " seconds. "
    << ((long double) COUNT / t2) / 1000000
    << " million enqueue/dequeue pairs per second."
    << std::endl;
    
    typedef SLQueue<size_t> Q3;
    Q3 q3;
    t3 = run_test(std::bind(&producer_func<Q3>, &q3),
                  std::bind(&consumer_func<Q3>, &q3));
    
    std::cout << "std::mutex queue completed "
    << COUNT
    << " iterations in "
    << t3
    << " seconds. "
    << ((long double) COUNT / t3) / 1000000
    << " million enqueue/dequeue pairs per second."
    << std::endl;
    
}