// 代码4
#include<functional>
#include<iostream>
class ScopeGuard
{
    std::function<void()> mFunc;

public:
    ScopeGuard(std::function<void()> f)
    {
        mFunc = f;
    }
    ~ScopeGuard()
    {
        mFunc();
    }
};

int doSomething(int* p) {
   return -1;
}
void finalize(int* p) {
}
void f() {
   int* p = new int{3};
   ScopeGuard s([&p]() {
        if (p) {
            delete p;
        };
        std::cout << "delete point\n";
    });
   int error = doSomething(p);
   if (error) {
       return;
    }  
   finalize(p);
   std::cout<<"Function ends!\n";
}
int main()
{
    f();
}
