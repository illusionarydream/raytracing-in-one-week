#include <bits/stdc++.h>
using namespace std;
int main(){
    int a=23;
    int *b=&a;
    shared_ptr<int> shared1=std::make_shared<int>(a);
    std::cout<<*shared1;
    return 0;
}