#include <iostream>
#include <vector>

#include "ranges.hpp"

std::vector<int> v{1,2,3,4,5};
std::vector<int> v2{1,2,3,4,5};
int main(){
    // test 1 : drop_view
    //
    // view_interface 很容易理解
    // 就是一个复制了原 vector->v 的类,自己实现了 begin,end
    myranges::drop_view dv(v, 2);
    // use range for loop
    for(auto i : dv){
        std::cout << i << " ";
    }
    std::cout << "\n" ;
    // use iterator
    for(auto it = dv.begin() ; it != dv.end() ; it++){
        std::cout << *it << " ";
    }
    std::cout << "\n" ;

    // ==========================
    std::cout <<" ========================== \n";
    // test 2 : 创建drop实例
    // drop 是 _Adaptor 实例
    // d = drop.operator(2) 返回一个 _RangeAdaptorClosure 实例
    auto d = myranges::views::drop(2);
    // view1 d.operator(_Range) 得到的最终的 drop_view 实例
    auto view1 = d(v); //其实调用了 _RangeAdaptorClosure.operator(viewable_range)

     
    // view type std::is_same_v<>
    std::cout 
        << 
        "std::is_same_v<decltype(view1), myranges::drop_view<myranges::ref_view< std::vector<int, std::allocator<int>> > >>"
        << "-->"
        << std::is_same_v<decltype(view1), myranges::drop_view<myranges::ref_view< std::vector<int, std::allocator<int>> > >>
        << std::endl;
    std::cout <<" ========================== \n";
    for( auto i :  view1){
        std::cout << i << " ";
    }
    std::cout << "\n";

    std::cout <<" ========ref_view========== \n";
    // test 3 ref_view
    auto refview = myranges::ref_view (v2);
    for(auto i : refview){
        std::cout << i << " ";
    }
    std::cout << "\n";


    std::cout <<" ========drop(2)(ref_view)========== \n";
    auto v3 = myranges::views::drop(2)(std::move(refview));
    for(auto i : v3) {
        std::cout << i << " ";
    }
    std::cout << "\n" ;

    //
    // test 3 operator |
    //
    // drop_view construct use std::move() --> compile error
    std::cout <<" ========operator | ================ \n";
    for( auto i : std::move(refview) | myranges::views::drop(2) ) {
        std::cout << i << " ";
    }
    //std::cout << "\n" ;
    return 0;
}
