#include <iostream>
#include <utility> // для std::move

#include "SmrtPtr.h"
#include "tests.h"

int failed = 0;
void Check(bool predicat, const char* message) // типа Assert, но не кладет программу
{
    if (predicat) std::cout << "[ OK ]  " << message << "\n";
    else 
    {
        std::cout << "[ FAIL ]  " << message << "\n";
        failed++;
    }
}


struct Point
{
    Point(int x, int y) : x_(x), y_(y) {}
    Point() 
    {
        x_ = 0;
        y_ = 0;
    }

    Point operator+(const Point& right)
    {
        Point sum(x_ + right.x_, y_ + right.y_);
        return sum;
    }

    int x_;
    int y_;
};


struct Shape
{
    Shape(double sq) : square(sq) {}
    Shape() : square() {}
    double square;
};

struct Circle : public Shape
{
    Circle(int rad) : Shape(3.14 * rad * rad), radius(rad) {}
    Circle() : Shape(), radius() {}
    int radius;
};



void TestCreate()
{
    smrt::SmrtPtr<Point> p1 = smrt::MakeSmrtPtr<Point>(1, 2);
    smrt::SmrtPtr<Point> p2 = smrt::MakeSmrtPtr<Point>();

    Check(p1->x_ == 1 && p1->y_ == 2, "Create Point(1, 2), test ->");
    Check((*p1).x_ == 1 && (*p1).y_ == 2, "Create Point(1, 2), test *");
    Check(p2->x_ == 0 && p2->y_ == 0, "Create Point(), test ->");
    Check((*p2).x_ == 0 && (*p2).y_ == 0, "Create Point(), test *");
}

void TestNullptr()
{
    smrt::SmrtPtr<Point> ptr;

    Check((!ptr), "Nullptr");

    bool fl_logicerror = false;

    try
    {
        int x = ptr->x_;
    }
    catch(const std::logic_error& e)
    {
        fl_logicerror = true;
    }
    catch(const std::exception& e) // ловим любое другое исключение, чтобы программа не падала
    {
        std::cerr << e.what() << "\n";
    }

    Check(fl_logicerror, "-> exception must be std::logic_error");


    bool fl_logicerror1 = false;

    try
    {
        Point p = *ptr;
    }
    catch(const std::logic_error& e)
    {
        fl_logicerror1 = true;
    }
    catch(const std::exception& e) // ловим любое стандартное исключение, чтобы программа не падала
    {
        std::cerr << e.what() << "\n";
    }

    Check(fl_logicerror1, "* exception must be std::logic_error");
    
}


void TestCopy()
{
    smrt::SmrtPtr<Point> p1 = smrt::MakeSmrtPtr<Point>(4, 5);
    smrt::SmrtPtr<Point> p2 = p1;

    Check(p2->x_ == 4 && p2->y_ == 5, "Copy is the same as original");

    p2->x_ = 2;
    p2->y_ =  7;
    Check(p1->x_ == 2 && p1->y_ == 7, "Original change with copy");

    smrt::SmrtPtr<int> empty;
    smrt::SmrtPtr<int> empty_copy = empty;
    Check(!empty_copy, "Copy of the empty is empty");   
}

void TestMove()
{
    smrt::SmrtPtr<int> p1 = smrt::MakeSmrtPtr<int>(5);
    smrt::SmrtPtr<int> p2 = std::move(p1);

    Check(*p2 == 5, "p2 after move has field");
    Check(!p1, "p1 after move is empty");

    smrt::SmrtPtr<int> empty;
    smrt::SmrtPtr<int> move_empty = std::move(empty);
    Check(!empty && !move_empty, "Move from the empty is empty");
}


void TestCopyAssignment()
{
    smrt::SmrtPtr<Point> point_empty;
    smrt::SmrtPtr<Point> point1 = smrt::MakeSmrtPtr<Point>(19, -15);
    point_empty = point1;
    Check(point_empty->x_ == 19 && point_empty->y_ == -15, "Point after copy assignment isn't empty");
    point_empty->x_ = -10;
    Check(point1->x_ == -10 && point1->y_ == -15, "Point original was changed through the copy assignment");

    smrt::SmrtPtr<Point> point2 = smrt::MakeSmrtPtr<Point>(4, 5);
    point1 = point2;
    Check(point1->x_ == 4 && point1->y_ == 5, "Copy assignment from not-empty point");

    smrt::SmrtPtr<Point> empty2;
    point1 = empty2;
    Check(!point1, "Copy assignment from empty is empty");

    point2 = point2; // для Valgrind, проверит что память не утекла
}


void TestMoveAssignment()
{
    smrt::SmrtPtr<Point> p1 = smrt::MakeSmrtPtr<Point>(10, 20);
    smrt::SmrtPtr<Point> p2;
    p2 = std::move(p1);
    
    Check(!p1, "p1 after move assignment is empty");
    Check(p2->x_ == 10 && p2->y_ == 20, "p2 after move assignment has field from p1");

    p2 = std::move(p1);
    Check(!p2 && !p1, "Move assignment full from empty");

    smrt::SmrtPtr<Point> empty_point;
    empty_point = std::move(p2);
    Check(!p1 && !empty_point, "Move assignmnet empty from empty");

    smrt::SmrtPtr<Point> p3 = smrt::MakeSmrtPtr<Point>(3, 1);
    smrt::SmrtPtr<Point> p4 = smrt::MakeSmrtPtr<Point>(5, 4);
    p4 = std::move(p3);
    Check(p4->x_ == 3 && !p3, "Move assignment full from full");

    p4 = std::move(p4);
    Check(bool(p4), "Self copy assignment wasn't change object");   
}

void TestUpcast()
{
    smrt::SmrtPtr<Circle> circle = smrt::MakeSmrtPtr<Circle>(2);
    smrt::SmrtPtr<Shape> shape = circle;
    circle->square = 200;
    Check(shape->square == 200, "Upcast copy constructor");

    smrt::SmrtPtr<Shape> shape1 = smrt::MakeSmrtPtr<Shape>(2);
    shape1 = circle;
    circle->square = 100;
    Check(shape1->square == 100, "Upcast copy assignment");
    
    smrt::SmrtPtr<Circle> circle1 = smrt::MakeSmrtPtr<Circle>(2);
    int radius1 = circle1->radius;
    shape1 = std::move(circle1);
    Check(shape1->square == 3.14 * radius1 * radius1 && !circle1, "Upcast move assignment");

    smrt::SmrtPtr<Shape> shape2 = std::move(circle);
    Check(shape2->square == 100 && !circle, "Upcast move constructor");
}


int main()
{
    TestCreate();
    TestNullptr();
    TestCopy();
    TestMove();
    TestCopyAssignment();
    TestMoveAssignment();
    TestUpcast();
}
