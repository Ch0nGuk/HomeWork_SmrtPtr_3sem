#include <iostream>
#include <chrono>
#include <memory> // для std::shared_ptr, только для сравнения

#include "SmrtPtr.h"

using Clock = std::chrono::steady_clock;

// возвращает миллисекунды между двумя отметками времени
double Ms(Clock::time_point from, Clock::time_point to)
{
    return std::chrono::duration<double, std::milli>(to - from).count();
}

int main()
{
    long long sum = 0; // копим значения, чтобы оптимизатор не выбросил циклы

    std::cout << "N,raw,smrt,shared_make,shared_new\n";

    for (long long n = 10; n <= 100000000; n *= 10)
    {
        Clock::time_point t1 = Clock::now();
        for (long long i = 0; i < n; i++)
        {
            int* p = new int(1);
            sum += *p;
            delete p;
        }

        Clock::time_point t2 = Clock::now();
        for (long long i = 0; i < n; i++)
        {
            smrt::SmrtPtr<int> p = smrt::MakeSmrtPtr<int>(1);
            sum += *p;
        }

        Clock::time_point t3 = Clock::now();
        for (long long i = 0; i < n; i++)
        {
            std::shared_ptr<int> p = std::make_shared<int>(1);
            sum += *p;
        }

        Clock::time_point t4 = Clock::now();
        for (long long i = 0; i < n; i++)
        {
            std::shared_ptr<int> p(new int(1)); // две аллокации, как у MakeSmrtPtr
            sum += *p;
        }

        Clock::time_point t5 = Clock::now();

        std::cout << n << "," << Ms(t1, t2) << "," << Ms(t2, t3) << "," << Ms(t3, t4) << "," << Ms(t4, t5) << "\n";
    }

    std::cerr << "sum: " << sum << "\n"; // печать в cerr, чтобы не портить CSV

    return 0;
}
