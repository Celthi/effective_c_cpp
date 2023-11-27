#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class Base1
{
public:
    virtual void doSomething() = 0;
private:
    int index1;
};

class Base2
{
public:
    virtual void doSomething() = 0;
private:
    int index2;
};

class Derived : public Base1, public Base2
{
public:
    virtual void doSomething()
    {
        number = 9;
    }
private:
    int number;
};
 
class Derived2 : public Base2
{
public:
    virtual void doSomething()
    {
        number = 9;
    }
private:
    int number;
};

void foo(Base2* ipBase2)
{
    ipBase2->doSomething();
}

int main()
{
    Base2* lpBase2  = new Derived;

    memset(lpBase2, 0, sizeof(Base2));

    foo(lpBase2);

    return 0;
}
