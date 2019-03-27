#include <memory>
#include <iostream>
#include "TSharedPtr.h"

struct Foo {
    Foo() { std::cout << "Foo...\n"; }
    ~Foo() { std::cout << "~Foo...\n"; }
};

struct Foo2 {
    Foo2() { std::cout << "Foo2...\n"; }
    Foo2(Foo f) { }
    ~Foo2() { std::cout << "~Foo2...\n"; }
};

struct D {
    void operator()(Foo* p) const {
        std::cout << "Call delete from function object...\n";
        delete p;
    }
};

int main()
{
    {
        std::cout << "constructor with no managed object\n";
        TSharedPtr<Foo> sh1;
    }

    {
        std::cout << "constructor with object\n";
        TSharedPtr<Foo> sh2(new Foo);
        TSharedPtr<Foo> sh3(sh2);
        TSharedPtr<Foo> sh4(sh3);
        TSharedPtr<Foo> sh5(new Foo);
        sh5 = sh2;
        TSharedPtr<Foo2> sh6 = MakeShared<Foo2>(Foo{});
        TSharedPtr<Foo2> sh7(sh6);
        TSharedPtr<Foo2> sh8;
        sh8 = sh7;
        std::cout << sh2.use_count() << '\n';
        std::cout << sh3.use_count() << '\n';
        std::cout << sh4.use_count() << '\n';
        std::cout << sh5.use_count() << '\n';
        std::cout << sh6.use_count() << '\n';
        std::cout << sh7.use_count() << '\n';
        std::cout << sh8.use_count() << '\n';
    }

    /*{
        std::cout << "constructor with object and deleter\n";
        TSharedPtr<Foo> sh4(new Foo, D());
        TSharedPtr<Foo> sh5(new Foo, [](auto p) {
            std::cout << "Call delete from lambda...\n";
            delete p;
        });
    }*/
}