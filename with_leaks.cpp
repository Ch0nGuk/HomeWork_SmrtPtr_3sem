#include "SmrtPtr.h"

using namespace smrt;

struct Empty
{
    SmrtPtr<Empty> collegue;
};


int main()
{
    SmrtPtr<Empty> A = MakeSmrtPtr<Empty>();
    SmrtPtr<Empty> B = MakeSmrtPtr<Empty>();

    A->collegue = B;
    B->collegue = A;
}