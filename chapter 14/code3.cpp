// 代码3
class MyInt {
public:
  int* p;
  MyInt(int i) {
   p = new int{i};
  }
  ~MyInt() {
    delete p;
  }
};
int doSomething(int* p) {
   return -1;
}
void finalize(int* p) {
}
void f() {
   MyInt my(3);
   int error = doSomething(my.p);
   if (error) {
       return;
    }  
   finalize(my.p);
}
