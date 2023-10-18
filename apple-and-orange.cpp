#include<iostream>
#include<bits/stdc++.h>

using namespace std;




void readInput(){

   int s{0};
   int t{0};
   int a{0};
   int b{0};
   int m{0};
   int n{0};
   vector<int> apples;
   vector<int> oranges;
   cin >> s;
   cin >> t;
   cin >> a;
   cin >> b;
   cin >> m;
   cin >> n;
   //cout << "\n" << s << " " << t << " " << a << " " << b << " " << m << " " << n << "\n";

   for(int i = 0; i < m; ++i){
      int tmp{0};
      cin >> tmp;
      //cout << tmp << " ";
      apples.push_back(tmp);
   }
   // cout << "\napples:";
   // for(auto& num : apples){
   //    cout << num  << " ";
   // }
   for(int i = 0; i < n; ++i){
      int tmp{0};
      cin >> tmp;
      //cout << tmp << " ";
      oranges.push_back(tmp);
   }

   // cout << "\noranges:";
   // for(auto& num : oranges){
   //    cout << num  << " ";
   // }
   int a1{0};
   int a2{0};
   for(auto& num : apples){
      num += a;
      if(num >= s && num <= t){
         ++a1;
      }
   }
   for(auto& num : oranges){
      num += b;
      if(num >= s && num <= t){
         ++a2;
      }
   }
   //cout << "\n";
   cout << a1 << "\n";
   cout << a2 << "\n";
}

int main(){
   readInput();
   return 0;
}

