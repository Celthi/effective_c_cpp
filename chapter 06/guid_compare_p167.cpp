typedef struct _GUID
{
    unsigned int Data1;
    unsigned short Data2;
    unsigned short Data3;
    unsigned char Data4[ 8 ];
} GUID;

inline bool IsGUIDNULL(const ::GUID& irGUID)
{
    return ((reinterpret_cast<const int*>(&irGUID))[0] == 0
       && (reinterpret_cast<const int*>(&irGUID))[1] == 0
       && (reinterpret_cast<const int*>(&irGUID))[2] == 0
       && (reinterpret_cast<const int*>(&irGUID))[3] == 0);
}

void bar()
{
    GUID guid;

    guid.Data1 = 0;
    guid.Data2 = 0;
    guid.Data3 = 0;
    guid.Data4[0] = 0;
    guid.Data4[1] = 0;
    guid.Data4[2] = 0;
    guid.Data4[3] = 0;
    guid.Data4[4] = 0;
    guid.Data4[5] = 0;
    guid.Data4[6] = 0;
    guid.Data4[7] = 0;

    if (!IsGUIDNULL(guid))
    {
        // Fails here
        fprintf(stderr, "test failed\n");
    }
}
