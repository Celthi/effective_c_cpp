#ifndef MY_HEADER_H
#define MY_HEADER_H
#include <iostream>
#include <atomic>
#include <map>
#include <string>
// Context用于构建一个全局变量，在构造和析构时输出一些信息
class Context
{
private:
    std::atomic_size_t mRef{0};

public:
    Context(/* args */);
    ~Context();
};

extern "C" void f0f();
#endif

