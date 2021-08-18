#include <type_traits>
#include <iostream>
using namespace std;

int main(){
    cout << is_same_v<int, make_unsigned<int>::type> << endl;
    cout << is_same_v<unsigned int, make_unsigned<int>::type> << endl;

    cout << is_same_v<int, make_signed<int>::type> << endl;
    return 0;
}
