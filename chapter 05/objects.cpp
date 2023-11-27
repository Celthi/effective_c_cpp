class POD_STRUCT
{
public:
    short s;
    int a;
    double d;
};

class NONE_POD_STRUCT
{
    virtual bool Verify() { return true; }

public:
    short s;
    int a;
    double d;
};

double Sum(int i_int0,
             int i_int1,
             POD_STRUCT i_pod,
             NONE_POD_STRUCT i_nonpod,
             long* ip_long,
             float i_float,
             long i_long0,
             long i_long1)
{
    double result = i_int0 + i_int1
                     + i_pod.a + i_pod.d + i_pod.s
                     + i_nonpod.a + i_nonpod.d + i_nonpod.s
                     + *ip_long + i_float + i_long0 + i_long1;

    return result;
}

int main()
{
    int a_int_0 = 0;
    int a_int_1 = 1;
    POD_STRUCT a_pod = {0, 1, 2.2};
    NONE_POD_STRUCT a_nonpod;
    long a_long   = 3;
    float a_float = 4.4;

    double sum = Sum(a_int_0, a_int_1, a_pod, a_nonpod, &a_long, a_float, a_long, a_long);

    return 0;
}
