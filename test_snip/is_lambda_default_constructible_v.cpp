#include <iostream>
#include <type_traits>
int a;
auto lambda1 = [](){};
auto lambda2 = [ a(a) ](){};
int main(){
    
    std::cout << "lambda1 is_lambda_default_constructible_v : "
        << std::is_default_constructible_v<decltype(lambda1)>
        << "\n";

    // 这里说明只要是有 lambda capture 就不能 default_constuctibl
    std::cout << "lambda2 is_lambda_default_constructible_v : "
        << std::is_default_constructible_v<decltype(lambda2)>
        << "\n";
    return 0;
}
