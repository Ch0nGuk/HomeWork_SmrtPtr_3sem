#include "SmrtPtr.h"


struct Emp
{
    smrt::SmrtPtr<Emp> collegue;
};


int main()
{
    smrt::SmrtPtr<Emp> A = smrt::MakeSmrtPtr<Emp>();
    smrt::SmrtPtr<Emp> B = smrt::MakeSmrtPtr<Emp>();

    A->collegue = B;
    B->collegue = A;

    return 0;
}