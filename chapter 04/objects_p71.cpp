// 基本数据类型
typedef struct _GUID
{
    unsigned int Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[8];
} GUID;

extern "C" const GUID IID_IClassFactory;

typedef int(_ATL_CREATORARGFUNC)(void* pv,
                                       const GUID & riid,
                                       void** ppv,
                                       unsigned long* dw);
typedef int(_ATL_CREATORFUNC)(void*pv, const GUID&riid, void** ppv);

struct _ATL_INTMAP_ENTRY
{
    const GUID* piid;
    unsigned long dw;
    _ATL_CREATORARGFUNC* pFunc;
};

struct _ATL_CHAINDATA
{
    unsigned long dwOffset;
    const _ATL_INTMAP_ENTRY* (*pFunc)();
};

// 基类
struct IUnknown
{
public:
    virtual int QueryInterface(const GUID&riid, void**ppvObject)= 0;

    virtual unsigned int AddRef(void) = 0;

    virtual unsigned int Release(void) = 0;
};

struct IClassFactory : public IUnknown
{
public:
    virtual int CreateInstance(IUnknown* pUnkOuter,
                                     const GUID& riid,
                                     void** ppvObject) = 0;

    virtual int LockServer(bool fLock) = 0;
};

class CComObjectRootBase
{
public:
    CComObjectRootBase() {}

    int FinalConstruct();

    void FinalRelease();

    static int
    InternalQueryInterface(void* pThis,
                                const _ATL_INTMAP_ENTRY* pEntries,
                                const GUID& iid,
                                void** ppvObject);

    unsigned int OuterAddRef();

    unsigned int OuterRelease();

    int OuterQueryInterface(const GUID& iid, void** ppvObject);

    void SetVoid(void*);

    void InternalFinalConstructAddRef();

    void InternalFinalConstructRelease();

    static int _Cache(void* pv,
                          const GUID& iid,
                          void** ppvObject,
                          unsigned long dw);

    static int _Delegate(void* pv,
                              const GUID& iid,
                              void** ppvObject,
                              unsigned long dw)
    {
        int hRes = ((int)0x80004002);
        IUnknown* p = *(IUnknown**)((unsigned long)pv + dw);
        if (p != 0)
            hRes = p->QueryInterface(iid, ppvObject);
        return hRes;
    }

    static int _Chain(void* pv,
                          const GUID& iid,
                          void** ppvObject,
                          unsigned long dw)
    {
        _ATL_CHAINDATA* pcd = (_ATL_CHAINDATA*)dw;
        void* p = (void*)((unsigned long)pv + pcd->dwOffset);
        return InternalQueryInterface(p,
                                            pcd->pFunc(),
                                           iid,
                                           ppvObject);
    }

    unsigned int m_dwRef;
    IUnknown* m_pOuterUnknown;
};

template <class ThreadModel>
  class CComObjectRootEx;

template <class ThreadModel>
  class CComObjectLockT
{
public:
    CComObjectLockT(CComObjectRootEx<ThreadModel>* p)
    {
        if (p)
             p->Lock();
        m_p = p;
    }

    ~CComObjectLockT()
    {
        if (m_p)
             m_p->Unlock();
    }
    CComObjectRootEx<ThreadModel>* m_p;
};

template <class ThreadModel>
class CComObjectRootEx : public CComObjectRootBase
{
public:
    typedef ThreadModel _ThreadModel;
    typedef typename _ThreadModel::AutoCriticalSection _CritSec;
    typedef CComObjectLockT<_ThreadModel> ObjectLock;

    CComObjectRootEx()
    {
        m_pcritsec = new _CritSec();
    }

    virtual ~CComObjectRootEx()
    {
        if (m_pcritsec)
        {
            delete m_pcritsec;
            m_pcritsec = 0;
        }
    }

    unsigned int InternalAddRef()
    {
        return _ThreadModel::Increment(&m_dwRef);
    }
  
    unsigned int InternalRelease()
    {
        return _ThreadModel::Decrement(&m_dwRef);
    }

    void Lock()
    {
        if (m_pcritsec)
             m_pcritsec->Lock();
    }

    void Unlock()
    {
       if (m_pcritsec)
            m_pcritsec->Unlock();
    }

protected:
    void _SetSubjectObject() { }

private:
    _CritSec *m_pcritsec;
};

class CComFakeCriticalSection
{
public:
    CComFakeCriticalSection() { }

    void Lock() { }

    void Unlock() { }

    void Init() { }

    void Term() { }
};

class CComMultiThreadModel
{
public:
    static unsigned int Increment(unsigned int* p)
    {
        return ++(*p);
    }

    static unsigned int Decrement(unsigned int * p)
    {
        return --(*p);
    }

    typedef CComFakeCriticalSection AutoCriticalSection;
};

// 我们感兴趣的类
class CComClassFactory : public IClassFactory,
                         public CComObjectRootEx<CComMultiThreadModel>
{
public:
    CComClassFactory() { }

public: 
    typedef CComClassFactory _ComMapClass;
    static int _Cache(void* pv,
                          const GUID& iid,
                          void** ppvObject,
                          unsigned long dw)
    {
        _ComMapClass* p = (_ComMapClass*)pv;
        p->Lock();
        int hRes = CComObjectRootBase::_Cache(pv,
                                                     iid,
                                                     ppvObject,
                                                     dw);
        p->Unlock();
        return hRes;
    }

    IUnknown* _GetRawUnknown()
    {
        return (IUnknown*)((unsigned long)this+_GetEntries()->dw);
    }

    IUnknown* GetUnknown()
    {
        return _GetRawUnknown();
    }

    int QueryInterface(const GUID& iid, void** ppvObject)
    {
        return InternalQueryInterface(this,
                                            _GetEntries(),
                                            iid,
                                            ppvObject);
    }

    unsigned int AddRef()
    {
        return InternalAddRef();
    }
  
    unsigned int Release()
    {
        return InternalRelease();
    }

    const static _ATL_INTMAP_ENTRY* _GetEntries()
    {
        static const _ATL_INTMAP_ENTRY _entries[] = {
            {&IID_IClassFactory,
             ((unsigned long)(static_cast<IClassFactory*>
             ((_ComMapClass*)8))-8), ((_ATL_CREATORARGFUNC*)1)},
            {0, 0, 0}};
        return _entries;
    }

    virtual int CreateInstance(IUnknown * pUnkOuter,
                                    const GUID & riid,
                                    void** ppvObj)
    {
        // fake
        return 0;
    }

    virtual int LockServer(bool fLock)
    {
        // fake
        return 0;
    }

    void SetVoid(void* pv);

    _ATL_CREATORFUNC* m_pfnCreateInstance;
};

// 测试代码
IClassFactory* GetClassFactory()
{
    CComClassFactory* lpNewFactory = new CComClassFactory;
    IClassFactory* lpFactoryInterface = lpNewFactory;
    CComObjectRootBase* lpRootBase = lpNewFactory;

    lpFactoryInterface->LockServer(false);

    return lpFactoryInterface;
}

void ReleaseClassFactory(CComClassFactory* ipFactory)
{
    delete ipFactory;
}
