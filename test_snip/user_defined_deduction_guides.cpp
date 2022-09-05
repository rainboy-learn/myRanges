#include <iostream>
#include <type_traits>

template<typename T>
struct A {
    A() = default;
    A(T&& __a) : t(__a)
    {}
    T t{};
};


template<typename T>
struct B {
    B(T&& __args) : t(__args)
    {}
    T t{};
};

template<typename T>
B(T && ) -> B<A<T>>;


int main(){
    B b1(1);
    // 相当于 B<A<int>> b1(1);
    std::cout
        << "std::is_same_v<decltype(b1), B<A<int>>> "
        << std::is_same_v<decltype(b1), B<A<int>>>
        << std::endl;
    return 0;
}
