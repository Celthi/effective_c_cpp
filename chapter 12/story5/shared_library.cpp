//shared_library.cpp
#include "header.h"
#include <map>
#include <string>
//__attribute__ ((visibility ("hidden"))) std::map<int, char*> mxx;
std::map<int, char *> mxx;
Context::Context(/* args */)
{
    std::cout << "Constructing Context: this= " << this << " refcount=" << mRef << std::endl;
    mRef++;
}

Context::~Context()
{
    std::cout << "destroying Context: this= " << this << " refcount=" << mRef << "\t map address " << &mxx << std::endl;
}
Context g;
extern "C" void f0f()
{
    std::cout << "map address " << &mxx << std::endl;
    for (int i = 0; i < 20; i++)
    {
        mxx[i] = "ddd";
    }
}
