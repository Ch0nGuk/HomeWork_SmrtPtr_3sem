#include "tests.h"
#include "SmrtPtr.h"

#include <iostream>

namespace
{
    int failed = 0;

    void Check(bool condition, const char* what)
    {
        std::cout << (condition ? "[ OK ] " : "[FAIL] ") << what << "\n";
        if (!condition) failed++;
    }
}

struct Point
{
    static int alive;

    int x;
    int y;

    Point(int x, int y) : x(x), y(y) { alive++; }
    ~Point() { alive--; }
};

int Point::alive = 0;

void RunBasicTest()
{
    Storage storage;
    Check(storage.Size() == 0, "new storage is empty");

    {
        SmrtPtr<Point> a = storage.Create<Point>(1, 2);
        Check(storage.Size() == 1, "Create occupies one slot");
        Check(Point::alive == 1, "object is created");
        Check(a->x == 1 && a->y == 2, "pointer gives access to the object");

        {
            SmrtPtr<Point> b = a;
            Check(storage.Size() == 1, "copy does not create a second object");
            Check(b->x == 1 && b->y == 2, "copy sees the same object");
        }

        Check(storage.Size() == 1, "object is alive while one pointer remains");
        Check(Point::alive == 1, "object is not destroyed too early");
    }

    Check(storage.Size() == 0, "slot is released");
    Check(Point::alive == 0, "object is destroyed automatically");

    std::cout << "\n";
    if (failed == 0) std::cout << "ALL GOOD: reference counting works, no leaks\n";
    else std::cout << failed << " CHECK(S) FAILED\n";
}

int main()
{
    RunBasicTest();
    return failed;
}
