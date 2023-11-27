#include <iostream>
#include <functional>
template<typename T>
class ScopeGuard {
    public:
    ScopeGuard(T iFunc) {
        mFunc = iFunc;
    }
    ~ScopeGuard() {
        std::cout<<"Exit the scope, so run the scope guard.\n";
        (*mFunc)();
    }
    private:
    T mFunc;
};

int main() {
    int i = 3;
    auto lGuard = ScopeGuard(&[&]() {
        std::cout<<"access value " << i<<std::endl;
    });
    return 0;
}
