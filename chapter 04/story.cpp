    #include <stdio.h>

    class Base
    {
    public:
        virtual void f() = 0;
        virtual ~Base() {};
    };

    class Derived: public Base
    {
    public:
        Derived(int i): mid(i) {}
        ~Derived() {}

        virtual void f()  {  printf("f() is called in class
                                                 Derived\n");  }
    private:
        int mid;
    };
 
    int main()
    {
        int rc = 0;

        Base* lpBase = new Derived(8);
        lpBase->f();
        delete lpBase;
        lpBase->f();

        return rc;
    }
