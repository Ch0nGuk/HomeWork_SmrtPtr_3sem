#include <iostream>
#include <chrono>
#include <memory> // для std::shared_ptr, только для сравнения

#include "ShrdPtr.h"

using Clock = std::chrono::steady_clock;
const int* volatile observed_ptr; // адрес выходит из цикла, чтобы компилятор не убрал аллокацию

// возвращает миллисекунды между двумя отметками времени
double Ms(Clock::time_point from, Clock::time_point to)
{
    return std::chrono::duration<double, std::milli>(to - from).count();
}

int main()
{
    long long sum = 0; // проверяем обращения к объектам

    std::cout << "N,raw,shrd,shared_make\n";

    for (long long n = 10; n <= 100000000; n *= 10)
    {
        Clock::time_point t1 = Clock::now();
        for (long long i = 0; i < n; i++)
        {
            int* p = new int(1);
            observed_ptr = p;
            sum += *p;
            delete p;
        }

        Clock::time_point t2 = Clock::now();
        for (long long i = 0; i < n; i++)
        {
            shrd::ShrdPtr<int> p = shrd::MakeShrdPtr<int>(1);
            observed_ptr = &*p;
            sum += *p;
        }

        Clock::time_point t3 = Clock::now();
        for (long long i = 0; i < n; i++)
        {
            std::shared_ptr<int> p = std::make_shared<int>(1);
            observed_ptr = &*p;
            sum += *p;
        }

        Clock::time_point t4 = Clock::now();

        std::cout << n << "," << Ms(t1, t2) << "," << Ms(t2, t3) << "," << Ms(t3, t4) << "\n";
    }

    std::cerr << "sum: " << sum << "\n"; // печать в cerr, чтобы не портить CSV

    return 0;
}
