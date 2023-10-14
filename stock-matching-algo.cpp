#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <climits>
#include <algorithm>
using namespace std;

pair<int,int> doBinSearch(vector<pair<int,int>>& pairs, int& num){
   int n = pairs.size();
   int i = 0;
   int j = n-1;
   pair<int,int> ans;
   ans.first = 0;
   ans.second = 0;
   while(i <= j){
      int mid = i + (j-i)/2;
      if(pairs[mid].first <= num){
         if(ans.first <  pairs[mid].first){
            ans =  pairs[mid];
         }
         i = mid+1;
      }else{
         j = mid-1;
      }
   }
   return ans;
}

void readInput(){
   size_t n{0};
   cin >> n;
   size_t k{0};
   vector<int> s;
   vector<int> p;
   vector<pair<int,int>> pairs;
   vector<int> q;
   for(size_t i = 0; i < n; ++i){
      int temp{0}; 
      cin >> temp;
      s.push_back(temp);
   }
   for(size_t i = 0; i < n; ++i){
      int temp{0}; 
      cin >> temp;
      pairs.push_back( make_pair( s[i], temp) );
   }
   //cout << "\n\n\n";
   sort(pairs.begin(), pairs.end());
   cin >> k;
   //cout << "\n k:" << k << "\n";
   for(size_t i = 0; i < k; ++i){
      int temp{0};
      cin >> temp;
      q.push_back(temp);
      //cout << "\n temp:" << temp << "\n";
      //ans = doBinSearch(pairs, temp);
      //cout << ans << "\n";
   }
   cout << "\n";
   for(auto& num : q){
      //int ans{INT_MIN};
      pair<int,int> ans;
      ans.first = INT_MIN;
      ans.second = INT_MIN;
      ans = doBinSearch(pairs, num);
      cout << ans.second << "\n";
   }
}

int main(){
   readInput();
   return 0;
}