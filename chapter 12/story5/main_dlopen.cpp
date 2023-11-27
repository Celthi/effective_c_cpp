//main-dlopen.cpp

#include <iostream>
#include "header.h"
#include <dlfcn.h>

int main()
{
    f0f();
    void* p = dlopen("libshared.so", RTLD_LAZY | RTLD_GLOBAL);
    if (p == nullptr)
    {
        std::cout << "load shared.so failed" << dlerror() << std::endl;
        exit(3);
    }
    else
    {
        std::cout << "load shared.so successful\n";
        auto func = (void (*)()) dlsym(p, "f0f");

        std::cout<<"get func"<<dlerror() << std::endl;
        if (func)
        {
            std::cout << "called\n\t";
            (*func)();
        }
    }
    dlclose(p);
    std::cout << "main finished" << std::endl;
}

