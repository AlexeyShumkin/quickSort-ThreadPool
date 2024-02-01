#pragma once
#include "pool.h"

class RequestHandler
{
public:
    RequestHandler();
    ~RequestHandler();
    void push_task(FuncType f, RequestHandler& rh, int* arr, long arg1, long arg2);
    void wait();
private:
    ThreadPool pool_;
};
