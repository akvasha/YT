#include <memory>
#include <iostream>
#include "TSharedPtr.h"
#include "Protobuf.h"
#include <folly/futures/Future.h>
#include "gtest/gtest.h"

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

TEST(EmptyConstructor, 1) {
    TSharedPtr<Foo> sh1;
    std::shared_ptr<Foo> sh;
    ASSERT_EQ(sh.use_count(), sh1.use_count());
    ASSERT_EQ(sh.get(), sh1.get());
}

TEST(ConstructorWithObject, Init) {
    Foo *foo = new Foo;
    TSharedPtr<Foo> sh1(foo);
    std::shared_ptr<Foo> sh(foo);
    ASSERT_EQ(sh.use_count(), sh1.use_count());
    ASSERT_EQ(sh.get(), sh1.get());
}

TEST(ConstructorWithObject, Copy) {
    Foo *foo = new Foo;
    TSharedPtr<Foo> sh1(foo);
    TSharedPtr<Foo> sh2(sh1);
    std::shared_ptr<Foo> sh3(foo);
    std::shared_ptr<Foo> sh4(sh3);
    ASSERT_EQ(sh2.use_count(), sh4.use_count());
    ASSERT_EQ(sh2.get(), sh4.get());
}

TEST(ConstructorWithObject, Assigment) {
    Foo *foo = new Foo;
    TSharedPtr<Foo> sh1(foo);
    TSharedPtr<Foo> sh2;
    std::shared_ptr<Foo> sh3(foo);
    std::shared_ptr<Foo> sh4;
    ASSERT_EQ(sh2.use_count(), sh4.use_count());
    ASSERT_EQ(sh2.get(), sh4.get());
    sh2 = sh1;
    sh4 = sh3;
    ASSERT_EQ(sh2.use_count(), sh4.use_count());
    ASSERT_EQ(sh2.get(), sh4.get());
}

TEST(ConstructorWithObject, MakeShared) {
    Foo object = Foo{};
    TSharedPtr<Foo2> sh1 = MakeShared<Foo2>(object);
    std::shared_ptr sh2 = std::make_shared<Foo2>(object);
    ASSERT_EQ(sh1.use_count(), sh2.use_count());
    TSharedPtr<Foo2> sh3 = sh1;
    std::shared_ptr<Foo2> sh4 = sh2;
    ASSERT_EQ(sh3.use_count(), sh4.use_count());
}

TEST(ConstructorWithObject, AssigmentEmpty) {
    TSharedPtr<int> sh1(new int(1));
    TSharedPtr<int> sh2;
    sh1 = sh2;
    std::shared_ptr<int> sh3(new int(1));
    std::shared_ptr<int> sh4;
    sh3 = sh4;
    ASSERT_EQ(sh1.use_count(), sh3.use_count());
    ASSERT_EQ(sh1.get(), sh3.get());
}

int main(int argc, char *argv[])
{
    work2();
    /*::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();*/
}