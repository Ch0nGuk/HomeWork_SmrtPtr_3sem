#include "ShrdPtr.h"

using namespace shrd;

struct Empty
{
    ShrdPtr<Empty> collegue;
};


int main()
{
    ShrdPtr<Empty> A = MakeShrdPtr<Empty>();
    ShrdPtr<Empty> B = MakeShrdPtr<Empty>();

    A->collegue = B;
    B->collegue = A;
}