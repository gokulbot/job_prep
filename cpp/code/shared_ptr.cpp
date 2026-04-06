#include <iostream>
#include <memory>
class B;
class A {
   public:
    std::shared_ptr<B> b;
    ~A() { std::cout << "A destroyed" << "\n"; }
};
class B {
   public:
    std::shared_ptr<A> a;
    ~B() { std::cout << "B destroyed" << "\n"; }
};

int main() {
    std::shared_ptr<A> a = std::make_shared<A>();
    std::shared_ptr<B> b = std::make_shared<B>();

    a->b = b;
    b->a = a;

    return 0;
}