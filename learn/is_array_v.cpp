/* author: Rainboy email: rainboylvx@qq.com  time: 2021年 08月 17日 星期二 23:37:29 CST */
#include <bits/stdc++.h>
using namespace std;
typedef long long ll;
const int maxn = 1e6+5,maxe = 1e6+5; //点与边的数量

int n,m;
/* 定义全局变量 */

int a[10];
int main(int argc,char * argv[]){
    cout << is_array_v<decltype(a)> << endl;
    cout << is_array_v<decltype(a[0])> << endl;
    cout << is_array_v<decltype(&a[0])> << endl;
    int *b = a;
    cout << is_array_v<decltype(b)> << endl;
    vector<int> v;
    int c[1][1];
    cout << is_array_v<decltype(v)> << endl;
    cout << is_array_v<decltype(c)> << endl;
    cout << is_array_v<std::array<int, 10>> << endl;
    return 0;
}
