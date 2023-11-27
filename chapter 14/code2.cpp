// 代码2
void f() {
   int*p = new int{3};
   int error = doSomething(p);
   if (error) {
       delete p; //释放内存，当出现错误的时候
       return;
    }
   
   finalize(p);
   delete p;
}
