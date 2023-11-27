// 命令：$ g++ -g –S foo.cpp
// 或者 $ clang++ -g –S foo.cpp
int gInt = 1;
int GlobalFunc(int i)
{
    return i+gInt;
}
