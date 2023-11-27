#include <stdlib.h>
#include <stdio.h>
#include <string.h>
void AccessFree(){
    // 一个临时数组
    long* lArray = new long[10];
    delete lArray;

    // 访问已释放的内存并损坏它
    lArray[1] = 1;
}
void Victim() {
    void* p = malloc(32);
    free(p);
}

int main(int argc, char** argv)
{
    // 程序初始化
    void* p = malloc(128);
    char* str = strdup("a string");
    free(p);

    // 初次感染
    AccessFree();

    // 更多工作 ...

    // 因先前感染而失败
    Victim();

    return 0;
}
