#pragma once
#include <vector>
#include <thread>
#include <functional>
#include <future>
#include <memory>
#include "blockedQueue.h"

class RequestHandler;

using FuncType = void (*)(RequestHandler&, int*, long, long);
using res_type = std::future<void>;
using task_type = std::function<void()>;

class ThreadPool
{
public:
    ThreadPool();
    void start();
    void stop();
    void push_task(FuncType f, RequestHandler& rh, int* arr, long arg1, long arg2);
    void threadFunc(int qindex, std::promise<void>& local);
    void wait();
private:
    int m_thread_count{};
    std::vector<std::thread> m_threads;
    std::vector<BlockedQueue<task_type>> m_thread_queues;
    int m_index{};
    std::vector<res_type> results;
};