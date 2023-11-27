#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#include <sys/mman.h>

# 定义PAGE_SZ 0x1000ul
extern "C" typedef void*(*MALLOC_FUNC)(size_t);

unsigned char malloc_stub[PAGE_SZ];
MALLOC_FUNC default_malloc = (MALLOC_FUNC) &malloc_stub[0];

// 替代malloc函数
extern "C" void* my_malloc(size_t sz)
{
    // 简单地跟踪malloc调用
    printf("malloc request %ld bytes\n", sz);

    return default_malloc(sz);
}

// 在给定的位置注入jmp指令
void InjectJumpInstr(unsigned char* target, unsigned char* new_func)
{
    unsigned char jmp_code[14] = {
        // jmpq *0(%rip)
        0xff, 0x25, 0x00, 0x00, 0x00, 0x00,
        // 跳转的绝对地址为8字节
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
    };

    *(unsigned long*)(jmp_code+6) = (unsigned long) new_func;

    // 将目标的保护位更改为可写
    char* pageaddr = (char*) ((unsigned long)target & ~(PAGE_SZ-1));
    if (mprotect(pageaddr, PAGE_SZ, PROT_READ | PROT_WRITE
                                    | PROT_EXEC ))
    {
        printf("failed to change protection mode\n");
        exit(-1);
    }

    // 写入jmp指令
    memcpy(target, jmp_code, sizeof(jmp_code));
}

int main()
{
    // 获取函数的地址
    unsigned char* lpMalloc = (unsigned char*)
                              dlsym(RTLD_DEFAULT, "malloc");
    unsigned char* lpMyMalloc = (unsigned char*) &my_malloc;

    // 设置malloc存根
    memcpy(&malloc_stub[0], lpMalloc, 18);
    InjectJumpInstr(&malloc_stub[18], lpMalloc+18);

    // 现在更改目标函数
    InjectJumpInstr(lpMalloc, lpMyMalloc);

    // 测试malloc
    void* parray[16];
    for (int i=0; i<16; i++)
        parray[i] = malloc(i*8);
    for (int i=0; i<16; i++)
        free(parray[i]);

    return 0;
}
