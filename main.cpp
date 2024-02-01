#include <iostream>
#include <chrono>
#include <random>
#include <filesystem>
#include <fstream>
#include "handler.h"

bool make_thread{};

void quicksort(RequestHandler& rh, int* array, long left, long right)
{
    if (left >= right)
        return;
    long left_bound = left;
    long right_bound = right;
    long middle = array[(left_bound + right_bound) / 2];
    do
    {
        while (array[left_bound] < middle)
        {
            left_bound++;
        }
        while (array[right_bound] > middle)
        {
            right_bound--;
        }
        if (left_bound <= right_bound)
        {
            std::swap(array[left_bound], array[right_bound]);
            left_bound++;
            right_bound--;
        }
    } while (left_bound <= right_bound);

    if (make_thread && right_bound - left > 10000)
    {
        rh.push_task(quicksort, rh, array, left, right_bound);
        quicksort(rh, array, left_bound, right);
        rh.wait();
    }
    else
    {
        quicksort(rh, array, left, right_bound);
        quicksort(rh, array, left_bound, right);
    }
}

void doSpecSort(FuncType f, RequestHandler& rh, int* arr, long left, long right, std::filesystem::path path)
{
    auto start = std::chrono::high_resolution_clock::now();
    f(rh, arr, left, right - 1);
    auto finish = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> res = finish - start;
    std::cout << "sorting time: " << res.count() << " sec" << std::endl;
    std::ofstream out(path);
    for(size_t i = 0; i < right; ++i)
    {
        out << arr[i];
    }
    out.close();
    std::cout << "file size: " << std::filesystem::file_size(path) << " bytes" << std::endl;
}

int main()
{
    size_t size{ 100000 };
    int* arr1 = new int[size];
    int* arr2 = new int[size];
    std::random_device rd;
    std::uniform_int_distribution<int> dist(0, 100);
    for(size_t i = 0; i < size; ++i)
    {
        arr1[i] = dist(rd);
        arr2[i] = arr1[i];
    }
    RequestHandler rh;
    std::filesystem::path path1 = "size_test_1";
    std::cout << "Single thread sorting results: \n";
    doSpecSort(quicksort, rh, arr1, 0, size, path1);
    make_thread = true;
    std::cout << "\nMultithread sorting results: \n";
    std::filesystem::path path2 = "size_test_2";
    // doSpecSort(quicksort, rh, arr2, 0, size, path2);
    delete[] arr1;
    delete[] arr2;
}
