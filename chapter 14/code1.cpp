// 代码1
void f() {
   int*p = new int{3};
   int error = doSomething(p);
   if (error)
       return;
   
   finalize(p);
   delete p;
}
