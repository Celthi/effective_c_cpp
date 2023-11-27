// 找到进程的链接表
struct link_map *GetLinkMapList()
{
    static link_map *gLinkMap = NULL;

    if(gLinkMap)
    {
        return gLinkMap;
    }

    // 使用符号_DYNAMIC来获取运行时链接器数据结构
    Elf64_Dyn* dyn = _DYNAMIC;
    while (dyn->d_tag != DT_PLTGOT) {
        dyn++;
    }

    // GOT节保存了GOT的地址
    Elf64_Xword got = (Elf64_Xword) dyn->d_un.d_ptr;
    // 第二个GOT条目保存了链接映射
    got += sizeof(got);
    //现在，读取第一个link_map项并返回它
    gLinkMap = *(struct link_map**)got;
    //如果设置了LD_PRELOAD，则这不是链表的首部
    // 它实际上指向预加载的库
    // 真正的头（执行文件）在它之前
    while(gLinkMap->l_prev)
    {
        gLinkMap = gLinkMap->l_prev;
    }
    return gLinkMap;
}

// 替换进程中的函数
//
// 参数：iModuleName：如果不为NULL，则仅修补此模块
//        iFunctionName: 要修补的函数
//        iFuncPtr: 新函数
//        oOldFuncptr: 被替换的函数指针
//
// 返回: Patch_OK 表示成功，否则表示失败
Patch_RC PatchFunction(ModuleHandle iModuleHandle,
                       const char* iModuleName,
                       const char* iSymbolToFind,
                       FunctionPtr iFuncPtr,
                       FunctionPtr* oOldFuncPtr)
{
    *oOldFuncPtr = NULL;
    // 链接映射是可执行文件和已加载库的链接列表
    // 每个节点都包含指向模块基地址的指针、它的名称，以及指向其动态段的指针
    link_map *map = GetLinkMapList();
    if(!map)
    {
        return Patch_Unknown_Platform;
    }

    Patch_RC rc = Patch_Function_Name_Not_Found;

    if(!iModuleName || !iSymbolToFind)
    {
        return Patch_Invalid_Params;
    }

    // 找到指定的模块
    while(map && !strstr(map->l_name, iModuleName))
    {
        map = map->l_next;
    }
    // 模块名称不匹配
    if(!map)
    {
        return Patch_Module_Not_Found;
    }

    // 查找DT_SYMTAB和DT_STRTAB的位置
    // 也保存哈希表中的nchains
    void* symtab = NULL;
    void* strtab = NULL;
    void* jmprel = NULL;
    Elf64_Xword pltrelsz = 0;
    Elf64_Sword reltype = -1;
    Elf64_Xword relentsz = 0;
    Elf64_Xword relaentsz = 0;

    Elf64_Dyn *dyn = (Elf64_Dyn *)map->l_ld;
    while (dyn->d_tag) 
    {
        switch (dyn->d_tag) 
        {
        case DT_STRTAB:
            strtab = (void*)dyn->d_un.d_ptr;
            break;
        case DT_SYMTAB:
            symtab = (void*)dyn->d_un.d_ptr;
            break;
        case DT_JMPREL:
            // 这仅指向PLT的重定位条目
            jmprel = (void*)dyn->d_un.d_ptr;
            break;
        case DT_PLTRELSZ:
            pltrelsz = dyn->d_un.d_val;
            break;
        case DT_PLTREL:
            // 此项显示PLT是否使用RELA或REL的重定位表类型
            reltype = dyn->d_un.d_val;
            break;
        case DT_RELENT:
            relentsz = dyn->d_un.d_val;
            break;
        case DT_RELAENT:
            relaentsz = dyn->d_un.d_val;
            break;
        default:
            break;
        }
        dyn++;
    }

    // 在重定位节中搜索函数
    if(jmprel && pltrelsz && (reltype==DT_REL || reltype==DT_RELA)
        && (relentsz || relaentsz) && symtab && strtab)
    {
        int i;
        int symindex;
        Elf64_Sym* sym;

        if(reltype==DT_REL)
        {
            // 重定位类型为rel
            // 
            // i386 使用这种重定位类型
            ...
        }
        else
        {
            // 重定位类型为rela
            //
            // 仅AMD64/x86_64使用这种重定位类型
            // 对于函数钩子的设置，应满足以下条件：
            // ELF64_R_TYPE(rela->r_info) == R_X86_64_JUMP_SLOT
            // rela->r_addend==0 because R_X86_64_JUMP_SLOT reloc
            // 条目不使用addend
            Elf64_Rela* rela = (Elf64_Rela*)jmprel;
            for(i=0; i<pltrelsz/relaentsz; i++, rela=(Elf64_Rela* )((char*)rela+relaentsz))
            {
                symindex = ELF64_R_SYM(rela->r_info);
                sym = (Elf64_Sym*)symtab + symindex;
                // 从字符串表中获取符号名称
                const char* str = (const char*)strtab + sym->st_name;
                // 将它与我们的符号进行比较
                if(::strcmp(str, iSymbolToFind) == 0)
                {
                    // r_offset 具有到包含函数指针地址的GOT条目的偏移量（相对于模块加载基址）
                    Elf64_Addr lSymbolAddr = map->l_addr + rela->r_offset;
                    // 检索旧的函数指针时注意
                    // 如果延迟绑定有效并且函数在之前从未被调用过
                    // 那么这可能是指向动态链接器的指针
                    *oOldFuncPtr = *(FunctionPtr*)lSymbolAddr;
                    // 插入替代函数
                    *(FunctionPtr*)lSymbolAddr = iFuncPtr;
                    rc = Patch_OK;
                    break;
                }
            }
        }
    }
    else
    {
        // 对于根本没有文本段的某些模块来说，这可能是正常的
    }

    return rc;
}
