#include <iostream>
#include <bits/stdc++.h>

using namespace std;




bool cancelClass(){
   int n{};
   int k{};
   vector<int> vec;
   int onTime{0};
   cin >> n;
   cin >> k;
   for(int i = 0; i < n; ++i){
      int tmp{0};
      cin >> tmp;
      if(tmp <= 0){
         ++onTime;
      }
   }
   if(onTime < k){
      return true;
   }else{
      return false;
   }
}

void readInput(){
   int t{};
   cin >> t;
   for(int i = 0; i < t; ++i){
      if(cancelClass()){
         cout << "YES\n";
      }else{
         cout << "NO\n";
      }
   }
}

int main(){
   readInput();
   return 0;
}