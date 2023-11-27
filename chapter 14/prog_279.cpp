void g();

void f()
{
    int *p = new int(3);
    g();
    delete p;
}

void g()
{
    throw 3;
}
int main()
{
    try
    {
        f();
    }
    catch (...)
    {
    }
}
