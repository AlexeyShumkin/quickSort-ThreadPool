#include "pool.h"

ThreadPool::ThreadPool() :
    m_thread_count(std::thread::hardware_concurrency() != 0 ? std::thread::hardware_concurrency() : 4),
    m_thread_queues(m_thread_count),
    results(m_thread_count){}

void ThreadPool::start()
{
    for (int i = 0; i < m_thread_count; ++i)
    {
        std::promise<void> local;
        results[i] = local.get_future();
        m_threads.emplace_back(&ThreadPool::threadFunc, this, i, std::ref(local));
    }
}

void ThreadPool::stop()
{
    for (int i = 0;i < m_thread_count; ++i)
    {
        task_type empty_task;
        m_thread_queues[i].push(empty_task);
    }
    for (auto& t : m_threads)
    {
        t.join();
    }
}

void ThreadPool::push_task(FuncType f, RequestHandler& rh, int* arr, long arg1, long arg2)
{
    int queue_to_push = m_index++ % m_thread_count;
    task_type task([=, &rh] {f(rh, arr, arg1, arg2);});
    m_thread_queues[queue_to_push].push(task);
}

void ThreadPool::threadFunc(int qindex, std::promise<void>& local)
{
    while (true)
    {
        task_type task_to_do;
        bool res{};
        int i{};
        for (; i < m_thread_count; ++i)
        {
            if (res = m_thread_queues[(qindex + i) % m_thread_count].fast_pop(task_to_do))
                break;
        }
        if (!res)
        {
            m_thread_queues[qindex].pop(task_to_do);
        }
        else if (!task_to_do)
        {
            m_thread_queues[(qindex + i) % m_thread_count].push(task_to_do);
        }
        if (!task_to_do)
        {
            return;
        }
        task_to_do();
        local.set_value_at_thread_exit();
    }
}

void ThreadPool::wait()
{
    for(auto& f : results)
    {
        f.wait();
    }
}