// g++ -std=c++20 -g 2.cpp
// 为什么 std::ranges 可以直接得到v呢?
// 我写的代码 v 不能以 rvalue 来初始drop_view
#include <iostream>
#include <ranges>
#include <vector>

using namespace std;

vector<int> v{1,2,3,4};

int main(){
    for( auto i : v | std::ranges::views::drop(1)){
        cout << i << " ";
    }
    return 0;
}
