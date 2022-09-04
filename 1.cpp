#include <iostream>
#include <vector>

#include "ranges.hpp"

std::vector<int> v{1,2,3,4,5};
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

    // test 2 : drop
    // auto d = myranges::drop(2);
    // auto view1 = d(v);
    // view type std::is_same_v<>
    //
    // for( auto i :  view1){
    //      cout << i << " ";
    // }
    // cout << "\n";
    //
    // test 3 operator |
    //
    // for( auto i : v | drop(2) ) {
    // }
    return 0;
}
