    class AtomicLong
{
public:
    ...
    long operator++() throw()
    {
        return AtomicIncrement(&mnAtomicLongData);
    }
    long operator--() throw()
    {
        return AtomicDecrement(&mnAtomicLongData);
    }
    ...
private:
    long mnAtomicLongData;
    }

    virtual void AddRef() const throw(void* ipObject, AtomicLong& irRefCount)
    {
        RefCountLog("AddRef", ipObject, irRefCount);
        ++irRefCount;
    }

    virtual void Release(void* ipObject, AtomicLong& irRefCount)const throw()
    {
        RefCountLog("Release", ipObject, irRefCount);
        if (--irRefCount == 0)
        {
            // 这是最后一次引用
            free(ipObject);
        }
    }

    void RefCountLog(const char* ipFunc, void* ipObject, long iRefCount)
    {
        FILE* lpFile = GetLogFile();
        fprintf(lpFile, "tid= %ld\tFunction=%s\tObjectID=0x%lx”
                        “\tRefCount=%ld\n",
                        pthread_self(), ipFunc, ipObject, iRefCount);

        // 检索调用者的堆栈跟踪
        const int max_stack_depth = 32;
        void* lTrace[max_stack_depth];
        int lTraceSize = ::backtrace(lTrace, max_stack_depth);
        _ASSERT(lTraceSize > 0);

        // backtrace_symbols 分配一块内存来存储符号
        char** lpFunctionNames = ::backtrace_symbols(lTrace, lTraceSize);
        for(int i=0; i<lTraceSize; i++)
        {
            _ASSERT(lpFunctionNames[i] != NULL);
            fprintf(lpFile, "\t[%d] %s\n", i, lpFunctionNames[i]);
        }	
    }
