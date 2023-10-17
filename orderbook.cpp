#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <climits>
#include <algorithm>
#include <stdexcept>
#include <set>
using namespace std;

struct order{
   char action{'\0'};
   uint64_t id{0};
   uint64_t timestamp{0};
   string symbol{""};
   char type{'\0'};
   char side{'\0'};
   float price{0.0};
   uint64_t quantity{0};
};

class matchingBook{
   
   vector<order> sell;
   vector<order> buy;
   public:

   bool searchForId(uint64_t id){
      for(auto& c : buy)
         if(c.id == id) return true;
      
      for(auto& c : sell)
         if(c.id == id) return true;

      return false;
   }

   order createNewOrder(vector<string> tokens){
      order order;
      order.action = tokens[0][0];
      order.id = stoul(tokens[1]);
      order.timestamp = stoul(tokens[2]);
      order.symbol = tokens[3];
      order.type = tokens[4][0];
      order.side = tokens[5][0];
      order.price = atof(tokens[6].c_str());
      order.quantity = stoul(tokens[7]);
      return order;
   }

   void readNewOrder(vector<string> tokens){
      if(tokens.size() != 8){
         cout << tokens[1] << " - Reject - 303 - Invalid order details\n";
      }
      size_t sz{0};
      order order = createNewOrder(tokens);
      for(auto & c : buy){
         if(order.id == c.id){
            cout << tokens[1] << " - Reject - 303 - Invalid order details\n";
            return;
         }
      }
      for(auto & c : sell){
         if(order.id == c.id){
            cout << tokens[1] << " - Reject - 303 - Invalid order details\n";
            return;
         }
      }
      if( searchForId(order.id) ){
         cout << tokens[1] << " - Reject - 303 - Invalid order details\n";
      } else if (order.side == 'B'){
         buy.push_back(order);
      }else if( order.side == 'S'){
         sell.push_back(order);
      }else{
         cout << tokens[1] << " - Reject - 303 - Invalid order details\n";
      }
      cout << order.id << " - Accept\n";
   }

void processAmend(vector<string> tokens){
   size_t sz{0};
   order order = createNewOrder(tokens);
   for(auto & c : buy){
      if( c.id == order.id ){
         if(   c.timestamp <= order.timestamp &&
               c.symbol == order.symbol &&
               c.type == order.type &&
               c.side == order.side
            ){
               c.price = order.price;
               c.quantity = order.quantity;
               cout << c.id << "- AmendAccept\n";
               return;
         }
      }
   }
   for(auto & c : sell){
      if( c.id == order.id ){
         if(   c.timestamp <= order.timestamp &&
               c.symbol == order.symbol &&
               c.type == order.type &&
               c.side == order.side
            ){
               c.price = order.price;
               c.quantity = order.quantity;
               cout << c.id << "- AmendAccept\n";
               return;
         }
      }
   }
   cout << order.id << "AmendReject - 404 - Order does not exist\n";
}

void processCancel(vector<string> tokens){
   struct order order; //= createNewOrder(tokens);
   
   order.id = stoul(tokens[1]);
   order.timestamp = stoul(tokens[2]);

   for(int i = 0; i < buy.size(); ++i){
      struct order tmp = buy[i];
      if( tmp.id == order.id && tmp.timestamp <= order.timestamp ){
         cout << tmp.id << " - CancelAccept" << "\n";
         buy.erase(buy.begin()+i);
         return;
      }
   }
   for(int i = 0; i < sell.size(); ++i){
      struct order tmp = sell[i];
      if( tmp.id == order.id && tmp.timestamp <= order.timestamp ){
         cout << tmp.id << " - CancelAccept" << "\n";
         sell.erase(sell.begin()+i);
         return;
      }
   }
   cout << order.id << " - CancelReject - 404 - Order does not exist\n";
}

// struct matchSymbol{
//    inline bool operator() (const order& lhs, const order& rhs){
//       return (lhs.symbol <= rhs.symbol &&
//              lhs.price < rhs.price );
//    }
// };

struct matchPrice{
   inline bool operator() (const order& lhs, const order& rhs){
      if(lhs.symbol <= rhs.symbol ){
         return lhs.price < rhs.price;
      }
      return false;
   }
};



// void matchOrder(uint64_t timestamp, string symbol){
//    if(symbol.size()){
//       // sort by symbol and price and timestamp
//       sort(buy.begin(), buy.end(), matchSymbol());
//       sort(sell.begin(), sell.end(), matchSymbol());
//    }else{
//       sort(buy.begin(), buy.end(), matchPrice());
//       sort(sell.begin(), sell.end(), matchPrice());
//    }
// }

//void 

void processMatch(vector<string> tokens){
   uint64_t id{0};
   string symbol{""};
   // if(isdigit(tokens[0][0])){

   // }
   //matchOrder();
}

int getBestSell(string symbol, int i){
   for(; i < sell.size(); ++i){
      if( sell[i].symbol == symbol ){
         return i;
      }
   }
   return -1;
}

void queryAll(){
   sort(buy.begin(), buy.end(), matchPrice());
   sort(sell.begin(), sell.end(), matchPrice());
   //int i = 0;
   int j = 0;
}

set<string> getAllSymbols(){
   set<string> ans;
   for(auto& o : buy){
      ans.emplace(o.symbol);
   }
   for(auto& o : sell){
      ans.emplace(o.symbol);
   }
   return ans;
}
void processQuery(vector<string> tokens){
   // if(tokens.size() == 1){

   // }
   // set<string> symbols = getAllSymbols();
   // //sort(symbols.begin(),symbols.end());
   // sort(buy.begin(), buy.end(),matchPrice());
   // sort(sell.begin(), sell.end(),matchPrice());
   // auto iter = symbols.begin();
   // while(iter != symbols.end()){
   //    vector<order> temp1;
   //    vector<order> temp2;
   //    for(auto & c : buy)
   //       if(c.symbol == *iter) temp1.push_back(c);
   //    for(auto & c : sell)
   //       if(c.symbol == *iter) temp2.push_back(c);
   //    sort(temp1.begin(), temp1.end(),matchPrice());
   //    sort(temp2.begin(), temp2.end(),matchPrice());

   //    ++iter;
   // }
}

};

matchingBook orderBook;

void readLine(string line){
   stringstream ss(line);
   string tmp{""};
   vector<string> tokens;
   while( getline( ss, tmp,',' ) ){
      tokens.push_back(tmp);
   }

   if( tokens[0][0] == 'N') {
      orderBook.readNewOrder(tokens);
   }else  if( tokens[0][0] == 'A') {
      orderBook.processAmend(tokens);
   }if( tokens[0][0] == 'X') {
      orderBook.processCancel(tokens);
   }else  if( tokens[0][0] == 'M') {

   }if( tokens[0][0] == 'Q') {
      orderBook.processQuery(tokens);
   }else {

   }
}

void readInput(){
   int n{0};
   cin >> n;
   for(int i = 0; i <= n; ++i){
      string tmp{""};
      getline(cin,tmp);
      if(tmp.size()){
         readLine(tmp);
      }
   }
}


int main(){
   readInput();
   return 0;
}