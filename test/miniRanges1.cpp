#include <iostream>
#include <vector>
#include "miniRanges.hpp"

using namespace std;

int main(){
    vector<int> v{1,2,3,4};
    //miniRanges::ref_view<vector<int>> t = v;
    //std::cout << v.empty() << std::endl;
    //for (const auto& e : t) {
        //std::cout << e << std::endl;
    //}
    //miniRanges::__Make_RangeAdaptorClosure fiter = [](){
    //};
    auto even = [](auto &v){return v % 2 == 1;};
    auto adaptor = miniRanges::views::filter(even);
    //std::cout << adaptor << std::endl;
    
    auto object = adaptor(v);
    object.begin();
    //for( auto it = object.begin() ; it != object.end() ; it++){

    //}
    for (const auto& e : object) {
        std::cout << e << std::endl;   
    }
    return 0;
}
