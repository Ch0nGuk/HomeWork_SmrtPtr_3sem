#include "tests.h"
#include "SmrtPtr.h"

#include <iostream>

struct Point
{
    int x;
    int y;

    Point(int x, int y) : x(x), y(y)
    {
        std::cout << "    Point(" << x << ", " << y << ") created\n";
    }

    ~Point()
    {
        std::cout << "    Point(" << x << ", " << y << ") destroyed\n";
    }
};

void RunBasicTest()
{
    Storage storage;
    std::cout << "occupied slots: " << storage.Size() << "\n";

    {
        SmrtPtr<Point> a = storage.Create<Point>(1, 2);
        std::cout << "after Create:   " << storage.Size() << "\n";
        std::cout << "a points to:    (" << a->x << ", " << a->y << ")\n";

        {
            SmrtPtr<Point> b = a;
            std::cout << "after copy:     " << storage.Size() << "\n";
            std::cout << "b points to:    (" << b->x << ", " << b->y << ")\n";
        }

        std::cout << "copy is gone:   " << storage.Size() << "\n";
    }

    std::cout << "last one gone:  " << storage.Size() << "\n";
}

int main()
{
    RunBasicTest();
}
