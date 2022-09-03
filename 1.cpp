#include <vector>

#include "ranges.hpp"

std::vector<int> v{1,2,3,4,5};
int main(){
    // test 1 : drop_view
    myranges::drop_view dv(v, 2);
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
