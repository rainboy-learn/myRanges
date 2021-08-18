#include <iostream>
#include <vector>
#include "ranges.hpp"

int main(){
    for (const auto& e : {1,2,3}) {
        std::cout << e << std::endl;
    }

    std::cout << "++++++" << std::endl;
    std::vector v{1,2,3,4};
    auto ttt = myranges::views::all(v);
    for (const auto& e : ttt ) {
        std::cout << e << std::endl;
    }
    return 0;
}

